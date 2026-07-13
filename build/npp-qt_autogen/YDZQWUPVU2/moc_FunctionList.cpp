/****************************************************************************
** Meta object code from reading C++ file 'FunctionList.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/FunctionList.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FunctionList.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FunctionListTreeWidget_t {
    uint offsetsAndSizes[12];
    char stringdata0[23];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[5];
    char stringdata5[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FunctionListTreeWidget_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FunctionListTreeWidget_t qt_meta_stringdata_FunctionListTreeWidget = {
    {
        QT_MOC_LITERAL(0, 22),  // "FunctionListTreeWidget"
        QT_MOC_LITERAL(23, 16),  // "itemExpandToggle"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(58, 4),  // "item"
        QT_MOC_LITERAL(63, 18)   // "labelEditRequested"
    },
    "FunctionListTreeWidget",
    "itemExpandToggle",
    "",
    "QTreeWidgetItem*",
    "item",
    "labelEditRequested"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FunctionListTreeWidget[] = {

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
       1,    1,   26,    2, 0x06,    1 /* Public */,
       5,    1,   29,    2, 0x06,    3 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject FunctionListTreeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTreeWidget::staticMetaObject>(),
    qt_meta_stringdata_FunctionListTreeWidget.offsetsAndSizes,
    qt_meta_data_FunctionListTreeWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FunctionListTreeWidget_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FunctionListTreeWidget, std::true_type>,
        // method 'itemExpandToggle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        // method 'labelEditRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>
    >,
    nullptr
} };

void FunctionListTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FunctionListTreeWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->itemExpandToggle((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 1: _t->labelEditRequested((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FunctionListTreeWidget::*)(QTreeWidgetItem * );
            if (_t _q_method = &FunctionListTreeWidget::itemExpandToggle; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FunctionListTreeWidget::*)(QTreeWidgetItem * );
            if (_t _q_method = &FunctionListTreeWidget::labelEditRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *FunctionListTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FunctionListTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FunctionListTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int FunctionListTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
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
void FunctionListTreeWidget::itemExpandToggle(QTreeWidgetItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FunctionListTreeWidget::labelEditRequested(QTreeWidgetItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
namespace {
struct qt_meta_stringdata_FunctionListPanel_t {
    uint offsetsAndSizes[28];
    char stringdata0[18];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[9];
    char stringdata5[16];
    char stringdata6[14];
    char stringdata7[16];
    char stringdata8[21];
    char stringdata9[20];
    char stringdata10[20];
    char stringdata11[5];
    char stringdata12[18];
    char stringdata13[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FunctionListPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FunctionListPanel_t qt_meta_stringdata_FunctionListPanel = {
    {
        QT_MOC_LITERAL(0, 17),  // "FunctionListPanel"
        QT_MOC_LITERAL(18, 14),  // "closeRequested"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 16),  // "functionSelected"
        QT_MOC_LITERAL(51, 8),  // "position"
        QT_MOC_LITERAL(60, 15),  // "reloadRequested"
        QT_MOC_LITERAL(76, 13),  // "onSortToggled"
        QT_MOC_LITERAL(90, 15),  // "onReloadClicked"
        QT_MOC_LITERAL(106, 20),  // "onPreferencesClicked"
        QT_MOC_LITERAL(127, 19),  // "onInitialSortAction"
        QT_MOC_LITERAL(147, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(167, 4),  // "text"
        QT_MOC_LITERAL(172, 17),  // "onEditorUiChanged"
        QT_MOC_LITERAL(190, 7)   // "cleanup"
    },
    "FunctionListPanel",
    "closeRequested",
    "",
    "functionSelected",
    "position",
    "reloadRequested",
    "onSortToggled",
    "onReloadClicked",
    "onPreferencesClicked",
    "onInitialSortAction",
    "onSearchTextChanged",
    "text",
    "onEditorUiChanged",
    "cleanup"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FunctionListPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,
       3,    1,   75,    2, 0x06,    2 /* Public */,
       5,    0,   78,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   79,    2, 0x0a,    5 /* Public */,
       7,    0,   80,    2, 0x0a,    6 /* Public */,
       8,    0,   81,    2, 0x0a,    7 /* Public */,
       9,    0,   82,    2, 0x0a,    8 /* Public */,
      10,    1,   83,    2, 0x0a,    9 /* Public */,
      12,    0,   86,    2, 0x0a,   11 /* Public */,
      13,    0,   87,    2, 0x0a,   12 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FunctionListPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_meta_stringdata_FunctionListPanel.offsetsAndSizes,
    qt_meta_data_FunctionListPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FunctionListPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FunctionListPanel, std::true_type>,
        // method 'closeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'functionSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'reloadRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSortToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReloadClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPreferencesClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onInitialSortAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onEditorUiChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cleanup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FunctionListPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FunctionListPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->closeRequested(); break;
        case 1: _t->functionSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->reloadRequested(); break;
        case 3: _t->onSortToggled(); break;
        case 4: _t->onReloadClicked(); break;
        case 5: _t->onPreferencesClicked(); break;
        case 6: _t->onInitialSortAction(); break;
        case 7: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onEditorUiChanged(); break;
        case 9: _t->cleanup(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FunctionListPanel::*)();
            if (_t _q_method = &FunctionListPanel::closeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FunctionListPanel::*)(int );
            if (_t _q_method = &FunctionListPanel::functionSelected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FunctionListPanel::*)();
            if (_t _q_method = &FunctionListPanel::reloadRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *FunctionListPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FunctionListPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FunctionListPanel.stringdata0))
        return static_cast<void*>(this);
    return DockingDlgInterface::qt_metacast(_clname);
}

int FunctionListPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockingDlgInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void FunctionListPanel::closeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FunctionListPanel::functionSelected(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FunctionListPanel::reloadRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
