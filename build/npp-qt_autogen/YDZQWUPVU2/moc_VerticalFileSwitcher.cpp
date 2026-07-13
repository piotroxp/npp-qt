/****************************************************************************
** Meta object code from reading C++ file 'VerticalFileSwitcher.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/VerticalFileSwitcher.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VerticalFileSwitcher.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_VerticalFileSwitcherListView_t {
    uint offsetsAndSizes[20];
    char stringdata0[29];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[5];
    char stringdata5[11];
    char stringdata6[13];
    char stringdata7[7];
    char stringdata8[10];
    char stringdata9[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_VerticalFileSwitcherListView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_VerticalFileSwitcherListView_t qt_meta_stringdata_VerticalFileSwitcherListView = {
    {
        QT_MOC_LITERAL(0, 28),  // "VerticalFileSwitcherListView"
        QT_MOC_LITERAL(29, 13),  // "fileActivated"
        QT_MOC_LITERAL(43, 0),  // ""
        QT_MOC_LITERAL(44, 8),  // "bufferID"
        QT_MOC_LITERAL(53, 4),  // "view"
        QT_MOC_LITERAL(58, 10),  // "fileClosed"
        QT_MOC_LITERAL(69, 12),  // "columnSorted"
        QT_MOC_LITERAL(82, 6),  // "column"
        QT_MOC_LITERAL(89, 9),  // "direction"
        QT_MOC_LITERAL(99, 19)   // "middleButtonClicked"
    },
    "VerticalFileSwitcherListView",
    "fileActivated",
    "",
    "bufferID",
    "view",
    "fileClosed",
    "columnSorted",
    "column",
    "direction",
    "middleButtonClicked"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_VerticalFileSwitcherListView[] = {

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
       1,    2,   38,    2, 0x06,    1 /* Public */,
       5,    2,   43,    2, 0x06,    4 /* Public */,
       6,    2,   48,    2, 0x06,    7 /* Public */,
       9,    2,   53,    2, 0x06,   10 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject VerticalFileSwitcherListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_meta_stringdata_VerticalFileSwitcherListView.offsetsAndSizes,
    qt_meta_data_VerticalFileSwitcherListView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_VerticalFileSwitcherListView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<VerticalFileSwitcherListView, std::true_type>,
        // method 'fileActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fileClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'columnSorted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'middleButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void VerticalFileSwitcherListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VerticalFileSwitcherListView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileActivated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->fileClosed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->columnSorted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->middleButtonClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VerticalFileSwitcherListView::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcherListView::fileActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VerticalFileSwitcherListView::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcherListView::fileClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VerticalFileSwitcherListView::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcherListView::columnSorted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (VerticalFileSwitcherListView::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcherListView::middleButtonClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *VerticalFileSwitcherListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VerticalFileSwitcherListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VerticalFileSwitcherListView.stringdata0))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int VerticalFileSwitcherListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
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
void VerticalFileSwitcherListView::fileActivated(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VerticalFileSwitcherListView::fileClosed(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VerticalFileSwitcherListView::columnSorted(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VerticalFileSwitcherListView::middleButtonClicked(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
namespace {
struct qt_meta_stringdata_VerticalFileSwitcher_t {
    uint offsetsAndSizes[12];
    char stringdata0[21];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[5];
    char stringdata5[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_VerticalFileSwitcher_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_VerticalFileSwitcher_t qt_meta_stringdata_VerticalFileSwitcher = {
    {
        QT_MOC_LITERAL(0, 20),  // "VerticalFileSwitcher"
        QT_MOC_LITERAL(21, 13),  // "fileActivated"
        QT_MOC_LITERAL(35, 0),  // ""
        QT_MOC_LITERAL(36, 8),  // "bufferID"
        QT_MOC_LITERAL(45, 4),  // "view"
        QT_MOC_LITERAL(50, 10)   // "fileClosed"
    },
    "VerticalFileSwitcher",
    "fileActivated",
    "",
    "bufferID",
    "view",
    "fileClosed"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_VerticalFileSwitcher[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   26,    2, 0x06,    1 /* Public */,
       5,    2,   31,    2, 0x06,    4 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject VerticalFileSwitcher::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_meta_stringdata_VerticalFileSwitcher.offsetsAndSizes,
    qt_meta_data_VerticalFileSwitcher,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_VerticalFileSwitcher_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<VerticalFileSwitcher, std::true_type>,
        // method 'fileActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fileClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void VerticalFileSwitcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VerticalFileSwitcher *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileActivated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->fileClosed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VerticalFileSwitcher::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcher::fileActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VerticalFileSwitcher::*)(int , int );
            if (_t _q_method = &VerticalFileSwitcher::fileClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *VerticalFileSwitcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VerticalFileSwitcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VerticalFileSwitcher.stringdata0))
        return static_cast<void*>(this);
    return DockingDlgInterface::qt_metacast(_clname);
}

int VerticalFileSwitcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockingDlgInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void VerticalFileSwitcher::fileActivated(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VerticalFileSwitcher::fileClosed(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
