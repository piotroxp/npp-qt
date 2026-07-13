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

// Semantic Lift: Win32 fileBrowser_rc.h → Qt6 header-only resource constants
// Original:  PowerEditor/src/WinControls/FileBrowser/fileBrowser_rc.h
// Target:    npp-qt/src/WinControls/FileBrowser/fileBrowser_rc.h

#pragma once

// Resource IDs for FileBrowser (Folder as Workspace) dialog and context menus
// These mirror the Win32 dialog/menu resource IDs from the original .rc file

// Main FileBrowser panel dialog ID
constexpr int IDD_FILEBROWSER      = 3500;

// FileBrowser context menu IDs
constexpr int IDD_FILEBROWSER_MENU  = IDD_FILEBROWSER + 10;  // 3510

// Context menu command IDs (IDM_FILEBROWSER_*)
constexpr int IDM_FILEBROWSER_REMOVEROOTFOLDER = IDD_FILEBROWSER_MENU + 1;  // 3511
constexpr int IDM_FILEBROWSER_REMOVEALLROOTS   = IDD_FILEBROWSER_MENU + 2;  // 3512
constexpr int IDM_FILEBROWSER_ADDROOT          = IDD_FILEBROWSER_MENU + 3;  // 3513
constexpr int IDM_FILEBROWSER_SHELLEXECUTE     = IDD_FILEBROWSER_MENU + 4;  // 3514
constexpr int IDM_FILEBROWSER_OPENINNPP        = IDD_FILEBROWSER_MENU + 5;  // 3515
constexpr int IDM_FILEBROWSER_COPYPATH         = IDD_FILEBROWSER_MENU + 6;  // 3516
constexpr int IDM_FILEBROWSER_FINDINFILES      = IDD_FILEBROWSER_MENU + 7;  // 3517
constexpr int IDM_FILEBROWSER_EXPLORERHERE     = IDD_FILEBROWSER_MENU + 8;  // 3518
constexpr int IDM_FILEBROWSER_CMDHERE          = IDD_FILEBROWSER_MENU + 9;  // 3519
constexpr int IDM_FILEBROWSER_COPYFILENAME     = IDD_FILEBROWSER_MENU + 10; // 3520

// FileBrowser tree control ID
constexpr int IDD_FILEBROWSER_CTRL = IDD_FILEBROWSER + 30;  // 3530
constexpr int ID_FILEBROWSERTREEVIEW = IDD_FILEBROWSER_CTRL + 1;  // 3531
