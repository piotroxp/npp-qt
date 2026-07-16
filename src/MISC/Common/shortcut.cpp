// Ported to Qt6: Keyboard shortcut implementation
#include "shortcut.h"
#include "NppNotification.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QMap>
#include <QPair>

// =============================================================================
// Shortcut Implementation
// =============================================================================

Shortcut::Shortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint32 key, QObject* parent)
    : QObject(parent), _keyCombo{isCtrl, isAlt, isShift, key}, _name(name) {}

void Shortcut::setName(const QString& menuName, const QString& shortcutName) {
    _menuName = menuName;
    if (!shortcutName.isEmpty()) {
        _name = shortcutName;
        _canModifyName = true;
    }
}

QString Shortcut::toString() const {
    QString result;

    // Normalize modifier order: Ctrl, Alt, Shift
    if (_keyCombo._isCtrl)  result += "Ctrl+";
    if (_keyCombo._isAlt)   result += "Alt+";
    if (_keyCombo._isShift) result += "Shift+";

    // Add key name
    if (_keyCombo._key != 0) {
        result += keyToString(_keyCombo._key);
    }

    return result;
}

QString Shortcut::toMenuItemString() const {
    if (_keyCombo._key == 0) return QString();

    QString result;
    if (_keyCombo._isCtrl)  result += "Ctrl+";
    if (_keyCombo._isAlt)   result += "Alt+";
    if (_keyCombo._isShift) result += "Shift+";
    result += keyToString(_keyCombo._key);
    return result;
}

QKeySequence Shortcut::toQKeySequence() const {
    int key = 0;
    if (_keyCombo._isCtrl)  key |= Qt::CTRL;
    if (_keyCombo._isAlt)   key |= Qt::ALT;
    if (_keyCombo._isShift) key |= Qt::SHIFT;

    // Convert virtual key to Qt key
    key |= vkToQtKey(_keyCombo._key);
    return QKeySequence(key);
}

Shortcut& Shortcut::operator=(const Shortcut& sc) {
    _keyCombo = sc._keyCombo;
    _name = sc._name;
    _menuName = sc._menuName;
    _canModifyName = sc._canModifyName;
    return *this;
}

// =============================================================================
// CommandShortcut Implementation
// =============================================================================

CommandShortcut::CommandShortcut(const Shortcut& sc, unsigned long id, QObject* parent)
    : Shortcut(sc.getName(), sc.getKeyCombo()._isCtrl, sc.getKeyCombo()._isAlt,
               sc.getKeyCombo()._isShift, sc.getKeyCombo()._key, parent),
      _id(id) {}

CommandShortcut::CommandShortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift,
                               quint32 key, unsigned long id, QObject* parent)
    : Shortcut(name, isCtrl, isAlt, isShift, key, parent), _id(id) {}

// =============================================================================
// ScintillaKeyMap Implementation
// =============================================================================

ScintillaKeyMap::ScintillaKeyMap(const Shortcut& sc, unsigned long scintillaKeyID,
                                 unsigned long menuCmdID, QObject* parent)
    : Shortcut(sc.getName(), sc.getKeyCombo()._isCtrl, sc.getKeyCombo()._isAlt,
               sc.getKeyCombo()._isShift, sc.getKeyCombo()._key, parent),
      _scintillaKeyID(scintillaKeyID), _menuCmdID(menuCmdID) {
    addKeyCombo(sc.getKeyCombo());
}

bool ScintillaKeyMap::isEnabled() const {
    return !_keyCombos.isEmpty() && Shortcut::isEnabled();
}

void ScintillaKeyMap::addKeyCombo(KeyCombo combo) {
    _keyCombos.append(combo);
}

void ScintillaKeyMap::clearDups() {
    // Remove duplicate key combos
    for (int i = 0; i < _keyCombos.size(); ++i) {
        for (int j = i + 1; j < _keyCombos.size(); ) {
            if (_keyCombos[i] == _keyCombos[j]) {
                _keyCombos.removeAt(j);
            } else {
                ++j;
            }
        }
    }
}

QString ScintillaKeyMap::toString() const {
    QStringList parts;
    for (const auto& combo : _keyCombos) {
        QString part;
        if (combo._isCtrl)  part += "Ctrl+";
        if (combo._isAlt)   part += "Alt+";
        if (combo._isShift) part += "Shift+";
        part += keyToString(combo._key);
        parts.append(part);
    }
    return parts.join(" | ");
}

