// Semantic Lift: Win32 resource.h → Qt6 resource ID constants
// Original: PowerEditor/src/resource.h
// Target: npp-qt/src/resource.h
//
// Provides resource ID constants (dialog IDs, menu IDs)
// that were defined as Win32 #define in the original.
// Icon IDs (IDI_*) are defined in NppConstants.h — do NOT duplicate here.
//
// In Qt, these are used for identifying UI elements from Notepad++
// message maps and dialog IDs. Icons are loaded from Qt resource (.qrc).
//
// NOTE: Uses constexpr instead of #define to avoid Windows macro conflicts
// (USER, MACRO_USER, COLOURPOPUP_USER, BABYGRID_USER are Win32 macros).

#pragma once

// ============================================================================
// Version info
// ============================================================================
#define NOTEPAD_PLUS_VERSION L"Notepad++ v8.9.6"
#define VERSION_INTERNAL_VALUE L"8.96"
#define VERSION_PRODUCT_VALUE L"Notepad++ v8.9.6"
#define VERSION_DIGITALVALUE 8, 9, 6, 0

// ============================================================================
// Dialog IDs
// ============================================================================
constexpr int IDD_ABOUTBOX           = 1700;
constexpr int IDD_DEBUGINFOBOX       = 1750;
constexpr int IDD_COMMANDLINEARGSBOX = 1755;
constexpr int IDD_DOSAVEORNOTBOX     = 1760;
constexpr int IDD_DOSAVEALLBOX       = 1765;
constexpr int IDD_GOLINE             = 2000;
constexpr int IDD_BUTTON_DLG         = 2410;
constexpr int IDD_SETTING_DLG        = 2500;

// ============================================================================
// Combo box / list control IDs
// ============================================================================
#ifndef IDC_STATIC
constexpr int IDC_STATIC = -1;
#endif

// ============================================================================
// Custom window messages (use WM_USER base + offset)
// ============================================================================
constexpr int NOTEPADPLUS_USER_INTERNAL = 0x0400 + 0;   // WM_USER

// ============================================================================
// Scintilla messages
// ============================================================================
constexpr int NPP_SCINTILLA_USER = 0x0400 + 2000;   // WM_USER + 2000

// ============================================================================
// Macro / Colour / BabyGrid user messages
// ============================================================================
constexpr int NPP_MACRO_USER = 0x0400 + 4000;
// WM_MACRODLGRUNMACRO is defined in NppConstants.h — do not duplicate here

constexpr int NPP_COLOURPOPUP_USER = 0x0400 + 6000;
constexpr int WM_PICKUP_COLOR = NPP_COLOURPOPUP_USER + 1;

// Note: BABYGRID_USER is defined as constexpr in WinControls/Grid/BabyGrid.h
// to avoid macro conflicts with Windows headers