/****************************************************************************
** Meta object code from reading C++ file 'DockingWnd.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/DockingWnd.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DockingWnd.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19DockingDlgInterfaceE_t {};
} // unnamed namespace

template <> constexpr inline auto DockingDlgInterface::qt_create_metaobjectdata<qt_meta_tag_ZN19DockingDlgInterfaceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DockingDlgInterface"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DockingDlgInterface, qt_meta_tag_ZN19DockingDlgInterfaceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DockingDlgInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DockingDlgInterfaceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DockingDlgInterfaceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19DockingDlgInterfaceE_t>.metaTypes,
    nullptr
} };

void DockingDlgInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DockingDlgInterface *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *DockingDlgInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingDlgInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DockingDlgInterfaceE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DockingDlgInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN16DockingContainerE_t {};
} // unnamed namespace

template <> constexpr inline auto DockingContainer::qt_create_metaobjectdata<qt_meta_tag_ZN16DockingContainerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'widgetShown'
        QtMocHelpers::SignalData<void(QWidget *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'widgetHidden'
        QtMocHelpers::SignalData<void(QWidget *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'tabChanged'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'closeRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'moveRequested'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'dockToggleRequested'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'floatAllRequested'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'closeAllTabs'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onTabChanged'
        QtMocHelpers::SlotData<void(int)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'onTabCloseRequested'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DockingContainer, qt_meta_tag_ZN16DockingContainerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DockingContainer::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DockingContainerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DockingContainerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16DockingContainerE_t>.metaTypes,
    nullptr
} };

void DockingContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DockingContainer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->widgetShown((*reinterpret_cast<std::add_pointer_t<QWidget*>>(_a[1]))); break;
        case 1: _t->widgetHidden((*reinterpret_cast<std::add_pointer_t<QWidget*>>(_a[1]))); break;
        case 2: _t->tabChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->closeRequested(); break;
        case 4: _t->moveRequested(); break;
        case 5: _t->dockToggleRequested(); break;
        case 6: _t->floatAllRequested(); break;
        case 7: _t->closeAllTabs(); break;
        case 8: _t->onTabChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onTabCloseRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)(QWidget * )>(_a, &DockingContainer::widgetShown, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)(QWidget * )>(_a, &DockingContainer::widgetHidden, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)(int )>(_a, &DockingContainer::tabChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)()>(_a, &DockingContainer::closeRequested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)()>(_a, &DockingContainer::moveRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)()>(_a, &DockingContainer::dockToggleRequested, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockingContainer::*)()>(_a, &DockingContainer::floatAllRequested, 6))
            return;
    }
}

const QMetaObject *DockingContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DockingContainerE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DockingContainer::widgetShown(QWidget * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DockingContainer::widgetHidden(QWidget * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DockingContainer::tabChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
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
struct qt_meta_tag_ZN15DockingSplitterE_t {};
} // unnamed namespace

template <> constexpr inline auto DockingSplitter::qt_create_metaobjectdata<qt_meta_tag_ZN15DockingSplitterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DockingSplitter",
        "splitterMoved",
        "",
        "newSize"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'splitterMoved'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DockingSplitter, qt_meta_tag_ZN15DockingSplitterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DockingSplitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DockingSplitterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DockingSplitterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15DockingSplitterE_t>.metaTypes,
    nullptr
} };

void DockingSplitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DockingSplitter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->splitterMoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DockingSplitter::*)(int )>(_a, &DockingSplitter::splitterMoved, 0))
            return;
    }
}

const QMetaObject *DockingSplitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingSplitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DockingSplitterE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DockingSplitter::splitterMoved(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN14DockingManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto DockingManager::qt_create_metaobjectdata<qt_meta_tag_ZN14DockingManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'dockInfoChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'showFloatingContainers'
        QtMocHelpers::SlotData<void(bool)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 4 },
        }}),
        // Slot 'createDockableDialog'
        QtMocHelpers::SlotData<void(DockedWidgetData, int, bool)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { QMetaType::Int, 8 }, { QMetaType::Bool, 9 },
        }}),
        // Slot 'createDockableDialog'
        QtMocHelpers::SlotData<void(DockedWidgetData, int)>(5, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { QMetaType::Int, 8 },
        }}),
        // Slot 'createDockableDialog'
        QtMocHelpers::SlotData<void(DockedWidgetData)>(5, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'showDockableDialog'
        QtMocHelpers::SlotData<void(QWidget *, bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { QMetaType::Bool, 4 },
        }}),
        // Slot 'showDockableDialogByName'
        QtMocHelpers::SlotData<void(const QString &, bool)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::Bool, 4 },
        }}),
        // Slot 'setActiveTab'
        QtMocHelpers::SlotData<void(int, int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 16 },
        }}),
        // Slot 'setDockedContainerSize'
        QtMocHelpers::SlotData<void(int, int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 18 },
        }}),
        // Slot 'setDockedContSize'
        QtMocHelpers::SlotData<void(int, int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 18 },
        }}),
        // Slot 'dockedContainerSize'
        QtMocHelpers::SlotData<int(int) const>(20, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'getDockedContSize'
        QtMocHelpers::SlotData<int(int) const>(21, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'setStyleCaption'
        QtMocHelpers::SlotData<void(bool)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
        }}),
        // Slot 'getContainerInfo'
        QtMocHelpers::SlotData<std::vector<DockingContainer*>() const>(24, 2, QMC::AccessPublic, 0x80000000 | 25),
        // Slot 'resize'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onFocusChanged'
        QtMocHelpers::SlotData<void(QWidget *, QWidget *)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 28 }, { 0x80000000 | 11, 29 },
        }}),
        // Slot 'focusClient'
        QtMocHelpers::SlotData<QWidget *()>(30, 2, QMC::AccessPublic, 0x80000000 | 11),
        // Slot 'doesContExist'
        QtMocHelpers::SlotData<bool(size_t) const>(31, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Slot 'isInitialized'
        QtMocHelpers::SlotData<bool() const>(34, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'isRegistered'
        QtMocHelpers::SlotData<bool()>(35, 2, QMC::AccessPublic, QMetaType::Bool),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DockingManager, qt_meta_tag_ZN14DockingManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DockingManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14DockingManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14DockingManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14DockingManagerE_t>.metaTypes,
    nullptr
} };

void DockingManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DockingManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->dockInfoChanged(); break;
        case 1: _t->showFloatingContainers((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->createDockableDialog((*reinterpret_cast<std::add_pointer_t<DockedWidgetData>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 3: _t->createDockableDialog((*reinterpret_cast<std::add_pointer_t<DockedWidgetData>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->createDockableDialog((*reinterpret_cast<std::add_pointer_t<DockedWidgetData>>(_a[1]))); break;
        case 5: _t->showDockableDialog((*reinterpret_cast<std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->showDockableDialogByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->setActiveTab((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->setDockedContainerSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 9: _t->setDockedContSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 10: { int _r = _t->dockedContainerSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 11: { int _r = _t->getDockedContSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->setStyleCaption((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: { std::vector<DockingContainer*> _r = _t->getContainerInfo();
            if (_a[0]) *reinterpret_cast<std::vector<DockingContainer*>*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->resize(); break;
        case 15: _t->onFocusChanged((*reinterpret_cast<std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QWidget*>>(_a[2]))); break;
        case 16: { QWidget* _r = _t->focusClient();
            if (_a[0]) *reinterpret_cast<QWidget**>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->doesContExist((*reinterpret_cast<std::add_pointer_t<size_t>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->isInitialized();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 19: { bool _r = _t->isRegistered();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DockingManager::*)()>(_a, &DockingManager::dockInfoChanged, 0))
            return;
    }
}

const QMetaObject *DockingManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockingManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14DockingManagerE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
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
