/****************************************************************************
** Meta object code from reading C++ file 'WindowsDlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/WindowsDlg.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WindowsDlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_WindowsDlg_t {
    uint offsetsAndSizes[38];
    char stringdata0[11];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[12];
    char stringdata5[13];
    char stringdata6[11];
    char stringdata7[8];
    char stringdata8[13];
    char stringdata9[9];
    char stringdata10[15];
    char stringdata11[11];
    char stringdata12[7];
    char stringdata13[8];
    char stringdata14[7];
    char stringdata15[14];
    char stringdata16[4];
    char stringdata17[11];
    char stringdata18[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_WindowsDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_WindowsDlg_t qt_meta_stringdata_WindowsDlg = {
    {
        QT_MOC_LITERAL(0, 10),  // "WindowsDlg"
        QT_MOC_LITERAL(11, 15),  // "windowActivated"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 5),  // "index"
        QT_MOC_LITERAL(34, 11),  // "windowSaved"
        QT_MOC_LITERAL(46, 12),  // "windowClosed"
        QT_MOC_LITERAL(59, 10),  // "QList<int>"
        QT_MOC_LITERAL(70, 7),  // "indices"
        QT_MOC_LITERAL(78, 12),  // "windowSorted"
        QT_MOC_LITERAL(91, 8),  // "newOrder"
        QT_MOC_LITERAL(100, 14),  // "closeRequested"
        QT_MOC_LITERAL(115, 10),  // "onActivate"
        QT_MOC_LITERAL(126, 6),  // "onSave"
        QT_MOC_LITERAL(133, 7),  // "onClose"
        QT_MOC_LITERAL(141, 6),  // "onSort"
        QT_MOC_LITERAL(148, 13),  // "onContextMenu"
        QT_MOC_LITERAL(162, 3),  // "pos"
        QT_MOC_LITERAL(166, 10),  // "onCopyName"
        QT_MOC_LITERAL(177, 10)   // "onCopyPath"
    },
    "WindowsDlg",
    "windowActivated",
    "",
    "index",
    "windowSaved",
    "windowClosed",
    "QList<int>",
    "indices",
    "windowSorted",
    "newOrder",
    "closeRequested",
    "onActivate",
    "onSave",
    "onClose",
    "onSort",
    "onContextMenu",
    "pos",
    "onCopyName",
    "onCopyPath"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_WindowsDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       4,    1,   89,    2, 0x06,    3 /* Public */,
       5,    1,   92,    2, 0x06,    5 /* Public */,
       8,    1,   95,    2, 0x06,    7 /* Public */,
      10,    0,   98,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   99,    2, 0x0a,   10 /* Public */,
      12,    0,  100,    2, 0x0a,   11 /* Public */,
      13,    0,  101,    2, 0x0a,   12 /* Public */,
      14,    0,  102,    2, 0x0a,   13 /* Public */,
      15,    1,  103,    2, 0x0a,   14 /* Public */,
      17,    0,  106,    2, 0x0a,   16 /* Public */,
      18,    0,  107,    2, 0x0a,   17 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    9,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   16,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject WindowsDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_meta_stringdata_WindowsDlg.offsetsAndSizes,
    qt_meta_data_WindowsDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_WindowsDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WindowsDlg, std::true_type>,
        // method 'windowActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'windowSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'windowClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<int>, std::false_type>,
        // method 'windowSorted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<int>, std::false_type>,
        // method 'closeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onActivate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onContextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onCopyName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopyPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void WindowsDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WindowsDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->windowActivated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->windowSaved((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->windowClosed((*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[1]))); break;
        case 3: _t->windowSorted((*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[1]))); break;
        case 4: _t->closeRequested(); break;
        case 5: _t->onActivate(); break;
        case 6: _t->onSave(); break;
        case 7: _t->onClose(); break;
        case 8: _t->onSort(); break;
        case 9: _t->onContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 10: _t->onCopyName(); break;
        case 11: _t->onCopyPath(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WindowsDlg::*)(int );
            if (_t _q_method = &WindowsDlg::windowActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WindowsDlg::*)(int );
            if (_t _q_method = &WindowsDlg::windowSaved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WindowsDlg::*)(QVector<int> );
            if (_t _q_method = &WindowsDlg::windowClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WindowsDlg::*)(QVector<int> );
            if (_t _q_method = &WindowsDlg::windowSorted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WindowsDlg::*)();
            if (_t _q_method = &WindowsDlg::closeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *WindowsDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WindowsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WindowsDlg.stringdata0))
        return static_cast<void*>(this);
    return StaticDialog::qt_metacast(_clname);
}

int WindowsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StaticDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void WindowsDlg::windowActivated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WindowsDlg::windowSaved(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WindowsDlg::windowClosed(QVector<int> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WindowsDlg::windowSorted(QVector<int> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WindowsDlg::closeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
namespace {
struct qt_meta_stringdata_StringDlg_t {
    uint offsetsAndSizes[2];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_StringDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_StringDlg_t qt_meta_stringdata_StringDlg = {
    {
        QT_MOC_LITERAL(0, 9)   // "StringDlg"
    },
    "StringDlg"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_StringDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject StringDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_StringDlg.offsetsAndSizes,
    qt_meta_data_StringDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_StringDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<StringDlg, std::true_type>
    >,
    nullptr
} };

void StringDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *StringDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StringDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StringDlg.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int StringDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
