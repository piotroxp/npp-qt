// ShortcutManager.cpp — Full key binding management for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "ShortcutManager.h"
#include "EditorCommandManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QKeySequence>

// ============================================================================
// Singleton
// ============================================================================
ShortcutManager& ShortcutManager::instance() {
    static ShortcutManager instance;
    return instance;
}

ShortcutManager::ShortcutManager() = default;
ShortcutManager::~ShortcutManager() = default;

// ============================================================================
// KeyBinding helpers
// ============================================================================
static int _parseKeyCode(const QString& keyName) {
    static const QMap<QString, int> keyMap = {
        {"SPACE", Qt::Key_Space},
        {"TAB", Qt::Key_Tab},
        {"ENTER", Qt::Key_Return}, {"RETURN", Qt::Key_Return},
        {"ESCAPE", Qt::Key_Escape}, {"ESC", Qt::Key_Escape},
        {"BACKSPACE", Qt::Key_Backspace}, {"BS", Qt::Key_Backspace},
        {"DELETE", Qt::Key_Delete}, {"DEL", Qt::Key_Delete},
        {"UP", Qt::Key_Up},
        {"DOWN", Qt::Key_Down},
        {"LEFT", Qt::Key_Left},
        {"RIGHT", Qt::Key_Right},
        {"HOME", Qt::Key_Home},
        {"END", Qt::Key_End},
        {"PAGEUP", Qt::Key_PageUp}, {"PGUP", Qt::Key_PageUp},
        {"PAGEDOWN", Qt::Key_PageDown}, {"PGDN", Qt::Key_PageDown},
        {"INSERT", Qt::Key_Insert}, {"INS", Qt::Key_Insert},
        {"F1", Qt::Key_F1}, {"F2", Qt::Key_F2}, {"F3", Qt::Key_F3},
        {"F4", Qt::Key_F4}, {"F5", Qt::Key_F5}, {"F6", Qt::Key_F6},
        {"F7", Qt::Key_F7}, {"F8", Qt::Key_F8}, {"F9", Qt::Key_F9},
        {"F10", Qt::Key_F10}, {"F11", Qt::Key_F11}, {"F12", Qt::Key_F12},
        {"+", Qt::Key_Plus}, {"=", Qt::Key_Equal},
        {"-", Qt::Key_Minus}, {"_", Qt::Key_Underscore},
        {"[", Qt::Key_BracketLeft}, {"{", Qt::Key_BraceLeft},
        {"]", Qt::Key_BracketRight}, {"}", Qt::Key_BraceRight},
        {"(", Qt::Key_ParenLeft},
        {")", Qt::Key_ParenRight},
        {",", Qt::Key_Comma}, {"<", Qt::Key_Less},
        {".", Qt::Key_Period}, {">", Qt::Key_Greater},
        {"/", Qt::Key_Slash}, {"?", Qt::Key_Question},
        {"\\", Qt::Key_Backslash}, {"|", Qt::Key_Bar},
        {";", Qt::Key_Semicolon}, {":", Qt::Key_Colon},
        {"'", Qt::Key_QuoteLeft}, {"\"", Qt::Key_QuoteDbl},
        {"`", Qt::Key_AsciiTilde}, {"~", Qt::Key_AsciiTilde},
    };

    QString upper = keyName.toUpper();
    if (keyMap.contains(upper)) return keyMap[upper];
    if (upper.size() == 1) return upper[0].unicode();
    return Qt::Key_unknown;
}

// Unused — reserved for future shortcut modifier parsing
// static int _parseModifiers(const QString& str) { ... }

QString KeyBinding::toString() const {
    if (shortcutText.isEmpty()) return QString();
    return shortcutText;
}

KeyBinding KeyBinding::fromString(const QString& str, int cmdId, const QString& name) {
    KeyBinding b;
    b.commandId = cmdId;
    b.commandName = name;
    b.shortcutText = str.toUpper();

    // Parse: "Ctrl+Shift+P" → mods + key
    QStringList parts = str.split('+', Qt::SkipEmptyParts);
    b.modifiers = 0;
    b.keyCode = 0;

    for (const QString& part : parts) {
        QString p = part.trimmed().toUpper();
        if (p == "CTRL") b.modifiers |= Qt::ControlModifier;
        else if (p == "ALT") b.modifiers |= Qt::AltModifier;
        else if (p == "SHIFT") b.modifiers |= Qt::ShiftModifier;
        else if (p == "META" || p == "WIN" || p == "CMD") b.modifiers |= Qt::MetaModifier;
        else {
            // Try to parse as a key name or character
            if (!b.keyCode) b.keyCode = _parseKeyCode(p);
        }
    }

    return b;
}

