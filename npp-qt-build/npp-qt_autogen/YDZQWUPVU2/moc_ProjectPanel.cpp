/****************************************************************************
** Meta object code from reading C++ file 'ProjectPanel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/ProjectPanel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProjectPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN18FileRelocalizerDlgE_t {};
} // unnamed namespace

template <> constexpr inline auto FileRelocalizerDlg::qt_create_metaobjectdata<qt_meta_tag_ZN18FileRelocalizerDlgE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileRelocalizerDlg"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FileRelocalizerDlg, qt_meta_tag_ZN18FileRelocalizerDlgE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FileRelocalizerDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18FileRelocalizerDlgE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18FileRelocalizerDlgE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18FileRelocalizerDlgE_t>.metaTypes,
    nullptr
} };

void FileRelocalizerDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileRelocalizerDlg *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *FileRelocalizerDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileRelocalizerDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18FileRelocalizerDlgE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FileRelocalizerDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN12ProjectPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto ProjectPanel::qt_create_metaobjectdata<qt_meta_tag_ZN12ProjectPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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
        "QPoint",
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'fileActivated'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'workSpaceSaved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'dirtyChanged'
        QtMocHelpers::SignalData<void(bool)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 6 },
        }}),
        // Signal 'findInProjectsRequested'
        QtMocHelpers::SignalData<void(const QStringList &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 8 },
        }}),
        // Slot 'onItemDoubleClicked'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *, int)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::Int, 12 },
        }}),
        // Slot 'onContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Slot 'onItemExpanded'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onItemCollapsed'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onNewWorkspace'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenWorkspace'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveWorkspace'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveAsWorkspace'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveACopyAsWorkspace'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReloadWorkspace'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddFolder'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddFiles'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddFilesRecursive'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveItem'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRenameItem'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifyFilePath'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFindInProjects'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMoveUp'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMoveDown'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ProjectPanel, qt_meta_tag_ZN12ProjectPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ProjectPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ProjectPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ProjectPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12ProjectPanelE_t>.metaTypes,
    nullptr
} };

void ProjectPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ProjectPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->fileActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->workSpaceSaved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->dirtyChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->findInProjectsRequested((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 4: _t->onItemDoubleClicked((*reinterpret_cast<std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->onContextMenu((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 6: _t->onItemExpanded((*reinterpret_cast<std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 7: _t->onItemCollapsed((*reinterpret_cast<std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
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
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ProjectPanel::*)(const QString & )>(_a, &ProjectPanel::fileActivated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProjectPanel::*)(const QString & )>(_a, &ProjectPanel::workSpaceSaved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProjectPanel::*)(bool )>(_a, &ProjectPanel::dirtyChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProjectPanel::*)(const QStringList & )>(_a, &ProjectPanel::findInProjectsRequested, 3))
            return;
    }
}

const QMetaObject *ProjectPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProjectPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ProjectPanelE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void ProjectPanel::fileActivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ProjectPanel::workSpaceSaved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ProjectPanel::dirtyChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ProjectPanel::findInProjectsRequested(const QStringList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
