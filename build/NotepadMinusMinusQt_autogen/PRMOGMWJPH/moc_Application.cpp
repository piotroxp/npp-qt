/****************************************************************************
** Meta object code from reading C++ file 'Application.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/Application.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Application.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Application_t {
    uint offsetsAndSizes[126];
    char stringdata0[12];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[7];
    char stringdata5[13];
    char stringdata6[16];
    char stringdata7[10];
    char stringdata8[12];
    char stringdata9[5];
    char stringdata10[13];
    char stringdata11[10];
    char stringdata12[16];
    char stringdata13[10];
    char stringdata14[15];
    char stringdata15[18];
    char stringdata16[15];
    char stringdata17[16];
    char stringdata18[14];
    char stringdata19[4];
    char stringdata20[17];
    char stringdata21[12];
    char stringdata22[25];
    char stringdata23[15];
    char stringdata24[10];
    char stringdata25[11];
    char stringdata26[11];
    char stringdata27[13];
    char stringdata28[10];
    char stringdata29[12];
    char stringdata30[11];
    char stringdata31[7];
    char stringdata32[19];
    char stringdata33[7];
    char stringdata34[7];
    char stringdata35[6];
    char stringdata36[7];
    char stringdata37[8];
    char stringdata38[9];
    char stringdata39[12];
    char stringdata40[7];
    char stringdata41[10];
    char stringdata42[11];
    char stringdata43[11];
    char stringdata44[11];
    char stringdata45[14];
    char stringdata46[8];
    char stringdata47[10];
    char stringdata48[19];
    char stringdata49[24];
    char stringdata50[15];
    char stringdata51[18];
    char stringdata52[16];
    char stringdata53[18];
    char stringdata54[13];
    char stringdata55[4];
    char stringdata56[14];
    char stringdata57[9];
    char stringdata58[5];
    char stringdata59[18];
    char stringdata60[21];
    char stringdata61[21];
    char stringdata62[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Application_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Application_t qt_meta_stringdata_Application = {
    {
        QT_MOC_LITERAL(0, 11),  // "Application"
        QT_MOC_LITERAL(12, 12),  // "bufferOpened"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 8),  // "BufferID"
        QT_MOC_LITERAL(35, 6),  // "buffer"
        QT_MOC_LITERAL(42, 12),  // "bufferClosed"
        QT_MOC_LITERAL(55, 15),  // "bufferActivated"
        QT_MOC_LITERAL(71, 9),  // "fileSaved"
        QT_MOC_LITERAL(81, 11),  // "std::string"
        QT_MOC_LITERAL(93, 4),  // "path"
        QT_MOC_LITERAL(98, 12),  // "themeChanged"
        QT_MOC_LITERAL(111, 9),  // "themeName"
        QT_MOC_LITERAL(121, 15),  // "commandExecuted"
        QT_MOC_LITERAL(137, 9),  // "commandId"
        QT_MOC_LITERAL(147, 14),  // "onBufferOpened"
        QT_MOC_LITERAL(162, 17),  // "onBufferActivated"
        QT_MOC_LITERAL(180, 14),  // "onBufferClosed"
        QT_MOC_LITERAL(195, 15),  // "onBufferChanged"
        QT_MOC_LITERAL(211, 13),  // "onMenuCommand"
        QT_MOC_LITERAL(225, 3),  // "cmd"
        QT_MOC_LITERAL(229, 16),  // "onToolBarCommand"
        QT_MOC_LITERAL(246, 11),  // "onFileSaved"
        QT_MOC_LITERAL(258, 24),  // "onFileModifiedExternally"
        QT_MOC_LITERAL(283, 14),  // "onThemeChanged"
        QT_MOC_LITERAL(298, 9),  // "onNewFile"
        QT_MOC_LITERAL(308, 10),  // "onOpenFile"
        QT_MOC_LITERAL(319, 10),  // "onSaveFile"
        QT_MOC_LITERAL(330, 12),  // "onSaveFileAs"
        QT_MOC_LITERAL(343, 9),  // "onSaveAll"
        QT_MOC_LITERAL(353, 11),  // "onCloseFile"
        QT_MOC_LITERAL(365, 10),  // "onCloseAll"
        QT_MOC_LITERAL(376, 6),  // "onExit"
        QT_MOC_LITERAL(383, 18),  // "onClearRecentFiles"
        QT_MOC_LITERAL(402, 6),  // "onUndo"
        QT_MOC_LITERAL(409, 6),  // "onRedo"
        QT_MOC_LITERAL(416, 5),  // "onCut"
        QT_MOC_LITERAL(422, 6),  // "onCopy"
        QT_MOC_LITERAL(429, 7),  // "onPaste"
        QT_MOC_LITERAL(437, 8),  // "onDelete"
        QT_MOC_LITERAL(446, 11),  // "onSelectAll"
        QT_MOC_LITERAL(458, 6),  // "onFind"
        QT_MOC_LITERAL(465, 9),  // "onReplace"
        QT_MOC_LITERAL(475, 10),  // "onGotoLine"
        QT_MOC_LITERAL(486, 10),  // "onFindNext"
        QT_MOC_LITERAL(497, 10),  // "onFindPrev"
        QT_MOC_LITERAL(508, 13),  // "onFindInFiles"
        QT_MOC_LITERAL(522, 7),  // "onCount"
        QT_MOC_LITERAL(530, 9),  // "onMarkAll"
        QT_MOC_LITERAL(540, 18),  // "onToggleFullScreen"
        QT_MOC_LITERAL(559, 23),  // "onToggleDistractionFree"
        QT_MOC_LITERAL(583, 14),  // "onToggleTabBar"
        QT_MOC_LITERAL(598, 17),  // "onToggleStatusBar"
        QT_MOC_LITERAL(616, 15),  // "onToggleToolBar"
        QT_MOC_LITERAL(632, 17),  // "onConvertEncoding"
        QT_MOC_LITERAL(650, 12),  // "EncodingType"
        QT_MOC_LITERAL(663, 3),  // "enc"
        QT_MOC_LITERAL(667, 13),  // "onSetLanguage"
        QT_MOC_LITERAL(681, 8),  // "LangType"
        QT_MOC_LITERAL(690, 4),  // "lang"
        QT_MOC_LITERAL(695, 17),  // "onShowPreferences"
        QT_MOC_LITERAL(713, 20),  // "onShowShortcutMapper"
        QT_MOC_LITERAL(734, 20),  // "onShowCommandPalette"
        QT_MOC_LITERAL(755, 11)   // "onShowAbout"
    },
    "Application",
    "bufferOpened",
    "",
    "BufferID",
    "buffer",
    "bufferClosed",
    "bufferActivated",
    "fileSaved",
    "std::string",
    "path",
    "themeChanged",
    "themeName",
    "commandExecuted",
    "commandId",
    "onBufferOpened",
    "onBufferActivated",
    "onBufferClosed",
    "onBufferChanged",
    "onMenuCommand",
    "cmd",
    "onToolBarCommand",
    "onFileSaved",
    "onFileModifiedExternally",
    "onThemeChanged",
    "onNewFile",
    "onOpenFile",
    "onSaveFile",
    "onSaveFileAs",
    "onSaveAll",
    "onCloseFile",
    "onCloseAll",
    "onExit",
    "onClearRecentFiles",
    "onUndo",
    "onRedo",
    "onCut",
    "onCopy",
    "onPaste",
    "onDelete",
    "onSelectAll",
    "onFind",
    "onReplace",
    "onGotoLine",
    "onFindNext",
    "onFindPrev",
    "onFindInFiles",
    "onCount",
    "onMarkAll",
    "onToggleFullScreen",
    "onToggleDistractionFree",
    "onToggleTabBar",
    "onToggleStatusBar",
    "onToggleToolBar",
    "onConvertEncoding",
    "EncodingType",
    "enc",
    "onSetLanguage",
    "LangType",
    "lang",
    "onShowPreferences",
    "onShowShortcutMapper",
    "onShowCommandPalette",
    "onShowAbout"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Application[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  314,    2, 0x06,    1 /* Public */,
       5,    1,  317,    2, 0x06,    3 /* Public */,
       6,    1,  320,    2, 0x06,    5 /* Public */,
       7,    1,  323,    2, 0x06,    7 /* Public */,
      10,    1,  326,    2, 0x06,    9 /* Public */,
      12,    1,  329,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    1,  332,    2, 0x0a,   13 /* Public */,
      15,    1,  335,    2, 0x0a,   15 /* Public */,
      16,    1,  338,    2, 0x0a,   17 /* Public */,
      17,    1,  341,    2, 0x0a,   19 /* Public */,
      18,    1,  344,    2, 0x0a,   21 /* Public */,
      20,    1,  347,    2, 0x0a,   23 /* Public */,
      21,    1,  350,    2, 0x0a,   25 /* Public */,
      22,    1,  353,    2, 0x0a,   27 /* Public */,
      23,    1,  356,    2, 0x0a,   29 /* Public */,
      24,    0,  359,    2, 0x0a,   31 /* Public */,
      25,    0,  360,    2, 0x0a,   32 /* Public */,
      26,    0,  361,    2, 0x0a,   33 /* Public */,
      27,    0,  362,    2, 0x0a,   34 /* Public */,
      28,    0,  363,    2, 0x0a,   35 /* Public */,
      29,    0,  364,    2, 0x0a,   36 /* Public */,
      30,    0,  365,    2, 0x0a,   37 /* Public */,
      31,    0,  366,    2, 0x0a,   38 /* Public */,
      32,    0,  367,    2, 0x0a,   39 /* Public */,
      33,    0,  368,    2, 0x0a,   40 /* Public */,
      34,    0,  369,    2, 0x0a,   41 /* Public */,
      35,    0,  370,    2, 0x0a,   42 /* Public */,
      36,    0,  371,    2, 0x0a,   43 /* Public */,
      37,    0,  372,    2, 0x0a,   44 /* Public */,
      38,    0,  373,    2, 0x0a,   45 /* Public */,
      39,    0,  374,    2, 0x0a,   46 /* Public */,
      40,    0,  375,    2, 0x0a,   47 /* Public */,
      41,    0,  376,    2, 0x0a,   48 /* Public */,
      42,    0,  377,    2, 0x0a,   49 /* Public */,
      43,    0,  378,    2, 0x0a,   50 /* Public */,
      44,    0,  379,    2, 0x0a,   51 /* Public */,
      45,    0,  380,    2, 0x0a,   52 /* Public */,
      46,    0,  381,    2, 0x0a,   53 /* Public */,
      47,    0,  382,    2, 0x0a,   54 /* Public */,
      48,    0,  383,    2, 0x0a,   55 /* Public */,
      49,    0,  384,    2, 0x0a,   56 /* Public */,
      50,    0,  385,    2, 0x0a,   57 /* Public */,
      51,    0,  386,    2, 0x0a,   58 /* Public */,
      52,    0,  387,    2, 0x0a,   59 /* Public */,
      53,    1,  388,    2, 0x0a,   60 /* Public */,
      56,    1,  391,    2, 0x0a,   62 /* Public */,
      59,    0,  394,    2, 0x0a,   64 /* Public */,
      60,    0,  395,    2, 0x0a,   65 /* Public */,
      61,    0,  396,    2, 0x0a,   66 /* Public */,
      62,    0,  397,    2, 0x0a,   67 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,   11,
    QMetaType::Void, QMetaType::Int,   13,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,   11,
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
    QMetaType::Void, 0x80000000 | 54,   55,
    QMetaType::Void, 0x80000000 | 57,   58,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Application::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Application.offsetsAndSizes,
    qt_meta_data_Application,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Application_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Application, std::true_type>,
        // method 'bufferOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'bufferClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'bufferActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'fileSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'themeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'commandExecuted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onBufferOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'onBufferActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'onBufferClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'onBufferChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<BufferID, std::false_type>,
        // method 'onMenuCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onToolBarCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onFileSaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'onFileModifiedExternally'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'onThemeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::string &, std::false_type>,
        // method 'onNewFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOpenFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveFileAs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCloseFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCloseAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClearRecentFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUndo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRedo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopy'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPaste'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDelete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReplace'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGotoLine'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFindNext'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFindPrev'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFindInFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCount'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMarkAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onToggleFullScreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onToggleDistractionFree'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onToggleTabBar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onToggleStatusBar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onToggleToolBar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConvertEncoding'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<EncodingType, std::false_type>,
        // method 'onSetLanguage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LangType, std::false_type>,
        // method 'onShowPreferences'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onShowShortcutMapper'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onShowCommandPalette'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onShowAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Application::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Application *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bufferOpened((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 1: _t->bufferClosed((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 2: _t->bufferActivated((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 3: _t->fileSaved((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 4: _t->themeChanged((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 5: _t->commandExecuted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onBufferOpened((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 7: _t->onBufferActivated((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 8: _t->onBufferClosed((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 9: _t->onBufferChanged((*reinterpret_cast< std::add_pointer_t<BufferID>>(_a[1]))); break;
        case 10: _t->onMenuCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onToolBarCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->onFileSaved((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 13: _t->onFileModifiedExternally((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 14: _t->onThemeChanged((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1]))); break;
        case 15: _t->onNewFile(); break;
        case 16: _t->onOpenFile(); break;
        case 17: _t->onSaveFile(); break;
        case 18: _t->onSaveFileAs(); break;
        case 19: _t->onSaveAll(); break;
        case 20: _t->onCloseFile(); break;
        case 21: _t->onCloseAll(); break;
        case 22: _t->onExit(); break;
        case 23: _t->onClearRecentFiles(); break;
        case 24: _t->onUndo(); break;
        case 25: _t->onRedo(); break;
        case 26: _t->onCut(); break;
        case 27: _t->onCopy(); break;
        case 28: _t->onPaste(); break;
        case 29: _t->onDelete(); break;
        case 30: _t->onSelectAll(); break;
        case 31: _t->onFind(); break;
        case 32: _t->onReplace(); break;
        case 33: _t->onGotoLine(); break;
        case 34: _t->onFindNext(); break;
        case 35: _t->onFindPrev(); break;
        case 36: _t->onFindInFiles(); break;
        case 37: _t->onCount(); break;
        case 38: _t->onMarkAll(); break;
        case 39: _t->onToggleFullScreen(); break;
        case 40: _t->onToggleDistractionFree(); break;
        case 41: _t->onToggleTabBar(); break;
        case 42: _t->onToggleStatusBar(); break;
        case 43: _t->onToggleToolBar(); break;
        case 44: _t->onConvertEncoding((*reinterpret_cast< std::add_pointer_t<EncodingType>>(_a[1]))); break;
        case 45: _t->onSetLanguage((*reinterpret_cast< std::add_pointer_t<LangType>>(_a[1]))); break;
        case 46: _t->onShowPreferences(); break;
        case 47: _t->onShowShortcutMapper(); break;
        case 48: _t->onShowCommandPalette(); break;
        case 49: _t->onShowAbout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferOpened; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Application::*)(BufferID );
            if (_t _q_method = &Application::bufferActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Application::*)(const std::string & );
            if (_t _q_method = &Application::fileSaved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Application::*)(const std::string & );
            if (_t _q_method = &Application::themeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Application::*)(int );
            if (_t _q_method = &Application::commandExecuted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *Application::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Application::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Application.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "NonCopyable"))
        return static_cast< NonCopyable*>(this);
    return QObject::qt_metacast(_clname);
}

int Application::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 50)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void Application::bufferOpened(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Application::bufferClosed(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Application::bufferActivated(BufferID _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Application::fileSaved(const std::string & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Application::themeChanged(const std::string & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Application::commandExecuted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
