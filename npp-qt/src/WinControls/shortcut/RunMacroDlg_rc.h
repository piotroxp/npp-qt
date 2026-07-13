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

// created by Daniel Volk mordorpost@volkarts.com

// Semantic Lift: Win32 .rc.h resource IDs → Qt6 constant definitions
// .rc.h files are pure integer constants for dialog/layout IDs.
// Qt6 uses these values for findChild/objectName lookups and signal mappings.

#pragma once

// Run Macro dialog IDs — base 8000
#define IDD_RUN_MACRO_DLG        8000
#define IDC_M_RUN_MULTI          8001
#define IDC_M_RUN_EOF            8002
#define IDC_M_RUN_TIMES          8003
#define IDC_MACRO_COMBO          8004
#define IDC_TIMES_STATIC         8005
#define IDC_MACRO2RUN_STATIC     8006
