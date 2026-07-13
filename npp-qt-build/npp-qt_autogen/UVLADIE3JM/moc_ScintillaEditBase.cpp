/****************************************************************************
** Meta object code from reading C++ file 'ScintillaEditBase.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/ScintillaEditBase.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScintillaEditBase.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17ScintillaEditBaseE_t {};
} // unnamed namespace

template <> constexpr inline auto ScintillaEditBase::qt_create_metaobjectdata<qt_meta_tag_ZN17ScintillaEditBaseE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScintillaEditBase",
        "marginClicked",
        "",
        "Scintilla::Position",
        "pos",
        "Scintilla::KeyMod",
        "mod",
        "margin",
        "doubleClick",
        "line",
        "modified",
        "Scintilla::ModificationFlags",
        "type",
        "len",
        "text",
        "Scintilla::FoldLevel",
        "foldNow",
        "foldPrev",
        "updateUi",
        "Scintilla::Update",
        "updated",
        "charAdded",
        "ch",
        "savePointChanged",
        "dirty",
        "modifyAttemptReadOnly",
        "zoom",
        "zoomLevel",
        "needShown",
        "painted",
        "userListSelection",
        "linesAdded",
        "lines",
        "autoCompleteCancel",
        "autoCompleteSelect"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'marginClicked'
        QtMocHelpers::SignalData<void(Scintilla::Position, Scintilla::KeyMod, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 5, 6 }, { QMetaType::Int, 7 },
        }}),
        // Signal 'doubleClick'
        QtMocHelpers::SignalData<void(Scintilla::Position, Scintilla::Position)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 9 },
        }}),
        // Signal 'modified'
        QtMocHelpers::SignalData<void(Scintilla::ModificationFlags, Scintilla::Position, Scintilla::Position, const QByteArray &, Scintilla::Position, Scintilla::FoldLevel, Scintilla::FoldLevel)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 13 }, { QMetaType::QByteArray, 14 },
            { 0x80000000 | 3, 9 }, { 0x80000000 | 15, 16 }, { 0x80000000 | 15, 17 },
        }}),
        // Signal 'updateUi'
        QtMocHelpers::SignalData<void(Scintilla::Update)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Signal 'charAdded'
        QtMocHelpers::SignalData<void(int)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 22 },
        }}),
        // Signal 'savePointChanged'
        QtMocHelpers::SignalData<void(bool)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 24 },
        }}),
        // Signal 'modifyAttemptReadOnly'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'zoom'
        QtMocHelpers::SignalData<void(int)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 27 },
        }}),
        // Signal 'needShown'
        QtMocHelpers::SignalData<void(Scintilla::Position, Scintilla::Position)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 13 },
        }}),
        // Signal 'painted'
        QtMocHelpers::SignalData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'userListSelection'
        QtMocHelpers::SignalData<void(const QString &, int)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::Int, 12 },
        }}),
        // Signal 'linesAdded'
        QtMocHelpers::SignalData<void(int)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 32 },
        }}),
        // Slot 'autoCompleteCancel'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'autoCompleteSelect'
        QtMocHelpers::SlotData<void(const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScintillaEditBase, qt_meta_tag_ZN17ScintillaEditBaseE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScintillaEditBase::staticMetaObject = { {
    QMetaObject::SuperData::link<QsciScintilla::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ScintillaEditBaseE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ScintillaEditBaseE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ScintillaEditBaseE_t>.metaTypes,
    nullptr
} };

void ScintillaEditBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScintillaEditBase *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->marginClicked((*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Scintilla::KeyMod>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 1: _t->doubleClick((*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[2]))); break;
        case 2: _t->modified((*reinterpret_cast<std::add_pointer_t<Scintilla::ModificationFlags>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<Scintilla::FoldLevel>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<Scintilla::FoldLevel>>(_a[7]))); break;
        case 3: _t->updateUi((*reinterpret_cast<std::add_pointer_t<Scintilla::Update>>(_a[1]))); break;
        case 4: _t->charAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->savePointChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->modifyAttemptReadOnly(); break;
        case 7: _t->zoom((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->needShown((*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Scintilla::Position>>(_a[2]))); break;
        case 9: _t->painted(); break;
        case 10: _t->userListSelection((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 11: _t->linesAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->autoCompleteCancel(); break;
        case 13: _t->autoCompleteSelect((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(Scintilla::Position , Scintilla::KeyMod , int )>(_a, &ScintillaEditBase::marginClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(Scintilla::Position , Scintilla::Position )>(_a, &ScintillaEditBase::doubleClick, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(Scintilla::ModificationFlags , Scintilla::Position , Scintilla::Position , const QByteArray & , Scintilla::Position , Scintilla::FoldLevel , Scintilla::FoldLevel )>(_a, &ScintillaEditBase::modified, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(Scintilla::Update )>(_a, &ScintillaEditBase::updateUi, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(int )>(_a, &ScintillaEditBase::charAdded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(bool )>(_a, &ScintillaEditBase::savePointChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)()>(_a, &ScintillaEditBase::modifyAttemptReadOnly, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(int )>(_a, &ScintillaEditBase::zoom, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(Scintilla::Position , Scintilla::Position )>(_a, &ScintillaEditBase::needShown, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)()>(_a, &ScintillaEditBase::painted, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(const QString & , int )>(_a, &ScintillaEditBase::userListSelection, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScintillaEditBase::*)(int )>(_a, &ScintillaEditBase::linesAdded, 11))
            return;
    }
}

const QMetaObject *ScintillaEditBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaEditBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ScintillaEditBaseE_t>.strings))
        return static_cast<void*>(this);
    return QsciScintilla::qt_metacast(_clname);
}

int ScintillaEditBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QsciScintilla::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ScintillaEditBase::marginClicked(Scintilla::Position _t1, Scintilla::KeyMod _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void ScintillaEditBase::doubleClick(Scintilla::Position _t1, Scintilla::Position _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void ScintillaEditBase::modified(Scintilla::ModificationFlags _t1, Scintilla::Position _t2, Scintilla::Position _t3, const QByteArray & _t4, Scintilla::Position _t5, Scintilla::FoldLevel _t6, Scintilla::FoldLevel _t7)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3, _t4, _t5, _t6, _t7);
}

// SIGNAL 3
void ScintillaEditBase::updateUi(Scintilla::Update _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ScintillaEditBase::charAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ScintillaEditBase::savePointChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ScintillaEditBase::modifyAttemptReadOnly()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ScintillaEditBase::zoom(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ScintillaEditBase::needShown(Scintilla::Position _t1, Scintilla::Position _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void ScintillaEditBase::painted()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ScintillaEditBase::userListSelection(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void ScintillaEditBase::linesAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}
QT_WARNING_POP
