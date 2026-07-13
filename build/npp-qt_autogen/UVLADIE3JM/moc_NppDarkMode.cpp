/****************************************************************************
** Meta object code from reading C++ file 'NppDarkMode.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/NppDarkMode.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NppDarkMode.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_NppDarkMode__NppDarkMode_t {
    uint offsetsAndSizes[16];
    char stringdata0[25];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[14];
    char stringdata5[19];
    char stringdata6[5];
    char stringdata7[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NppDarkMode__NppDarkMode_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NppDarkMode__NppDarkMode_t qt_meta_stringdata_NppDarkMode__NppDarkMode = {
    {
        QT_MOC_LITERAL(0, 24),  // "NppDarkMode::NppDarkMode"
        QT_MOC_LITERAL(25, 15),  // "darkModeChanged"
        QT_MOC_LITERAL(41, 0),  // ""
        QT_MOC_LITERAL(42, 7),  // "enabled"
        QT_MOC_LITERAL(50, 13),  // "colorsChanged"
        QT_MOC_LITERAL(64, 18),  // "accentColorChanged"
        QT_MOC_LITERAL(83, 4),  // "QRgb"
        QT_MOC_LITERAL(88, 5)   // "color"
    },
    "NppDarkMode::NppDarkMode",
    "darkModeChanged",
    "",
    "enabled",
    "colorsChanged",
    "accentColorChanged",
    "QRgb",
    "color"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NppDarkMode__NppDarkMode[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       1,   39, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x06,    2 /* Public */,
       4,    0,   35,    2, 0x06,    4 /* Public */,
       5,    1,   36,    2, 0x06,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,

 // properties: name, type, flags
       3, QMetaType::Bool, 0x00015001, uint(0), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject NppDarkMode::NppDarkMode::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NppDarkMode__NppDarkMode.offsetsAndSizes,
    qt_meta_data_NppDarkMode__NppDarkMode,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NppDarkMode__NppDarkMode_t,
        // property 'enabled'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NppDarkMode, std::true_type>,
        // method 'darkModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'colorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'accentColorChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QRgb, std::false_type>
    >,
    nullptr
} };

void NppDarkMode::NppDarkMode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NppDarkMode *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->darkModeChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->colorsChanged(); break;
        case 2: _t->accentColorChanged((*reinterpret_cast< std::add_pointer_t<QRgb>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NppDarkMode::*)(bool );
            if (_t _q_method = &NppDarkMode::darkModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NppDarkMode::*)();
            if (_t _q_method = &NppDarkMode::colorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NppDarkMode::*)(QRgb );
            if (_t _q_method = &NppDarkMode::accentColorChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<NppDarkMode *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isEnabled(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *NppDarkMode::NppDarkMode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppDarkMode::NppDarkMode::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NppDarkMode__NppDarkMode.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NppDarkMode::NppDarkMode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void NppDarkMode::NppDarkMode::darkModeChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NppDarkMode::NppDarkMode::colorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NppDarkMode::NppDarkMode::accentColorChanged(QRgb _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
namespace {
struct qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle_t {
    uint offsetsAndSizes[2];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle_t qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle = {
    {
        QT_MOC_LITERAL(0, 36)   // "NppDarkMode::DarkScrollBarPro..."
    },
    "NppDarkMode::DarkScrollBarProxyStyle"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NppDarkMode__DarkScrollBarProxyStyle[] = {

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

Q_CONSTINIT const QMetaObject NppDarkMode::DarkScrollBarProxyStyle::staticMetaObject = { {
    QMetaObject::SuperData::link<QProxyStyle::staticMetaObject>(),
    qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle.offsetsAndSizes,
    qt_meta_data_NppDarkMode__DarkScrollBarProxyStyle,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DarkScrollBarProxyStyle, std::true_type>
    >,
    nullptr
} };

void NppDarkMode::DarkScrollBarProxyStyle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *NppDarkMode::DarkScrollBarProxyStyle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppDarkMode::DarkScrollBarProxyStyle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NppDarkMode__DarkScrollBarProxyStyle.stringdata0))
        return static_cast<void*>(this);
    return QProxyStyle::qt_metacast(_clname);
}

int NppDarkMode::DarkScrollBarProxyStyle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProxyStyle::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
