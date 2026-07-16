// NppBigSwitch.cpp — Command ID dispatch for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "NppBigSwitch.h"
#include "Application.h"
#include <QCoreApplication>
#include <QDebug>

NppBigSwitch::NppBigSwitch(NppCommands* commands, Application* app, QObject* parent)
    : QObject(parent), m_commands(commands), m_app(app)
{}

void NppBigSwitch::init() {
    // ── Name table (human-readable names for all registered commands) ────────
    m_names = {
        // File commands
        {IDM_FILE_NEW,                QStringLiteral("New File")},
        {IDM_FILE_OPEN,               QStringLiteral("Open...")},
        {IDM_FILE_CLOSE,              QStringLiteral("Close")},
        {IDM_FILE_SAVE,               QStringLiteral("Save")},
        {IDM_FILE_SAVEAS,             QStringLiteral("Save As...")},
        {IDM_FILE_SAVEALL,            QStringLiteral("Save All")},
        {IDM_FILE_RELOAD,             QStringLiteral("Reload from Disk")},
        {IDM_FILE_DELETE,             QStringLiteral("Delete File")},
        {IDM_FILE_RENAME,             QStringLiteral("Rename...")},
        {IDM_FILE_PRINT,              QStringLiteral("Print...")},
        {IDM_FILE_CLOSEALL,           QStringLiteral("Close All")},
        {IDM_FILE_CLOSEALL_BUT_CURRENT,QStringLiteral("Close All But Current")},
        {IDM_FILE_LOADSESSION,        QStringLiteral("Load Session...")},
        {IDM_FILE_SAVESESSION,       QStringLiteral("Save Session...")},
        // Edit commands
        {IDM_EDIT_UNDO,              QStringLiteral("Undo")},
        {IDM_EDIT_REDO,              QStringLiteral("Redo")},
        {IDM_EDIT_CUT,               QStringLiteral("Cut")},
        {IDM_EDIT_COPY,              QStringLiteral("Copy")},
        {IDM_EDIT_PASTE,             QStringLiteral("Paste")},
        {IDM_EDIT_DELETE,            QStringLiteral("Delete")},
        {IDM_EDIT_SELECTALL,         QStringLiteral("Select All")},
        {IDM_EDIT_DELETELINE,        QStringLiteral("Delete Line")},
        {IDM_EDIT_JOINLINES,        QStringLiteral("Join Lines")},
        {IDM_EDIT_TRIM_TRAILING,     QStringLiteral("Trim Trailing Space")},
        {IDM_EDIT_TRIM_LEADING,      QStringLiteral("Trim Leading Space")},
        {IDM_EDIT_SORT_ASC,          QStringLiteral("Sort Lines Ascending")},
        {IDM_EDIT_SORT_DESC,         QStringLiteral("Sort Lines Descending")},
        {IDM_EDIT_SORT_INT_ASC,      QStringLiteral("Sort Lines (Integer) Ascending")},
        {IDM_EDIT_SORT_INT_DESC,     QStringLiteral("Sort Lines (Integer) Descending")},
        {IDM_EDIT_SORT_REVERSE,      QStringLiteral("Sort Lines (Reversed)")},
        {IDM_EDIT_OPENINBROW,        QStringLiteral("Open Containing Folder")},
        {IDM_EDIT_SEARCHONINTERNET,  QStringLiteral("Search on Internet")},
        // Search commands
        {IDM_SEARCH_FIND,             QStringLiteral("Find...")},
        {IDM_SEARCH_REPLACE,          QStringLiteral("Replace...")},
        {IDM_SEARCH_FINDNEXT,        QStringLiteral("Find Next")},
        {IDM_SEARCH_FINDPREV,        QStringLiteral("Find Previous")},
        {IDM_SEARCH_GOTOLINE,        QStringLiteral("Go to Line...")},
        {IDM_SEARCH_MARKALL,          QStringLiteral("Mark All")},
        {IDM_SEARCH_UNMARKALL,       QStringLiteral("Unmark All")},
        {IDM_SEARCH_FINDFILES,       QStringLiteral("Find in Files...")},
        // View commands
        {IDM_VIEW_ZOOMIN,            QStringLiteral("Zoom In")},
        {IDM_VIEW_ZOOMOUT,           QStringLiteral("Zoom Out")},
        {IDM_VIEW_ZOOMRESTORE,       QStringLiteral("Restore Default Zoom")},
        {IDM_VIEW_WORDWRAP,          QStringLiteral("Toggle Word Wrap")},
        {IDM_VIEW_EOL,               QStringLiteral("Show EOL")},
        {IDM_VIEW_ALLCHAR,          QStringLiteral("Show All Characters")},
        {IDM_VIEW_DOC_MAP,           QStringLiteral("Toggle Document Map")},
        {IDM_VIEW_FUNC_LIST,        QStringLiteral("Toggle Function List")},
        {IDM_VIEW_FILE_BROWSER,     QStringLiteral("Toggle File Browser")},
        {IDM_VIEW_FULLSCREEN,       QStringLiteral("Toggle Full Screen")},
        {IDM_VIEW_SWITCH_MAIN,       QStringLiteral("Move to Main View")},
        {IDM_VIEW_SWITCH_SUB,        QStringLiteral("Move to Sub View")},
        {IDM_VIEW_CLONE,             QStringLiteral("Clone to Other View")},
        {IDM_VIEW_MOVETO_SUB,        QStringLiteral("Move to Sub View")},
        // Encoding commands
        {IDM_ENCODING_ANSI,          QStringLiteral("Character Sets → ANSI")},
        {IDM_ENCODING_UTF8,         QStringLiteral("UTF-8")},
        {IDM_ENCODING_UTF16,        QStringLiteral("UTF-16 LE with BOM")},
        // EOL commands
        {IDM_EOL_CRLF,              QStringLiteral("Windows (CRLF)")},
        {IDM_EOL_LF,                QStringLiteral("Unix (LF)")},
        {IDM_EOL_CR,                QStringLiteral("Mac (CR)")},
        // Language commands
        {IDM_LANG_MANAGE_UDL,       QStringLiteral("User Defined Language...")},
        // Macro commands
        {IDM_MACRO_RECORD,          QStringLiteral("Start Recording")},
        {IDM_MACRO_STOP,            QStringLiteral("Stop Recording")},
        {IDM_MACRO_PLAYBACK,        QStringLiteral("Playback")},
        {IDM_MACRO_SAVE,            QStringLiteral("Save Recording...")},
        // Run commands
        {IDM_RUN_LAUNCH,            QStringLiteral("Run...")},
    };

    // ── File commands ─────────────────────────────────────────────────────────
    reg(IDM_FILE_NEW,                [this]() { m_app->newFile(); });
    reg(IDM_FILE_OPEN,               [this]() { m_app->onOpenFile(); });
    reg(IDM_FILE_SAVE,              [this]() { m_app->onSaveFile(); });
    reg(IDM_FILE_SAVEAS,            [this]() { m_app->onSaveFileAs(); });
    reg(IDM_FILE_SAVEALL,           [this]() { m_app->onSaveAll(); });
    reg(IDM_FILE_CLOSE,             [this]() { m_app->onCloseFile(); });
    reg(IDM_FILE_CLOSEALL,          [this]() { m_app->onCloseAll(); });
    reg(IDM_FILE_CLOSEALL_BUT_CURRENT,[this]() { m_app->closeAllBuffersExcept(m_app->getActiveBuffer()); });
    reg(IDM_FILE_RELOAD,            [this]() { m_app->onReloadFile(); });
    reg(IDM_FILE_DELETE,            [this]() { m_app->onDeleteFile(); });
    reg(IDM_FILE_RENAME,           [this]() { m_app->onRenameFile(); });
    reg(IDM_FILE_PRINT,             [this]() { m_app->onPrint(); });
    reg(IDM_FILE_LOADSESSION,       [this]() { m_app->loadSession(""); });
    reg(IDM_FILE_SAVESESSION,      [this]() { m_app->saveSession(""); });

    // ── Edit commands ─────────────────────────────────────────────────────────
    reg(IDM_EDIT_UNDO,              [this]() { m_app->onUndo(); });
    reg(IDM_EDIT_REDO,              [this]() { m_app->onRedo(); });
    reg(IDM_EDIT_CUT,               [this]() { m_app->onCut(); });
    reg(IDM_EDIT_COPY,              [this]() { m_app->onCopy(); });
    reg(IDM_EDIT_PASTE,             [this]() { m_app->onPaste(); });
    reg(IDM_EDIT_DELETE,            [this]() { m_app->onDelete(); });
    reg(IDM_EDIT_SELECTALL,         [this]() { m_app->onSelectAll(); });
    reg(IDM_EDIT_DELETELINE,       [this]() { m_app->onDeleteLine(); });
    reg(IDM_EDIT_JOINLINES,        [this]() { m_app->onJoinLines(); });
    reg(IDM_EDIT_TRIM_TRAILING,    [this]() { m_app->onTrimTrailing(); });
    reg(IDM_EDIT_TRIM_LEADING,     [this]() { m_app->onTrimLeading(); });
    reg(IDM_EDIT_SORT_ASC,         [this]() { m_app->onSortAscending(); });
    reg(IDM_EDIT_SORT_DESC,        [this]() { m_app->onSortDescending(); });
    reg(IDM_EDIT_SORT_INT_ASC,     [this]() { m_app->onSortIntAscending(); });
    reg(IDM_EDIT_SORT_INT_DESC,    [this]() { m_app->onSortIntDescending(); });
    reg(IDM_EDIT_SORT_REVERSE,     [this]() { m_app->onSortReverse(); });
    reg(IDM_EDIT_OPENINBROW,       [this]() { m_app->onOpenContainingFolder(); });
    reg(IDM_EDIT_SEARCHONINTERNET, [this]() { m_app->onSearchOnInternet(); });

    // ── Search commands ───────────────────────────────────────────────────────
    reg(IDM_SEARCH_FIND,             [this]() { m_app->onFind(); });
    reg(IDM_SEARCH_REPLACE,          [this]() { m_app->onReplace(); });
    reg(IDM_SEARCH_FINDNEXT,         [this]() { m_app->onFindNext(); });
    reg(IDM_SEARCH_FINDPREV,        [this]() { m_app->onFindPrev(); });
    reg(IDM_SEARCH_GOTOLINE,        [this]() { m_app->onGotoLine(); });
    reg(IDM_SEARCH_MARKALL,         [this]() { m_app->onMarkAll(); });
    reg(IDM_SEARCH_UNMARKALL,       [this]() { m_app->onUnmarkAll(); });
    reg(IDM_SEARCH_FINDFILES,       [this]() { m_app->onFindInFiles(); });

    // ── View commands ────────────────────────────────────────────────────────
    reg(IDM_VIEW_ZOOMIN,             [this]() { m_app->onZoomIn(); });
    reg(IDM_VIEW_ZOOMOUT,            [this]() { m_app->onZoomOut(); });
    reg(IDM_VIEW_ZOOMRESTORE,       [this]() { m_app->onZoomRestore(); });
    reg(IDM_VIEW_WORDWRAP,          [this]() { m_app->onToggleWordWrap(); });
    reg(IDM_VIEW_EOL,               [this]() { m_app->onToggleEolVisibility(); });
    reg(IDM_VIEW_ALLCHAR,           [this]() { m_app->onShowAllCharacters(); });
    reg(IDM_VIEW_DOC_MAP,           [this]() { m_app->onToggleDocMap(); });
    reg(IDM_VIEW_FUNC_LIST,         [this]() { m_app->onToggleFunctionList(); });
    reg(IDM_VIEW_FILE_BROWSER,      [this]() { m_app->onToggleFileBrowser(); });
    reg(IDM_VIEW_FULLSCREEN,        [this]() { m_app->toggleFullScreen(); });
    reg(IDM_VIEW_SWITCH_MAIN,       [this]() { m_app->switchToView(0); });
    reg(IDM_VIEW_SWITCH_SUB,        [this]() { m_app->switchToView(1); });
    reg(IDM_VIEW_CLONE,             [this]() { m_app->cloneToOtherView(m_app->getActiveBuffer()); });
    reg(IDM_VIEW_MOVETO_SUB,        [this]() { m_app->moveToSubView(m_app->getActiveBuffer()); });

    // ── Encoding commands ────────────────────────────────────────────────────
    reg(IDM_ENCODING_ANSI,          [this]() { m_app->onConvertEncoding(EncodingType::ANSI); });
    reg(IDM_ENCODING_UTF8,          [this]() { m_app->onConvertEncoding(EncodingType::UTF_8); });
    reg(IDM_ENCODING_UTF16,         [this]() { m_app->onConvertEncoding(EncodingType::UTF_16_LE); });

    // ── EOL commands ────────────────────────────────────────────────────────
    reg(IDM_EOL_CRLF,              [this]() { m_app->onSetEol(EolType::EOL_CRLF); });
    reg(IDM_EOL_LF,                [this]() { m_app->onSetEol(EolType::EOL_LF); });
    reg(IDM_EOL_CR,                [this]() { m_app->onSetEol(EolType::EOL_CR); });

    // ── Language commands ────────────────────────────────────────────────────
    reg(IDM_LANG_MANAGE_UDL,       [this]() { m_app->onManageUserLanguages(); });

    // ── Macro commands ───────────────────────────────────────────────────────
    reg(IDM_MACRO_RECORD,          [this]() { m_app->onMacroRecord(); });
    reg(IDM_MACRO_STOP,            [this]() { m_app->onMacroStop(); });
    reg(IDM_MACRO_PLAYBACK,        [this]() { m_app->onMacroPlayback(); });
    reg(IDM_MACRO_SAVE,            [this]() { m_app->onMacroSave(); });

    // ── Run commands ─────────────────────────────────────────────────────────
    reg(IDM_RUN_LAUNCH,            [this]() { m_app->onRun(); });

    qDebug("[NppBigSwitch] Registered %d commands", m_dispatch.size());
}

void NppBigSwitch::reg(int id, std::function<void()> handler) {
    m_dispatch[id] = std::move(handler);
}

bool NppBigSwitch::dispatch(int commandId) {
    auto it = m_dispatch.find(commandId);
    if (it != m_dispatch.end()) {
        if (it.value()) it.value()();
        return true;
    }
    qWarning("[NppBigSwitch] Unregistered command ID: %d (%s)",
             commandId, qPrintable(m_names.value(commandId, QStringLiteral("unknown"))));
    return false;
}

bool NppBigSwitch::hasCommand(int commandId) const {
    return m_dispatch.contains(commandId);
}

QList<int> NppBigSwitch::registeredCommands() const {
    return m_dispatch.keys();
}

QString NppBigSwitch::commandName(int id) const {
    return m_names.value(id, QStringLiteral("Unknown(%1)").arg(id));
}
