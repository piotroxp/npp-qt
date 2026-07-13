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

// Semantic Lift: Win32 targetver.h → cross-platform stub
// Original:  PowerEditor/src/WinControls/ReadDirectoryChanges/targetver.h
// Target:    npp-qt/src/WinControls/ReadDirectoryChanges/targetver.h
//
// Win32: This file includes <sdkddkver.h> to define the highest available
// Windows platform (WINVER / _WIN32_WINNT).
//
// Qt6/Linux: On non-Windows platforms this is a no-op stub. The platform
// is determined at CMake configure time, not via Windows SDK headers.

#pragma once

#ifdef _WIN32
    // Delegate to the Windows SDK DDK version header
    #include <sdkddkver.h>
#else
    // Non-Windows platforms: no-op stub
    // Qt6 provides platform abstraction, no WINVER/_WIN32_WINNT needed.
    //
    // If you need to detect the Windows version on a cross-compile host,
    // define -D_WIN32_WINNT=<hex_version> at CMake configure time, e.g.:
    //   -D_WIN32_WINNT=0x0A00  (Windows 10)
#endif
