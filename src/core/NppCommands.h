// NppCommands.h — Menu command handlers for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
//
// Semantic lift: Win32 menu command handlers → Qt6 slot methods.
// Each handler delegates to Application (formerly Notepad_plus).
//
// Win32 → Qt translation:
//   Menu command IDs (IDM_*)            → handler method names (camelCase)
//   SendMessage(WM_COMMAND, id, 0)     → command(id) → handler method
//   Scintilla message proxies          → ScintillaEditView / ScintillaEditor wrappers
//   Win32 menu state queries           → Application state queries
//
// Command categories mirror Notepad++ menu layout:
//   File (40001–40049), Edit (40110–40199), Search (40200–40299),
//   View (40300–40399), Encoding (40500–40599), EOL (40600–40699),
//   Macro (43000–43099), Run (44000–44099)
#pragma once

#include <QObject>
#include <QString>
#include "common/Types.h"

class Application;
class ScintillaEditor;

// Entry point: routes integer command IDs to handler methods.
// NppBigSwitch dispatches by ID → calls NppCommands::command(int).
class NppCommands : public QObject {
    Q_OBJECT
public:
    explicit NppCommands(Application* app, QObject* parent = nullptr);
    Application* app() const { return m_app; }

    // ── Dispatch ─────────────────────────────────────────────────────────────
    /** Main dispatch: routes IDM_* command IDs to their handler methods.
     *  Win32: SendMessage(hwnd, WM_COMMAND, MAKE_WPARAM(id, 0), 0)
     *  @param id Menu command ID (e.g. 40001 = IDM_FILE_NEW) */
    void command(int id);

    // ── File commands (IDM_FILE_*) ───────────────────────────────────────────
    /** Create a new untitled buffer. Win32: IDM_FILE_NEW */
    void newFile();
    /** Open file via QFileDialog, then load into Application. Win32: IDM_FILE_OPEN */
    void openFile();
    /** Save current buffer to its existing path. Win32: IDM_FILE_SAVE */
    void save();
    /** Save current buffer to a new path via QFileDialog. Win32: IDM_FILE_SAVEAS */
    void saveAs();
    /** Save all dirty buffers. Win32: IDM_FILE_SAVEALL */
    void saveAll();
    /** Reload current file from disk, discarding unsaved changes. Win32: IDM_FILE_RELOAD */
    void reload();
    /** Close current buffer. Win32: IDM_FILE_CLOSE */
    void closeFile();
    /** Close all buffers. Win32: IDM_FILE_CLOSEALL */
    void closeAll();
    /** Close all buffers except the active one. Win32: IDM_FILE_CLOSEALL_BUT_CURRENT */
    void closeAllButCurrent();
    /** Print the current document (stubbed on Linux). Win32: IDM_FILE_PRINT */
    void print();

    // ── Edit commands (IDM_EDIT_*) ──────────────────────────────────────────
    /** Undo last edit operation. Win32: IDM_EDIT_UNDO */
    void undo();
    /** Redo previously undone edit. Win32: IDM_EDIT_REDO */
    void redo();
    /** Cut selection to clipboard. Win32: IDM_EDIT_CUT */
    void cut();
    /** Copy selection to clipboard. Win32: IDM_EDIT_COPY */
    void copy();
    /** Paste clipboard contents. Win32: IDM_EDIT_PASTE */
    void paste();
    /** Select entire document. Win32: IDM_EDIT_SELECTALL */
    void selectAll();
    /** Delete the current line. Win32: IDM_EDIT_DELETELINE */
    void deleteLine();
    /** Join current and next line. Win32: IDM_EDIT_JOINLINES */
    void joinLines();
    /** Remove trailing whitespace on each line. Win32: IDM_EDIT_TRIM_TRAILING */
    void trimTrailing();
    /** Remove leading whitespace on each line. Win32: IDM_EDIT_TRIM_LEADING */
    void trimLeading();
    /** Sort selected lines by length, ascending. Win32: IDM_EDIT_SORT_INT_ASC */
    void sortLinesLengthAsc();
    /** Sort selected lines by length, descending. Win32: IDM_EDIT_SORT_INT_DESC */
    void sortLinesLengthDesc();
    /** Sort selected lines locale-aware ascending. Win32: IDM_EDIT_SORT_ASC */
    void sortLinesLocaleAsc();
    /** Sort selected lines locale-aware descending. Win32: IDM_EDIT_SORT_DESC */
    void sortLinesLocaleDesc();
    /** Remove duplicate lines from selection. Win32: IDM_EDIT_REMOVE_DUPLICATES */
    void removeDuplicateLines();

