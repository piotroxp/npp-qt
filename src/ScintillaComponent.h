#pragma once

// Forward declarations to break circular dependency.
// ScintillaEditView is defined as an alias for ScintillaComponent at the end of this file.
class ScintillaComponent;

// Semantic Lift: ScintillaComponent from Win32 → Qt6
// Original: PowerEditor/src/ScintillaComponent/ScintillaEditView.h
//           PowerEditor/src/ScintillaComponent/ScintillaCtrls.h
// Target:   npp-qt/src/ScintillaComponent.h
//
// Qt6 semantic equivalents:
//   HWND _hSelf                  → ScintillaEditBase* (inherits from QAbstractScrollArea)
//   CreateWindowEx() / DestroyWindow() → constructor / deleteLater()
//   SendMessage(hwnd, SCI_*, ...) → send(SCI_*, wparam, lparam)
//   SetWindowSubclass()          → installEventFilter()
//   ::GetModuleHandle / GetProcAddress → QLibrary or direct symbol
//   HWND hParent / HINSTANCE      → QWidget* parent / QApplication*
//
// This file contains:
//   - ScintillaComponent  (was ScintillaEditView — single editor widget)
//   - ScintillaCtrls      (was ScintillaCtrls  — factory/manager for multiple editors)

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QColor>
#include <QFont>
#include <QApplication>
#include <QScrollBar>
#include <QEvent>
#include "Buffer.h"
#include "NppConstants.h"
#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexeroctave.h>
#include <vector>
// Forward declarations for Win32 interface types not yet fully lifted
class ISorter;         // line-sorter interface (stubbed in npp-qt)
// column-mode selection data (stubbed from Win32 original)
struct ColumnModeInfo {
    intptr_t _selLpos = 0;
    intptr_t _selRpos = 0;
    intptr_t _order = 0;
    bool _isL2R = true;
    intptr_t _nbVirtualAnchorSpc = 0;
    intptr_t _nbVirtualCaretSpc = 0;
    ColumnModeInfo() = default;
    ColumnModeInfo(intptr_t lPos, intptr_t rPos, intptr_t order, bool isL2R = true,
                  intptr_t vAnchorNbSpc = 0, intptr_t vCaretNbSpc = 0)
        : _selLpos(lPos), _selRpos(rPos), _order(order), _isL2R(isL2R),
          _nbVirtualAnchorSpc(vAnchorNbSpc), _nbVirtualCaretSpc(vCaretNbSpc) {}
    bool isValid() const { return _selLpos < _selRpos; }
};
using ColumnModeInfos = std::vector<ColumnModeInfo>;

// ScintillaEditBase provides the Qt6 platform layer for Scintilla.
// Stub implementation lives in src/ScintillaEditBase.{h,cpp}; replace with
// the real Scintilla Qt port (scintilla/qt/) when available.
#include "ScintillaEditBase.h"

// Sci_CharacterRangeFull — needed by Notepad_plus.h (which includes this header).
// Scintilla.h (included via NppSciCompat.h → ScintillaEditBase.h) defines it inside
// an extern "C" block, so it's not accessible as a C++ type here without including
// Scintilla.h directly (which would poison the namespace with SCI_* macros).
// Provide a compatible C++ definition; guard against double-definition.
#ifndef SCI_CHARACTERRANGEFULL_DEFINED
#define SCI_CHARACTERRANGEFULL_DEFINED
struct Sci_CharacterRangeFull {
    intptr_t cpMin;
    intptr_t cpMax;
};
#endif

// =============================================================================
// Constants lifted from ScintillaEditView.h / Notepad_plus_msgs.h
// =============================================================================

