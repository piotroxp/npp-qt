/****************************************************************************
** Meta object code from reading C++ file 'SplitterContainer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/SplitterContainer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SplitterContainer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17SplitterContainerE_t {};
} // unnamed namespace

template <> constexpr inline auto SplitterContainer::qt_create_metaobjectdata<qt_meta_tag_ZN17SplitterContainerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SplitterContainer",
        "splitterMoved",
        "",
        "newRatio",
        "rotate",
        "onSplitterMoved"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'splitterMoved'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Slot 'rotate'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSplitterMoved'
        QtMocHelpers::SlotData<void(double)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SplitterContainer, qt_meta_tag_ZN17SplitterContainerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SplitterContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<WindowBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SplitterContainerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SplitterContainerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17SplitterContainerE_t>.metaTypes,
    nullptr
} };

void SplitterContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SplitterContainer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->splitterMoved((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->rotate(); break;
        case 2: _t->onSplitterMoved((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SplitterContainer::*)(double )>(_a, &SplitterContainer::splitterMoved, 0))
            return;
    }
}

const QMetaObject *SplitterContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SplitterContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SplitterContainerE_t>.strings))
        return static_cast<void*>(this);
    return WindowBase::qt_metacast(_clname);
}

int SplitterContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WindowBase::qt_metacall(_c, _id, _a);
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
void SplitterContainer::splitterMoved(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
