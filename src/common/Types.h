// Types.h - Common type definitions for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <cstdint>
#include <string>
#include <QString>
#include <QStringList>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <filesystem>
#include <chrono>

// Forward declarations
class Buffer;
class ScintillaEditor;

// ============================================================================
// ID Types
// ============================================================================
using BufferID = Buffer*;
#define BUFFER_INVALID nullptr

using EditorID = ScintillaEditor*;
#define EDITOR_INVALID nullptr

using ViewID = int;
#define VIEW_INVALID (-1)

// ============================================================================
// Buffer / Document Status
// ============================================================================
enum class DocFileStatus : uint8_t {
    DOC_REGULAR       = 0x01,
    DOC_UNNAMED       = 0x02,
    DOC_DELETED       = 0x04,
    DOC_MODIFIED      = 0x08,
    DOC_NEEDRELOAD    = 0x10,
    DOC_INACCESSIBLE  = 0x20
};

enum class BufferStatusInfo : uint32_t {
    BufferChangeNone       = 0x000,
    BufferChangeLanguage   = 0x001,
    BufferChangeDirty      = 0x002,
    BufferChangeFormat     = 0x004,
    BufferChangeUnicode    = 0x008,
    BufferChangeReadonly   = 0x010,
    BufferChangeStatus     = 0x020,
    BufferChangeTimestamp  = 0x040,
    BufferChangeFilename   = 0x080,
    BufferChangeRecentTag  = 0x100,
    BufferChangeLexing     = 0x200
};

enum class SavingStatus : int {
    SaveOK                       = 0,
    SaveOpenFailed               = 1,
    SaveWritingFailed            = 2,
    NotEnoughRoom                = 3,
    FullReadOnlySavingForbidden   = 4
};

// ============================================================================
// Encoding Types
// ============================================================================
enum class EncodingType : int {
    NONE       = -1,  // No encoding detected / unknown
    ANSI       = 0,
    UTF_8      = 1,
    UTF_8_BOM  = 2,
    UTF_16_LE  = 3,
    UTF_16_BE  = 4,
    UTF_32_LE  = 5,
    UTF_32_BE  = 6,
    EBCDIC     = 7,
    OEM        = 8,  // DOS code page
    ASCII_7    = 9,  // ASCII (7-bit)
    UTF_16_LE_BOM  = 10,
    UTF_16_BE_BOM  = 11,
    ISO88591       = 12,
    Windows1252   = 13,
    Other          = 14,
};

enum class NewDocDefaultSettings : int {
    defaultNewFile = 0,
    defaultLF     = 1,
    defaultCRLF   = 2,
    defaultTabSettings = 3
};

// ============================================================================
// EOL Types
// ============================================================================
enum class EolType : uint8_t {
    EOL_NONE  = 0,
    EOL_LF    = 1,   // Unix
    EOL_CRLF  = 2,   // Windows
    EOL_CR    = 3    // Old Mac
};

// ============================================================================
// Language / Lexer Types
// ============================================================================
enum class LangType : int {
    L_NOT_SET    = -1,
    L_TEXT       = 0,
    L_C          = 1,
    L_CPP        = 2,
    L_JAVA       = 3,
    L_CS         = 4,
    L_OBJC       = 5,
    L_HTML       = 6,
    L_XML        = 7,
    L_XAML       = 8,
    L_LUA        = 9,
    L_PERL       = 10,
    L_PHP        = 11,
    L_RUBY       = 12,
    L_PYTHON     = 13,
    L_JS         = 14,
    L_JSON       = 15,
    L_CSS        = 16,
    L_YAML       = 17,
    L_MAKEFILE   = 18,
    L_CSHARP     = 19,
    L_MARKDOWN   = 20,
    L_BATCH      = 21,
    L_INI        = 22,
    L_NFO        = 23,
    L_GO         = 24,
    L_RUST       = 25,
    L_SWIFT      = 26,
    L_KOTLIN     = 27,
    L_SCALA      = 28,
    L_TS         = 29,
    L_RUBY2      = 30,
    L_LISP       = 31,
    L_CLOJURE    = 32,
    L_HASKELL    = 33,
    L_ERLANG     = 34,
    L_ELIXIR     = 35,
    L_FORTRAN    = 36,
    L_PASCAL     = 37,
    L_ADA        = 38,
    L_VERILOG    = 39,
    L_VHDL       = 40,
    L_ASM        = 41,
    L_TCL        = 42,
    L_BASH       = 43,
    L_POWERSHELL = 44,
    L_NSIS       = 45,
    L_NSIS2      = 46,
    L_DOCKERFILE = 47,
    L_CMAKE      = 48,
    L_R          = 49,
    L_JULIA      = 50,
    L_MATLAB     = 51,
    L_DIFF       = 52,
    L_TEX        = 53,
    L_REGEX      = 55,
    L_GRAPHVIZ   = 56,
    L_SQL        = 57,
    L_FSHARP     = 58,
    L_GROOVY     = 59,
    L_WEBASSEMBLY = 60,
    L_PROPERTIES  = 61,
    L_USER       = 1000,  // User-defined languages start here
    L_EXTERNAL   = 2000,
    L_REGISTRY   = 3000
};

