/****************************************************************************
** Meta object code from reading C++ file 'NppBigSwitch.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/NppBigSwitch.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NppBigSwitch.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12NppBigSwitchE_t {};
} // unnamed namespace

template <> constexpr inline auto NppBigSwitch::qt_create_metaobjectdata<qt_meta_tag_ZN12NppBigSwitchE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'nppEventReceived'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'commandDispatched'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'dpiChanged'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'taskbarButtonCreated'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NppBigSwitch, qt_meta_tag_ZN12NppBigSwitchE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NppBigSwitch::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NppBigSwitchE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NppBigSwitchE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12NppBigSwitchE_t>.metaTypes,
    nullptr
} };

void NppBigSwitch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NppBigSwitch *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->nppEventReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->commandDispatched((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->dpiChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->taskbarButtonCreated(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NppBigSwitch::*)(int )>(_a, &NppBigSwitch::nppEventReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppBigSwitch::*)(int )>(_a, &NppBigSwitch::commandDispatched, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppBigSwitch::*)(int )>(_a, &NppBigSwitch::dpiChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppBigSwitch::*)()>(_a, &NppBigSwitch::taskbarButtonCreated, 3))
            return;
    }
}

const QMetaObject *NppBigSwitch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppBigSwitch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NppBigSwitchE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NppBigSwitch::nppEventReceived(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NppBigSwitch::commandDispatched(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void NppBigSwitch::dpiChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void NppBigSwitch::taskbarButtonCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
