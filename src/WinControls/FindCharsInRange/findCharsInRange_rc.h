// Semantic Lift: Win32 resource header → Qt6 enum-based IDs
// Original: PowerEditor/src/WinControls/FindCharsInRange/findCharsInRange_rc.h
// Target: npp-qt/WinControls/FindCharsInRange/findCharsInRange_rc.h
//
// This file defines control IDs used in the Find Characters dialog.
// In Qt6 these map to QDialog::registerPropertyNames / QMetaObject metadata,
// but the integer values are kept identical to the Win32 originals so that
// the docking manager and NPPM_* message codes stay in sync.

#pragma once

// Win32 resource header convention: IDC_STATIC = -1 suppresses warnings
#ifndef IDC_STATIC
#define IDC_STATIC -1
#endif

// Dialog ID base for Find Characters in Range (IDD = dialog, IDC = control)
constexpr int IDD_FINDCHARACTERS       = 2900;

// Control IDs within the Find Characters dialog
constexpr int IDC_NONASCII_RADIO       = IDD_FINDCHARACTERS + 1;  // Find non-ASCII chars
constexpr int IDC_ASCII_RADIO          = IDD_FINDCHARACTERS + 2;  // Find ASCII chars
constexpr int IDC_MYRANGE_RADIO        = IDD_FINDCHARACTERS + 3;  // Custom character range
constexpr int IDC_RANGESTART_EDIT      = IDD_FINDCHARACTERS + 4;  // Range start (e.g. 33)
constexpr int IDC_RANGEEND_EDIT         = IDD_FINDCHARACTERS + 5;  // Range end   (e.g. 126)
constexpr int ID_FINDCHAR_DIRUP         = IDD_FINDCHARACTERS + 6;  // Search upward
constexpr int ID_FINDCHAR_DIRDOWN       = IDD_FINDCHARACTERS + 7;  // Search downward
constexpr int IDC_FINDCHAR_DIR_STATIC   = IDD_FINDCHARACTERS + 8;  // Direction label
constexpr int ID_FINDCHAR_WRAP          = IDD_FINDCHARACTERS + 9;  // Wrap around checkbox
constexpr int ID_FINDCHAR_NEXT          = IDD_FINDCHARACTERS + 10; // "Find Next" button
