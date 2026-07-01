// NppConstants — shared constants for the NPP port
// Extracted from NppConstants.h in source
#pragma once

#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtGui/qrgb.h>   // QRgb used by COLORREF typedef
#include <QtCore/QString>
#include <cstdarg>

// Forward declarations for types used across npp-qt
class ScintillaComponent;       // defined in ScintillaComponent.h
using ScintillaEditView = ScintillaComponent;  // canonical alias (also in ScintillaComponent.h)
class NppGUI;
class NppParameters;

// Internal function/module name sentinel for docking data (mirrors Win32 definition)
inline constexpr wchar_t NPP_INTERNAL_FUNCTION_STR[] = L"NPP_INTERNAL_FUNCTION";

// Scintilla markers
inline constexpr int MARKER_NEW = 0;
inline constexpr int MARKER_UNPIN = 1;
inline constexpr int MARKER_BOOKMARK = 2;
inline constexpr int MARKER_FOLDER = 3;
inline constexpr int MARKER_FOLDER_OPEN = 4;
inline constexpr int MARKER_FOLDER_END = 5;
inline constexpr int MARKER_FOLDER_SUB = 6;
inline constexpr int MARKER_FOLDER_EMPTY = 7;
inline constexpr int MARKER_HIDELINESBEGIN = 8;
inline constexpr int MARKER_HIDELINESUNDERLINE = 9;
inline constexpr int MARKER_FINDMARK_ALL = 10;
inline constexpr int MARKER_FINDMARK_1 = 11;
inline constexpr int MARKER_FINDMARK_2 = 12;
inline constexpr int MARKER_FINDMARK_3 = 13;
inline constexpr int MARKER_FINDMARK_4 = 14;
inline constexpr int MARKER_FINDMARK_5 = 15;
inline constexpr int MARKER_FINDMARK_LOG_1 = 16;
inline constexpr int MARKER_FINDMARK_LOG_2 = 17;
inline constexpr int MARKER_FINDMARK_LOG_3 = 18;
inline constexpr int MARKER_FINDMARK_LOG_4 = 19;
inline constexpr int MARKER_FINDMARK_LOG_5 = 20;
inline constexpr int MARKER_INDICATOR = 21;
inline constexpr int MARKER_FOLD_MARK = 22;
inline constexpr int MARKER_PLUGINS = 24;

// Scintilla indicator
inline constexpr int INDICATOR_CONTAINER = 0;
inline constexpr int INDICATOR_HISTORYMARK = 1;
inline constexpr int INDICATOR_FIND_MATCH = 2;
inline constexpr int INDICATOR_FIND_MATCH_2 = 3;
inline constexpr int INDICATOR_FIND_MATCH_3 = 4;
inline constexpr int INDICATOR_FIND_MATCH_4 = 5;
inline constexpr int INDICATOR_FIND_MATCH_5 = 6;
inline constexpr int INDICATOR_WORD_PART = 7;
inline constexpr int INDICATOR_URL = 8;
inline constexpr int INDICATOR_IME = 9;
inline constexpr int INDICATOR_IME_LONG = 10;
inline constexpr int INDICATOR_PLUGINS = 30;

// Scintilla margins
inline constexpr int MARGIN_FOLD_INDEX = 2;
inline constexpr int MARGIN_FOLDER_INDEX = 1;
inline constexpr int MARGIN_BOOKMARK_INDEX = 0;
inline constexpr int MARGIN_SNUMBER_INDEX = -1;

// NPPN_* notification codes
inline constexpr int NPPN_FIRST = 1000;
inline constexpr int NPPN_READY = NPPN_FIRST + 1;
inline constexpr int NPPN_TB_MODIFICATION = NPPN_FIRST + 2;
inline constexpr int NPPN_FILEBEFORECLOSE = NPPN_FIRST + 3;
inline constexpr int NPPN_FILECLOSED = NPPN_FIRST + 4;
inline constexpr int NPPN_FILEOPENED = NPPN_FIRST + 5;
inline constexpr int NPPN_FILEBEFOREOPEN = NPPN_FIRST + 6;
inline constexpr int NPPN_BUFFERACTIVATED = NPPN_FIRST + 7;
inline constexpr int NPPN_LANGCHANGED = NPPN_FIRST + 8;
inline constexpr int NPPN_READONLYCHANGED = NPPN_FIRST + 16;
inline constexpr int NPPN_WORDSTYLECHANGED = NPPN_FIRST + 9;
inline constexpr int NPPN_SHORTCUTREMAPPED = NPPN_FIRST + 10;
inline constexpr int NPPN_READCONFIGS = NPPN_FIRST + 11;
inline constexpr int NPPN_AREYOUSEARCHING = NPPN_FIRST + 12;
inline constexpr int NPPN_DIRLISTING = NPPN_FIRST + 13;
inline constexpr int NPPN_SESSIONLOADED = NPPN_FIRST + 14;
inline constexpr int NPPN_SESSIONCLOSED = NPPN_FIRST + 15;
inline constexpr int NPPN_SESSIONSAVE = NPPN_FIRST + 16;
inline constexpr int NPPN_DARKMODECHANGED = NPPN_FIRST + 17;
inline constexpr int NPPN_SOFTRECALC = NPPN_FIRST + 18;
inline constexpr int NPPN_DOCMODIFIED = NPPN_FIRST + 19;
inline constexpr int NPPN_FILEBEFORESAVE = NPPN_FIRST + 20;
inline constexpr int NPPN_FILESAVED = NPPN_FIRST + 21;
inline constexpr int NPPN_UPDATEUI = NPPN_FIRST + 22;
inline constexpr int NPPN_CURRENTSITZCHANGED = NPPN_FIRST + 23;
inline constexpr int NPPN_DOCORDERCHANGED = NPPN_FIRST + 24;
inline constexpr int NPPN_SNAPSHOTDIRTYFILELOADED = NPPN_FIRST + 25;
inline constexpr int NPPN_QUERYMODIFIED = NPPN_FIRST + 26;
inline constexpr int NPPN_CANCEL = NPPN_FIRST + 27;
inline constexpr int NPPN_FILEDELETED = NPPN_FIRST + 28;
inline constexpr int NPPN_FILEBEFORELOAD = NPPN_FIRST + 30;
inline constexpr int NPPN_FILELOADFAILED = NPPN_FIRST + 31;

// Buffer ID sentinel
inline constexpr int INVALID_BUFFER_ID = -1;
inline constexpr int INVALID_DOCUMENT_INDEX = -1;

// Scintilla style not used
inline constexpr int STYLE_NOT_USED = 32;

// WriteTechnologyEngine
inline constexpr int SC_TECH_DEFAULT = 0;
inline constexpr int SC_TECH_WIN32 = 1;

// Style hints / constants
inline constexpr int FONTSTYLE_NONE = 0;
inline constexpr int FONTSTYLE_BOLD = 1;
inline constexpr int FONTSTYLE_ITALIC = 2;
inline constexpr int FONTSTYLE_UNDERLINE = 4;
inline constexpr int FONTSTYLE_HIDDEN = 8;
inline constexpr int FONTSTYLE_HEAVY = 16;
inline constexpr int FONTSTYLE_BOLD_ITALIC = 3;
inline constexpr int FONTSTYLE_BOLD_UNDERLINE = 5;
inline constexpr int FONTSTYLE_ITALIC_UNDERLINE = 6;
inline constexpr int FONTSTYLE_BOLD_ITALIC_UNDERLINE = 7;

inline constexpr int TAB_REDUCE = 0;
inline constexpr int TAB_PINBUTTON = 1;
inline constexpr int TAB_DRAWTOPBAR = 2;
inline constexpr int TAB_DRAWINACTIVETAB = 3;
inline constexpr int TAB_DRAGNDROP = 4;
inline constexpr int TAB_CLOSEBUTTON = 5;
inline constexpr int TAB_REBAR = 6;
inline constexpr int TAB_ALTICONS = 0x0400;    // 0000 0100 0000 0000