// Custom WM_ range for NPP-internal messages routed through Scintilla
constexpr int SCINTILLA_USER = 2000;
constexpr int WM_DOCK_USERDEFINE_DLG     = SCINTILLA_USER + 1;
constexpr int WM_UNDOCK_USERDEFINE_DLG   = SCINTILLA_USER + 2;
constexpr int WM_CLOSE_USERDEFINE_DLG    = SCINTILLA_USER + 3;
constexpr int WM_REMOVE_USERLANG         = SCINTILLA_USER + 4;
constexpr int WM_RENAME_USERLANG         = SCINTILLA_USER + 5;
constexpr int WM_REPLACEALL_INOPENEDDOC  = SCINTILLA_USER + 6;
constexpr int WM_FINDALL_INOPENEDDOC     = SCINTILLA_USER + 7;
constexpr int WM_DOOPEN                   = SCINTILLA_USER + 8;
constexpr int WM_FINDINFILES              = SCINTILLA_USER + 9;
constexpr int WM_REPLACEINFILES          = SCINTILLA_USER + 10;
constexpr int WM_FINDALL_INCURRENTDOC    = SCINTILLA_USER + 11;
constexpr int WM_FRSAVE_INT              = SCINTILLA_USER + 12;
constexpr int WM_FRSAVE_STR              = SCINTILLA_USER + 13;
constexpr int WM_FINDALL_INCURRENTFINDER = SCINTILLA_USER + 14;
constexpr int WM_FINDINPROJECTS          = SCINTILLA_USER + 15;
constexpr int WM_REPLACEINPROJECTS       = SCINTILLA_USER + 16;

// Codepage constants (Win32 CP_* equivalents for Qt6)
// CP_ACP = 0 is the Windows ANSI code page; Qt6 has no direct equivalent so we use UTF-8
constexpr int CP_ACP                  = 0;
constexpr int CP_UTF8                = 65001;
constexpr int SC_CP_UTF8              = 65001; // Scintilla constant
constexpr int CP_CHINESE_TRADITIONAL  = 950;
constexpr int CP_CHINESE_SIMPLIFIED   = 936;
constexpr int CP_JAPANESE             = 932;
constexpr int CP_KOREAN               = 949;
constexpr int CP_GREEK                = 1253;

// Word-list mask flags for lexer keyword lists
constexpr int LIST_NONE = 0;
constexpr int LIST_0    = 1;
constexpr int LIST_1    = 2;
constexpr int LIST_2    = 4;
constexpr int LIST_3    = 8;
constexpr int LIST_4    = 16;
constexpr int LIST_5    = 32;
constexpr int LIST_6    = 64;
constexpr int LIST_7    = 128;
constexpr int LIST_8    = 256;

// Number of folder/folding states supported
constexpr int NB_FOLDER_STATE = 7;

// Marker indices used by Notepad++
constexpr int MARK_BOOKMARK         = 20;
constexpr int MARK_HIDELINESBEGIN   = 19;
constexpr int MARK_HIDELINESEND     = 18;

// Margin indices
constexpr int SC_MARGE_LINENUMBER    = 0;
constexpr int SC_MARGE_SYMBOL        = 1;
constexpr int SC_MARGE_CHANGEHISTORY = 2;
constexpr int SC_MARGE_FOLDER        = 3;

// Invalid buffer sentinel (nullptr-compatible, works with BufferID = Buffer*)
constexpr std::nullptr_t BUFFER_INVALID = nullptr;

// =============================================================================
// Forward declarations
// =============================================================================

class Buffer;
class UserDefineDialog;

// =============================================================================
// Language name table — mirrors Win32 ScintillaEditView::_langNameInfoArray
// This static table provides lexer name ↔ LangType mapping for menu labels,
// document type detection, and lexer activation.
//
// NOTE: Must be defined in the same order as LangType enum (Buffer.h).
// Used by Parameters.cpp and Notepad_plus.cpp for lexer→language mapping.
// =============================================================================
struct LanguageNameInfo {
    const char* _langName = nullptr;    // internal name (e.g. "php", "c", "html")
    const char* _shortName = nullptr;   // display name (e.g. "PHP", "C", "HTML")
    const char* _longName = nullptr;    // long description
    LangType _langID = LangType::L_TEXT;
    const char* _lexerID = nullptr;     // Scintilla lexer name
};
extern const LanguageNameInfo _langNameInfoArray[];

