// NppBigSwitch.h — Command ID dispatch table for Notepad--Qt
// Maps integer menu command IDs to NppCommands handler method calls
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <functional>

class NppCommands;
class Application;

// QMap<int, std::function<void()>> dispatch table built once at startup.
// Commands registered by category: File, Edit, Search, View, Encoding, EOL, Macro, Run.
class NppBigSwitch : public QObject {
    Q_OBJECT
public:
    explicit NppBigSwitch(NppCommands* commands, Application* app, QObject* parent = nullptr);
    void init();
    bool dispatch(int commandId);
    bool hasCommand(int commandId) const;
    QList<int> registeredCommands() const;
    QString commandName(int id) const;

private:
    void reg(int id, std::function<void()> handler);

    NppCommands* m_commands = nullptr;
    Application* m_app = nullptr;
    QMap<int, std::function<void()>> m_dispatch;
    QMap<int, QString> m_names;

    // ── File commands (40001–40049) ─────────────────────────────────────────
    static constexpr int IDM_FILE_NEW                = 40001;
    static constexpr int IDM_FILE_OPEN              = 40002;
    static constexpr int IDM_FILE_CLOSE             = 40003;
    static constexpr int IDM_FILE_SAVE             = 40004;
    static constexpr int IDM_FILE_SAVEAS            = 40005;
    static constexpr int IDM_FILE_SAVEALL           = 40006;
    static constexpr int IDM_FILE_RELOAD            = 40007;
    static constexpr int IDM_FILE_DELETE            = 40008;
    static constexpr int IDM_FILE_RENAME            = 40009;
    static constexpr int IDM_FILE_PRINT              = 40010;
    static constexpr int IDM_FILE_CLOSEALL           = 40015;
    static constexpr int IDM_FILE_CLOSEALL_BUT_CURRENT = 40016;
    static constexpr int IDM_FILE_LOADSESSION       = 40019;
    static constexpr int IDM_FILE_SAVESESSION      = 40020;

    // ── Edit commands (40110–40199) ──────────────────────────────────────────
    static constexpr int IDM_EDIT_UNDO             = 40110;
    static constexpr int IDM_EDIT_REDO             = 40111;
    static constexpr int IDM_EDIT_CUT              = 40120;
    static constexpr int IDM_EDIT_COPY             = 40121;
    static constexpr int IDM_EDIT_PASTE             = 40122;
    static constexpr int IDM_EDIT_DELETE            = 40123;
    static constexpr int IDM_EDIT_SELECTALL         = 40130;
    static constexpr int IDM_EDIT_DELETELINE        = 40140;
    static constexpr int IDM_EDIT_JOINLINES        = 40150;
    static constexpr int IDM_EDIT_TRIM_TRAILING    = 40160;
    static constexpr int IDM_EDIT_TRIM_LEADING     = 40161;
    static constexpr int IDM_EDIT_SORT_ASC         = 40180;
    static constexpr int IDM_EDIT_SORT_DESC        = 40181;
    static constexpr int IDM_EDIT_SORT_INT_ASC     = 40184;
    static constexpr int IDM_EDIT_SORT_INT_DESC    = 40185;
    static constexpr int IDM_EDIT_SORT_REVERSE     = 40186;
    static constexpr int IDM_EDIT_OPENINBROW      = 40190;
    static constexpr int IDM_EDIT_SEARCHONINTERNET  = 40192;

    // ── Search commands (40200–40299) ───────────────────────────────────────
    static constexpr int IDM_SEARCH_FIND           = 40200;
    static constexpr int IDM_SEARCH_REPLACE        = 40201;
    static constexpr int IDM_SEARCH_FINDNEXT      = 40202;
    static constexpr int IDM_SEARCH_FINDPREV      = 40203;
    static constexpr int IDM_SEARCH_GOTOLINE       = 40210;
    static constexpr int IDM_SEARCH_MARKALL        = 40220;
    static constexpr int IDM_SEARCH_UNMARKALL      = 40221;
    static constexpr int IDM_SEARCH_FINDFILES      = 40230;

    // ── View commands (40300–40399) ─────────────────────────────────────────
    static constexpr int IDM_VIEW_ZOOMIN           = 40300;
    static constexpr int IDM_VIEW_ZOOMOUT         = 40301;
    static constexpr int IDM_VIEW_ZOOMRESTORE     = 40302;
    static constexpr int IDM_VIEW_WORDWRAP         = 40310;
    static constexpr int IDM_VIEW_EOL             = 40321;
    static constexpr int IDM_VIEW_ALLCHAR         = 40320;
    static constexpr int IDM_VIEW_DOC_MAP          = 40350;
    static constexpr int IDM_VIEW_FUNC_LIST       = 40351;
    static constexpr int IDM_VIEW_FILE_BROWSER    = 40352;
    static constexpr int IDM_VIEW_FULLSCREEN      = 40353;
    static constexpr int IDM_VIEW_SWITCH_MAIN     = 40360;
    static constexpr int IDM_VIEW_SWITCH_SUB       = 40361;
    static constexpr int IDM_VIEW_CLONE           = 40362;
    static constexpr int IDM_VIEW_MOVETO_SUB       = 40363;

    // ── Encoding commands (40500–40599) ──────────────────────────────────────
    static constexpr int IDM_ENCODING_ANSI         = 40500;
    static constexpr int IDM_ENCODING_UTF8         = 40501;
    static constexpr int IDM_ENCODING_UTF16        = 40502;

    // ── EOL commands (40600–40699) ─────────────────────────────────────────
    static constexpr int IDM_EOL_CRLF             = 40600;
    static constexpr int IDM_EOL_LF              = 40601;
    static constexpr int IDM_EOL_CR              = 40602;

    // ── Language commands (42000–42099) ─────────────────────────────────────
    static constexpr int IDM_LANG_MANAGE_UDL      = 42000;

    // ── Macro commands (43000–43099) ───────────────────────────────────────
    static constexpr int IDM_MACRO_RECORD          = 43000;
    static constexpr int IDM_MACRO_STOP           = 43001;
    static constexpr int IDM_MACRO_PLAYBACK       = 43002;
    static constexpr int IDM_MACRO_SAVE           = 43003;

    // ── Run commands (44000–44099) ─────────────────────────────────────────
    static constexpr int IDM_RUN_LAUNCH            = 44001;
};
