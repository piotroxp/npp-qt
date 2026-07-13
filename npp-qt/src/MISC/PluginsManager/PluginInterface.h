// Semantic lift: PluginInterface.h — Win32 plugin DLL API → Qt6 plugin interface
// Original: PowerEditor/src/MISC/PluginsManager/PluginInterface.h
// Target: npp-qt/src/MISC/PluginsManager/PluginInterface.h
// Win32→Qt6:
//   HWND         → QWidget*
//   __declspec(dllexport) → no-op (no DLL export needed on Linux)
//   wchar_t*     → const char* (plugin name is always ASCII in NPP)
//   __cdecl      → normal calling convention on Linux
//
// This file defines the plugin API that all Notepad++ plugins must implement.
// Plugins are loaded as shared libraries (.so on Linux, .dll on Windows).
// The Qt6 plugin host is in PluginsManager.
//
// Copyright (C) 2025 Don HO <don.h@free.fr> — GPLv3

#pragma once

#include <QString>
#include <QWidget>

// Scintilla notification struct (cross-platform)
#include "Scintilla.h"

// Notepad++ message constants (cross-platform)
#include "Notepad_plus_msgs.h"

#include "NppData.h"

// =============================================================================
// Plugin function pointer types
// =============================================================================

// PFUNCGETNAME — return the plugin's display name
// Win32: const wchar_t* __cdecl (UTF-16LE)
// Qt6:  const char* (UTF-8, null-terminated)
using PFUNCGETNAME = const char* (*)();

// PFUNCSETINFO — receive NppData from the host
// Win32: void __cdecl (NppData)
// Qt6:  void (NppData) — QWidget* handles passed directly
using PFUNCSETINFO = void (*)(NppData);

// PFUNCPLUGINCMD — plugin command function (no parameters)
// Win32: void __cdecl
// Qt6:  void (no change needed)
using PFUNCPLUGINCMD = void (*)();

// PBENOTIFIED — receive Scintilla notifications from the host
// Win32: void __cdecl (SCNotification*)
// Qt6:  void (SCNotification*) — SCNotification is cross-platform
using PBENOTIFIED = void (*)(SCNotification*);

// PMESSAGEPROC — handle custom NPPM_* messages
// Win32: LRESULT __cdecl (UINT Message, WPARAM wParam, LPARAM lParam)
// Qt6:  long long (unsigned int message, long long wParam, long long lParam)
//       LRESULT maps to the native int/pointer size; use intptr_t/long long
using PMESSAGEPROC = long long (*)(unsigned int message, long long wParam, long long lParam);

// PFUNCISUNICODE — check if plugin is Unicode-aware
// Win32: BOOL __cdecl
// Qt6:  bool
using PFUNCISUNICODE = bool (*)();

// PFUNCGETFUNCSARRAY — return the plugin's FuncItem array
// Win32: FuncItem* __cdecl (int* nbItem)
// Qt6:  void* (int* nbItem) — returns pointer, type-erased for plugin loader
using PFUNCGETFUNCSARRAY = void* (*)(int*);

// =============================================================================
// ShortcutKey — keyboard shortcut description
// =============================================================================

// Win32: struct with bool _isCtrl/_isAlt/_isShift and UCHAR _key
// Qt6:   identical layout — no changes needed (shortcut description is portable)
struct ShortcutKey
{
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    unsigned char _key = 0;
};

// =============================================================================
// FuncItem — plugin menu command descriptor
// =============================================================================

// Menu item name buffer size (64 wide chars = 64 UTF-16LE chars = up to 64 ASCII chars)
// Qt6:   keep same limit for binary compatibility with existing plugins
static constexpr int menuItemSize = 64;

// Win32: wchar_t _itemName[menuItemSize]
// Qt6:   QString _itemName — Qt handles Unicode natively; the size limit
//         is advisory only for plugins that use the raw wchar_t array format.
//         For Qt plugins, use QString for proper Unicode support.
struct FuncItem
{
    // Menu item display name (UTF-8 in Qt6, UTF-16LE in Win32)
    QString _itemName;

    // Function pointer to the command handler
    PFUNCPLUGINCMD _pFunc = nullptr;

    // Command ID assigned by Notepad++
    int _cmdID = 0;

    // Whether to check/uncheck this menu item on initial display
    bool _init2Check = false;

    // Keyboard shortcut (optional, may be nullptr)
    ShortcutKey* _pShKey = nullptr;
};

// =============================================================================
// Plugin API functions — each plugin DLL must implement these
// =============================================================================

// setInfo — receive NppData from the host (called immediately after loading)
// Win32: __declspec(dllexport) void __cdecl setInfo(NppData)
// Qt6:   no __declspec needed; function linkage handles export
extern "C" void setInfo(NppData);

// getName — return the plugin's display name (used in Plugins menu)
// Win32: __declspec(dllexport) const wchar_t* __cdecl getName()
// Qt6:  __declspec(dllexport) const char* __cdecl getName()
extern "C" const char* getName();

// getFuncsArray — return the plugin's command array and count
// Win32: __declspec(dllexport) FuncItem* __cdecl getFuncsArray(int* nbItem)
// Qt6:  returns void* (cast to FuncItem* by caller); plugins use FuncItem
extern "C" void* getFuncsArray(int* nbItem);

// beNotified — handle Scintilla notifications (SCN_*) and NPPM_* messages
// Win32: __declspec(dllexport) void __cdecl beNotified(SCNotification*)
// Qt6:  same signature (SCNotification is cross-platform from Scintilla)
extern "C" void beNotified(SCNotification*);

// messageProc — handle NPPM_* custom messages from Notepad++
// Win32: __declspec(dllexport) LRESULT __cdecl messageProc(UINT, WPARAM, LPARAM)
// Qt6:  returns intptr_t (maps LRESULT); message/wParam/lParam are native ints
extern "C" long long messageProc(unsigned int Message, long long wParam, long long lParam);

// isUnicode — return true if the plugin is Unicode-aware (always true in modern NPP)
// Win32: __declspec(dllexport) BOOL __cdecl isUnicode()
// Qt6:  returns bool; all Qt6 plugins are Unicode-capable
extern "C" bool isUnicode();