    // ── Bookmark commands (IDM_BOOKMARK_*) ─────────────────────────────────
    /** Toggle bookmark on current line. Win32: IDM_BOOKMARK_TOGGLE */
    void toggleBookmark();
    /** Jump to next bookmark. Win32: IDM_BOOKMARK_NEXT */
    void nextBookmark();
    /** Jump to previous bookmark. Win32: IDM_BOOKMARK_PREV */
    void prevBookmark();
    /** Clear all bookmarks in current document. Win32: IDM_BOOKMARK_CLEAR_ALL */
    void clearBookmarks();

    // ── Search commands (IDM_SEARCH_*) ─────────────────────────────────────
    /** Show Find dialog. Win32: IDM_SEARCH_FIND */
    void showFind();
    /** Show Find & Replace dialog. Win32: IDM_SEARCH_REPLACE */
    void showReplace();
    /** Repeat last search forward. Win32: IDM_SEARCH_FINDNEXT */
    void findNext();
    /** Repeat last search backward. Win32: IDM_SEARCH_FINDPREV */
    void findPrevious();
    /** Show Go To Line dialog. Win32: IDM_SEARCH_GOTOLINE */
    void goToLine();
    /** Highlight all matches of given text. Win32: IDM_SEARCH_MARKALL */
    void markAll(const QString& text, int styleId = 0);
    /** Remove all search marks. Win32: IDM_SEARCH_UNMARKALL */
    void unmarkAll(int styleId = 0);
    /** Search in files dialog. Win32: IDM_SEARCH_FINDFILES */
    void findInFiles(const QString& dir, const QString& pattern);

    // ── View commands (IDM_VIEW_*) ──────────────────────────────────────────
    /** Increase font size by one step. Win32: IDM_VIEW_ZOOMIN */
    void zoomIn();
    /** Decrease font size by one step. Win32: IDM_VIEW_ZOOMOUT */
    void zoomOut();
    /** Restore default font size. Win32: IDM_VIEW_ZOOMRESTORE */
    void zoomRestore();
    /** Toggle word wrap on/off. Win32: IDM_VIEW_WORDWRAP */
    void toggleWordWrap();
    /** Toggle EOL symbol visibility. Win32: IDM_VIEW_EOL */
    void toggleEolVisibility();
    /** Toggle all characters visibility (spaces, tabs, EOL). Win32: IDM_VIEW_ALLCHAR */
    void toggleShowAllChars();
    /** Toggle document map panel. Win32: IDM_VIEW_DOC_MAP */
    void toggleDocumentMap();
    /** Toggle function list panel. Win32: IDM_VIEW_FUNC_LIST */
    void toggleFunctionList();
    /** Toggle file browser panel. Win32: IDM_VIEW_FILE_BROWSER */
    void toggleFileBrowser();
    /** Toggle fullscreen mode. Win32: IDM_VIEW_FULLSCREEN */
    void toggleFullscreen();
    /** Switch to main view (view 0). Win32: IDM_VIEW_SWITCH_MAIN */
    void switchToMainView();
    /** Switch to sub view (view 1). Win32: IDM_VIEW_SWITCH_SUB */
    void switchToSubView();
    /** Clone current document to other view. Win32: IDM_VIEW_CLONE */
    void cloneToOtherView();
    /** Move current document to other view. Win32: IDM_VIEW_MOVETO_SUB */
    void moveToOtherView();

    // ── Selection mode commands ─────────────────────────────────────────────
    /** Enable column/box selection mode. Win32: IDM_EDIT_COLUMNMODE */
    void enableColumnMode();
    /** Perform column cut. Win32: IDM_EDIT_CUT_COLUMN */
    void cutColumn();
    /** Perform column copy. Win32: IDM_EDIT_COPY_COLUMN */
    void copyColumn();
    /** Perform column paste. Win32: IDM_EDIT_PASTE_COLUMN */
    void pasteColumn();

