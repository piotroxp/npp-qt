// Semantic lift: IatHook.h — Win32 PE Import Address Table hooking → stub
// Original: PowerEditor/src/DarkMode/IatHook.h
// Target: npp-qt/src/DarkMode/IatHook.h
//
// WARNING: This file contains Win32 PE (Portable Executable) hooking logic that is
// entirely specific to the Windows platform. The IAT (Import Address Table) patching
// technique is used to intercept DLL function calls at runtime by modifying the
// PE image's import directory.
//
// On Linux/Unix there is no PE format, no IAT, no uxtheme.dll, and no
// equivalent API that requires this kind of low-level hooking.
//
// All functions in this header are stubs that do nothing on non-Windows platforms.
// The dark scroll bar fix that uses this (DarkMode.cpp FixDarkScrollBar) is
// replaced by Qt stylesheet-based dark scroll bars in NppDarkMode on Qt6.
//
// Win32 source reference: PolyHook_2_0 (MIT license) — stevemk14ebr
// https://github.com/stevemk14ebr/PolyHook_2_0

#pragma once

#include <cstdint>
#include <cstddef>

// =============================================================================
// Stub type aliases — preserve signatures for callers that compile on both
// platforms (the function bodies are stubs/empty on Linux)
// =============================================================================

using PIMAGE_DOS_HEADER = void*;
using PIMAGE_NT_HEADERS = void*;
using PIMAGE_THUNK_DATA = void*;
using PIMAGE_IMPORT_DESCRIPTOR = void*;
using PIMAGE_DELAYLOAD_DESCRIPTOR = void*;
using PIMAGE_IMPORT_BY_NAME = void*;

// =============================================================================
// PE utilities (Win32 only — stubs on Linux)
// =============================================================================

// RVA2VA — Relative Virtual Address to Virtual Address conversion
// Win32: base + rva → pointer
// Qt6:  stub returning nullptr (PE format doesn't exist on Linux)
template <typename T, typename T1, typename T2>
constexpr T RVA2VA(T1 /*base*/, T2 /*rva*/)
{
    return nullptr;  // No PE → no VA
}

// DataDirectoryFromModuleBase — get data directory entry from PE header
// Win32: reads OptionalHeader.DataDirectory[entryID]
// Qt6:  stub returning nullptr
template <typename T>
constexpr T DataDirectoryFromModuleBase(void* /*moduleBase*/, size_t /*entryID*/)
{
    return nullptr;
}

// FindAddressByName — scan IAT thunk array for named import
// Win32: iterates IMAGE_THUNK_DATA array, matches by name
// Qt6:  stub returning nullptr
inline void* FindAddressByName(void*, void*, void*, const char*)
{
    return nullptr;
}

// FindAddressByOrdinal — scan IAT thunk array for ordinal import
// Win32: matches IMAGE_SNAP_BY_ORDINAL + IMAGE_ORDINAL
// Qt6:  stub returning nullptr
inline void* FindAddressByOrdinal(void*, void*, void*, uint16_t)
{
    return nullptr;
}

// FindIatThunkInModule — find IAT entry by DLL name + function name
// Win32: iterates IMAGE_IMPORT_DESCRIPTOR array, then thunk arrays
// Qt6:  stub returning nullptr
inline void* FindIatThunkInModule(void*, const char*, const char*)
{
    return nullptr;
}

// FindDelayLoadThunkInModule — find delay-load IAT entry by DLL name + func name
// Win32: scans IMAGE_DELAYLOAD_DESCRIPTOR array
// Qt6:  stub returning nullptr
inline void* FindDelayLoadThunkInModule(void*, const char*, const char*)
{
    return nullptr;
}

// FindDelayLoadThunkInModule — find delay-load IAT entry by DLL name + ordinal
// Win32: scans IMAGE_DELAYLOAD_DESCRIPTOR array, matches by ordinal
// Qt6:  stub returning nullptr
inline void* FindDelayLoadThunkInModule(void*, const char*, uint16_t)
{
    return nullptr;
}

// =============================================================================
// TODO: Cross-platform dark scroll bar replacement
// =============================================================================
// The FixDarkScrollBar() function in DarkMode.cpp uses IAT hooking on Windows
// to intercept uxtheme!OpenNcThemeData and redirect "ScrollBar" class to
// "Explorer::ScrollBar" for dark scroll bars.
//
// On Qt6 this is replaced by:
//   - NppDarkMode::setDarkScrollBar() which applies a stylesheet with
//     dark background (#2a2a2a) and dark handle (#4a4a4a) to all QScrollBars.
//   - No PE hooking is needed; Qt stylesheets operate at a higher level.
//
// The dark scrollbar stylesheet is applied via:
//   w->setStyleSheet("QScrollBar:vertical { background: #2a2a2a; ... }");
//   (see NppDarkMode::setDarkScrollBar() in NppDarkMode.cpp)
