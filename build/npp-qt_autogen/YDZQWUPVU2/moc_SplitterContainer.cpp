/****************************************************************************
** Meta object code from reading C++ file 'SplitterContainer.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/SplitterContainer.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SplitterContainer.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_SplitterContainer_t {
    uint offsetsAndSizes[12];
    char stringdata0[18];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[7];
    char stringdata5[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SplitterContainer_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SplitterContainer_t qt_meta_stringdata_SplitterContainer = {
    {
        QT_MOC_LITERAL(0, 17),  // "SplitterContainer"
        QT_MOC_LITERAL(18, 13),  // "splitterMoved"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 8),  // "newRatio"
        QT_MOC_LITERAL(42, 6),  // "rotate"
        QT_MOC_LITERAL(49, 15)   // "onSplitterMoved"
    },
    "SplitterContainer",
    "splitterMoved",
    "",
    "newRatio",
    "rotate",
    "onSplitterMoved"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SplitterContainer[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   35,    2, 0x0a,    3 /* Public */,
       5,    1,   36,    2, 0x0a,    4 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject SplitterContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<WindowBase::staticMetaObject>(),
    qt_meta_stringdata_SplitterContainer.offsetsAndSizes,
    qt_meta_data_SplitterContainer,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SplitterContainer_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SplitterContainer, std::true_type>,
        // method 'splitterMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'rotate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSplitterMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>
    >,
    nullptr
} };

void SplitterContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SplitterContainer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->splitterMoved((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->rotate(); break;
        case 2: _t->onSplitterMoved((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SplitterContainer::*)(double );
            if (_t _q_method = &SplitterContainer::splitterMoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *SplitterContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SplitterContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SplitterContainer.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void SplitterContainer::splitterMoved(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
