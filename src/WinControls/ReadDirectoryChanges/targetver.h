// Semantic Lift: Win32 SDK version headers → Qt6 no-op stub
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/targetver.h
// Target: npp-qt/src/WinControls/ReadDirectoryChanges/targetver.h
//
// In Win32, this header defines the highest available Windows platform version
// (_WIN32_WINNT, NTDDI_VERSION, etc.) to control API availability.
//
// On Qt6/Linux there is no equivalent — platform SDK versioning is irrelevant.
// This stub is kept so that other headers that #include it can compile unchanged.

#pragma once

// Win32 target platform version macros — no-op on non-Windows
// These would normally be defined in <sdkddkver.h> (Windows SDK).
// Stub definitions ensure portability without conditional compilation.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601  // Windows 7 — conservative baseline
#endif

#ifndef WINVER
#define WINVER _WIN32_WINNT
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06010000  // Windows 8 — conservative baseline
#endif