// =============================================================================
// ScintillaComponent
//
// Lifts ScintillaEditView (single editor widget) from Win32 to Qt6.
// Inherits from ScintillaEditBase (the Qt6 platform layer for Scintilla).
//
// Key Win32 → Qt6 translations:
//   HWND _hSelf (Win32 window handle) → ScintillaEditBase* (is the Qt widget)
//   SendMessage(_hSelf, SCI_GETDIRECTFUNCTION, ...) → send(SCI_GETDIRECTFUNCTION, ...)
//   SetWindowSubclass(_hSelf, ...) → installEventFilter() or subclass override
//   Scintilla_RegisterClasses(hInst) → implicit (ScintillaEditBase handles init)
//   DestroyWindow(_hSelf) → deleteLater()
// =============================================================================

class ScintillaComponent : public ScintillaEditBase
{
    Q_OBJECT

public:
    // --- Constructors / lifecycle ---

    explicit ScintillaComponent(QWidget* parent = nullptr);
    ~ScintillaComponent() override;

    void redraw() { update(); }  // Force repaint — mirrors Win32 InvalidateRect/RedrawWindow
    void redraw(bool /*unused*/) { update(); }  // overload used by DocumentMap.cpp

    // Semantic lift: Win32 init(hInst, hParent) → constructor + setup
    // Called once per editor instance to wire up the Scintilla widget.
    void init(QWidget* parent);
    void init(void* /*HINSTANCE*/, QWidget* parent) { init(parent); }
    void destroy();

    // Semantic lift: getHSelf() → this ScintillaEditBase* (QWidget*)
    ScintillaEditBase* getHSelf() { return this; }

    // --- Core: execute() mirrors Win32 SendMessage ---
    // Translates Win32-style SendMessage(hwnd, msg, wParam, lParam) to Qt6.
    // In Win32: _pScintillaFunc(_pScintillaPtr, Msg, wParam, lParam)
    // In Qt6:  send(Msg, wParam, lParam)  [inherited from ScintillaEditBase]
    // We keep an alias for clarity during the lift.
    sptr_t execute(unsigned int Msg, uptr_t wParam = 0, sptr_t lParam = 0) const {
        return send(Msg, wParam, lParam);
    }

    // --- Buffer / document management ---

    // Semantic lift: activateBuffer() — switch Scintilla to display a different buffer.
    // Win32: saves scroll/selection state, calls SCI_SETDOCPOINTER.
    // Qt6:   same, via send(SCI_SETDOCPOINTER, ...).
    void activateBuffer(Buffer* buffer);

    // Semantic lift: bufferUpdated() — applies buffer property changes to the view.
    // Win32: responds to BufferChange* flags (language, lexing, format, readonly, unicode).
    // Qt6:   same pattern.
    void bufferUpdated(Buffer* buffer, int changeMask);

    // Current buffer accessors
    Buffer* currentBuffer() const { return _currentBuffer; }
    void setCurrentBuffer(Buffer* buf);

    // Win32 compatibility shims
    Buffer* getCurrentBuffer() const { return currentBuffer(); }
    Buffer* getCurrentBufferID() const { return getCurrentBuffer(); }  // alias for plugin compat
    intptr_t getCurrentLineNumber() const { return static_cast<intptr_t>(ScintillaEditBase::currentLine()); }
    void setLineIndent(int line, int indent) { send(SCI_SETLINEINDENTATION, line, indent); }

    // Attach the default (startup) document as a Buffer
    void attachDefaultDoc();

    // --- Text / selection retrieval ---

    // Get text in [startPos, endPos) as narrow string
    void getText(char* dest, size_t start, size_t end) const;

    // getGenericText — 4-arg overload used by Notepad_plus.cpp (mirrors Win32 API)
    // dest: output buffer, bufSize: buffer capacity, start/end: byte positions
    void getGenericText(char* dest, size_t bufSize, size_t start, size_t end) const;

    // Get text as QString (Unicode)
    QString getTextAsString(size_t start, size_t end) const;

    // Insert text at position
    void insertText(size_t position, const char* text) const;
    void insertText(size_t position, const QString& text) const;

    // Replace the current selection with text
    void replaceSelWith(const char* replaceText) const;
    void replaceSelWith(const QString& text) const;

    // Add/append text without moving cursor
    void addText(size_t length, const char* buf) const;

