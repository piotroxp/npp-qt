/****************************************************************************
** Meta object code from reading C++ file 'DockingWnd.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/DockingWnd.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DockingWnd.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DockingDlgInterface_t {
    uint offsetsAndSizes[2];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DockingDlgInterface_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DockingDlgInterface_t qt_meta_stringdata_DockingDlgInterface = {
    {
        QT_MOC_LITERAL(0, 19)   // "DockingDlgInterface"
    },
    "DockingDlgInterface"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DockingDlgInterface[] = {

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

Q_CONSTINIT const QMetaObject DockingDlgInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DockingDlgInterface.offsetsAndSizes,
    qt_meta_data_DockingDlgInterface,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DockingDlgInterface_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DockingDlgInterface, std::true_type>
    >,
    nullptr
} };

void DockingDlgInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DockingDlgInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingDlgInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockingDlgInterface.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DockingDlgInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_DockingContainer_t {
    uint offsetsAndSizes[30];
    char stringdata0[17];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[7];
    char stringdata5[13];
    char stringdata6[11];
    char stringdata7[6];
    char stringdata8[15];
    char stringdata9[14];
    char stringdata10[20];
    char stringdata11[18];
    char stringdata12[13];
    char stringdata13[13];
    char stringdata14[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DockingContainer_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DockingContainer_t qt_meta_stringdata_DockingContainer = {
    {
        QT_MOC_LITERAL(0, 16),  // "DockingContainer"
        QT_MOC_LITERAL(17, 11),  // "widgetShown"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 8),  // "QWidget*"
        QT_MOC_LITERAL(39, 6),  // "client"
        QT_MOC_LITERAL(46, 12),  // "widgetHidden"
        QT_MOC_LITERAL(59, 10),  // "tabChanged"
        QT_MOC_LITERAL(70, 5),  // "index"
        QT_MOC_LITERAL(76, 14),  // "closeRequested"
        QT_MOC_LITERAL(91, 13),  // "moveRequested"
        QT_MOC_LITERAL(105, 19),  // "dockToggleRequested"
        QT_MOC_LITERAL(125, 17),  // "floatAllRequested"
        QT_MOC_LITERAL(143, 12),  // "closeAllTabs"
        QT_MOC_LITERAL(156, 12),  // "onTabChanged"
        QT_MOC_LITERAL(169, 19)   // "onTabCloseRequested"
    },
    "DockingContainer",
    "widgetShown",
    "",
    "QWidget*",
    "client",
    "widgetHidden",
    "tabChanged",
    "index",
    "closeRequested",
    "moveRequested",
    "dockToggleRequested",
    "floatAllRequested",
    "closeAllTabs",
    "onTabChanged",
    "onTabCloseRequested"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DockingContainer[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       5,    1,   77,    2, 0x06,    3 /* Public */,
       6,    1,   80,    2, 0x06,    5 /* Public */,
       8,    0,   83,    2, 0x06,    7 /* Public */,
       9,    0,   84,    2, 0x06,    8 /* Public */,
      10,    0,   85,    2, 0x06,    9 /* Public */,
      11,    0,   86,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   87,    2, 0x0a,   11 /* Public */,
      13,    1,   88,    2, 0x08,   12 /* Private */,
      14,    1,   91,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

