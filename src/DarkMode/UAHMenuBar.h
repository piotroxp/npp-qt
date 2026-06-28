// Semantic lift: UAHMenuBar.h — Win32 UAH menu bar dark mode → Qt6 stub
// Original: PowerEditor/src/DarkMode/UAHMenuBar.h
// Target: npp-qt/src/DarkMode/UAHMenuBar.h
//
// UAH (User Alert / Acrylic) menu bar dark mode is a Windows 10/11-specific
// technique for drawing the native menu bar with dark mode using WM_* menu
// window messages (UAHDRAWMENU, UAHDRAWMENUITEM, UAHMEASUREMENUITEM, etc.).
//
// On Qt6:
//   - Menu bars are rendered using QMenuBar / QMenu which are fully styled
//     via Qt stylesheets and the NppDarkMode palette.
//   - No WM_* UAH messages exist on Linux/X11/Wayland.
//   - The Qt menu rendering is cross-platform by default.
//
// All Win32 types (HMENU, HDC, DRAWITEMSTRUCT, MEASUREITEMSTRUCT) are removed.
// Menu bar dark mode on Qt6 is handled by applying the NppDarkMode palette to
// QMenuBar and its QMenu children.
//
// Copyright (C) 2021 adzm / Adam D. Walling — MIT / GPLv3

#pragma once

#include <QWidget>

// =============================================================================
// Menu bar dark mode message handler (Win32 → Qt6 stub)
// =============================================================================

// UAHDarkModeWndProc — process UAH menu bar messages for dark mode
// Win32:
//   - WM_UAHDRAWMENU       → draw dark background for menu bar
//   - WM_UAHDRAWMENUITEM   → draw dark menu items
//   - WM_UAHMEASUREMENUITEM→ measure menu item size for dark text
//   - WM_UAHDESTROYWINDOW  → cleanup (DefWindowProc)
//   - WM_UAHINITMENU       → no-op (DefWindowProc)
//   - WM_UAHNCPAINTMENUPOPUP → no-op (DefWindowProc)
// Qt6:
//   Menu bar dark mode is handled by Qt's style system + NppDarkMode palette.
//   This function is a no-op on Linux — returns false (message not handled).
//
// Parameters:
//   widget   — QWidget* equivalent of HWND hWnd
//   message  — Qt event type (maps to Win32 WM_*)
//   wParam   — Qt event data (maps to WPARAM)
//   lParam   — Qt event data (maps to LPARAM)
//   result   — out parameter for return value (maps to LRESULT*)
//
// Returns: true if handled (no further processing), false to continue
inline bool UAHDarkModeWndProc(QWidget* /*widget*/,
                                unsigned int /*message*/,
                                long long /*wParam*/,
                                long long /*lParam*/,
                                long* /*result*/)
{
    // Win32: processes WM_UAHDESTROYWINDOW, WM_UAHDRAWMENU, WM_UAHDRAWMENUITEM,
    //        WM_UAHINITMENU, WM_UAHMEASUREMENUITEM, WM_UAHNCPAINTMENUPOPUP
    // Qt6:   all menu rendering is handled by Qt's QMenuBar/QMenu stylesheets
    //        and the NppDarkMode palette. No UAH message processing needed.
    return false;
}

// =============================================================================
// Win32 UAH window messages — documented for reference only
// These do not exist on Qt6/Linux; the values here are for source-reference
// =============================================================================

// Win32: WM_UAHDESTROYWINDOW    0x0090  — menu bar being destroyed
// Win32: WM_UAHDRAWMENU         0x0091  — lParam = UAHMENU*
// Win32: WM_UAHDRAWMENUITEM     0x0092  — lParam = UAHDRAWMENUITEM*
// Win32: WM_UAHINITMENU         0x0093  — menu initialized
// Win32: WM_UAHMEASUREMENUITEM  0x0094  — lParam = UAHMEASUREMENUITEM*
// Win32: WM_UAHNCPAINTMENUPOPUP 0x0095  — non-client paint for popup menu
static constexpr unsigned int WM_UAHDESTROYWINDOW    = 0x0090;
static constexpr unsigned int WM_UAHDRAWMENU         = 0x0091;
static constexpr unsigned int WM_UAHDRAWMENUITEM     = 0x0092;
static constexpr unsigned int WM_UAHINITMENU         = 0x0093;
static constexpr unsigned int WM_UAHMEASUREMENUITEM  = 0x0094;
static constexpr unsigned int WM_UAHNCPAINTMENUPOPUP = 0x0095;

// =============================================================================
// Win32 UAH structures — documented for reference only
// Not applicable on Qt6; menu metrics are handled by QMenuBar/QMenu sizing
// =============================================================================

// UAHMENUITEMMETRICS — menu bar / popup item dimensions
// Win32 fields: cx, cy for bar items [2] and popup items [4]
// Qt6: handled automatically by QMenuBar layout engine

// UAHMENUPOPUPMETRICS — popup menu metrics
// Win32 fields: rgcx[4], fUpdateMaxWidths
// Qt6: handled automatically by QMenu

// UAHMENU — menu bar state during draw
// Win32: HMENU hmenu, HDC hdc, DWORD dwFlags
// Qt6:  N/A — Qt paints menus internally

// UAHMENUITEM — menu item position + metrics
// Win32: iPosition (0-based index), umim (metrics), umpm (popup metrics)
// Qt6:  N/A — item positions are managed by QMenu

// UAHDRAWMENUITEM — draw item state
// Win32: DRAWITEMSTRUCT + UAHMENU + UAHMENUITEM
// Qt6:  N/A — Qt handles all menu item painting

// UAHMEASUREMENUITEM — measure item for sizing
// Win32: MEASUREITEMSTRUCT + UAHMENU + UAHMENUITEM
// Qt6:  N/A — Qt measures items via QStyle::sizeFromContents()

// =============================================================================
// Cross-platform menu bar dark mode on Qt6
// =============================================================================
// To apply dark mode to a QMenuBar on Qt6:
//
//   1. Call NppDarkMode::instance().applyToWidget(menuBar);
//      This applies the dark palette to the menu bar widget.
//
//   2. For menu items, set the stylesheet on the menu:
//      menuBar->setStyleSheet(
//        "QMenuBar { background: #202020; color: #E0E0E0; }"
//        "QMenuBar::item:selected { background: #454545; }"
//        "QMenu { background: #202020; color: #E0E0E0; }"
//        "QMenu::item:selected { background: #454545; }"
//      );
//
//   3. Or simply enable dark mode globally:
//      NppDarkMode::instance().setEnabled(true);
//      NppDarkMode::instance().applyToChildWidgets(mainWindow);
//
// The QMenuBar and QMenu classes handle all drawing, sizing, and interaction
// natively in a cross-platform way. No WM_* message interception is needed.
