// MISC/Common/NppConstants.h - Qt6 port of Notepad++ constants
#pragma once

#include <QtGlobal>

// Direction constants
inline constexpr bool dirUp = true;
inline constexpr bool dirDown = false;

// Codepage constants
inline constexpr int NPP_CP_WIN_1252 = 1252;
inline constexpr int NPP_CP_DOS_437 = 437;
inline constexpr int NPP_CP_BIG5 = 950;

// Internal message for plugin communication
#define LINKTRIGGERED (QEvent::User + 555)

// Color constants
inline constexpr QRgb BCKGRD_COLOR = qRgb(255, 102, 102);
inline constexpr QRgb TXT_COLOR = qRgb(255, 255, 255);

// Constants
#define NPP_INTERNAL_FUNCTION_STR QStringLiteral("Notepad++::InternalFunction")

#define IDT_HIDE_TOOLTIP 1001

// UAC signatures for elevated operations
#define NPP_UAC_SAVE_SIGN QStringLiteral("#UAC-SAVE#")
#define NPP_UAC_SETFILEATTRIBUTES_SIGN QStringLiteral("#UAC-SETFILEATTRIBUTES#")
#define NPP_UAC_MOVEFILE_SIGN QStringLiteral("#UAC-MOVEFILE#")
#define NPP_UAC_CREATEEMPTYFILE_SIGN QStringLiteral("#UAC-CREATEEMPTYFILE#")

// Subclass IDs
enum class SubclassID : unsigned int
{
    darkMode = 42,
    first = 666,
    second = 1984
};

// Change history states
enum changeHistoryState
{
    disable,
    margin,
    indicator,
    marginIndicator
};

// Folder styles for document tree
enum folderStyle
{
    FOLDER_TYPE,
    FOLDER_STYLE_SIMPLE,
    FOLDER_STYLE_ARROW,
    FOLDER_STYLE_CIRCLE,
    FOLDER_STYLE_BOX,
    FOLDER_STYLE_NONE
};

// Missing SC_MARKNUM_* constants (Scintilla bundled header only defines SC_MARKNUM_HISTORY_* + SC_MARKNUM_FOLDER*)
// These values follow the standard Scintilla numbering scheme
#define SC_MARKNUM_EMPTY 24
#define SC_MARKNUM_VERTLINE 24
#define SC_MARKNUM_FOLDEREND 25
#define SC_MARKNUM_FOLDEROPENMID 26
#define SC_MARKNUM_FOLDERMIDTAIL 27
#define SC_MARKNUM_FOLDERTAIL 28
#define SC_MARKNUM_FOLDERSUB 29
#define SC_MARKNUM_FOLDER 30
#define SC_MARKNUM_FOLDEROPEN 31
#define SC_MARKNUM_BOXPLUS 0
#define SC_MARKNUM_BOXMINUS 2
#define SC_MARKNUM_BOXPLUSCONNECTED 3
#define SC_MARKNUM_BOXMINUSCONNECTED 4
#define SC_MARKNUM_LINES 6
#define SC_MARKNUM_MINUS 7

// Line wrap methods
enum lineWrapMethod
{
    LINEWRAP_DEFAULT,
    LINEWRAP_ALIGNED,
    LINEWRAP_INDENT
};

// Line highlight modes
enum lineHiliteMode
{
    LINEHILITE_NONE,
    LINEHILITE_HILITE,
    LINEHILITE_FRAME
};

// Fold constants
inline constexpr bool fold_expand = true;
inline constexpr bool fold_collapse = false;
inline constexpr int MODEVENTMASK_OFF = 0;

// Text case conversion modes
enum class TextCase : unsigned char
{
    UPPERCASE,
    LOWERCASE,
    PROPERCASE_FORCE,
    PROPERCASE_BLEND,
    SENTENCECASE_FORCE,
    SENTENCECASE_BLEND,
    INVERTCASE,
    RANDOMCASE
};

// Numeric base
enum class NumBase : unsigned char
{
    BASE_10,
    BASE_16,
    BASE_08,
    BASE_02,
    BASE_16_UPPERCASE
};

// Document map mouse wheel event
inline constexpr int DOCUMENTMAP_MOUSEWHEEL = (QEvent::User + 3);

// Position constants
inline constexpr bool POS_VERTICAL = true;
inline constexpr bool POS_HORIZONTAL = false;

// UDD (User Defined Dialog) states
inline constexpr int UDD_SHOW = 0x01;
inline constexpr int UDD_DOCKED = 0x02;

