// NppCommands.cpp — Menu command handlers for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
//
// Semantic lift: Win32 menu command handlers → Qt6 slot methods.
// Each handler delegates to Application (formerly Notepad_plus).
//
// Win32 → Qt translation:
//   Menu command IDs (IDM_*)            → handler method names
//   SendMessage(WM_COMMAND, id, 0)     → command(id) → handler method
//   Win32 file dialogs (GetOpenFileName) → QFileDialog
//   Win32 clipboard (GetClipboardData)  → QApplication::clipboard()
//   Win32 ShellExecute                  → QDesktopServices::openUrl
//
// Command dispatch mirrors Notepad++ menu structure:
//   40001–40049  File
//   40110–40199  Edit
//   40200–40299  Search
//   40300–40399  View
//   40500–40599  Encoding
//   40600–40699  EOL
//   41000–41099  Bookmark
//   42000–42099  Language
//   43000–43099  Macro
//   44000–44099  Run

#include "NppCommands.h"
#include "Application.h"
#include "core/FileManager.h"
#include "dialogs/GoToLineDialog.h"
#include "ui/MainWindow.h"
#include "editor/ScintillaEditor.h"
#include "editor/ScintillaEditor.h"
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>

NppCommands::NppCommands(Application* app, QObject* parent)
    : QObject(parent), m_app(app)
{}

// ============================================================================
// Dispatch
// ============================================================================

void NppCommands::command(int id) {
    Q_EMIT commandExecuted(id);
    switch (id) {

    // ── File commands (40001–40049) ────────────────────────────────────────
    case 40001: newFile(); break;
    case 40002: openFile(); break;
    case 40003: closeFile(); break;
    case 40004: save(); break;
    case 40005: saveAs(); break;
    case 40006: saveAll(); break;
    case 40007: reload(); break;
    case 40008: /* deleteFile — not implemented */ break;
    case 40009: /* renameFile — not implemented */ break;
    case 40010: print(); break;
    case 40015: closeAll(); break;
    case 40016: closeAllButCurrent(); break;
    case 40019: loadSession(); break;
    case 40020: saveSession(); break;

    // ── Edit commands (40110–40199) ─────────────────────────────────────────
    case 40110: undo(); break;
    case 40111: redo(); break;
    case 40120: cut(); break;
    case 40121: copy(); break;
    case 40122: paste(); break;
    case 40123: /* delete — handled by editor */ break;
    case 40130: selectAll(); break;
    case 40140: deleteLine(); break;
    case 40150: joinLines(); break;
    case 40160: trimTrailing(); break;
    case 40161: trimLeading(); break;
    case 40180: sortLinesLocaleAsc(); break;
    case 40181: sortLinesLocaleDesc(); break;
    case 40184: sortLinesLengthAsc(); break;
    case 40185: sortLinesLengthDesc(); break;
    case 40186: removeDuplicateLines(); break;
    case 40190: openContainingFolder(); break;
    case 40192: searchOnInternet(); break;

    // ── Bookmark commands (41000–41099) ────────────────────────────────────
    case 41000: toggleBookmark(); break;
    case 41001: nextBookmark(); break;
    case 41002: prevBookmark(); break;
    case 41003: clearBookmarks(); break;

    // ── Search commands (40200–40299) ──────────────────────────────────────
    case 40200: showFind(); break;
    case 40201: showReplace(); break;
    case 40202: findNext(); break;
    case 40203: findPrevious(); break;
    case 40210: goToLine(); break;
    case 40220: markAll({}); break;
    case 40221: unmarkAll(); break;
    case 40230: findInFiles({}, {}); break;

    // ── View commands (40300–40399) ─────────────────────────────────────────
    case 40300: zoomIn(); break;
    case 40301: zoomOut(); break;
    case 40302: zoomRestore(); break;
    case 40310: toggleWordWrap(); break;
    case 40320: toggleShowAllChars(); break;
    case 40321: toggleEolVisibility(); break;
    case 40350: toggleDocumentMap(); break;
    case 40351: toggleFunctionList(); break;
    case 40352: toggleFileBrowser(); break;
    case 40353: toggleFullscreen(); break;
    case 40360: switchToMainView(); break;
    case 40361: switchToSubView(); break;
    case 40362: cloneToOtherView(); break;
    case 40363: moveToOtherView(); break;

    // ── Encoding commands (40500–40599) ─────────────────────────────────────
    case 40500: convertToAnsi(); break;
    case 40501: convertToUtf8(); break;
    case 40502: convertToUtf8Bom(); break;
    case 40503: convertToUtf16Le(); break;
    case 40504: convertToUtf16Be(); break;
    case 40505: convertToUtf32(); break;
    case 40506: convertToUtf32Be(); break;
    case 40507: convertToWindows1252(); break;
    case 40508: convertToIso88591(); break;

    // ── EOL commands (40600–40699) ───────────────────────────────────────
    case 40600: convertToWindowsEol(); break;
    case 40601: convertToUnixEol(); break;
    case 40602: convertToMacEol(); break;

    // ── Selection mode commands (40700–40799) ─────────────────────────────
    case 40700: enableColumnMode(); break;
    case 40701: cutColumn(); break;
    case 40702: copyColumn(); break;
    case 40703: pasteColumn(); break;

    // ── Macro commands (43000–43099) ───────────────────────────────────────
    case 43000: startMacroRecording(); break;
    case 43001: stopMacroRecording(); break;
    case 43002: playMacro(); break;
    case 43003: saveMacro(); break;

    // ── Settings commands (42000–42099) ────────────────────────────────────
    case 42000: showPreferences(); break;
    case 42001: showShortcutMapper(); break;
    case 42010: toggleToolbar(); break;
    case 42011: toggleStatusBar(); break;

    // ── Window commands (43000 range repurposed) ────────────────────────────
    case 43010: minimizeWindow(); break;
    case 43011: maximizeWindow(); break;
    case 43012: toggleAlwaysOnTop(); break;
    case 43020: switchToNextDocument(); break;
    case 43021: switchToPrevDocument(); break;

    // ── Run commands (44000–44099) ─────────────────────────────────────────
    case 44001: showRunDialog(); break;

    default:
        qDebug() << "[NppCommands] Unknown command:" << id;
        break;
    }
}