// =============================================================================
// Macro Implementation
// =============================================================================

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam), _macroType(mtUseLParameter) {}

RecordedMacroStep::RecordedMacroStep(int iCommandID)
    : _message(0), _wParameter(0), _lParameter(iCommandID), _macroType(mtMenuCommand) {}

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam,
                                    const QString& sParam, int type)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam),
      _sParameter(sParam), _macroType(static_cast<MacroTypeIndex>(type)) {}

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam,
                                    const char* sParam, int type)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam),
      _sParameter(QString::fromLatin1(sParam)), _macroType(static_cast<MacroTypeIndex>(type)) {}

void RecordedMacroStep::playBack() {
    // Macro playback is handled by the macro manager
    // This is a placeholder - actual implementation depends on Scintilla interface
    qDebug() << "Macro step playback:" << _message << _wParameter << _lParameter
             << "string:" << _sParameter;
}

// =============================================================================
// MacroShortcut Implementation
// =============================================================================

MacroShortcut::MacroShortcut(const Shortcut& sc, const Macro& macro, int id, QObject* parent)
    : CommandShortcut(sc, id, parent), _macro(macro) {}

// =============================================================================
// UserCommand Implementation
// =============================================================================

UserCommand::UserCommand(const Shortcut& sc, const QString& cmd, int id, QObject* parent)
    : CommandShortcut(sc, id, parent), _cmd(cmd) {}

// =============================================================================
// PluginCmdShortcut Implementation
// =============================================================================

PluginCmdShortcut::PluginCmdShortcut(const Shortcut& sc, int id, const QString& moduleName,
                                     int internalID, QObject* parent)
    : CommandShortcut(sc, id, parent), _id(id), _moduleName(moduleName), _internalID(internalID) {}

bool PluginCmdShortcut::isValid() const {
    if (_moduleName.isEmpty() || _internalID < 0) {
        return false;
    }
    return Shortcut::isValid();
}

// =============================================================================
// Accelerator Implementation
// =============================================================================

Accelerator::Accelerator(QObject* parent) : QObject(parent) {}

Accelerator::~Accelerator() {
    // Cleanup
}

void Accelerator::init(QMenu* hMenu, QWidget* menuParent) {
    _hAccelMenu = hMenu;
    _hMenuParent = menuParent;
}

void Accelerator::updateShortcuts() {
    // Update accelerator table from registered shortcuts
    _accelerators.clear();

    // This would typically iterate through registered shortcuts
    // and rebuild the accelerator map
}

void Accelerator::updateFullMenu() {
    // Update menu items with accelerator text
    if (!_hAccelMenu) return;

    for (auto* action : _hAccelMenu->actions()) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty()) {
            // Menu item already has shortcut - nothing to do
        }
    }
}

void Accelerator::updateMenuItemByCommand(const CommandShortcut& csc) {
    // Find and update menu item for this command
    Q_UNUSED(csc);
    // Implementation depends on menu structure
}

// =============================================================================
// ScintillaAccelerator Implementation
// =============================================================================

ScintillaAccelerator::ScintillaAccelerator(QObject* parent)
    : QObject(parent) {}

void ScintillaAccelerator::init(QVector<QWidget*> vScintillas, QMenu* hMenu, QWidget* menuParent) {
    _vScintillas = vScintillas;
    _hAccelMenu = hMenu;
    _hMenuParent = menuParent;
}

void ScintillaAccelerator::updateKeys() {
    // Update scintilla key bindings
    // Implementation depends on Scintilla interface
}

// =============================================================================
// Utility Functions
// =============================================================================

void getKeyStrFromVal(quint8 keyVal, QString& str) {
    // Convert virtual key code to string
    str = Shortcut::keyToString(keyVal);
}

void getNameStrFromCmd(unsigned long cmd, QString& str) {
    Q_UNUSED(cmd);
    // Would resolve command ID to menu item name
    str = QString("Command %1").arg(cmd);
}

// =============================================================================
// Key Conversion Utilities (static in Shortcut class)
// =============================================================================

