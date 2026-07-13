/****************************************************************************
** Meta object code from reading C++ file 'RunMacroDlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/RunMacroDlg.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunMacroDlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_RunMacroDlg_t {
    uint offsetsAndSizes[28];
    char stringdata0[12];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[6];
    char stringdata5[10];
    char stringdata6[13];
    char stringdata7[16];
    char stringdata8[16];
    char stringdata9[6];
    char stringdata10[15];
    char stringdata11[5];
    char stringdata12[15];
    char stringdata13[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RunMacroDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RunMacroDlg_t qt_meta_stringdata_RunMacroDlg = {
    {
        QT_MOC_LITERAL(0, 11),  // "RunMacroDlg"
        QT_MOC_LITERAL(12, 8),  // "runMacro"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 10),  // "macroIndex"
        QT_MOC_LITERAL(33, 5),  // "times"
        QT_MOC_LITERAL(39, 9),  // "cancelled"
        QT_MOC_LITERAL(49, 12),  // "onRunClicked"
        QT_MOC_LITERAL(62, 15),  // "onCancelClicked"
        QT_MOC_LITERAL(78, 15),  // "onMacroSelected"
        QT_MOC_LITERAL(94, 5),  // "index"
        QT_MOC_LITERAL(100, 14),  // "onTimesChanged"
        QT_MOC_LITERAL(115, 4),  // "text"
        QT_MOC_LITERAL(120, 14),  // "onMultiToggled"
        QT_MOC_LITERAL(135, 7)   // "checked"
    },
    "RunMacroDlg",
    "runMacro",
    "",
    "macroIndex",
    "times",
    "cancelled",
    "onRunClicked",
    "onCancelClicked",
    "onMacroSelected",
    "index",
    "onTimesChanged",
    "text",
    "onMultiToggled",
    "checked"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RunMacroDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   56,    2, 0x06,    1 /* Public */,
       5,    0,   61,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   62,    2, 0x0a,    5 /* Public */,
       7,    0,   63,    2, 0x0a,    6 /* Public */,
       8,    1,   64,    2, 0x0a,    7 /* Public */,
      10,    1,   67,    2, 0x0a,    9 /* Public */,
      12,    1,   70,    2, 0x0a,   11 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::Bool,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject RunMacroDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_meta_stringdata_RunMacroDlg.offsetsAndSizes,
    qt_meta_data_RunMacroDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RunMacroDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RunMacroDlg, std::true_type>,
        // method 'runMacro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'cancelled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRunClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancelClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMacroSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTimesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onMultiToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void RunMacroDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RunMacroDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->runMacro((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->cancelled(); break;
        case 2: _t->onRunClicked(); break;
        case 3: _t->onCancelClicked(); break;
        case 4: _t->onMacroSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onTimesChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onMultiToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RunMacroDlg::*)(int , int );
            if (_t _q_method = &RunMacroDlg::runMacro; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RunMacroDlg::*)();
            if (_t _q_method = &RunMacroDlg::cancelled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *RunMacroDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RunMacroDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RunMacroDlg.stringdata0))
        return static_cast<void*>(this);
    return StaticDialog::qt_metacast(_clname);
}

int RunMacroDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StaticDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void RunMacroDlg::runMacro(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RunMacroDlg::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
