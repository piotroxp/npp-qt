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

// Qt6 port: Windows SDK version header
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/targetver.h
//
// In Qt6 (cross-platform), this file provides Windows version macros
// for Win32 builds only. On non-Windows platforms it is a no-op stub.

#pragma once

#if defined(_WIN32)

// Request Windows 7 / Windows Server 2008 R2 API set minimum.
// NPP targets Windows 7+ (WINVER = _WIN32_WINNT_WIN7 = 0x0601).
#define WINVER       0x0601
#define _WIN32_WINNT 0x0601

// Prevent inclusion of legacy CRT insecure functions in MinGW builds.
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

// Exclude rarely-used headers from Windows SDK.
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#endif // _WIN32
