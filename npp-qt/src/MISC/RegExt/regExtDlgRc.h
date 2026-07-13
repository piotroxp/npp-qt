// Semantic Lift: Win32 resource constants → Qt6 resource constants
// Original: PowerEditor/src/MISC/RegExt/regExtDlgRc.h
// Target: npp-qt/src/MISC/RegExt/regExtDlgRc.h
// Win32→Qt6: resource IDs → enum; Win32 .rc dialog definition → Qt Designer .ui or code layout

#pragma once

// Dialog resource IDs
constexpr int IDD_REGEXT_BOX = 4000;

// Control IDs
constexpr int IDC_REGEXT_LANG_LIST           = IDD_REGEXT_BOX + 1;
constexpr int IDC_REGEXT_LANGEXT_LIST        = IDD_REGEXT_BOX + 2;
constexpr int IDC_REGEXT_REGISTEREDEXTS_LIST = IDD_REGEXT_BOX + 3;
constexpr int IDC_ADDFROMLANGEXT_BUTTON      = IDD_REGEXT_BOX + 4;
constexpr int IDI_POUPELLE_ICON              = IDD_REGEXT_BOX + 5;
constexpr int IDC_CUSTOMEXT_EDIT             = IDD_REGEXT_BOX + 6;
constexpr int IDC_REMOVEEXT_BUTTON           = IDD_REGEXT_BOX + 7;
constexpr int IDC_ADMINMUSTBEONMSG_STATIC    = IDD_REGEXT_BOX + 8;
constexpr int IDC_SUPPORTEDEXTS_STATIC      = IDD_REGEXT_BOX + 9;
constexpr int IDC_REGISTEREDEXTS_STATIC     = IDD_REGEXT_BOX + 10;
