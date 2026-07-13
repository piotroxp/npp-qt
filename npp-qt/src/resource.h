// Semantic lift: resource.h — Win32 resource IDs → Qt resource paths
// Original: PowerEditor/src/resource.h (797 lines) — contains icon/menu IDs
#pragma once

// Icon IDs — mapped to rgba_icons.h
namespace IconID
{
    const int IDI_ICON_NULL = 0;
    const int IDI_ICON_MAIN = 1;
    const int IDI_ICON_NEW = 2;
    const int IDI_ICON_SAVE = 3;
    const int IDI_ICON_OPEN = 4;
    const int IDI_ICON_CLOSE = 5;
    const int IDI_ICON_CUT = 6;
    const int IDI_ICON_COPY = 7;
    const int IDI_ICON_PASTE = 8;
    const int IDI_ICON_UNDO = 9;
    const int IDI_ICON_REDO = 10;
    const int IDI_ICON_FIND = 11;
    const int IDI_ICON_REPLACE = 12;
    const int IDI_ICON_PRINT = 13;
    const int IDI_ICON_EXIT = 14;
    const int IDI_ICON_ABOUT = 15;
    const int IDI_ICON_SETTINGS = 16;
    const int IDI_ICON_LOCK = 17;
    const int IDI_ICON_UNLOCK = 18;
    const int IDI_ICON_FOLDER = 19;
    const int IDI_ICON_FILE = 20;
    const int IDI_ICON_DOCUMENT = 21;
    const int IDI_ICON_TAB_CLOSE = 22;
    const int IDI_ICON_TAB_PREV = 23;
    const int IDI_ICON_TAB_NEXT = 24;
    // Add more as needed
}

// Dialog IDs
namespace DialogID
{
    const int IDD_ABOUT = 100;
    const int IDD_FIND_REPLACE = 101;
    const int IDD_GOTO = 102;
    const int IDD_RUN = 103;
    const int IDD_SETTINGS = 104;
    const int IDD_SHORTCUT_MAPPER = 105;
    const int IDD_WORD_STYLE = 106;
    const int IDD_STYLER = 107;
}

// Menu IDs — start at 40000 (WM_USER)
namespace MenuID
{
    const int IDM_FILE = 40000;
    const int IDM_EDIT = 40050;
    const int IDM_SEARCH = 40100;
    const int IDM_VIEW = 40150;
    const int IDM_FORMAT = 40200;
    const int IDM_LANGUAGE = 40250;
    const int IDM_SETTINGS = 40300;
    const int IDM_TOOLS = 40350;
    const int IDM_WINDOW = 40400;
    const int IDM_HELP = 40450;
}