// ============================================================================
// Binding management
// ============================================================================
void ShortcutManager::bindKey(int commandId, const QString& shortcut) {
    if (shortcut.isEmpty()) return;

    KeyBinding binding = KeyBinding::fromString(shortcut, commandId);
    binding.commandName = QString::fromStdString(EditorCommandManager::instance().getCommandName(commandId));

    // Remove old binding for this command
    for (auto it = _globalBindings.begin(); it != _globalBindings.end(); ++it) {
        if (it->commandId == commandId) {
            int oldKey = _makeKey(it->shortcutText);
            _globalShortcutMap.remove(oldKey);
            _globalBindings.erase(it);
            break;
        }
    }

    // Check for conflicts
    int conflictId = getBoundCommand(shortcut);
    if (conflictId >= 0 && conflictId != commandId) {
        notifyConflict(shortcut, conflictId, commandId);
    }

    int key = _makeKey(shortcut);
    _globalShortcutMap[key] = commandId;
    _globalBindings.append(binding);

    emit shortcutChanged(shortcut, commandId);
    emit allShortcutsChanged();
}

void ShortcutManager::unbindKey(const QString& shortcut) {
    int key = _makeKey(shortcut);
    for (auto it = _globalBindings.begin(); it != _globalBindings.end(); ++it) {
        if (_makeKey(it->shortcutText) == key) {
            _globalShortcutMap.remove(key);
            _globalBindings.erase(it);
            emit allShortcutsChanged();
            return;
        }
    }
}

int ShortcutManager::resolveBinding(const QString& shortcut, BufferID buffer) const {
    int key = _makeKey(shortcut);
    if (buffer) {
        QString localKey = QString::number(reinterpret_cast<quintptr>(buffer)) + ":" + shortcut;
        auto it = _localBindings.find(localKey);
        if (it != _localBindings.end()) return it->commandId;
    }
    auto it = _globalShortcutMap.find(key);
    return (it != _globalShortcutMap.end()) ? it.value() : -1;
}

int ShortcutManager::resolveBinding(int keyCode, int modifiers, BufferID buffer) const {
    // Build shortcut string from keyCode+modifiers
    QString text = _makeShortcutText(keyCode, modifiers);
    return resolveBinding(text, buffer);
}

QString ShortcutManager::_makeShortcutText(int keyCode, int modifiers) const {
    QStringList parts;
    if (modifiers & Qt::ControlModifier) parts.append("Ctrl");
    if (modifiers & Qt::AltModifier) parts.append("Alt");
    if (modifiers & Qt::ShiftModifier) parts.append("Shift");
    if (modifiers & Qt::MetaModifier) parts.append("Meta");
    // Convert keyCode back to string
    if (keyCode >= Qt::Key_Space && keyCode <= Qt::Key_AsciiTilde) {
        parts.append(QString(QChar(keyCode)));
    } else {
        // Named keys
        if (keyCode >= Qt::Key_F1 && keyCode <= Qt::Key_F12)
            parts.append(QString("F%1").arg(keyCode - Qt::Key_F1 + 1));
        else if (keyCode == Qt::Key_Return) parts.append("Enter");
        else if (keyCode == Qt::Key_Backspace) parts.append("Backspace");
        else if (keyCode == Qt::Key_Delete) parts.append("Delete");
        else if (keyCode == Qt::Key_Escape) parts.append("Escape");
        else if (keyCode == Qt::Key_Tab) parts.append("Tab");
        else if (keyCode == Qt::Key_Up) parts.append("Up");
        else if (keyCode == Qt::Key_Down) parts.append("Down");
        else if (keyCode == Qt::Key_Left) parts.append("Left");
        else if (keyCode == Qt::Key_Right) parts.append("Right");
        else if (keyCode == Qt::Key_Home) parts.append("Home");
        else if (keyCode == Qt::Key_End) parts.append("End");
        else if (keyCode == Qt::Key_PageUp) parts.append("PageUp");
        else if (keyCode == Qt::Key_PageDown) parts.append("PageDown");
        else if (keyCode == Qt::Key_Insert) parts.append("Insert");
        else parts.append(QKeySequence(keyCode).toString());
    }
    return parts.join("+");
}

int ShortcutManager::_makeKey(const QString& shortcut) const {
    return qHash(shortcut.toUpper());
}

int ShortcutManager::getBoundCommand(const QString& shortcut) const {
    int key = _makeKey(shortcut);
    auto it = _globalShortcutMap.find(key);
    return (it != _globalShortcutMap.end()) ? it.value() : -1;
}

bool ShortcutManager::hasConflict(const QString& shortcut) const {
    int count = 0;
    for (const KeyBinding& b : _globalBindings) {
        if (_makeKey(b.shortcutText) == _makeKey(shortcut)) {
            ++count;
        }
    }
    return count > 1;
}

