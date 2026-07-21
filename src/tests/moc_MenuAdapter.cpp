/****************************************************************************
** Meta object code from reading C++ file 'MenuAdapter.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../common/MenuAdapter.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MenuAdapter.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MenuAdapter_t {
    uint offsetsAndSizes[16];
    char stringdata0[12];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[14];
    char stringdata5[8];
    char stringdata6[10];
    char stringdata7[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MenuAdapter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MenuAdapter_t qt_meta_stringdata_MenuAdapter = {
    {
        QT_MOC_LITERAL(0, 11),  // "MenuAdapter"
        QT_MOC_LITERAL(12, 7),  // "command"
        QT_MOC_LITERAL(20, 0),  // ""
        QT_MOC_LITERAL(21, 5),  // "cmdID"
        QT_MOC_LITERAL(27, 13),  // "commandString"
        QT_MOC_LITERAL(41, 7),  // "trigger"
        QT_MOC_LITERAL(49, 9),  // "qtCommand"
        QT_MOC_LITERAL(59, 17)   // "onActionTriggered"
    },
    "MenuAdapter",
    "command",
    "",
    "cmdID",
    "commandString",
    "trigger",
    "qtCommand",
    "onActionTriggered"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MenuAdapter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       4,    1,   47,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   50,    2, 0x0a,    5 /* Public */,
       5,    1,   53,    2, 0x0a,    7 /* Public */,
       7,    0,   56,    2, 0x08,    9 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    1,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MenuAdapter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MenuAdapter.offsetsAndSizes,
    qt_meta_data_MenuAdapter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MenuAdapter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MenuAdapter, std::true_type>,
        // method 'command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'commandString'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'trigger'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'trigger'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onActionTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MenuAdapter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MenuAdapter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->command((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->commandString((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->trigger((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->trigger((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onActionTriggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MenuAdapter::*)(int );
            if (_t _q_method = &MenuAdapter::command; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MenuAdapter::*)(const QString & );
            if (_t _q_method = &MenuAdapter::commandString; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *MenuAdapter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MenuAdapter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MenuAdapter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MenuAdapter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void MenuAdapter::command(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MenuAdapter::commandString(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
namespace {
struct qt_meta_stringdata_ContextMenuAdapter_t {
    uint offsetsAndSizes[12];
    char stringdata0[19];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[16];
    char stringdata5[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ContextMenuAdapter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ContextMenuAdapter_t qt_meta_stringdata_ContextMenuAdapter = {
    {
        QT_MOC_LITERAL(0, 18),  // "ContextMenuAdapter"
        QT_MOC_LITERAL(19, 9),  // "triggered"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 5),  // "cmdID"
        QT_MOC_LITERAL(36, 15),  // "triggeredString"
        QT_MOC_LITERAL(52, 7)   // "command"
    },
    "ContextMenuAdapter",
    "triggered",
    "",
    "cmdID",
    "triggeredString",
    "command"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ContextMenuAdapter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    1 /* Public */,
       4,    1,   29,    2, 0x06,    3 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,

       0        // eod
};

Q_CONSTINIT const QMetaObject ContextMenuAdapter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ContextMenuAdapter.offsetsAndSizes,
    qt_meta_data_ContextMenuAdapter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ContextMenuAdapter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ContextMenuAdapter, std::true_type>,
        // method 'triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'triggeredString'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ContextMenuAdapter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ContextMenuAdapter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->triggered((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->triggeredString((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ContextMenuAdapter::*)(int );
            if (_t _q_method = &ContextMenuAdapter::triggered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ContextMenuAdapter::*)(const QString & );
            if (_t _q_method = &ContextMenuAdapter::triggeredString; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *ContextMenuAdapter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ContextMenuAdapter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ContextMenuAdapter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ContextMenuAdapter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ContextMenuAdapter::triggered(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ContextMenuAdapter::triggeredString(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
