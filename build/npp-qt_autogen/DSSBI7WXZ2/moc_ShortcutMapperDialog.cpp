/****************************************************************************
** Meta object code from reading C++ file 'ShortcutMapperDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/Dialogs/ShortcutMapperDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShortcutMapperDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_ShortcutMapperDialog_t {
    uint offsetsAndSizes[30];
    char stringdata0[21];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[20];
    char stringdata5[5];
    char stringdata6[20];
    char stringdata7[17];
    char stringdata8[5];
    char stringdata9[7];
    char stringdata10[16];
    char stringdata11[11];
    char stringdata12[8];
    char stringdata13[5];
    char stringdata14[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ShortcutMapperDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ShortcutMapperDialog_t qt_meta_stringdata_ShortcutMapperDialog = {
    {
        QT_MOC_LITERAL(0, 20),  // "ShortcutMapperDialog"
        QT_MOC_LITERAL(21, 23),  // "onCategoryFilterChanged"
        QT_MOC_LITERAL(45, 0),  // ""
        QT_MOC_LITERAL(46, 5),  // "index"
        QT_MOC_LITERAL(52, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(72, 4),  // "text"
        QT_MOC_LITERAL(77, 19),  // "onItemDoubleClicked"
        QT_MOC_LITERAL(97, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(114, 4),  // "item"
        QT_MOC_LITERAL(119, 6),  // "column"
        QT_MOC_LITERAL(126, 15),  // "onClearShortcut"
        QT_MOC_LITERAL(142, 10),  // "onResetAll"
        QT_MOC_LITERAL(153, 7),  // "onApply"
        QT_MOC_LITERAL(161, 4),  // "onOk"
        QT_MOC_LITERAL(166, 8)   // "onCancel"
    },
    "ShortcutMapperDialog",
    "onCategoryFilterChanged",
    "",
    "index",
    "onSearchTextChanged",
    "text",
    "onItemDoubleClicked",
    "QTreeWidgetItem*",
    "item",
    "column",
    "onClearShortcut",
    "onResetAll",
    "onApply",
    "onOk",
    "onCancel"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ShortcutMapperDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x08,    1 /* Private */,
       4,    1,   65,    2, 0x08,    3 /* Private */,
       6,    2,   68,    2, 0x08,    5 /* Private */,
      10,    0,   73,    2, 0x08,    8 /* Private */,
      11,    0,   74,    2, 0x08,    9 /* Private */,
      12,    0,   75,    2, 0x08,   10 /* Private */,
      13,    0,   76,    2, 0x08,   11 /* Private */,
      14,    0,   77,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ShortcutMapperDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ShortcutMapperDialog.offsetsAndSizes,
    qt_meta_data_ShortcutMapperDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ShortcutMapperDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ShortcutMapperDialog, std::true_type>,
        // method 'onCategoryFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onClearShortcut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onResetAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onApply'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ShortcutMapperDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ShortcutMapperDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCategoryFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->onClearShortcut(); break;
        case 4: _t->onResetAll(); break;
        case 5: _t->onApply(); break;
        case 6: _t->onOk(); break;
        case 7: _t->onCancel(); break;
        default: ;
        }
    }
}

const QMetaObject *ShortcutMapperDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShortcutMapperDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ShortcutMapperDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ShortcutMapperDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
