// Ported to Qt6
#include "shortcut.h"
#include <QApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QObject>
#include "Common.h"
#include "menuCmdID.h"
#include "NppCommands.h"

Shortcut::Shortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint32 key, QObject* parent)
    : QObject(parent)
{
    setName(name);
    _keyCombo._isCtrl = isCtrl;
    _keyCombo._isAlt = isAlt;
    _keyCombo._isShift = isShift;
    _keyCombo._key = key;
}

void Shortcut::setName(const QString& menuName, const QString& shortcutName)
{
    _menuName = menuName;
    if (!shortcutName.isEmpty())
        _name = shortcutName;
    else
        _name = menuName;
}

QKeySequence Shortcut::toQKeySequence() const
{
    int key = _keyCombo._key;
    Qt::KeyboardModifiers mods;
    if (_keyCombo._isCtrl) mods |= Qt::ControlModifier;
    if (_keyCombo._isAlt) mods |= Qt::AltModifier;
    if (_keyCombo._isShift) mods |= Qt::ShiftModifier;
    int qtKey = key;
    switch (key) {
        case VK_BACK: qtKey = Qt::Key_Backspace; break;
        case VK_TAB: qtKey = Qt::Key_Tab; break;
        case VK_RETURN: qtKey = Qt::Key_Return; break;
        case VK_ESCAPE: qtKey = Qt::Key_Escape; break;
        case VK_SPACE: qtKey = Qt::Key_Space; break;
        case VK_DELETE: qtKey = Qt::Key_Delete; break;
        case VK_INSERT: qtKey = Qt::Key_Insert; break;
        case VK_HOME: qtKey = Qt::Key_Home; break;
        case VK_END: qtKey = Qt::Key_End; break;
        case VK_PRIOR: qtKey = Qt::Key_PageUp; break;
        case VK_NEXT: qtKey = Qt::Key_PageDown; break;
        case VK_UP: qtKey = Qt::Key_Up; break;
        case VK_DOWN: qtKey = Qt::Key_Down; break;
        case VK_LEFT: qtKey = Qt::Key_Left; break;
        case VK_RIGHT: qtKey = Qt::Key_Right; break;
        default:
            if (key >= 'A' && key <= 'Z') qtKey = Qt::Key_A + (key - 'A');
            else if (key >= '0' && key <= '9') qtKey = Qt::Key_0 + (key - '0');
            break;
    }
    return QKeySequence(mods | qtKey);
}

QString Shortcut::toString() const
{
    QString str;
    if (_keyCombo._isCtrl) str += "Ctrl+";
    if (_keyCombo._isAlt) str += "Alt+";
    if (_keyCombo._isShift) str += "Shift+";
    int key = _keyCombo._key;
    switch (key) {
        case VK_BACK: str += "Backspace"; break;
        case VK_TAB: str += "Tab"; break;
        case VK_RETURN: str += "Enter"; break;
        case VK_ESCAPE: str += "Esc"; break;
        case VK_SPACE: str += "Space"; break;
        case VK_DELETE: str += "Delete"; break;
        default:
            if (key >= 'A' && key <= 'Z') str += QChar(key);
            else if (key >= '0' && key <= '9') str += QChar(key);
            else str += QChar(key);
            break;
    }
    return str;
}

QString Shortcut::toMenuItemString() const
{
    QString str = _menuName;
    if (isEnabled()) { str += "\t"; str += toString(); }
    return str;
}

Shortcut& Shortcut::operator=(const Shortcut& sc)
{
    if (_name.isEmpty() || !sc._name.isEmpty())
        setName(sc._menuName, sc._name);
    _keyCombo = sc._keyCombo;
    _canModifyName = sc._canModifyName;
    return *this;
}

CommandShortcut::CommandShortcut(const Shortcut& sc, unsigned long id, QObject* parent)
    : Shortcut(sc.getMenuName(), sc.getKeyCombo()._isCtrl, sc.getKeyCombo()._isAlt,
               sc.getKeyCombo()._isShift, sc.getKeyCombo()._key, parent), _id(id)
{
    _shortcutName = sc.getName();
}

CommandShortcut::CommandShortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift,
                                 quint32 key, unsigned long id, QObject* parent)
    : Shortcut(name, isCtrl, isAlt, isShift, key, parent), _id(id) {}

ScintillaKeyMap::ScintillaKeyMap(const Shortcut& sc, unsigned long scintillaKeyID,
                                   unsigned long menuCmdID, QObject* parent)
    : Shortcut(sc.getMenuName(), sc.getKeyCombo()._isCtrl, sc.getKeyCombo()._isAlt,
               sc.getKeyCombo()._isShift, sc.getKeyCombo()._key, parent),
      _scintillaKeyID(scintillaKeyID), _menuCmdID(static_cast<int>(menuCmdID))
{
    _keyCombos.append(_keyCombo);
    _keyCombo._key = 0;
}

