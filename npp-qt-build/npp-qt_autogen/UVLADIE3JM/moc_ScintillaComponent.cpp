/****************************************************************************
** Meta object code from reading C++ file 'ScintillaComponent.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/ScintillaComponent.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScintillaComponent.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18ScintillaComponentE_t {};
} // unnamed namespace

template <> constexpr inline auto ScintillaComponent::qt_create_metaobjectdata<qt_meta_tag_ZN18ScintillaComponentE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScintillaComponent",
        "marginClicked",
        "",
        "size_t",
        "position",
        "modifiers",
        "margin",
        "doubleClicked",
        "line",
        "modified",
        "type",
        "length",
        "text",
        "foldLevelNow",
        "foldLevelPrev",
        "updateUi",
        "updated",
        "charAdded",
        "ch",
        "savePointChanged",
        "dirty",
        "modifyAttemptReadOnly",
        "zoomChanged",
        "zoom",
        "needShown",
        "painted",
        "userListSelection",
        "bufferActivated",
        "Buffer*",
        "buffer",
        "uiChanged",
        "textChanged",
        "cursorPositionChanged",
        "index"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'marginClicked'
        QtMocHelpers::SignalData<void(size_t, int, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Int, 5 }, { QMetaType::Int, 6 },
        }}),
        // Signal 'doubleClicked'
        QtMocHelpers::SignalData<void(size_t, size_t)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 8 },
        }}),
        // Signal 'modified'
        QtMocHelpers::SignalData<void(int, size_t, size_t, const QString &, size_t, int, int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 }, { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 11 }, { QMetaType::QString, 12 },
            { 0x80000000 | 3, 8 }, { QMetaType::Int, 13 }, { QMetaType::Int, 14 },
        }}),
        // Signal 'updateUi'
        QtMocHelpers::SignalData<void(int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 },
        }}),
        // Signal 'charAdded'
        QtMocHelpers::SignalData<void(int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 18 },
        }}),
        // Signal 'savePointChanged'
        QtMocHelpers::SignalData<void(bool)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 20 },
        }}),
        // Signal 'modifyAttemptReadOnly'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'zoomChanged'
        QtMocHelpers::SignalData<void(int)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 23 },
        }}),
        // Signal 'needShown'
        QtMocHelpers::SignalData<void(size_t, size_t)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 11 },
        }}),
        // Signal 'painted'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'userListSelection'
        QtMocHelpers::SignalData<void(const QString &, int)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::Int, 10 },
        }}),
        // Signal 'bufferActivated'
        QtMocHelpers::SignalData<void(Buffer *)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 28, 29 },
        }}),
        // Signal 'uiChanged'
        QtMocHelpers::SignalData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'textChanged'
        QtMocHelpers::SignalData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cursorPositionChanged'
        QtMocHelpers::SignalData<void(int, int)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 33 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScintillaComponent, qt_meta_tag_ZN18ScintillaComponentE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScintillaComponent::staticMetaObject = { {
    QMetaObject::SuperData::link<ScintillaEditBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ScintillaComponentE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ScintillaComponentE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18ScintillaComponentE_t>.metaTypes,
    nullptr
} };

void ScintillaComponent::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScintillaComponent *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->marginClicked((*reinterpret_cast<std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 1: _t->doubleClicked((*reinterpret_cast<std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<size_t>>(_a[2]))); break;
        case 2: _t->modified((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<size_t>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<size_t>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<size_t>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[7]))); break;
        case 3: _t->updateUi((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->charAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->savePointChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->modifyAttemptReadOnly(); break;
        case 7: _t->zoomChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->needShown((*reinterpret_cast<std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<size_t>>(_a[2]))); break;
        case 9: _t->painted(); break;
        case 10: _t->userListSelection((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 11: _t->bufferActivated((*reinterpret_cast<std::add_pointer_t<Buffer*>>(_a[1]))); break;
        case 12: _t->uiChanged(); break;
        case 13: _t->textChanged(); break;
        case 14: _t->cursorPositionChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Buffer* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(size_t , int , int )>(_a, &ScintillaComponent::marginClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(size_t , size_t )>(_a, &ScintillaComponent::doubleClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(int , size_t , size_t , const QString & , size_t , int , int )>(_a, &ScintillaComponent::modified, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(int )>(_a, &ScintillaComponent::updateUi, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(int )>(_a, &ScintillaComponent::charAdded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(bool )>(_a, &ScintillaComponent::savePointChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)()>(_a, &ScintillaComponent::modifyAttemptReadOnly, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(int )>(_a, &ScintillaComponent::zoomChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(size_t , size_t )>(_a, &ScintillaComponent::needShown, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)()>(_a, &ScintillaComponent::painted, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(const QString & , int )>(_a, &ScintillaComponent::userListSelection, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(Buffer * )>(_a, &ScintillaComponent::bufferActivated, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)()>(_a, &ScintillaComponent::uiChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)()>(_a, &ScintillaComponent::textChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaComponent::*)(int , int )>(_a, &ScintillaComponent::cursorPositionChanged, 14))
            return;
    }
}

const QMetaObject *ScintillaComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ScintillaComponentE_t>.strings))
        return static_cast<void*>(this);
    return ScintillaEditBase::qt_metacast(_clname);
}

int ScintillaComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ScintillaEditBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ScintillaComponent::marginClicked(size_t _t1, int _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void ScintillaComponent::doubleClicked(size_t _t1, size_t _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void ScintillaComponent::modified(int _t1, size_t _t2, size_t _t3, const QString & _t4, size_t _t5, int _t6, int _t7)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3, _t4, _t5, _t6, _t7);
}

// SIGNAL 3
void ScintillaComponent::updateUi(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ScintillaComponent::charAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ScintillaComponent::savePointChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ScintillaComponent::modifyAttemptReadOnly()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ScintillaComponent::zoomChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ScintillaComponent::needShown(size_t _t1, size_t _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void ScintillaComponent::painted()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ScintillaComponent::userListSelection(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void ScintillaComponent::bufferActivated(Buffer * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void ScintillaComponent::uiChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ScintillaComponent::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ScintillaComponent::cursorPositionChanged(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN14ScintillaCtrlsE_t {};
} // unnamed namespace

template <> constexpr inline auto ScintillaCtrls::qt_create_metaobjectdata<qt_meta_tag_ZN14ScintillaCtrlsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScintillaCtrls",
        "setDarkMode",
        "",
        "enabled"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'setDarkMode'
        QtMocHelpers::SlotData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScintillaCtrls, qt_meta_tag_ZN14ScintillaCtrlsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScintillaCtrls::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScintillaCtrlsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScintillaCtrlsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14ScintillaCtrlsE_t>.metaTypes,
    nullptr
} };

void ScintillaCtrls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScintillaCtrls *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->setDarkMode((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ScintillaCtrls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaCtrls::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScintillaCtrlsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScintillaCtrls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