// Menu item name buffer max length (128 = 64 + 64 for localization compatibility)
inline constexpr int menuItemStrLenMax = 64 + 64;

// Uni mode constants
inline constexpr int uniUTF8_NoBOM = 0;
inline constexpr int uniUTF8_BOM = 1;
inline constexpr int uniCookie = 2;
inline constexpr int uni16BE_NoBOM = 3;
inline constexpr int uni16BE_BOM = 4;
inline constexpr int uni16LE_NoBOM = 5;
inline constexpr int uni16LE_BOM = 6;
inline constexpr int uni7BIT = 7;
inline constexpr int uni8BIT = 8;

// NPP constants
inline constexpr int NPP_STYLING_FILESIZE_LIMIT_DEFAULT = 1048576;  // 1 MB
inline constexpr int NB_MAX_USER_LANG = 150;
inline constexpr int NB_MAX_EXTERNAL_LANG = 40;
inline constexpr int NB_MAX_LRF_FILE = 10;
inline constexpr int NB_LIST = 0;
inline constexpr int HIGHCAPTION = 0;
inline constexpr int HIGH_CAPTION = 0;
inline constexpr int CDENABLEDNEW = 0;
inline constexpr int CDENABLEDOLD = 1;
inline constexpr int CDGO2END = 2;
inline constexpr int CDAUTOUPDATE = 4;

// Find/Replace defaults
inline constexpr int FINDREPLACE_INSELECTION_THRESHOLD_DEFAULT = 0;
inline constexpr int FILL_FINDWHAT_THRESHOLD_DEFAULT = 0;

// Recent file list
inline constexpr int RECENTFILES_SHOWFULLPATH = 0;

// UDD docked
inline constexpr int UDD_DOCKED = 2;
inline constexpr int UDD_SHOW = 1;

// Position
inline constexpr int POS_VERTICAL = 0;
inline constexpr int POS_HORIZONTAL = 1;

// Rebar IDs (stubbed — no REBAR in Qt6)
inline constexpr int REBAR_BAR_TOOLBAR = 0;

// Virtual key codes used in ShortcutMapper & shortcut.h

// Virtual key codes used in ShortcutMapper & shortcut.h

// Platform
struct Platform {};

// Undo
inline constexpr int UDD_DOCKED_UNDO = 0;

// Virtual key codes (Win32 VK_* → Qt::Key_* mappings)

// Letter key virtual key codes (missing from above, needed by Parameters.cpp)

// Win32 type aliases for cross-platform compatibility
using DWORD = unsigned long;
using LONG = long;
using ULONG = unsigned long;
using INT = int;
using UINT = unsigned int;
using WPARAM = quint64;
using LPARAM = quint64;
using LRESULT = quint64;

// Win32 dialog result codes
inline constexpr int IDOK = 1;
inline constexpr int IDCANCEL = 2;
inline constexpr int IDYES = 6;
inline constexpr int IDNO = 7;
inline constexpr int IDRETRY = 4;
inline constexpr int IDABORT = 3;
inline constexpr int IDIGNORE = 5;
inline constexpr int IDCLOSE = 8;

// Win32 GetMenuString stub (always returns empty)
inline int GetMenuString(int /*hMenu*/, unsigned int, char*, int, unsigned int) { return 0; }
inline int GetMenuString(int /*hMenu*/, unsigned int, wchar_t*, int, unsigned int) { return 0; }
using HWND = void*;
using HANDLE = void*;
using HINSTANCE = void*;
using HMODULE = void*;
using HDC = void*;
using HPEN = void*;
using HBRUSH = void*;
using HRGN = void*;
using HFONT = void*;
using HBITMAP = void*;
using HMENU = void*;
using ATOM = unsigned short;
using WORD = unsigned short;
using CHAR = char;
using WCHAR = wchar_t;
using LPSTR = char*;
using LPCSTR = const char*;
using LPWSTR = wchar_t*;
using LPCWSTR = const wchar_t*;
using LPTSTR = wchar_t*;
using LPCTSTR = const wchar_t*;
using BOOL = int;
using BYTE = unsigned char;
using FLOAT = float;
using INT_PTR = intptr_t;
using UINT_PTR = uintptr_t;
using LONG_PTR = intptr_t;
using ULONG_PTR = uintptr_t;
using SIZE_T = size_t;
using PSIZE_T = size_t*;
using DWORD_PTR = quint64;
#define TRUE 1
#define FALSE 0
#define MAX_PATH 260
#define INVALID_HANDLE_VALUE nullptr

// Calling convention shim (no-op on non-Windows)
#ifndef WINAPI
#define WINAPI
#endif
#ifndef CALLBACK
#define CALLBACK
#endif

// IDC_* command IDs used by Parameters.cpp for shortcut bindings
// (mirrors Win32 IDC_* resource IDs — values must not clash with IDM_* menu commands)
constexpr int IDC_PREV_DOC                  = 0x10000001;
constexpr int IDC_NEXT_DOC                  = 0x10000002;
constexpr int IDC_EDIT_TOGGLEMACRORECORDING = 0x10000003;
constexpr int IDC_CHECK_TAB_ALTICONS        = 0x10000004;  // TabBar alternate icons preference

// Win32 error codes
inline constexpr DWORD ERROR_SUCCESS = 0;
inline constexpr DWORD ERROR_FILE_NOT_FOUND = 2;
inline constexpr DWORD ERROR_FILE_EXISTS = 80;
inline constexpr DWORD ERROR_PATH_NOT_FOUND = 3;

// Win32 file constants
inline constexpr DWORD FILE_ATTRIBUTE_DIRECTORY = 0x10;
inline constexpr DWORD FILE_ATTRIBUTE_HIDDEN = 0x02;
inline constexpr DWORD FILE_ATTRIBUTE_READONLY = 0x01;
inline constexpr DWORD FILE_ATTRIBUTE_SYSTEM = 0x04;
inline constexpr DWORD FILE_ATTRIBUTE_NORMAL = 0x80;
inline constexpr DWORD INVALID_FILE_ATTRIBUTES = static_cast<DWORD>(-1);
inline constexpr DWORD IMAGE_FILE_MACHINE_I386 = 0x014c;

// CopyFile flags
inline constexpr DWORD MOVEFILE_REPLACE_EXISTING = 0x01;
inline constexpr DWORD MOVEFILE_COPY_ALLOWED = 0x02;
inline constexpr DWORD MOVEFILE_WRITE_THROUGH = 0x08;

// Platform architecture flags (original: NppConstants.h)
inline constexpr int PF_UNKNOWN = 0;
inline constexpr int PF_X86     = 100;
inline constexpr int PF_X64    = 101;
inline constexpr int PF_IA64   = 102;
inline constexpr int PF_ARM64  = 103;

