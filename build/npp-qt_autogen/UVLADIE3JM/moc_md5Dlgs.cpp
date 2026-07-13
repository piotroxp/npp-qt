/****************************************************************************
** Meta object code from reading C++ file 'md5Dlgs.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/md5Dlgs.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'md5Dlgs.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MD5FromTextDlg_t {
    uint offsetsAndSizes[10];
    char stringdata0[15];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MD5FromTextDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MD5FromTextDlg_t qt_meta_stringdata_MD5FromTextDlg = {
    {
        QT_MOC_LITERAL(0, 14),  // "MD5FromTextDlg"
        QT_MOC_LITERAL(15, 9),  // "onCompute"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 17),  // "onCopyToClipboard"
        QT_MOC_LITERAL(44, 7)   // "onClose"
    },
    "MD5FromTextDlg",
    "onCompute",
    "",
    "onCopyToClipboard",
    "onClose"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MD5FromTextDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x08,    1 /* Private */,
       3,    0,   33,    2, 0x08,    2 /* Private */,
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MD5FromTextDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_MD5FromTextDlg.offsetsAndSizes,
    qt_meta_data_MD5FromTextDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MD5FromTextDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MD5FromTextDlg, std::true_type>,
        // method 'onCompute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopyToClipboard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MD5FromTextDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MD5FromTextDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCompute(); break;
        case 1: _t->onCopyToClipboard(); break;
        case 2: _t->onClose(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *MD5FromTextDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MD5FromTextDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MD5FromTextDlg.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int MD5FromTextDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
namespace {
struct qt_meta_stringdata_MD5FromFilesDlg_t {
    uint offsetsAndSizes[12];
    char stringdata0[16];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[18];
    char stringdata5[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MD5FromFilesDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MD5FromFilesDlg_t qt_meta_stringdata_MD5FromFilesDlg = {
    {
        QT_MOC_LITERAL(0, 15),  // "MD5FromFilesDlg"
        QT_MOC_LITERAL(16, 8),  // "onBrowse"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 9),  // "onCompute"
        QT_MOC_LITERAL(36, 17),  // "onCopyToClipboard"
        QT_MOC_LITERAL(54, 7)   // "onClose"
    },
    "MD5FromFilesDlg",
    "onBrowse",
    "",
    "onCompute",
    "onCopyToClipboard",
    "onClose"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MD5FromFilesDlg[] = {

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
       5,    0,   41,    2, 0x08,    4 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MD5FromFilesDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_MD5FromFilesDlg.offsetsAndSizes,
    qt_meta_data_MD5FromFilesDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MD5FromFilesDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MD5FromFilesDlg, std::true_type>,
        // method 'onBrowse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCompute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopyToClipboard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MD5FromFilesDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MD5FromFilesDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBrowse(); break;
        case 1: _t->onCompute(); break;
        case 2: _t->onCopyToClipboard(); break;
        case 3: _t->onClose(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *MD5FromFilesDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MD5FromFilesDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MD5FromFilesDlg.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int MD5FromFilesDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
