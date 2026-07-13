/****************************************************************************
** Meta object code from reading C++ file 'regExtDlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/MISC/RegExt/regExtDlg.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'regExtDlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_RegExtDlg_t {
    uint offsetsAndSizes[28];
    char stringdata0[10];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[17];
    char stringdata5[21];
    char stringdata6[19];
    char stringdata7[18];
    char stringdata8[5];
    char stringdata9[23];
    char stringdata10[4];
    char stringdata11[26];
    char stringdata12[32];
    char stringdata13[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RegExtDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RegExtDlg_t qt_meta_stringdata_RegExtDlg = {
    {
        QT_MOC_LITERAL(0, 9),  // "RegExtDlg"
        QT_MOC_LITERAL(10, 14),  // "extensionAdded"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 3),  // "ext"
        QT_MOC_LITERAL(30, 16),  // "extensionRemoved"
        QT_MOC_LITERAL(47, 20),  // "onAddFromLangClicked"
        QT_MOC_LITERAL(68, 18),  // "onRemoveExtClicked"
        QT_MOC_LITERAL(87, 17),  // "onCustomExtEdited"
        QT_MOC_LITERAL(105, 4),  // "text"
        QT_MOC_LITERAL(110, 22),  // "onLangSelectionChanged"
        QT_MOC_LITERAL(133, 3),  // "row"
        QT_MOC_LITERAL(137, 25),  // "onLangExtSelectionChanged"
        QT_MOC_LITERAL(163, 31),  // "onRegisteredExtSelectionChanged"
        QT_MOC_LITERAL(195, 15)   // "onCancelClicked"
    },
    "RegExtDlg",
    "extensionAdded",
    "",
    "ext",
    "extensionRemoved",
    "onAddFromLangClicked",
    "onRemoveExtClicked",
    "onCustomExtEdited",
    "text",
    "onLangSelectionChanged",
    "row",
    "onLangExtSelectionChanged",
    "onRegisteredExtSelectionChanged",
    "onCancelClicked"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RegExtDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       4,    1,   71,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   74,    2, 0x0a,    5 /* Public */,
       6,    0,   75,    2, 0x0a,    6 /* Public */,
       7,    1,   76,    2, 0x0a,    7 /* Public */,
       9,    1,   79,    2, 0x0a,    9 /* Public */,
      11,    1,   82,    2, 0x0a,   11 /* Public */,
      12,    1,   85,    2, 0x0a,   13 /* Public */,
      13,    0,   88,    2, 0x0a,   15 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RegExtDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_meta_stringdata_RegExtDlg.offsetsAndSizes,
    qt_meta_data_RegExtDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RegExtDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RegExtDlg, std::true_type>,
        // method 'extensionAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'extensionRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAddFromLangClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRemoveExtClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCustomExtEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onLangSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onLangExtSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onRegisteredExtSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onCancelClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RegExtDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RegExtDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->extensionAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->extensionRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onAddFromLangClicked(); break;
        case 3: _t->onRemoveExtClicked(); break;
        case 4: _t->onCustomExtEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onLangSelectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onLangExtSelectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->onRegisteredExtSelectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onCancelClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RegExtDlg::*)(const QString & );
            if (_t _q_method = &RegExtDlg::extensionAdded; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RegExtDlg::*)(const QString & );
            if (_t _q_method = &RegExtDlg::extensionRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *RegExtDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegExtDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegExtDlg.stringdata0))
        return static_cast<void*>(this);
    return StaticDialog::qt_metacast(_clname);
}

int RegExtDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StaticDialog::qt_metacall(_c, _id, _a);
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
void RegExtDlg::extensionAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RegExtDlg::extensionRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
