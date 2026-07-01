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

// Semantic Lift: Win32 documentSnapshot_rc.h → Qt6 header-only resource constants
// Original:  PowerEditor/src/WinControls/DocumentMap/documentSnapshot_rc.h
// Target:    npp-qt/src/WinControls/DocumentMap/documentSnapshot_rc.h

#pragma once

// Resource IDs for DocumentSnapshot / DocumentPeeker floating preview pane
// Mirrors IDD_DOCUMENTSNAPSHOT from the Win32 .rc file

constexpr int IDD_DOCUMENTSNAPSHOT = 3600;
