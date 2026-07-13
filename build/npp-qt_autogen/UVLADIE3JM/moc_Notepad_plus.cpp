/****************************************************************************
** Meta object code from reading C++ file 'Notepad_plus.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/Notepad_plus.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Notepad_plus.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Notepad_plus_t {
    uint offsetsAndSizes[16];
    char stringdata0[13];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[24];
    char stringdata5[17];
    char stringdata6[9];
    char stringdata7[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Notepad_plus_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Notepad_plus_t qt_meta_stringdata_Notepad_plus = {
    {
        QT_MOC_LITERAL(0, 12),  // "Notepad_plus"
        QT_MOC_LITERAL(13, 23),  // "onTrayIconDoubleClicked"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 22),  // "onTrayIconRightClicked"
        QT_MOC_LITERAL(61, 23),  // "onTrayIconMiddleClicked"
        QT_MOC_LITERAL(85, 16),  // "onTrayMenuAction"
        QT_MOC_LITERAL(102, 8),  // "QAction*"
        QT_MOC_LITERAL(111, 6)   // "action"
    },
    "Notepad_plus",
    "onTrayIconDoubleClicked",
    "",
    "onTrayIconRightClicked",
    "onTrayIconMiddleClicked",
    "onTrayMenuAction",
    "QAction*",
    "action"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Notepad_plus[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x08,    1 /* Private */,
       3,    0,   39,    2, 0x08,    2 /* Private */,
       4,    0,   40,    2, 0x08,    3 /* Private */,
       5,    1,   41,    2, 0x08,    4 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

Q_CONSTINIT const QMetaObject Notepad_plus::staticMetaObject = { {
    QMetaObject::SuperData::link<MainWindow::staticMetaObject>(),
    qt_meta_stringdata_Notepad_plus.offsetsAndSizes,
    qt_meta_data_Notepad_plus,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Notepad_plus_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Notepad_plus, std::true_type>,
        // method 'onTrayIconDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTrayIconRightClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTrayIconMiddleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTrayMenuAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>
    >,
    nullptr
} };

void Notepad_plus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Notepad_plus *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onTrayIconDoubleClicked(); break;
        case 1: _t->onTrayIconRightClicked(); break;
        case 2: _t->onTrayIconMiddleClicked(); break;
        case 3: _t->onTrayMenuAction((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *Notepad_plus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Notepad_plus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Notepad_plus.stringdata0))
        return static_cast<void*>(this);
    return MainWindow::qt_metacast(_clname);
}

int Notepad_plus::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
