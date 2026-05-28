// This file is part of Notepad++ project
// Copyright (c) 2025 Notepad++ team

// This file contains Windows-only PE hooking code stubbed for Linux.

#pragma once

// Note: Windows types (HWND, UINT, etc.) are provided by WindowsCompat.h.
// Ensure WindowsCompat.h is included before this header.

// Stub functions - no-op on Linux
inline bool UAHDarkModeWndProc(void*, unsigned int, unsigned long, long, long*) {
    return false;
}