// ============================================================================
// File commands
// ============================================================================

/**
 * Create a new untitled document.
 * Win32: IDM_FILE_NEW → Notepad_plus::fileNew()
 */
void NppCommands::newFile() {
    if (m_app) m_app->newFile();
}

/**
 * Open file via native file dialog.
 * Win32: IDM_FILE_OPEN → GetOpenFileName() → Notepad_plus::doOpen()
 */
void NppCommands::openFile() {
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Open File"));
    if (!path.isEmpty() && m_app)
        m_app->openFile(path.toStdString());
}

/**
 * Save current buffer to its existing path.
 * Win32: IDM_FILE_SAVE → Notepad_plus::saveCurrentFile()
 */
void NppCommands::save() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) m_app->saveFile(id);
}

/**
 * Save current buffer to a new path.
 * Win32: IDM_FILE_SAVEAS → GetSaveFileName() → Notepad_plus::saveCurrentFileAs()
 */
void NppCommands::saveAs() {
    if (!m_app) return;
    QString path = QFileDialog::getSaveFileName(nullptr, tr("Save As"));
    if (!path.isEmpty()) {
        BufferID id = m_app->getActiveBuffer();
        if (id) m_app->saveFile(id, path.toStdString());
    }
}

/**
 * Save all modified buffers.
 * Win32: IDM_FILE_SAVEALL
 */
void NppCommands::saveAll() {
    if (m_app) m_app->saveAllFiles();
}

/**
 * Reload current file from disk, discarding unsaved changes.
 * Win32: IDM_FILE_RELOAD → reloadCurrentFile()
 */
void NppCommands::reload() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) {
        QString path = FileManager::instance().getBufferPath(id);
        if (!path.isEmpty())
            m_app->openFile(path.toStdString());
    }
}

/**
 * Close current buffer.
 * Win32: IDM_FILE_CLOSE → fileClose()
 */
void NppCommands::closeFile() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) m_app->closeFile(id, false);
}

/**
 * Close all open buffers.
 * Win32: IDM_FILE_CLOSEALL → fileCloseAll()
 */
void NppCommands::closeAll() {
    if (m_app) m_app->closeAllFiles();
}

