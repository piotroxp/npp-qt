/****************************************************************************
** Meta object code from reading C++ file 'DocumentMapPanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../panels/DocumentMapPanel.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentMapPanel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MapTextView_t {
    uint offsetsAndSizes[8];
    char stringdata0[12];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MapTextView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MapTextView_t qt_meta_stringdata_MapTextView = {
    {
        QT_MOC_LITERAL(0, 11),  // "MapTextView"
        QT_MOC_LITERAL(12, 15),  // "viewportClicked"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 3)   // "pos"
    },
    "MapTextView",
    "viewportClicked",
    "",
    "pos"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MapTextView[] = {

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
    QMetaType::Void, QMetaType::QPoint,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject MapTextView::staticMetaObject = { {
    QMetaObject::SuperData::link<QsciScintilla::staticMetaObject>(),
    qt_meta_stringdata_MapTextView.offsetsAndSizes,
    qt_meta_data_MapTextView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MapTextView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MapTextView, std::true_type>,
        // method 'viewportClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>
    >,
    nullptr
} };

void MapTextView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapTextView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->viewportClicked((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MapTextView::*)(const QPoint & );
            if (_t _q_method = &MapTextView::viewportClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *MapTextView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapTextView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MapTextView.stringdata0))
        return static_cast<void*>(this);
    return QsciScintilla::qt_metacast(_clname);
}

int MapTextView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QsciScintilla::qt_metacall(_c, _id, _a);
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
void MapTextView::viewportClicked(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {
struct qt_meta_stringdata_DocumentMapPanel_t {
    uint offsetsAndSizes[28];
    char stringdata0[17];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[9];
    char stringdata5[16];
    char stringdata6[12];
    char stringdata7[17];
    char stringdata8[4];
    char stringdata9[16];
    char stringdata10[15];
    char stringdata11[24];
    char stringdata12[5];
    char stringdata13[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DocumentMapPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DocumentMapPanel_t qt_meta_stringdata_DocumentMapPanel = {
    {
        QT_MOC_LITERAL(0, 16),  // "DocumentMapPanel"
        QT_MOC_LITERAL(17, 21),  // "scheduleMinimapUpdate"
        QT_MOC_LITERAL(39, 0),  // ""
        QT_MOC_LITERAL(40, 14),  // "onEditorScroll"
        QT_MOC_LITERAL(55, 8),  // "newValue"
        QT_MOC_LITERAL(64, 15),  // "onMiniMapScroll"
        QT_MOC_LITERAL(80, 11),  // "sliderValue"
        QT_MOC_LITERAL(92, 16),  // "onMiniMapClicked"
        QT_MOC_LITERAL(109, 3),  // "pos"
        QT_MOC_LITERAL(113, 15),  // "onViewportTimer"
        QT_MOC_LITERAL(129, 14),  // "onLinesChanged"
        QT_MOC_LITERAL(144, 23),  // "onCursorPositionChanged"
        QT_MOC_LITERAL(168, 4),  // "line"
        QT_MOC_LITERAL(173, 3)   // "col"
    },
    "DocumentMapPanel",
    "scheduleMinimapUpdate",
    "",
    "onEditorScroll",
    "newValue",
    "onMiniMapScroll",
    "sliderValue",
    "onMiniMapClicked",
    "pos",
    "onViewportTimer",
    "onLinesChanged",
    "onCursorPositionChanged",
    "line",
    "col"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DocumentMapPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x0a,    1 /* Public */,
       3,    1,   57,    2, 0x08,    2 /* Private */,
       5,    1,   60,    2, 0x08,    4 /* Private */,
       7,    1,   63,    2, 0x08,    6 /* Private */,
       9,    0,   66,    2, 0x08,    8 /* Private */,
      10,    0,   67,    2, 0x08,    9 /* Private */,
      11,    2,   68,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   12,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject DocumentMapPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_DocumentMapPanel.offsetsAndSizes,
    qt_meta_data_DocumentMapPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DocumentMapPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DocumentMapPanel, std::true_type>,
        // method 'scheduleMinimapUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditorScroll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onMiniMapScroll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onMiniMapClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onViewportTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLinesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCursorPositionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void DocumentMapPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DocumentMapPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->scheduleMinimapUpdate(); break;
        case 1: _t->onEditorScroll((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onMiniMapScroll((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onMiniMapClicked((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 4: _t->onViewportTimer(); break;
        case 5: _t->onLinesChanged(); break;
        case 6: _t->onCursorPositionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *DocumentMapPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocumentMapPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DocumentMapPanel.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int DocumentMapPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
