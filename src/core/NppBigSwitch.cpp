// NppBigSwitch.cpp — Command ID dispatch for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "NppBigSwitch.h"
#include "Application.h"
#include <QCoreApplication>

NppBigSwitch::NppBigSwitch(NppCommands* commands, Application* app, QObject* parent)
    : QObject(parent), m_commands(commands), m_app(app)
{}

void NppBigSwitch::init() {
    // ── File commands ─────────────────────────────────────────────────────
    reg(IDM_FILE_NEW,         [this]() { m_app->newFile(); });
    reg(IDM_FILE_OPEN,       [this]() { m_app->onOpenFile(); });
    reg(IDM_FILE_SAVE,        [this]() { m_app->onSaveFile(); });
    reg(IDM_FILE_SAVEAS,     [this]() { m_app->onSaveFileAs(); });
    reg(IDM_FILE_SAVEALL,     [this]() { m_app->onSaveAll(); });
    reg(IDM_FILE_CLOSE,        [this]() { m_app->onCloseFile(); });
    reg(IDM_FILE_CLOSEALL,    [this]() { m_app->onCloseAll(); });
    reg(IDM_FILE_RELOAD,       [this]() { m_app->onMenuCommand("reload"); });
    reg(IDM_FILE_PRINT,       [this]() { m_app->onMenuCommand("print"); });
    reg(IDM_FILE_LOADSESSION,  [this]() { m_app->loadSession(""); });
    reg(IDM_FILE_SAVESESSION, [this]() { m_app->saveSession(""); });

    // ── Edit commands ─────────────────────────────────────────────────────
    reg(IDM_EDIT_UNDO,         [this]() { m_app->onUndo(); });
    reg(IDM_EDIT_REDO,         [this]() { m_app->onRedo(); });
    reg(IDM_EDIT_CUT,          [this]() { m_app->onCut(); });
    reg(IDM_EDIT_COPY,         [this]() { m_app->onCopy(); });
    reg(IDM_EDIT_PASTE,        [this]() { m_app->onPaste(); });
    reg(IDM_EDIT_DELETE,       [this]() { m_app->onDelete(); });
    reg(IDM_EDIT_SELECTALL,   [this]() { m_app->onSelectAll(); });
    reg(IDM_EDIT_DELETELINE,  [this]() { m_app->onMenuCommand("deleteLine"); });
    reg(IDM_EDIT_JOINLINES,  [this]() { m_app->onMenuCommand("joinLines"); });
    reg(IDM_EDIT_TRIM_TRAILING, [this]() { m_app->onMenuCommand("trimTrailing"); });
    reg(IDM_EDIT_TRIM_LEADING,  [this]() { m_app->onMenuCommand("trimLeading"); });
    reg(IDM_EDIT_SORT_ASC,     [this]() { m_app->onMenuCommand("sortAsc"); });
    reg(IDM_EDIT_SORT_DESC,    [this]() { m_app->onMenuCommand("sortDesc"); });
    reg(IDM_EDIT_SORT_INT_ASC,[this]() { m_app->onMenuCommand("sortIntAsc"); });
    reg(IDM_EDIT_SORT_INT_DESC,[this]() { m_app->onMenuCommand("sortIntDesc"); });
    reg(IDM_EDIT_OPENINBROW, [this]() { m_app->onMenuCommand("openInFolder"); });
    reg(IDM_EDIT_SEARCHONINTERNET,[this]() { m_app->onMenuCommand("searchOnInternet"); });

    // ── Search commands ────────────────────────────────────────────────────
    reg(IDM_SEARCH_FIND,        [this]() { m_app->onFind(); });
    reg(IDM_SEARCH_REPLACE,     [this]() { m_app->onReplace(); });
    reg(IDM_SEARCH_FINDNEXT,   [this]() { m_app->onFindNext(); });
    reg(IDM_SEARCH_FINDPREV,   [this]() { m_app->onFindPrev(); });
    reg(IDM_SEARCH_GOTOLINE,   [this]() { m_app->onGotoLine(); });
    reg(IDM_SEARCH_MARKALL,    [this]() { m_app->onMarkAll(); });
    reg(IDM_SEARCH_UNMARKALL,  [this]() { m_app->onMenuCommand("unmarkAll"); });
    reg(IDM_SEARCH_FINDFILES,  [this]() { m_app->onFindInFiles(); });

    // ── View commands ──────────────────────────────────────────────────────
    reg(IDM_VIEW_ZOOMIN,     [this]() { m_app->onMenuCommand("zoomIn"); });
    reg(IDM_VIEW_ZOOMOUT,    [this]() { m_app->onMenuCommand("zoomOut"); });
    reg(IDM_VIEW_ZOOMRESTORE,[this]() { m_app->onMenuCommand("zoomRestore"); });
    reg(IDM_VIEW_WORDWRAP,   [this]() { m_app->onMenuCommand("toggleWordWrap"); });
    reg(IDM_VIEW_EOL,        [this]() { m_app->onMenuCommand("toggleEol"); });
    reg(IDM_VIEW_ALLCHAR,    [this]() { m_app->onMenuCommand("showAllChars"); });
    reg(IDM_VIEW_DOC_MAP,    [this]() { m_app->onMenuCommand("toggleDocMap"); });
    reg(IDM_VIEW_FUNC_LIST,  [this]() { m_app->onMenuCommand("toggleFuncList"); });
    reg(IDM_VIEW_FILE_BROWSER,[this]() { m_app->onMenuCommand("toggleFileBrowser"); });
    reg(IDM_VIEW_FULLSCREEN, [this]() { m_app->toggleFullScreen(); });
    reg(IDM_VIEW_SWITCH_MAIN,[this]() { m_app->switchToView(0); });
    reg(IDM_VIEW_SWITCH_SUB, [this]() { m_app->switchToView(1); });

    // ── Encoding commands ─────────────────────────────────────────────────
    reg(IDM_ENCODING_ANSI,  [this]() { m_app->onConvertEncoding(EncodingType::ANSI); });
    reg(IDM_ENCODING_UTF8,  [this]() { m_app->onConvertEncoding(EncodingType::UTF_8); });
    reg(IDM_ENCODING_UTF16, [this]() { m_app->onConvertEncoding(EncodingType::UTF_8_BOM); });

    // ── EOL commands ─────────────────────────────────────────────────────
    reg(IDM_EOL_CRLF, [this]() { m_app->onMenuCommand("eol:CRLF"); });
    reg(IDM_EOL_LF,  [this]() { m_app->onMenuCommand("eol:LF"); });
    reg(IDM_EOL_CR,  [this]() { m_app->onMenuCommand("eol:CR"); });

    // ── Macro commands ─────────────────────────────────────────────────────
    reg(IDM_MACRO_RECORD,   [this]() { m_app->onMenuCommand("macroRecord"); });
    reg(IDM_MACRO_STOP,     [this]() { m_app->onMenuCommand("macroStop"); });
    reg(IDM_MACRO_PLAYBACK, [this]() { m_app->onMenuCommand("macroPlayback"); });
    reg(IDM_MACRO_SAVE,     [this]() { m_app->onMenuCommand("macroSave"); });

    // ── Run commands ──────────────────────────────────────────────────────
    reg(IDM_RUN_LAUNCH, [this]() { m_app->onMenuCommand("run"); });

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
