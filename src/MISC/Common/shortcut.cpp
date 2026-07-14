// Ported to Qt6: Keyboard shortcut implementation
#include "shortcut.h"
#include "NppNotification.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QDebug>

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
        case VK_BACK:    return "Backspace";
        case VK_TAB:     return "Tab";
        case VK_RETURN:  return "Enter";
        case VK_ESCAPE:  return "Escape";
        case VK_SPACE:   return "Space";
        case VK_END:     return "End";
        case VK_HOME:    return "Home";
        case VK_LEFT:    return "Left";
        case VK_UP:      return "Up";
        case VK_RIGHT:   return "Right";
        case VK_DOWN:    return "Down";
        case VK_INSERT:  return "Insert";
        case VK_DELETE:  return "Delete";
        case VK_PRIOR:   return "PageUp";
        case VK_NEXT:    return "PageDown";
        case VK_MULTIPLY: return "*";
        case VK_ADD:     return "+";
        case VK_SUBTRACT: return "-";
        case VK_DECIMAL: return ".";
        case VK_DIVIDE:  return "/";
        case VK_NUMLOCK: return "Num Lock";
        case VK_SCROLL:  return "Scroll";
        case VK_LSHIFT:  return "Left Shift";
        case VK_RSHIFT:  return "Right Shift";
        case VK_LCONTROL: return "Left Ctrl";
        case VK_RCONTROL: return "Right Ctrl";
        case VK_LMENU:   return "Left Alt";
        case VK_RMENU:   return "Right Alt";
        case VK_CAPITAL: return "Caps Lock";
        case VK_LWIN:    return "Left Win";
        case VK_RWIN:    return "Right Win";
        case VK_APPS:    return "Menu";
        default:
            break;
    }

    // Function keys
    if (key >= VK_F1 && key <= VK_F24) {
        return QString("F%1").arg(key - VK_F1 + 1);
    }

    // Numpad keys
    if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
        return QString("Num %1").arg(key - VK_NUMPAD0);
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
    if (vk >= VK_F1 && vk <= VK_F12) return Qt::Key_F1 + (vk - VK_F1);
    if (vk >= VK_F13 && vk <= VK_F24) return Qt::Key_F13 + (vk - VK_F13);

    switch (vk) {
        case VK_BACK:    return Qt::Key_Backspace;
        case VK_TAB:     return Qt::Key_Tab;
        case VK_RETURN:  return Qt::Key_Return;
        case VK_ESCAPE:  return Qt::Key_Escape;
        case VK_SPACE:   return Qt::Key_Space;
        case VK_END:     return Qt::Key_End;
        case VK_HOME:    return Qt::Key_Home;
        case VK_LEFT:    return Qt::Key_Left;
        case VK_UP:      return Qt::Key_Up;
        case VK_RIGHT:   return Qt::Key_Right;
        case VK_DOWN:    return Qt::Key_Down;
        case VK_INSERT:  return Qt::Key_Insert;
        case VK_DELETE:  return Qt::Key_Delete;
        case VK_PRIOR:   return Qt::Key_PageUp;
        case VK_NEXT:    return Qt::Key_PageDown;
        case VK_MULTIPLY: return Qt::Key_Asterisk;
        case VK_ADD:     return Qt::Key_Plus;
        case VK_SUBTRACT: return Qt::Key_Minus;
        case VK_DECIMAL: return Qt::Key_Period;
        case VK_DIVIDE:  return Qt::Key_Slash;
        case VK_NUMLOCK: return Qt::Key_NumLock;
        case VK_SCROLL:  return Qt::Key_ScrollLock;
        case VK_CAPITAL: return Qt::Key_CapsLock;
        case VK_NUMPAD0: return Qt::Key_0;
        case VK_NUMPAD1: return Qt::Key_1;
        case VK_NUMPAD2: return Qt::Key_2;
        case VK_NUMPAD3: return Qt::Key_3;
        case VK_NUMPAD4: return Qt::Key_4;
        case VK_NUMPAD5: return Qt::Key_5;
        case VK_NUMPAD6: return Qt::Key_6;
        case VK_NUMPAD7: return Qt::Key_7;
        case VK_NUMPAD8: return Qt::Key_8;
        case VK_NUMPAD9: return Qt::Key_9;
        case VK_OEM_PLUS:   return Qt::Key_Equal;
        case VK_OEM_MINUS:  return Qt::Key_Minus;
        case VK_OEM_COMMA:  return Qt::Key_Comma;
        case VK_OEM_PERIOD: return Qt::Key_Period;
        case VK_OEM_1:      return Qt::Key_Semicolon;
        case VK_OEM_2:      return Qt::Key_Slash;
        case VK_OEM_3:      return Qt::Key_QuoteLeft;
        case VK_OEM_4:      return Qt::Key_BracketLeft;
        case VK_OEM_5:      return Qt::Key_Backslash;
        case VK_OEM_6:      return Qt::Key_BracketRight;
        case VK_OEM_7:      return Qt::Key_QuoteLeft;
        default:            return 0;
    }
}

