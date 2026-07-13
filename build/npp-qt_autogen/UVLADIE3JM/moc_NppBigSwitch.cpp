/****************************************************************************
** Meta object code from reading C++ file 'NppBigSwitch.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/NppBigSwitch.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NppBigSwitch.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_NppBigSwitch_t {
    uint offsetsAndSizes[18];
    char stringdata0[13];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[18];
    char stringdata5[10];
    char stringdata6[11];
    char stringdata7[7];
    char stringdata8[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NppBigSwitch_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NppBigSwitch_t qt_meta_stringdata_NppBigSwitch = {
    {
        QT_MOC_LITERAL(0, 12),  // "NppBigSwitch"
        QT_MOC_LITERAL(13, 16),  // "nppEventReceived"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 7),  // "eventId"
        QT_MOC_LITERAL(39, 17),  // "commandDispatched"
        QT_MOC_LITERAL(57, 9),  // "commandId"
        QT_MOC_LITERAL(67, 10),  // "dpiChanged"
        QT_MOC_LITERAL(78, 6),  // "newDpi"
        QT_MOC_LITERAL(85, 20)   // "taskbarButtonCreated"
    },
    "NppBigSwitch",
    "nppEventReceived",
    "",
    "eventId",
    "commandDispatched",
    "commandId",
    "dpiChanged",
    "newDpi",
    "taskbarButtonCreated"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NppBigSwitch[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       4,    1,   41,    2, 0x06,    3 /* Public */,
       6,    1,   44,    2, 0x06,    5 /* Public */,
       8,    0,   47,    2, 0x06,    7 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject NppBigSwitch::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NppBigSwitch.offsetsAndSizes,
    qt_meta_data_NppBigSwitch,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NppBigSwitch_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NppBigSwitch, std::true_type>,
        // method 'nppEventReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'commandDispatched'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'dpiChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'taskbarButtonCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void NppBigSwitch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NppBigSwitch *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->nppEventReceived((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->commandDispatched((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->dpiChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->taskbarButtonCreated(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NppBigSwitch::*)(int );
            if (_t _q_method = &NppBigSwitch::nppEventReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NppBigSwitch::*)(int );
            if (_t _q_method = &NppBigSwitch::commandDispatched; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NppBigSwitch::*)(int );
            if (_t _q_method = &NppBigSwitch::dpiChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NppBigSwitch::*)();
            if (_t _q_method = &NppBigSwitch::taskbarButtonCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *NppBigSwitch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppBigSwitch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NppBigSwitch.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NppBigSwitch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NppBigSwitch::nppEventReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NppBigSwitch::commandDispatched(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NppBigSwitch::dpiChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NppBigSwitch::taskbarButtonCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
