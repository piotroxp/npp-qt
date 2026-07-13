/****************************************************************************
** Meta object code from reading C++ file 'shortcut.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/MISC/Common/shortcut.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shortcut.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8ShortcutE_t {};
} // unnamed namespace

template <> constexpr inline auto Shortcut::qt_create_metaobjectdata<qt_meta_tag_ZN8ShortcutE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Shortcut",
        "triggered",
        ""
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'triggered'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Shortcut, qt_meta_tag_ZN8ShortcutE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Shortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ShortcutE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ShortcutE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8ShortcutE_t>.metaTypes,
    nullptr
} };

void Shortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Shortcut *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->triggered(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Shortcut::*)()>(_a, &Shortcut::triggered, 0))
            return;
    }
}

const QMetaObject *Shortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Shortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ShortcutE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Shortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void Shortcut::triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
namespace {
struct qt_meta_tag_ZN15CommandShortcutE_t {};
} // unnamed namespace

template <> constexpr inline auto CommandShortcut::qt_create_metaobjectdata<qt_meta_tag_ZN15CommandShortcutE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CommandShortcut"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CommandShortcut, qt_meta_tag_ZN15CommandShortcutE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CommandShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<Shortcut::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CommandShortcutE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CommandShortcutE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15CommandShortcutE_t>.metaTypes,
    nullptr
} };

void CommandShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CommandShortcut *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *CommandShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CommandShortcutE_t>.strings))
        return static_cast<void*>(this);
    return Shortcut::qt_metacast(_clname);
}

int CommandShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Shortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN15ScintillaKeyMapE_t {};
} // unnamed namespace

template <> constexpr inline auto ScintillaKeyMap::qt_create_metaobjectdata<qt_meta_tag_ZN15ScintillaKeyMapE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScintillaKeyMap"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScintillaKeyMap, qt_meta_tag_ZN15ScintillaKeyMapE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScintillaKeyMap::staticMetaObject = { {
    QMetaObject::SuperData::link<Shortcut::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15ScintillaKeyMapE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15ScintillaKeyMapE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15ScintillaKeyMapE_t>.metaTypes,
    nullptr
} };

void ScintillaKeyMap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScintillaKeyMap *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *ScintillaKeyMap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaKeyMap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15ScintillaKeyMapE_t>.strings))
        return static_cast<void*>(this);
    return Shortcut::qt_metacast(_clname);
}

int ScintillaKeyMap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Shortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN13MacroShortcutE_t {};
} // unnamed namespace

template <> constexpr inline auto MacroShortcut::qt_create_metaobjectdata<qt_meta_tag_ZN13MacroShortcutE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MacroShortcut"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MacroShortcut, qt_meta_tag_ZN13MacroShortcutE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MacroShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13MacroShortcutE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13MacroShortcutE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13MacroShortcutE_t>.metaTypes,
    nullptr
} };

void MacroShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MacroShortcut *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *MacroShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MacroShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13MacroShortcutE_t>.strings))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int MacroShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN11UserCommandE_t {};
} // unnamed namespace

template <> constexpr inline auto UserCommand::qt_create_metaobjectdata<qt_meta_tag_ZN11UserCommandE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "UserCommand"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<UserCommand, qt_meta_tag_ZN11UserCommandE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject UserCommand::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11UserCommandE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11UserCommandE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11UserCommandE_t>.metaTypes,
    nullptr
} };

void UserCommand::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UserCommand *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *UserCommand::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserCommand::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11UserCommandE_t>.strings))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int UserCommand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN17PluginCmdShortcutE_t {};
} // unnamed namespace

template <> constexpr inline auto PluginCmdShortcut::qt_create_metaobjectdata<qt_meta_tag_ZN17PluginCmdShortcutE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PluginCmdShortcut"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PluginCmdShortcut, qt_meta_tag_ZN17PluginCmdShortcutE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PluginCmdShortcut::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandShortcut::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PluginCmdShortcutE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PluginCmdShortcutE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17PluginCmdShortcutE_t>.metaTypes,
    nullptr
} };

void PluginCmdShortcut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PluginCmdShortcut *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *PluginCmdShortcut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginCmdShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PluginCmdShortcutE_t>.strings))
        return static_cast<void*>(this);
    return CommandShortcut::qt_metacast(_clname);
}

int PluginCmdShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandShortcut::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN11AcceleratorE_t {};
} // unnamed namespace

template <> constexpr inline auto Accelerator::qt_create_metaobjectdata<qt_meta_tag_ZN11AcceleratorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Accelerator"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Accelerator, qt_meta_tag_ZN11AcceleratorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Accelerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11AcceleratorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11AcceleratorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11AcceleratorE_t>.metaTypes,
    nullptr
} };

void Accelerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Accelerator *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *Accelerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Accelerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11AcceleratorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Accelerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN20ScintillaAcceleratorE_t {};
} // unnamed namespace

template <> constexpr inline auto ScintillaAccelerator::qt_create_metaobjectdata<qt_meta_tag_ZN20ScintillaAcceleratorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScintillaAccelerator"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScintillaAccelerator, qt_meta_tag_ZN20ScintillaAcceleratorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScintillaAccelerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ScintillaAcceleratorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ScintillaAcceleratorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20ScintillaAcceleratorE_t>.metaTypes,
    nullptr
} };

void ScintillaAccelerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScintillaAccelerator *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *ScintillaAccelerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScintillaAccelerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ScintillaAcceleratorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScintillaAccelerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