// LangType enum — original Win32 ordering from N++ Notepad_plus_msgs.h
// Plain enum (not enum class) so L_TEXT, L_PHP, etc. are in enclosing scope
enum LangType {
    L_TEXT = 0,        L_PHP = 1,      L_C = 2,          L_CPP = 3,
    L_CS = 4,          L_OBJC = 5,     L_JAVA = 6,       L_RC = 7,
    L_HTML = 8,        L_XML = 9,      L_MAKEFILE = 10,  L_PASCAL = 11,
    L_BATCH = 12,      L_INI = 13,     L_ASCII = 14,     L_USER = 15,
    L_ASP = 16,        L_SQL = 17,     L_VB = 18,        L_JS_EMBEDDED = 19,
    L_CSS = 20,        L_PERL = 21,    L_PYTHON = 22,    L_LUA = 23,
    L_TEX = 24,        L_FORTRAN = 25, L_BASH = 26,      L_FLASH = 27,
    L_NSIS = 28,       L_TCL = 29,     L_LISP = 30,      L_SCHEME = 31,
    L_ASM = 32,        L_DIFF = 33,    L_PROPS = 34,     L_PS = 35,
    L_RUBY = 36,       L_SMALLTALK = 37, L_VHDL = 38,    L_KIX = 39,
    L_AU3 = 40,        L_CAML = 41,    L_ADA = 42,       L_VERILOG = 43,
    L_MATLAB = 44,     L_HASKELL = 45, L_INNO = 46,      L_SEARCHRESULT = 47,
    L_CMAKE = 48,      L_YAML = 49,    L_COBOL = 50,     L_GUI4CLI = 51,
    L_D = 52,          L_POWERSHELL = 53, L_R = 54,      L_JSP = 55,
    L_COFFEESCRIPT = 56, L_JSON = 57,  L_JAVASCRIPT = 58, L_FORTRAN_77 = 59,
    L_BAANC = 60,      L_SREC = 61,    L_IHEX = 62,      L_TEHEX = 63,
    L_SWIFT = 64,      L_ASN1 = 65,   L_AVS = 66,        L_BLITZBASIC = 67,
    L_PUREBASIC = 68,  L_FREEBASIC = 69, L_CSOUND = 70,  L_ERLANG = 71,
    L_ESCRIPT = 72,    L_FORTH = 73,   L_LATEX = 74,     L_MMIXAL = 75,
    L_NIM = 76,        L_NNCRONTAB = 77, L_OSCRIPT = 78, L_REBOL = 79,
    L_REGISTRY = 80,   L_RUST = 81,    L_SPICE = 82,     L_TXT2TAGS = 83,
    L_VISUALPROLOG = 84, L_TYPESCRIPT = 85, L_JSON5 = 86, L_MSSQL = 87,
    L_GDSCRIPT = 88,   L_HOLLYWOOD = 89, L_GOLANG = 90,  L_RAKU = 91,
    L_TOML = 92,       L_SAS = 93,     L_ERRORLIST = 94, L_EXTERNAL = 95,
};

// Windows version enum (from N++ Notepad_plus_msgs.h)
enum class WinVersion {
    WV_UNKNOWN = 0, WV_WIN32S = 1, WV_95 = 2, WV_98 = 3, WV_ME = 4,
    WV_NT = 5, WV_W2K = 6, WV_XP = 7, WV_S2003 = 8, WV_XPX64 = 9,
    WV_VISTA = 10, WV_WIN7 = 11, WV_WIN8 = 12, WV_WIN81 = 13,
    WV_WIN10 = 14, WV_WIN11 = 15
};
enum winVer {
    WV_UNKNOWN = 0, WV_WIN32S = 1, WV_95 = 2, WV_98 = 3, WV_ME = 4,
    WV_NT = 5, WV_W2K = 6, WV_XP = 7, WV_S2003 = 8, WV_XPX64 = 9,
    WV_VISTA = 10, WV_WIN7 = 11, WV_WIN8 = 12, WV_WIN81 = 13,
    WV_WIN10 = 14, WV_WIN11 = 15
};
extern WinVersion currentWinVer;
inline WinVersion winVersion() { return currentWinVer; }

// LangIdxStyle — Scintilla lexer keyword list index (original: NppConstants.h LangIdxStyle enum)
enum LangIdxStyle {
    LANG_INDEX_INSTR = 0,
    LANG_INDEX_INSTR2 = 1,
    LANG_INDEX_TYPE = 2,
    LANG_INDEX_TYPE2 = 3,
    LANG_INDEX_TYPE3 = 4,
    LANG_INDEX_TYPE4 = 5,
    LANG_INDEX_TYPE5 = 6,
    LANG_INDEX_TYPE6 = 7,
    LANG_INDEX_TYPE7 = 8,
    LANG_INDEX_SUBSTYLE1 = 9,
    LANG_INDEX_SUBSTYLE2 = 10,
    LANG_INDEX_SUBSTYLE3 = 11,
    LANG_INDEX_SUBSTYLE4 = 12,
    LANG_INDEX_SUBSTYLE5 = 13,
    LANG_INDEX_SUBSTYLE6 = 14,
    LANG_INDEX_SUBSTYLE7 = 15,
    LANG_INDEX_SUBSTYLE8 = 16
};

// EolType — line ending format
enum class EolType { EolWindows = 0, EolUnix = 1, EolMac = 2, unknown = -1, osdefault = 0 };

// ToolBarStatusType is defined in ToolBar.h (enum class ToolBarDisplayType + using alias)

// NumBase enum is defined in Parameters.h (with BASE_10, BASE_16, BASE_16_UPPERCASE, BASE_08, BASE_02)

// Line wrap method
enum class LineWrapMethod { WrapNone = 0, WrapByFont = 1, WrapByWindow = 2 };
enum lineWrapMethod { WrapNone = 0, WrapByFont = 1, WrapByWindow = 2 };
inline constexpr int LINEWRAP_ALIGNED = 3;   // Scintilla SC_WRAPINDENT_FIXED

// Line highlight mode
enum class LineHiliteMode { LineHiliteNone = 0, LineHiliteCurrent = 1, LineHiliteAll = 2,
    LINEHILITE_NONE = 0, LINEHILITE_HILITE = 1, LINEHILITE_FRAME = 2 };
enum lineHiliteMode { LineHiliteNone = 0, LineHiliteCurrent = 1, LineHiliteAll = 2,
    LINEHILITE_NONE = 0, LINEHILITE_HILITE = 1, LINEHILITE_FRAME = 2 };

// Folder style
enum class FolderStyle { FolderSymbolNone = 0, FolderSymbolArrow = 1, FolderSymbolBox = 2, FolderSymbolCircle = 3 };
enum folderStyle {
    FOLDER_TYPE = 0,
    FOLDER_STYLE_SIMPLE = 1,
    FOLDER_STYLE_ARROW = 2,
    FOLDER_STYLE_CIRCLE = 3,
    FOLDER_STYLE_BOX = 4,
    FOLDER_STYLE_NONE = 5,
    // aliases
    FolderSymbolNone = 0, FolderSymbolArrow = 1, FolderSymbolBox = 2, FolderSymbolCircle = 3
};

// URL mode
enum class UrlMode { URL_NONE = 0, URL_HTML = 1, URL_XML = 2, URL_JS = 4, URL_CSS = 8 };
enum urlMode {
    urlDisable = 0,
    urlNoUnderLineFg = 1,
    urlUnderLineFg = 2,
    urlNoUnderLineBg = 3,
    urlUnderLineBg = 4,
    URL_NONE = 0,
    URL_HTML = 1,
    URL_XML = 2,
    URL_JS = 4,
    URL_CSS = 8,
    urlMin = urlDisable,
    urlMax = urlUnderLineBg
};

// Change history state
enum class ChangeHistoryState { CHG_HIST_DISABLED = 0, CHG_HIST_ENABLED = 1, CHG_HIST_PERSISTENT = 2 };
enum changeHistoryState { CHG_HIST_DISABLED = 0, CHG_HIST_ENABLED = 1, CHG_HIST_PERSISTENT = 2 };

// Open/Save dir setting
enum class OpenSaveDirSetting { USER_DEFAULT_DIR = 0, USER_DEFINE_DIR = 1, ORIGINAL_DIR = 2 };
// dir_last is the "remember last" directory option (maps to USER_DEFINE_DIR)
inline constexpr OpenSaveDirSetting dir_last = OpenSaveDirSetting::USER_DEFINE_DIR;

// Multi instance — defined inside NppGUI struct in Parameters.h (NppGUI::MultiInstSetting)
inline constexpr int monoInst = 0;   // MultiInstSetting::MONO_INSTANCE
inline constexpr int multiInst = 1; // MultiInstSetting::MULTI_INSTANCE
inline constexpr int panamaInst = 2; // MultiInstSetting::PANAMA_INSTANCE (was PANAMA_INSTANCE)

// Backup feature
enum class BackupFeature { BackupNone = 0, BackupSimple = 1, BackupVerbose = 2 };

// Auto indent mode
enum class AutoIndentMode { AI_NONE = 0, AI_BASIC = 1, AI_SMART = 2 };

// External lexer auto indent
enum class ExternalLexerAutoIndentMode { EXTLexAi_None = 0, EXTLexAi_Param = 1, EXTLexAi_aggressive = 2, Standard = 0, Custom = 3, C_Like = 4 };

