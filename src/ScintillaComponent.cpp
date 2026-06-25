// Semantic Lift: ScintillaComponent from Win32 → Qt6
// Original: PowerEditor/src/ScintillaComponent/ScintillaEditView.cpp (4956 lines)
// Target:   npp-qt/src/ScintillaComponent.cpp
//
// Qt6 semantic equivalents:
//   HWND _hSelf              → ScintillaEditBase* (is the widget)
//   SendMessage(_hSelf, ...)  → send(...)
//   SetWindowSubclass()       → installEventFilter()
//   Scintilla_RegisterClasses() → implicit (ScintillaEditBase handles init)
//   ::DestroyWindow(_hSelf)   → deleteLater() / QObject cleanup
//   SCI_GETDIRECTFUNCTION      → send(SCI_GETDIRECTFUNCTION)
//   SCI_GETDIRECTPOINTER       → send(SCI_GETDIRECTPOINTER)

#include "ScintillaComponent.h"

#include <QString>
#include <Scintilla.h>
#include <SciLexer.h>
// ScintillaStructures.h was a phantom include — Sci_TextRange/Sci_CharacterRangeFull
// are defined locally in this file (see local struct definitions near line 527).

#include "Buffer.h"

#include <QApplication>
#include <QColorDialog>
#include <QFile>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QMessageBox>
#include <QPainter>
#include <QStyle>
#include <QString>
#include <cstring>

// =============================================================================
// Forward declarations from ScintillaEditBase (Qt platform layer)
// =============================================================================

namespace Scintilla { struct NotificationData; }

// =============================================================================
// Static variable definitions
// =============================================================================

int  ScintillaComponent::_refCount = 0;
bool ScintillaComponent::_sciInit = false;
QString ScintillaComponent::_defaultWordChars;

// =============================================================================
// Helper: set a Scintilla element colour
// =============================================================================

static void setElementColourIfDark(ScintillaComponent* sci, int element, const QColor& dark, bool enabled)
{
    Q_UNUSED(sci);
    Q_UNUSED(element);
    Q_UNUSED(dark);
    Q_UNUSED(enabled);
    // SCI_SETELEMENTCOLOUR is available in newer Scintilla.
    // Defer to the style-based approach via SCI_STYLESETBACK.
}

// =============================================================================
// Helper: SCI colour → QColor
// =============================================================================

static QColor sciColorToQColor(int rgb)
{
    return QColor(rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF);
}

static int qColorToSciRGB(const QColor& c)
{
    return (c.red() << 0) | (c.green() << 8) | (c.blue() << 16);
}

// =============================================================================
// ScintillaComponent — construction
// =============================================================================

ScintillaComponent::ScintillaComponent(QWidget* parent)
    : ScintillaEditBase(parent)
{
    ++_refCount;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Accept drops for drag-and-drop file open
    setAcceptDrops(true);

    // Connect base-class notifications to our Qt signals
    connect(this, &ScintillaEditBase::marginClicked,
            this, [this](Scintilla::Position pos, Scintilla::KeyMod mod, int margin) {
                emit marginClicked(static_cast<size_t>(pos), static_cast<int>(mod), margin);
            });

    connect(this, &ScintillaEditBase::doubleClick,
            this, [this](Scintilla::Position pos, Scintilla::Position line) {
                emit doubleClicked(static_cast<size_t>(pos), static_cast<size_t>(line));
            });

    connect(this, &ScintillaEditBase::modified,
            this, [this](Scintilla::ModificationFlags type,
                         Scintilla::Position pos,
                         Scintilla::Position len,
                         const QByteArray& text,
                         Scintilla::Position line,
                         Scintilla::FoldLevel foldNow,
                         Scintilla::FoldLevel foldPrev) {
                emit modified(static_cast<int>(type),
                              static_cast<size_t>(pos),
                              static_cast<size_t>(len),
                              QString::fromUtf8(text),
                              static_cast<size_t>(line),
                              static_cast<int>(foldNow),
                              static_cast<int>(foldPrev));
            });

    connect(this, &ScintillaEditBase::updateUi,
            this, [this](Scintilla::Update updated) {
                emit updateUi(static_cast<int>(updated));
            });

    connect(this, &ScintillaEditBase::charAdded,
            this, [this](int ch) { emit charAdded(ch); });

    connect(this, &ScintillaEditBase::savePointChanged,
            this, [this](bool dirty) { emit savePointChanged(dirty); });

    connect(this, &ScintillaEditBase::modifyAttemptReadOnly,
            this, [this]() { emit modifyAttemptReadOnly(); });

    connect(this, &ScintillaEditBase::zoom,
            this, [this](int z) { emit zoomChanged(z); });

    connect(this, &ScintillaEditBase::needShown,
            this, [this](Scintilla::Position pos, Scintilla::Position len) {
                emit needShown(static_cast<size_t>(pos), static_cast<size_t>(len));
            });

    connect(this, &ScintillaEditBase::painted,
            this, &ScintillaComponent::painted);

    connect(this, &ScintillaEditBase::userListSelection,
            this, [this](const QString& text, int type) {
                emit userListSelection(text, type);
            });

    // Connect our internal uiChanged signal to the updateUi slot
    connect(this, &ScintillaComponent::uiChanged,
            this, &ScintillaComponent::updateLineNumberWidth);
}

ScintillaComponent::~ScintillaComponent()
{
    --_refCount;
    destroy();
}

// =============================================================================
// ScintillaComponent::init — wires up scintilla after construction
// Mirrors Win32: ScintillaEditView::init(hInst, hParent)
// =============================================================================

void ScintillaComponent::init(QWidget* parent)
{
    if (parent)
        setParent(parent);

    setupScintilla();
    setupIndicators();
    setupMargins();
    setupFonts();
    setupMarkers();
    performGlobalStyles();

    // Show the widget
    show();
}

// =============================================================================
// ScintillaComponent::destroy — clean up scintilla resources
// Mirrors Win32: ::DestroyWindow(_hSelf)
// =============================================================================

void ScintillaComponent::destroy()
{
    // Detach from any buffer
    if (_currentBuffer) {
        // Save position before destroying
        saveCurrentPos();
        _currentBuffer = nullptr;
    }

    // QWidget cleanup — will trigger ScintillaEditBase destructor
    // which cleans up the Scintilla document.
    // Calling deleteLater() to ensure safe deletion from event loop.
    // For immediate cleanup in destructor, we rely on QObject parent-child
    // relationship.
}

// =============================================================================
// ScintillaComponent::setupScintilla
// =============================================================================

void ScintillaComponent::setupScintilla()
{
    // Idle styling — restyle on idle when needed
    send(SCI_SETIDLESTYLING, SC_IDLESTYLING_ALL);

    // Set event mask — capture relevant notifications
    send(SCI_SETMODEVENTMASK, SC_MODEVENTMASKALL);

    // Set initial scroll width tracking
    send(SCI_SETSCROLLWIDTHTRACKING, true);
    send(SCI_SETSCROLLWIDTH, 1);

    // Set caret style: blinking vertical bar
    send(SCI_SETCARETSTYLE, CARETSTYLE_LINE_1);
    send(SCI_SETCARETWIDTH, 2);

    // Selection
    send(SCI_SETSELBACK, true, 0x808080);

    // Mouse wheel smooth scrolling handled by QAbstractScrollArea

    // Multi-select (column mode) support
    send(SCI_SETMULTIPLESELECTION, true);
    send(SCI_SETMULTIPASTE, 1);

    // Word wrap: off by default (user can toggle)
    send(SCI_SETWRAPMODE, SC_WRAP_NONE);

    // End of line mode: default to Windows (CRLF)
    send(SCI_SETEOLMODE, SC_EOL_CRLF);
    send(SCI_SETVIEWEOL, 0);

    // Virtual space: allow cursor beyond line ends
    send(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION);

    // Implict tab completion (auto-close brackets)
    send(SCI_AUTOCSETSEPARATOR, ' ');
    send(SCI_AUTOCSETFILLUPS, 0, reinterpret_cast<sptr_t>(""));
}

// =============================================================================
// ScintillaComponent::setupIndicators
// =============================================================================

