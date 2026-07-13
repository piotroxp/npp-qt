/****************************************************************************
** Meta object code from reading C++ file 'Processus.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/Processus.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Processus.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7ProcessE_t {};
} // unnamed namespace

template <> constexpr inline auto Process::qt_create_metaobjectdata<qt_meta_tag_ZN7ProcessE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'processFinished'
        QtMocHelpers::SignalData<void(int, QProcess::ExitStatus)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'readyReadStdOut'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'readyReadStdErr'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(QProcess::ProcessError)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onFinished'
        QtMocHelpers::SlotData<void(int, QProcess::ExitStatus)>(12, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { 0x80000000 | 4, 13 },
        }}),
        // Slot 'onReadyReadStandardOutput'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessProtected, QMetaType::Void),
        // Slot 'onReadyReadStandardError'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessProtected, QMetaType::Void),
        // Slot 'onErrorOccurred'
        QtMocHelpers::SlotData<void(QProcess::ProcessError)>(16, 2, QMC::AccessProtected, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Process, qt_meta_tag_ZN7ProcessE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Process::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ProcessE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ProcessE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7ProcessE_t>.metaTypes,
    nullptr
} };

void Process::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Process *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->processFinished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 1: _t->readyReadStdOut((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->readyReadStdErr((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 4: _t->onFinished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 5: _t->onReadyReadStandardOutput(); break;
        case 6: _t->onReadyReadStandardError(); break;
        case 7: _t->onErrorOccurred((*reinterpret_cast<std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Process::*)(int , QProcess::ExitStatus )>(_a, &Process::processFinished, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Process::*)(const QString & )>(_a, &Process::readyReadStdOut, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Process::*)(const QString & )>(_a, &Process::readyReadStdErr, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Process::*)(QProcess::ProcessError )>(_a, &Process::errorOccurred, 3))
            return;
    }
}

const QMetaObject *Process::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Process::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ProcessE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Process::processFinished(int _t1, QProcess::ExitStatus _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void Process::readyReadStdOut(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Process::readyReadStdErr(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Process::errorOccurred(QProcess::ProcessError _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
