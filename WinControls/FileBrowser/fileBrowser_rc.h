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

#pragma once

// File browser dialog IDs
constexpr int IDD_FILEBROWSER = 3500;
constexpr int IDD_FILEBROWSER_MENU = (IDD_FILEBROWSER + 10);

// File browser menu commands
constexpr int IDM_FILEBROWSER_REMOVEROOTFOLDER = (IDD_FILEBROWSER_MENU + 1);
constexpr int IDM_FILEBROWSER_REMOVEALLROOTS = (IDD_FILEBROWSER_MENU + 2);
constexpr int IDM_FILEBROWSER_ADDROOT = (IDD_FILEBROWSER_MENU + 3);
constexpr int IDM_FILEBROWSER_SHELLEXECUTE = (IDD_FILEBROWSER_MENU + 4);
constexpr int IDM_FILEBROWSER_OPENINNPP = (IDD_FILEBROWSER_MENU + 5);
constexpr int IDM_FILEBROWSER_COPYPATH = (IDD_FILEBROWSER_MENU + 6);
constexpr int IDM_FILEBROWSER_FINDINFILES = (IDD_FILEBROWSER_MENU + 7);
constexpr int IDM_FILEBROWSER_EXPLORERHERE = (IDD_FILEBROWSER_MENU + 8);
constexpr int IDM_FILEBROWSER_CMDHERE = (IDD_FILEBROWSER_MENU + 9);
constexpr int IDM_FILEBROWSER_COPYFILENAME = (IDD_FILEBROWSER_MENU + 10);

// File browser control IDs
constexpr int IDD_FILEBROWSER_CTRL = (IDD_FILEBROWSER + 30);
constexpr int ID_FILEBROWSERTREEVIEW = (IDD_FILEBROWSER_CTRL + 1);