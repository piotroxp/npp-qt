// Semantic Lift: Win32 docking resource constants → Qt6
// Original: PowerEditor/src/WinControls/DockingWnd/dockingResource.h
// Target: npp-qt/src/WinControls/DockingWnd/dockingResource.h

#pragma once

// =============================================================================
// Docking resource IDs and message constants
// These map Win32 resource IDs and DMM_*/DMN_* messages to Qt6 equivalents.
// =============================================================================

// Win32: property name for "no focus while clicking caption"
#define DM_NOFOCUSWHILECLICKINGCAPTION "NoFocusWhileClickingCaption"

// Win32: dialog resource IDs
constexpr int IDD_PLUGIN_DLG = 103;
constexpr int IDC_EDIT1      = 1000;

// Win32: bitmap IDs for close button
constexpr int IDB_CLOSE_DOWN = 137;
constexpr int IDB_CLOSE_UP   = 138;

// Win32: container dialog resource ID
constexpr int IDD_CONTAINER_DLG = 139;

// Win32: control IDs within container
constexpr int IDC_TAB_CONT   = 1027;
constexpr int IDC_CLIENT_TAB = 1028;
constexpr int IDC_BTN_CAPTION = 1050;

// Note: The Win32 DMM_MSG/DMN_FIRST message constants are defined in Docking.h
// as constexpr integers, since Qt6 doesn't use Windows message maps.