    // Search in target (SciLexer find/replace)
    intptr_t searchInTarget(const QString& text, size_t fromPos, size_t toPos) const;
    intptr_t searchInTarget(const char* text, size_t len, size_t fromPos, size_t toPos) const;
    // Overloads used by Notepad_plus.cpp: std::string& text, non-const ref for fromPos
    // Overload 1: toPos as intptr_t by value — disambiguates lvalue calls (preferred over refs)
    intptr_t searchInTarget(const std::string& text, intptr_t& fromPos, intptr_t toPos) const;
    // Overload 2: len as int
    intptr_t searchInTarget(const std::string& text, intptr_t& fromPos, int len) const;
    // Overload 3: toPos as rvalue ref (for callers passing rvalue — intptr_t&& only binds temporaries)
    intptr_t searchInTarget(const std::string& text, intptr_t& fromPos, intptr_t&& toPos) const;

    // Replace the target range
    intptr_t replaceTarget(const QString& replacement, intptr_t fromPos = -1, intptr_t toPos = -1);
    intptr_t replaceTarget(const char* replacement, intptr_t fromPos = -1, intptr_t toPos = -1);
    // wstring overload for Notepad_plus.cpp compatibility
    intptr_t replaceTarget(const std::wstring& replacement, intptr_t fromPos = -1, intptr_t toPos = -1);

    // Get selected text as QString
    QString getSelectedText() const;

    // Get current line text
    QString getLine(size_t lineNumber) const;

    // Get word at caret
    QString getWordOnCaret() const;

    // --- Display / UI control ---

    // Show or hide a margin (line number, symbol, folder, change history)
    void showMargin(int whichMargin, bool visible = true);
    bool hasMarginShown(int whichMargin) const;

    // Bookmark margin click handler
    void marginClick(size_t position, int modifiers);

    // Bookmarks
    void addBookmark(size_t line);
    void removeBookmark(size_t line);
    bool hasBookmark(size_t line) const;
    void clearAllBookmarks();

    // Folding
    void fold(size_t line, bool collapse) const;
    void fold(size_t line, bool collapse, bool /*unused*/) const { fold(line, collapse); }
    void foldAll(bool collapse) const;
    void expand(size_t& line, bool doExpand, bool force = false) const;
    bool isFolded(size_t line) const;
    void foldCurrentPos(bool collapse);
    bool isCurrentLineFolded() const;

    // Visibility helpers
    void showInvisibleChars(bool show = true);
    bool isShownInvisibleChars() const;
    void showIndentGuide(bool show = true);
    bool isShownIndentGuide() const;
    void wrapText(bool wrap = true);
    void wrap(bool doWrap) { wrapText(doWrap); }  // public alias for wrapText
    bool isWrap() const;
    void showWrapSymbol(bool show = true);
    bool isWrapSymbolVisible() const;
    void showEOL(bool show = true);
    bool isShownEOL() const;

    // Win32 compat: getWidth() — return viewport width
    int getWidth() const { return width(); }

    // --- Scrolling / cursor ---

    void scrollTo(size_t line);
    void scrollToCenter(size_t position);
    void gotoLine(size_t line);
    void scroll(int columns, int lines);
    intptr_t currentColumn() const;
    intptr_t currentXOffset() const { return send(SCI_GETXOFFSET); }
    void setXOffset(long offset) { send(SCI_SETXOFFSET, offset); }
    intptr_t pointXFromPosition(size_t pos) const { return send(SCI_POINTXFROMPOSITION, 0, pos); }
    intptr_t pointYFromPosition(size_t pos) const { return send(SCI_POINTYFROMPOSITION, 0, pos); }
    intptr_t textHeight() const { return send(SCI_TEXTHEIGHT); }

    // --- Selection / editing helpers ---

    void beginSelect();
    void endSelect();
    // selectAll() is inherited from QsciScintilla — no override needed.
    bool hasSelection() const;
    size_t selectionStart() const { return send(SCI_GETSELECTIONSTART); }
    size_t selectionEnd() const { return send(SCI_GETSELECTIONEND); }
    Sci_CharacterRangeFull getSelection() const {
        Sci_CharacterRangeFull cr;
        cr.cpMin = static_cast<Sci_Position>(send(SCI_GETSELECTIONSTART));
        cr.cpMax = static_cast<Sci_Position>(send(SCI_GETSELECTIONEND));
        return cr;
    }
    void setSelection(size_t start, size_t end);
    void gotoPos(size_t pos);