// ============================================================================
// Window State
// ============================================================================
enum class WindowStatus : uint8_t {
    WindowMainActive = 0x01,
    WindowSubActive  = 0x02,
    WindowBothActive = 0x03,
    WindowUserActive = 0x04
};

enum class WindowMode : uint8_t {
    Normal      = 0,
    FullScreen  = 1,
    PostIt      = 2,
    DistractionFree = 3
};

// ============================================================================
// Search / Find Types
// ============================================================================
enum class FindOption : uint32_t {
    None            = 0,
    WholeWord       = 1 << 0,
    MatchCase       = 1 << 1,
    WrapAround      = 1 << 2,
    InSelection     = 1 << 3,
    Incremental     = 1 << 4,
    Hidden          = 1 << 5,
    PreserveCase    = 1 << 6,
    Regex           = 1 << 7
};

inline FindOption operator|(FindOption a, FindOption b) {
    return static_cast<FindOption>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline FindOption operator&(FindOption a, FindOption b) {
    return static_cast<FindOption>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline FindOption& operator|=(FindOption& a, FindOption b) {
    a = a | b;
    return a;
}

enum class FindTarget : uint8_t {
    Normal       = 0,
    Selection    = 1,
    AllDocuments = 2
};

enum class SearchMode : uint8_t {
    Normal       = 0,
    Extended     = 1,
    Regex        = 2
};

// ============================================================================
// Clipboard Formats
// ============================================================================
enum class ClipboardFormat : uint32_t {
    CF_TEXT       = 1,
    CF_UNICODETEXT = 13,
    CF_HDROP      = 15,
    CF_LOCALE     = 16
};

// ============================================================================
// Colour / Style Types
// ============================================================================
struct ColourRGB {
    uint8_t r = 0, g = 0, b = 0;
    constexpr ColourRGB() = default;
    constexpr ColourRGB(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}
    uint32_t toInt() const { return (r << 16) | (g << 8) | b; }
};

struct Style {
    int             _styleID = 0;
    std::string      _fgColour;
    std::string      _bgColour;
    std::string      _fontName;
    int              _fontSize = 10;
    bool             _bold = false;
    bool             _italic = false;
    bool             _underline = false;
    bool             _strike = false;
    int              _foreground = -1;
    int              _background = -1;
    int              _fontStyle = 0;  // SC_WEIGHT_NORMAL, SC_WEIGHT_BOLD etc.
};

// ============================================================================
// File Types
// ============================================================================
enum class FileTransMode : uint8_t {
    TransferClone = 0x01,
    TransferMove  = 0x02
};

// ============================================================================
// Tab / View Types
// ============================================================================
struct BufferViewInfo {
    BufferID _bufID = nullptr;
    int       _iView = 0;
    BufferViewInfo() = default;
    BufferViewInfo(BufferID buf, int view) : _bufID(buf), _iView(view) {}
};

// ============================================================================
// Session Types
// ============================================================================
struct Session {
    std::vector<BufferViewInfo> _buffers;
    QStringList _recentFiles;
    QString _activeView;
    int     _activeTab = 0;
    QString _workingDir;
};

// ============================================================================
// Recent Files
// ============================================================================
struct RecentFileInfo {
    QString _fullPath;
    std::chrono::system_clock::time_point _lastAccess;
    bool    _inCurrentSession = false;
};

// ============================================================================
// Recycle Bin (trash)
// ============================================================================
struct RecycleBin {
    QString _filePath;
    QString _originalPath;
};

// ============================================================================
// Utility Macros
// ============================================================================
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// ============================================================================
// Settings structs (referenced by NppParameters)
// ============================================================================
struct NppGUI {
    bool toolbarShow = true;
    int  toolbarLines = 1;
    bool showStatusBar = true;
    bool showMenuBar = true;
    bool autoUpdate = true;
    bool tabBarHide = false;
    bool tabBarHideShow = false;
    bool multiInst = false;
    bool smartHighLight = true;
    bool highlightToken = true;
    bool matchCase = false;
    bool matchWholeWord = false;
    bool autoIndent = true;
    bool backspaceUnindents = false;
    int  tabSize = 4;
    bool useSpaces = false;
    bool showEol = false;
    bool showSpaceAndTab = false;
    bool showWrapSymbol = false;
    int  edgeMode = 0;
    int  edgeColumn = 80;
    bool wrapMode = false;
    int  folderStyle = 0;
    bool showFileTree = true;
    bool titleBarShort = false;
    QString titleBarAdd;

    // Backup / Auto-save settings
    int  backupMode = 0;          // 0=None, 1=Simple, 2=Numbered, 3=Verbose
    int  maxBackups = 3;          // 1-10
    bool useCustomBackupDir = false;
    QString customBackupDir;      // empty = same dir as file
    int  autoSaveInterval = 0;    // 0=disabled, 1-30 minutes
    bool autoSaveWithBackup = true;
    bool autoReloadOnChange = false;
};

struct ScintillaViewParams {
    bool displayeEol = false;
    bool showIndentGuide = true;
    bool wrap = false;

    // Nested view parameters
    struct SubParams {
        bool _lineNumberMarginShow = true;
        bool _bookmarkMarginShow = true;
        bool _folderMarginShow = true;
        bool _currentLineHilightingShow = true;
        bool _currentLineFrameShow = false;
        bool _marginFont = false;
        int  _vrSymbol = 0;
        int  _vpSymbol = 0;
        int  _whitespaceSymbol = 0;
        bool _smoothFont = true;
        int  _braceAnalyser = 1;
        int  _currentLineColor = 0xFF0000;
        bool _highlightCurrentBlock = true;
        bool _highlightOtherBlock = true;
        int  _eolMode = 0;
    } svp;
};

struct FindHistory {
    bool _isSearchDialogChecked = false;
    bool _isFilterChecked = false;
    bool _isInSubFolderChecked = false;
    bool _isInHiddenFolderChecked = false;
    bool _searchResultSelected = false;
    int  _transparencyMode = 0;
    int  _transparency = 100;
    bool _dotMatchesNewline = false;
    bool _isSearch2ButtonsMode = false;
    bool _regexBackward4PowerUser = false;
    bool _isBookmarkLine = false;
    bool _matchWord = false;
    bool _matchCase = false;
    int  _searchMode = 0;
    int  _counter1 = 0;
    int  _counter2 = 0;
    int  _counter3 = 0;
};

// ShortcutDef — plain data struct (for serialization). Named to avoid clash
// with MISC::Common::Shortcut (QObject subclass in shortcut.h).
struct ShortcutDef {
    std::string name;
    int id = 0;
    int ctrl = 0;
    int alt = 0;
    int shift = 0;
    int key = 0;
};


struct FindResult {
    QString filePath;
    int lineNumber = 0;
    int column = 0;
    QString lineContent;
    QString context;
};
// ============================================================================
// STL Hash specializations
// ============================================================================
namespace std {
    // QString is already hashable via qHash()
}
