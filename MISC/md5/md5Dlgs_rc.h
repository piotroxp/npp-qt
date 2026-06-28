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

#pragma once

// Qt resource definitions equivalent for MD5/SHA hash dialogs.
// Win32 .rc IDs → Qt widget object names.

// --- IDD_HASHFROMFILES_DLG (1920) ---
constexpr int IDD_HASHFROMFILES_DLG = 1920;
constexpr char ObjName_HashPathEdit[]      = "hashPathEdit";
constexpr char ObjName_HashFileBrowserBtn[] = "hashFileBrowserBtn";
constexpr char ObjName_HashResultEdit[]    = "hashResultEdit";
constexpr char ObjName_HashToClipboardBtn[] = "hashToClipboardBtn";

constexpr int IDC_HASH_PATH_EDIT        = IDD_HASHFROMFILES_DLG + 1;
constexpr int IDC_HASH_FILEBROWSER_BUTTON = IDD_HASHFROMFILES_DLG + 2;
constexpr int IDC_HASH_RESULT_EDIT      = IDD_HASHFROMFILES_DLG + 3;
constexpr int IDC_HASH_TOCLIPBOARD_BUTTON = IDD_HASHFROMFILES_DLG + 4;

// --- IDD_HASHFROMTEXT_DLG (1930) ---
constexpr int IDD_HASHFROMTEXT_DLG = 1930;
constexpr char ObjName_HashTextEdit[]            = "hashTextEdit";
constexpr char ObjName_HashEachLineCheck[]       = "hashEachLineCheck";
constexpr char ObjName_HashResultFromTextEdit[]  = "hashResultFromTextEdit";
constexpr char ObjName_HashFromTextToClipBtn[]   = "hashFromTextToClipBtn";

constexpr int IDC_HASH_TEXT_EDIT              = IDD_HASHFROMTEXT_DLG + 1;
constexpr int IDC_HASH_EACHLINE_CHECK         = IDD_HASHFROMTEXT_DLG + 2;
constexpr int IDC_HASH_RESULT_FOMTEXT_EDIT     = IDD_HASHFROMTEXT_DLG + 3;
constexpr int IDC_HASH_FROMTEXT_TOCLIPBOARD_BUTTON = IDD_HASHFROMTEXT_DLG + 4;