QString Shortcut::keyToString(quint32 key) {
    // Handle special keys
    switch (key) {
        case NppKeys::VK_BACK:    return "Backspace";
        case NppKeys::VK_TAB:     return "Tab";
        case NppKeys::VK_RETURN:  return "Enter";
        case NppKeys::VK_ESCAPE:  return "Escape";
        case NppKeys::VK_SPACE:   return "Space";
        case NppKeys::VK_END:     return "End";
        case NppKeys::VK_HOME:    return "Home";
        case NppKeys::VK_LEFT:    return "Left";
        case NppKeys::VK_UP:      return "Up";
        case NppKeys::VK_RIGHT:   return "Right";
        case NppKeys::VK_DOWN:    return "Down";
        case NppKeys::VK_INSERT:  return "Insert";
        case NppKeys::VK_DELETE:  return "Delete";
        case NppKeys::VK_PRIOR:   return "PageUp";
        case NppKeys::VK_NEXT:    return "PageDown";
        case NppKeys::VK_MULTIPLY: return "*";
        case NppKeys::VK_ADD:     return "+";
        case NppKeys::VK_SUBTRACT: return "-";
        case NppKeys::VK_DECIMAL: return ".";
        case NppKeys::VK_DIVIDE:  return "/";
        case NppKeys::VK_NUMLOCK: return "Num Lock";
        case NppKeys::VK_SCROLL:  return "Scroll";
        case NppKeys::VK_LSHIFT:  return "Left Shift";
        case NppKeys::VK_RSHIFT:  return "Right Shift";
        case NppKeys::VK_LCONTROL: return "Left Ctrl";
        case NppKeys::VK_RCONTROL: return "Right Ctrl";
        case NppKeys::VK_LMENU:   return "Left Alt";
        case NppKeys::VK_RMENU:   return "Right Alt";
        case NppKeys::VK_CAPITAL: return "Caps Lock";
        case NppKeys::VK_LWIN:    return "Left Win";
        case NppKeys::VK_RWIN:    return "Right Win";
        case NppKeys::VK_APPS:    return "Menu";
        default:
            break;
    }

    // Function keys
    if (key >= NppKeys::VK_F1 && key <= NppKeys::VK_F24) {
        return QString("F%1").arg(key - NppKeys::VK_F1 + 1);
    }

    // Numpad keys
    if (key >= NppKeys::VK_NUMPAD0 && key <= NppKeys::VK_NUMPAD9) {
        return QString("Num %1").arg(key - NppKeys::VK_NUMPAD0);
    }

    // OEM keys
    if (key >= 0xBA && key <= 0xDF) {
        static const char* const oemKeys[] = {
            ";:", "=+", ",<", "-_", ".>", "/?", "`~",
            "[{", "\\|", "]}", "'\"", "OEM_8", "OEM_102"
        };
        int idx = key - 0xBA;
        if (idx >= 0 && idx < 12) {
            return QString::fromLatin1(oemKeys[idx]);
        }
    }

    // Letter keys
    if (key >= 'A' && key <= 'Z') {
        return QChar(key);
    }

    // Number keys
    if (key >= '0' && key <= '9') {
        return QChar(key);
    }

    // Hexadecimal digits
    if (key >= 0x41 && key <= 0x46) {
        return QChar(key);
    }

    return QString("Unknown (0x%1)").arg(key, 2, 16, QChar('0')).toUpper();
}