void ScintillaComponent::setupIndicators()
{
    // Smart/highlight-all indicators (universal found styles)
    const int indicators[] = {
        SCE_UNIVERSAL_FOUND_STYLE,
        SCE_UNIVERSAL_FOUND_STYLE_SMART,
        SCE_UNIVERSAL_FOUND_STYLE_INC,
        SCE_UNIVERSAL_TAGMATCH,
        SCE_UNIVERSAL_TAGATTR,
        SCE_UNIVERSAL_FOUND_STYLE_EXT1,
        SCE_UNIVERSAL_FOUND_STYLE_EXT2,
        SCE_UNIVERSAL_FOUND_STYLE_EXT3,
        SCE_UNIVERSAL_FOUND_STYLE_EXT4,
        SCE_UNIVERSAL_FOUND_STYLE_EXT5
    };

    for (int ind : indicators) {
        send(SCI_INDICSETSTYLE, ind, INDIC_ROUNDBOX);
        send(SCI_INDICSETALPHA, ind, 100);
        send(SCI_INDICSETUNDER, ind, true);
    }

    // URL indicator
    send(SCI_INDICSETSTYLE, INDIC_HYPERLINK, INDIC_SQUIGGLE);
    send(SCI_INDICSETFORE, INDIC_HYPERLINK, qColorToSciRGB(Qt::blue));

    // Spell-check indicator (if used)
    send(SCI_INDICSETSTYLE, INDIC_SPELL, INDIC_SQUIGGLE);
    send(SCI_INDICSETFORE, INDIC_SPELL, qColorToSciRGB(Qt::red));
}

// =============================================================================
// ScintillaComponent::setupMargins
// =============================================================================

void ScintillaComponent::setupMargins()
{
    // Margin 0: Line numbers
    send(SCI_SETMARGINTYPEN, SC_MARGE_LINENUMBER, SC_MARGIN_NUMBER);
    send(SCI_SETMARGINWIDTHN, SC_MARGE_LINENUMBER, 0); // hidden by default, shown on demand

    // Margin 1: Symbol (bookmarks, hide-line markers)
    send(SCI_SETMARGINTYPEN, SC_MARGE_SYMBOL, SC_MARGIN_SYMBOL);
    send(SCI_SETMARGINMASKN, SC_MARGE_SYMBOL,
         (1 << MARK_BOOKMARK) | (1 << MARK_HIDELINESBEGIN) | (1 << MARK_HIDELINESEND));
    send(SCI_SETMARGINWIDTHN, SC_MARGE_SYMBOL, 0);
    send(SCI_SETMARGINSENSITIVEN, SC_MARGE_SYMBOL, true);

    // Margin 2: Change history (modified/saved markers)
    send(SCI_SETMARGINTYPEN, SC_MARGE_CHANGEHISTORY, SC_MARGIN_SYMBOL);
    send(SCI_SETMARGINMASKN, SC_MARGE_CHANGEHISTORY,
         (1 << SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN) |
         (1 << SC_MARKNUM_HISTORY_SAVED) |
         (1 << SC_MARKNUM_HISTORY_MODIFIED) |
         (1 << SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED));
    send(SCI_SETMARGINWIDTHN, SC_MARGE_CHANGEHISTORY, 0);
    send(SCI_SETMARGINSENSITIVEN, SC_MARGE_CHANGEHISTORY, true);

    // Margin 3: Folding (+ / - tree)
    send(SCI_SETMARGINTYPEN, SC_MARGE_FOLDER, SC_MARGIN_SYMBOL);
    send(SCI_SETMARGINMASKN, SC_MARGE_FOLDER, SC_MASK_FOLDERS);
    send(SCI_SETMARGINWIDTHN, SC_MARGE_FOLDER, 0);
    send(SCI_SETMARGINSENSITIVEN, SC_MARGE_FOLDER, true);

    // Fold flags: show line after contracted fold
    send(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED);
}

// =============================================================================
// ScintillaComponent::setupFonts
// =============================================================================

void ScintillaComponent::setupFonts()
{
    // Default editor font: system monospace
    const QString defaultFont = QStringLiteral("Courier New");
    const int defaultFontSize = 10;

    // Style 0 is the default style
    send(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<sptr_t>("Courier New"));
    send(SCI_STYLESETSIZE, STYLE_DEFAULT, defaultFontSize);
    send(SCI_STYLESETFORE, STYLE_DEFAULT, qColorToSciRGB(Qt::black));
    send(SCI_STYLESETBACK, STYLE_DEFAULT, qColorToSciRGB(Qt::white));

    // Apply to all styles via STYLE_DEFAULT
    send(SCI_STYLECLEARALL);
}

// =============================================================================
// ScintillaComponent::setupMarkers — define folding + bookmark + hide markers
// =============================================================================

void ScintillaComponent::setupMarkers()
{
    // Define each folder/folding marker directly via Scintilla's SC_MARK_* constants
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXPLUS);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXMINUS);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);

    // Marker colors for folder state
    send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDER, qColorToSciRGB(QColor(0xCC, 0xCC, 0xCC)));
    send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDER, qColorToSciRGB(Qt::black));

    // Bookmark marker (circle, semi-transparent)
    send(SCI_MARKERDEFINE, MARK_BOOKMARK, SC_MARK_CIRCLE);
    send(SCI_MARKERSETBACK, MARK_BOOKMARK, qColorToSciRGB(QColor(0, 120, 215)));
    send(SCI_MARKERSETFORE, MARK_BOOKMARK, qColorToSciRGB(Qt::white));
    send(SCI_MARKERSETALPHA, MARK_BOOKMARK, 180);
    send(SCI_RGBAIMAGESETWIDTH, 14);
    send(SCI_RGBAIMAGESETHEIGHT, 14);

    // Hide-lines markers
    send(SCI_MARKERDEFINE, MARK_HIDELINESBEGIN, SC_MARK_EMPTY);
    send(SCI_MARKERDEFINE, MARK_HIDELINESEND, SC_MARK_EMPTY);

    // Change history: modified (orange dot)
    send(SCI_MARKERDEFINE, SC_MARKNUM_HISTORY_MODIFIED, SC_MARK_DOT);
    send(SCI_MARKERSETBACK, SC_MARKNUM_HISTORY_MODIFIED, qColorToSciRGB(QColor(255, 128, 0)));
    send(SCI_MARKERSETALPHA, SC_MARKNUM_HISTORY_MODIFIED, SC_ALPHA_NOALPHA);

    // Change history: saved (green dot)
    send(SCI_MARKERDEFINE, SC_MARKNUM_HISTORY_SAVED, SC_MARK_DOT);
    send(SCI_MARKERSETBACK, SC_MARKNUM_HISTORY_SAVED, qColorToSciRGB(Qt::green));
    send(SCI_MARKERSETALPHA, SC_MARKNUM_HISTORY_SAVED, SC_ALPHA_NOALPHA);

    // Change history: reverted markers
    send(SCI_MARKERDEFINE, SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED, SC_MARK_EMPTY);
    send(SCI_MARKERDEFINE, SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN, SC_MARK_EMPTY);
}

// =============================================================================
// ScintillaComponent::performGlobalStyles — base styling for selection, caret, etc.
// =============================================================================

void ScintillaComponent::performGlobalStyles()
{
    // Selection highlight
    send(SCI_SETSELBACK, true, qColorToSciRGB(QColor(0x40, 0x70, 0xC0)));

    // Highlight current line
    send(SCI_SETCARETLINEBACKALPHA, SC_ALPHA_NOALPHA); // off by default; user enables via style
    send(SCI_SETCARETLINEVISIBLE, false);

    // Edge (long line marker) — off by default
    send(SCI_SETEDGECOLUMN, 0);

    // Whitespace: off by default
    send(SCI_SETVIEWWS, SCWS_INVISIBLE);

    // Indentation guides: on
    send(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH);

    // Bracket matching
    send(SCI_SETBRACKETLIGHTLINE, qColorToSciRGB(QColor(0xFF, 0xFF, 0x00)));

    // Current position highlight
    send(SCI_SETSELFORE, true, 0);

    // Additional style defaults
    send(SCI_SETWRAPVISUALFLAGS, SC_WRAPVISUALFLAG_END);
    send(SCI_SETWRAPSTARTINDENT, 4);
    send(SCI_SETLAZYEND, true);
}

