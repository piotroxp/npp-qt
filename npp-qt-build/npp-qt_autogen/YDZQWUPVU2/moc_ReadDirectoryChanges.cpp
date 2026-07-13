/****************************************************************************
** Meta object code from reading C++ file 'ReadDirectoryChanges.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/ReadDirectoryChanges.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ReadDirectoryChanges.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16DirectoryWatcherE_t {};
} // unnamed namespace

template <> constexpr inline auto DirectoryWatcher::qt_create_metaobjectdata<qt_meta_tag_ZN16DirectoryWatcherE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DirectoryWatcher",
        "directoryChanged",
        "",
        "action",
        "fileName",
        "fileAdded",
        "error",
        "errorMsg",
        "fileRemoved",
        "fileModified",
        "watcherError",
        "errorString",
        "onDirectoryChanged",
        "path",
        "onFileChanged",
        "onWatcherError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'directoryChanged'
        QtMocHelpers::SignalData<void(quint32, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'fileAdded'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'fileRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'fileModified'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'watcherError'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onDirectoryChanged'
        QtMocHelpers::SlotData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'onFileChanged'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'onWatcherError'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DirectoryWatcher, qt_meta_tag_ZN16DirectoryWatcherE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DirectoryWatcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DirectoryWatcherE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DirectoryWatcherE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16DirectoryWatcherE_t>.metaTypes,
    nullptr
} };

void DirectoryWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DirectoryWatcher *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->directoryChanged((*reinterpret_cast<std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->fileAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->fileRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->fileModified((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->watcherError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onDirectoryChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onFileChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onWatcherError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(quint32 , const QString & )>(_a, &DirectoryWatcher::directoryChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(const QString & )>(_a, &DirectoryWatcher::fileAdded, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(const QString & )>(_a, &DirectoryWatcher::error, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(const QString & )>(_a, &DirectoryWatcher::fileRemoved, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(const QString & )>(_a, &DirectoryWatcher::fileModified, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DirectoryWatcher::*)(const QString & )>(_a, &DirectoryWatcher::watcherError, 5))
            return;
    }
}

const QMetaObject *DirectoryWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DirectoryWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DirectoryWatcherE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DirectoryWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DirectoryWatcher::directoryChanged(quint32 _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void DirectoryWatcher::fileAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DirectoryWatcher::error(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DirectoryWatcher::fileRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DirectoryWatcher::fileModified(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DirectoryWatcher::watcherError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
