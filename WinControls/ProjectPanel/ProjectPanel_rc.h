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

// Qt6 port: project panel resource IDs → Qt object names / property keys

#pragma once

// Project panel dialog ID
constexpr int IDD_PROJECTPANEL = 3100;

// Project panel context menu IDs
constexpr int IDD_PROJECTPANEL_MENU = (IDD_PROJECTPANEL + 10);
constexpr int IDM_PROJECT_RENAME         = (IDD_PROJECTPANEL_MENU + 1);
constexpr int IDM_PROJECT_NEWFOLDER      = (IDD_PROJECTPANEL_MENU + 2);
constexpr int IDM_PROJECT_ADDFILES       = (IDD_PROJECTPANEL_MENU + 3);
constexpr int IDM_PROJECT_DELETEFOLDER   = (IDD_PROJECTPANEL_MENU + 4);
constexpr int IDM_PROJECT_DELETEFILE     = (IDD_PROJECTPANEL_MENU + 5);
constexpr int IDM_PROJECT_MODIFYFILEPATH = (IDD_PROJECTPANEL_MENU + 6);
constexpr int IDM_PROJECT_ADDFILESRECUSIVELY = (IDD_PROJECTPANEL_MENU + 7);
constexpr int IDM_PROJECT_MOVEUP         = (IDD_PROJECTPANEL_MENU + 8);
constexpr int IDM_PROJECT_MOVEDOWN       = (IDD_PROJECTPANEL_MENU + 9);

// Project panel workspace submenu IDs
constexpr int IDD_PROJECTPANEL_MENUWS = (IDD_PROJECTPANEL + 20);
constexpr int IDM_PROJECT_NEWPROJECT    = (IDD_PROJECTPANEL_MENUWS + 1);
constexpr int IDM_PROJECT_NEWWS         = (IDD_PROJECTPANEL_MENUWS + 2);
constexpr int IDM_PROJECT_OPENWS        = (IDD_PROJECTPANEL_MENUWS + 3);
constexpr int IDM_PROJECT_RELOADWS      = (IDD_PROJECTPANEL_MENUWS + 4);
constexpr int IDM_PROJECT_SAVEWS        = (IDD_PROJECTPANEL_MENUWS + 5);
constexpr int IDM_PROJECT_SAVEASWS      = (IDD_PROJECTPANEL_MENUWS + 6);
constexpr int IDM_PROJECT_SAVEACOPYASWS = (IDD_PROJECTPANEL_MENUWS + 7);
constexpr int IDM_PROJECT_FINDINPROJECTSWS = (IDD_PROJECTPANEL_MENUWS + 8);

// Project panel control IDs
constexpr int IDD_PROJECTPANEL_CTRL = (IDD_PROJECTPANEL + 30);
constexpr int ID_PROJECTTREEVIEW     = (IDD_PROJECTPANEL_CTRL + 1);
constexpr int IDB_PROJECT_BTN        = (IDD_PROJECTPANEL_CTRL + 2);
constexpr int IDB_EDIT_BTN          = (IDD_PROJECTPANEL_CTRL + 3);

// File relocalizer dialog
constexpr int IDD_FILERELOCALIZER_DIALOG = 3200;
constexpr int IDC_EDIT_FILEFULLPATHNAME  = (IDD_FILERELOCALIZER_DIALOG + 1);
