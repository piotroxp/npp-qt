// Semantic Lift: Win32 Docking.h → Qt6 Docking data structures
// Original: PowerEditor/src/WinControls/DockingWnd/Docking.h
// Target: npp-qt/src/WinControls/DockingWnd/Docking.h
//
// This file provides the core docking data structures and constants.
// The full implementation lives in the combined DockingWnd.h.

#pragma once

// Forward to the full combined header which contains all docking infrastructure
#include "DockingWnd.h"

// =============================================================================
// Legacy constants (backwards compatibility)
// =============================================================================

// Caption position
#define CAPTION_TOP     true
#define CAPTION_BOTTOM  false

// Container positions (mirrored from DockingWnd.h)
#define CONT_LEFT       0
#define CONT_RIGHT      1
#define CONT_TOP        2
#define CONT_BOTTOM     3
#define DOCKCONT_MAX    4

// Docking widget flags
#define DWS_ICONTAB      0x00000001
#define DWS_ICONBAR      0x00000002
#define DWS_ADDINFO      0x00000004
#define DWS_USEOWNDARKMODE 0x00000008
#define DWS_PARAMSALL    (DWS_ICONTAB | DWS_ICONBAR | DWS_ADDINFO)

// Default docking positions (packed: container << 28 | flags)
#define DWS_DF_CONT_LEFT   (CONT_LEFT   << 28)
#define DWS_DF_CONT_RIGHT  (CONT_RIGHT  << 28)
#define DWS_DF_CONT_TOP    (CONT_TOP    << 28)
#define DWS_DF_CONT_BOTTOM (CONT_BOTTOM << 28)
#define DWS_DF_FLOATING    0x80000000

// Splitter geometry
#define HIT_TEST_THICKNESS  20
#define SPLITTER_WIDTH       4

// =============================================================================
// Legacy struct aliases for source compatibility
// (these types are re-exported from DockingWnd.h)
// =============================================================================

// DockedWidgetData is already defined in DockingWnd.h
// typedef DockedWidgetData tTbData; // Already in DockingWnd.h

// tDockMgr / DockMgr are already defined in DockingWnd.h
