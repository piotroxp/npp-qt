/****************************************************************************
** Meta object code from reading C++ file 'TrayIcon.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/TrayIcon.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TrayIcon.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_TrayIconController_t {
    uint offsetsAndSizes[12];
    char stringdata0[19];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[21];
    char stringdata5[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_TrayIconController_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_TrayIconController_t qt_meta_stringdata_TrayIconController = {
    {
        QT_MOC_LITERAL(0, 18),  // "TrayIconController"
        QT_MOC_LITERAL(19, 15),  // "trayIconClicked"
        QT_MOC_LITERAL(35, 0),  // ""
        QT_MOC_LITERAL(36, 21),  // "trayIconDoubleClicked"
        QT_MOC_LITERAL(58, 20),  // "trayIconRightClicked"
        QT_MOC_LITERAL(79, 21)   // "trayIconMiddleClicked"
    },
    "TrayIconController",
    "trayIconClicked",
    "",
    "trayIconDoubleClicked",
    "trayIconRightClicked",
    "trayIconMiddleClicked"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_TrayIconController[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,    1 /* Public */,
       3,    0,   39,    2, 0x06,    2 /* Public */,
       4,    0,   40,    2, 0x06,    3 /* Public */,
       5,    0,   41,    2, 0x06,    4 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject TrayIconController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TrayIconController.offsetsAndSizes,
    qt_meta_data_TrayIconController,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_TrayIconController_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TrayIconController, std::true_type>,
        // method 'trayIconClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trayIconDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trayIconRightClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trayIconMiddleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void TrayIconController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TrayIconController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->trayIconClicked(); break;
        case 1: _t->trayIconDoubleClicked(); break;
        case 2: _t->trayIconRightClicked(); break;
        case 3: _t->trayIconMiddleClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TrayIconController::*)();
            if (_t _q_method = &TrayIconController::trayIconClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TrayIconController::*)();
            if (_t _q_method = &TrayIconController::trayIconDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TrayIconController::*)();
            if (_t _q_method = &TrayIconController::trayIconRightClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TrayIconController::*)();
            if (_t _q_method = &TrayIconController::trayIconMiddleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *TrayIconController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TrayIconController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TrayIconController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TrayIconController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void TrayIconController::trayIconClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TrayIconController::trayIconDoubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TrayIconController::trayIconRightClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TrayIconController::trayIconMiddleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
