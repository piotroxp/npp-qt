/****************************************************************************
** Meta object code from reading C++ file 'Application.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/Application.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Application.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Application_t {
    uint offsetsAndSizes[42];
    char stringdata0[12];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[7];
    char stringdata5[13];
    char stringdata6[16];
    char stringdata7[10];
    char stringdata8[12];
    char stringdata9[5];
    char stringdata10[13];
    char stringdata11[10];
    char stringdata12[16];
    char stringdata13[10];
    char stringdata14[16];
    char stringdata15[14];
    char stringdata16[4];
    char stringdata17[17];
    char stringdata18[12];
    char stringdata19[25];
    char stringdata20[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Application_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Application_t qt_meta_stringdata_Application = {
    {
        QT_MOC_LITERAL(0, 11),  // "Application"
        QT_MOC_LITERAL(12, 12),  // "bufferOpened"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 8),  // "BufferID"
        QT_MOC_LITERAL(35, 6),  // "buffer"
        QT_MOC_LITERAL(42, 12),  // "bufferClosed"
        QT_MOC_LITERAL(55, 15),  // "bufferActivated"
        QT_MOC_LITERAL(71, 9),  // "fileSaved"
        QT_MOC_LITERAL(81, 11),  // "std::string"
        QT_MOC_LITERAL(93, 4),  // "path"
        QT_MOC_LITERAL(98, 12),  // "themeChanged"
        QT_MOC_LITERAL(111, 9),  // "themeName"
        QT_MOC_LITERAL(121, 15),  // "commandExecuted"
        QT_MOC_LITERAL(137, 9),  // "commandId"
        QT_MOC_LITERAL(147, 15),  // "onBufferChanged"
        QT_MOC_LITERAL(163, 13),  // "onMenuCommand"
        QT_MOC_LITERAL(177, 3),  // "cmd"
        QT_MOC_LITERAL(181, 16),  // "onToolBarCommand"
        QT_MOC_LITERAL(198, 11),  // "onFileSaved"
        QT_MOC_LITERAL(210, 24),  // "onFileModifiedExternally"
        QT_MOC_LITERAL(235, 14)   // "onThemeChanged"
    },
    "Application",
    "bufferOpened",
    "",
    "BufferID",
    "buffer",
    "bufferClosed",
    "bufferActivated",
    "fileSaved",
    "std::string",
    "path",
    "themeChanged",
    "themeName",
    "commandExecuted",
    "commandId",
    "onBufferChanged",
    "onMenuCommand",
    "cmd",
    "onToolBarCommand",
    "onFileSaved",
    "onFileModifiedExternally",
    "onThemeChanged"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Application[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       5,    1,   89,    2, 0x06,    3 /* Public */,
       6,    1,   92,    2, 0x06,    5 /* Public */,
       7,    1,   95,    2, 0x06,    7 /* Public */,
      10,    1,   98,    2, 0x06,    9 /* Public */,
      12,    1,  101,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    1,  104,    2, 0x0a,   13 /* Public */,
      15,    1,  107,    2, 0x0a,   15 /* Public */,
      17,    1,  110,    2, 0x0a,   17 /* Public */,
      18,    1,  113,    2, 0x0a,   19 /* Public */,
      19,    1,  116,    2, 0x0a,   21 /* Public */,
      20,    1,  119,    2, 0x0a,   23 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,   11,
    QMetaType::Void, QMetaType::Int,   13,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject Application::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Application.offsetsAndSizes,
    qt_meta_data_Application,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Application_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Application, std::true_type>,
        // method 'bufferOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'bufferClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'bufferActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'fileSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'themeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'commandExecuted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onBufferChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'onMenuCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onToolBarCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onFileSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'onFileModifiedExternally'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'onThemeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>
    >,
    nullptr
} };

void Application::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Application *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bufferOpened((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 1: _t->bufferClosed((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 2: _t->bufferActivated((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 3: _t->fileSaved((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 4: _t->themeChanged((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 5: _t->commandExecuted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onBufferChanged((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 7: _t->onMenuCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onToolBarCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onFileSaved((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 10: _t->onFileModifiedExternally((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 11: _t->onThemeChanged((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferOpened; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Application::*)(const std::string & );
            if (_t _q_method = &Application::fileSaved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Application::*)(const std::string & );
            if (_t _q_method = &Application::themeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Application::*)(int );
            if (_t _q_method = &Application::commandExecuted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *Application::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Application::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Application.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "NonCopyable"))
        return static_cast< NonCopyable*>(this);
    return QObject::qt_metacast(_clname);
}

int Application::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Application::bufferOpened(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Application::bufferClosed(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Application::bufferActivated(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Application::fileSaved(const std::string & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Application::themeChanged(const std::string & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Application::commandExecuted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
