// shortcut.cpp — Qt6 translation of ACCEL → QShortcut
#include "shortcut.h"
#include <QApplication>
#include <QAction>
#include <QMenu>

// Key name map
struct KeyIDName { const char* name; quint8 id; };

static const KeyIDName namedKeyArray[] = {
    {"None", 0},
    {"Backspace", 0x08}, {"Tab", 0x09}, {"Enter", 0x0D}, {"Esc", 0x1B}, {"Space", 0x20},
    {"Page Up", 0x21}, {"Page Down", 0x22}, {"End", 0x23}, {"Home", 0x24},
    {"Left", 0x25}, {"Up", 0x26}, {"Right", 0x27}, {"Down", 0x28},
    {"Ins", 0x2D}, {"Del", 0x2E},
    {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34},
    {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39},
    {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45},
    {"F", 0x46}, {"G", 0x47}, {"H", 0x48}, {"I", 0x49}, {"J", 0x4A},
    {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F},
    {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
    {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},
    {"F1", 0x70}, {"F2", 0x71}, {"F3", 0x72}, {"F4", 0x73}, {"F5", 0x74},
    {"F6", 0x75}, {"F7", 0x76}, {"F8", 0x77}, {"F9", 0x78}, {"F10", 0x79},
    {"F11", 0x7A}, {"F12", 0x7B},
};

static const int nbKeys = sizeof(namedKeyArray) / sizeof(KeyIDName);

quint32 keyTranslate(quint32 keyIn)
{
    // Map virtual keys to Scintilla keys
    switch (keyIn)
    {
        case 0x25: return 0x2000; // SCK_LEFT
        case 0x27: return 0x2001; // SCK_RIGHT
        case 0x26: return 0x2002; // SCK_UP
        case 0x28: return 0x2003; // SCK_DOWN
        case 0x24: return 0x2004; // SCK_HOME
        case 0x23: return 0x2005; // SCK_END
        case 0x21: return 0x2006; // SCK_PRIOR
        case 0x22: return 0x2007; // SCK_NEXT
        case 0x2E: return 0x2008; // SCK_DELETE
        case 0x2D: return 0x2009; // SCK_INSERT
        case 0x1B: return 0x200A; // SCK_ESCAPE
        case 0x08: return 0x200B; // SCK_BACK
        case 0x09: return 0x200C; // SCK_TAB
        case 0x0D: return 0x200D; // SCK_RETURN
        default: return keyIn;
    }
}

void getKeyStrFromVal(quint8 keyVal, QString& str)
{
    for (int i = 0; i < nbKeys; ++i)
    {
        if (namedKeyArray[i].id == keyVal)
        {
            str = QString::fromLatin1(namedKeyArray[i].name);
            return;
        }
    }
    str = "Unlisted";
}

void getNameStrFromCmd(int /*cmd*/, QString& str)
{
    // Would query menu system for command name
    str = "Unknown";
}

// ─── Shortcut ───────────────────────────────────────────────────────

Shortcut::Shortcut(QObject* parent)
    : QObject(parent)
{
}

Shortcut::Shortcut(const QString& name, bool isCtrl, bool isAlt, bool isShift, quint8 key, QObject* parent)
    : QObject(parent), _name(name), _menuName(name)
{
    _keyCombo._isCtrl = isCtrl;
    _keyCombo._isAlt = isAlt;
    _keyCombo._isShift = isShift;
    _keyCombo._key = key;
}

bool Shortcut::isValid() const
{
    if (_keyCombo._key == 0)
        return true; // Disabled is always valid

    // Keys A-Z, 0-9, Space, etc. need modifier
    if (((_keyCombo._key >= 'A' && _keyCombo._key <= 'Z') ||
        (_keyCombo._key >= '0' && _keyCombo._key <= '9') ||
        _keyCombo._key == 0x20 || _keyCombo._key == 0x0D)
    {
        return _keyCombo._isCtrl || _keyCombo._isAlt;
    }
    return true;
}

quint8 Shortcut::getAcceleratorModifiers() const
{
    quint8 mods = FVIRTKEY;
    if (_keyCombo._isCtrl) mods |= FCONTROL;
    if (_keyCombo._isAlt) mods |= FALT;
    if (_keyCombo._isShift) mods |= FSHIFT;
    return mods;
}

Qt::KeyboardModifiers Shortcut::qtModifiers() const
{
    Qt::KeyboardModifiers mods;
    if (_keyCombo._isCtrl) mods |= Qt::ControlModifier;
    if (_keyCombo._isAlt) mods |= Qt::AltModifier;
    if (_keyCombo._isShift) mods |= Qt::ShiftModifier;
    return mods;
}

QString Shortcut::toString() const
{
    if (!isEnabled())
        return QString();

    QString sc;
    if (_keyCombo._isCtrl) sc += "Ctrl+";
    if (_keyCombo._isAlt) sc += "Alt+";
    if (_keyCombo._isShift) sc += "Shift+";

    QString keyStr;
    getKeyStrFromVal(_keyCombo._key, keyStr);
    sc += keyStr;
    return sc;
}

QString Shortcut::toMenuItemString() const
{
    if (isEnabled())
        return _menuName + "\t" + toString();
    return _menuName;
}

void Shortcut::setName(const QString& menuName, const QString& shortcutName)
{
    _menuName = menuName;
    _name = shortcutName.isEmpty() ? menuName : shortcutName;
}

void Shortcut::clear()
{
    _keyCombo._isCtrl = false;
    _keyCombo._isAlt = false;
    _keyCombo._isShift = false;
    _keyCombo._key = 0;
}

Shortcut& Shortcut::operator=(const Shortcut& other)
{
    if (_name.isEmpty() || !other._name.isEmpty())
        setName(other._menuName, other._name);
    _keyCombo = other._keyCombo;
    return *this;
}

bool operator==(const Shortcut& a, const Shortcut& b)
{
    return a._menuName == b._menuName &&
           a._keyCombo._isCtrl == b._keyCombo._isCtrl &&
           a._keyCombo._isAlt == b._keyCombo._isAlt &&
           a._keyCombo._isShift == b._keyCombo._isShift &&
           a._keyCombo._key == b._keyCombo._key;
}

bool operator!=(const Shortcut& a, const Shortcut& b)
{
    return !(a == b);
}

// ─── CommandShortcut ──────────────────────────────────────────────

CommandShortcut::CommandShortcut(const Shortcut& sc, int id, QObject* parent)
    : Shortcut(sc), _id(id)
{
    Q_UNUSED(parent);
}

// ─── Accelerator ───────────────────────────────────────────────────

Accelerator::Accelerator(QObject* parent)
    : QObject(parent)
{
}

Accelerator::~Accelerator()
{
    qDeleteAll(_shortcuts);
}

void Accelerator::init()
{
    // Called during startup to connect shortcuts
}

void Accelerator::updateShortcuts()
{
    // Rebuild shortcuts from _shortcutList
    qDeleteAll(_shortcuts);
    _shortcuts.clear();

    for (int i = 0; i < _shortcutList.size(); ++i)
    {
        const Shortcut& sc = _shortcutList[i];
        if (sc.isEnabled())
        {
            QShortcut* shortcut = new QShortcut(sc.qtModifiers() | Qt::Key(sc._keyCombo._key), qApp->activeWindow());
            shortcut->setContext(Qt::ApplicationShortcut);
            connect(shortcut, &QShortcut::activated, this, &Accelerator::activated);
            _shortcuts[sc.getID()] = shortcut;
        }
    }
}

void Accelerator::updateFullMenu()
{
    // Update menu items with shortcut text
}

QKeySequence Accelerator::getShortcut(int cmdID) const
{
    if (_shortcuts.contains(cmdID))
    {
        return _shortcuts[cmdID]->key();
    }
    return QKeySequence();
}

// ─── ScintillaKeyMap ─────────────────────────────────────────────

ScintillaKeyMap::ScintillaKeyMap(const Shortcut& sc, quint32 scintillaKeyID, int menuCmdID, QObject* parent)
    : Shortcut(sc), _scintillaKeyID(scintillaKeyID), _menuCmdID(menuCmdID)
{
    _keyCombos.append(sc._keyCombo);
}

bool ScintillaKeyMap::isEnabled() const
{
    return !_keyCombos.isEmpty() && _keyCombos[0]._key != 0;
}

void ScintillaKeyMap::addKeyCombo(KeyCombo combo)
{
    if (combo._key == 0)
        return;

    for (const auto& kc : _keyCombos)
    {
        if (combo._key == kc._key &&
            combo._isCtrl == kc._isCtrl &&
            combo._isAlt == kc._isAlt &&
            combo._isShift == kc._isShift)
            return; // Already exists
    }
    _keyCombos.append(combo);
}

void ScintillaKeyMap::clearDups()
{
    if (_keyCombos.size() > 1)
        _keyCombos = _keyCombos.mid(0, 1);
}

QString ScintillaKeyMap::toString() const
{
    QStringList parts;
    for (const auto& kc : _keyCombos)
    {
        QString sc;
        if (kc._isCtrl) sc += "Ctrl+";
        if (kc._isAlt) sc += "Alt+";
        if (kc._isShift) sc += "Shift+";
        QString keyStr;
        getKeyStrFromVal(kc._key, keyStr);
        sc += keyStr;
        parts.append(sc);
    }
    return parts.join(" / ");
}
