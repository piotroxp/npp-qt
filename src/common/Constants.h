// Constants.h - Application-wide constants for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <cstdint>

// ============================================================================
// Version Info
// ============================================================================
#define APP_NAME        "Notepad--"
#define APP_VERSION     "1.0.0"
#define APP_Title       "Notepad--"
#define APP_YEAR        "2026"

// ============================================================================
// Large File Threshold
// ============================================================================
#define LARGE_FILE_SIZE 10485760  // 10 MB

// ============================================================================
// Path Constants
// ============================================================================
#define APP_DATA_DIR        ".notepad--"
#define SESSION_FILENAME    "session.xml"
#define CONFIG_FILENAME     "config.xml"
#define PLUGIN_CONFIG_DIR   "plugins"
#define USER_CONFIG_DIR     "user.defined"
#define RECENT_FILE_LIST    "recent_files.xml"
#define SHORTCUT_MAPPER_XML "shortcuts.xml"
#define LANG_XML            "langs.xml"
#define LANG_LEXER_STYLE    "lexerStyles.xml"
#define THEMES_DIR          "themes"
#define PLUGIN_DIR          "plugins"

// ============================================================================
// Scintilla Markers (NPP-specific bookmarks only)
// ============================================================================
#ifndef NPP_SCINTILLA_MARKER_DEFINED
#define NPP_SCINTILLA_MARKER_DEFINED 1
#define MARK_BOOKMARK            20
#define MARK_HIDELINESBEGIN      19
#define MARK_HIDELINESEND        18
#define MARK_HIDELINESUDDERLINE  17
#endif

// ============================================================================
// Scintilla User Messages (NPP specific)
// ============================================================================
#define SCINTILLA_USER  2000
#define WM_DOCK_USERDEF_DLG     (SCINTILLA_USER + 1)
#define WM_UNDOCK_USERDEF_DLG   (SCINTILLA_USER + 2)
#define WM_CLOSE_USERDEF_DLG    (SCINTILLA_USER + 3)
