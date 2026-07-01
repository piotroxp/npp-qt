// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Semantic Lift: Win32 WordStyleDlgRes.h → Qt6 header-only resource constants
// Original:  PowerEditor/src/WinControls/ColourPicker/WordStyleDlgRes.h
// Target:    npp-qt/src/WinControls/ColourPicker/WordStyleDlgRes.h
//
// Defines dialog and control IDs for the Style Editor (Word Style) dialogs.
// Both IDD_STYLER_DLG and IDD_GLOBAL_STYLER_DLG are defined here to match
// the Win32 resource script structure.

#pragma once

// =============================================================================
// IDD_STYLER_DLG — Style Editor / Word Style dialog (ID 2200)
// Mirrors IDD_STYLER_DLG from the Win32 .rc file
// =============================================================================
constexpr int IDD_STYLER_DLG = 2200;

// Control IDs for IDD_STYLER_DLG (base 2200)
constexpr int IDC_FONT_COMBO         = IDD_STYLER_DLG + 2;   // 2202
constexpr int IDC_FONTSIZE_COMBO     = IDD_STYLER_DLG + 3;   // 2203
constexpr int IDC_BOLD_CHECK         = IDD_STYLER_DLG + 4;   // 2204
constexpr int IDC_ITALIC_CHECK       = IDD_STYLER_DLG + 5;   // 2205
constexpr int IDC_FG_STATIC          = IDD_STYLER_DLG + 6;   // 2206
constexpr int IDC_BG_STATIC          = IDD_STYLER_DLG + 7;   // 2207
constexpr int IDC_FONTNAME_STATIC     = IDD_STYLER_DLG + 8;   // 2208
constexpr int IDC_FONTSIZE_STATIC     = IDD_STYLER_DLG + 9;   // 2209
// Note: IDC_STYLEDEFAULT_WARNING_STATIC (2210) deliberately unused
// for translation file compatibility
constexpr int IDC_STYLEDESC_STATIC    = IDD_STYLER_DLG + 11;  // 2211
constexpr int IDC_COLOURGROUP_STATIC  = IDD_STYLER_DLG + 12;  // 2212
constexpr int IDC_FONTGROUP_STATIC    = IDD_STYLER_DLG + 13;  // 2213

constexpr int IDC_DEF_EXT_STATIC      = IDD_STYLER_DLG + 14;  // 2214
constexpr int IDC_DEF_EXT_EDIT        = IDD_STYLER_DLG + 15;  // 2215
constexpr int IDC_USER_EXT_STATIC     = IDD_STYLER_DLG + 16;  // 2216
constexpr int IDC_USER_EXT_EDIT       = IDD_STYLER_DLG + 17;  // 2217
constexpr int IDC_UNDERLINE_CHECK     = IDD_STYLER_DLG + 18;  // 2218
constexpr int IDC_DEF_KEYWORDS_STATIC  = IDD_STYLER_DLG + 19;  // 2219
constexpr int IDC_DEF_KEYWORDS_EDIT   = IDD_STYLER_DLG + 20;  // 2220
constexpr int IDC_USER_KEYWORDS_STATIC = IDD_STYLER_DLG + 21;  // 2221
constexpr int IDC_USER_KEYWORDS_EDIT  = IDD_STYLER_DLG + 22;  // 2222
constexpr int IDC_PLUSSYMBOL_STATIC   = IDD_STYLER_DLG + 23;  // 2223
constexpr int IDC_PLUSSYMBOL2_STATIC  = IDD_STYLER_DLG + 24;  // 2224
constexpr int IDC_LANGDESC_STATIC     = IDD_STYLER_DLG + 25;  // 2225

constexpr int IDC_GLOBAL_FG_CHECK      = IDD_STYLER_DLG + 26;  // 2226
constexpr int IDC_GLOBAL_BG_CHECK      = IDD_STYLER_DLG + 27;  // 2227
constexpr int IDC_GLOBAL_FONT_CHECK    = IDD_STYLER_DLG + 28;  // 2228
constexpr int IDC_GLOBAL_FONTSIZE_CHECK = IDD_STYLER_DLG + 29; // 2229
constexpr int IDC_GLOBAL_BOLD_CHECK    = IDD_STYLER_DLG + 30;  // 2230
constexpr int IDC_GLOBAL_ITALIC_CHECK  = IDD_STYLER_DLG + 31;  // 2231
constexpr int IDC_GLOBAL_UNDERLINE_CHECK = IDD_STYLER_DLG + 32; // 2232
constexpr int IDC_STYLEDESCRIPTION_STATIC = IDD_STYLER_DLG + 33; // 2233
constexpr int IDC_GLOBAL_GOTOSETTINGS_LINK = IDD_STYLER_DLG + 34; // 2234
constexpr int IDC_GLOBAL_WHATISGLOBALOVERRIDE_LINK = IDD_STYLER_DLG + 35; // 2235

// =============================================================================
// IDD_GLOBAL_STYLER_DLG — Global Styles dialog (ID 2300)
// Mirrors IDD_GLOBAL_STYLER_DLG from the Win32 .rc file
// =============================================================================
constexpr int IDD_GLOBAL_STYLER_DLG = 2300;

// Control IDs for IDD_GLOBAL_STYLER_DLG (base 2300)
constexpr int IDC_SAVECLOSE_BUTTON      = IDD_GLOBAL_STYLER_DLG + 1;  // 2301
constexpr int IDC_SC_PERCENTAGE_SLIDER  = IDD_GLOBAL_STYLER_DLG + 2;  // 2302
constexpr int IDC_SC_TRANSPARENT_CHECK  = IDD_GLOBAL_STYLER_DLG + 3;  // 2303
constexpr int IDC_LANGUAGES_COMBO       = IDD_GLOBAL_STYLER_DLG + 4;  // 2304
constexpr int IDC_STYLES_LIST           = IDD_GLOBAL_STYLER_DLG + 5;  // 2305
constexpr int IDC_SWITCH2THEME_STATIC   = IDD_GLOBAL_STYLER_DLG + 6;  // 2306
constexpr int IDC_SWITCH2THEME_COMBO    = IDD_GLOBAL_STYLER_DLG + 7;  // 2307