    // --- Lexer / styling ---

    // Apply lexer and default styles for the given language
    void defineDocType(int langType);
    void styleChange();

    // Set lexer language (maps LangType → Scintilla lexer constant)
    void setLexerLanguage(int langType);
    void setKeywords(int keywordSet, const char* keywords);
    void setStyle(const QColor& fore, const QColor& back, const QString& fontName = QString(), int fontSize = 0, int fontStyle = 0);

    // Clear a highlight indicator
    void clearIndicator(int indicatorNumber);

    // Smart highlighting
    void highlightAll(const QString& text);
    void clearHighlightAll();

    // --- Position save/restore (for buffer switching) ---

    void saveCurrentPos();
    void restoreCurrentPos();
    // Additional restore steps needed by Notepad_plus.cpp
    void restoreCurrentPosPreStep() { restoreCurrentPos(); }
    void restoreHiddenLines() { /* stub: line state restored via Buffer */ }

    // --- Visibility helpers for URL highlighting ---

    void getVisibleStartAndEndPosition(intptr_t* startPos, intptr_t* endPos) const;
    intptr_t getCurrentColumnNumber() const { return currentColumn(); }

    // --- Line operations ---

    void insertNewLineAbove();
    void insertNewLineBelow();
    void currentLinesUp() const;
    void currentLinesDown() const;

    // --- EOL ---
    QString getEolString() const;

    // --- Scroll / navigation ---
    void scrollPosToCenter(size_t pos);

    // --- Line sorting / manipulation ---
    void sortLines(size_t fromLine, size_t toLine, ISorter* pSort);
    void removeDuplicateLines();
    void convertSelectedTextToUpperCase() { execute(SCI_UPPERCASE); }
    void convertSelectedTextToLowerCase() { execute(SCI_LOWERCASE); }
    void removeEmptyLine(bool includeBlankLines);  // stub: uses regex to remove empty lines
    bool hidelineMarkerClicked(size_t lineNumber);

    // --- Hide/show lines ---
    void hideMarkedLines(size_t searchStart, bool toEndOfDoc) const;
    void showHiddenLines(size_t searchStart, bool toEndOfDoc, bool doDelete) const;

    // --- NPC (non-printing characters) ---
    void showNpc(bool willBeShown, bool isSearchResult = false);
    void showCcUniEol(bool willBeShown, bool isSearchResult = false);
    void setNpcAndCcUniEOL(long color = -1);

    // --- Column mode ---
    void beginOrEndSelect(bool isColumnMode);
    void updateBeginEndSelectPosition(bool isInsert, size_t position, size_t length);
    ColumnModeInfos getColumnModeSelectInfo();
    void columnReplaceString(ColumnModeInfos& cmi, const char* str) const;
    void columnReplaceNumeric(ColumnModeInfos& cmi, size_t initial, size_t incr,
                             size_t repeat, int format, int lead) const;
    void setMultiSelections(const ColumnModeInfos& cmi);
    bool pasteToMultiSelection();

    // --- Word selection ---
    std::pair<size_t, size_t> getWordRange();
    bool expandWordSelection();

    // --- Selection info ---
    size_t getUnicodeSelectedLength() const;
    std::pair<size_t, size_t> getSelectionLinesRangeSel(intptr_t selNum) const;
    std::pair<size_t, size_t> getSelectionLinesRange() const {
        return getSelectionLinesRangeSel(0);
    }
    bool getIndicatorRangeForPos(size_t indicatorNumber, size_t* from = nullptr,
                                  size_t* to = nullptr, size_t* cur = nullptr);

    // --- Folding expand overloads ---
    void expandFull(size_t& line, bool doExpand, bool force, int visLevels, int level) const;
    void marginClickExpand(size_t position, int modifiers);

    // --- Margin display with DPI ---
    void showMarginWithDpi(int whichMargin, bool visible, int dpi);
    void showChangeHistoryMargin(bool visible, int dpi);

    // --- Indentation ---
    bool isPythonStyleIndentationForLang(LangType typeDoc) const;

