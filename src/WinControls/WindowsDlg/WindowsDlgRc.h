// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Semantic Lift: Win32 .rc.h resource IDs → Qt6 constant definitions
// .rc.h files are pure integer constants for dialog/layout IDs.
// Qt6 uses these values for findChild/objectName lookups and signal mappings.

#pragma once

// Windows dialog IDs — base 7000
#define IDD_WINDOWS 7000
#define IDC_WINDOWS_LIST     (IDD_WINDOWS + 1)
#define IDC_WINDOWS_SAVE     (IDD_WINDOWS + 2)
#define IDC_WINDOWS_CLOSE    (IDD_WINDOWS + 3)
#define IDC_WINDOWS_SORT     (IDD_WINDOWS + 4)