// Write technology engine — plain enum (used in NppGUI as writeTechnologyEngine)
// Guard against any Scintilla/QScintilla macro definitions of SC_TECH_*
// (some older Scintilla versions used these as macros)
#if defined(SC_TECH_DEFAULT) || defined(SC_TECH_WIN32)
#error "SC_TECH_* macros conflict with NppConstants writeTechnologyEngine enum"
#endif

enum writeTechnologyEngine {
    directWriteTechnology = 0,
    directWriteTechnologyUnavailable = 1,
};

// Dec sep
enum class DecSep { DECSEP_DOT = 0, DECSEP_COMMA = 1 };

// Color style
enum class ColorStyle { COLORSTYLE_NONE = 0, COLORSTYLE_ALL = 1 };

// Update at exit
enum class UpdateAtExit { UA_DONT_SAVE = 0, UA_SAVE = 1, UA_SAVEALL = 2 };

// FluentColor is defined in WinControls/ImageListSet.h
// (original Win32 location, kept there for the toolbar icon system)

// N++ app state
enum class NppState { sta_none = 0, sta_ready = 1, sta_shuttingdown = 2 };
inline constexpr int sta_none = 0;

// Backup
inline constexpr int bak_none = 0;

// Directory follow
inline constexpr int dir_followCurrent = 0;

// Decimal separator
inline constexpr int DECSEP_DOT = 0;
inline constexpr int DECSEP_COMMA = 1;

// Color style
inline constexpr int COLORSTYLE_ALL = 1;

// =============================================================================
// Additional Win32/Linux stubs for Qt6 port
// =============================================================================

// Win32 type stubs
using UCHAR = unsigned char;
using COLORREF = QRgb;          // 0x00BBGGRR
using HDROP = void*;
using HACCEL = void*;           // accelerator table handle stub
using HICON = void*;            // icon handle stub
using LPNMTOOLBARW = void*;     // NMTOOLBARW stub

// Menu flags (Windows stubs)
constexpr unsigned int MF_BYCOMMAND = 0;

// Scintilla marker macros — now exclusively via npp_sci namespace in NppSciCompat.h
// (using namespace npp_sci brings SCI_MARKERADD/DELETE/GET/DELETEALL into global scope)

// Views
constexpr int MAIN_VIEW = 0;
constexpr int SUB_VIEW = 1;

// Sci_CharacterRangeFull — defined in NppSciCompat.h (Sci_CharacterRangeFull struct)
// and in src/Scintilla.h. NppSciCompat.h uses #pragma once + SCI_CHARACTERRANGEFULL_DEFINED
// guard so the struct is only defined once.

// Stub forward declarations for missing dialog classes
class FindInFinderDlg;
class FindIncrementDlg;
class HashFromFilesDlg;
class HashFromTextDlg;
class DocumentPeeker;
struct FindersInfo {
    void* dummy = nullptr;
    const char* _pFileName = nullptr;
    // Qt-port additions
    class FindInFinderDlg* _pDestFinder = nullptr;
    class FindInFinderDlg* _pSourceFinder = nullptr;  // added for Qt6 port
    struct FindOption {
        bool _isRecursive = false;
        bool _isInHiddenDir = false;
        bool _dotDotMatch = false;
        bool _isMatchCase = false;
        bool _wholeWord = false;
        QString _dirFilter;
        std::wstring _str2Search;  // search string (added for Qt6 port)
    } _findOption;
    QString _findRes;
    int _nbMatch = 0;
};

// TrayIconController (corrected spelling)
class TrayIconController;

// Windows message constants — standard messages (0x0000–0x0400 range)
// These are normally defined by <windows.h>; defined here for Linux builds.
// NPP-specific WM_* constants are defined in ScintillaComponent.h.
// Using inline constexpr so multiple definitions across TU (via headers) are allowed.
#ifndef WM_CREATE
inline constexpr unsigned int WM_NULL              = 0x0000;
inline constexpr unsigned int WM_CREATE            = 0x0001;
inline constexpr unsigned int WM_DESTROY            = 0x0002;
inline constexpr unsigned int WM_MOVE              = 0x0003;
inline constexpr unsigned int WM_SIZE              = 0x0005;
inline constexpr unsigned int WM_ACTIVATE          = 0x0006;
inline constexpr unsigned int WM_SETFOCUS          = 0x0007;
inline constexpr unsigned int WM_KILLFOCUS         = 0x0008;
inline constexpr unsigned int WM_ENABLE            = 0x000A;
inline constexpr unsigned int WM_SETREDRAW         = 0x000B;
inline constexpr unsigned int WM_SETTEXT           = 0x000C;
inline constexpr unsigned int WM_GETTEXT           = 0x000D;
inline constexpr unsigned int WM_GETTEXTLENGTH    = 0x000E;
inline constexpr unsigned int WM_PAINT             = 0x000F;
inline constexpr unsigned int WM_CLOSE             = 0x0010;
inline constexpr unsigned int WM_QUERYENDSESSION   = 0x0011;
inline constexpr unsigned int WM_QUIT              = 0x0012;
inline constexpr unsigned int WM_QUERYOPEN         = 0x0013;
inline constexpr unsigned int WM_ERASEBKGND        = 0x0014;
inline constexpr unsigned int WM_ENDSESSION        = 0x0016;
inline constexpr unsigned int WM_SHOWWINDOW        = 0x0018;
inline constexpr unsigned int WM_SETTINGCHANGE     = 0x001A;
inline constexpr unsigned int WM_ACTIVATEAPP      = 0x001C;
inline constexpr unsigned int WM_FONTCHANGE        = 0x001D;
inline constexpr unsigned int WM_TIMECHANGE         = 0x001E;
inline constexpr unsigned int WM_NCCREATE          = 0x0081;
inline constexpr unsigned int WM_NCDESTROY         = 0x0082;
inline constexpr unsigned int WM_NCHITTEST         = 0x0084;
inline constexpr unsigned int WM_NCACTIVATE        = 0x0086;
inline constexpr unsigned int WM_SYNCPAINT         = 0x0088;
inline constexpr unsigned int WM_NCLBUTTONDOWN     = 0x00A1;
inline constexpr unsigned int WM_NCRBUTTONDOWN     = 0x00A4;
inline constexpr unsigned int WM_NCRBUTTONUP       = 0x00A5;
inline constexpr unsigned int WM_COMMAND            = 0x0111;
inline constexpr unsigned int WM_SYSCOMMAND        = 0x0112;
inline constexpr unsigned int WM_INITMENUPOPUP     = 0x0117;
inline constexpr unsigned int WM_MENUSELECT        = 0x011F;
inline constexpr unsigned int WM_INITDIALOG        = 0x0110;
inline constexpr unsigned int WM_ENTERMENULOOP     = 0x0211;
inline constexpr unsigned int WM_EXITMENULOOP      = 0x0212;
inline constexpr unsigned int WM_SIZING            = 0x0214;
inline constexpr unsigned int WM_MOVING            = 0x0216;
inline constexpr unsigned int WM_ENTERSIZEMOVE     = 0x0231;
inline constexpr unsigned int WM_EXITSIZEMOVE       = 0x0232;
inline constexpr unsigned int WM_DROPFILES         = 0x0233;
inline constexpr unsigned int WM_CONTEXTMENU       = 0x007B;
inline constexpr unsigned int WM_COPY              = 0x0301;
inline constexpr unsigned int WM_NOTIFY            = 0x004E;
inline constexpr unsigned int WM_DPICHANGED         = 0x02E0;
inline constexpr unsigned int WM_LBUTTONUP         = 0x0202;
inline constexpr unsigned int WM_LBUTTONDBLCLK     = 0x0203;
inline constexpr unsigned int WM_RBUTTONUP         = 0x0205;
inline constexpr unsigned int WM_MBUTTONUP         = 0x0208;
inline constexpr unsigned int WM_MBUTTONDBLCLK    = 0x0209;
inline constexpr unsigned int WM_MOUSEWHEEL        = 0x020A;
inline constexpr unsigned int WM_XBUTTONUP         = 0x020C;
inline constexpr unsigned int WM_XBUTTONDBLCLK    = 0x020D;
inline constexpr unsigned int WM_APPCOMMAND        = 0x0319;
inline constexpr unsigned int WM_SETFONT           = 0x0030;
inline constexpr unsigned int WM_GETFONT           = 0x0031;
inline constexpr unsigned int WM_DRAWITEM          = 0x002B;
inline constexpr unsigned int WM_MEASUREITEM       = 0x002C;
inline constexpr unsigned int WM_DELETEITEM         = 0x002D;
inline constexpr unsigned int WM_VKEYTOITEM        = 0x002E;
inline constexpr unsigned int WM_CHARTOITEM         = 0x002F;
inline constexpr unsigned int WM_PARENTNOTIFY       = 0x0210;
inline constexpr unsigned int WM_CHAR              = 0x0102;

