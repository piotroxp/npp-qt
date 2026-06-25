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

#pragma once

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QColor>
#include <QApplication>
#include <QScrollBar>
#include <QEvent>
#include "NppConstants.h"

// ScintillaEditBase provides the Qt6 platform layer for Scintilla.
// Stub implementation lives in src/ScintillaEditBase.{h,cpp}; replace with
// the real Scintilla Qt port (scintilla/qt/) when available.
#include "ScintillaEditBase.h"

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

// Invalid buffer sentinel
constexpr intptr_t BUFFER_INVALID    = 0;

// =============================================================================
// Forward declarations
// =============================================================================

class Buffer;

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

    // Semantic lift: Win32 init(hInst, hParent) → constructor + setup
    // Called once per editor instance to wire up the Scintilla widget.
    void init(QWidget* parent);
    void init(void* /*HINSTANCE*/, QWidget* parent) { init(parent); }
    void destroy();

    // Semantic lift: getHSelf() → this ScintillaEditBase* (QWidget*)
    ScintillaEditBase* getHSelf() const { return const_cast<ScintillaComponent*>(this); }

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

    // Attach the default (startup) document as a Buffer
    void attachDefaultDoc();

    // --- Text / selection retrieval ---

    // Get text in [startPos, endPos) as narrow string
    void getText(char* dest, size_t start, size_t end) const;

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

    // Replace the target range
    intptr_t replaceTarget(const QString& replacement, intptr_t fromPos = -1, intptr_t toPos = -1);
    intptr_t replaceTarget(const char* replacement, intptr_t fromPos = -1, intptr_t toPos = -1);

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
    bool isWrap() const;
    void showWrapSymbol(bool show = true);
    bool isWrapSymbolVisible() const;
    void showEOL(bool show = true);
    bool isShownEOL() const;

    // --- Scrolling / cursor ---

    void scrollTo(size_t line);
    void scrollToCenter(size_t position);
    void gotoLine(size_t line);
    void scroll(int columns, int lines);
    intptr_t currentLine() const;
    intptr_t currentColumn() const;
    intptr_t currentXOffset() const { return send(SCI_GETXOFFSET); }
    void setXOffset(long offset) { send(SCI_SETXOFFSET, offset); }
    intptr_t pointXFromPosition(size_t pos) const { return send(SCI_POINTXFROMPOSITION, 0, pos); }
    intptr_t pointYFromPosition(size_t pos) const { return send(SCI_POINTYFROMPOSITION, 0, pos); }
    intptr_t textHeight() const { return send(SCI_TEXTHEIGHT); }

    // --- Selection / editing helpers ---

    void beginSelect();
    void endSelect();
    void selectAll();
    bool hasSelection() const;
    size_t selectionStart() const { return send(SCI_GETSELECTIONSTART); }
    size_t selectionEnd() const { return send(SCI_GETSELECTIONEND); }
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

    // --- Line operations ---

    void insertNewLineAbove();
    void insertNewLineBelow();
    void currentLinesUp() const;
    void currentLinesDown() const;

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

    // Win32 compatibility aliases
    void showIndentGuideLine(bool show = true) { showIndentGuide(show); }
    void wrap(bool doWrap) { wrapText(doWrap); }

    // Win32 compatibility stubs (Win32 method names with Qt6 or no-op implementations)
    // setMakerStyle(style) — sets the folder/collapse marker style
    void setMakerStyle(int style);

    // setWrapMode(mode) — SCI_WRAPMODE values
    void setWrapMode(int mode);

    // setBorderEdge(show) — draws border around editor
    void setBorderEdge(bool show);

    // Win32 compatibility: getCurrentBuffer() → currentBuffer()
    Buffer* getCurrentBuffer() const { return currentBuffer(); }

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

    // Destroy all editors and clear the list
    void destroyAll();

    // Remove a specific editor
    void removeEditor(ScintillaComponent* editor);

public slots:
    // Propagate dark mode changes to all editors
    void setDarkMode(bool enabled);

private:
    int indexOf(ScintillaComponent* editor) const;

    QVector<ScintillaComponent*> _editors;
};
