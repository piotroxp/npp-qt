/****************************************************************************
** Meta object code from reading C++ file 'DocTabView.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/DocTabView.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocTabView.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10DocTabViewE_t {};
} // unnamed namespace

template <> constexpr inline auto DocTabView::qt_create_metaobjectdata<qt_meta_tag_ZN10DocTabViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DocTabView",
        "bufferActivatedSignal",
        "",
        "BufferID",
        "buffer",
        "bufferClosedSignal"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'bufferActivatedSignal'
        QtMocHelpers::SignalData<void(BufferID)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'bufferClosedSignal'
        QtMocHelpers::SignalData<void(BufferID)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DocTabView, qt_meta_tag_ZN10DocTabViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DocTabView::staticMetaObject = { {
    QMetaObject::SuperData::link<TabBarPlus::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DocTabViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DocTabViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10DocTabViewE_t>.metaTypes,
    nullptr
} };

void DocTabView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DocTabView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->bufferActivatedSignal((*reinterpret_cast<std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 1: _t->bufferClosedSignal((*reinterpret_cast<std::add_pointer_t<BufferID>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DocTabView::*)(BufferID )>(_a, &DocTabView::bufferActivatedSignal, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocTabView::*)(BufferID )>(_a, &DocTabView::bufferClosedSignal, 1))
            return;
    }
}

const QMetaObject *DocTabView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocTabView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DocTabViewE_t>.strings))
        return static_cast<void*>(this);
    return TabBarPlus::qt_metacast(_clname);
}

int DocTabView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TabBarPlus::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DocTabView::bufferActivatedSignal(BufferID _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DocTabView::bufferClosedSignal(BufferID _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