// Tab states
inline constexpr int TAB_DRAWTOPBAR = 0x0001;
inline constexpr int TAB_DRAWINACTIVETAB = 0x0002;
inline constexpr int TAB_DRAGNDROP = 0x0004;
inline constexpr int TAB_REDUCE = 0x0008;
inline constexpr int TAB_CLOSEBUTTON = 0x0010;
inline constexpr int TAB_DBCLK2CLOSE = 0x0020;
inline constexpr int TAB_VERTICAL = 0x0040;
inline constexpr int TAB_MULTILINE = 0x0080;
inline constexpr int TAB_HIDE = 0x0100;
inline constexpr int TAB_QUITONEMPTY = 0x0200;
inline constexpr int TAB_ALTICONS = 0x0400;
inline constexpr int TAB_PINBUTTON = 0x0800;
inline constexpr int TAB_INACTIVETABSHOWBUTTON = 0x1000;
inline constexpr int TAB_SHOWONLYPINNEDBUTTON = 0x2000;

inline constexpr bool activeText = true;
inline constexpr bool activeNumeric = false;

// EOL types
enum class EolType : quint8
{
    windows,
    macos,
    unix,
    unknown,
    osdefault = windows
};

// Unicode mode
enum UniMode
{
    uni8Bit = 0,
    uniUTF8 = 1,
    uni16BE = 2,
    uni16LE = 3,
    uniUTF8_NoBOM = 4,
    uni7Bit = 5,
    uni16BE_NoBOM = 6,
    uni16LE_NoBOM = 7,
    uniEnd
};

// Change detection modes
enum ChangeDetect
{
    cdDisabled = 0x00,
    cdEnabledOld = 0x01,
    cdEnabledNew = 0x02,
    cdAutoUpdate = 0x04,
    cdGo2end = 0x08
};

// Backup feature modes
enum BackupFeature
{
    bak_none,
    bak_simple,
    bak_verbose
};

// Open/Save directory settings
enum OpenSaveDirSetting
{
    dir_followCurrent,
    dir_last,
    dir_userDef
};

// Multi-instance settings
enum MultiInstSetting
{
    monoInst,
    multiInstOnSession,
    multiInst
};

// URL modes
enum urlMode
{
    urlDisable,
    urlNoUnderLineFg,
    urlUnderLineFg,
    urlNoUnderLineBg,
    urlUnderLineBg,
    urlMin = urlDisable,
    urlMax = urlUnderLineBg
};

// Auto-indent modes
enum AutoIndentMode
{
    autoIndent_none,
    autoIndent_advanced,
    autoIndent_basic
};

// System tray actions
enum SysTrayAction
{
    sta_none,
    sta_minimize,
    sta_close,
    sta_minimize_close
};

// Copy data parameters
enum CopyDataParam
{
    COPYDATA_PARAMS = 0,
    COPYDATA_FILENAMESW = 2,
    COPYDATA_FULL_CMDLINE = 3
};

// Decimal separator options
enum UdlDecSep
{
    DECSEP_DOT,
    DECSEP_COMMA,
    DECSEP_BOTH
};

// Default file size limit for styling (200MB)
inline constexpr int NPP_STYLING_FILESIZE_LIMIT_DEFAULT = (200 * 1024 * 1024);

// Find/replace maximum length
inline constexpr int FINDREPLACE_MAXLENGTH = 16384;
inline constexpr int FINDREPLACE_INSELECTION_THRESHOLD_DEFAULT = 1024;
inline constexpr int FILL_FINDWHAT_THRESHOLD_DEFAULT = 1024;

// Font sizes
inline constexpr const wchar_t fontSizeStrs[][3] = {
    L"", L"5", L"6", L"7", L"8", L"9", L"10", L"11", L"12", L"14", L"16", L"18", L"20", L"22", L"24", L"26", L"28"
};

// Font styles
enum FontStyle
{
    FONTSTYLE_NONE = 0,
    FONTSTYLE_BOLD = 1,
    FONTSTYLE_ITALIC = 2,
    FONTSTYLE_UNDERLINE = 4
};

// Style constants
inline constexpr int STYLE_NOT_USED = -1;
inline constexpr int COLORSTYLE_FOREGROUND = 0x01;
inline constexpr int COLORSTYLE_BACKGROUND = 0x02;
inline constexpr int COLORSTYLE_ALL = COLORSTYLE_FOREGROUND | COLORSTYLE_BACKGROUND;