    // --- Clipboard ---
    void markedTextToClipboard(int indiStyle, bool doAll = false);

    // --- EOL conversion ---
    void setEolModeAndConvert(int mode);

    // --- Search/highlight helper ---
    void searchAndHighlight(ScintillaComponent* pHighlightView, const QByteArray& textUtf8);

    // --- Line number width ---

    void updateLineNumberWidth();
    void setLineNumberWidth(int width);

    // --- Zoom ---

    void zoomIn()  { send(SCI_ZOOMIN); }
    void zoomOut() { send(SCI_ZOOMOUT); }
    int zoom() const { return send(SCI_GETZOOM); }
    void setZoom(int zoomLevel) { send(SCI_SETZOOM, zoomLevel); }

    // --- Read-only state ---

    void setReadOnly(bool ro) { send(SCI_SETREADONLY, ro); }
    bool isReadOnly() const { return send(SCI_GETREADONLY) != 0; }

    // --- Encoding / EOL ---

    int codepage() const { return _codepage; }
    void setCodepage(int cp) { _codepage = cp; send(SCI_SETCODEPAGE, cp); }
    void setEolMode(int mode) { send(SCI_SETEOLMODE, mode); }
    int eolMode() const { return send(SCI_GETEOLMODE); }
    void setCRLF() { send(SCI_SETEOLMODE, SC_EOL_CRLF); send(SCI_CONVERTEOLS, SC_EOL_CRLF); }

    // --- Selection info ---
    std::pair<size_t, size_t> getSelectedCharsAndLinesCount(size_t = 0) const;  // stub with optional maxSels param

    // --- Undo/Redo ---

    void undo() { send(SCI_UNDO); }
    void redo() { send(SCI_REDO); }
    bool canUndo() const { return send(SCI_CANUNDO) != 0; }
    bool canRedo() const { return send(SCI_CANREDO) != 0; }

    // --- Search/Find helpers ---

    // Set search flags (SCFIND_*)
    void setSearchFlags(unsigned int flags) { send(SCI_SETSEARCHFLAGS, flags); }
    unsigned int searchFlags() const { return send(SCI_GETSEARCHFLAGS); }

    // Target range for search
    void setTargetRange(size_t start, size_t end) {
        send(SCI_SETTARGETSTART, start);
        send(SCI_SETTARGETEND, end);
    }

    // --- DocumentMap / minimap helpers (stubs for Qt6 port) ---

    // Returns the width of the text display zone (used by minimap for text rendering)
    int getTextZoneWidth() const { return 0; }

    // Syncs fold state from a buffer's stored line state vector
    void syncFoldStateWith(const std::vector<size_t>& lineStateVector) { Q_UNUSED(lineStateVector); }

    // Sets the Scintilla document pointer directly (mirrors SCI_SETDOCPOINTER)
    void setDocPointer(intptr_t doc) { send(SCI_SETDOCPOINTER, 0, doc); }

    // --- Missing API stubs for compatibility ---

    // Returns the zero-based index of the last line (lineCount - 1)
    intptr_t lastZeroBasedLineNumber() const {
        return static_cast<intptr_t>(send(SCI_GETLINECOUNT)) - 1;
    }

    // Qt6: forwards to QWidget::setFocus()
    void grabFocus() { setFocus(); }

    // Returns the length (in bytes) of the given line
    size_t getLineLength(size_t line) const {
        return send(SCI_LINELENGTH, line);
    }

    // Returns the indentation of the given line in spaces
    int getLineIndent(size_t line) const {
        return static_cast<int>(send(SCI_GETLINEINDENTATION, line));
    }

    // Stub: maintains NPC state for a buffer
    void maintainStateForNpc(Buffer* /*buf*/) {}
    void maintainStateForNpc() {}  // no-arg overload used by Notepad_plus.cpp

    // Inserts generic text at a given line indentation position
    // Position: zero-based line indent position; text: null-terminated string
    void insertGenericTextFrom(size_t position, const char* text) {
        if (!text) return;
        send(SCI_INSERTTEXT, static_cast<uptr_t>(position),
             reinterpret_cast<sptr_t>(text));
    }

