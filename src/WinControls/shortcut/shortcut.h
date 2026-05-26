// shortcut.h — Qt6 translation: ACCEL table → QList<QShortcut>
#pragma once

#include <QWidget>
#include <QKeySequence>
#include <QShortcut>
#include <QMap>
#include <QVector>
#include <QString>

// Key modifiers mapping
#define FVIRTKEY    0x01
#define FCONTROL    0x02
#define FALT        0x04
#define FSHIFT      0x08

// KeyCombo — accelerator key combination
struct KeyCombo {
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    quint8 _key = 0;
};

// Convert Qt key to Scintilla key
quint32 keyTranslate(quint32 keyIn);

// Get key string from virtual key
void getKeyStrFromVal(quint8 keyVal, QString& str);

// Get command name from ID
void getNameStrFromCmd(int cmd, QString& str);

// ─── Shortcut ──────────────────────────────────────────────────────────

class Shortcut : public QObject
{
    Q_OBJECT

public:
    Shortcut(QObject* parent = nullptr);
    Shortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint8 key, QObject* parent = nullptr);

    // Enable/disable
    bool isEnabled() const { return _keyCombo._key != 0; }
    bool isValid() const;

    // Accessors
    const KeyCombo& getKeyCombo() const { return _keyCombo; }
    const QString& getName() const { return _name; }
    const QString& getMenuName() const { return _menuName; }

    // Modifiers
    quint8 getAcceleratorModifiers() const;
    Qt::KeyboardModifiers qtModifiers() const;

    // String representations
    QString toString() const;           // "Ctrl+Alt+S"
    QString toMenuItemString() const;     // "Menu Name\tCtrl+S"

    // Set name
    void setName(const QString& menuName, const QString& shortcutName = QString());

    // Clear shortcut
    void clear();

    // Assign
    Shortcut& operator=(const Shortcut& other);

    // Equality
    friend bool operator==(const Shortcut& a, const Shortcut& b);
    friend bool operator!=(const Shortcut& a, const Shortcut& b);

signals:
    void activated();

protected:
    KeyCombo _keyCombo;
    QString _name;
    QString _menuName;
    bool _canModifyName = false;
};

// ─── CommandShortcut ────────────────────────────────────────────────

class CommandShortcut : public Shortcut
{
    Q_OBJECT

public:
    CommandShortcut(const Shortcut& sc, int id, QObject* parent = nullptr);

    int getID() const { return _id; }
    void setID(int id) { _id = id; }

private:
    int _id = 0;
};

// ─── Accelerator ───────────────────────────────────────────────────

class Accelerator : public QObject
{
    Q_OBJECT

public:
    Accelerator(QObject* parent = nullptr);
    ~Accelerator() override;

    void init();
    void updateShortcuts();
    void updateFullMenu();

    // Get Qt shortcut for command ID
    QKeySequence getShortcut(int cmdID) const;

private:
    QMap<int, QShortcut*> _shortcuts; // cmdID → shortcut
    QList<Shortcut> _shortcutList;
};

// ─── ScintillaKeyMap ─────────────────────────────────────────────

class ScintillaKeyMap : public Shortcut
{
    Q_OBJECT

public:
    ScintillaKeyMap(const Shortcut& sc, quint32 scintillaKeyID, int menuCmdID, QObject* parent = nullptr);

    quint32 getScintillaKeyID() const { return _scintillaKeyID; }
    int getMenuCmdID() const { return _menuCmdID; }
    QString toString() const override;

    bool isEnabled() const override;
    size_t getSize() const { return _keyCombos.size(); }
    void addKeyCombo(KeyCombo combo);
    void clearDups();

private:
    quint32 _scintillaKeyID = 0;
    int _menuCmdID = 0;
    QVector<KeyCombo> _keyCombos;
};