int Shortcut::vkToQtKey(quint32 vk) {
    // Convert Win32 virtual key to Qt key code
    if (vk >= 'A' && vk <= 'Z') return Qt::Key_A + (vk - 'A');
    if (vk >= '0' && vk <= '9') return Qt::Key_0 + (vk - '0');
    if (vk >= NppKeys::VK_F1 && vk <= NppKeys::VK_F12) return Qt::Key_F1 + (vk - NppKeys::VK_F1);
    if (vk >= NppKeys::VK_F13 && vk <= NppKeys::VK_F24) return Qt::Key_F13 + (vk - NppKeys::VK_F13);

    switch (vk) {
        case NppKeys::VK_BACK:    return Qt::Key_Backspace;
        case NppKeys::VK_TAB:     return Qt::Key_Tab;
        case NppKeys::VK_RETURN:  return Qt::Key_Return;
        case NppKeys::VK_ESCAPE:  return Qt::Key_Escape;
        case NppKeys::VK_SPACE:   return Qt::Key_Space;
        case NppKeys::VK_END:     return Qt::Key_End;
        case NppKeys::VK_HOME:    return Qt::Key_Home;
        case NppKeys::VK_LEFT:    return Qt::Key_Left;
        case NppKeys::VK_UP:      return Qt::Key_Up;
        case NppKeys::VK_RIGHT:   return Qt::Key_Right;
        case NppKeys::VK_DOWN:    return Qt::Key_Down;
        case NppKeys::VK_INSERT:  return Qt::Key_Insert;
        case NppKeys::VK_DELETE:  return Qt::Key_Delete;
        case NppKeys::VK_PRIOR:   return Qt::Key_PageUp;
        case NppKeys::VK_NEXT:    return Qt::Key_PageDown;
        case NppKeys::VK_MULTIPLY: return Qt::Key_Asterisk;
        case NppKeys::VK_ADD:     return Qt::Key_Plus;
        case NppKeys::VK_SUBTRACT: return Qt::Key_Minus;
        case NppKeys::VK_DECIMAL: return Qt::Key_Period;
        case NppKeys::VK_DIVIDE:  return Qt::Key_Slash;
        case NppKeys::VK_NUMLOCK: return Qt::Key_NumLock;
        case NppKeys::VK_SCROLL:  return Qt::Key_ScrollLock;
        case NppKeys::VK_CAPITAL: return Qt::Key_CapsLock;
        case NppKeys::VK_NUMPAD0: return Qt::Key_0;
        case NppKeys::VK_NUMPAD1: return Qt::Key_1;
        case NppKeys::VK_NUMPAD2: return Qt::Key_2;
        case NppKeys::VK_NUMPAD3: return Qt::Key_3;
        case NppKeys::VK_NUMPAD4: return Qt::Key_4;
        case NppKeys::VK_NUMPAD5: return Qt::Key_5;
        case NppKeys::VK_NUMPAD6: return Qt::Key_6;
        case NppKeys::VK_NUMPAD7: return Qt::Key_7;
        case NppKeys::VK_NUMPAD8: return Qt::Key_8;
        case NppKeys::VK_NUMPAD9: return Qt::Key_9;
        case NppKeys::VK_OEM_PLUS:   return Qt::Key_Equal;
        case NppKeys::VK_OEM_MINUS:  return Qt::Key_Minus;
        case NppKeys::VK_OEM_COMMA:  return Qt::Key_Comma;
        case NppKeys::VK_OEM_PERIOD: return Qt::Key_Period;
        case NppKeys::VK_OEM_1:      return Qt::Key_Semicolon;
        case NppKeys::VK_OEM_2:      return Qt::Key_Slash;
        case NppKeys::VK_OEM_3:      return Qt::Key_QuoteLeft;
        case NppKeys::VK_OEM_4:      return Qt::Key_BracketLeft;
        case NppKeys::VK_OEM_5:      return Qt::Key_Backslash;
        case NppKeys::VK_OEM_6:      return Qt::Key_BracketRight;
        case NppKeys::VK_OEM_7:      return Qt::Key_QuoteLeft;
        default:            return 0;
    }
}

quint32 Shortcut::qtKeyToVk(int qtKey) {
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z)
        return 'A' + (qtKey - Qt::Key_A);
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9)
        return '0' + (qtKey - Qt::Key_0);
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12)
        return NppKeys::VK_F1 + (qtKey - Qt::Key_F1);

    switch (qtKey) {
        case Qt::Key_Backspace: return NppKeys::VK_BACK;
        case Qt::Key_Tab:       return NppKeys::VK_TAB;
        case Qt::Key_Return:    return NppKeys::VK_RETURN;
        case Qt::Key_Escape:    return NppKeys::VK_ESCAPE;
        case Qt::Key_Space:     return NppKeys::VK_SPACE;
        case Qt::Key_End:       return NppKeys::VK_END;
        case Qt::Key_Home:      return NppKeys::VK_HOME;
        case Qt::Key_Left:      return NppKeys::VK_LEFT;
        case Qt::Key_Up:        return NppKeys::VK_UP;
        case Qt::Key_Right:     return NppKeys::VK_RIGHT;
        case Qt::Key_Down:      return NppKeys::VK_DOWN;
        case Qt::Key_Insert:    return NppKeys::VK_INSERT;
        case Qt::Key_Delete:    return NppKeys::VK_DELETE;
        case Qt::Key_PageUp:    return NppKeys::VK_PRIOR;
        case Qt::Key_PageDown:  return NppKeys::VK_NEXT;
        default:                return 0;
    }
}