/**
 * Close all buffers except the active one.
 * Win32: IDM_FILE_CLOSEALL_BUT_CURRENT
 */
void NppCommands::closeAllButCurrent() {
    if (!m_app) return;
    BufferID current = m_app->getActiveBuffer();
    m_app->closeAllBuffersExcept(current);
}

/**
 * Print current document.
 * Win32: IDM_FILE_PRINT (uses Scintilla printing)
 * Qt: QPrintSupport dialog → Scintilla print
 */
void NppCommands::print() {
    // Delegate to Application which manages the print dialog
    if (m_app) m_app->onMenuCommand("print");
}

// ============================================================================
// Edit commands
// ============================================================================

void NppCommands::undo()   { if (m_app) m_app->onUndo(); }
void NppCommands::redo()   { if (m_app) m_app->onRedo(); }
void NppCommands::cut()    { if (m_app) m_app->onCut(); }
void NppCommands::copy()    { if (m_app) m_app->onCopy(); }
void NppCommands::paste()   { if (m_app) m_app->onPaste(); }
void NppCommands::selectAll() { if (m_app) m_app->onSelectAll(); }
void NppCommands::deleteLine() { if (m_app) m_app->onMenuCommand("deleteLine"); }
void NppCommands::joinLines()  { if (m_app) m_app->onMenuCommand("joinLines"); }
void NppCommands::trimTrailing() { if (m_app) m_app->onMenuCommand("trimTrailing"); }
void NppCommands::trimLeading()  { if (m_app) m_app->onMenuCommand("trimLeading"); }
void NppCommands::sortLinesLengthAsc()  { if (m_app) m_app->onMenuCommand("sortLengthAsc"); }
void NppCommands::sortLinesLengthDesc() { if (m_app) m_app->onMenuCommand("sortLengthDesc"); }
void NppCommands::sortLinesLocaleAsc()  { if (m_app) m_app->onMenuCommand("sortAsc"); }
void NppCommands::sortLinesLocaleDesc() { if (m_app) m_app->onMenuCommand("sortDesc"); }

/**
 * Remove duplicate consecutive lines from selection.
 * Win32: IDM_EDIT_REMOVE_DUPLICATES
 */
void NppCommands::removeDuplicateLines() {
    if (m_app) m_app->onMenuCommand("removeDuplicateLines");
}

// ============================================================================
// Bookmark commands
// ============================================================================

/**
 * Toggle bookmark on the current line.
 * Win32: IDM_BOOKMARK_TOGGLE → SciMarkerHandle or view->currentMarkerToggle()
 * Qt: ScintillaComponent marker API (SC_MARKERADD, SC_MARKERDELETE, SC_MARKERGET)
 */
void NppCommands::toggleBookmark() {
    if (m_app) m_app->onMenuCommand("toggleBookmark");
}

/**
 * Jump to the next bookmarked line.
 * Win32: IDM_BOOKMARK_NEXT → markerNext()
 */
void NppCommands::nextBookmark() {
    if (m_app) m_app->onMenuCommand("nextBookmark");
}

/**
 * Jump to the previous bookmarked line.
 * Win32: IDM_BOOKMARK_PREV → markerPrevious()
 */
void NppCommands::prevBookmark() {
    if (m_app) m_app->onMenuCommand("prevBookmark");
}

/**
 * Clear all bookmarks in the current document.
 * Win32: IDM_BOOKMARK_CLEAR_ALL → markerDeleteAll(MARKER_BOOKMARK)
 */
void NppCommands::clearBookmarks() {
    if (m_app) m_app->onMenuCommand("clearBookmarks");
}

// ============================================================================
// Search commands
// ============================================================================

void NppCommands::showFind()    { if (m_app) m_app->onFind(); }
void NppCommands::showReplace() { if (m_app) m_app->onReplace(); }
void NppCommands::findNext()    { if (m_app) m_app->onFindNext(); }
void NppCommands::findPrevious(){ if (m_app) m_app->onFindPrev(); }

/**
 * Show Go To Line dialog with current cursor position.
 * Win32: IDM_SEARCH_GOTOLINE → GoToDialog
 * Qt: GoToLineDialog wrapping ScintillaEditor
 */
