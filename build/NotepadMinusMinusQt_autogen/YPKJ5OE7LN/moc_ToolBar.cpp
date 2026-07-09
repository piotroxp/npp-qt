/****************************************************************************
** Meta object code from reading C++ file 'ToolBar.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/ToolBar.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ToolBar.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_ToolBar_t {
    uint offsetsAndSizes[28];
    char stringdata0[8];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[6];
    char stringdata5[7];
    char stringdata6[7];
    char stringdata7[7];
    char stringdata8[7];
    char stringdata9[6];
    char stringdata10[7];
    char stringdata11[8];
    char stringdata12[7];
    char stringdata13[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ToolBar_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ToolBar_t qt_meta_stringdata_ToolBar = {
    {
        QT_MOC_LITERAL(0, 7),  // "ToolBar"
        QT_MOC_LITERAL(8, 14),  // "toolBarCommand"
        QT_MOC_LITERAL(23, 0),  // ""
        QT_MOC_LITERAL(24, 3),  // "cmd"
        QT_MOC_LITERAL(28, 5),  // "onNew"
        QT_MOC_LITERAL(34, 6),  // "onOpen"
        QT_MOC_LITERAL(41, 6),  // "onSave"
        QT_MOC_LITERAL(48, 6),  // "onUndo"
        QT_MOC_LITERAL(55, 6),  // "onRedo"
        QT_MOC_LITERAL(62, 5),  // "onCut"
        QT_MOC_LITERAL(68, 6),  // "onCopy"
        QT_MOC_LITERAL(75, 7),  // "onPaste"
        QT_MOC_LITERAL(83, 6),  // "onFind"
        QT_MOC_LITERAL(90, 9)   // "onReplace"
    },
    "ToolBar",
    "toolBarCommand",
    "",
    "cmd",
    "onNew",
    "onOpen",
    "onSave",
    "onUndo",
    "onRedo",
    "onCut",
    "onCopy",
    "onPaste",
    "onFind",
    "onReplace"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ToolBar[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   83,    2, 0x0a,    3 /* Public */,
       5,    0,   84,    2, 0x0a,    4 /* Public */,
       6,    0,   85,    2, 0x0a,    5 /* Public */,
       7,    0,   86,    2, 0x0a,    6 /* Public */,
       8,    0,   87,    2, 0x0a,    7 /* Public */,
       9,    0,   88,    2, 0x0a,    8 /* Public */,
      10,    0,   89,    2, 0x0a,    9 /* Public */,
      11,    0,   90,    2, 0x0a,   10 /* Public */,
      12,    0,   91,    2, 0x0a,   11 /* Public */,
      13,    0,   92,    2, 0x0a,   12 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ToolBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QToolBar::staticMetaObject>(),
    qt_meta_stringdata_ToolBar.offsetsAndSizes,
    qt_meta_data_ToolBar,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ToolBar_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ToolBar, std::true_type>,
        // method 'toolBarCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onNew'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOpen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUndo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRedo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopy'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPaste'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToolBar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->toolBarCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onNew(); break;
        case 2: _t->onOpen(); break;
        case 3: _t->onSave(); break;
        case 4: _t->onUndo(); break;
        case 5: _t->onRedo(); break;
        case 6: _t->onCut(); break;
        case 7: _t->onCopy(); break;
        case 8: _t->onPaste(); break;
        case 9: _t->onFind(); break;
        case 10: _t->onReplace(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ToolBar::*)(const QString & );
            if (_t _q_method = &ToolBar::toolBarCommand; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToolBar.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int ToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ToolBar::toolBarCommand(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
