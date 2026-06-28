// Semantic Lift: Win32 DockingDlgInterface.h → Qt6 docking dialog interface
// Original: PowerEditor/src/WinControls/DockingWnd/DockingDlgInterface.h
// Target: npp-qt/src/WinControls/DockingWnd/DockingDlgInterface.h
//
// Provides the plugin docking dialog interface.
// The DockingDlgInterface base class and supporting types are defined in
// the combined DockingWnd.h.  This header re-exports them for source-level
// compatibility with the original Win32 include graph.

#pragma once

// Pull in the full docking infrastructure (also transitively brings in
// StaticDialog/DockingContainer/DockedWidgetData/etc.)
#include "DockingWnd.h"

// =============================================================================
// DockingDlgInterface — plugin panel base class
//
// Mirrors the Win32 class of the same name (inherits from StaticDialog).
// In the Qt6 port, StaticDialog is a QWidget base (see StaticDialog.h) and
// DockingDlgInterface inherits from it directly, so the full hierarchy is:
//
//   QObject
//     └─ QWidget
//         └─ StaticDialog
//             └─ DockingDlgInterface
//
// Plugins should subclass DockingDlgInterface and implement run_dlgProc().
// =============================================================================

// DockingDlgInterface is already defined in DockingWnd.h
// (re-exported here for source-file compatibility)

// =============================================================================
// Convenience typedef for backwards-compat with Win32 code that used
// the global ::create() overloads.
// =============================================================================

// The two-argument and three-argument create() overloads are implemented in
// DockingWnd.cpp.  Plugins that override create() should call through to the
// base implementation and then set their own UI controls.