// NPP custom messages — ScintillaComponent.h uses SCINTILLA_USER (2000) base.
// WM_TASKBARCREATED is registered at runtime on Win32; placeholder here.
inline constexpr unsigned int WM_GETTASKLISTINFO   = 0x0403;
inline constexpr unsigned int WM_UPDATEMAINMENUBITMAPS = 0x0404;
inline constexpr unsigned int WM_UPDATESCINTILLAS  = 0x0405;
inline constexpr unsigned int WM_SYSTRAY_MENU     = 0x0406;
inline constexpr unsigned int WM_MACRODLGRUNMACRO = 0x0411;
inline constexpr unsigned int WM_TASKBARCREATED   = 0x8000;
inline constexpr unsigned int WM_CHAR_REPLACEMENT = 0x1220;  // type-ahead search in dialogs
// MessageBeep and MB_* constants for audio feedback
inline constexpr unsigned int MB_ICONEXCLAMATION = 0x00000030;
inline constexpr unsigned int MB_ICONASTERISK     = 0x00000040;
inline constexpr unsigned int MB_ICONQUESTION    = 0x00000020;
inline constexpr unsigned int MB_ICONSTOP        = 0x00000010;
inline void MessageBeep(unsigned int) { /* Stubbed on Linux — no system beep available */ }
// Windows icon/resource IDs — stubbed for Qt6/Linux (not loaded from .rc files)
inline constexpr int IDI_NEW_ICON           = 100;
inline constexpr int IDI_NEW_ICON2          = 101;
inline constexpr int IDI_NEW_ICON_DM        = 102;
inline constexpr int IDI_NEW_ICON_DM2       = 103;
inline constexpr int IDI_OPEN_ICON          = 110;
inline constexpr int IDI_OPEN_ICON2         = 111;
inline constexpr int IDI_OPEN_ICON_DM       = 112;
inline constexpr int IDI_OPEN_ICON_DM2      = 113;
inline constexpr int IDI_SAVE_ICON          = 120;
inline constexpr int IDI_SAVE_DISABLE_ICON  = 121;
inline constexpr int IDI_SAVE_ICON2         = 122;
inline constexpr int IDI_SAVE_DISABLE_ICON2 = 123;
inline constexpr int IDI_SAVE_ICON_DM       = 124;
inline constexpr int IDI_SAVE_DISABLE_ICON_DM = 125;
inline constexpr int IDI_SAVE_ICON_DM2      = 126;
inline constexpr int IDI_SAVE_DISABLE_ICON_DM2 = 127;
inline constexpr int IDI_SAVEALL_ICON       = 130;
inline constexpr int IDI_SAVEALL_DISABLE_ICON = 131;
inline constexpr int IDI_SAVEALL_ICON2      = 132;
inline constexpr int IDI_SAVEALL_DISABLE_ICON2 = 133;
inline constexpr int IDI_SAVEALL_ICON_DM    = 134;
inline constexpr int IDI_SAVEALL_DISABLE_ICON_DM = 135;
inline constexpr int IDI_SAVEALL_ICON_DM2   = 136;
inline constexpr int IDI_SAVEALL_DISABLE_ICON_DM2 = 137;
inline constexpr int IDI_CLOSE_ICON         = 140;
inline constexpr int IDI_CLOSE_ICON2         = 141;
inline constexpr int IDI_CLOSE_ICON_DM      = 142;
inline constexpr int IDI_CLOSE_ICON_DM2     = 143;
inline constexpr int IDI_CLOSEALL_ICON      = 150;
inline constexpr int IDI_CLOSEALL_ICON2     = 151;
inline constexpr int IDI_CLOSEALL_ICON_DM   = 152;
inline constexpr int IDI_CLOSEALL_ICON_DM2  = 153;
inline constexpr int IDI_REDO_ICON         = 160;
inline constexpr int IDI_REDO_ICON2         = 161;
inline constexpr int IDI_UNDO_ICON          = 170;
inline constexpr int IDI_UNDO_ICON2         = 171;
inline constexpr int IDI_STOP_ICON          = 180;
inline constexpr int IDI_PLAY_ICON          = 181;


