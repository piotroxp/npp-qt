// Semantic Lift: Win32 Docking.h → Qt6 docking data structures
// Original: PowerEditor/src/WinControls/DockingWnd/Docking.h
// Target: npp-qt/WinControls/DockingWnd/Docking.h

#pragma once

#include <QString>
#include <QRect>
#include <QWidget>
#include <QPoint>

// =============================================================================
// Docking manager constants — Qt6 port (no Win32 types)
// =============================================================================

// Caption side styles
constexpr bool CAPTION_TOP    = true;
constexpr bool CAPTION_BOTTOM = false;

// Container positions
constexpr int CONT_LEFT   = 0;
constexpr int CONT_RIGHT  = 1;
constexpr int CONT_TOP    = 2;
constexpr int CONT_BOTTOM = 3;
constexpr int DOCKCONT_MAX = 4;

// Mask params for plugin/internal dialogs
constexpr int DWS_ICONTAB        = 0x00000001;  // Icon for tabs are available
constexpr int DWS_ICONBAR        = 0x00000002;  // Icon for icon bar (not yet supported)
constexpr int DWS_ADDINFO       = 0x00000004;  // Additional information in use
constexpr int DWS_USEOWNDARKMODE = 0x00000008; // Use plugin's own dark mode
constexpr int DWS_PARAMSALL      = (DWS_ICONTAB | DWS_ICONBAR | DWS_ADDINFO);

// Default docking values for first plugin call
constexpr unsigned int DWS_DF_CONT_LEFT   = (CONT_LEFT   << 28);
constexpr unsigned int DWS_DF_CONT_RIGHT  = (CONT_RIGHT  << 28);
constexpr unsigned int DWS_DF_CONT_TOP   = (CONT_TOP    << 28);
constexpr unsigned int DWS_DF_CONT_BOTTOM = (CONT_BOTTOM << 28);
constexpr unsigned int DWS_DF_FLOATING    = 0x80000000;

// Splitter dimensions
constexpr int HIT_TEST_THICKNESS = 20;
constexpr int SPLITTER_WIDTH    = 4;

// =============================================================================
// DockedWidgetData (tTbData) — data passed by a plugin/plugin dialog to the
// docking manager.  Win32 HWND/RECT/HICON replaced with Qt6 equivalents.
// =============================================================================
struct DockedWidgetData {
    QWidget*    hClient      = nullptr;  // client widget handle
    QString     name;                    // name of plugin (shown in window)
    int         dlgID        = 0;        // dialog ID from funcItem

    // User modifications
    unsigned int uMask       = 0;        // mask params (DWS_* flags above)
    QString     iconPath;                // icon path (replaces HICON hIconTab)
    QString     addInfo;                 // additional info string (replaces pszAddInfo)

    // Internal data — do not use
    QRect       rcFloat;                 // floating position (replaces RECT)
    int         iPrevCont   = 0;         // previous container when toggling float↔dock
    QString     moduleName;              // plugin file name (replaces pszModuleName)
};

using tTbData = DockedWidgetData;

// =============================================================================
// tDockMgr — docking manager data (one per docking manager)
// =============================================================================
struct tDockMgr {
    QWidget* hWnd         = nullptr;       // the docking manager widget
    QRect    rcRegion[DOCKCONT_MAX] = {}; // positions of docked dialogs
};
