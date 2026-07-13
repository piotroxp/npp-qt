/****************************************************************************
** Meta object code from reading C++ file 'ClipboardHistory.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/ClipboardHistory.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ClipboardHistory.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_ClipboardListDelegate_t {
    uint offsetsAndSizes[2];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ClipboardListDelegate_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ClipboardListDelegate_t qt_meta_stringdata_ClipboardListDelegate = {
    {
        QT_MOC_LITERAL(0, 21)   // "ClipboardListDelegate"
    },
    "ClipboardListDelegate"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ClipboardListDelegate[] = {

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

Q_CONSTINIT const QMetaObject ClipboardListDelegate::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractItemDelegate::staticMetaObject>(),
    qt_meta_stringdata_ClipboardListDelegate.offsetsAndSizes,
    qt_meta_data_ClipboardListDelegate,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ClipboardListDelegate_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ClipboardListDelegate, std::true_type>
    >,
    nullptr
} };

void ClipboardListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ClipboardListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClipboardListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClipboardListDelegate.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemDelegate::qt_metacast(_clname);
}

int ClipboardListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ClipboardHistoryPanel_t {
    uint offsetsAndSizes[26];
    char stringdata0[22];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[14];
    char stringdata5[19];
    char stringdata6[17];
    char stringdata7[5];
    char stringdata8[20];
    char stringdata9[17];
    char stringdata10[5];
    char stringdata11[14];
    char stringdata12[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ClipboardHistoryPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ClipboardHistoryPanel_t qt_meta_stringdata_ClipboardHistoryPanel = {
    {
        QT_MOC_LITERAL(0, 21),  // "ClipboardHistoryPanel"
        QT_MOC_LITERAL(22, 17),  // "itemDoubleClicked"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 5),  // "index"
        QT_MOC_LITERAL(47, 13),  // "itemActivated"
        QT_MOC_LITERAL(61, 18),  // "onClipboardChanged"
        QT_MOC_LITERAL(80, 16),  // "QClipboard::Mode"
        QT_MOC_LITERAL(97, 4),  // "mode"
        QT_MOC_LITERAL(102, 19),  // "onItemDoubleClicked"
        QT_MOC_LITERAL(122, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(139, 4),  // "item"
        QT_MOC_LITERAL(144, 13),  // "onItemClicked"
        QT_MOC_LITERAL(158, 15)   // "onItemActivated"
    },
    "ClipboardHistoryPanel",
    "itemDoubleClicked",
    "",
    "index",
    "itemActivated",
    "onClipboardChanged",
    "QClipboard::Mode",
    "mode",
    "onItemDoubleClicked",
    "QListWidgetItem*",
    "item",
    "onItemClicked",
    "onItemActivated"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ClipboardHistoryPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       4,    1,   53,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   56,    2, 0x08,    5 /* Private */,
       8,    1,   59,    2, 0x08,    7 /* Private */,
      11,    1,   62,    2, 0x08,    9 /* Private */,
      12,    1,   65,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject ClipboardHistoryPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_meta_stringdata_ClipboardHistoryPanel.offsetsAndSizes,
    qt_meta_data_ClipboardHistoryPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ClipboardHistoryPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ClipboardHistoryPanel, std::true_type>,
        // method 'itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'itemActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onClipboardChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QClipboard::Mode, std::false_type>,
        // method 'onItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onItemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onItemActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>
    >,
    nullptr
} };

void ClipboardHistoryPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ClipboardHistoryPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->itemActivated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onClipboardChanged((*reinterpret_cast< std::add_pointer_t<QClipboard::Mode>>(_a[1]))); break;
        case 3: _t->onItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 4: _t->onItemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 5: _t->onItemActivated((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ClipboardHistoryPanel::*)(int );
            if (_t _q_method = &ClipboardHistoryPanel::itemDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ClipboardHistoryPanel::*)(int );
            if (_t _q_method = &ClipboardHistoryPanel::itemActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *ClipboardHistoryPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClipboardHistoryPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClipboardHistoryPanel.stringdata0))
        return static_cast<void*>(this);
    return DockingDlgInterface::qt_metacast(_clname);
}

int ClipboardHistoryPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockingDlgInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ClipboardHistoryPanel::itemDoubleClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClipboardHistoryPanel::itemActivated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