// Helper: build specialKeys map using explicit insertion (Qt6/C++20 compatible)
static QMap<QString, quint32> buildSpecialKeysMap() {
    QMap<QString, quint32> m;
    m.insert("Backspace", NppKeys::VK_BACK);
    m.insert("Tab", NppKeys::VK_TAB);
    m.insert("Enter", NppKeys::VK_RETURN);
    m.insert("Return", NppKeys::VK_RETURN);
    m.insert("Escape", NppKeys::VK_ESCAPE);
    m.insert("Esc", NppKeys::VK_ESCAPE);
    m.insert("Space", NppKeys::VK_SPACE);
    m.insert("End", NppKeys::VK_END);
    m.insert("Home", NppKeys::VK_HOME);
    m.insert("Left", NppKeys::VK_LEFT);
    m.insert("Up", NppKeys::VK_UP);
    m.insert("Right", NppKeys::VK_RIGHT);
    m.insert("Down", NppKeys::VK_DOWN);
    m.insert("Insert", NppKeys::VK_INSERT);
    m.insert("Delete", NppKeys::VK_DELETE);
    m.insert("Del", NppKeys::VK_DELETE);
    m.insert("PageUp", NppKeys::VK_PRIOR);
    m.insert("Page Down", NppKeys::VK_PRIOR);
    m.insert("Prior", NppKeys::VK_PRIOR);
    m.insert("Next", NppKeys::VK_NEXT);
    m.insert("Num0", NppKeys::VK_NUMPAD0);
    m.insert("Num1", NppKeys::VK_NUMPAD1);
    m.insert("Num2", NppKeys::VK_NUMPAD2);
    m.insert("Num3", NppKeys::VK_NUMPAD3);
    m.insert("Num4", NppKeys::VK_NUMPAD4);
    m.insert("Num5", NppKeys::VK_NUMPAD5);
    m.insert("Num6", NppKeys::VK_NUMPAD6);
    m.insert("Num7", NppKeys::VK_NUMPAD7);
    m.insert("Num8", NppKeys::VK_NUMPAD8);
    m.insert("Num9", NppKeys::VK_NUMPAD9);
    m.insert("Num*", NppKeys::VK_MULTIPLY);
    m.insert("Num+", NppKeys::VK_ADD);
    m.insert("Num-", NppKeys::VK_SUBTRACT);
    m.insert("Num.", NppKeys::VK_DECIMAL);
    m.insert("Num/", NppKeys::VK_DIVIDE);
    m.insert("Multiply", NppKeys::VK_MULTIPLY);
    m.insert("Add", NppKeys::VK_ADD);
    m.insert("Subtract", NppKeys::VK_SUBTRACT);
    m.insert("Decimal", NppKeys::VK_DECIMAL);
    m.insert("Divide", NppKeys::VK_DIVIDE);
    m.insert("F1", NppKeys::VK_F1);
    m.insert("F2", NppKeys::VK_F2);
    m.insert("F3", NppKeys::VK_F3);
    m.insert("F4", NppKeys::VK_F4);
    m.insert("F5", NppKeys::VK_F5);
    m.insert("F6", NppKeys::VK_F6);
    m.insert("F7", NppKeys::VK_F7);
    m.insert("F8", NppKeys::VK_F8);
    m.insert("F9", NppKeys::VK_F9);
    m.insert("F10", NppKeys::VK_F10);
    m.insert("F11", NppKeys::VK_F11);
    m.insert("F12", NppKeys::VK_F12);
    m.insert("CapsLock", NppKeys::VK_CAPITAL);
    m.insert("Caps", NppKeys::VK_CAPITAL);
    m.insert("NumLock", NppKeys::VK_NUMLOCK);
    m.insert("ScrollLock", NppKeys::VK_SCROLL);
    return m;
}

