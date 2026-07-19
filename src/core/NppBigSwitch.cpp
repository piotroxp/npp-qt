// NppBigSwitch.cpp — Command ID dispatch for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
//
// Maps canonical Notepad++ integer menu command IDs (from menuCmdID.h) to
// Application method calls.  All IDs verified against menuCmdID.h.
//
// ID ranges:  IDM_FILE=40000  IDM_EDIT=41000  IDM_SEARCH=42000
//             IDM_VIEW=43000  IDM_ENCODING=44000  IDM_LANGUAGE=45000
//             IDM_MACRO=47000  IDM_RUN=48000

#include "NppBigSwitch.h"
#include "core/Application.h"
#include <QCoreApplication>
#include <QDebug>

NppBigSwitch::NppBigSwitch(Application* app, QObject* parent)
    : QObject(parent)
    , m_app(app)
{}

void NppBigSwitch::init() {
    // ── Name table ──────────────────────────────────────────────────────────
    m_names = QMap<int, QString>{
        // File
        { IDM_NEW,                  QStringLiteral("New") },
        { IDM_OPEN,                 QStringLiteral("Open...") },
        { IDM_CLOSE,                QStringLiteral("Close") },
        { IDM_SAVE,                 QStringLiteral("Save") },
        { IDM_SAVEAS,               QStringLiteral("Save As...") },
        { IDM_SAVEALL,              QStringLiteral("Save All") },
        { IDM_RELOAD,               QStringLiteral("Reload") },
        { IDM_DELETE,               QStringLiteral("Delete File") },
        { IDM_RENAME,               QStringLiteral("Rename...") },
        { IDM_PRINT,                QStringLiteral("Print...") },
        { IDM_CLOSEALL,             QStringLiteral("Close All") },
        { IDM_CLOSEALLBUTTHIS,      QStringLiteral("Close All But Current") },
        { IDM_LOADSESSION,          QStringLiteral("Load Session...") },
        { IDM_SAVESESSION,          QStringLiteral("Save Session...") },
        { IDM_OPENFOLDER,           QStringLiteral("Open Containing Folder") },
        // Edit
        { IDM_UNDO,                 QStringLiteral("Undo") },
        { IDM_REDO,                 QStringLiteral("Redo") },
        { IDM_CUT,                  QStringLiteral("Cut") },
        { IDM_COPY,                 QStringLiteral("Copy") },
        { IDM_PASTE,                QStringLiteral("Paste") },
        { IDM_DELETE,               QStringLiteral("Delete") },
        { IDM_SELECTALL,            QStringLiteral("Select All") },
        { IDM_DELETE_LINE,          QStringLiteral("Delete Line") },
        { IDM_DELETE_LINES,         QStringLiteral("Delete Line(s)") },
        { IDM_JOIN_LINES,          QStringLiteral("Join Lines") },
        { IDM_SPLIT_LINES,         QStringLiteral("Split Lines") },
        { IDM_UPPERCASE,            QStringLiteral("UPPERCASE") },
        { IDM_LOWERCASE,            QStringLiteral("lowercase") },
        { IDM_PROPERCASE,           QStringLiteral("Proper Case") },
        { IDM_SENTENCECASE,         QStringLiteral("Sentence case") },
        { IDM_INVERTCASE,           QStringLiteral("Invert Case") },
        { IDM_RANDOMCASE,           QStringLiteral("rAnDoM cAsE") },
        { IDM_EXPAND_TAB,           QStringLiteral("Expand Tab") },
        { IDM_UNEXPAND_TAB,         QStringLiteral("Untabify") },
        { IDM_TAB_TO_SPACE,         QStringLiteral("Tab to Space") },
        { IDM_SPACE_TO_TAB,         QStringLiteral("Space to Tab") },
        { IDM_TRIMTRAILING,         QStringLiteral("Trim Trailing Space") },
        { IDM_TRIMLEADING,         QStringLiteral("Trim Leading Space") },
        { IDM_TRIMBOTH,             QStringLiteral("Trim Leading and Trailing") },
        { IDM_EOL_DOS,             QStringLiteral("Windows (CRLF)") },
        { IDM_EOL_UNIX,            QStringLiteral("Unix (LF)") },
        { IDM_EOL_MAC,             QStringLiteral("Mac (CR)") },
        { IDM_UTF8,                QStringLiteral("UTF-8") },
        { IDM_UTF8_BOM,            QStringLiteral("UTF-8 with BOM") },
        { IDM_ANSI,                QStringLiteral("ANSI") },
        // Search
        { IDM_FIND,                 QStringLiteral("Find...") },
        { IDM_REPLACE,              QStringLiteral("Replace...") },
        { IDM_FINDNEXT,             QStringLiteral("Find Next") },
        { IDM_FINDPREV,             QStringLiteral("Find Previous") },
        { IDM_FINDINCREMENT,        QStringLiteral("Incremental Find") },
        { IDM_FINDCHAR,             QStringLiteral("Find Characters in Range...") },
        { IDM_GOTO,                 QStringLiteral("Go to Line...") },
        { IDM_MARK,                 QStringLiteral("Mark...") },
        { IDM_MARKALL,              QStringLiteral("Mark All") },
        { IDM_UNMARKALL,            QStringLiteral("Unmark All") },
        { IDM_FINDINFILES,          QStringLiteral("Find in Files...") },
        // View
        { IDM_ZOOMIN,               QStringLiteral("Zoom In") },
        { IDM_ZOOMOUT,              QStringLiteral("Zoom Out") },
        { IDM_ZOOMRESTORE,         QStringLiteral("Restore Default Zoom") },
        { IDM_VIEW_WRAP,           QStringLiteral("Word Wrap") },
        { IDM_VIEW_EOL,            QStringLiteral("Show EOL") },
        { IDM_VIEW_ALL_CHAR,       QStringLiteral("Show All Characters") },
        { IDM_VIEW_DOC_MAP,         QStringLiteral("Document Map") },
        { IDM_VIEW_FUNC_LIST,       QStringLiteral("Function List") },
        { IDM_VIEW_FILE_BROWSER,   QStringLiteral("Folder as Workspace") },
        { IDM_VIEW_CLONE,          QStringLiteral("Clone to Other View") },
        { IDM_VIEW_SWITCH_VIEW,    QStringLiteral("Move to Other View") },
        { IDM_TOGGLE_FOLDALL,      QStringLiteral("Collapse All Levels") },
        { IDM_VIEW_HIDELINES,      QStringLiteral("Hide Lines") },
        { IDM_VIEW_MONITORING,     QStringLiteral("Monitor Tail") },
        // Encoding
        { IDM_CONV_UTF8,           QStringLiteral("Convert to UTF-8") },
        { IDM_CONV_UTF8_BOM,       QStringLiteral("Convert to UTF-8 with BOM") },
        { IDM_CONV_ANSI,           QStringLiteral("Convert to ANSI") },
        { IDM_CONV_UTF16_LE,       QStringLiteral("Convert to UTF-16 LE") },
        { IDM_CONV_UTF16_BE,       QStringLiteral("Convert to UTF-16 BE") },
        { IDM_CONV_UTF16_BE_BOM,   QStringLiteral("Convert to UTF-16 BE with BOM") },
        // EOL conversion
        { IDM_EOL_DOS,        QStringLiteral("Convert to Windows (CRLF)") },
        { IDM_EOL_UNIX,       QStringLiteral("Convert to Unix (LF)") },
        { IDM_EOL_MAC,        QStringLiteral("Convert to Mac (CR)") },
        // Macro
        { IDM_MACRO_START_RECORD,   QStringLiteral("Start Recording") },
        { IDM_MACRO_STOP_RECORD,    QStringLiteral("Stop Recording") },
        { IDM_MACRO_PLAYBACK,       QStringLiteral("Playback") },
        { IDM_MACRO_SAVE,          QStringLiteral("Save...") },
        // Run
        { IDM_RUN_LAUNCH,          QStringLiteral("Run...") },
    };

    // ── File commands (40001–40028) ─────────────────────────────────────────
    reg(IDM_NEW,          [this]() { m_app->newFile(); });
    reg(IDM_OPEN,         [this]() { m_app->onOpenFile(); });
    reg(IDM_SAVE,         [this]() { m_app->onSaveFile(); });
    reg(IDM_SAVEAS,       [this]() { m_app->onSaveFileAs(); });
    reg(IDM_SAVEALL,      [this]() { m_app->onSaveAll(); });
    reg(IDM_CLOSE,        [this]() { m_app->onCloseFile(); });
    reg(IDM_CLOSEALL,     [this]() { m_app->onCloseAll(); });
    reg(IDM_CLOSEALLBUTTHIS,[this](){ m_app->onCloseAllButCurrent(); });
    reg(IDM_RELOAD,       [this]() { m_app->onReloadFile(); });
    reg(IDM_DELETE,       [this]() { m_app->onDeleteFile(); });
    reg(IDM_RENAME,       [this]() { m_app->onRenameFile(); });
    reg(IDM_PRINT,        [this]() { m_app->onPrint(); });
    reg(IDM_LOADSESSION,  [this]() { m_app->saveSession(""); });
    reg(IDM_SAVESESSION,  [this]() { m_app->saveSession(""); });
    reg(IDM_OPENFOLDER,   [this]() { m_app->onOpenContainingFolder(); });

    // ── Edit commands (41001–41040) ──────────────────────────────────────────
    reg(IDM_UNDO,         [this]() { m_app->onUndo(); });
    reg(IDM_REDO,         [this]() { m_app->onRedo(); });
    reg(IDM_CUT,          [this]() { m_app->onCut(); });
    reg(IDM_COPY,         [this]() { m_app->onCopy(); });
    reg(IDM_PASTE,        [this]() { m_app->onPaste(); });
    reg(IDM_DELETE,       [this]() { m_app->onDelete(); });
    reg(IDM_SELECTALL,    [this]() { m_app->onSelectAll(); });
    reg(IDM_DELETE_LINE,  [this]() { m_app->onDeleteLine(); });
    reg(IDM_DELETE_LINES, [this]() { m_app->onDeleteLine(); }); // same action
    reg(IDM_JOIN_LINES,   [this]() { m_app->onJoinLines(); });
    reg(IDM_SPLIT_LINES,  [this]() { m_app->onMenuCommand(QStringLiteral("edit.splitLines")); });
    // Case operations
    reg(IDM_UPPERCASE,    [this]() { m_app->onMenuCommand(QStringLiteral("edit.uppercase")); });
    reg(IDM_LOWERCASE,    [this]() { m_app->onMenuCommand(QStringLiteral("edit.lowercase")); });
    reg(IDM_PROPERCASE,   [this]() { m_app->onMenuCommand(QStringLiteral("edit.propercase")); });
    reg(IDM_SENTENCECASE, [this]() { m_app->onMenuCommand(QStringLiteral("edit.sentencecase")); });
    reg(IDM_INVERTCASE,   [this]() { m_app->onMenuCommand(QStringLiteral("edit.invertcase")); });
    reg(IDM_RANDOMCASE,   [this]() { m_app->onMenuCommand(QStringLiteral("edit.randomcase")); });
    // Tab/space operations
    reg(IDM_EXPAND_TAB,   [this]() { m_app->onMenuCommand(QStringLiteral("edit.expandTab")); });
    reg(IDM_UNEXPAND_TAB, [this]() { m_app->onMenuCommand(QStringLiteral("edit.unexpandTab")); });
    reg(IDM_TAB_TO_SPACE, [this]() { m_app->onMenuCommand(QStringLiteral("edit.tabToSpace")); });
    reg(IDM_SPACE_TO_TAB, [this]() { m_app->onMenuCommand(QStringLiteral("edit.spaceToTab")); });
    // Trim operations
    reg(IDM_TRIMTRAILING, [this]() { m_app->onTrimTrailing(); });
    reg(IDM_TRIMLEADING,  [this]() { m_app->onTrimLeading(); });
    reg(IDM_TRIMBOTH,     [this]() {
        m_app->onTrimLeading();
        m_app->onTrimTrailing();
    });
    // EOL conversion (in-place)
    reg(IDM_EOL_DOS,      [this]() { m_app->onSetEol(EolType::EOL_CRLF); });
    reg(IDM_EOL_UNIX,     [this]() { m_app->onSetEol(EolType::EOL_LF); });
    reg(IDM_EOL_MAC,      [this]() { m_app->onSetEol(EolType::EOL_CR); });
    // Encoding in-place
    reg(IDM_UTF8,         [this]() { m_app->onConvertEncoding(EncodingType::UTF_8); });
    reg(IDM_UTF8_BOM,     [this]() { m_app->onConvertEncoding(EncodingType::UTF_8_BOM); });
    reg(IDM_ANSI,         [this]() { m_app->onConvertEncoding(EncodingType::ANSI); });

    // ── Search commands (42001–42018) ──────────────────────────────────────
    reg(IDM_FIND,          [this]() { m_app->onFind(); });
    reg(IDM_REPLACE,       [this]() { m_app->onReplace(); });
    reg(IDM_FINDNEXT,      [this]() { m_app->onFindNext(); });
    reg(IDM_FINDPREV,      [this]() { m_app->onFindPrev(); });
    reg(IDM_FINDINCREMENT,  [this]() { m_app->onFind(); });  // opens same dialog
    reg(IDM_FINDCHAR,      [this]() { m_app->onMenuCommand(QStringLiteral("search.findChar")); });
    reg(IDM_GOTO,          [this]() { m_app->onGotoLine(); });
    reg(IDM_MARK,          [this]() { m_app->onMenuCommand(QStringLiteral("search.mark")); });
    reg(IDM_MARKALL,       [this]() { m_app->onMarkAll(); });
    reg(IDM_UNMARKALL,     [this]() { m_app->onUnmarkAll(); });
    reg(IDM_FINDINFILES,   [this]() { m_app->onFindInFiles(); });

    // ── View commands (43001–43056) ──────────────────────────────────────────
    reg(IDM_ZOOMIN,         [this]() { m_app->zoomIn(); });
    reg(IDM_ZOOMOUT,        [this]() { m_app->zoomOut(); });
    reg(IDM_ZOOMRESTORE,    [this]() { m_app->zoomRestore(); });
    reg(IDM_VIEW_WRAP,      [this]() { m_app->toggleWordWrap(); });
    reg(IDM_VIEW_EOL,       [this]() { m_app->toggleEolVisibility(); });
    reg(IDM_VIEW_ALL_CHAR,  [this]() { m_app->onShowAllCharacters(); });
    reg(IDM_VIEW_DOC_MAP,   [this]() { m_app->toggleDocMap(); });
    reg(IDM_VIEW_FUNC_LIST, [this]() { m_app->toggleFunctionList(); });
    reg(IDM_VIEW_FILE_BROWSER,[this](){ m_app->toggleFileBrowser(); });
    reg(IDM_VIEW_CLONE,     [this]() { m_app->onCloneToOtherView(); });
    reg(IDM_VIEW_SWITCH_VIEW,[this](){ m_app->onMoveToSubView(); });
    reg(IDM_TOGGLE_FOLDALL, [this]() { m_app->onMenuCommand(QStringLiteral("view.foldAll")); });
    reg(IDM_VIEW_HIDELINES, [this]() { m_app->onMenuCommand(QStringLiteral("view.hideLines")); });
    reg(IDM_VIEW_MONITORING,[this]() { m_app->onMenuCommand(QStringLiteral("view.monitoring")); });

    // ── Encoding conversion commands (44001–44010) ──────────────────────────
    reg(IDM_CONV_UTF8,       [this]() { m_app->onConvertEncoding(EncodingType::UTF_8); });
    reg(IDM_CONV_UTF8_BOM,   [this]() { m_app->onConvertEncoding(EncodingType::UTF_8_BOM); });
    reg(IDM_CONV_ANSI,       [this]() { m_app->onConvertEncoding(EncodingType::ANSI); });
    reg(IDM_CONV_UTF16_LE,   [this]() { m_app->onConvertEncoding(EncodingType::UTF_16_LE); });
    reg(IDM_CONV_UTF16_BE,   [this]() { m_app->onConvertEncoding(EncodingType::UTF_16_BE); });
    reg(IDM_CONV_UTF16_BE_BOM,[this](){ m_app->onConvertEncoding(EncodingType::UTF_16_BE_BOM); });

    // ── EOL file conversion (separate from edit EOL) ──────────────────────────
    reg(IDM_EOL_DOS,    [this]() { m_app->onSetEol(EolType::EOL_CRLF); });
    reg(IDM_EOL_UNIX,   [this]() { m_app->onSetEol(EolType::EOL_LF); });
    reg(IDM_EOL_MAC,    [this]() { m_app->onSetEol(EolType::EOL_CR); });

    // ── Macro commands (47001–47005) ─────────────────────────────────────────
    reg(IDM_MACRO_START_RECORD,[this](){ m_app->onMacroStartRecording(); });
    reg(IDM_MACRO_STOP_RECORD,[this](){ m_app->onMacroStopRecording(); });
    reg(IDM_MACRO_PLAYBACK,   [this](){ m_app->onMacroPlaybackLast(); });
    reg(IDM_MACRO_SAVE,       [this](){ m_app->onMacroSave(); });
    reg(IDM_MACRO_RUN,        [this](){ m_app->onMacroPlaybackLast(); });

    // ── Run commands (48001+) ────────────────────────────────────────────────
    reg(IDM_RUN_LAUNCH,      [this]() { m_app->onRun(); });

    qDebug("[NppBigSwitch] Registered %d commands", m_dispatch.size());
}

