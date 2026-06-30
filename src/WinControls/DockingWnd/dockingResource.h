// npp-qt: Resource IDs for docking windows
// Translated from Win32 DockingWnd/dockingResource.h

#pragma once

// Qt6 equivalent of Win32 resource IDs
constexpr int IDD_PLUGIN_DLG = 103;
constexpr int IDC_EDIT1 = 1000;

constexpr int IDB_CLOSE_DOWN = 137;
constexpr int IDB_CLOSE_UP = 138;
constexpr int IDD_CONTAINER_DLG = 139;

constexpr int IDC_TAB_CONT = 1027;
constexpr int IDC_CLIENT_TAB = 1028;
constexpr int IDC_BTN_CAPTION = 1050;

// Docking manager messages (Win32 message IDs for DMM_* callbacks)
// These are used as Qt event types or signal indices
constexpr int DMM_MSG = 0x5000;
constexpr int DMM_CLOSE = DMM_MSG + 1;
constexpr int DMM_DOCK = DMM_MSG + 2;
constexpr int DMM_FLOAT = DMM_MSG + 3;
constexpr int DMM_DOCKALL = DMM_MSG + 4;
constexpr int DMM_FLOATALL = DMM_MSG + 5;
constexpr int DMM_MOVE = DMM_MSG + 6;
constexpr int DMM_UPDATEDISPINFO = DMM_MSG + 7;
constexpr int DMM_DROPDATA = DMM_MSG + 10;
constexpr int DMM_MOVE_SPLITTER = DMM_MSG + 11;
constexpr int DMM_CANCEL_MOVE = DMM_MSG + 12;
constexpr int DMM_LBUTTONUP = DMM_MSG + 13;

// Notification codes from container to parent
constexpr int DMN_FIRST = 1050;
constexpr int DMN_CLOSE = DMN_FIRST + 1;
constexpr int DMN_DOCK = DMN_FIRST + 2;
constexpr int DMN_FLOAT = DMN_FIRST + 3;
constexpr int DMN_SWITCHIN = DMN_FIRST + 4;
constexpr int DMN_SWITCHOFF = DMN_FIRST + 5;
constexpr int DMN_FLOATDROPPED = DMN_FIRST + 6;
