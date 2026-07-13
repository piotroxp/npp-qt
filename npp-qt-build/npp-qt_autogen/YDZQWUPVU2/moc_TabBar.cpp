/****************************************************************************
** Meta object code from reading C++ file 'TabBar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/TabBar.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TabBar.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6TabBarE_t {};
} // unnamed namespace

template <> constexpr inline auto TabBar::qt_create_metaobjectdata<qt_meta_tag_ZN6TabBarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TabBar",
        "tabDropped",
        "",
        "index",
        "tabDelete",
        "customStyleChanged",
        "isVertical",
        "isMultiLine"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'tabDropped'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'tabDelete'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'customStyleChanged'
        QtMocHelpers::SignalData<void(bool, bool)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 6 }, { QMetaType::Bool, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TabBar, qt_meta_tag_ZN6TabBarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TabBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QTabWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TabBarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TabBarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6TabBarE_t>.metaTypes,
    nullptr
} };

void TabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TabBar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->tabDropped((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->tabDelete((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->customStyleChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TabBar::*)(int )>(_a, &TabBar::tabDropped, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TabBar::*)(int )>(_a, &TabBar::tabDelete, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TabBar::*)(bool , bool )>(_a, &TabBar::customStyleChanged, 2))
            return;
    }
}

const QMetaObject *TabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TabBarE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TabBar::tabDropped(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void TabBar::tabDelete(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void TabBar::customStyleChanged(bool _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10TabBarPlusE_t {};
} // unnamed namespace

template <> constexpr inline auto TabBarPlus::qt_create_metaobjectdata<qt_meta_tag_ZN10TabBarPlusE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TabBarPlus"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TabBarPlus, qt_meta_tag_ZN10TabBarPlusE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TabBarPlus::staticMetaObject = { {
    QMetaObject::SuperData::link<TabBar::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10TabBarPlusE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10TabBarPlusE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10TabBarPlusE_t>.metaTypes,
    nullptr
} };

void TabBarPlus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TabBarPlus *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *TabBarPlus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabBarPlus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10TabBarPlusE_t>.strings))
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