void NppBigSwitch::reg(int id, std::function<void()> handler) {
    m_dispatch[id] = std::move(handler);
}

bool NppBigSwitch::dispatch(int commandId) {
    auto it = m_dispatch.find(commandId);
    if (it != m_dispatch.end()) {
        if (it.value()) {
            it.value()();
            emit commandExecuted(commandId);
        }
        return true;
    }
    qWarning("[NppBigSwitch] Unregistered command ID: %d (%s)",
             commandId, qPrintable(m_names.value(commandId, QStringLiteral("unknown"))));
    emit commandNotFound(commandId);
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

const char* NppBigSwitch::idToName(int id) {
    static const QMap<int, const char*> table = {
        { IDM_NEW,              "New" },
        { IDM_OPEN,             "Open" },
        { IDM_SAVE,             "Save" },
        { IDM_SAVEAS,           "Save As" },
        { IDM_SAVEALL,          "Save All" },
        { IDM_CLOSE,            "Close" },
        { IDM_UNDO,             "Undo" },
        { IDM_REDO,             "Redo" },
        { IDM_CUT,              "Cut" },
        { IDM_COPY,             "Copy" },
        { IDM_PASTE,            "Paste" },
        { IDM_FIND,             "Find" },
        { IDM_REPLACE,          "Replace" },
        { IDM_FINDNEXT,         "Find Next" },
        { IDM_FINDPREV,         "Find Previous" },
        { IDM_ZOOMIN,           "Zoom In" },
        { IDM_ZOOMOUT,          "Zoom Out" },
        { IDM_ZOOMRESTORE,      "Zoom Restore" },
        { IDM_VIEW_WRAP,        "Word Wrap" },
        { IDM_VIEW_DOC_MAP,     "Doc Map" },
        { IDM_VIEW_FUNC_LIST,   "Function List" },
        { IDM_VIEW_FILE_BROWSER,"File Browser" },
    };
    auto it = table.find(id);
    return (it != table.end()) ? it.value() : "???";
}
