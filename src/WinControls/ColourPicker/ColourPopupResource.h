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

// Semantic Lift: Win32 ColourPopupResource.h → Qt6 header-only resource constants
// Original:  PowerEditor/src/WinControls/ColourPicker/ColourPopupResource.h
// Target:    npp-qt/src/WinControls/ColourPicker/ColourPopupResource.h

#pragma once

// Resource IDs for the colour picker popup dialog
// These mirror the Win32 dialog resource IDs from the original .rc file

constexpr int IDD_COLOUR_POPUP = 2100;
constexpr int IDC_COLOUR_LIST  = IDD_COLOUR_POPUP + 1;  // 2101