    // Document map / fold helpers
    QRect getClientRect() const { return rect(); }
    size_t getCurrentDocLen() const { return static_cast<size_t>(send(SCI_GETLENGTH)); }
    std::vector<size_t> getCurrentFoldStates() const;
    // Win32 compat: getCurrentFoldStates(out_vector) — fills the provided vector
    void getCurrentFoldStates(std::vector<size_t>& outFoldStates) const;
    bool isTextDirectionRTL() const;
    void changeTextDirection(bool toRTL);

    // Static: returns the UserDefineDialog singleton
    static UserDefineDialog* getUserDefineDlg();

    // Static: returns the UserDefineDialog singleton (alternate entry point)
    static UserDefineDialog* getUserDefineDialog() { return getUserDefineDlg(); }

signals:
    // Forward Scintilla notifications as Qt signals.
    // These are connected by MainWindow to drive the UI (status bar, tab state, etc.)
    // Corresponds to Win32 WM_NOTIFY / SCN_* notifications via WindowProc.
    void marginClicked(size_t position, int modifiers, int margin);
    void doubleClicked(size_t position, size_t line);
    void modified(int type, size_t position, size_t length,
                  const QString& text, size_t line, int foldLevelNow, int foldLevelPrev);
    void updateUi(int updated);
    void charAdded(int ch);
    void savePointChanged(bool dirty);
    void modifyAttemptReadOnly();
    void zoomChanged(int zoom);
    void needShown(size_t position, size_t length);
    void painted();
    void userListSelection(const QString& text, int type);

    // Emitted when the current buffer changes
    void bufferActivated(Buffer* buffer);

    // Emitted on each SCN_UPDATEUI notification
    void uiChanged();

    // Emitted when text content changes (used to mark buffer dirty)
    void textChanged();

    // Emitted when cursor moves (line, index) — used to update status bar
    void cursorPositionChanged(int line, int index);

protected:
    // Install event filter to handle custom messages that previously went through
    // the subclassed ScintillaProc (Win32) → now handled via Qt's event() system.
    bool event(QEvent* event) override;

    // Forward SCN_* notifications from Scintilla to Qt signals
    // (ScintillaEditBase already emits some; override to emit more)
    void notifyParent(const SCNotification* scn) override;


private:
    // --- Setup helpers ---
    void setupScintilla();
    void setupIndicators();
    void setupMargins();
    void setupFonts();
    void setupMarkers();

    // --- Lexer management ---
    QsciLexer* _lexer = nullptr;  // owned by this widget (QsciScintilla takes ownership)
    QsciLexer* createLexerForLanguage(int langType);

public:
    // Apply global styles (folding, selection, caret, etc.)
    void performGlobalStyles();

    // Restore position after buffer switch (called from activateBuffer)
    void restoreScrollState();
    void restoreSelectionState();
    void saveScrollState();
    void saveSelectionState();

    // --- Buffer state ---
    Buffer* _currentBuffer = nullptr;

    // --- Codepage ---
    int _codepage = 0;  // 0 = system ANSI codepage (CP_ACP)

    // --- Position save/restore ---
    size_t _savedFirstVisibleLine = 0;
    size_t _savedAnchor = 0;
    size_t _savedCaret = 0;
    size_t _savedXOffset = 0;
    int    _savedSelMode = 0;
    size_t _savedScrollWidth = 1;

    // --- Selection state ---
    intptr_t _beginSelectPosition = -1;  // -1 = no active column selection

    // --- Fold state ---
    QVector<size_t> _foldedLines;  // lines currently collapsed

    // --- Scintilla resource management (static refCount pattern) ---
    static int _refCount;           // number of living ScintillaComponent instances
    static bool _sciInit;           // whether Scintilla library has been initialized
    static QString _defaultWordChars;  // original word-character set from Scintilla

    // -------------------------------------------------------------------------
    // Aliases for Win32 compatibility — Notepad_plus.cpp uses ScintillaEditView*
    // These static consts mirror the Win32 _SC_MARGE_* names with underscore prefix
    // -------------------------------------------------------------------------
    static constexpr int _SC_MARGE_LINENUMBER = SC_MARGE_LINENUMBER;
    static constexpr int _SC_MARGE_SYMBOL     = SC_MARGE_SYMBOL;
    static constexpr int _SC_MARGE_FOLDER    = SC_MARGE_FOLDER;
    static constexpr int _SC_MARGE_CHANGEHISTORY = SC_MARGE_CHANGEHISTORY;

