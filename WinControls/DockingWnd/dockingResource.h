// This file is part of Notepad++ project
// Copyright (C)2006 Jens Lorenz <jens.plugin.npp@gmx.de>

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

// Docking manager message IDs (0x5000-based, Win32 message range)
constexpr int DMM_MSG = 0x5000;
constexpr int DMM_CLOSE = (DMM_MSG + 1);
constexpr int DMM_DOCK = (DMM_MSG + 2);
constexpr int DMM_FLOAT = (DMM_MSG + 3);
constexpr int DMM_DOCKALL = (DMM_MSG + 4);
constexpr int DMM_FLOATALL = (DMM_MSG + 5);
constexpr int DMM_MOVE = (DMM_MSG + 6);
constexpr int DMM_UPDATEDISPINFO = (DMM_MSG + 7);
constexpr int DMM_DROPDATA = (DMM_MSG + 10);
constexpr int DMM_MOVE_SPLITTER = (DMM_MSG + 11);
constexpr int DMM_CANCEL_MOVE = (DMM_MSG + 12);
constexpr int DMM_LBUTTONUP = (DMM_MSG + 13);

// Docking notification IDs (1050-based)
constexpr int DMN_FIRST = 1050;
constexpr int DMN_CLOSE = (DMN_FIRST + 1);
constexpr int DMN_DOCK = (DMN_FIRST + 2);
constexpr int DMN_FLOAT = (DMN_FIRST + 3);
constexpr int DMN_SWITCHIN = (DMN_FIRST + 4);
constexpr int DMN_SWITCHOFF = (DMN_FIRST + 5);
constexpr int DMN_FLOATDROPPED = (DMN_FIRST + 6);

// Dialog IDs
constexpr int IDD_PLUGIN_DLG = 103;
constexpr int IDD_CONTAINER_DLG = 139;

// Control IDs
constexpr int IDC_EDIT1 = 1000;
constexpr int IDC_TAB_CONT = 1027;
constexpr int IDC_CLIENT_TAB = 1028;
constexpr int IDC_BTN_CAPTION = 1050;

// Bitmap IDs
constexpr int IDB_CLOSE_DOWN = 137;
constexpr int IDB_CLOSE_UP = 138;

// Property key for no-focus while clicking caption
#define DM_NOFOCUSWHILECLICKINGCAPTION QStringLiteral("NOFOCUSWHILECLICKINGCAPTION")