    // ── Encoding commands (IDM_ENCODING_*) ─────────────────────────────────
    /** Convert current buffer to ANSI (ISO-8859-1 / system locale). Win32: IDM_ENCODING_ANSI */
    void convertToAnsi();
    /** Convert current buffer to UTF-8 without BOM. Win32: IDM_ENCODING_UTF8 */
    void convertToUtf8();
    /** Convert current buffer to UTF-8 with BOM. Win32: IDM_ENCODING_UTF8_BOM */
    void convertToUtf8Bom();
    /** Convert to UTF-16 LE (native). Win32: IDM_ENCODING_UTF16 */
    void convertToUtf16Le();
    /** Convert to UTF-16 BE. Win32: IDM_ENCODING_UTF16BE */
    void convertToUtf16Be();
    /** Convert to UTF-32 LE. Win32: IDM_ENCODING_UTF32_LE */
    void convertToUtf32();
    /** Convert to UTF-32 BE. Win32: IDM_ENCODING_UTF32_BE */
    void convertToUtf32Be();
    /** Convert to Windows-1252 (Western European). Win32: IDM_ENCODING_ANSI + codepage 1252 */
    void convertToWindows1252();
    /** Convert to ISO-8859-1. Win32: IDM_ENCODING_ANSI */
    void convertToIso88591();

    // ── EOL commands (IDM_EOL_*) ────────────────────────────────────────────
    /** Convert line endings to Windows CRLF. Win32: IDM_EOL_CRLF */
    void convertToWindowsEol();
    /** Convert line endings to Unix LF. Win32: IDM_EOL_LF */
    void convertToUnixEol();
    /** Convert line endings to classic Mac CR. Win32: IDM_EOL_CR */
    void convertToMacEol();

    // ── Macro commands (IDM_MACRO_*) ────────────────────────────────────────
    /** Start recording a macro. Win32: IDM_MACRO_RECORD */
    void startMacroRecording();
    /** Stop recording a macro. Win32: IDM_MACRO_STOP */
    void stopMacroRecording();
    /** Play the last recorded macro. Win32: IDM_MACRO_PLAYBACK */
    void playMacro();
    /** Save the last recorded macro with a name. Win32: IDM_MACRO_SAVE */
    void saveMacro();

    // ── Settings / Preferences commands ────────────────────────────────────
    /** Show Preferences / Settings dialog. Win32: IDM_PREF */
    void showPreferences();
    /** Open shortcut mapper dialog. Win32: IDM_SETTING_SHORTCUT_MAPPER */
    void showShortcutMapper();
    /** Toggle toolbar visibility. Win32: IDM_SETTING_TOOLBAR */
    void toggleToolbar();
    /** Toggle status bar visibility. Win32: IDM_SETTING_STATUSBAR */
    void toggleStatusBar();

    // ── Window commands (IDM_WINDOW_*) ───────────────────────────────────────
    /** Minimize the main window. Win32: IDM_WINDOW_MINIMIZE */
    void minimizeWindow();
    /** Maximize/restore the main window. Win32: IDM_WINDOW_MAXIMIZE */
    void maximizeWindow();
    /** Toggle always-on-top (pin) mode. Win32: IDM_WINDOW_ALWAYSONTOP */
    void toggleAlwaysOnTop();
    /** Switch to the next open document. Win32: IDM_WINDOW_SWITCH */
    void switchToNextDocument();
    /** Switch to the previous open document. Win32: IDM_WINDOW_SWITCH */
    void switchToPrevDocument();

    // ── Session / Shell commands ────────────────────────────────────────────
    /** Load a session file (.session). Win32: IDM_FILE_LOADSESSION */
    void loadSession();
    /** Save current layout as a session file. Win32: IDM_FILE_SAVESESSION */
    void saveSession();
    /** Open file's directory in system file manager. Win32: IDM_EDIT_OPENINBROW */
    void openContainingFolder();
    /** Open selected directory as workspace folder. Win32: IDM_EDIT_OPENASWORSPACE */
    void openFolderAsWorkspace();
    void openInFolder();
    /** Search selected text on Google. Win32: IDM_EDIT_SEARCHONINTERNET */
    void searchOnInternet();
    /** Show Run dialog to execute shell commands. Win32: IDM_RUN_LAUNCH */
    void showRunDialog();

Q_SIGNALS:
    /** Emitted whenever a command is successfully executed.
     *  @param id The command ID that was executed */
    void commandExecuted(int id);
    /** Status bar message signal. @param msg Message to display */
    void statusMessage(const QString& msg);

private:
    Application* m_app = nullptr;
};