Q_CONSTINIT const QMetaObject DockingContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DockingContainer.offsetsAndSizes,
    qt_meta_data_DockingContainer,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DockingContainer_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DockingContainer, std::true_type>,
        // method 'widgetShown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        // method 'widgetHidden'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        // method 'tabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'closeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'moveRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dockToggleRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'floatAllRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closeAllTabs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTabCloseRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void DockingContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DockingContainer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->widgetShown((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1]))); break;
        case 1: _t->widgetHidden((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1]))); break;
        case 2: _t->tabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->closeRequested(); break;
        case 4: _t->moveRequested(); break;
        case 5: _t->dockToggleRequested(); break;
        case 6: _t->floatAllRequested(); break;
        case 7: _t->closeAllTabs(); break;
        case 8: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onTabCloseRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DockingContainer::*)(QWidget * );
            if (_t _q_method = &DockingContainer::widgetShown; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)(QWidget * );
            if (_t _q_method = &DockingContainer::widgetHidden; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)(int );
            if (_t _q_method = &DockingContainer::tabChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)();
            if (_t _q_method = &DockingContainer::closeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)();
            if (_t _q_method = &DockingContainer::moveRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)();
            if (_t _q_method = &DockingContainer::dockToggleRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (DockingContainer::*)();
            if (_t _q_method = &DockingContainer::floatAllRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject *DockingContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockingContainer.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DockingContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DockingContainer::widgetShown(QWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DockingContainer::widgetHidden(QWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DockingContainer::tabChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DockingContainer::closeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DockingContainer::moveRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DockingContainer::dockToggleRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DockingContainer::floatAllRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
namespace {
struct qt_meta_stringdata_DockingSplitter_t {
    uint offsetsAndSizes[8];
    char stringdata0[16];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DockingSplitter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DockingSplitter_t qt_meta_stringdata_DockingSplitter = {
    {
        QT_MOC_LITERAL(0, 15),  // "DockingSplitter"
        QT_MOC_LITERAL(16, 13),  // "splitterMoved"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 7)   // "newSize"
    },
    "DockingSplitter",
    "splitterMoved",
    "",
    "newSize"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DockingSplitter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject DockingSplitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DockingSplitter.offsetsAndSizes,
    qt_meta_data_DockingSplitter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DockingSplitter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DockingSplitter, std::true_type>,
        // method 'splitterMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void DockingSplitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DockingSplitter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->splitterMoved((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DockingSplitter::*)(int );
            if (_t _q_method = &DockingSplitter::splitterMoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *DockingSplitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingSplitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockingSplitter.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DockingSplitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DockingSplitter::splitterMoved(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {
struct qt_meta_stringdata_DockingManager_t {
    uint offsetsAndSizes[72];
    char stringdata0[15];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[5];
    char stringdata5[21];
    char stringdata6[17];
    char stringdata7[5];
    char stringdata8[10];
    char stringdata9[8];
    char stringdata10[19];
    char stringdata11[9];
    char stringdata12[4];
    char stringdata13[25];
    char stringdata14[5];
    char stringdata15[13];
    char stringdata16[6];
    char stringdata17[23];
    char stringdata18[5];
    char stringdata19[18];
    char stringdata20[20];
    char stringdata21[18];
    char stringdata22[16];
    char stringdata23[13];
    char stringdata24[17];
    char stringdata25[31];
    char stringdata26[7];
    char stringdata27[15];
    char stringdata28[4];
    char stringdata29[4];
    char stringdata30[12];
    char stringdata31[14];
    char stringdata32[7];
    char stringdata33[6];
    char stringdata34[14];
    char stringdata35[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DockingManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DockingManager_t qt_meta_stringdata_DockingManager = {
    {
        QT_MOC_LITERAL(0, 14),  // "DockingManager"
        QT_MOC_LITERAL(15, 15),  // "dockInfoChanged"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 22),  // "showFloatingContainers"
        QT_MOC_LITERAL(55, 4),  // "show"
        QT_MOC_LITERAL(60, 20),  // "createDockableDialog"
        QT_MOC_LITERAL(81, 16),  // "DockedWidgetData"
        QT_MOC_LITERAL(98, 4),  // "data"
        QT_MOC_LITERAL(103, 9),  // "container"
        QT_MOC_LITERAL(113, 7),  // "visible"
        QT_MOC_LITERAL(121, 18),  // "showDockableDialog"
        QT_MOC_LITERAL(140, 8),  // "QWidget*"
        QT_MOC_LITERAL(149, 3),  // "dlg"
        QT_MOC_LITERAL(153, 24),  // "showDockableDialogByName"
        QT_MOC_LITERAL(178, 4),  // "name"
        QT_MOC_LITERAL(183, 12),  // "setActiveTab"
        QT_MOC_LITERAL(196, 5),  // "index"
        QT_MOC_LITERAL(202, 22),  // "setDockedContainerSize"
        QT_MOC_LITERAL(225, 4),  // "size"
        QT_MOC_LITERAL(230, 17),  // "setDockedContSize"
        QT_MOC_LITERAL(248, 19),  // "dockedContainerSize"
        QT_MOC_LITERAL(268, 17),  // "getDockedContSize"
        QT_MOC_LITERAL(286, 15),  // "setStyleCaption"
        QT_MOC_LITERAL(302, 12),  // "captionOnTop"
        QT_MOC_LITERAL(315, 16),  // "getContainerInfo"
        QT_MOC_LITERAL(332, 30),  // "std::vector<DockingContainer*>"
        QT_MOC_LITERAL(363, 6),  // "resize"
        QT_MOC_LITERAL(370, 14),  // "onFocusChanged"
        QT_MOC_LITERAL(385, 3),  // "old"
        QT_MOC_LITERAL(389, 3),  // "now"
        QT_MOC_LITERAL(393, 11),  // "focusClient"
        QT_MOC_LITERAL(405, 13),  // "doesContExist"
        QT_MOC_LITERAL(419, 6),  // "size_t"
        QT_MOC_LITERAL(426, 5),  // "iCont"
        QT_MOC_LITERAL(432, 13),  // "isInitialized"
        QT_MOC_LITERAL(446, 12)   // "isRegistered"
    },
    "DockingManager",
    "dockInfoChanged",
    "",
    "showFloatingContainers",
    "show",
    "createDockableDialog",
    "DockedWidgetData",
    "data",
    "container",
    "visible",
    "showDockableDialog",
    "QWidget*",
    "dlg",
    "showDockableDialogByName",
    "name",
    "setActiveTab",
    "index",
    "setDockedContainerSize",
    "size",
    "setDockedContSize",
    "dockedContainerSize",
    "getDockedContSize",
    "setStyleCaption",
    "captionOnTop",
    "getContainerInfo",
    "std::vector<DockingContainer*>",
    "resize",
    "onFocusChanged",
    "old",
    "now",
    "focusClient",
    "doesContExist",
    "size_t",
    "iCont",
    "isInitialized",
    "isRegistered"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DockingManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  134,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,  135,    2, 0x0a,    2 /* Public */,
       5,    3,  138,    2, 0x0a,    4 /* Public */,
       5,    2,  145,    2, 0x2a,    8 /* Public | MethodCloned */,
       5,    1,  150,    2, 0x2a,   11 /* Public | MethodCloned */,
      10,    2,  153,    2, 0x0a,   13 /* Public */,
      13,    2,  158,    2, 0x0a,   16 /* Public */,
      15,    2,  163,    2, 0x0a,   19 /* Public */,
      17,    2,  168,    2, 0x0a,   22 /* Public */,
      19,    2,  173,    2, 0x0a,   25 /* Public */,
      20,    1,  178,    2, 0x10a,   28 /* Public | MethodIsConst  */,
      21,    1,  181,    2, 0x10a,   30 /* Public | MethodIsConst  */,
      22,    1,  184,    2, 0x0a,   32 /* Public */,
      24,    0,  187,    2, 0x10a,   34 /* Public | MethodIsConst  */,
      26,    0,  188,    2, 0x0a,   35 /* Public */,
      27,    2,  189,    2, 0x0a,   36 /* Public */,
      30,    0,  194,    2, 0x0a,   39 /* Public */,
      31,    1,  195,    2, 0x10a,   40 /* Public | MethodIsConst  */,
      34,    0,  198,    2, 0x10a,   42 /* Public | MethodIsConst  */,
      35,    0,  199,    2, 0x0a,   43 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int, QMetaType::Bool,    7,    8,    9,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int,    7,    8,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Bool,   12,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   14,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,   16,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,   18,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,   18,
    QMetaType::Int, QMetaType::Int,    8,
    QMetaType::Int, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Bool,   23,
    0x80000000 | 25,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 11,   28,   29,
    0x80000000 | 11,
    QMetaType::Bool, 0x80000000 | 32,   33,
    QMetaType::Bool,
    QMetaType::Bool,

       0        // eod
};

Q_CONSTINIT const QMetaObject DockingManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DockingManager.offsetsAndSizes,
    qt_meta_data_DockingManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DockingManager_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DockingManager, std::true_type>,
        // method 'dockInfoChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showFloatingContainers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'createDockableDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<DockedWidgetData, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'createDockableDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<DockedWidgetData, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createDockableDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<DockedWidgetData, std::false_type>,
        // method 'showDockableDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'showDockableDialogByName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setActiveTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setDockedContainerSize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setDockedContSize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'dockedContainerSize'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getDockedContSize'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setStyleCaption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'getContainerInfo'
        QtPrivate::TypeAndForceComplete<std::vector<DockingContainer*>, std::false_type>,
        // method 'resize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFocusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        // method 'focusClient'
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        // method 'doesContExist'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'isInitialized'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'isRegistered'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void DockingManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DockingManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dockInfoChanged(); break;
        case 1: _t->showFloatingContainers((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->createDockableDialog((*reinterpret_cast< std::add_pointer_t<DockedWidgetData>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 3: _t->createDockableDialog((*reinterpret_cast< std::add_pointer_t<DockedWidgetData>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->createDockableDialog((*reinterpret_cast< std::add_pointer_t<DockedWidgetData>>(_a[1]))); break;
        case 5: _t->showDockableDialog((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->showDockableDialogByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->setActiveTab((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->setDockedContainerSize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 9: _t->setDockedContSize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 10: { int _r = _t->dockedContainerSize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 11: { int _r = _t->getDockedContSize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->setStyleCaption((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: { std::vector<DockingContainer*> _r = _t->getContainerInfo();
            if (_a[0]) *reinterpret_cast< std::vector<DockingContainer*>*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->resize(); break;
        case 15: _t->onFocusChanged((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[2]))); break;
        case 16: { QWidget* _r = _t->focusClient();
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->doesContExist((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->isInitialized();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 19: { bool _r = _t->isRegistered();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DockingManager::*)();
            if (_t _q_method = &DockingManager::dockInfoChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *DockingManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockingManager.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DockingManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void DockingManager::dockInfoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