void NppCommands::goToLine() {
    if (!m_app) return;
    ScintillaEditor* editor = m_app->getActiveEditor();
    if (!editor) return;

    GoToLineDialog dlg(static_cast<QWidget*>(m_app->mainWindow()));
    if (editor) {
        dlg.setMaxLineNumber(editor->lineCount());
    }
    dlg.exec();
}

/**
 * Highlight all occurrences of text as indicators.
 * Win32: IDM_SEARCH_MARKALL → indicatorFillRange
 * @param text  Text to mark (empty = use last find pattern)
 * @param styleId  Indicator style (0 = default find match)
 */
void NppCommands::markAll(const QString& text, int styleId) {
    Q_UNUSED(text); Q_UNUSED(styleId);
    if (m_app) m_app->onMarkAll();
}

/**
 * Remove all search mark indicators.
 * Win32: IDM_SEARCH_UNMARKALL → indicatorClearRange
 * @param styleId  Which indicator style to clear (0 = all)
 */
void NppCommands::unmarkAll(int styleId) {
    Q_UNUSED(styleId);
    if (m_app) m_app->onMenuCommand("unmarkAll");
}

/**
 * Show Find in Files dialog.
 * Win32: IDM_SEARCH_FINDFILES → FindInFilesDlg
 */
void NppCommands::findInFiles(const QString& dir, const QString& pattern) {
    Q_UNUSED(dir); Q_UNUSED(pattern);
    if (m_app) m_app->onFindInFiles();
}

// ============================================================================
// View commands
// ============================================================================

void NppCommands::zoomIn()          { if (m_app) m_app->onMenuCommand("zoomIn"); }
void NppCommands::zoomOut()         { if (m_app) m_app->onMenuCommand("zoomOut"); }
void NppCommands::zoomRestore()     { if (m_app) m_app->onMenuCommand("zoomRestore"); }
void NppCommands::toggleWordWrap()  { if (m_app) m_app->onMenuCommand("toggleWordWrap"); }

/**
 * Toggle EOL symbol visibility (show/hide CRLF, LF, CR glyphs).
 * Win32: IDM_VIEW_EOL → SCI_SETVEOLDISPLAY
 */
void NppCommands::toggleEolVisibility() {
    if (m_app) m_app->onMenuCommand("toggleEol");
}

/**
 * Toggle all characters visibility (spaces, tabs, line ends).
 * Win32: IDM_VIEW_ALLCHAR → SCI_SETVIEWWS
 */
void NppCommands::toggleShowAllChars() {
    if (m_app) m_app->onMenuCommand("showAllChars");
}

void NppCommands::toggleDocumentMap()   { if (m_app) m_app->onMenuCommand("toggleDocMap"); }
void NppCommands::toggleFunctionList() { if (m_app) m_app->onMenuCommand("toggleFuncList"); }
void NppCommands::toggleFileBrowser()  { if (m_app) m_app->onMenuCommand("toggleFileBrowser"); }

/**
 * Toggle fullscreen mode (no title bar, no menu bar).
 * Win32: IDM_VIEW_FULLSCREEN → toggleFullScreen()
 * Qt: QWindow::setWindowState(Qt::WindowFullScreen)
 */
void NppCommands::toggleFullscreen() {
    if (m_app) m_app->toggleFullScreen();
}

void NppCommands::switchToMainView() { if (m_app) m_app->switchToView(0); }
void NppCommands::switchToSubView()  { if (m_app) m_app->switchToView(1); }
void NppCommands::cloneToOtherView(){ if (m_app) m_app->onMenuCommand("cloneToOtherView"); }
void NppCommands::moveToOtherView() { if (m_app) m_app->onMenuCommand("moveToOtherView"); }

// ============================================================================
// Column / Box selection mode commands
// ============================================================================

/**
 * Enable column (box) selection mode.
 * Win32: IDM_EDIT_COLUMNMODE → SCI_SETSELECTIONMODE(SC_SEL_RECTANGLE)
 * Qt: ScintillaComponent setSelectionMode(Scintilla::SelectionRectangle)
 */
void NppCommands::enableColumnMode() {
    if (m_app) m_app->onMenuCommand("columnMode");
}

