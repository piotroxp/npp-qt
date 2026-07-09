/****************************************************************************
** Meta object code from reading C++ file 'FileBrowserPanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/panels/FileBrowserPanel.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileBrowserPanel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FileBrowserPanel_t {
    uint offsetsAndSizes[24];
    char stringdata0[17];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[16];
    char stringdata5[12];
    char stringdata6[6];
    char stringdata7[14];
    char stringdata8[4];
    char stringdata9[17];
    char stringdata10[20];
    char stringdata11[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FileBrowserPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FileBrowserPanel_t qt_meta_stringdata_FileBrowserPanel = {
    {
        QT_MOC_LITERAL(0, 16),  // "FileBrowserPanel"
        QT_MOC_LITERAL(17, 17),  // "fileDoubleClicked"
        QT_MOC_LITERAL(35, 0),  // ""
        QT_MOC_LITERAL(36, 4),  // "path"
        QT_MOC_LITERAL(41, 15),  // "onDoubleClicked"
        QT_MOC_LITERAL(57, 11),  // "QModelIndex"
        QT_MOC_LITERAL(69, 5),  // "index"
        QT_MOC_LITERAL(75, 13),  // "onContextMenu"
        QT_MOC_LITERAL(89, 3),  // "pos"
        QT_MOC_LITERAL(93, 16),  // "openSelectedFile"
        QT_MOC_LITERAL(110, 19),  // "copyPathToClipboard"
        QT_MOC_LITERAL(130, 19)   // "revealInFileManager"
    },
    "FileBrowserPanel",
    "fileDoubleClicked",
    "",
    "path",
    "onDoubleClicked",
    "QModelIndex",
    "index",
    "onContextMenu",
    "pos",
    "openSelectedFile",
    "copyPathToClipboard",
    "revealInFileManager"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FileBrowserPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,   53,    2, 0x08,    3 /* Private */,
       7,    1,   56,    2, 0x08,    5 /* Private */,
       9,    0,   59,    2, 0x08,    7 /* Private */,
      10,    0,   60,    2, 0x08,    8 /* Private */,
      11,    0,   61,    2, 0x08,    9 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FileBrowserPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_FileBrowserPanel.offsetsAndSizes,
    qt_meta_data_FileBrowserPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FileBrowserPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FileBrowserPanel, std::true_type>,
        // method 'fileDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'onContextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'openSelectedFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copyPathToClipboard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'revealInFileManager'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FileBrowserPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileBrowserPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileDoubleClicked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onDoubleClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 2: _t->onContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 3: _t->openSelectedFile(); break;
        case 4: _t->copyPathToClipboard(); break;
        case 5: _t->revealInFileManager(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileBrowserPanel::*)(const QString & );
            if (_t _q_method = &FileBrowserPanel::fileDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *FileBrowserPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileBrowserPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileBrowserPanel.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int FileBrowserPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
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
void FileBrowserPanel::fileDoubleClicked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