// Progress dialog stub — no-op for Qt port (QProgressDialog managed by caller)
class Progress {
public:
    Progress(void* /* HINSTANCE */) {}
    void open(void* /* HWND */, const char* /* msg */) {}
    void setPercent(int /* percent */, const char* /* file */, int /* total */) {}
    void setInfo(const char* /* file */, int /* total */) {}
    bool isCancelled() const { return false; }
    void close() {}
};
#endif  // WM_CREATE
// Windows resource IDs (IDR_*) — stubbed for Qt6/Linux (not loaded from .rc files)
inline constexpr int IDR_FILENEW          = 200;
inline constexpr int IDR_FILEOPEN          = 201;
inline constexpr int IDR_FILESAVE          = 202;
inline constexpr int IDR_CLOSEFILE        = 203;
inline constexpr int IDR_CLOSEALL         = 204;
inline constexpr int IDR_SAVEALL          = 205;
inline constexpr int IDR_CUT              = 210;
inline constexpr int IDR_COPY             = 211;
inline constexpr int IDR_PASTE            = 212;
inline constexpr int IDR_UNDO             = 213;
inline constexpr int IDR_REDO             = 214;
inline constexpr int IDR_FIND             = 220;
inline constexpr int IDR_REPLACE          = 221;
inline constexpr int IDR_PRINT            = 222;
inline constexpr int IDR_SYNCH            = 223;
inline constexpr int IDR_SYNCV            = 224;
inline constexpr int IDR_WRAP             = 225;
inline constexpr int IDR_INDENTGUIDE     = 226;
inline constexpr int IDR_INVISIBLECHAR    = 227;
inline constexpr int IDR_ZOOMIN           = 228;
inline constexpr int IDR_ZOOMOUT          = 229;
inline constexpr int IDR_PLAYRECORD        = 230;
inline constexpr int IDR_STOPRECORD       = 231;
inline constexpr int IDR_STARTRECORD      = 232;
inline constexpr int IDR_M_PLAYRECORD     = 233;
inline constexpr int IDR_SAVERECORD       = 234;
inline constexpr int IDR_DOCMAP           = 240;
inline constexpr int IDR_DOCMAP_ICO       = 241;
inline constexpr int IDR_DOCMAP_ICO2      = 242;
inline constexpr int IDR_DOCMAP_ICO_DM    = 243;
inline constexpr int IDR_FUNC_LIST         = 250;
inline constexpr int IDR_FUNC_LIST_ICO    = 251;
inline constexpr int IDR_FUNC_LIST_ICO2   = 252;
inline constexpr int IDR_FUNC_LIST_ICO_DM = 253;
inline constexpr int IDR_FILEBROWSER      = 260;
inline constexpr int IDR_FILEBROWSER_ICO  = 261;
inline constexpr int IDR_FILEBROWSER_ICO2 = 262;
inline constexpr int IDR_FILEBROWSER_ICO_DM= 263;
inline constexpr int IDR_DOCLIST          = 270;
inline constexpr int IDR_DOCLIST_ICO      = 271;
inline constexpr int IDR_DOCLIST_ICO2     = 272;
inline constexpr int IDR_DOCLIST_ICO_DM   = 273;
inline constexpr int IDR_PROJECTPANEL_ICO  = 280;
inline constexpr int IDR_PROJECTPANEL_ICO2 = 281;
inline constexpr int IDR_PROJECTPANEL_ICO_DM= 282;
inline constexpr int IDR_ASCIIPANEL_ICO    = 290;
inline constexpr int IDR_ASCIIPANEL_ICO2   = 291;
inline constexpr int IDR_ASCIIPANEL_ICO_DM = 292;
inline constexpr int IDR_CLIPBOARDPANEL_ICO = 300;
inline constexpr int IDR_CLIPBOARDPANEL_ICO2= 301;
inline constexpr int IDR_CLIPBOARDPANEL_ICO_DM= 302;
inline constexpr int IDR_FILEMONITORING   = 310;
inline constexpr int IDR_SHOWPANNEL       = 320;
// Additional icon/resource IDs
inline constexpr int IDI_PRINT_ICON         = 182;
inline constexpr int IDI_PRINT_ICON2         = 183;
inline constexpr int IDI_PRINT_ICON_DM      = 184;
inline constexpr int IDI_PRINT_ICON_DM2     = 185;
inline constexpr int IDI_SYNCH_ICON         = 190;
inline constexpr int IDI_SYNCH_ICON2         = 191;
inline constexpr int IDI_SYNCH_ICON_DM      = 192;
inline constexpr int IDI_SYNCH_ICON_DM2     = 193;
inline constexpr int IDI_SYNCH_DISABLE_ICON  = 194;
inline constexpr int IDI_SYNCH_DISABLE_ICON2 = 195;
inline constexpr int IDI_SYNCH_DISABLE_ICON_DM = 196;
inline constexpr int IDI_SYNCH_DISABLE_ICON_DM2 = 197;
inline constexpr int IDI_SYNCV_ICON         = 200;
inline constexpr int IDI_SYNCV_ICON2         = 201;
inline constexpr int IDI_SYNCV_ICON_DM      = 202;
inline constexpr int IDI_SYNCV_ICON_DM2     = 203;
inline constexpr int IDI_SYNCV_DISABLE_ICON  = 204;
inline constexpr int IDI_SYNCV_DISABLE_ICON2 = 205;
inline constexpr int IDI_SYNCV_DISABLE_ICON_DM = 206;
inline constexpr int IDI_SYNCV_DISABLE_ICON_DM2 = 207;
inline constexpr int IDI_WRAP_ICON          = 210;
inline constexpr int IDI_WRAP_ICON2          = 211;
inline constexpr int IDI_WRAP_ICON_DM       = 212;
inline constexpr int IDI_WRAP_ICON_DM2      = 213;
// Missing VIEW_ICON constants (stub IDs for toolbar buttons)
inline constexpr int IDI_VIEW_WRAP_ICON          = 310;
inline constexpr int IDI_VIEW_WRAP_ICON2         = 311;
inline constexpr int IDI_VIEW_WRAP_ICON_DM        = 312;
inline constexpr int IDI_VIEW_WRAP_ICON_DM2      = 313;
inline constexpr int IDI_VIEW_ALL_CHAR_ICON      = 320;
inline constexpr int IDI_VIEW_ALL_CHAR_ICON2     = 321;
inline constexpr int IDI_VIEW_ALL_CHAR_DM        = 322;
inline constexpr int IDI_VIEW_ALL_CHAR_DM2        = 323;
inline constexpr int IDI_UNDO_ICON_DM            = 330;
inline constexpr int IDI_UNDO_ICON_DM2           = 331;
inline constexpr int IDI_REDO_ICON_DM            = 332;
inline constexpr int IDI_REDO_ICON_DM2           = 333;

