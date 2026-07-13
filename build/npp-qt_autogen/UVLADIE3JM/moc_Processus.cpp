/****************************************************************************
** Meta object code from reading C++ file 'Processus.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/Processus.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Processus.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Process_t {
    uint offsetsAndSizes[34];
    char stringdata0[8];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[21];
    char stringdata5[11];
    char stringdata6[16];
    char stringdata7[5];
    char stringdata8[16];
    char stringdata9[14];
    char stringdata10[23];
    char stringdata11[6];
    char stringdata12[11];
    char stringdata13[7];
    char stringdata14[26];
    char stringdata15[25];
    char stringdata16[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Process_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Process_t qt_meta_stringdata_Process = {
    {
        QT_MOC_LITERAL(0, 7),  // "Process"
        QT_MOC_LITERAL(8, 15),  // "processFinished"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 8),  // "exitCode"
        QT_MOC_LITERAL(34, 20),  // "QProcess::ExitStatus"
        QT_MOC_LITERAL(55, 10),  // "exitStatus"
        QT_MOC_LITERAL(66, 15),  // "readyReadStdOut"
        QT_MOC_LITERAL(82, 4),  // "data"
        QT_MOC_LITERAL(87, 15),  // "readyReadStdErr"
        QT_MOC_LITERAL(103, 13),  // "errorOccurred"
        QT_MOC_LITERAL(117, 22),  // "QProcess::ProcessError"
        QT_MOC_LITERAL(140, 5),  // "error"
        QT_MOC_LITERAL(146, 10),  // "onFinished"
        QT_MOC_LITERAL(157, 6),  // "status"
        QT_MOC_LITERAL(164, 25),  // "onReadyReadStandardOutput"
        QT_MOC_LITERAL(190, 24),  // "onReadyReadStandardError"
        QT_MOC_LITERAL(215, 15)   // "onErrorOccurred"
    },
    "Process",
    "processFinished",
    "",
    "exitCode",
    "QProcess::ExitStatus",
    "exitStatus",
    "readyReadStdOut",
    "data",
    "readyReadStdErr",
    "errorOccurred",
    "QProcess::ProcessError",
    "error",
    "onFinished",
    "status",
    "onReadyReadStandardOutput",
    "onReadyReadStandardError",
    "onErrorOccurred"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Process[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   62,    2, 0x06,    1 /* Public */,
       6,    1,   67,    2, 0x06,    4 /* Public */,
       8,    1,   70,    2, 0x06,    6 /* Public */,
       9,    1,   73,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    2,   76,    2, 0x09,   10 /* Protected */,
      14,    0,   81,    2, 0x09,   13 /* Protected */,
      15,    0,   82,    2, 0x09,   14 /* Protected */,
      16,    1,   83,    2, 0x09,   15 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, 0x80000000 | 10,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject Process::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Process.offsetsAndSizes,
    qt_meta_data_Process,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Process_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Process, std::true_type>,
        // method 'processFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'readyReadStdOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'readyReadStdErr'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'onFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'onReadyReadStandardOutput'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReadyReadStandardError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>
    >,
    nullptr
} };

void Process::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Process *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->processFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 1: _t->readyReadStdOut((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->readyReadStdErr((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 4: _t->onFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 5: _t->onReadyReadStandardOutput(); break;
        case 6: _t->onReadyReadStandardError(); break;
        case 7: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Process::*)(int , QProcess::ExitStatus );
            if (_t _q_method = &Process::processFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Process::*)(const QString & );
            if (_t _q_method = &Process::readyReadStdOut; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Process::*)(const QString & );
            if (_t _q_method = &Process::readyReadStdErr; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Process::*)(QProcess::ProcessError );
            if (_t _q_method = &Process::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *Process::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Process::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Process.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Process::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Process::processFinished(int _t1, QProcess::ExitStatus _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Process::readyReadStdOut(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Process::readyReadStdErr(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Process::errorOccurred(QProcess::ProcessError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