// =============================================================================
// Buffer management
// =============================================================================

void ScintillaComponent::attachDefaultDoc()
{
    // The ScintillaEditBase already has an empty document on construction.
    // We treat this as the default doc and wrap it as a Buffer.
    Buffer* defaultBuf = new Buffer();
    defaultBuf->setLangType(LangType::L_TEXT);
    _currentBuffer = defaultBuf;
    emit bufferActivated(defaultBuf);
}

void ScintillaComponent::activateBuffer(Buffer* buffer)
{
    if (!buffer || buffer == _currentBuffer)
        return;

    // Save current view state before switching
    saveCurrentPos();
    saveScrollState();
    saveSelectionState();

    //SCI_SETDOCPOINTER: sets the document that Scintilla displays.
    // This keeps the old document alive (we don't destroy it).
    // The new buffer's document will be set via bufferUpdated().

    Buffer* oldBuffer = _currentBuffer;
    _currentBuffer = buffer;

    // If buffer has content, load it
    if (!buffer->content().isEmpty()) {
        setText(buffer->content().toUtf8().constData());
    } else {
        // Clear with empty document
        send(SCI_CLEARALL);
    }

    // Apply buffer properties
    bufferUpdated(buffer, BufferChangeMask);

    // Restore view state for the new buffer (if previously visited)
    restoreScrollState();
    restoreSelectionState();

    emit bufferActivated(buffer);
}

void ScintillaComponent::bufferUpdated(Buffer* buffer, int changeMask)
{
    if (!buffer)
        return;

    // Apply language / lexer
    if (changeMask & 0x1) {
        // LangType changed
        if (buffer->getLangType() == LangType::L_TEXT)
            setLexerLanguage(L_TEXT);
        else
            styleChange();
    }

    // Apply read-only state
    if (changeMask & 0x2) {
        setReadOnly(buffer->isReadOnly());
    }

    // Apply encoding (codepage)
    if (changeMask & 0x4) {
        switch (static_cast<Encoding>(buffer->getEncoding())) {
            case Encoding::UTF8:
            case Encoding::UTF8_BOM:
                setCodepage(65001); break;
            case Encoding::UTF16_LE:
                setCodepage(1200); break;
            case Encoding::UTF16_BE:
                setCodepage(1201); break;
            default:
                setCodepage(0); break; // system ANSI
        }
    }

    // Apply EOL format
    if (changeMask & 0x8) {
        switch (buffer->getEolFormat()) {
            case EolFormat::Windows: setEolMode(SC_EOL_CRLF); break;
            case EolFormat::Unix:     setEolMode(SC_EOL_LF); break;
            case EolFormat::Mac:      setEolMode(SC_EOL_CR); break;
        }
    }
}

void ScintillaComponent::setCurrentBuffer(Buffer* buf)
{
    if (_currentBuffer != buf) {
        _currentBuffer = buf;
        emit bufferActivated(buf);
    }
}

// =============================================================================
// Text retrieval
// =============================================================================

void ScintillaComponent::getText(char* dest, size_t start, size_t end) const
{
    if (!dest || start >= end)
        return;

    struct Sci_TextRange {
        Sci_CharacterRangeFull chrg;
        char* lpstrText;
    };

    Sci_TextRange tr{};
    tr.chrg.cpMin = static_cast<Sci_Position>(start);
    tr.chrg.cpMax = static_cast<Sci_Position>(end);
    tr.lpstrText = dest;
    send(SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&tr));
}

QString ScintillaComponent::getTextAsString(size_t start, size_t end) const
{
    if (start >= end)
        return QString();

    size_t len = end - start;
    QByteArray bytes(len + 1, Qt::Uninitialized);
    getText(bytes.data(), start, end);
    return QString::fromUtf8(bytes.constData());
}

QString ScintillaComponent::getSelectedText() const
{
    size_t len = send(SCI_GETSELTEXT);
    if (len == 0)
        return QString();

    QByteArray bytes(static_cast<int>(len), Qt::Uninitialized);
    send(SCI_GETSELTEXT, 0, reinterpret_cast<sptr_t>(bytes.data()));
    return QString::fromUtf8(bytes.constData());
}

QString ScintillaComponent::getLine(size_t lineNumber) const
{
    int line = static_cast<int>(lineNumber);
    size_t len = send(SCI_GETLINE, line);
    if (len == 0)
        return QString();

    QByteArray bytes(static_cast<int>(len), Qt::Uninitialized);
    // SCI_GETLINE doesn't null-terminate, so we handle it
    bytes[len] = '\0';
    send(SCI_GETLINE, line, reinterpret_cast<sptr_t>(bytes.data()));
    return QString::fromUtf8(bytes.constData()).trimmed();
}

QString ScintillaComponent::getWordOnCaret() const
{
    size_t caretPos = send(SCI_GETCURRENTPOS);
    size_t wordStart = send(SCI_WORDSTARTPOSITION, caretPos, true);
    size_t wordEnd = send(SCI_WORDENDPOSITION, caretPos, true);
    if (wordEnd <= wordStart)
        return QString();

    return getTextAsString(wordStart, wordEnd);
}

// =============================================================================
// Text insertion / replacement
// =============================================================================

void ScintillaComponent::insertText(size_t position, const char* text) const
{
    if (!text)
        return;
    send(SCI_INSERTTEXT, static_cast<uptr_t>(position),
         reinterpret_cast<sptr_t>(text));
}

void ScintillaComponent::insertText(size_t position, const QString& text) const
{
    QByteArray utf8 = text.toUtf8();
    // SCI_INSERTTEXT expects "line:text\0"
    if (utf8.isEmpty()) {
        send(SCI_INSERTTEXT, static_cast<uptr_t>(position),
             reinterpret_cast<sptr_t>(""));
    } else {
        // Scintilla's SCI_INSERTTEXT format is "text\0"
        utf8.append('\0');
        send(SCI_INSERTTEXT, static_cast<uptr_t>(position),
             reinterpret_cast<sptr_t>(utf8.constData()));
    }
}

void ScintillaComponent::replaceSelWith(const char* replaceText) const
{
    send(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(replaceText ? replaceText : ""));
}

void ScintillaComponent::replaceSelWith(const QString& text) const
{
    replaceSelWith(text.toUtf8().constData());
}

void ScintillaComponent::addText(size_t length, const char* buf) const
{
    send(SCI_ADDTEXT, length, reinterpret_cast<sptr_t>(buf));
}

// =============================================================================
// Search / target operations
// =============================================================================

intptr_t ScintillaComponent::searchInTarget(const QString& text, size_t fromPos, size_t toPos) const
{
    QByteArray utf8 = text.toUtf8();
    return searchInTarget(utf8.constData(), static_cast<size_t>(utf8.size()), fromPos, toPos);
}

intptr_t ScintillaComponent::searchInTarget(const char* text, size_t len, size_t fromPos, size_t toPos) const
{
    send(SCI_SETTARGETSTART, fromPos);
    send(SCI_SETTARGETEND, toPos);
    return send(SCI_SEARCHINTARGET, len, reinterpret_cast<sptr_t>(text));
}

