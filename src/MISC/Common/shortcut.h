// Ported to Qt6: Keyboard shortcut management
#pragma once
#include <QKeySequence>
#include <QMenu>
#include <QObject>
#include <QShortcut>
#include "../NppConstants.h"
#include "keys.h"      // NppKeys::VK_* virtual key constants
using namespace NppKeys;  // bare VK_* names used throughout shortcut.h
#include <QString>
#include <QVector>
// NppSciCompat.h defines all needed SCI_* constants as constexpr.
// No need for Scintilla.h here — its macros would conflict with qsciscintilla.h.
#include "NppSciCompat.h"
#include "menuCmdID.h"

class NppParameters;

struct KeyCombo {
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    quint32 _key = 0;
};

static inline quint32 keyTranslate(quint32 keyIn) {
    switch (keyIn) {
        case 0x28: return SCK_DOWN;
        case 0x26: return SCK_UP;
        case 0x25: return SCK_LEFT;
        case 0x27: return SCK_RIGHT;
        case 0x24: return SCK_HOME;
        case 0x23: return SCK_END;
        case 0x21: return SCK_PRIOR;
        case 0x22: return SCK_NEXT;
        case 0x2E: return SCK_DELETE;
        case 0x2D: return SCK_INSERT;
        case 0x1B: return SCK_ESCAPE;
        case 0x08: return SCK_BACK;
        case 0x09: return SCK_TAB;
        case 0x0D: return SCK_RETURN;
        case 0x6B: return SCK_ADD;
        case 0x6D: return SCK_SUBTRACT;
        case 0x6F: return SCK_DIVIDE;
        default:   return keyIn;
    }
}

class Shortcut : public QObject {
    Q_OBJECT
public:
    Shortcut(QObject* parent = nullptr) : QObject(parent) {}
    Shortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint32 key, QObject* parent = nullptr);
    void setName(const QString& menuName, const QString& shortcutName = QString());
    virtual bool isEnabled() const { return _keyCombo._key != 0; }
    virtual bool isValid() const {
        if (_keyCombo._key == 0) return true;
        quint32 k = _keyCombo._key;
        if ((k >= 'A' && k <= 'Z') || (k >= '0' && k <= '9') || k == VK_SPACE || k == VK_CAPITAL || k == VK_BACK || k == VK_RETURN)
            return _keyCombo._isCtrl || _keyCombo._isAlt;
        return true;
    }
    const KeyCombo& getKeyCombo() const { return _keyCombo; }
    const QString& getName() const { return _name; }
    const QString& getMenuName() const { return _menuName; }
    void clear() { _keyCombo._isCtrl = false; _keyCombo._isAlt = false; _keyCombo._isShift = false; _keyCombo._key = 0; }
    virtual QString toString() const;
    QString toMenuItemString() const;
    QKeySequence toQKeySequence() const;
    Shortcut& operator=(const Shortcut& sc);
    friend inline bool operator==(const Shortcut& a, const Shortcut& b) {
        return a._keyCombo._isCtrl == b._keyCombo._isCtrl && a._keyCombo._isAlt == b._keyCombo._isAlt && a._keyCombo._isShift == b._keyCombo._isShift && a._keyCombo._key == b._keyCombo._key;
    }
    friend inline bool operator!=(const Shortcut& a, const Shortcut& b) { return !(a == b); }
signals: void triggered();
protected:
    KeyCombo _keyCombo;
    QString _name;
    QString _menuName;
    bool _canModifyName = false;
};

class CommandShortcut : public Shortcut {
    Q_OBJECT
public:
    CommandShortcut(const Shortcut& sc, unsigned long id, QObject* parent = nullptr);
    CommandShortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint32 key, unsigned long id, QObject* parent = nullptr);
    unsigned long getID() const { return _id; }
    void setID(unsigned long id) { _id = id; }
    QString getCategory() const { return _category; }
    QString getShortcutName() const { return _shortcutName; }
private:
    unsigned long _id = 0;
    QString _category;
    QString _shortcutName;
};

