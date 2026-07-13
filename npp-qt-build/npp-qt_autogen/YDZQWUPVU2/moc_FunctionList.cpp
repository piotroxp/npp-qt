/****************************************************************************
** Meta object code from reading C++ file 'FunctionList.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/FunctionList.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FunctionList.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN22FunctionListTreeWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto FunctionListTreeWidget::qt_create_metaobjectdata<qt_meta_tag_ZN22FunctionListTreeWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FunctionListTreeWidget",
        "itemExpandToggle",
        "",
        "QTreeWidgetItem*",
        "item",
        "labelEditRequested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'itemExpandToggle'
        QtMocHelpers::SignalData<void(QTreeWidgetItem *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'labelEditRequested'
        QtMocHelpers::SignalData<void(QTreeWidgetItem *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FunctionListTreeWidget, qt_meta_tag_ZN22FunctionListTreeWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FunctionListTreeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTreeWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22FunctionListTreeWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22FunctionListTreeWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22FunctionListTreeWidgetE_t>.metaTypes,
    nullptr
} };

void FunctionListTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FunctionListTreeWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->itemExpandToggle((*reinterpret_cast<std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 1: _t->labelEditRequested((*reinterpret_cast<std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FunctionListTreeWidget::*)(QTreeWidgetItem * )>(_a, &FunctionListTreeWidget::itemExpandToggle, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FunctionListTreeWidget::*)(QTreeWidgetItem * )>(_a, &FunctionListTreeWidget::labelEditRequested, 1))
            return;
    }
}

const QMetaObject *FunctionListTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FunctionListTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22FunctionListTreeWidgetE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void FunctionListTreeWidget::itemExpandToggle(QTreeWidgetItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void FunctionListTreeWidget::labelEditRequested(QTreeWidgetItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN17FunctionListPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto FunctionListPanel::qt_create_metaobjectdata<qt_meta_tag_ZN17FunctionListPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'closeRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'functionSelected'
        QtMocHelpers::SignalData<void(int)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Signal 'reloadRequested'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSortToggled'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onReloadClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPreferencesClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onInitialSortAction'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSearchTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onEditorUiChanged'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'cleanup'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FunctionListPanel, qt_meta_tag_ZN17FunctionListPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FunctionListPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<DockingDlgInterface::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17FunctionListPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17FunctionListPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17FunctionListPanelE_t>.metaTypes,
    nullptr
} };

void FunctionListPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FunctionListPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->closeRequested(); break;
        case 1: _t->functionSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->reloadRequested(); break;
        case 3: _t->onSortToggled(); break;
        case 4: _t->onReloadClicked(); break;
        case 5: _t->onPreferencesClicked(); break;
        case 6: _t->onInitialSortAction(); break;
        case 7: _t->onSearchTextChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onEditorUiChanged(); break;
        case 9: _t->cleanup(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FunctionListPanel::*)()>(_a, &FunctionListPanel::closeRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FunctionListPanel::*)(int )>(_a, &FunctionListPanel::functionSelected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FunctionListPanel::*)()>(_a, &FunctionListPanel::reloadRequested, 2))
            return;
    }
}

const QMetaObject *FunctionListPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FunctionListPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17FunctionListPanelE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void FunctionListPanel::reloadRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