bool ScintillaKeyMap::isEnabled() const
{
    return !_keyCombos.isEmpty() && _keyCombos[0]._key != 0;
}

void ScintillaKeyMap::addKeyCombo(KeyCombo combo)
{
    for (const KeyCombo& existing : _keyCombos)
        if (existing._isCtrl == combo._isCtrl && existing._isAlt == combo._isAlt &&
            existing._isShift == combo._isShift && existing._key == combo._key) return;
    _keyCombos.append(combo);
}

void ScintillaKeyMap::clearDups()
{
    if (_keyCombos.size() > 1) _keyCombos = _keyCombos.mid(0, 1);
}

QString ScintillaKeyMap::toString() const
{
    if (_keyCombos.isEmpty()) return QString();
    Shortcut tmp(_keyCombos[0]._key ? "x" : "",
                 _keyCombos[0]._isCtrl, _keyCombos[0]._isAlt, _keyCombos[0]._isShift, _keyCombos[0]._key);
    return tmp.toString();
}

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam), _macroType(mtMenuCommand) {}

RecordedMacroStep::RecordedMacroStep(int iCommandID)
    : _wParameter(static_cast<uintptr_t>(iCommandID)), _macroType(mtMenuCommand) {}

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam,
                                     const QString& sParam, int type)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam),
      _sParameter(sParam), _macroType(static_cast<MacroTypeIndex>(type)) {}

RecordedMacroStep::RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam,
                                     const char* sParam, int type)
    : _message(iMessage), _wParameter(wParam), _lParameter(lParam),
      _sParameter(QString::fromLatin1(sParam ? sParam : "")),
      _macroType(static_cast<MacroTypeIndex>(type)) {}

void RecordedMacroStep::playBack(ScintillaEditView* view)
{
    if (!view)
        return;

    if (_macroType == mtMenuCommand) {
        // Menu command: dispatch via NppCommands::recordMacroStepMenu()
        // The view's window is Notepad_plus (QMainWindow), which owns NppCommands.
        // Use qobject_cast to get the NppCommands instance.
        QWidget* w = view->window();
        if (w) {
            NppCommands* nppCmd = qobject_cast<NppCommands*>(w);
            if (!nppCmd) {
                // Try parent chain
                nppCmd = qobject_cast<NppCommands*>(w->parent());
            }
            if (nppCmd) {
                nppCmd->recordMacroStepMenu(static_cast<int>(_wParameter));
            }
        }
        return;
    }

    // Ensure it's a macroable message before sending
    if (!isMacroable())
        return;

    if (_macroType == mtUseSParameter) {
        // String parameter — convert QString to UTF-8 for Scintilla
        QByteArray utf8 = _sParameter.toUtf8();
        view->send(_message, static_cast<uintptr_t>(_wParameter),
                   reinterpret_cast<uintptr_t>(utf8.constData()));
    } else {
        // Numeric (lParam) parameter
        view->send(_message, _wParameter, _lParameter);
    }
}

MacroShortcut::MacroShortcut(const Shortcut& sc, const Macro& macro, int id, QObject* parent)
    : CommandShortcut(sc, static_cast<unsigned long>(id), parent), _macro(macro)
{
    _canModifyName = true;
}

UserCommand::UserCommand(const Shortcut& sc, const QString& cmd, int id, QObject* parent)
    : CommandShortcut(sc, static_cast<unsigned long>(id), parent), _cmd(cmd)
{
    _canModifyName = true;
}

PluginCmdShortcut::PluginCmdShortcut(const Shortcut& sc, int id, const QString& moduleName,
                                     int internalID, QObject* parent)
    : CommandShortcut(sc, static_cast<unsigned long>(id), parent),
      _id(static_cast<unsigned long>(id)), _moduleName(moduleName), _internalID(internalID) {}

bool PluginCmdShortcut::isValid() const
{
    if (!Shortcut::isValid()) return false;
    if (_moduleName.isEmpty() || _internalID == -1) return false;
    return true;
}

Accelerator::Accelerator(QObject* parent) : QObject(parent) {}
Accelerator::~Accelerator() {}

void Accelerator::init(QMenu* hMenu, QWidget* menuParent)
{
    _hAccelMenu = hMenu;
    _hMenuParent = menuParent;
    updateShortcuts();
}

void Accelerator::updateShortcuts() { /* Update QAction shortcuts */ }
void Accelerator::updateFullMenu() { /* Rebuild menu */ }
void Accelerator::updateMenuItemByCommand(const CommandShortcut&) { /* Update menu item */ }

ScintillaAccelerator::ScintillaAccelerator(QObject* parent) : QObject(parent) {}