/**
 * Cut selected columns to clipboard.
 * Win32: IDM_EDIT_CUT_COLUMN
 * Qt: Scintilla cut with column selection active
 */
void NppCommands::cutColumn() {
    if (m_app) m_app->onMenuCommand("cutColumn");
}

/**
 * Copy selected columns to clipboard.
 * Win32: IDM_EDIT_COPY_COLUMN
 * Qt: Scintilla copy with column selection active
 */
void NppCommands::copyColumn() {
    if (m_app) m_app->onMenuCommand("copyColumn");
}

/**
 * Paste clipboard contents into column selection.
 * Win32: IDM_EDIT_PASTE_COLUMN
 * Qt: Scintilla paste with column selection active
 */
void NppCommands::pasteColumn() {
    if (m_app) m_app->onMenuCommand("pasteColumn");
}

// ============================================================================
// Encoding commands
// ============================================================================

/**
 * Convert current buffer to ANSI (system locale / ISO-8859-1).
 * Win32: IDM_ENCODING_ANSI → iso_8859_1 → documentSetEncoding(CP_ACP)
 * Qt: QStringConverter::System (locale-dependent) or Latin1
 */
void NppCommands::convertToAnsi() {
    if (m_app) m_app->onConvertEncoding(EncodingType::ANSI);
}

/**
 * Convert current buffer to UTF-8 without BOM.
 * Win32: IDM_ENCODING_UTF8 → documentSetEncoding(CP_UTF8)
 * Qt: QStringConverter::Utf8 (no BOM prepended)
 */
void NppCommands::convertToUtf8() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_8);
}

/**
 * Convert current buffer to UTF-8 with BOM (0xEF 0xBB 0xBF).
 * Win32: IDM_ENCODING_UTF8_BOM
 * Qt: QStringConverter::Utf8 + BOM prepended manually
 */
void NppCommands::convertToUtf8Bom() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_8_BOM);
}

/**
 * Convert current buffer to UTF-16 LE (native endianness with BOM).
 * Win32: IDM_ENCODING_UTF16 → documentSetEncoding(CP_UTF16LE)
 * Qt: QStringConverter::Utf16 LE
 */
void NppCommands::convertToUtf16Le() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_16_LE);
}

/**
 * Convert current buffer to UTF-16 BE (byte-reversed with BOM 0xFE 0xFF).
 * Win32: IDM_ENCODING_UTF16BE → documentSetEncoding(CP_UTF16BE)
 * Qt: QStringConverter::Utf16 BE
 */
void NppCommands::convertToUtf16Be() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_16_BE);
}

/**
 * Convert current buffer to UTF-32 LE.
 * Win32: IDM_ENCODING_UTF32_LE
 * Qt: QStringConverter::Utf32 LE
 */
void NppCommands::convertToUtf32() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_32_LE);
}

/**
 * Convert current buffer to UTF-32 BE.
 * Win32: IDM_ENCODING_UTF32_BE
 * Qt: QStringConverter::Utf32 BE
 */
void NppCommands::convertToUtf32Be() {
    if (m_app) m_app->onConvertEncoding(EncodingType::UTF_32_BE);
}

/**
 * Convert to Windows-1252 (Western European, code page 1252).
 * Win32: Not natively supported; achieved via codepage conversion
 * Qt: ISO-8859-1 with Windows-1252 superset (0x80-0x9F mapped to special chars)
 */
void NppCommands::convertToWindows1252() {
    // Windows-1252 is not directly supported by QStringConverter.
    // Map to ISO-8859-1 with the understanding that bytes 0x80-0x9F
    // will be rendered differently. True W1252 support requires QTextCodec.
    if (m_app) m_app->onMenuCommand("encoding:windows-1252");
}

/**
 * Convert to ISO-8859-1 (Latin-1, 8-bit Western European).
 * Win32: IDM_ENCODING_ANSI with codepage 28591
 * Qt: QStringConverter::Latin1
 */
void NppCommands::convertToIso88591() {
    if (m_app) m_app->onConvertEncoding(EncodingType::ANSI);
}

// ============================================================================
// EOL commands
// ============================================================================

