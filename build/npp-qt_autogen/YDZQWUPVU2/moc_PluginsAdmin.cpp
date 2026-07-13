/****************************************************************************
** Meta object code from reading C++ file 'PluginsAdmin.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/WinControls/PluginsAdmin.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PluginsAdmin.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PluginListView_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PluginListView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PluginListView_t qt_meta_stringdata_PluginListView = {
    {
        QT_MOC_LITERAL(0, 14)   // "PluginListView"
    },
    "PluginListView"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PluginListView[] = {

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

Q_CONSTINIT const QMetaObject PluginListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListWidget::staticMetaObject>(),
    qt_meta_stringdata_PluginListView.offsetsAndSizes,
    qt_meta_data_PluginListView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PluginListView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PluginListView, std::true_type>
    >,
    nullptr
} };

void PluginListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PluginListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PluginListView.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int PluginListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PluginsAdminDlg_t {
    uint offsetsAndSizes[12];
    char stringdata0[16];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[24];
    char stringdata4[8];
    char stringdata5[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PluginsAdminDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PluginsAdminDlg_t qt_meta_stringdata_PluginsAdminDlg = {
    {
        QT_MOC_LITERAL(0, 15),  // "PluginsAdminDlg"
        QT_MOC_LITERAL(16, 16),  // "pluginsInstalled"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 23),  // "QList<PluginUpdateInfo>"
        QT_MOC_LITERAL(58, 7),  // "plugins"
        QT_MOC_LITERAL(66, 14)   // "pluginsRemoved"
    },
    "PluginsAdminDlg",
    "pluginsInstalled",
    "",
    "QList<PluginUpdateInfo>",
    "plugins",
    "pluginsRemoved"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PluginsAdminDlg[] = {

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

Q_CONSTINIT const QMetaObject PluginsAdminDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_meta_stringdata_PluginsAdminDlg.offsetsAndSizes,
    qt_meta_data_PluginsAdminDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PluginsAdminDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PluginsAdminDlg, std::true_type>,
        // method 'pluginsInstalled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<PluginUpdateInfo> &, std::false_type>,
        // method 'pluginsRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<PluginUpdateInfo> &, std::false_type>
    >,
    nullptr
} };

void PluginsAdminDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PluginsAdminDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pluginsInstalled((*reinterpret_cast< std::add_pointer_t<QList<PluginUpdateInfo>>>(_a[1]))); break;
        case 1: _t->pluginsRemoved((*reinterpret_cast< std::add_pointer_t<QList<PluginUpdateInfo>>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PluginsAdminDlg::*)(const QVector<PluginUpdateInfo> & );
            if (_t _q_method = &PluginsAdminDlg::pluginsInstalled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PluginsAdminDlg::*)(const QVector<PluginUpdateInfo> & );
            if (_t _q_method = &PluginsAdminDlg::pluginsRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *PluginsAdminDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginsAdminDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PluginsAdminDlg.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PluginsAdminDlg::pluginsInstalled(const QVector<PluginUpdateInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PluginsAdminDlg::pluginsRemoved(const QVector<PluginUpdateInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
