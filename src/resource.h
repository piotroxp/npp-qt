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