/**
 * Convert line endings to Windows CRLF (0x0D 0x0A).
 * Win32: IDM_EOL_CRLF → Scintilla::convertEOL(SC_EOL_CRLF)
 * Qt: EolType::EOL_CRLF
 */
void NppCommands::convertToWindowsEol() {
    if (m_app) m_app->onMenuCommand("eol:CRLF");
}

/**
 * Convert line endings to Unix LF (0x0A).
 * Win32: IDM_EOL_LF → Scintilla::convertEOL(SC_EOL_LF)
 * Qt: EolType::EOL_LF
 */
void NppCommands::convertToUnixEol() {
    if (m_app) m_app->onMenuCommand("eol:LF");
}

/**
 * Convert line endings to classic Mac CR (0x0D).
 * Win32: IDM_EOL_CR → Scintilla::convertEOL(SC_EOL_CR)
 * Qt: EolType::EOL_CR
 */
void NppCommands::convertToMacEol() {
    if (m_app) m_app->onMenuCommand("eol:CR");
}

// ============================================================================
// Macro commands
// ============================================================================

/**
 * Start recording macro actions.
 * Win32: IDM_MACRO_RECORD → macroRecordBegin()
 * Qt: MacroManager::startRecording() + Scintilla recording
 */
void NppCommands::startMacroRecording() {
    if (m_app) m_app->onMenuCommand("macroRecord");
}

/**
 * Stop recording macro actions.
 * Win32: IDM_MACRO_STOP → macroRecordEnd()
 * Qt: MacroManager::stopRecording()
 */
void NppCommands::stopMacroRecording() {
    if (m_app) m_app->onMenuCommand("macroStop");
}

/**
 * Play back the last recorded macro.
 * Win32: IDM_MACRO_PLAYBACK → macroPlay()
 * Qt: MacroManager::playback()
 */
void NppCommands::playMacro() {
    if (m_app) m_app->onMenuCommand("macroPlayback");
}

/**
 * Save the last recorded macro with a name.
 * Win32: IDM_MACRO_SAVE → macroSave()
 * Qt: MacroManager::saveMacro() → prompts for name
 */
void NppCommands::saveMacro() {
    if (m_app) m_app->onMenuCommand("macroSave");
}

// ============================================================================
// Settings / Preferences commands
// ============================================================================

/**
 * Show Preferences / Settings dialog.
 * Win32: IDM_PREF → PreferenceDlg
 * Qt: PreferenceDialog (tabbed dialog for all settings)
 */
void NppCommands::showPreferences() {
    if (m_app) m_app->onMenuCommand("preferences");
}

/**
 * Open the shortcut mapper dialog.
 * Win32: IDM_SETTING_SHORTCUT_MAPPER → ShortcutMapper
 */
void NppCommands::showShortcutMapper() {
    if (m_app) m_app->onMenuCommand("shortcutMapper");
}

/**
 * Toggle main toolbar visibility.
 * Win32: IDM_SETTING_TOOLBAR
 * Qt: MainWindow::toggleToolbar()
 */
void NppCommands::toggleToolbar() {
    if (m_app) m_app->onMenuCommand("toggleToolbar");
}

/**
 * Toggle status bar visibility.
 * Win32: IDM_SETTING_STATUSBAR
 * Qt: MainWindow::toggleStatusBar()
 */
void NppCommands::toggleStatusBar() {
    if (m_app) m_app->onMenuCommand("toggleStatusBar");
}

// ============================================================================
// Window commands
// ============================================================================

/**
 * Minimize the main window to taskbar.
 * Win32: IDM_WINDOW_MINIMIZE → ShowWindow(SW_MINIMIZE)
 * Qt: QWindow::setWindowState(Qt::WindowMinimized)
 */
void NppCommands::minimizeWindow() {
    if (m_app) {
        QWidget* w = qobject_cast<QWidget*>(m_app->mainWindow());
        if (w) w->showMinimized();
    }
}

/**
 * Maximize or restore the main window.
 * Win32: IDM_WINDOW_MAXIMIZE → ShowWindow(SW_MAXIMIZE)
 * Qt: QWindow::setWindowState(Qt::WindowMaximized / Qt::WindowNoState)
 */
