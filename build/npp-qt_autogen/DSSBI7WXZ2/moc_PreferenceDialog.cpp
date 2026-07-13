/****************************************************************************
** Meta object code from reading C++ file 'PreferenceDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/Dialogs/PreferenceDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreferenceDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PreferencePage_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PreferencePage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PreferencePage_t qt_meta_stringdata_PreferencePage = {
    {
        QT_MOC_LITERAL(0, 14)   // "PreferencePage"
    },
    "PreferencePage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PreferencePage[] = {

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

Q_CONSTINIT const QMetaObject PreferencePage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PreferencePage.offsetsAndSizes,
    qt_meta_data_PreferencePage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PreferencePage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PreferencePage, std::true_type>
    >,
    nullptr
} };

void PreferencePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PreferencePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreferencePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreferencePage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PreferencePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_GeneralPage_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GeneralPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GeneralPage_t qt_meta_stringdata_GeneralPage = {
    {
        QT_MOC_LITERAL(0, 11)   // "GeneralPage"
    },
    "GeneralPage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GeneralPage[] = {

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

Q_CONSTINIT const QMetaObject GeneralPage::staticMetaObject = { {
    QMetaObject::SuperData::link<PreferencePage::staticMetaObject>(),
    qt_meta_stringdata_GeneralPage.offsetsAndSizes,
    qt_meta_data_GeneralPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GeneralPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GeneralPage, std::true_type>
    >,
    nullptr
} };

void GeneralPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GeneralPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GeneralPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GeneralPage.stringdata0))
        return static_cast<void*>(this);
    return PreferencePage::qt_metacast(_clname);
}

int GeneralPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PreferencePage::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_EditingPage_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_EditingPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_EditingPage_t qt_meta_stringdata_EditingPage = {
    {
        QT_MOC_LITERAL(0, 11)   // "EditingPage"
    },
    "EditingPage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_EditingPage[] = {

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

Q_CONSTINIT const QMetaObject EditingPage::staticMetaObject = { {
    QMetaObject::SuperData::link<PreferencePage::staticMetaObject>(),
    qt_meta_stringdata_EditingPage.offsetsAndSizes,
    qt_meta_data_EditingPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_EditingPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<EditingPage, std::true_type>
    >,
    nullptr
} };

void EditingPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *EditingPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditingPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EditingPage.stringdata0))
        return static_cast<void*>(this);
    return PreferencePage::qt_metacast(_clname);
}

int EditingPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PreferencePage::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_LanguagePage_t {
    uint offsetsAndSizes[2];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LanguagePage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LanguagePage_t qt_meta_stringdata_LanguagePage = {
    {
        QT_MOC_LITERAL(0, 12)   // "LanguagePage"
    },
    "LanguagePage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LanguagePage[] = {

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

Q_CONSTINIT const QMetaObject LanguagePage::staticMetaObject = { {
    QMetaObject::SuperData::link<PreferencePage::staticMetaObject>(),
    qt_meta_stringdata_LanguagePage.offsetsAndSizes,
    qt_meta_data_LanguagePage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LanguagePage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LanguagePage, std::true_type>
    >,
    nullptr
} };

void LanguagePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *LanguagePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LanguagePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LanguagePage.stringdata0))
        return static_cast<void*>(this);
    return PreferencePage::qt_metacast(_clname);
}

int LanguagePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PreferencePage::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_BackupPage_t {
    uint offsetsAndSizes[6];
    char stringdata0[11];
    char stringdata1[17];
    char stringdata2[1];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_BackupPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_BackupPage_t qt_meta_stringdata_BackupPage = {
    {
        QT_MOC_LITERAL(0, 10),  // "BackupPage"
        QT_MOC_LITERAL(11, 16),  // "browseBackupPath"
        QT_MOC_LITERAL(28, 0)   // ""
    },
    "BackupPage",
    "browseBackupPath",
    ""
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_BackupPage[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x08,    1 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject BackupPage::staticMetaObject = { {
    QMetaObject::SuperData::link<PreferencePage::staticMetaObject>(),
    qt_meta_stringdata_BackupPage.offsetsAndSizes,
    qt_meta_data_BackupPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_BackupPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BackupPage, std::true_type>,
        // method 'browseBackupPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void BackupPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BackupPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->browseBackupPath(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *BackupPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BackupPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BackupPage.stringdata0))
        return static_cast<void*>(this);
    return PreferencePage::qt_metacast(_clname);
}

int BackupPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PreferencePage::qt_metacall(_c, _id, _a);
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
namespace {
struct qt_meta_stringdata_PrintPage_t {
    uint offsetsAndSizes[2];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PrintPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PrintPage_t qt_meta_stringdata_PrintPage = {
    {
        QT_MOC_LITERAL(0, 9)   // "PrintPage"
    },
    "PrintPage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PrintPage[] = {

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

Q_CONSTINIT const QMetaObject PrintPage::staticMetaObject = { {
    QMetaObject::SuperData::link<PreferencePage::staticMetaObject>(),
    qt_meta_stringdata_PrintPage.offsetsAndSizes,
    qt_meta_data_PrintPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PrintPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PrintPage, std::true_type>
    >,
    nullptr
} };

void PrintPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PrintPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PrintPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PrintPage.stringdata0))
        return static_cast<void*>(this);
    return PreferencePage::qt_metacast(_clname);
}

int PrintPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PreferencePage::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PreferenceDialog_t {
    uint offsetsAndSizes[18];
    char stringdata0[17];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[8];
    char stringdata5[9];
    char stringdata6[5];
    char stringdata7[9];
    char stringdata8[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PreferenceDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PreferenceDialog_t qt_meta_stringdata_PreferenceDialog = {
    {
        QT_MOC_LITERAL(0, 16),  // "PreferenceDialog"
        QT_MOC_LITERAL(17, 17),  // "onCategoryChanged"
        QT_MOC_LITERAL(35, 0),  // ""
        QT_MOC_LITERAL(36, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(53, 7),  // "current"
        QT_MOC_LITERAL(61, 8),  // "previous"
        QT_MOC_LITERAL(70, 4),  // "onOk"
        QT_MOC_LITERAL(75, 8),  // "onCancel"
        QT_MOC_LITERAL(84, 7)   // "onApply"
    },
    "PreferenceDialog",
    "onCategoryChanged",
    "",
    "QListWidgetItem*",
    "current",
    "previous",
    "onOk",
    "onCancel",
    "onApply"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PreferenceDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   38,    2, 0x08,    1 /* Private */,
       6,    0,   43,    2, 0x08,    4 /* Private */,
       7,    0,   44,    2, 0x08,    5 /* Private */,
       8,    0,   45,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PreferenceDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PreferenceDialog.offsetsAndSizes,
    qt_meta_data_PreferenceDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PreferenceDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PreferenceDialog, std::true_type>,
        // method 'onCategoryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onOk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onApply'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PreferenceDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreferenceDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCategoryChanged((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[2]))); break;
        case 1: _t->onOk(); break;
        case 2: _t->onCancel(); break;
        case 3: _t->onApply(); break;
        default: ;
        }
    }
}

const QMetaObject *PreferenceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreferenceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreferenceDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PreferenceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
