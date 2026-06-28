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

// Qt6 port: Win32 resource ID constants → Qt object names / property keys
// These are used by FindReplaceDlg and related Find/Replace UI components.
// In Qt6, dialog layout/object names replace numeric IDs; flags stored as
// QVariant or bitmask enums.

#pragma once

// Dialog base IDs (Qt6: objectName, or stored as int constants in code)
#define IDD_FIND_REPLACE_DLG          1600
#define IDF_WHOLEWORD                  1
#define IDF_MATCHCASE                  2
#define IDREGEXP                      1605
#define IDF_WRAP                    256
#define IDUNSLASH                     1607
#define IDREPLACE                     1608
#define IDREPLACEALL                  1609
#define IDF_WHICH_DIRECTION         512
#define IDCCOUNTALL                  1614
#define IDCMARKALL                   1615
#define IDF_MARKLINE_CHECK            16
#define IDF_STYLEFOUND_CHECK           8
#define IDF_PURGE_CHECK                4
#define IDF_IN_SELECTION_CHECK       128
#define IDF_FINDINFILES_RECURSIVE_CHECK   32
#define IDF_FINDINFILES_INHIDDENDIR_CHECK 64
#define IDF_FINDINFILES_PROJECT1_CHECK  128
#define IDF_FINDINFILES_PROJECT2_CHECK  256
#define IDF_FINDINFILES_PROJECT3_CHECK  512
#define IDF_REDOTMATCHNL            1024

// Dialog IDs
#define IDD_FINDINFILES_DLG         1650
#define IDD_FINDRESULT              1670
#define IDD_INCREMENT_FIND          1680
#define IDD_FINDINFINDER_DLG        1710

// Qt6 resource key constants (mirrors Win32 resource IDs as string-based keys)
// These enable compatibility code that maps Win32 IDs to Qt object names.
namespace FindReplaceResources {
    // Control IDs for Find/Replace dialog
    constexpr int IDFindWhat          = 1601;
    constexpr int IDReplaceWith       = 1602;
    constexpr int IDWholeWord         = 1603;
    constexpr int IDMatchCase         = 1604;
    constexpr int IDWrap              = 1606;
    constexpr int IDReplace           = 1608;
    constexpr int IDReplaceAll        = 1609;
    constexpr int IDStaticTextReplace = 1611;
    constexpr int IDCountAll          = 1614;
    constexpr int IDMarkAll           = 1615;
    constexpr int IDMarkLineCheck     = 1616;
    constexpr int IDPurgeCheck        = 1618;
    constexpr int IDFindAllStatic     = 1619;
    constexpr int IDFindWhatStatic    = 1620;
    constexpr int IDFifFilterTipStatic = 1621;
    constexpr int IDPercentageSlider  = 1622;
    constexpr int IDTransparentGrpBox = 1623;
    constexpr int IDModeStatic        = 1624;
    constexpr int IDNormal            = 1625;
    constexpr int IDExtended          = 1626;
    constexpr int IDInSelectionCheck  = 1632;
    constexpr int IDClearAll          = 1633;
    constexpr int IDReplaceInSelection = 1634;
    constexpr int IDReplaceOpenedFiles = 1635;
    constexpr int IDFindAllOpenedFiles = 1636;
    constexpr int IDFindInFilesLaunch  = 1638;
    constexpr int IDGetCurrentDoctype  = 1639;
    constexpr int IDFindAllCurrentFile = 1641;

    // Find in Files dialog controls
    constexpr int IDDBrowseButton     = 1651;
    constexpr int IDDFiltersCombo     = 1652;
    constexpr int IDDDirCombo         = 1653;
    constexpr int IDDFiltersStatic    = 1654;
    constexpr int IDDDirStatic        = 1655;
    constexpr int IDDFindButton       = 1656;
    constexpr int IDDRecursiveCheck   = 1658;
    constexpr int IDDInHiddenDirCheck = 1659;
    constexpr int IDDReplaceInFiles   = 1660;
    constexpr int IDDSetDirFromDocBtn = 1661;
    constexpr int IDDProject1Check    = 1662;
    constexpr int IDDProject2Check    = 1663;
    constexpr int IDDProject3Check    = 1664;
    constexpr int IDDReplaceInProjects = 1665;
    constexpr int IDDFindInProjects   = 1666;

    // Incremental find controls
    constexpr int IDIncStatic         = 1681;
    constexpr int IDIncFindText       = 1682;
    constexpr int IDIncFindPrevOk     = 1683;
    constexpr int IDIncFindNextOk     = 1684;
    constexpr int IDIncFindMatchCase  = 1685;
    constexpr int IDTransparentCheck  = 1686;
    constexpr int IDTransparentLossFocusRadio = 1687;
    constexpr int IDTransparentAlwaysRadio   = 1688;
    constexpr int IDIncFindStatus     = 1689;
    constexpr int IDIncFindHiliteAll  = 1690;
    constexpr int IDBIncrementalBg    = 1691;

    // Find in Finder dialog controls
    constexpr int IDFindWhatStaticFiFolder  = 1711;
    constexpr int IDFindWhatFiFolder        = 1712;
    constexpr int IDMatchLineNumCheckFiFolder = 1713;
    constexpr int IDWholeWordFiFolder       = 1714;
    constexpr int IDMatchCaseFiFolder       = 1715;
    constexpr int IDModeStaticFiFolder      = 1716;
    constexpr int IDNormalFiFolder          = 1717;
    constexpr int IDExtendedFiFolder        = 1718;
    constexpr int IDRegExpFiFolder          = 1719;
    constexpr int IDRedoMatchNLFiFolder     = 1720;
    constexpr int IDFindPrev                = 1721;
    constexpr int IDBackwardDirection       = 1722;
    constexpr int IDFindNext                = 1723;
    constexpr int ID2ButtonsMode            = 1724;
    constexpr int IDCopyMarkedText          = 1725;
    constexpr int IDSwapFindReplace         = 1726;
    constexpr int IDCopyFind2Replace        = 1727;
    constexpr int IDCopyReplace2Find        = 1728;
} // namespace FindReplaceResources