class ScintillaKeyMap : public Shortcut {
    Q_OBJECT
public:
    ScintillaKeyMap(const Shortcut& sc, unsigned long scintillaKeyID, unsigned long menuCmdID, QObject* parent = nullptr);
    unsigned long getScintillaKeyID() const { return _scintillaKeyID; }
    int getMenuCmdID() const { return _menuCmdID; }
    bool isEnabled() const override;
    size_t getSize() const { return static_cast<size_t>(_keyCombos.size()); }
    QString toString() const override;
    void addKeyCombo(KeyCombo combo);
    void clearDups();
    friend inline bool operator==(const ScintillaKeyMap& a, const ScintillaKeyMap& b) {
        if (a._keyCombos.size() != b._keyCombos.size()) return false;
        for (size_t i = 0; i < a._keyCombos.size(); ++i)
            if (a._keyCombos[i]._isCtrl != b._keyCombos[i]._isCtrl || a._keyCombos[i]._isAlt != b._keyCombos[i]._isAlt || a._keyCombos[i]._isShift != b._keyCombos[i]._isShift || a._keyCombos[i]._key != b._keyCombos[i]._key) return false;
        return true;
    }
    friend inline bool operator!=(const ScintillaKeyMap& a, const ScintillaKeyMap& b) { return !(a == b); }
private:
    unsigned long _scintillaKeyID = 0;
    int _menuCmdID = 0;
    QVector<KeyCombo> _keyCombos;
};

struct RecordedMacroStep {
    enum MacroTypeIndex { mtUseLParameter, mtUseSParameter, mtMenuCommand, mtSavedSnR };
    int _message = 0;
    uintptr_t _wParameter = 0;
    uintptr_t _lParameter = 0;
    QString _sParameter;
    MacroTypeIndex _macroType = mtMenuCommand;
    RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam);
    explicit RecordedMacroStep(int iCommandID);
    RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam, const QString& sParam, int type);
    RecordedMacroStep(int iMessage, uintptr_t wParam, uintptr_t lParam, const char* sParam, int type);
    bool isScintillaMacro() const { return _macroType <= mtMenuCommand; }
    bool isMacroable() const { return true; }
    void playBack();
};

using Macro = QVector<RecordedMacroStep>;

class MacroShortcut : public CommandShortcut {
    Q_OBJECT
public:
    MacroShortcut(const Shortcut& sc, const Macro& macro, int id, QObject* parent = nullptr);
    Macro& getMacro() { return _macro; }
private:
    Macro _macro;
};

class UserCommand : public CommandShortcut {
    Q_OBJECT
public:
    UserCommand(const Shortcut& sc, const QString& cmd, int id, QObject* parent = nullptr);
    QString getCmd() const { return _cmd; }
private:
    QString _cmd;
};

class PluginCmdShortcut : public CommandShortcut {
    Q_OBJECT
public:
    PluginCmdShortcut(const Shortcut& sc, int id, const QString& moduleName, int internalID, QObject* parent = nullptr);
    bool isValid() const override;
    QString getModuleName() const { return _moduleName; }
    int getInternalID() const { return _internalID; }
    unsigned long getID() const { return _id; }
private:
    unsigned long _id = 0;
    QString _moduleName;
    int _internalID = -1;
};

class Accelerator : public QObject {
    Q_OBJECT
public:
    Accelerator(QObject* parent = nullptr);
    ~Accelerator();
    void init(QMenu* hMenu, QWidget* menuParent);
    void updateShortcuts();
    void updateFullMenu();
    QList<QKeySequence> getAccTable() const { return _accelerators.keys(); }
private:
    QMenu* _hAccelMenu = nullptr;
    QWidget* _hMenuParent = nullptr;
    QMap<QKeySequence, CommandShortcut*> _accelerators;
    void updateMenuItemByCommand(const CommandShortcut& csc);
};

class ScintillaAccelerator : public QObject {
    Q_OBJECT
public:
    ScintillaAccelerator(QObject* parent = nullptr);
    void init(QVector<QWidget*> vScintillas, QMenu* hMenu, QWidget* menuParent);
    void updateKeys();
private:
    QMenu* _hAccelMenu = nullptr;
    QWidget* _hMenuParent = nullptr;
    QVector<QWidget*> _vScintillas;
};

void getKeyStrFromVal(quint8 keyVal, QString& str);
void getNameStrFromCmd(unsigned long cmd, QString& str);