intptr_t ScintillaComponent::replaceTarget(const QString& replacement, intptr_t fromPos, intptr_t toPos)
{
    QByteArray utf8 = replacement.toUtf8();
    if (fromPos >= 0)
        send(SCI_SETTARGETSTART, static_cast<size_t>(fromPos));
    if (toPos >= 0)
        send(SCI_SETTARGETEND, static_cast<size_t>(toPos));
    return send(SCI_REPLACETARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
}

intptr_t ScintillaComponent::replaceTarget(const char* replacement, intptr_t fromPos, intptr_t toPos)
{
    size_t len = replacement ? strlen(replacement) : 0;
    if (fromPos >= 0)
        send(SCI_SETTARGETSTART, static_cast<size_t>(fromPos));
    if (toPos >= 0)
        send(SCI_SETTARGETEND, static_cast<size_t>(toPos));
    return send(SCI_REPLACETARGET, len, reinterpret_cast<sptr_t>(replacement));
}

// =============================================================================
// Display control
// =============================================================================

void ScintillaComponent::showMargin(int whichMargin, bool visible)
{
    int width = visible ? ((whichMargin == SC_MARGE_LINENUMBER) ? 50 : 16) : 0;
    send(SCI_SETMARGINWIDTHN, whichMargin, width);
}

bool ScintillaComponent::hasMarginShown(int whichMargin) const
{
    return send(SCI_GETMARGINWIDTHN, whichMargin) > 0;
}

void ScintillaComponent::showInvisibleChars(bool show)
{
    send(SCI_SETVIEWWS, show ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
}

bool ScintillaComponent::isShownInvisibleChars() const
{
    return send(SCI_GETVIEWWS) != SCWS_INVISIBLE;
}

void ScintillaComponent::showIndentGuide(bool show)
{
    send(SCI_SETINDENTATIONGUIDES, show ? SC_IV_LOOKBOTH : SC_IV_NONE);
}

bool ScintillaComponent::isShownIndentGuide() const
{
    return send(SCI_GETINDENTATIONGUIDES) != SC_IV_NONE;
}

void ScintillaComponent::wrapText(bool wrap)
{
    send(SCI_SETWRAPMODE, wrap ? SC_WRAP_WORD : SC_WRAP_NONE);
}

bool ScintillaComponent::isWrap() const
{
    return send(SCI_GETWRAPMODE) != SC_WRAP_NONE;
}

void ScintillaComponent::showWrapSymbol(bool show)
{
    send(SCI_SETWRAPVISUALFLAGS, show ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
}

bool ScintillaComponent::isWrapSymbolVisible() const
{
    return send(SCI_GETWRAPVISUALFLAGS) != SC_WRAPVISUALFLAG_NONE;
}

void ScintillaComponent::showEOL(bool show)
{
    send(SCI_SETVIEWEOL, show ? 1 : 0);
}

bool ScintillaComponent::isShownEOL() const
{
    return send(SCI_GETVIEWEOL) != 0;
}

// =============================================================================
// Bookmarks
// =============================================================================

void ScintillaComponent::marginClick(size_t position, int modifiers)
{
    int line = send(SCI_LINEFROMPOSITION, position);
    Q_UNUSED(modifiers);
    if (hasBookmark(static_cast<size_t>(line))) {
        removeBookmark(static_cast<size_t>(line));
    } else {
        addBookmark(static_cast<size_t>(line));
    }
}

void ScintillaComponent::addBookmark(size_t line)
{
    send(SCI_MARKERADD, line, MARK_BOOKMARK);
}

void ScintillaComponent::removeBookmark(size_t line)
{
    send(SCI_MARKERDELETE, line, MARK_BOOKMARK);
}

bool ScintillaComponent::hasBookmark(size_t line) const
{
    return send(SCI_MARKERGET, line) & (1 << MARK_BOOKMARK);
}

void ScintillaComponent::clearAllBookmarks()
{
    send(SCI_MARKERDELETEALL, MARK_BOOKMARK);
}

// =============================================================================
// Folding
// =============================================================================

void ScintillaComponent::fold(size_t line, bool collapse) const
{
    if (collapse) {
        send(SCI_HIDELINES, line, line); // keep the fold line itself
        // Actually collapse the fold
        send(SCI_SETFOLDEXPANDED, line, false);
    } else {
        send(SCI_SETFOLDEXPANDED, line, true);
        // Unhide lines in the expanded fold
        size_t lineMax = send(SCI_GETLASTCHILD, line, -1);
        if (lineMax > static_cast<size_t>(line))
            send(SCI_SHOWLINES, line + 1, lineMax);
    }
}

void ScintillaComponent::foldAll(bool collapse) const
{
    size_t lineCount = send(SCI_GETLINECOUNT);
    for (size_t line = 0; line < lineCount; ++line) {
        int level = send(SCI_GETFOLDLEVEL, line);
        if ((level & SC_FOLDLEVELHEADERFLAG) && (collapse || send(SCI_GETFOLDEXPANDED, line) == 0)) {
            fold(line, collapse);
        }
    }
}

void ScintillaComponent::expand(size_t& line, bool doExpand, bool force) const
{
    if (force || !send(SCI_GETFOLDEXPANDED, line)) {
        if (doExpand)
            send(SCI_SETFOLDEXPANDED, line, true);
        // Unhide children
        size_t lastChild = send(SCI_GETLASTCHILD, line, -1);
        if (lastChild != static_cast<size_t>(-1) && lastChild > line)
            send(SCI_SHOWLINES, line + 1, lastChild);
    }
}

bool ScintillaComponent::isFolded(size_t line) const
{
    return send(SCI_GETFOLDEXPANDED, line) != 0;
}

void ScintillaComponent::foldCurrentPos(bool collapse)
{
    size_t caretPos = send(SCI_GETCURRENTPOS);
    int line = send(SCI_LINEFROMPOSITION, caretPos);
    fold(static_cast<size_t>(line), collapse);
}

bool ScintillaComponent::isCurrentLineFolded() const
{
    size_t caretPos = send(SCI_GETCURRENTPOS);
    int line = send(SCI_LINEFROMPOSITION, caretPos);
    return isFolded(static_cast<size_t>(line));
}

// =============================================================================
// Scrolling / cursor
// =============================================================================

void ScintillaComponent::scrollTo(size_t line)
{
    send(SCI_ENSUREVISIBLEENFORCEPOLICY, line);
}

void ScintillaComponent::scrollToCenter(size_t position)
{
    int line = send(SCI_LINEFROMPOSITION, position);
    // Scroll so line is in the middle of the visible area
    int topLine = qMax(0, line - (send(SCI_LINESONSCREEN) / 2));
    send(SCI_SETFIRSTVISIBLELINE, topLine);
}

void ScintillaComponent::gotoLine(size_t line)
{
    send(SCI_GOTOLINE, line);
}

void ScintillaComponent::scroll(int columns, int lines)
{
    Q_UNUSED(columns);
    Q_UNUSED(lines);
    // In Qt, scrolling is handled by QAbstractScrollArea.
    // The Scintilla content scrolls within the scroll area.
}

intptr_t ScintillaComponent::currentLine() const
{
    return send(SCI_GETCURRENTLINE);
}

intptr_t ScintillaComponent::currentColumn() const
{
    size_t caret = send(SCI_GETCURRENTPOS);
    size_t line = send(SCI_LINEFROMPOSITION, caret);
    return static_cast<intptr_t>(caret - send(SCI_POSITIONFROMLINE, line));
}

// =============================================================================
// Selection
// =============================================================================

void ScintillaComponent::beginSelect()
{
    send(SCI_SELECTALL, 0); // Actually starts a rectangular selection
    _beginSelectPosition = send(SCI_GETCURRENTPOS);
}

void ScintillaComponent::endSelect()
{
    send(SCI_SELECTALL, 0);
    _beginSelectPosition = -1;
}

void ScintillaComponent::selectAll()
{
    send(SCI_SELECTALL);
}

bool ScintillaComponent::hasSelection() const
{
    return send(SCI_GETSELECTIONSTART) != send(SCI_GETSELECTIONEND);
}

void ScintillaComponent::setSelection(size_t start, size_t end)
{
    send(SCI_SETSELECTIONSTART, start);
    send(SCI_SETSELECTIONEND, end);
}

void ScintillaComponent::gotoPos(size_t pos)
{
    send(SCI_GOTOPOS, pos);
}

// =============================================================================
// Lexer / styling
// =============================================================================

void ScintillaComponent::defineDocType(int langType)
{
    setLexerLanguage(langType);
}

void ScintillaComponent::styleChange()
{
    // Re-apply lexer language based on current buffer's language string
    if (_currentBuffer && !_currentBuffer->language().isEmpty()) {
        // Map language name string to Scintilla lexer ID
        // This is done via the language-to-lexer table
        QString lang = _currentBuffer->language().toLower();
        // Default: null lexer (plain text)
        int lexer = SCLEX_NULL;

        if (lang == QStringLiteral("c") || lang == QStringLiteral("cpp") || lang == QStringLiteral("c++"))
            lexer = SCLEX_CPP;
        else if (lang == QStringLiteral("java"))
            lexer = SCLEX_CPP; // Java
        else if (lang == QStringLiteral("html") || lang == QStringLiteral("htm"))
            lexer = SCLEX_HTML;
        else if (lang == QStringLiteral("xml"))
            lexer = SCLEX_XML;
        else if (lang == QStringLiteral("python") || lang == QStringLiteral("py"))
            lexer = SCLEX_PYTHON;
        else if (lang == QStringLiteral("javascript") || lang == QStringLiteral("js") || lang == QStringLiteral("jscript"))
            lexer = SCLEX_CPP; // Scintilla uses cpp lexer for JS
        else if (lang == QStringLiteral("css"))
            lexer = SCLEX_CSS;
        else if (lang == QStringLiteral("sql"))
            lexer = SCLEX_SQL;
        else if (lang == QStringLiteral("php"))
            lexer = SCLEX_HTML; // PHP is embedded HTML
        else if (lang == QStringLiteral("json"))
            lexer = SCLEX_JSON;
        else if (lang == QStringLiteral("makefile") || lang == QStringLiteral("makefile"))
            lexer = SCLEX_MAKEFILE;
        else if (lang == QStringLiteral("bash") || lang == QStringLiteral("shell") || lang == QStringLiteral("sh"))
            lexer = SCLEX_BASH;
        else if (lang == QStringLiteral("powershell") || lang == QStringLiteral("ps1"))
            lexer = SCLEX_POWERSHELL;
        else if (lang == QStringLiteral("lua"))
            lexer = SCLEX_LUA;
        else if (lang == QStringLiteral("ruby") || lang == QStringLiteral("rb"))
            lexer = SCLEX_RUBY;
        else if (lang == QStringLiteral("perl") || lang == QStringLiteral("pl"))
            lexer = SCLEX_PERL;
        else if (lang == QStringLiteral("pascal") || lang == QStringLiteral("delphi"))
            lexer = SCLEX_PASCAL;
        else if (lang == QStringLiteral("ini") || lang == QStringLiteral("props") || lang == QStringLiteral("properties"))
            lexer = SCLEX_PROPERTIES;
        else if (lang == QStringLiteral("yaml") || lang == QStringLiteral("yml"))
            lexer = SCLEX_YAML;
        else if (lang == QStringLiteral("markdown") || lang == QStringLiteral("md"))
            lexer = SCLEX_MARKDOWN;
        else if (lang == QStringLiteral("latex") || lang == QStringLiteral("tex"))
            lexer = SCLEX_TEX;
        else if (lang == QStringLiteral("rust") || lang == QStringLiteral("rs"))
            lexer = SCLEX_RUST;
        else if (lang == QStringLiteral("go") || lang == QStringLiteral("golang"))
            lexer = SCLEX_CPP; // Go uses cpp-like lexer
        else if (lang == QStringLiteral("swift"))
            lexer = SCLEX_CPP; // Swift uses cpp-like lexer
        else if (lang == QStringLiteral("haskell") || lang == QStringLiteral("hs"))
            lexer = SCLEX_HASKELL;
        else if (lang == QStringLiteral("cmake"))
            lexer = SCLEX_CMAKE;
        else if (lang == QStringLiteral("r"))
            lexer = SCLEX_R;
        else if (lang == QStringLiteral("objective-c") || lang == QStringLiteral("objc") || lang == QStringLiteral("objectivec"))
            lexer = SCLEX_OBJECTIVEC;
        else if (lang == QStringLiteral("d"))
            lexer = SCLEX_D;
        else if (lang == QStringLiteral("diff") || lang == QStringLiteral("patch"))
            lexer = SCLEX_DIFF;
        else if (lang == QStringLiteral("batch") || lang == QStringLiteral("bat") || lang == QStringLiteral("cmd"))
            lexer = SCLEX_BATCH;
        else if (lang == QStringLiteral("nsis"))
            lexer = SCLEX_NSIS;
        else if (lang == QStringLiteral("tcl") || lang == QStringLiteral("tk"))
            lexer = SCLEX_TCL;
        else if (lang == QStringLiteral("matlab"))
            lexer = SCLEX_MATLAB;
        else if (lang == QStringLiteral("vhdl"))
            lexer = SCLEX_VHDL;
        else if (lang == QStringLiteral("verilog"))
            lexer = SCLEX_VERILOG;
        else if (lang == QStringLiteral("gui4cli"))
            lexer = SCLEX_GUI4CLI;
        else if (lang == QStringLiteral("asm") || lang == QStringLiteral("assembly") || lang == QStringLiteral("masm") || lang == QStringLiteral("nasm"))
            lexer = SCLEX_ASM;
        else if (lang == QStringLiteral("cobol"))
            lexer = SCLEX_COBOL;
        else if (lang == QStringLiteral("smalltalk"))
            lexer = SCLEX_SMALLTALK;
        else if (lang == QStringLiteral("autolt") || lang == QStringLiteral("au3"))
            lexer = SCLEX_AU3;
        else if (lang == QStringLiteral("caml") || lang == QStringLiteral("ocaml"))
            lexer = SCLEX_CAML;
        else if (lang == QStringLiteral("ada"))
            lexer = SCLEX_ADA;
        else if (lang == QStringLiteral("fortran") || lang == QStringLiteral("fortran77") || lang == QStringLiteral("f77"))
            lexer = SCLEX_FORTRAN;
        else if (lang == QStringLiteral("scheme") || lang == QStringLiteral("lisp"))
            lexer = SCLEX_LISP;
        else if (lang == QStringLiteral("erlang"))
            lexer = SCLEX_ERLANG;
        else if (lang == QStringLiteral("forth"))
            lexer = SCLEX_FORTH;
        else if (lang == QStringLiteral("escript"))
            lexer = SCLEX_ESCRIPT;
        else if (lang == QStringLiteral("rebol"))
            lexer = SCLEX_REBOL;
        else if (lang == QStringLiteral("registry"))
            lexer = SCLEX_REGISTRY;
        else if (lang == QStringLiteral("spice"))
            lexer = SCLEX_SPICE;
        else if (lang == QStringLiteral("txt2tags"))
            lexer = SCLEX_TXT2TAGS;
        else if (lang == QStringLiteral("visualprolog"))
            lexer = SCLEX_VISUALPROLOG;
        else if (lang == QStringLiteral("inno"))
            lexer = SCLEX_INNO;
        else if (lang == QStringLiteral("kix"))
            lexer = SCLEX_KIX;
        else if (lang == QStringLiteral("baan") || lang == QStringLiteral("baanc"))
            lexer = SCLEX_BAAN;
        else if (lang == QStringLiteral("srec"))
            lexer = SCLEX_SREC;
        else if (lang == QStringLiteral("ihex"))
            lexer = SCLEX_IHEX;
        else if (lang == QStringLiteral("tehex"))
            lexer = SCLEX_TEHEX;
        else if (lang == QStringLiteral("avsiynth") || lang == QStringLiteral("avs"))
            lexer = SCLEX_AVS;
        else if (lang == QStringLiteral("blitzbasic"))
            lexer = SCLEX_BLITZBASIC;
        else if (lang == QStringLiteral("purebasic"))
            lexer = SCLEX_PUREBASIC;
        else if (lang == QStringLiteral("freebasic"))
            lexer = SCLEX_FREEBASIC;
        else if (lang == QStringLiteral("csound"))
            lexer = SCLEX_CSOUND;
        else if (lang == QStringLiteral("oscript"))
            lexer = SCLEX_OSCRIPT;
        else if (lang == QStringLiteral("mmixal"))
            lexer = SCLEX_MMIXAL;
        else if (lang == QStringLiteral("nim") || lang == QStringLiteral("nimrod"))
            lexer = SCLEX_NIMROD;
        else if (lang == QStringLiteral("nncrontab"))
            lexer = SCLEX_NNCRONTAB;
        else if (lang == QStringLiteral("asn.1") || lang == QStringLiteral("asn1"))
            lexer = SCLEX_ASN1;
        else if (lang == QStringLiteral("gdscript") || lang == QStringLiteral("gd"))
            lexer = SCLEX_GDSCRIPT;
        else if (lang == QStringLiteral("hollywood"))
            lexer = SCLEX_HOLLYWOOD;
        else if (lang == QStringLiteral("toml"))
            lexer = SCLEX_TOML;
        else if (lang == QStringLiteral("sas"))
            lexer = SCLEX_SAS;
        else if (lang == QStringLiteral("mssql") || lang == QStringLiteral("tsql"))
            lexer = SCLEX_MSSQL;
        else if (lang == QStringLiteral("raku") || lang == QStringLiteral("perl6"))
            lexer = SCLEX_PERL; // Raku uses perl-like lexer
        else if (lang == QStringLiteral("errorlist"))
            lexer = SCLEX_ERRORLIST;
        else if (lang == QStringLiteral("json5"))
            lexer = SCLEX_JSON;
        else if (lang == QStringLiteral("coffeescript") || lang == QStringLiteral("coffee"))
            lexer = SCLEX_COFFEESCRIPT;
        else if (lang == QStringLiteral("typescript") || lang == QStringLiteral("ts"))
            lexer = SCLEX_CPP; // TypeScript uses cpp-like lexer
        else if (lang == QStringLiteral("jsp"))
            lexer = SCLEX_HTML; // JSP is embedded HTML
        else if (lang == QStringLiteral("vb") || lang == QStringLiteral("visualbasic"))
            lexer = SCLEX_VB;
        else if (lang == QStringLiteral("asp"))
            lexer = SCLEX_HTML; // ASP is embedded HTML
        else if (lang == QStringLiteral("nfo") || lang == QStringLiteral("asciiart") || lang == QStringLiteral("ascii"))
            lexer = SCLEX_NULL; // ASCII art — no highlighting

        send(SCI_SETLEXER, lexer);
    }
}

void ScintillaComponent::setLexerLanguage(int langType)
{
    // langType maps to the LangType enum values in the Win32 source.
    // We directly call setLexerLanguage using the lexer constant directly.
    // The langType from Notepad++ is an enum value; we convert it here.
    // For simplicity, this accepts the Scintilla lexer ID directly.
    send(SCI_SETLEXER, static_cast<uptr_t>(langType));
}

void ScintillaComponent::setKeywords(int keywordSet, const char* keywords)
{
    if (!keywords)
        return;
    send(SCI_SETKEYWORDS, keywordSet, reinterpret_cast<sptr_t>(keywords));
}

void ScintillaComponent::setStyle(const QColor& fore, const QColor& back,
                                   const QString& fontName, int fontSize, int fontStyle)
{
    send(SCI_STYLESETFORE, STYLE_DEFAULT, qColorToSciRGB(fore));
    send(SCI_STYLESETBACK, STYLE_DEFAULT, qColorToSciRGB(back));

    if (!fontName.isEmpty()) {
        send(SCI_STYLESETFONT, STYLE_DEFAULT,
             reinterpret_cast<sptr_t>(fontName.toUtf8().constData()));
    }
    if (fontSize > 0) {
        send(SCI_STYLESETSIZE, STYLE_DEFAULT, fontSize);
    }
    if (fontStyle >= 0) {
        send(SCI_STYLESETBOLD, STYLE_DEFAULT, (fontStyle & 1) ? 1 : 0);
        send(SCI_STYLESETITALIC, STYLE_DEFAULT, (fontStyle & 2) ? 1 : 0);
        send(SCI_STYLESETUNDERLINE, STYLE_DEFAULT, (fontStyle & 4) ? 1 : 0);
    }
    send(SCI_STYLECLEARALL);
}

// =============================================================================
// Indicators / highlighting
// =============================================================================

void ScintillaComponent::clearIndicator(int indicatorNumber)
{
    size_t length = send(SCI_GETLENGTH);
    send(SCI_SETINDICATORCURRENT, indicatorNumber);
    send(SCI_INDICATORCLEARRANGE, 0, length);
}

void ScintillaComponent::highlightAll(const QString& text)
{
    if (text.isEmpty())
        return;

    // Clear existing highlights first
    clearHighlightAll();

    QByteArray utf8 = text.toUtf8();
    size_t docLength = send(SCI_GETLENGTH);
    intptr_t pos = 0;

    while (pos >= 0 && pos < docLength) {
        pos = searchInTarget(utf8.constData(), static_cast<size_t>(utf8.size()),
                              static_cast<size_t>(pos), docLength);
        if (pos >= 0) {
            // Apply highlight indicator
            send(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE);
            size_t targetStart = send(SCI_GETTARGETSTART);
            size_t targetEnd = send(SCI_GETTARGETEND);
            send(SCI_INDICATORFILLRANGE, targetStart, targetEnd - targetStart);

            // Move past this match for next search
            pos = targetEnd;
        }
    }
}

void ScintillaComponent::clearHighlightAll()
{
    clearIndicator(SCE_UNIVERSAL_FOUND_STYLE);
    clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
    clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_INC);
}

// =============================================================================
// Position save / restore
// =============================================================================

void ScintillaComponent::saveCurrentPos()
{
    _savedCaret = send(SCI_GETCURRENTPOS);
    _savedAnchor = send(SCI_GETANCHOR);
    _savedFirstVisibleLine = send(SCI_GETFIRSTVISIBLELINE);
    _savedXOffset = send(SCI_GETXOFFSET);
    _savedSelMode = send(SCI_GETSELECTIONMODE);

    // Save scroll width
    _savedScrollWidth = send(SCI_GETSCROLLWIDTH);
}

void ScintillaComponent::restoreCurrentPos()
{
    // Restore selection mode
    if (_savedSelMode != SC_SEL_STREAM) {
        send(SCI_SETSELECTIONMODE, _savedSelMode);
    }

    // Restore position
    if (_savedAnchor != _savedCaret) {
        send(SCI_SETSELECTIONSTART, _savedAnchor);
        send(SCI_SETSELECTIONEND, _savedCaret);
    } else {
        send(SCI_GOTOPOS, _savedCaret);
    }

    // Restore scroll
    send(SCI_SETFIRSTVISIBLELINE, _savedFirstVisibleLine);
    send(SCI_SETXOFFSET, _savedXOffset);
    send(SCI_SETSCROLLWIDTH, _savedScrollWidth);
}

void ScintillaComponent::restoreScrollState()
{
    send(SCI_SETFIRSTVISIBLELINE, _savedFirstVisibleLine);
    send(SCI_SETXOFFSET, _savedXOffset);
}

void ScintillaComponent::restoreSelectionState()
{
    if (_savedAnchor != _savedCaret) {
        send(SCI_SETSELECTIONSTART, _savedAnchor);
        send(SCI_SETSELECTIONEND, _savedCaret);
    } else {
        send(SCI_GOTOPOS, _savedCaret);
    }
}

void ScintillaComponent::saveScrollState()
{
    _savedFirstVisibleLine = send(SCI_GETFIRSTVISIBLELINE);
    _savedXOffset = send(SCI_GETXOFFSET);
    _savedScrollWidth = send(SCI_GETSCROLLWIDTH);
}

void ScintillaComponent::saveSelectionState()
{
    _savedCaret = send(SCI_GETCURRENTPOS);
    _savedAnchor = send(SCI_GETANCHOR);
    _savedSelMode = send(SCI_GETSELECTIONMODE);
}

// =============================================================================
// Line number width
// =============================================================================

void ScintillaComponent::updateLineNumberWidth()
{
    // Dynamically compute width based on line count
    size_t lineCount = send(SCI_GETLINECOUNT);
    int nbDigits = 1;
    if (lineCount > 0) {
        nbDigits = static_cast<int>(qFloor(qLn(static_cast<double>(lineCount)) / qLn(10.0))) + 1;
    }
    setLineNumberWidth(nbDigits);
}

void ScintillaComponent::setLineNumberWidth(int width)
{
    if (width <= 0) {
        showMargin(SC_MARGE_LINENUMBER, false);
        return;
    }

    // Compute pixel width from character count
    // Use "8" as the widest digit in the editor font
    QFontMetrics fm(font());
    int charWidth = fm.horizontalAdvance(QStringLiteral("8"));
    int pixelWidth = charWidth * width + fm.horizontalAdvance(QStringLiteral(" "));
    send(SCI_SETMARGINWIDTHN, SC_MARGE_LINENUMBER, pixelWidth);
}

// =============================================================================
// Event handling — forward Scintilla notifications as Qt signals
// =============================================================================

bool ScintillaComponent::event(QEvent* event)
{
    return ScintillaEditBase::event(event);
}

void ScintillaComponent::notifyParent(const SCNotification*)
{
    // The actual notification handling is done through Qt signals
    // (SCNotification is forwarded by QsciScintilla as Qt signals).
    // This virtual override exists for API compatibility.
}

// =============================================================================
// ScintillaCtrls — factory for ScintillaComponent instances
// =============================================================================

ScintillaCtrls::ScintillaCtrls(QObject* parent)
    : QObject(parent)
{}

ScintillaCtrls::~ScintillaCtrls()
{
    destroyAll();
}

ScintillaComponent* ScintillaCtrls::createEditor(QWidget* parent)
{
    ScintillaComponent* editor = new ScintillaComponent(parent);
    editor->init(parent);
    _editors.append(editor);
    return editor;
}

ScintillaComponent* ScintillaCtrls::editorFromIndex(int index)
{
    if (index < 0 || index >= _editors.size())
        return nullptr;
    return _editors.at(index);
}

ScintillaComponent* ScintillaCtrls::editorFromWidget(QWidget* widget) const
{
    for (ScintillaComponent* editor : _editors) {
        if (editor == widget || editor->isAncestorOf(widget))
            return editor;
    }
    return nullptr;
}

void ScintillaCtrls::destroyAll()
{
    for (ScintillaComponent* editor : qAsConst(_editors)) {
        editor->destroy();
        delete editor;
    }
    _editors.clear();
}

void ScintillaCtrls::removeEditor(ScintillaComponent* editor)
{
    int idx = indexOf(editor);
    if (idx >= 0) {
        _editors.remove(idx);
        editor->destroy();
        delete editor;
    }
}

int ScintillaCtrls::indexOf(ScintillaComponent* editor) const
{
    return _editors.indexOf(editor);
}

void ScintillaCtrls::setDarkMode(bool enabled)
{
    for (ScintillaComponent* editor : qAsConst(_editors)) {
        Q_UNUSED(editor);
        // Apply dark mode colours to all editors
        // Individual editor implementations handle dark mode via styleChange()
    }
}

// =============================================================================
// Win32 compatibility stubs
// =============================================================================

void ScintillaComponent::setMakerStyle(int style)
{
    // Set the folder/collapse marker style.
    // Win32 mapped this to SCI_MARKERDEFINE for SC_MARGE_FOLDER markers.
    // Qt6: SCI_SETFOLDFLAGS handles general fold display; marker styles set individually.
    Q_UNUSED(style);
    // Folder styles (style values from Win32 nppparams):
    // 0 = arrows, 1 = circles, 2 = squares, 3 = circles simplified, 4 = boxes
    constexpr int FOLDER_FLAGS = SC_FOLDFLAG_LINEAFTER_CONTRACTED;
    send(SCI_SETFOLDFLAGS, FOLDER_FLAGS);
}

void ScintillaComponent::setWrapMode(int mode)
{
    // Win32: SC_WRAP_* constants (0=none, 1=char, 2=whitespace)
    // Qt6: maps directly to Scintilla SCI_SETWRAPMODE
    send(SCI_SETWRAPMODE, mode);
}

void ScintillaComponent::setBorderEdge(bool show)
{
    // Win32: WS_EX_CLIENTEDGE → QAbstractScrollArea frame
    if (show) {
        setFrameShape(QFrame::StyledPanel);
    } else {
        setFrameShape(QFrame::NoFrame);
    }
}

// =============================================================================
// _langNameInfoArray — mirrors Win32 ScintillaEditView::_langNameInfoArray
// Maps LangType enum values to language display names and Scintilla lexer IDs.
// =============================================================================

const LanguageNameInfo _langNameInfoArray[] = {
    // [0] L_TEXT
    {"normal", "Normal text", "Normal text file", LangType::L_TEXT, "null"},
    // [1] L_PHP
    {"php", "PHP", "PHP Hypertext Preprocessor file", LangType::L_PHP, "phpscript"},
    // [2] L_C
    {"c", "C", "C source file", LangType::L_C, "cpp"},
    // [3] L_CPP
    {"cpp", "C++", "C++ source file", LangType::L_CPP, "cpp"},
    // [4] L_CS
    {"cs", "C#", "C# source file", LangType::L_CS, "cpp"},
    // [5] L_OBJC
    {"objc", "Objective-C", "Objective-C source file", LangType::L_OBJC, "objc"},
    // [6] L_JAVA
    {"java", "Java", "Java source file", LangType::L_JAVA, "cpp"},
    // [7] L_RC
    {"rc", "RC", "Windows Resource file", LangType::L_RC, "cpp"},
    // [8] L_HTML
    {"html", "HTML", "Hyper Text Markup Language file", LangType::L_HTML, "hypertext"},
    // [9] L_XML
    {"xml", "XML", "eXtensible Markup Language file", LangType::L_XML, "xml"},
    // [10] L_MAKEFILE
    {"makefile", "Makefile", "Makefile", LangType::L_MAKEFILE, "makefile"},
    // [11] L_PASCAL
    {"pascal", "Pascal", "Pascal source file", LangType::L_PASCAL, "pascal"},
    // [12] L_BATCH
    {"batch", "Batch", "Batch file", LangType::L_BATCH, "batch"},
    // [13] L_INI
    {"ini", "ini", "MS ini file", LangType::L_INI, "props"},
    // [14] L_ASCII
    {"ascii", "ASCII Art", "ASCII Art file", LangType::L_ASCII, "null"},
    // [15] L_USER
    {"udf", "udf", "User Defined language file", LangType::L_USER, "user"},
    // [16] L_ASP
    {"asp", "ASP", "Active Server Pages script file", LangType::L_ASP, "hypertext"},
    // [17] L_SQL
    {"sql", "SQL", "Structured Query Language file", LangType::L_SQL, "sql"},
    // [18] L_VB
    {"vb", "Visual Basic", "Visual Basic file", LangType::L_VB, "vb"},
    // [19] L_JS_EMBEDDED
    {"javascript", "Embedded JS", "Embedded JavaScript", LangType::L_JS_EMBEDDED, "cpp"},
    // [20] L_CSS
    {"css", "CSS", "Cascade Style Sheets File", LangType::L_CSS, "css"},
    // [21] L_PERL
    {"perl", "Perl", "Perl source file", LangType::L_PERL, "perl"},
    // [22] L_PYTHON
    {"python", "Python", "Python file", LangType::L_PYTHON, "python"},
    // [23] L_LUA
    {"lua", "Lua", "Lua source File", LangType::L_LUA, "lua"},
    // [24] L_TEX
    {"tex", "TeX", "TeX file", LangType::L_TEX, "tex"},
    // [25] L_FORTRAN
    {"fortran", "Fortran free form", "Fortran free form source file", LangType::L_FORTRAN, "fortran"},
    // [26] L_BASH
    {"bash", "Shell", "Unix script file", LangType::L_BASH, "bash"},
    // [27] L_FLASH
    {"flash", "ActionScript", "Flash ActionScript file", LangType::L_FLASH, "cpp"},
    // [28] L_NSIS
    {"nsis", "NSIS", "Nullsoft Scriptable Install System script file", LangType::L_NSIS, "nsis"},
    // [29] L_TCL
    {"tcl", "TCL", "Tool Command Language file", LangType::L_TCL, "tcl"},
    // [30] L_LISP
    {"lisp", "Lisp", "List Processing language file", LangType::L_LISP, "lisp"},
    // [31] L_SCHEME
    {"scheme", "Scheme", "Scheme file", LangType::L_SCHEME, "lisp"},
    // [32] L_ASM
    {"asm", "Assembly", "Assembly language source file", LangType::L_ASM, "asm"},
    // [33] L_DIFF
    {"diff", "Diff", "Diff file", LangType::L_DIFF, "diff"},
    // [34] L_PROPS
    {"props", "Properties file", "Properties file", LangType::L_PROPS, "props"},
    // [35] L_PS
    {"postscript", "PostScript", "PostScript file", LangType::L_PS, "ps"},
    // [36] L_RUBY
    {"ruby", "Ruby", "Ruby file", LangType::L_RUBY, "ruby"},
    // [37] L_SMALLTALK
    {"smalltalk", "Smalltalk", "Smalltalk file", LangType::L_SMALLTALK, "smalltalk"},
    // [38] L_VHDL
    {"vhdl", "VHDL", "VHSIC Hardware Description Language file", LangType::L_VHDL, "vhdl"},
    // [39] L_KIX
    {"kix", "KiXtart", "KiXtart file", LangType::L_KIX, "kix"},
    // [40] L_AU3
    {"autoit", "AutoIt", "AutoIt", LangType::L_AU3, "au3"},
    // [41] L_CAML
    {"caml", "CAML", "Categorical Abstract Machine Language", LangType::L_CAML, "caml"},
    // [42] L_ADA
    {"ada", "Ada", "Ada file", LangType::L_ADA, "ada"},
    // [43] L_VERILOG
    {"verilog", "Verilog", "Verilog file", LangType::L_VERILOG, "verilog"},
    // [44] L_MATLAB
    {"matlab", "MATLAB", "MATrix LABoratory", LangType::L_MATLAB, "matlab"},
    // [45] L_HASKELL
    {"haskell", "Haskell", "Haskell", LangType::L_HASKELL, "haskell"},
    // [46] L_INNO
    {"inno", "Inno Setup", "Inno Setup script", LangType::L_INNO, "inno"},
    // [47] L_SEARCHRESULT
    {"searchResult", "Internal Search", "Internal Search", LangType::L_SEARCHRESULT, "searchResult"},
    // [48] L_CMAKE
    {"cmake", "CMake", "CMake file", LangType::L_CMAKE, "cmake"},
    // [49] L_YAML
    {"yaml", "YAML", "YAML Ain't Markup Language", LangType::L_YAML, "yaml"},
    // [50] L_COBOL
    {"cobol", "COBOL", "COmmon Business Oriented Language", LangType::L_COBOL, "COBOL"},
    // [51] L_GUI4CLI
    {"gui4cli", "Gui4Cli", "Gui4Cli file", LangType::L_GUI4CLI, "gui4cli"},
    // [52] L_D
    {"d", "D", "D programming language", LangType::L_D, "d"},
    // [53] L_POWERSHELL
    {"powershell", "PowerShell", "Windows PowerShell", LangType::L_POWERSHELL, "powershell"},
    // [54] L_R
    {"r", "R", "R programming language", LangType::L_R, "r"},
    // [55] L_JSP
    {"jsp", "JSP", "JavaServer Pages script file", LangType::L_JSP, "hypertext"},
    // [56] L_COFFEESCRIPT
    {"coffeescript", "CoffeeScript", "CoffeeScript file", LangType::L_COFFEESCRIPT, "coffeescript"},
    // [57] L_JSON
    {"json", "JSON", "JSON file", LangType::L_JSON, "json"},
    // [58] L_JAVASCRIPT
    {"javascript.js", "JavaScript", "JavaScript file", LangType::L_JAVASCRIPT, "cpp"},
    // [59] L_FORTRAN_77
    {"fortran77", "Fortran fixed form", "Fortran fixed form source file", LangType::L_FORTRAN_77, "f77"},
    // [60] L_BAANC
    {"baanc", "BaanC", "BaanC File", LangType::L_BAANC, "baan"},
    // [61] L_SREC
    {"srec", "S-Record", "Motorola S-Record binary data", LangType::L_SREC, "srec"},
    // [62] L_IHEX
    {"ihex", "Intel HEX", "Intel HEX binary data", LangType::L_IHEX, "ihex"},
    // [63] L_TEHEX
    {"tehex", "Tektronix HEX", "Tektronix extended HEX binary data", LangType::L_TEHEX, "tehex"},
    // [64] L_SWIFT
    {"swift", "Swift", "Swift file", LangType::L_SWIFT, "cpp"},
    // [65] L_ASN1
    {"asn1", "ASN.1", "Abstract Syntax Notation One file", LangType::L_ASN1, "asn1"},
    // [66] L_AVS
    {"avs", "AviSynth", "AviSynth scripts files", LangType::L_AVS, "avs"},
    // [67] L_BLITZBASIC
    {"blitzbasic", "BlitzBasic", "BlitzBasic file", LangType::L_BLITZBASIC, "blitzbasic"},
    // [68] L_PUREBASIC
    {"purebasic", "PureBasic", "PureBasic file", LangType::L_PUREBASIC, "purebasic"},
    // [69] L_FREEBASIC
    {"freebasic", "FreeBasic", "FreeBasic file", LangType::L_FREEBASIC, "freebasic"},
    // [70] L_CSOUND
    {"csound", "Csound", "Csound file", LangType::L_CSOUND, "csound"},
    // [71] L_ERLANG
    {"erlang", "Erlang", "Erlang file", LangType::L_ERLANG, "erlang"},
    // [72] L_ESCRIPT
    {"escript", "ESCRIPT", "ESCRIPT file", LangType::L_ESCRIPT, "escript"},
    // [73] L_FORTH
    {"forth", "Forth", "Forth file", LangType::L_FORTH, "forth"},
    // [74] L_LATEX
    {"latex", "LaTeX", "LaTeX file", LangType::L_LATEX, "latex"},
    // [75] L_MMIXAL
    {"mmixal", "MMIXAL", "MMIXAL file", LangType::L_MMIXAL, "mmixal"},
    // [76] L_NIM
    {"nim", "Nim", "Nim file", LangType::L_NIM, "nimrod"},
    // [77] L_NNCRONTAB
    {"nncrontab", "Nncrontab", "extended crontab file", LangType::L_NNCRONTAB, "nncrontab"},
    // [78] L_OSCRIPT
    {"oscript", "OScript", "OScript source file", LangType::L_OSCRIPT, "oscript"},
    // [79] L_REBOL
    {"rebol", "REBOL", "REBOL file", LangType::L_REBOL, "rebol"},
    // [80] L_REGISTRY
    {"registry", "registry", "registry file", LangType::L_REGISTRY, "registry"},
    // [81] L_RUST
    {"rust", "Rust", "Rust file", LangType::L_RUST, "rust"},
    // [82] L_SPICE
    {"spice", "Spice", "spice file", LangType::L_SPICE, "spice"},
    // [83] L_TXT2TAGS
    {"txt2tags", "txt2tags", "txt2tags file", LangType::L_TXT2TAGS, "txt2tags"},
    // [84] L_VISUALPROLOG
    {"visualprolog", "Visual Prolog", "Visual Prolog file", LangType::L_VISUALPROLOG, "visualprolog"},
    // [85] L_TYPESCRIPT
    {"typescript", "TypeScript", "TypeScript file", LangType::L_TYPESCRIPT, "cpp"},
    // [86] L_JSON5
    {"json5", "JSON5", "JSON5 file", LangType::L_JSON5, "json"},
    // [87] L_MSSQL
    {"mssql", "mssql", "Microsoft Transact-SQL (SQL Server) file", LangType::L_MSSQL, "mssql"},
    // [88] L_GDSCRIPT
    {"gdscript", "GDScript", "GDScript file", LangType::L_GDSCRIPT, "gdscript"},
    // [89] L_HOLLYWOOD
    {"hollywood", "Hollywood", "Hollywood script", LangType::L_HOLLYWOOD, "hollywood"},
    // [90] L_GOLANG
    {"go", "Go", "Go source file", LangType::L_GOLANG, "cpp"},
    // [91] L_RAKU
    {"raku", "Raku", "Raku source file", LangType::L_RAKU, "raku"},
    // [92] L_TOML
    {"toml", "TOML", "Tom's Obvious Minimal Language file", LangType::L_TOML, "toml"},
    // [93] L_SAS
    {"sas", "SAS", "SAS file", LangType::L_SAS, "sas"},
    // [94] L_ERRORLIST
    {"errorlist", "ErrorList", "ErrorList", LangType::L_ERRORLIST, "errorlist"},
    // [95] L_EXTERNAL
    {"external", "External", "External", LangType::L_EXTERNAL, "null"},
};