void NppCommands::maximizeWindow() {
    if (m_app) {
        QWidget* w = qobject_cast<QWidget*>(m_app->mainWindow());
        if (w) {
            if (w->windowState() & Qt::WindowMaximized)
                w->showNormal();
            else
                w->showMaximized();
        }
    }
}

/**
 * Toggle always-on-top (pin) mode for the main window.
 * Win32: IDM_WINDOW_ALWAYSONTOP → SetWindowPos(HWND_TOPMOST) / HWND_NOTOPMOST
 * Qt: QWindow::setFlag(Qt::WindowStaysOnTopHint)
 */
void NppCommands::toggleAlwaysOnTop() {
    if (m_app) m_app->onMenuCommand("toggleAlwaysOnTop");
}

/**
 * Switch to the next open document in the tab bar.
 * Win32: IDM_WINDOW_SWITCH → nextDocument()
 */
void NppCommands::switchToNextDocument() {
    if (m_app) m_app->onMenuCommand("nextDocument");
}

/**
 * Switch to the previous open document in the tab bar.
 * Win32: IDM_WINDOW_SWITCH → prevDocument()
 */
void NppCommands::switchToPrevDocument() {
    if (m_app) m_app->onMenuCommand("prevDocument");
}

// ============================================================================
// Session / Shell commands
// ============================================================================

/**
 * Load a session file (.session) containing open files and layout.
 * Win32: IDM_FILE_LOADSESSION → GetOpenFileName() → loadSession()
 * Qt: QFileDialog → Application::loadSession()
 */
void NppCommands::loadSession() {
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Load Session"),
                                                QString(), tr("Session (*.session)"));
    if (!path.isEmpty() && m_app)
        m_app->loadSession(path.toStdString());
}

/**
 * Save current layout as a session file.
 * Win32: IDM_FILE_SAVESESSION → GetSaveFileName() → saveSession()
 */
void NppCommands::saveSession() {
    QString path = QFileDialog::getSaveFileName(nullptr, tr("Save Session"),
                                                QString(), tr("Session (*.session)"));
    if (!path.isEmpty() && m_app)
        m_app->saveSession(path.toStdString());
}

/**
 * Open the current file's directory in the system file manager.
 * Win32: IDM_EDIT_OPENINBROW → ShellExecute("explore", path)
 * Qt: QDesktopServices::openUrl(QUrl::fromLocalFile(dir))
 */
void NppCommands::openContainingFolder() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    QString path = FileManager::instance().getBufferPath(id);
    if (!path.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
}

/**
 * Open a directory as a workspace (folder browser root).
 * Win32: IDM_EDIT_OPENASWORSPACE → SHBrowseForFolder()
 * Qt: QFileDialog::getExistingDirectory()
 */
void NppCommands::openFolderAsWorkspace() {
    QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Open Folder as Workspace"));
    if (!dir.isEmpty() && m_app)
        m_app->openFolderAsWorkspace(dir.toStdString());
}

void NppCommands::openInFolder() { openContainingFolder(); }

/**
 * Search the clipboard text on Google.
 * Win32: IDM_EDIT_SEARCHONINTERNET → ShellExecute("open", google-url)
 * Qt: QDesktopServices::openUrl with Google search URL
 */
void NppCommands::searchOnInternet() {
    QClipboard* cb = QApplication::clipboard();
    QString text = cb->text(QClipboard::Selection).trimmed();
    if (text.isEmpty())
        text = cb->text().trimmed();
    if (!text.isEmpty()) {
        QUrl url(QStringLiteral("https://www.google.com/search?q=")
                 + QString::fromLatin1(QUrl::toPercentEncoding(text)));
        QDesktopServices::openUrl(url);
    }
}

/**
 * Show the Run dialog to execute shell commands.
 * Win32: IDM_RUN_LAUNCH → RunDlg → ShellExecute()
 * Qt: QInputDialog for command entry → QProcess::startDetached()
 */
void NppCommands::showRunDialog() {
    bool ok = false;
    QString cmd = QInputDialog::getText(nullptr, tr("Run"), tr("Command:"),
                                         QLineEdit::Normal, QString(), &ok).trimmed();
    if (ok && !cmd.isEmpty()) {
        QProcess::startDetached(QStringLiteral("sh"),
                                {QStringLiteral("-c"), cmd});
    }
}