inline constexpr int IDI_INDENT_ICON         = 214;
inline constexpr int IDI_INDENT_ICON2        = 215;
inline constexpr int IDI_INDENT_ICON_DM      = 216;
inline constexpr int IDI_INDENT_ICON_DM2     = 217;
inline constexpr int IDI_ZOOMIN_ICON        = 222;
inline constexpr int IDI_ZOOMIN_ICON2       = 223;
inline constexpr int IDI_ZOOMIN_ICON_DM     = 224;
inline constexpr int IDI_ZOOMIN_ICON_DM2    = 225;
inline constexpr int IDI_ZOOMOUT_ICON        = 226;
inline constexpr int IDI_ZOOMOUT_ICON2       = 227;
inline constexpr int IDI_ZOOMOUT_ICON_DM    = 228;
inline constexpr int IDI_ZOOMOUT_ICON_DM2   = 229;
inline constexpr int IDI_PLAYRECORD_ICON     = 230;
inline constexpr int IDI_PLAYRECORD_ICON2    = 231;
inline constexpr int IDI_PLAYRECORD_ICON_DM  = 232;
inline constexpr int IDI_PLAYRECORD_ICON_DM2 = 233;
inline constexpr int IDI_PLAYRECORD_DISABLE_ICON = 234;
inline constexpr int IDI_PLAYRECORD_DISABLE_ICON2 = 235;
inline constexpr int IDI_PLAYRECORD_DISABLE_ICON_DM = 236;
inline constexpr int IDI_PLAYRECORD_DISABLE_ICON_DM2 = 237;
inline constexpr int IDI_STOPRECORD_ICON     = 238;
inline constexpr int IDI_STOPRECORD_ICON2    = 239;
inline constexpr int IDI_STOPRECORD_ICON_DM  = 240;
inline constexpr int IDI_STOPRECORD_ICON_DM2 = 241;
inline constexpr int IDI_STOPRECORD_DISABLE_ICON = 242;
inline constexpr int IDI_STOPRECORD_DISABLE_ICON2 = 243;
inline constexpr int IDI_STOPRECORD_DISABLE_ICON_DM = 244;
inline constexpr int IDI_STOPRECORD_DISABLE_ICON_DM2 = 245;
inline constexpr int IDI_STARTRECORD_ICON    = 246;
inline constexpr int IDI_STARTRECORD_ICON2   = 247;
inline constexpr int IDI_STARTRECORD_ICON_DM = 248;
inline constexpr int IDI_STARTRECORD_ICON_DM2 = 249;
inline constexpr int IDI_STARTRECORD_DISABLE_ICON = 250;
inline constexpr int IDI_STARTRECORD_DISABLE_ICON2 = 251;
inline constexpr int IDI_STARTRECORD_DISABLE_ICON_DM = 252;
inline constexpr int IDI_STARTRECORD_DISABLE_ICON_DM2 = 253;
inline constexpr int IDI_SAVERECORD_ICON     = 254;
inline constexpr int IDI_SAVERECORD_ICON2    = 255;
inline constexpr int IDI_SAVERECORD_ICON_DM  = 256;
inline constexpr int IDI_SAVERECORD_ICON_DM2 = 257;
inline constexpr int IDI_SAVERECORD_DISABLE_ICON = 258;
inline constexpr int IDI_SAVERECORD_DISABLE_ICON2 = 259;
inline constexpr int IDI_SAVERECORD_DISABLE_ICON_DM = 260;
inline constexpr int IDI_SAVERECORD_DISABLE_ICON_DM2 = 261;
inline constexpr int IDI_MMPLAY_ICON         = 262;
inline constexpr int IDI_MMPLAY_ICON2        = 263;
inline constexpr int IDI_MMPLAY_ICON_DM      = 264;
inline constexpr int IDI_MMPLAY_ICON_DM2     = 265;
inline constexpr int IDI_MMPLAY_DIS_ICON     = 266;
inline constexpr int IDI_MMPLAY_DIS_ICON2    = 267;
inline constexpr int IDI_MMPLAY_DIS_ICON_DM  = 268;
inline constexpr int IDI_MMPLAY_DIS_ICON_DM2 = 269;
inline constexpr int IDI_FIND_ICON           = 270;
inline constexpr int IDI_FIND_ICON2          = 271;
inline constexpr int IDI_FIND_ICON_DM       = 272;
inline constexpr int IDI_FIND_ICON_DM2       = 273;
inline constexpr int IDI_REPLACE_ICON         = 274;
inline constexpr int IDI_REPLACE_ICON2       = 275;
inline constexpr int IDI_REPLACE_ICON_DM     = 276;
inline constexpr int IDI_REPLACE_ICON_DM2    = 277;
inline constexpr int IDI_CUT_ICON           = 280;
inline constexpr int IDI_CUT_ICON2           = 281;
inline constexpr int IDI_CUT_ICON_DM          = 257;  // dark mode regular
inline constexpr int IDI_CUT_ICON_DM2         = 357;  // dark mode filled
inline constexpr int IDI_CUT_DISABLE_ICON     = 284;
inline constexpr int IDI_CUT_DISABLE_ICON2    = 285;
inline constexpr int IDI_CUT_DISABLE_ICON_DM  = 286;
inline constexpr int IDI_CUT_DISABLE_ICON_DM2 = 287;
inline constexpr int IDI_COPY_ICON           = 290;
inline constexpr int IDI_COPY_ICON2           = 291;
inline constexpr int IDI_COPY_ICON_DM          = 258;
inline constexpr int IDI_COPY_ICON_DM2         = 358;
inline constexpr int IDI_COPY_DISABLE_ICON     = 294;
inline constexpr int IDI_COPY_DISABLE_ICON2    = 295;
inline constexpr int IDI_COPY_DISABLE_ICON_DM  = 296;
inline constexpr int IDI_COPY_DISABLE_ICON_DM2 = 297;
inline constexpr int IDI_PASTE_ICON          = 300;
inline constexpr int IDI_PASTE_ICON2         = 301;
inline constexpr int IDI_PASTE_ICON_DM         = 259;
inline constexpr int IDI_PASTE_ICON_DM2        = 359;
inline constexpr int IDI_PASTE_DISABLE_ICON   = 304;
inline constexpr int IDI_PASTE_DISABLE_ICON2  = 305;
inline constexpr int IDI_PASTE_DISABLE_ICON_DM = 306;
inline constexpr int IDI_PASTE_DISABLE_ICON_DM2 = 307;
inline constexpr int IDI_REDO_DISABLE_ICON    = 310;
inline constexpr int IDI_REDO_DISABLE_ICON2   = 311;
inline constexpr int IDI_REDO_DISABLE_ICON_DM = 312;
inline constexpr int IDI_REDO_DISABLE_ICON_DM2 = 313;
inline constexpr int IDI_UNDO_DISABLE_ICON   = 314;
inline constexpr int IDI_UNDO_DISABLE_ICON2   = 315;
inline constexpr int IDI_UNDO_DISABLE_ICON_DM = 316;
inline constexpr int IDI_UNDO_DISABLE_ICON_DM2 = 317;
inline constexpr int IDI_VIEW_DOC_MAP_ICON    = 320;
inline constexpr int IDI_VIEW_DOC_MAP_ICON2   = 321;
inline constexpr int IDI_VIEW_DOC_MAP_ICON_DM = 322;
inline constexpr int IDI_VIEW_DOC_MAP_ICON_DM2 = 323;
inline constexpr int IDI_VIEW_FUNCLIST_ICON   = 324;
inline constexpr int IDI_VIEW_FUNCLIST_ICON2  = 325;
inline constexpr int IDI_VIEW_FUNCLIST_ICON_DM = 326;
inline constexpr int IDI_VIEW_FUNCLIST_ICON_DM2 = 327;
inline constexpr int IDI_VIEW_FILEBROWSER_ICON = 328;
inline constexpr int IDI_VIEW_FILEBROWSER_ICON2 = 329;
inline constexpr int IDI_VIEW_FILEBROWSER_ICON_DM = 330;
inline constexpr int IDI_VIEW_FILEBROWSER_ICON_DM2 = 331;
inline constexpr int IDI_VIEW_DOCLIST_ICON    = 332;
inline constexpr int IDI_VIEW_DOCLIST_ICON2   = 333;
inline constexpr int IDI_VIEW_DOCLIST_ICON_DM  = 334;
inline constexpr int IDI_VIEW_DOCLIST_ICON_DM2 = 335;
inline constexpr int IDI_VIEW_MONITORING_ICON  = 336;
inline constexpr int IDI_VIEW_MONITORING_ICON2 = 337;
inline constexpr int IDI_VIEW_MONITORING_ICON_DM = 338;
inline constexpr int IDI_VIEW_MONITORING_ICON_DM2 = 339;
inline constexpr int IDI_VIEW_MONITORING_DIS_ICON = 340;
inline constexpr int IDI_VIEW_MONITORING_DIS_ICON2 = 341;
inline constexpr int IDI_VIEW_MONITORING_DIS_ICON_DM = 342;
inline constexpr int IDI_VIEW_MONITORING_DIS_ICON_DM2 = 343;
inline constexpr int IDI_VIEW_UD_DLG_ICON     = 344;
inline constexpr int IDI_VIEW_UD_DLG_ICON2     = 345;
inline constexpr int IDI_VIEW_UD_DLG_ICON_DM   = 346;
inline constexpr int IDI_VIEW_UD_DLG_ICON_DM2  = 347;
inline constexpr int IDI_VIEW_INDENT_ICON     = 348;
inline constexpr int IDI_VIEW_INDENT_ICON2     = 349;
inline constexpr int IDI_VIEW_INDENT_ICON_DM  = 350;
inline constexpr int IDI_VIEW_INDENT_ICON_DM2 = 351;
inline constexpr int IDI_SEPARATOR_ICON       = 360;
inline constexpr int IDI_M30ICON              = 361;
// Additional icons not in the first batch
inline constexpr int IDI_VIEW_INDENTGUIDE_ICON = 362;
inline constexpr int IDI_VIEW_INDENTGUIDE_ICON2 = 363;
inline constexpr int IDI_VIEW_INDENTGUIDE_ICON_DM = 364;
inline constexpr int IDI_VIEW_INDENTGUIDE_ICON_DM2 = 365;
inline constexpr int IDI_VIEW_ZOOMIN_ICON = 370;
inline constexpr int IDI_VIEW_ZOOMIN_ICON2 = 371;
inline constexpr int IDI_VIEW_ZOOMIN_ICON_DM = 372;
inline constexpr int IDI_VIEW_ZOOMIN_ICON_DM2 = 373;
inline constexpr int IDI_VIEW_ZOOMOUT_ICON = 374;
inline constexpr int IDI_VIEW_ZOOMOUT_ICON2 = 375;
inline constexpr int IDI_VIEW_ZOOMOUT_ICON_DM = 376;
inline constexpr int IDI_VIEW_ZOOMOUT_ICON_DM2 = 377;
// Menu index constants for menu item access
inline constexpr int MENUINDEX_FILE         = 0;
inline constexpr int MENUINDEX_EDIT         = 1;
inline constexpr int MENUINDEX_SEARCH       = 2;
inline constexpr int MENUINDEX_VIEW         = 3;
inline constexpr int MENUINDEX_ENCODING     = 4;
inline constexpr int MENUINDEX_LANGUAGE     = 5;
inline constexpr int MENUINDEX_SETTINGS     = 6;
inline constexpr int MENUINDEX_TOOLS        = 7;
inline constexpr int MENUINDEX_MACRO        = 8;
inline constexpr int MENUINDEX_RUN          = 9;

// Missing EolType enum values
inline constexpr int EolType_unix = 0;      // LF
inline constexpr int EolType_windows = 1;   // CRLF
inline constexpr int EolType_mac = 2;       // CR
inline constexpr int EolType_macLegacy = 3; // CR (classic)

// Missing DocFileStatus values
inline constexpr int DocFileStatus_clean = 0;
inline constexpr int DocFileStatus_observed = 1;
inline constexpr int DocFileStatus_modified = 2;

