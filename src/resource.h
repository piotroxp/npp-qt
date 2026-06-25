// Semantic Lift: Win32 resource.h → Qt6 resource ID constants
// Original: PowerEditor/src/resource.h
// Target: npp-qt/src/resource.h
//
// Provides resource ID constants (icon IDs, dialog IDs, menu IDs)
// that were defined as Win32 #define in the original.
// In Qt, these are used for identifying UI elements from Notepad++
// message maps and dialog IDs. Icons are loaded from Qt resource (.qrc).

#pragma once

// ============================================================================
// Version info
// ============================================================================
#define NOTEPAD_PLUS_VERSION L"Notepad++ v8.9.6"
#define VERSION_INTERNAL_VALUE L"8.96"
#define VERSION_PRODUCT_VALUE L"8.9.6"
#define VERSION_DIGITALVALUE 8, 9, 6, 0

// ============================================================================
// Dialog IDs
// ============================================================================
#define IDD_ABOUTBOX              1700
#define IDD_DEBUGINFOBOX          1750
#define IDD_COMMANDLINEARGSBOX    1755
#define IDD_DOSAVEORNOTBOX        1760
#define IDD_DOSAVEALLBOX          1765
#define IDD_GOLINE                2000
#define IDD_BUTTON_DLG            2410
#define IDD_SETTING_DLG          2500

// ============================================================================
// Combo box / list control IDs
// ============================================================================
#ifndef IDC_STATIC
#define IDC_STATIC                -1
#endif

// ============================================================================
// Icon IDs (IDI_*) — Qt resource paths loaded from npp-icons.qrc
// ============================================================================
#define IDI_NPPABOUT_LOGO         99
#define IDI_ICONABSENT           104
#define IDR_RT_MANIFEST          103

// Toolbar icon set 1
#define IDI_NEW_ICON                     201
#define IDI_OPEN_ICON                    202
#define IDI_CLOSE_ICON                   203
#define IDI_CLOSEALL_ICON               204
#define IDI_SAVE_ICON                   205
#define IDI_SAVEALL_ICON               206
#define IDI_CUT_ICON                   207
#define IDI_COPY_ICON                  208
#define IDI_PASTE_ICON                 209
#define IDI_UNDO_ICON                  210
#define IDI_REDO_ICON                  211
#define IDI_FIND_ICON                  212
#define IDI_REPLACE_ICON               213
#define IDI_ZOOMIN_ICON                214
#define IDI_ZOOMOUT_ICON               215
#define IDI_VIEW_UD_DLG_ICON          216
#define IDI_PRINT_ICON                 217
#define IDI_STARTRECORD_ICON           218
#define IDI_STOPRECORD_ICON            220
#define IDI_PLAYRECORD_ICON            222
#define IDI_SAVERECORD_ICON            224
#define IDI_VIEW_ALL_CHAR_ICON        228
#define IDI_VIEW_INDENT_ICON           229
#define IDI_VIEW_WRAP_ICON            230
#define IDI_SYNCV_ICON                238
#define IDI_SYNCH_ICON                240
#define IDI_VIEW_DOC_MAP_ICON         242
#define IDI_VIEW_FILEBROWSER_ICON     243
#define IDI_VIEW_FUNCLIST_ICON        244
#define IDI_VIEW_MONITORING_ICON       245
#define IDI_VIEW_DOCLIST_ICON          247

// Toolbar icon set 1 - Dark mode variants
#define IDI_NEW_ICON_DM                251
#define IDI_OPEN_ICON_DM               252
#define IDI_SAVE_ICON_DM              255
#define IDI_VIEW_DOC_MAP_ICON_DM      292
#define IDI_VIEW_FILEBROWSER_ICON_DM  293
#define IDI_VIEW_FUNCLIST_ICON_DM     294
#define IDI_VIEW_DOCLIST_ICON_DM      297

// Document status icons
#define IDI_SAVED_ICON        501
#define IDI_UNSAVED_ICON      502
#define IDI_READONLY_ICON     503
#define IDI_FIND_RESULT_ICON  504
#define IDI_MONITORING_ICON   505
#define IDI_READONLYSYS_ICON  513

// Project panel icons
#define IDI_PROJECT_WORKSPACE         601
#define IDI_PROJECT_WORKSPACEDIRTY   602
#define IDI_PROJECT_PROJECT          603
#define IDI_PROJECT_FOLDEROPEN       604
#define IDI_PROJECT_FOLDERCLOSE      605
#define IDI_PROJECT_FILE            606
#define IDI_PROJECT_FILEINVALID      607
#define IDI_FB_ROOTOPEN             608
#define IDI_FB_ROOTCLOSE           609

// File browser toolbar icons
#define IDI_FB_SELECTCURRENTFILE     630
#define IDI_FB_FOLDALL              631
#define IDI_FB_EXPANDALL            632

// ============================================================================
// Custom window messages
// ============================================================================
#define NOTEPADPLUS_USER_INTERNAL  (0x0400 + 0)   // WM_USER

// ============================================================================
// Scintilla messages
// ============================================================================
#define SCINTILLA_USER (0x0400 + 2000)   // WM_USER + 2000

// ============================================================================
// Macro / Colour / BabyGrid user messages
// ============================================================================
#define MACRO_USER    (0x0400 + 4000)
#define WM_MACRODLGRUNMACRO (MACRO_USER + 2)

#define COLOURPOPUP_USER (0x0400 + 6000)
#define WM_PICKUP_COLOR  (COLOURPOPUP_USER + 1)

#define BABYGRID_USER (0x0400 + 7000)
