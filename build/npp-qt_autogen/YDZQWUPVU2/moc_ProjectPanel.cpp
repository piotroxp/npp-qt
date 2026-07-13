/****************************************************************************
** Meta object code from reading C++ file 'ProjectPanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/ProjectPanel.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProjectPanel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FileRelocalizerDlg_t {
    uint offsetsAndSizes[2];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FileRelocalizerDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FileRelocalizerDlg_t qt_meta_stringdata_FileRelocalizerDlg = {
    {
        QT_MOC_LITERAL(0, 18)   // "FileRelocalizerDlg"
    },
    "FileRelocalizerDlg"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FileRelocalizerDlg[] = {

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

Q_CONSTINIT const QMetaObject FileRelocalizerDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FileRelocalizerDlg.offsetsAndSizes,
    qt_meta_data_FileRelocalizerDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FileRelocalizerDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FileRelocalizerDlg, std::true_type>
    >,
    nullptr
} };

void FileRelocalizerDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *FileRelocalizerDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileRelocalizerDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileRelocalizerDlg.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FileRelocalizerDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ProjectPanel_t {
    uint offsetsAndSizes[64];
    char stringdata0[13];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[15];
    char stringdata5[13];
    char stringdata6[8];
    char stringdata7[24];
    char stringdata8[6];
    char stringdata9[20];
    char stringdata10[17];
    char stringdata11[5];
    char stringdata12[7];
    char stringdata13[14];
    char stringdata14[4];
    char stringdata15[15];
    char stringdata16[16];
    char stringdata17[15];
    char stringdata18[16];
    char stringdata19[16];
    char stringdata20[18];
    char stringdata21[23];
    char stringdata22[18];
    char stringdata23[12];
    char stringdata24[11];
    char stringdata25[20];
    char stringdata26[13];
    char stringdata27[13];
    char stringdata28[17];
    char stringdata29[17];
    char stringdata30[9];
    char stringdata31[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ProjectPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ProjectPanel_t qt_meta_stringdata_ProjectPanel = {
    {
        QT_MOC_LITERAL(0, 12),  // "ProjectPanel"
        QT_MOC_LITERAL(13, 13),  // "fileActivated"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 4),  // "path"
        QT_MOC_LITERAL(33, 14),  // "workSpaceSaved"
        QT_MOC_LITERAL(48, 12),  // "dirtyChanged"
        QT_MOC_LITERAL(61, 7),  // "isDirty"
        QT_MOC_LITERAL(69, 23),  // "findInProjectsRequested"
        QT_MOC_LITERAL(93, 5),  // "files"
        QT_MOC_LITERAL(99, 19),  // "onItemDoubleClicked"
        QT_MOC_LITERAL(119, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(136, 4),  // "item"
        QT_MOC_LITERAL(141, 6),  // "column"
        QT_MOC_LITERAL(148, 13),  // "onContextMenu"
        QT_MOC_LITERAL(162, 3),  // "pos"
        QT_MOC_LITERAL(166, 14),  // "onItemExpanded"
        QT_MOC_LITERAL(181, 15),  // "onItemCollapsed"
        QT_MOC_LITERAL(197, 14),  // "onNewWorkspace"
        QT_MOC_LITERAL(212, 15),  // "onOpenWorkspace"
        QT_MOC_LITERAL(228, 15),  // "onSaveWorkspace"
        QT_MOC_LITERAL(244, 17),  // "onSaveAsWorkspace"
        QT_MOC_LITERAL(262, 22),  // "onSaveACopyAsWorkspace"
        QT_MOC_LITERAL(285, 17),  // "onReloadWorkspace"
        QT_MOC_LITERAL(303, 11),  // "onAddFolder"
        QT_MOC_LITERAL(315, 10),  // "onAddFiles"
        QT_MOC_LITERAL(326, 19),  // "onAddFilesRecursive"
        QT_MOC_LITERAL(346, 12),  // "onRemoveItem"
        QT_MOC_LITERAL(359, 12),  // "onRenameItem"
        QT_MOC_LITERAL(372, 16),  // "onModifyFilePath"
        QT_MOC_LITERAL(389, 16),  // "onFindInProjects"
        QT_MOC_LITERAL(406, 8),  // "onMoveUp"
        QT_MOC_LITERAL(415, 10)   // "onMoveDown"
    },
    "ProjectPanel",
    "fileActivated",
    "",
    "path",
    "workSpaceSaved",
    "dirtyChanged",
    "isDirty",
    "findInProjectsRequested",
    "files",
    "onItemDoubleClicked",
    "QTreeWidgetItem*",
    "item",
    "column",
    "onContextMenu",
    "pos",
    "onItemExpanded",
    "onItemCollapsed",
    "onNewWorkspace",
    "onOpenWorkspace",
    "onSaveWorkspace",
    "onSaveAsWorkspace",
    "onSaveACopyAsWorkspace",
    "onReloadWorkspace",
    "onAddFolder",
    "onAddFiles",
    "onAddFilesRecursive",
    "onRemoveItem",
    "onRenameItem",
    "onModifyFilePath",
    "onFindInProjects",
    "onMoveUp",
    "onMoveDown"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ProjectPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  152,    2, 0x06,    1 /* Public */,
       4,    1,  155,    2, 0x06,    3 /* Public */,
       5,    1,  158,    2, 0x06,    5 /* Public */,
       7,    1,  161,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    2,  164,    2, 0x08,    9 /* Private */,
      13,    1,  169,    2, 0x08,   12 /* Private */,
      15,    1,  172,    2, 0x08,   14 /* Private */,
      16,    1,  175,    2, 0x08,   16 /* Private */,
      17,    0,  178,    2, 0x08,   18 /* Private */,
      18,    0,  179,    2, 0x08,   19 /* Private */,
      19,    0,  180,    2, 0x08,   20 /* Private */,
      20,    0,  181,    2, 0x08,   21 /* Private */,
      21,    0,  182,    2, 0x08,   22 /* Private */,
      22,    0,  183,    2, 0x08,   23 /* Private */,
      23,    0,  184,    2, 0x08,   24 /* Private */,
      24,    0,  185,    2, 0x08,   25 /* Private */,
      25,    0,  186,    2, 0x08,   26 /* Private */,
      26,    0,  187,    2, 0x08,   27 /* Private */,
      27,    0,  188,    2, 0x08,   28 /* Private */,
      28,    0,  189,    2, 0x08,   29 /* Private */,
      29,    0,  190,    2, 0x08,   30 /* Private */,
      30,    0,  191,    2, 0x08,   31 /* Private */,
      31,    0,  192,    2, 0x08,   32 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QStringList,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,   11,   12,
    QMetaType::Void, QMetaType::QPoint,   14,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ProjectPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_meta_stringdata_ProjectPanel.offsetsAndSizes,
    qt_meta_data_ProjectPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ProjectPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ProjectPanel, std::true_type>,
        // method 'fileActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'workSpaceSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'dirtyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'findInProjectsRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'onItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onContextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onItemExpanded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        // method 'onItemCollapsed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        // method 'onNewWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOpenWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveAsWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveACopyAsWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReloadWorkspace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddFilesRecursive'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRemoveItem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRenameItem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifyFilePath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFindInProjects'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMoveUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMoveDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ProjectPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProjectPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileActivated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->workSpaceSaved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->dirtyChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->findInProjectsRequested((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 4: _t->onItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->onContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 6: _t->onItemExpanded((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 7: _t->onItemCollapsed((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 8: _t->onNewWorkspace(); break;
        case 9: _t->onOpenWorkspace(); break;
        case 10: _t->onSaveWorkspace(); break;
        case 11: _t->onSaveAsWorkspace(); break;
        case 12: _t->onSaveACopyAsWorkspace(); break;
        case 13: _t->onReloadWorkspace(); break;
        case 14: _t->onAddFolder(); break;
        case 15: _t->onAddFiles(); break;
        case 16: _t->onAddFilesRecursive(); break;
        case 17: _t->onRemoveItem(); break;
        case 18: _t->onRenameItem(); break;
        case 19: _t->onModifyFilePath(); break;
        case 20: _t->onFindInProjects(); break;
        case 21: _t->onMoveUp(); break;
        case 22: _t->onMoveDown(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProjectPanel::*)(const QString & );
            if (_t _q_method = &ProjectPanel::fileActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProjectPanel::*)(const QString & );
            if (_t _q_method = &ProjectPanel::workSpaceSaved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProjectPanel::*)(bool );
            if (_t _q_method = &ProjectPanel::dirtyChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProjectPanel::*)(const QStringList & );
            if (_t _q_method = &ProjectPanel::findInProjectsRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *ProjectPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProjectPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectPanel.stringdata0))
        return static_cast<void*>(this);
    return DockingDlgInterface::qt_metacast(_clname);
}

int ProjectPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockingDlgInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void ProjectPanel::fileActivated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProjectPanel::workSpaceSaved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProjectPanel::dirtyChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProjectPanel::findInProjectsRequested(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
