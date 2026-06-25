// NppConstants — shared constants for the NPP port
// Extracted from NppConstants.h in source
#pragma once

#include <QtCore/Qt>
#include <QtGui/qrgb.h>   // QRgb used by COLORREF typedef
#include <QtCore/QString>

// Forward declarations for types used across npp-qt
class ScintillaComponent;       // defined in ScintillaComponent.h
using ScintillaEditView = ScintillaComponent;  // canonical alias (also in ScintillaComponent.h)
class NppGUI;
class NppParameters;

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

// Find/Replace defaults
inline constexpr int FINDREPLACE_INSELECTION_THRESHOLD_DEFAULT = 0;
inline constexpr int FILL_FINDWHAT_THRESHOLD_DEFAULT = 0;

// Recent file list
inline constexpr int RECENTFILES_SHOWFULLPATH = 0;

// UDD docked
inline constexpr int UDD_DOCKED = 0;

// Position
inline constexpr int POS_VERTICAL = 0;

// Platform
struct Platform {};

// Undo
inline constexpr int UDD_DOCKED_UNDO = 0;

// Virtual key codes (Win32 VK_* → Qt::Key_* mappings)
inline constexpr int VK_SPACE = 0x20;
inline constexpr int VK_RETURN = 0x0D;
inline constexpr int VK_ESCAPE = 0x1B;
inline constexpr int VK_END = 0x23;
inline constexpr int VK_HOME = 0x24;
inline constexpr int VK_LEFT = 0x25;
inline constexpr int VK_UP = 0x26;
inline constexpr int VK_RIGHT = 0x27;
inline constexpr int VK_DOWN = 0x28;
inline constexpr int VK_DELETE = 0x2E;
inline constexpr int VK_TAB = 0x09;
inline constexpr int VK_BACK = 0x08;
inline constexpr int VK_F1 = 0x70;
inline constexpr int VK_F2 = 0x71;
inline constexpr int VK_F3 = 0x72;
inline constexpr int VK_F4 = 0x73;
inline constexpr int VK_F5 = 0x74;
inline constexpr int VK_F6 = 0x75;
inline constexpr int VK_F7 = 0x76;
inline constexpr int VK_F8 = 0x77;
inline constexpr int VK_F9 = 0x78;
inline constexpr int VK_F10 = 0x79;
inline constexpr int VK_F11 = 0x7A;
inline constexpr int VK_F12 = 0x7B;
inline constexpr int VK_CAPITAL = 0x14;  // Caps Lock
inline constexpr int VK_SHIFT = 0x10;
inline constexpr int VK_CONTROL = 0x11;
inline constexpr int VK_MENU = 0x12;    // ALT key

// Win32 type aliases for cross-platform compatibility
using DWORD = unsigned long;
using LONG = long;
using ULONG = unsigned long;
using INT = int;
using UINT = unsigned int;
using WPARAM = quint64;
using LPARAM = quint64;
using LRESULT = quint64;
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

// Platform function pointer type
using PFUNKNOWN = void*;
inline constexpr int PF_UNKNOWN = 0;

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

// EolType — line ending format
enum class EolType { EolWindows = 0, EolUnix = 1, EolMac = 2, unknown = -1, osdefault = 0 };

// ToolBarStatusType is defined in ToolBar.h (enum class ToolBarDisplayType + using alias)
enum toolBarStatusType {
    TBAR_NEW = 0, TBAR_OPEN = 1, TBAR_SAVE = 2, TBAR_CLOSE = 3,
    TB_STANDARD = 4, TB_SMALL = 0, TB_LARGE = 1, TB_SMALL2 = 2, TB_LARGE2 = 3,
};

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
enum urlMode { URL_NONE = 0, URL_HTML = 1, URL_XML = 2, URL_JS = 4, URL_CSS = 8 };

// Change history state
enum class ChangeHistoryState { CHG_HIST_DISABLED = 0, CHG_HIST_ENABLED = 1, CHG_HIST_PERSISTENT = 2 };
enum changeHistoryState { CHG_HIST_DISABLED = 0, CHG_HIST_ENABLED = 1, CHG_HIST_PERSISTENT = 2 };

// Open/Save dir setting
enum class OpenSaveDirSetting { USER_DEFAULT_DIR = 0, USER_DEFINE_DIR = 1, ORIGINAL_DIR = 2 };

// Multi instance — defined inside NppGUI struct in Parameters.h (NppGUI::MultiInstSetting)
inline constexpr int monoInst = 0;   // MultiInstSetting::MONO_INSTANCE
inline constexpr int multiInst = 1; // MultiInstSetting::MULTI_INSTANCE
inline constexpr int panamaInst = 2; // MultiInstSetting::PANAMA_INSTANCE (was PANAMA_INSTANCE)

// Backup feature
enum class BackupFeature { BackupNone = 0, BackupSimple = 1, BackupVerbose = 2 };

// Auto indent mode
enum class AutoIndentMode { AI_NONE = 0, AI_BASIC = 1, AI_SMART = 2 };

// External lexer auto indent
enum class ExternalLexerAutoIndentMode { EXTLexAi_None = 0, EXTLexAi_Param = 1, EXTLexAi_aggressive = 2, Standard = 0 };

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

// Sci_CharacterRangeFull struct — defined here ONLY if Scintilla.h hasn't defined it yet.
// (NppSciCompat.h includes Scintilla.h which defines it, so this is usually a no-op.)
#ifndef SCI_CHARACTERRANGEFULL_DEFINED
#define SCI_CHARACTERRANGEFULL_DEFINED
// NppQt extended version — use Scintilla.h's version when available.
struct Sci_CharacterRangeFull;
#endif

// Stub forward declarations for missing dialog classes
class FindInFinderDlg;
class FindIncrementDlg;
class HashFromFilesDlg;
class HashFromTextDlg;
class DocumentPeeker;
struct FindersInfo { void* dummy = nullptr; };

// TrayIconController (corrected spelling)
class TrayIconController;
