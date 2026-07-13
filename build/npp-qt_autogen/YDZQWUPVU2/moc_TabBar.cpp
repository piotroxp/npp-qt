/****************************************************************************
** Meta object code from reading C++ file 'TabBar.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/TabBar.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TabBar.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_TabBar_t {
    uint offsetsAndSizes[16];
    char stringdata0[7];
    char stringdata1[11];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[10];
    char stringdata5[19];
    char stringdata6[11];
    char stringdata7[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_TabBar_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_TabBar_t qt_meta_stringdata_TabBar = {
    {
        QT_MOC_LITERAL(0, 6),  // "TabBar"
        QT_MOC_LITERAL(7, 10),  // "tabDropped"
        QT_MOC_LITERAL(18, 0),  // ""
        QT_MOC_LITERAL(19, 5),  // "index"
        QT_MOC_LITERAL(25, 9),  // "tabDelete"
        QT_MOC_LITERAL(35, 18),  // "customStyleChanged"
        QT_MOC_LITERAL(54, 10),  // "isVertical"
        QT_MOC_LITERAL(65, 11)   // "isMultiLine"
    },
    "TabBar",
    "tabDropped",
    "",
    "index",
    "tabDelete",
    "customStyleChanged",
    "isVertical",
    "isMultiLine"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_TabBar[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x06,    1 /* Public */,
       4,    1,   35,    2, 0x06,    3 /* Public */,
       5,    2,   38,    2, 0x06,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    6,    7,

       0        // eod
};

Q_CONSTINIT const QMetaObject TabBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QTabWidget::staticMetaObject>(),
    qt_meta_stringdata_TabBar.offsetsAndSizes,
    qt_meta_data_TabBar,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_TabBar_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TabBar, std::true_type>,
        // method 'tabDropped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'tabDelete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'customStyleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void TabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabBar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tabDropped((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->tabDelete((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->customStyleChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TabBar::*)(int );
            if (_t _q_method = &TabBar::tabDropped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TabBar::*)(int );
            if (_t _q_method = &TabBar::tabDelete; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TabBar::*)(bool , bool );
            if (_t _q_method = &TabBar::customStyleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *TabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabBar.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int TabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TabBar::tabDropped(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TabBar::tabDelete(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TabBar::customStyleChanged(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
namespace {
struct qt_meta_stringdata_TabBarPlus_t {
    uint offsetsAndSizes[2];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_TabBarPlus_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_TabBarPlus_t qt_meta_stringdata_TabBarPlus = {
    {
        QT_MOC_LITERAL(0, 10)   // "TabBarPlus"
    },
    "TabBarPlus"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_TabBarPlus[] = {

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

Q_CONSTINIT const QMetaObject TabBarPlus::staticMetaObject = { {
    QMetaObject::SuperData::link<TabBar::staticMetaObject>(),
    qt_meta_stringdata_TabBarPlus.offsetsAndSizes,
    qt_meta_data_TabBarPlus,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_TabBarPlus_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TabBarPlus, std::true_type>
    >,
    nullptr
} };

void TabBarPlus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TabBarPlus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabBarPlus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabBarPlus.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IWindow"))
        return static_cast< IWindow*>(this);
    return TabBar::qt_metacast(_clname);
}

int TabBarPlus::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TabBar::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