quint32 Shortcut::qtKeyToVk(int qtKey) {
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z)
        return 'A' + (qtKey - Qt::Key_A);
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9)
        return '0' + (qtKey - Qt::Key_0);
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12)
        return VK_F1 + (qtKey - Qt::Key_F1);

    switch (qtKey) {
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Tab:       return VK_TAB;
        case Qt::Key_Return:    return VK_RETURN;
        case Qt::Key_Escape:    return VK_ESCAPE;
        case Qt::Key_Space:     return VK_SPACE;
        case Qt::Key_End:       return VK_END;
        case Qt::Key_Home:      return VK_HOME;
        case Qt::Key_Left:      return VK_LEFT;
        case Qt::Key_Up:        return VK_UP;
        case Qt::Key_Right:     return VK_RIGHT;
        case Qt::Key_Down:      return VK_DOWN;
        case Qt::Key_Insert:    return VK_INSERT;
        case Qt::Key_Delete:    return VK_DELETE;
        case Qt::Key_PageUp:    return VK_PRIOR;
        case Qt::Key_PageDown:  return VK_NEXT;
        default:                return 0;
    }
}

QString Shortcut::parseFromString(const QString& str) {
    // Parse a string like "Ctrl+Shift+S" into key components
    // Returns error message if invalid, empty string if OK
    QStringList parts = str.split('+', Qt::SkipEmptyParts);
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

quint32 Shortcut::parseKeyName(const QString& name) {
    QString n = name.trimmed();

    // Function keys
    if (n.startsWith("F", Qt::CaseInsensitive) && n.length() > 1) {
        bool ok;
        int num = n.mid(1).toInt(&ok);
        if (ok && num >= 1 && num <= 24) {
            return VK_F1 + (num - 1);
        }
    }

    // Special keys
    static const QMap<QString, quint32> specialKeys = {
        {"Backspace", VK_BACK},
        {"Tab", VK_TAB},
        {"Enter", VK_RETURN},
        {"Return", VK_RETURN},
        {"Escape", VK_ESCAPE},
        {"Esc", VK_ESCAPE},
        {"Space", VK_SPACE},
        {"End", VK_END},
        {"Home", VK_HOME},
        {"Left", VK_LEFT},
        {"Up", VK_UP},
        {"Right", VK_RIGHT},
        {"Down", VK_DOWN},
        {"Insert", VK_INSERT},
        {"Delete", VK_DELETE},
        {"Del", VK_DELETE},
        {"PageUp", VK_PRIOR},
        {"Page Down", VK_PRIOR},
        {"PageUp", VK_PRIOR},
        {"Prior", VK_PRIOR},
        {"Next", VK_NEXT},
        {"Num0", VK_NUMPAD0},
        {"Num1", VK_NUMPAD1},
        {"Num2", VK_NUMPAD2},
        {"Num3", VK_NUMPAD3},
        {"Num4", VK_NUMPAD4},
        {"Num5", VK_NUMPAD5},
        {"Num6", VK_NUMPAD6},
        {"Num7", VK_NUMPAD7},
        {"Num8", VK_NUMPAD8},
        {"Num9", VK_NUMPAD9},
        {"Num*", VK_MULTIPLY},
        {"Num+", VK_ADD},
        {"Num-", VK_SUBTRACT},
        {"Num.", VK_DECIMAL},
        {"Num/", VK_DIVIDE},
        {"Multiply", VK_MULTIPLY},
        {"Add", VK_ADD},
        {"Subtract", VK_SUBTRACT},
        {"Decimal", VK_DECIMAL},
        {"Divide", VK_DIVIDE},
        {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4},
        {"F5", VK_F5}, {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8},
        {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
        {"CapsLock", VK_CAPITAL},
        {"Caps", VK_CAPITAL},
        {"NumLock", VK_NUMLOCK},
        {"ScrollLock", VK_SCROLL},
    };

    auto it = specialKeys.find(n);
    if (it != specialKeys.end()) {
        return it.value();
    }

    // Single character keys
    if (n.length() == 1) {
        QChar c = n[0].toUpper()[0];
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

// Static member definitions
QString (*Shortcut::keyToStringFunc)(quint32) = nullptr;

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
    _keyCombo._key = obj["key"].toString().toUInt(Q_NULLPTR, 16);
    _name = obj["name"].toString();
    _menuName = obj["menuName"].toString();
    return true;
}

// KeyCombo operator overloads
bool KeyCombo::operator==(const KeyCombo& other) const {
    return _isCtrl == other._isCtrl && _isAlt == other._isAlt &&
           _isShift == other._isShift && _key == other._key;
}
