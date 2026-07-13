/****************************************************************************
** Meta object code from reading C++ file 'shortcut.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/MISC/Common/shortcut.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shortcut.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_Shortcut_t {
    uint offsetsAndSizes[6];
    char stringdata0[9];
    char stringdata1[10];
    char stringdata2[1];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Shortcut_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Shortcut_t qt_meta_stringdata_Shortcut = {
    {
        QT_MOC_LITERAL(0, 8),  // "Shortcut"
        QT_MOC_LITERAL(9, 9),  // "triggered"
        QT_MOC_LITERAL(19, 0)   // ""
    },
    "Shortcut",
    "triggered",
    ""
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Shortcut[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Shortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Shortcut.offsetsAndSizes,
    qt_meta_data_Shortcut,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Shortcut_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Shortcut, std::true_type>,
        // method 'triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Shortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Shortcut *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Shortcut::*)();
            if (_t _q_method = &Shortcut::triggered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *Shortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Shortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Shortcut.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Shortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Shortcut::triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
namespace {
struct qt_meta_stringdata_CommandShortcut_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CommandShortcut_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CommandShortcut_t qt_meta_stringdata_CommandShortcut = {
    {
        QT_MOC_LITERAL(0, 15)   // "CommandShortcut"
    },
    "CommandShortcut"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CommandShortcut[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject CommandShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<Shortcut::staticMetaObject>(),
    qt_meta_stringdata_CommandShortcut.offsetsAndSizes,
    qt_meta_data_CommandShortcut,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CommandShortcut_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CommandShortcut, std::true_type>
    >,
    nullptr
} };

void CommandShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CommandShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CommandShortcut.stringdata0))
        return static_cast<void*>(this);
    return Shortcut::qt_metacast(_clname);
}

int CommandShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Shortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ScintillaKeyMap_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ScintillaKeyMap_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ScintillaKeyMap_t qt_meta_stringdata_ScintillaKeyMap = {
    {
        QT_MOC_LITERAL(0, 15)   // "ScintillaKeyMap"
    },
    "ScintillaKeyMap"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ScintillaKeyMap[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject ScintillaKeyMap::staticMetaObject = { {
    QMetaObject::SuperData::link<Shortcut::staticMetaObject>(),
    qt_meta_stringdata_ScintillaKeyMap.offsetsAndSizes,
    qt_meta_data_ScintillaKeyMap,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ScintillaKeyMap_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScintillaKeyMap, std::true_type>
    >,
    nullptr
} };

void ScintillaKeyMap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ScintillaKeyMap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaKeyMap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScintillaKeyMap.stringdata0))
        return static_cast<void*>(this);
    return Shortcut::qt_metacast(_clname);
}

int ScintillaKeyMap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Shortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_MacroShortcut_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MacroShortcut_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MacroShortcut_t qt_meta_stringdata_MacroShortcut = {
    {
        QT_MOC_LITERAL(0, 13)   // "MacroShortcut"
    },
    "MacroShortcut"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MacroShortcut[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject MacroShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_meta_stringdata_MacroShortcut.offsetsAndSizes,
    qt_meta_data_MacroShortcut,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MacroShortcut_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MacroShortcut, std::true_type>
    >,
    nullptr
} };

void MacroShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MacroShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MacroShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MacroShortcut.stringdata0))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int MacroShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_UserCommand_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_UserCommand_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_UserCommand_t qt_meta_stringdata_UserCommand = {
    {
        QT_MOC_LITERAL(0, 11)   // "UserCommand"
    },
    "UserCommand"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_UserCommand[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject UserCommand::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_meta_stringdata_UserCommand.offsetsAndSizes,
    qt_meta_data_UserCommand,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_UserCommand_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UserCommand, std::true_type>
    >,
    nullptr
} };

void UserCommand::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *UserCommand::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserCommand::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UserCommand.stringdata0))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int UserCommand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PluginCmdShortcut_t {
    uint offsetsAndSizes[2];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PluginCmdShortcut_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PluginCmdShortcut_t qt_meta_stringdata_PluginCmdShortcut = {
    {
        QT_MOC_LITERAL(0, 17)   // "PluginCmdShortcut"
    },
    "PluginCmdShortcut"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PluginCmdShortcut[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject PluginCmdShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_meta_stringdata_PluginCmdShortcut.offsetsAndSizes,
    qt_meta_data_PluginCmdShortcut,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PluginCmdShortcut_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PluginCmdShortcut, std::true_type>
    >,
    nullptr
} };

void PluginCmdShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PluginCmdShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginCmdShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PluginCmdShortcut.stringdata0))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int PluginCmdShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_Accelerator_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Accelerator_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Accelerator_t qt_meta_stringdata_Accelerator = {
    {
        QT_MOC_LITERAL(0, 11)   // "Accelerator"
    },
    "Accelerator"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Accelerator[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject Accelerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Accelerator.offsetsAndSizes,
    qt_meta_data_Accelerator,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Accelerator_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Accelerator, std::true_type>
    >,
    nullptr
} };

void Accelerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *Accelerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Accelerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Accelerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Accelerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ScintillaAccelerator_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ScintillaAccelerator_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ScintillaAccelerator_t qt_meta_stringdata_ScintillaAccelerator = {
    {
        QT_MOC_LITERAL(0, 20)   // "ScintillaAccelerator"
    },
    "ScintillaAccelerator"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ScintillaAccelerator[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject ScintillaAccelerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ScintillaAccelerator.offsetsAndSizes,
    qt_meta_data_ScintillaAccelerator,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ScintillaAccelerator_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScintillaAccelerator, std::true_type>
    >,
    nullptr
} };

void ScintillaAccelerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ScintillaAccelerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaAccelerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScintillaAccelerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScintillaAccelerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
