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

// Qt resource definition equivalent for the RegExt dialog.
// Win32 .rc IDs → Qt widget object names.

constexpr int IDD_REGEXT_BOX = 4000;

constexpr char ObjName_LangList[]            = "langList";
constexpr char ObjName_LangExtList[]          = "langExtList";
constexpr char ObjName_RegisteredExtsList[]   = "registeredExtsList";
constexpr char ObjName_AddFromLangExtBtn[]    = "addFromLangExtBtn";
constexpr char ObjName_CustomExtEdit[]        = "customExtEdit";
constexpr char ObjName_RemoveExtBtn[]         = "removeExtBtn";
constexpr char ObjName_AdminMustBeOnMsg[]     = "adminMustBeOnMsg";
constexpr char ObjName_SupportedExtsLabel[]   = "supportedExtsLabel";
constexpr char ObjName_RegisteredExtsLabel[]  = "registeredExtsLabel";

// Control IDs (kept as integers for compatibility with slot mapping)
constexpr int IDC_REGEXT_LANG_LIST           = IDD_REGEXT_BOX + 1;
constexpr int IDC_REGEXT_LANGEXT_LIST        = IDD_REGEXT_BOX + 2;
constexpr int IDC_REGEXT_REGISTEREDEXTS_LIST = IDD_REGEXT_BOX + 3;
constexpr int IDC_ADDFROMLANGEXT_BUTTON      = IDD_REGEXT_BOX + 4;
constexpr int IDC_CUSTOMEXT_EDIT             = IDD_REGEXT_BOX + 6;
constexpr int IDC_REMOVEEXT_BUTTON           = IDD_REGEXT_BOX + 7;
constexpr int IDC_ADMINMUSTBEONMSG_STATIC    = IDD_REGEXT_BOX + 8;
constexpr int IDC_SUPPORTEDEXTS_STATIC       = IDD_REGEXT_BOX + 9;
constexpr int IDC_REGISTEREDEXTS_STATIC      = IDD_REGEXT_BOX + 10;