    // Win32 compatibility: display() → show()
    void display() { show(); }
    void display(bool doShow) { doShow ? show() : hide(); }
    void setPositionReached(intptr_t pos) { send(SCI_SETCURRENTPOS, pos); }
    void getGenericTextInPage() {}  // stub: used by N++ for clipboard text

public:
    // Win32 compatibility aliases
    void showIndentGuideLine(bool show = true) { showIndentGuide(show); }

    // showWSAndTab — controls whitespace and tab visibility via SCI_SETVIEWWS
    // SC_VIEWWS_INVISIBLE=0, SC_VIEWWS_VISIBLEALWAYS=1
    void showWSAndTab(bool show) {
        execute(SCI_SETVIEWWS, show ? 1 : 0);
    }

    // getGenericTextAsString — extract text range as std::wstring (public wrapper)
    std::wstring getGenericTextAsString(size_t start, size_t end) const {
        intptr_t len = execute(SCI_GETLENGTH);
        int bufLen = static_cast<int>(len * 4 + 1);
        std::string s(bufLen, 0);
        Sci_CharacterRangeFull r{static_cast<intptr_t>(start), static_cast<intptr_t>(end)};
        execute(SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&r));
        return QString::fromLatin1(s.data(), static_cast<int>(s.size())).toStdWString();
    }

    // Win32 compatibility stubs (Win32 method names with Qt6 or no-op implementations)
    // setMakerStyle(style) — sets the folder/collapse marker style
    void setMakerStyle(int style);
    void setWrapMode(int mode);

    // setBorderEdge(show) — draws border around editor
    void setBorderEdge(bool show);

    // Overload init(HINSTANCE, HWND) for Win32 API compatibility.
    // HINSTANCE and HWND are unused in Qt6 — the single-arg init() is sufficient.
};

// Forward type alias: Win32 used ScintillaEditView as the concrete editor class.
// In the Qt6 lift, ScintillaComponent IS that class. Buffer.h forward-declares
// ScintillaEditView, so this alias satisfies those declarations without renaming
// every usage throughout the codebase.
using ScintillaEditView = ScintillaComponent;

// =============================================================================
// ScintillaCtrls
//
// Lifts ScintillaCtrls (Win32) → Qt6 factory/manager for ScintillaComponent instances.
//
// Win32 patterns:
//   HWND createSintilla(HWND hParent)  →  createEditor(QWidget* parent)
//   std::vector<ScintillaEditView*>     →  QVector<ScintillaComponent*>
//   HINSTANCE _hInst                    →  QApplication* (via qApp)
// =============================================================================

class ScintillaCtrls : public QObject
{
    Q_OBJECT

public:
    explicit ScintillaCtrls(QObject* parent = nullptr);
    ~ScintillaCtrls() override;

    // Semantic lift: createSintilla(hParent) → createEditor(parent)
    // Creates a new ScintillaComponent and registers it.
    ScintillaComponent* createEditor(QWidget* parent);

    // Semantic lift: getScintillaEditViewFrom(handle) → editorFromIndex(index)
    ScintillaComponent* editorFromIndex(int index);
    ScintillaComponent* editorFromWidget(QWidget* widget) const;

    // Number of active editor instances
    int count() const { return _editors.size(); }

    // Semantic lift: init() — stub for Win32-style initialization.
    // In Qt6, editors are created directly via createEditor().
    void init(void* /*hInst*/, QWidget* /*parent*/) { /* no-op: Qt6 editors are created on demand */ }

    // Destroy all editors and clear the list
    void destroyAll();

    // destroy — clean up all Scintilla instances (called from Notepad_plus destructor)
    void destroy();

    // Remove a specific editor
    void removeEditor(ScintillaComponent* editor);

public slots:
    // Propagate dark mode changes to all editors
    void setDarkMode(bool enabled);

private:
    int indexOf(ScintillaComponent* editor) const;

    QVector<ScintillaComponent*> _editors;
};
