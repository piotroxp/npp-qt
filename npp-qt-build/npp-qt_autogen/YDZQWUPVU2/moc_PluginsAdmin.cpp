/****************************************************************************
** Meta object code from reading C++ file 'PluginsAdmin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/PluginsAdmin.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PluginsAdmin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14PluginListViewE_t {};
} // unnamed namespace

template <> constexpr inline auto PluginListView::qt_create_metaobjectdata<qt_meta_tag_ZN14PluginListViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PluginListView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PluginListView, qt_meta_tag_ZN14PluginListViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PluginListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14PluginListViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14PluginListViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14PluginListViewE_t>.metaTypes,
    nullptr
} };

void PluginListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PluginListView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *PluginListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14PluginListViewE_t>.strings))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int PluginListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN15PluginsAdminDlgE_t {};
} // unnamed namespace

template <> constexpr inline auto PluginsAdminDlg::qt_create_metaobjectdata<qt_meta_tag_ZN15PluginsAdminDlgE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PluginsAdminDlg",
        "pluginsInstalled",
        "",
        "QList<PluginUpdateInfo>",
        "plugins",
        "pluginsRemoved"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'pluginsInstalled'
        QtMocHelpers::SignalData<void(const QVector<PluginUpdateInfo> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'pluginsRemoved'
        QtMocHelpers::SignalData<void(const QVector<PluginUpdateInfo> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PluginsAdminDlg, qt_meta_tag_ZN15PluginsAdminDlgE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PluginsAdminDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PluginsAdminDlgE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PluginsAdminDlgE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15PluginsAdminDlgE_t>.metaTypes,
    nullptr
} };

void PluginsAdminDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PluginsAdminDlg *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->pluginsInstalled((*reinterpret_cast<std::add_pointer_t<QList<PluginUpdateInfo>>>(_a[1]))); break;
        case 1: _t->pluginsRemoved((*reinterpret_cast<std::add_pointer_t<QList<PluginUpdateInfo>>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PluginsAdminDlg::*)(const QVector<PluginUpdateInfo> & )>(_a, &PluginsAdminDlg::pluginsInstalled, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PluginsAdminDlg::*)(const QVector<PluginUpdateInfo> & )>(_a, &PluginsAdminDlg::pluginsRemoved, 1))
            return;
    }
}

const QMetaObject *PluginsAdminDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginsAdminDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PluginsAdminDlgE_t>.strings))
        return static_cast<void*>(this);
    return StaticDialog::qt_metacast(_clname);
}

int PluginsAdminDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StaticDialog::qt_metacall(_c, _id, _a);
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
void PluginsAdminDlg::pluginsInstalled(const QVector<PluginUpdateInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PluginsAdminDlg::pluginsRemoved(const QVector<PluginUpdateInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
