// PluginInterface.h - Qt port (Linux-compatible)
#pragma once
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include <QWidget>

// Calling convention macro - empty on non-Windows
#ifdef _WIN32
#define NPP_CDECL __cdecl
#else
#define NPP_CDECL
#endif

// Forward declarations for circular dependencies
struct NppData;
struct ShortcutKey;
struct FuncItem;

// Function pointer types
typedef const wchar_t* (NPP_CDECL * PFUNCGETNAME)();
typedef void (NPP_CDECL * PFUNCSETINFO)(struct NppData*);
typedef void (NPP_CDECL * PFUNCPLUGINCMD)();
typedef void (NPP_CDECL * PBENOTIFIED)(const void*);
typedef long (NPP_CDECL * PMESSAGEPROC)(unsigned int, unsigned long long, long long);
typedef FuncItem* (NPP_CDECL * PFUNCGETFUNCSARRAY)(int*);
typedef bool (NPP_CDECL * PFUNCISUNICODE)();

// NppData - data passed to plugins from Notepad++
struct NppData {
    HWND _nppHandle = nullptr;
    HWND _scintillaMainHandle = nullptr;
    HWND _scintillaSecondHandle = nullptr;
};

// Shortcut key structure for plugin menu items
struct ShortcutKey {
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    unsigned char _key = 0;
};

// FuncItem - each plugin menu item
const int menuItemSize = 64;
struct FuncItem {
    wchar_t _itemName[menuItemSize] = { '\0' };
    PFUNCPLUGINCMD _pFunc = nullptr;
    int _cmdID = 0;
    bool _init2Check = false;
    ShortcutKey* _pShKey = nullptr;
};

// HMENUFunctions - Windows menu handle function pointer (placeholder)
typedef intptr_t HMENUFunctions;
