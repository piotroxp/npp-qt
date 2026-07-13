/****************************************************************************
** Meta object code from reading C++ file 'FindReplaceDlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/FindReplaceDlg.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FindReplaceDlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FindInFilesWorker_t {
    uint offsetsAndSizes[32];
    char stringdata0[18];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[6];
    char stringdata5[9];
    char stringdata6[12];
    char stringdata7[5];
    char stringdata8[8];
    char stringdata9[6];
    char stringdata10[4];
    char stringdata11[12];
    char stringdata12[9];
    char stringdata13[13];
    char stringdata14[11];
    char stringdata15[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FindInFilesWorker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FindInFilesWorker_t qt_meta_stringdata_FindInFilesWorker = {
    {
        QT_MOC_LITERAL(0, 17),  // "FindInFilesWorker"
        QT_MOC_LITERAL(18, 8),  // "progress"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 7),  // "current"
        QT_MOC_LITERAL(36, 5),  // "total"
        QT_MOC_LITERAL(42, 8),  // "fileName"
        QT_MOC_LITERAL(51, 11),  // "resultReady"
        QT_MOC_LITERAL(63, 4),  // "line"
        QT_MOC_LITERAL(68, 7),  // "int64_t"
        QT_MOC_LITERAL(76, 5),  // "start"
        QT_MOC_LITERAL(82, 3),  // "end"
        QT_MOC_LITERAL(86, 11),  // "lineContent"
        QT_MOC_LITERAL(98, 8),  // "finished"
        QT_MOC_LITERAL(107, 12),  // "totalMatches"
        QT_MOC_LITERAL(120, 10),  // "totalFiles"
        QT_MOC_LITERAL(131, 3)   // "run"
    },
    "FindInFilesWorker",
    "progress",
    "",
    "current",
    "total",
    "fileName",
    "resultReady",
    "line",
    "int64_t",
    "start",
    "end",
    "lineContent",
    "finished",
    "totalMatches",
    "totalFiles",
    "run"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FindInFilesWorker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   38,    2, 0x06,    1 /* Public */,
       6,    5,   45,    2, 0x06,    5 /* Public */,
      12,    2,   56,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    0,   61,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8, 0x80000000 | 8, QMetaType::QString, QMetaType::QString,    7,    9,   10,    5,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   14,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FindInFilesWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FindInFilesWorker.offsetsAndSizes,
    qt_meta_data_FindInFilesWorker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FindInFilesWorker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FindInFilesWorker, std::true_type>,
        // method 'progress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'resultReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<int64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'run'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FindInFilesWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FindInFilesWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->progress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->resultReady((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int64_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int64_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5]))); break;
        case 2: _t->finished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->run(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FindInFilesWorker::*)(int , int , const QString & );
            if (_t _q_method = &FindInFilesWorker::progress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FindInFilesWorker::*)(int , int64_t , int64_t , const QString & , const QString & );
            if (_t _q_method = &FindInFilesWorker::resultReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FindInFilesWorker::*)(int , int );
            if (_t _q_method = &FindInFilesWorker::finished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *FindInFilesWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FindInFilesWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FindInFilesWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FindInFilesWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void FindInFilesWorker::progress(int _t1, int _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FindInFilesWorker::resultReady(int _t1, int64_t _t2, int64_t _t3, const QString & _t4, const QString & _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FindInFilesWorker::finished(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
namespace {
struct qt_meta_stringdata_FindReplaceDlg_t {
    uint offsetsAndSizes[32];
    char stringdata0[15];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[12];
    char stringdata5[13];
    char stringdata6[11];
    char stringdata7[27];
    char stringdata8[9];
    char stringdata9[11];
    char stringdata10[11];
    char stringdata11[10];
    char stringdata12[13];
    char stringdata13[10];
    char stringdata14[10];
    char stringdata15[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FindReplaceDlg_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FindReplaceDlg_t qt_meta_stringdata_FindReplaceDlg = {
    {
        QT_MOC_LITERAL(0, 14),  // "FindReplaceDlg"
        QT_MOC_LITERAL(15, 15),  // "searchCompleted"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 5),  // "count"
        QT_MOC_LITERAL(38, 11),  // "fifFinished"
        QT_MOC_LITERAL(50, 12),  // "totalMatches"
        QT_MOC_LITERAL(63, 10),  // "totalFiles"
        QT_MOC_LITERAL(74, 26),  // "fifResultFileOpenRequested"
        QT_MOC_LITERAL(101, 8),  // "filePath"
        QT_MOC_LITERAL(110, 10),  // "lineNumber"
        QT_MOC_LITERAL(121, 10),  // "onFindNext"
        QT_MOC_LITERAL(132, 9),  // "onReplace"
        QT_MOC_LITERAL(142, 12),  // "onReplaceAll"
        QT_MOC_LITERAL(155, 9),  // "onFindAll"
        QT_MOC_LITERAL(165, 9),  // "onMarkAll"
        QT_MOC_LITERAL(175, 7)   // "onClose"
    },
    "FindReplaceDlg",
    "searchCompleted",
    "",
    "count",
    "fifFinished",
    "totalMatches",
    "totalFiles",
    "fifResultFileOpenRequested",
    "filePath",
    "lineNumber",
    "onFindNext",
    "onReplace",
    "onReplaceAll",
    "onFindAll",
    "onMarkAll",
    "onClose"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FindReplaceDlg[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       4,    2,   71,    2, 0x06,    3 /* Public */,
       7,    2,   76,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    0,   81,    2, 0x0a,    9 /* Public */,
      11,    0,   82,    2, 0x0a,   10 /* Public */,
      12,    0,   83,    2, 0x0a,   11 /* Public */,
      13,    0,   84,    2, 0x0a,   12 /* Public */,
      14,    0,   85,    2, 0x0a,   13 /* Public */,
      15,    0,   86,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    8,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FindReplaceDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_FindReplaceDlg.offsetsAndSizes,
    qt_meta_data_FindReplaceDlg,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FindReplaceDlg_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FindReplaceDlg, std::true_type>,
        // method 'searchCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fifFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fifResultFileOpenRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onFindNext'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplaceAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFindAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMarkAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FindReplaceDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FindReplaceDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->searchCompleted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->fifFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->fifResultFileOpenRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->onFindNext(); break;
        case 4: _t->onReplace(); break;
        case 5: _t->onReplaceAll(); break;
        case 6: _t->onFindAll(); break;
        case 7: _t->onMarkAll(); break;
        case 8: _t->onClose(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FindReplaceDlg::*)(int );
            if (_t _q_method = &FindReplaceDlg::searchCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FindReplaceDlg::*)(int , int );
            if (_t _q_method = &FindReplaceDlg::fifFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FindReplaceDlg::*)(const QString & , int );
            if (_t _q_method = &FindReplaceDlg::fifResultFileOpenRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *FindReplaceDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FindReplaceDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FindReplaceDlg.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FindReplaceDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void FindReplaceDlg::searchCompleted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FindReplaceDlg::fifFinished(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FindReplaceDlg::fifResultFileOpenRequested(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
