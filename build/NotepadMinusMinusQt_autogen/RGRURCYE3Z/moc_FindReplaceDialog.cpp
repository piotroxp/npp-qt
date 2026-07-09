/****************************************************************************
** Meta object code from reading C++ file 'FindReplaceDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/dialogs/FindReplaceDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FindReplaceDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FindReplaceDialog_t {
    uint offsetsAndSizes[24];
    char stringdata0[18];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[11];
    char stringdata5[8];
    char stringdata6[17];
    char stringdata7[5];
    char stringdata8[8];
    char stringdata9[11];
    char stringdata10[10];
    char stringdata11[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FindReplaceDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FindReplaceDialog_t qt_meta_stringdata_FindReplaceDialog = {
    {
        QT_MOC_LITERAL(0, 17),  // "FindReplaceDialog"
        QT_MOC_LITERAL(18, 17),  // "findNextRequested"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 4),  // "text"
        QT_MOC_LITERAL(42, 10),  // "FindOption"
        QT_MOC_LITERAL(53, 7),  // "options"
        QT_MOC_LITERAL(61, 16),  // "replaceRequested"
        QT_MOC_LITERAL(78, 4),  // "find"
        QT_MOC_LITERAL(83, 7),  // "replace"
        QT_MOC_LITERAL(91, 10),  // "onFindNext"
        QT_MOC_LITERAL(102, 9),  // "onReplace"
        QT_MOC_LITERAL(112, 12)   // "onReplaceAll"
    },
    "FindReplaceDialog",
    "findNextRequested",
    "",
    "text",
    "FindOption",
    "options",
    "replaceRequested",
    "find",
    "replace",
    "onFindNext",
    "onReplace",
    "onReplaceAll"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FindReplaceDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   44,    2, 0x06,    1 /* Public */,
       6,    3,   49,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   56,    2, 0x08,    8 /* Private */,
      10,    0,   57,    2, 0x08,    9 /* Private */,
      11,    0,   58,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 4,    7,    8,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FindReplaceDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FindReplaceDialog.offsetsAndSizes,
    qt_meta_data_FindReplaceDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FindReplaceDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FindReplaceDialog, std::true_type>,
        // method 'findNextRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<FindOption, std::false_type>,
        // method 'replaceRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<FindOption, std::false_type>,
        // method 'onFindNext'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplaceAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FindReplaceDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FindReplaceDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->findNextRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<FindOption>>(_a[2]))); break;
        case 1: _t->replaceRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<FindOption>>(_a[3]))); break;
        case 2: _t->onFindNext(); break;
        case 3: _t->onReplace(); break;
        case 4: _t->onReplaceAll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FindReplaceDialog::*)(const QString & , FindOption );
            if (_t _q_method = &FindReplaceDialog::findNextRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FindReplaceDialog::*)(const QString & , const QString & , FindOption );
            if (_t _q_method = &FindReplaceDialog::replaceRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *FindReplaceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FindReplaceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FindReplaceDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FindReplaceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void FindReplaceDialog::findNextRequested(const QString & _t1, FindOption _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FindReplaceDialog::replaceRequested(const QString & _t1, const QString & _t2, FindOption _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
