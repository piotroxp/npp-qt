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

// Qt6 port: Win32 .rc resource IDs → Qt widget object-name constants
// These IDs were used in Win32 CreateDialog() calls.
// Qt6: use setObjectName() / findChild() instead of dialog resource loading.
// Control IDs are kept for test automation and lookup consistency.

#pragma once

// FunctionList panel dialog and control IDs
#define IDD_FUNCLIST_PANEL         3400
#define IDC_LIST_FUNCLIST          (IDD_FUNCLIST_PANEL + 1)
#define IDC_LIST_FUNCLIST_AUX      (IDD_FUNCLIST_PANEL + 2)
#define IDC_SEARCHFIELD_FUNCLIST   (IDD_FUNCLIST_PANEL + 3)
#define IDC_RELOADBUTTON_FUNCLIST  (IDD_FUNCLIST_PANEL + 4)
#define IDC_SORTBUTTON_FUNCLIST    (IDD_FUNCLIST_PANEL + 5)
#define IDC_PREFERENCEBUTTON_FUNCLIST (IDD_FUNCLIST_PANEL + 6)
