// Semantic lift: keys.h — Win32 virtual key codes → Qt6 key constants
#pragma once

#include <QKeySequence>

// NPP-specific key bindings
// These map to menuCmdID.h entries
namespace NppKeys
{
    // File operations
    const int IDM_FILE_NEW = 40001;
    const int IDM_FILE_OPEN = 40002;
    const int IDM_FILE_CLOSE = 40003;
    const int IDM_FILE_SAVE = 40004;
    const int IDM_FILE_SAVEAS = 40005;
    const int IDM_FILE_CLOSEALL = 40006;
    const int IDM_FILE_EXIT = 40007;

    // Edit operations
    const int IDM_EDIT_CUT = 40020;
    const int IDM_EDIT_COPY = 40021;
    const int IDM_EDIT_PASTE = 40022;
    const int IDM_EDIT_UNDO = 40023;
    const int IDM_EDIT_REDO = 40024;
    const int IDM_EDIT_SELECTALL = 40025;
    const int IDM_EDIT_FIND = 40026;
    const int IDM_EDIT_REPLACE = 40027;

    // Search operations
    const int IDM_SEARCH_FINDNEXT = 40100;
    const int IDM_SEARCH_FINDPREV = 40101;
    const int IDM_SEARCH_GOTOLINE = 40102;

    // View operations
    const int IDM_VIEW_FULLSCREEN = 40150;
    const int IDM_VIEW_ZOOMIN = 40151;
    const int IDM_VIEW_ZOOMOUT = 40152;
    const int IDM_VIEW_ZOOMRESET = 40153;

    // Encoding
    const int IDM_FORMAT_ANSI = 40200;
    const int IDM_FORMAT_UTF8 = 40201;
    const int IDM_FORMAT_UTF8_BOM = 40202;
    const int IDM_FORMAT_UCS2BE = 40203;
    const int IDM_FORMAT_UCS2LE = 40204;
}

