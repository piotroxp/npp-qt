/****************************************************************************
** Meta object code from reading C++ file 'NppDarkMode.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/NppDarkMode.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NppDarkMode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t {};
} // unnamed namespace

template <> constexpr inline auto NppDarkMode::NppDarkMode::qt_create_metaobjectdata<qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NppDarkMode::NppDarkMode",
        "darkModeChanged",
        "",
        "enabled",
        "colorsChanged",
        "accentColorChanged",
        "QRgb",
        "color"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'darkModeChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'colorsChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'accentColorChanged'
        QtMocHelpers::SignalData<void(QRgb)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'enabled'
        QtMocHelpers::PropertyData<bool>(3, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NppDarkMode, qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NppDarkMode::NppDarkMode::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>.metaTypes,
    nullptr
} };

void NppDarkMode::NppDarkMode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NppDarkMode *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->darkModeChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->colorsChanged(); break;
        case 2: _t->accentColorChanged((*reinterpret_cast<std::add_pointer_t<QRgb>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NppDarkMode::*)(bool )>(_a, &NppDarkMode::darkModeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppDarkMode::*)()>(_a, &NppDarkMode::colorsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppDarkMode::*)(QRgb )>(_a, &NppDarkMode::accentColorChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isEnabled(); break;
        default: break;
        }
    }
}

const QMetaObject *NppDarkMode::NppDarkMode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppDarkMode::NppDarkMode::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode11NppDarkModeE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
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
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NppDarkMode::NppDarkMode::colorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NppDarkMode::NppDarkMode::accentColorChanged(QRgb _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t {};
} // unnamed namespace

template <> constexpr inline auto NppDarkMode::DarkScrollBarProxyStyle::qt_create_metaobjectdata<qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NppDarkMode::DarkScrollBarProxyStyle"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DarkScrollBarProxyStyle, qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NppDarkMode::DarkScrollBarProxyStyle::staticMetaObject = { {
    QMetaObject::SuperData::link<QProxyStyle::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>.metaTypes,
    nullptr
} };

void NppDarkMode::DarkScrollBarProxyStyle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DarkScrollBarProxyStyle *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *NppDarkMode::DarkScrollBarProxyStyle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppDarkMode::DarkScrollBarProxyStyle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppDarkMode23DarkScrollBarProxyStyleE_t>.strings))
        return static_cast<void*>(this);
    return QProxyStyle::qt_metacast(_clname);
}

int NppDarkMode::DarkScrollBarProxyStyle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProxyStyle::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
