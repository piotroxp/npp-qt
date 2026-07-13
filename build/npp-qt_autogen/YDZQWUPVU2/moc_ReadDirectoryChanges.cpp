/****************************************************************************
** Meta object code from reading C++ file 'ReadDirectoryChanges.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/ReadDirectoryChanges.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ReadDirectoryChanges.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DirectoryWatcher_t {
    uint offsetsAndSizes[32];
    char stringdata0[17];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[9];
    char stringdata5[10];
    char stringdata6[6];
    char stringdata7[9];
    char stringdata8[12];
    char stringdata9[13];
    char stringdata10[13];
    char stringdata11[12];
    char stringdata12[19];
    char stringdata13[5];
    char stringdata14[14];
    char stringdata15[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DirectoryWatcher_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DirectoryWatcher_t qt_meta_stringdata_DirectoryWatcher = {
    {
        QT_MOC_LITERAL(0, 16),  // "DirectoryWatcher"
        QT_MOC_LITERAL(17, 16),  // "directoryChanged"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 6),  // "action"
        QT_MOC_LITERAL(42, 8),  // "fileName"
        QT_MOC_LITERAL(51, 9),  // "fileAdded"
        QT_MOC_LITERAL(61, 5),  // "error"
        QT_MOC_LITERAL(67, 8),  // "errorMsg"
        QT_MOC_LITERAL(76, 11),  // "fileRemoved"
        QT_MOC_LITERAL(88, 12),  // "fileModified"
        QT_MOC_LITERAL(101, 12),  // "watcherError"
        QT_MOC_LITERAL(114, 11),  // "errorString"
        QT_MOC_LITERAL(126, 18),  // "onDirectoryChanged"
        QT_MOC_LITERAL(145, 4),  // "path"
        QT_MOC_LITERAL(150, 13),  // "onFileChanged"
        QT_MOC_LITERAL(164, 14)   // "onWatcherError"
    },
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
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DirectoryWatcher[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   68,    2, 0x06,    1 /* Public */,
       5,    1,   73,    2, 0x06,    4 /* Public */,
       6,    1,   76,    2, 0x06,    6 /* Public */,
       8,    1,   79,    2, 0x06,    8 /* Public */,
       9,    1,   82,    2, 0x06,   10 /* Public */,
      10,    1,   85,    2, 0x06,   12 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    1,   88,    2, 0x0a,   14 /* Public */,
      14,    1,   91,    2, 0x0a,   16 /* Public */,
      15,    1,   94,    2, 0x0a,   18 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject DirectoryWatcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DirectoryWatcher.offsetsAndSizes,
    qt_meta_data_DirectoryWatcher,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DirectoryWatcher_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DirectoryWatcher, std::true_type>,
        // method 'directoryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileModified'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'watcherError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onDirectoryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onFileChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWatcherError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void DirectoryWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DirectoryWatcher *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->directoryChanged((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->fileAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->fileRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->fileModified((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->watcherError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onDirectoryChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onFileChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onWatcherError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DirectoryWatcher::*)(quint32 , const QString & );
            if (_t _q_method = &DirectoryWatcher::directoryChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DirectoryWatcher::*)(const QString & );
            if (_t _q_method = &DirectoryWatcher::fileAdded; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DirectoryWatcher::*)(const QString & );
            if (_t _q_method = &DirectoryWatcher::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DirectoryWatcher::*)(const QString & );
            if (_t _q_method = &DirectoryWatcher::fileRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DirectoryWatcher::*)(const QString & );
            if (_t _q_method = &DirectoryWatcher::fileModified; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DirectoryWatcher::*)(const QString & );
            if (_t _q_method = &DirectoryWatcher::watcherError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *DirectoryWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DirectoryWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DirectoryWatcher.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DirectoryWatcher::directoryChanged(quint32 _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DirectoryWatcher::fileAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DirectoryWatcher::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DirectoryWatcher::fileRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DirectoryWatcher::fileModified(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DirectoryWatcher::watcherError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