QVector<ShortcutManager::Conflict> ShortcutManager::getConflicts() const {
    QVector<Conflict> conflicts;
    QMap<int, QPair<int, int>> seen;
    for (const KeyBinding& b : _globalBindings) {
        int key = _makeKey(b.shortcutText);
        if (seen.contains(key)) {
            Conflict c;
            c.shortcut = b.shortcutText;
            c.cmd1 = seen[key].first;
            c.cmd2 = b.commandId;
            conflicts.append(c);
        } else {
            seen[key] = qMakePair(b.commandId, -1);
        }
    }
    return conflicts;
}

void ShortcutManager::notifyConflict(const QString& shortcut, int existingCmd, int newCmd) {
    qWarning() << "[ShortcutManager] Conflict:" << shortcut
               << "bound to both" << existingCmd << "and" << newCmd;
    emit conflictDetected(shortcut, existingCmd, newCmd);
}

void ShortcutManager::bindKeyLocal(BufferID buffer, int commandId, const QString& shortcut) {
    QString key = QString::number(reinterpret_cast<quintptr>(buffer)) + ":" + shortcut;
    KeyBinding b = KeyBinding::fromString(shortcut, commandId);
    b.commandName = QString::fromStdString(EditorCommandManager::instance().getCommandName(commandId));
    _localBindings[key] = b;
}

void ShortcutManager::unbindKeyLocal(BufferID buffer, const QString& shortcut) {
    QString key = QString::number(reinterpret_cast<quintptr>(buffer)) + ":" + shortcut;
    _localBindings.remove(key);
}

void ShortcutManager::clear() {
    _globalBindings.clear();
    _globalShortcutMap.clear();
    _localBindings.clear();
}

QString ShortcutManager::defaultConfigPath() const {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        configDir = QDir::home().filePath(".config/notepad--");
    return QDir(configDir).filePath("shortcuts.json");
}

void ShortcutManager::loadFromJson(const QString& path) {
    QFile file(path.isEmpty() ? defaultConfigPath() : path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    QJsonArray bindings = root["bindings"].toArray();

    clear();
    for (const QJsonValue& v : bindings) {
        QJsonObject b = v.toObject();
        int id = b["id"].toInt(-1);
        QString shortcut = b["shortcut"].toString();
        if (id >= 0 && !shortcut.isEmpty()) {
            bindKey(id, shortcut);
        }
    }
}

void ShortcutManager::saveToJson(const QString& path) const {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        configDir = QDir::home().filePath(".config/notepad--");
    QDir().mkpath(configDir);
    QString filePath = path.isEmpty() ? defaultConfigPath() : path;

    QJsonArray bindings;
    for (const KeyBinding& b : _globalBindings) {
        if (!b.shortcutText.isEmpty()) {
            QJsonObject obj;
            obj["id"] = b.commandId;
            obj["name"] = b.commandName;
            obj["shortcut"] = b.shortcutText;
            obj["key"] = b.keyCode;
            obj["ctrl"] = (b.modifiers & Qt::ControlModifier) ? 1 : 0;
            obj["alt"] = (b.modifiers & Qt::AltModifier) ? 1 : 0;
            obj["shift"] = (b.modifiers & Qt::ShiftModifier) ? 1 : 0;
            bindings.append(obj);
        }
    }

    QJsonObject root;
    root["version"] = 1;
    root["bindings"] = bindings;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

// ============================================================================
// Macro shortcuts
// ============================================================================
void ShortcutManager::bindMacro(int macroIndex, const QString& shortcut) {
    int key = _makeKey(shortcut);
    _macroShortcutMap[key] = macroIndex;
    emit allShortcutsChanged();
}

void ShortcutManager::unbindMacro(const QString& shortcut) {
    int key = _makeKey(shortcut);
    _macroShortcutMap.remove(key);
    emit allShortcutsChanged();
}

int ShortcutManager::getMacroForShortcut(const QString& shortcut) const {
    int key = _makeKey(shortcut);
    auto it = _macroShortcutMap.find(key);
    return (it != _macroShortcutMap.end()) ? it.value() : -1;
}

QString ShortcutManager::makeShortcutText(int keyCode, int modifiers) const {
    return _makeShortcutText(keyCode, modifiers);
}

// ============================================================================
// Macro command registration
// ============================================================================
void ShortcutManager::registerMacroCommand(const QString& command, const QString& macroName) {
    macroCommands_[command] = macroName;
}

bool ShortcutManager::resolveMacroBinding(const QString& command, QString& outMacroName) const {
    auto it = macroCommands_.find(command);
    if (it != macroCommands_.end()) {
        outMacroName = it.value();
        return true;
    }
    return false;
}