// Default colors for light/dark themes
inline constexpr QRgb g_cDefaultMainDark = qRgb(0xDE, 0xDE, 0xDE);
inline constexpr QRgb g_cDefaultSecondaryDark = qRgb(0x4C, 0xC2, 0xFF);
inline constexpr QRgb g_cDefaultMainLight = qRgb(0x21, 0x21, 0x21);
inline constexpr QRgb g_cDefaultSecondaryLight = qRgb(0x00, 0x78, 0xD4);

// Fluent color enum
enum class FluentColor
{
    defaultColor,
    red,
    green,
    blue,
    purple,
    cyan,
    olive,
    yellow,
    accent,
    custom,
    maxValue
};

// List and tab limits
inline constexpr int NB_LIST = 20;
inline constexpr int NB_MAX_LRF_FILE = 30;
inline constexpr int NB_MAX_USER_LANG = 30;
inline constexpr int NB_MAX_EXTERNAL_LANG = 30;
inline constexpr int NB_MAX_IMPORTED_UDL = 50;
inline constexpr int NB_DEFAULT_LRF_CUSTOMLENGTH = 100;
inline constexpr int NB_MAX_LRF_CUSTOMLENGTH = 260; // MAX_PATH - 1

// Tab display limits
inline constexpr int NB_MAX_TAB_COMPACT_LABEL_LEN = 257; // MAX_PATH - 3

// External lexer name max length
inline constexpr int MAX_EXTERNAL_LEXER_NAME_LEN = 128;

// Recent files display modes
inline constexpr int RECENTFILES_SHOWFULLPATH = -1;
inline constexpr int RECENTFILES_SHOWONLYFILENAME = 0;

// Network issue log filename
inline constexpr const wchar_t nppLogNetworkDriveIssue[] = L"nppLogNetworkDriveIssue";

// Tab bar dimensions
inline constexpr int g_TabIconSize = 16;
inline constexpr int g_TabHeight = 22;
inline constexpr int g_TabHeightLarge = 25;
inline constexpr int g_TabWidth = 45;
inline constexpr int g_TabWidthButton = 60;
inline constexpr int g_TabCloseBtnSize = 11;
inline constexpr int g_TabPinBtnSize = 11;
inline constexpr int g_TabCloseBtnSize_DM = 16;
inline constexpr int g_TabPinBtnSize_DM = 16;

// Style names
inline constexpr const wchar_t TABBAR_ACTIVEFOCUSEDINDCATOR[] = L"Active tab focused indicator";
inline constexpr const wchar_t TABBAR_ACTIVEUNFOCUSEDINDCATOR[] = L"Active tab unfocused indicator";
inline constexpr const wchar_t TABBAR_ACTIVETEXT[] = L"Active tab text";
inline constexpr const wchar_t TABBAR_INACTIVETEXT[] = L"Inactive tabs";

inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_1[] = L"Tab color 1";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_2[] = L"Tab color 2";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_3[] = L"Tab color 3";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_4[] = L"Tab color 4";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_5[] = L"Tab color 5";

inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_DM_1[] = L"Tab color dark mode 1";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_DM_2[] = L"Tab color dark mode 2";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_DM_3[] = L"Tab color dark mode 3";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_DM_4[] = L"Tab color dark mode 4";
inline constexpr const wchar_t TABBAR_INDIVIDUALCOLOR_DM_5[] = L"Tab color dark mode 5";

inline constexpr const wchar_t VIEWZONE_DOCUMENTMAP[] = L"Document map";

inline constexpr const wchar_t FINDDLG_STAUSNOTFOUND_COLOR[] = L"Find status: Not found";
inline constexpr const wchar_t FINDDLG_STAUSMESSAGE_COLOR[] = L"Find status: Message";
inline constexpr const wchar_t FINDDLG_STAUSREACHED_COLOR[] = L"Find status: Search end reached";

inline constexpr const wchar_t g_npcStyleName[] = L"Non-printing characters custom color";

// Maximum line length for user-defined languages
inline constexpr int langNameLenMax = 64;
inline constexpr int extsLenMax = 256;
inline constexpr int max_char = 30720; // 1024 * 30

// Shortcut XML content
inline constexpr const char SHORTCUT_XML_CONTENT[] = R"(<?xml version="1.0" encoding="UTF-8" ?>
<NotepadPlus>
	<InternalCommands />
	<Macros />
	<UserDefinedCommands />
	<PluginCommands />
	<ScintillaKeys />
</NotepadPlus>
)";

// Context menu XML content
inline constexpr const char CONTEXTMENU_XML_CONTENT[] = R"(<?xml version="1.0" encoding="UTF-8" ?>
<NotepadPlus>
	<ScintillaContextMenu />
</NotepadPlus>
)";