void ScintillaAccelerator::init(QVector<QWidget*> vScintillas, QMenu* hMenu, QWidget* menuParent)
{
    _vScintillas = vScintillas;
    _hAccelMenu = hMenu;
    _hMenuParent = menuParent;
    updateKeys();
}

void ScintillaAccelerator::updateKeys() { /* Update Scintilla key bindings */ }

void getKeyStrFromVal(quint8 keyVal, QString& str)
{
    switch (keyVal) {
        case VK_BACK: str = "Backspace"; break;
        case VK_TAB: str = "Tab"; break;
        case VK_RETURN: str = "Enter"; break;
        case VK_ESCAPE: str = "Esc"; break;
        case VK_SPACE: str = "Space"; break;
        case VK_DELETE: str = "Delete"; break;
        case VK_INSERT: str = "Insert"; break;
        case VK_HOME: str = "Home"; break;
        case VK_END: str = "End"; break;
        case VK_PRIOR: str = "PageUp"; break;
        case VK_NEXT: str = "PageDown"; break;
        case VK_UP: str = "Up"; break;
        case VK_DOWN: str = "Down"; break;
        case VK_LEFT: str = "Left"; break;
        case VK_RIGHT: str = "Right"; break;
        default:
            if (keyVal >= 'A' && keyVal <= 'Z') str = QChar(keyVal);
            else if (keyVal >= '0' && keyVal <= '9') str = QChar(keyVal);
            else str = "?";
            break;
    }
}

void getNameStrFromCmd(unsigned long cmd, QString& str)
{
    Q_UNUSED(cmd);
    str = QString();
}

// Mirrors Win32 CommandShortcut::setCategoryFromMenu(HMENU)
// Maps command ID ranges to menu category names for the Shortcut Mapper
void CommandShortcut::setCategoryFromMenu(unsigned long cmdID)
{
    if (cmdID >= IDM_WINDOW_WINDOWS && cmdID <= IDM_WINDOW_SORT_FS_DSC)
        _category = QStringLiteral("Window");
    else if (cmdID >= IDM_VIEW_GOTO_ANOTHER_VIEW && cmdID <= IDM_VIEW_GOTO_END)
        _category = QStringLiteral("View");
    else if (cmdID == IDM_EDIT_LTR || cmdID == IDM_EDIT_RTL)
        _category = QStringLiteral("View");
    else if (cmdID == IDC_PREV_DOC || cmdID == IDC_NEXT_DOC)
        _category = QStringLiteral("View");
    else if (cmdID == IDM_FORMAT_TODOS || cmdID == IDM_FORMAT_TOUNIX || cmdID == IDM_FORMAT_TOMAC)
        _category = QStringLiteral("Edit");
    else if (cmdID == IDM_EDIT_AUTOCOMPLETE || cmdID == IDM_EDIT_AUTOCOMPLETE_CURRENTFILE ||
             cmdID == IDM_EDIT_FUNCCALLTIP || cmdID == IDM_EDIT_AUTOCOMPLETE_PATH ||
             cmdID == IDM_EDIT_FUNCCALLTIP_PREVIOUS || cmdID == IDM_EDIT_FUNCCALLTIP_NEXT)
        _category = QStringLiteral("Edit");
    else if (cmdID == IDM_LANGSTYLE_CONFIG_DLG)
        _category = QStringLiteral("Settings");
    else if (cmdID == IDM_MACRO_STARTRECORDINGMACRO || cmdID == IDM_MACRO_STOPRECORDINGMACRO ||
             cmdID == IDM_MACRO_RUNMULTIMACRODLG || cmdID == IDM_MACRO_PLAYBACKRECORDEDMACRO ||
             cmdID == IDM_MACRO_SAVECURRENTMACRO || cmdID == IDC_EDIT_TOGGLEMACRORECORDING)
        _category = QStringLiteral("Macro");
    else if (cmdID < IDM_EDIT)
        _category = QStringLiteral("File");
    else if (cmdID < IDM_SEARCH)
        _category = QStringLiteral("Edit");
    else if (cmdID < IDM_VIEW)
        _category = QStringLiteral("Search");
    else if (cmdID < IDM_FORMAT)
        _category = QStringLiteral("View");
    else if (cmdID < IDM_LANGUAGE)
        _category = QStringLiteral("Format");
    else if (cmdID < IDM_SETTING)
        _category = QStringLiteral("Language");
    else if (cmdID < IDM_TOOLS)
        _category = QStringLiteral("Settings");
    else if (cmdID < IDM_WINDOW)
        _category = QStringLiteral("Tools");
    else if (cmdID < IDM_EDIT)
        _category = QStringLiteral("Window");
    else
        _category = QStringLiteral("Unknown");
}
