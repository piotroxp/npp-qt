// This file is part of Notepad++ project
// Copyright (c) 2025 Notepad++ team

// Windows PE hooking code - stubbed for non-Windows platforms.
// Only compiled on Windows; all functions are no-ops on Linux.

#pragma once

#include <cstdint>
#include <cstddef>

// Stub Windows PE structures
#define PVOID void*
#define ULONG_PTR uintptr_t
#define LPCSTR const char*
#define WORD unsigned short
#define DWORD unsigned int

// Template that converts RVAs to VAs
template <typename T, typename T1, typename T2>
constexpr T RVA2VA(T1 base, T2 rva) {
    return reinterpret_cast<T>(reinterpret_cast<ULONG_PTR>(base) + static_cast<ULONG_PTR>(rva));
}

// Stub PE structures (Windows-only, not used on Linux)
#define PIMAGE_DOS_HEADER void*
#define PIMAGE_NT_HEADERS void*
#define PIMAGE_THUNK_DATA void*
#define PIMAGE_IMPORT_BY_NAME void*
#define PIMAGE_IMPORT_DESCRIPTOR void*
#define PIMAGE_DELAYLOAD_DESCRIPTOR void*
#define IMAGE_SNAP_BY_ORDINAL(x) false
#define IMAGE_ORDINAL(x) 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 13

// Stub functions - these are Windows-only runtime hooks
inline void* FindAddressByName(void*, void*, void*, const char*) { return nullptr; }
inline void* FindAddressByOrdinal(void*, void*, void*, uint16_t) { return nullptr; }
inline void* FindIatThunkInModule(void*, const char*, const char*) { return nullptr; }
inline void* FindDelayLoadThunkInModule(void*, const char*, const char*) { return nullptr; }
inline void* FindDelayLoadThunkInModule(void*, const char*, uint16_t) { return nullptr; }