quint32 Shortcut::parseKeyName(const QString& name) {
    QString n = name.trimmed();

    // Function keys
    if (n.startsWith("F", Qt::CaseInsensitive) && n.length() > 1) {
        bool ok;
        int num = n.mid(1).toInt(&ok);
        if (ok && num >= 1 && num <= 24) {
            return NppKeys::VK_F1 + (num - 1);
        }
    }

    // Special keys
    static const QMap<QString, quint32> specialKeys = buildSpecialKeysMap();

    auto it = specialKeys.find(n);
    if (it != specialKeys.end()) {
        return it.value();
    }

    // Single character keys
    if (n.length() == 1) {
        QChar c = n[0].toUpper();
        if (c.isLetter()) return c.unicode();
        if (c.isDigit()) return c.unicode();
    }

    // OEM keys
    if (n.length() == 1) {
        char ch = n[0].toLatin1();
        switch (ch) {
            case ';': return 0xBA;
            case '=': return 0xBB;
            case ',': return 0xBC;
            case '-': return 0xBD;
            case '.': return 0xBE;
            case '/': return 0xBF;
            case '`': return 0xC0;
            case '[': return 0xDB;
            case '\\': return 0xDC;
            case ']': return 0xDD;
            case '\'': return 0xDE;
            default: break;
        }
    }

    return 0;
}

QString Shortcut::parseFromString(const QString& str) {
    // Parse a string like "Ctrl+Shift+S" into key components
    // Returns error message if invalid, empty string if OK
    QStringList parts = str.split('+', Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (parts.isEmpty()) return "Empty shortcut";

    _keyCombo._isCtrl = false;
    _keyCombo._isAlt = false;
    _keyCombo._isShift = false;
    _keyCombo._key = 0;

    for (const QString& part : parts) {
        QString p = part.trimmed();
        if (p.isEmpty()) continue;

        if (p.compare("Ctrl", Qt::CaseInsensitive) == 0 || p == "^") {
            _keyCombo._isCtrl = true;
        } else if (p.compare("Alt", Qt::CaseInsensitive) == 0 || p == "%") {
            _keyCombo._isAlt = true;
        } else if (p.compare("Shift", Qt::CaseInsensitive) == 0 || p == "+") {
            _keyCombo._isShift = true;
        } else if (p.compare("Win", Qt::CaseInsensitive) == 0 || p == "Super") {
            // Windows key - not directly supported in N++
        } else {
            // This should be the key
            if (_keyCombo._key != 0) {
                return QString("Multiple keys specified: %1 and %2").arg(keyToString(_keyCombo._key)).arg(p);
            }

            // Try to parse the key
            _keyCombo._key = parseKeyName(p);
            if (_keyCombo._key == 0) {
                return QString("Unknown key: %1").arg(p);
            }
        }
    }

    if (_keyCombo._key == 0) {
        return "No key specified";
    }

    return QString();
}

// Additional Shortcut methods
bool Shortcut::conflictsWith(const Shortcut& other) const {
    return _keyCombo._isCtrl == other._keyCombo._isCtrl &&
           _keyCombo._isAlt == other._keyCombo._isAlt &&
           _keyCombo._isShift == other._keyCombo._isShift &&
           _keyCombo._key == other._keyCombo._key;
}

QString Shortcut::canonical() const {
    // Normalize modifier order: Ctrl < Alt < Shift
    return toString();
}

QString Shortcut::toJson() const {
    QJsonObject obj;
    obj["ctrl"] = _keyCombo._isCtrl;
    obj["alt"] = _keyCombo._isAlt;
    obj["shift"] = _keyCombo._isShift;
    obj["key"] = QString::number(_keyCombo._key, 16);
    obj["name"] = _name;
    obj["menuName"] = _menuName;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

bool Shortcut::fromJson(const QString& json) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) return false;

    QJsonObject obj = doc.object();
    _keyCombo._isCtrl = obj["ctrl"].toBool();
    _keyCombo._isAlt = obj["alt"].toBool();
    _keyCombo._isShift = obj["shift"].toBool();
    _keyCombo._key = obj["key"].toString().toUInt(nullptr, 16);
    _name = obj["name"].toString();
    _menuName = obj["menuName"].toString();
    return true;
}