// cdModes flags (from original NPP's cdModes enum)
inline constexpr int cdAutoUpdate = 0x04;
inline constexpr int cdGo2end = 0x08;

// DOCSTATUS flags for NPPN_DOCMODIFIED notification idFrom field
inline constexpr int DOCSTATUS_READONLY = 0x01;
inline constexpr int DOCSTATUS_BUFFERDIRTY = 0x02;

// RGB macro (Win32 color)
inline constexpr int RGB(int r, int g, int b) { return ((((unsigned int)(r)) & 0xff) | ((((unsigned int)(g)) & 0xff) << 8) | ((((unsigned int)(b)) & 0xff) << 16)); }
inline constexpr int GetRValue(int rgb) { return ((int)(((unsigned int)(rgb)) & 0xff)); }
inline constexpr int GetGValue(int rgb) { return ((int)(((unsigned int)(rgb) >> 8) & 0xff)); }
inline constexpr int GetBValue(int rgb) { return ((int)(((unsigned int)(rgb) >> 16) & 0xff)); }

// ID_MACRO base (missing)
inline constexpr int ID_MACRO = 41000;

// wsprintf replacement (safe wstring formatting)
inline int npp_wsprintf(wchar_t* buf, size_t bufsize, const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vswprintf(buf, bufsize, format, args);
    va_end(args);
    return result;
}

// Additional missing IDI constants
inline constexpr int IDI_VIEW_ALL_CHAR_ICON_DM   = 324;
inline constexpr int IDI_VIEW_ALL_CHAR_ICON_DM2 = 325;
// MODEVENTMASK constant
inline constexpr int MODEVENTMASK_OFF = 0;
// NOTE: IDOK, IDCANCEL, WM_INITDIALOG, WM_SIZE, WM_DESTROY are already defined
// in NppConstants.h above (lines 189-190 for IDOK/IDCANCEL, lines ~504-539 for WM_*)
// with proper #ifndef guards — do NOT add duplicates here.

// Win32 SM_* system metrics (for DPIManagerV2::getSystemMetricsForDpi)
#ifndef SM_CXMINTRACK
inline constexpr int SM_CXMINTRACK = 112;  // minimum window drag width (Windows default)
#endif

// Win32 compatibility stubs for Qt6 port (Linux/Non-Windows builds)
// These map Win32 rect/notify structs to Qt equivalents

// RECT — Win32 rectangle, mapped to QRect
#ifndef RECT
struct RECT {
    long left;
    long top;
    long right;
    long bottom;
    long width()  const { return right - left; }
    long height() const { return bottom - top; }
    Q_DECL_CONSTEXPR operator QRect() const { return QRect(left, top, width(), height()); }
    Q_DECL_CONSTEXPR QRect toQRect() const { return QRect(left, top, width(), height()); }
};
#endif
#ifndef LPRECT
using LPRECT = RECT*;
#endif
#ifndef LPCRECT
using LPCRECT = const RECT*;
#endif

// NMHDR — Win32 WM_NOTIFY header structure
#ifndef NMHDR
struct NMHDR {
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;
};
#endif
#ifndef LPNMHDR
using LPNMHDR = NMHDR*;
#endif

// NMTBCUSTOMDRAW — Toolbar custom draw notify structure
// Maps: Win32 HDC/HBRUSH/HPEN/COLORREF fields → Qt shims
#ifndef NMCUSTOMDRAW
struct NMCUSTOMDRAW {
    NMHDR  nmcd;
    DWORD  dwDrawStage;
    HDC    hdc;
    RECT   rc;
    UINT   uItemState;
    LPARAM lItemlParam;
};
#endif
#ifndef LPNMCUSTOMDRAW
using LPNMCUSTOMDRAW = NMCUSTOMDRAW*;
#endif

#ifndef NMTBCUSTOMDRAW
struct NMTBCUSTOMDRAW {
    NMCUSTOMDRAW nmcd;  // base: NMHDR + dwDrawStage + hdc + rc / uItemState / clrFace etc.
    // Fields used by npp-qt:
    HDC         hdc;              // device context (Qt shim: void* = unused on Qt painter path)
    RECT        rc;              // bounding rectangle
    UINT        uItemState;      // CDIS_* flags
    quintptr    dwItemSpec;      // item id
    HBRUSH      hbrMonoDither;   // mono dither brush (Qt shim)
    HBRUSH      hbrLines;        // line brush (Qt shim)
    HPEN        hpenLines;       // line pen (Qt shim)
    COLORREF    clrText;         // text color (QRgb)
    COLORREF    clrTextHighlight;// highlighted text color
    COLORREF    clrBtnFace;      // button face color
    COLORREF    clrBtnHighlight; // button highlight color
    COLORREF    clrHighlightHotTrack; // hot-track highlight color
    int         nStringBkMode;    // text background mode (TRANSPARENT/OPAQUE)
    int         nHLStringBkMode; // hot-light text bg mode
};
#endif
#ifndef LPNMTBCUSTOMDRAW
using LPNMTBCUSTOMDRAW = NMTBCUSTOMDRAW*;
#endif

// NMTBCUSTOMDRAW — Toolbar custom draw notify structure
// WM_NOTIFY codes used in toolbar custom draw
#ifndef CDDS_PREPAINT
inline constexpr int CDDS_PREPAINT       = 0x00000001;
inline constexpr int CDDS_POSTPAINT      = 0x00000002;
inline constexpr int CDDS_PREERASE        = 0x00000003;
inline constexpr int CDDS_POSTERASE       = 0x00000004;
inline constexpr int CDDS_ITEM           = 0x00010000;
inline constexpr int CDDS_ITEMPREPAINT   = (CDDS_ITEM | CDDS_PREPAINT);
inline constexpr int CDDS_ITEMPOSTPAINT  = (CDDS_ITEM | CDDS_POSTPAINT);
inline constexpr int CDDS_ITEMPREERASE   = (CDDS_ITEM | CDDS_PREERASE);
inline constexpr int CDDS_ITEMPOSTERASE  = (CDDS_ITEM | CDDS_POSTERASE);
inline constexpr int CDDS_SUBITEM        = 0x00020000;
#endif
#ifndef CDIS_HOT
inline constexpr int CDIS_HOT      = 0x00000001;
inline constexpr int CDIS_SELECTED = 0x00000002;
inline constexpr int CDIS_DISABLED  = 0x00000004;
inline constexpr int CDIS_CHECKED   = 0x00000008;
#endif
#ifndef CDRF_DODEFAULT
inline constexpr int CDRF_DODEFAULT       = 0x00000000;
inline constexpr int CDRF_NEWFONT         = 0x00000002;
inline constexpr int CDRF_NOTIFYITEMDRAW  = 0x00000020;
inline constexpr int CDRF_NOTIFYPOSTPAINT = 0x00000010;
#endif

// NM_CUSTOMDRAW notification code
#ifndef NM_CUSTOMDRAW
inline constexpr int NM_CUSTOMDRAW = -12;
#endif

// TBBUTTONINFO for toolbar dropdown detection
#ifndef TBBUTTONINFO
struct TBBUTTONINFO {
    UINT   cbSize;
    DWORD  dwMask;
    int    id;
    int    iImage;
    DWORD  fsStyle;
    COLORREF clrKey;
    COLORREF clrBack;
    int    iSession;
};
#endif
#ifndef TBIF_STYLE
inline constexpr int TBIF_STYLE = 0x0008;
#endif
#ifndef BTNS_DROPDOWN
inline constexpr int BTNS_DROPDOWN = 0x0080;
#endif
#ifndef TRANSPARENT
inline constexpr int TRANSPARENT = 1;
#endif
#ifndef OPAQUE
inline constexpr int OPAQUE = 2;
#endif

#ifndef TBCDRF_USECDCOLORS
inline constexpr int TBCDRF_USECDCOLORS = 0x00000008; // Use custom colors (clrText, clrBtnFace etc.)
#endif
#ifndef TBCDRF_NOETC
inline constexpr int TBCDRF_NOETC = 0x00000020;
#endif
