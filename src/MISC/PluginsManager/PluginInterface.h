// PluginInterface.h - Qt port
#pragma once
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include <QWidget>
#include "WindowsCompat.h"

// Function pointer types - Qt6/WinForms compatibility
typedef const wchar_t* (__cdecl * PFUNCGETNAME)();
typedef void (__cdecl * PFUNCSETINFO)(NppData*);
typedef void (__cdecl * PFUNCPLUGINCMD)();
typedef void (__cdecl * PBENOTIFIED)(const void*);
typedef long (__cdecl * PMESSAGEPROC)(unsigned int, unsigned long long, long long);
typedef FuncItem* (__cdecl * PFUNCGETFUNCSARRAY)(int*);

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

// Plugin entry points (implemented by plugins)
extern "C" {
    __declspec(dllexport) void setInfo(NppData);
    __declspec(dllexport) const wchar_t* getName();
    __declspec(dllexport) FuncItem* getFuncsArray(int*);
    __declspec(dllexport) void beNotified(const void*);
    __declspec(dllexport) long messageProc(unsigned int, unsigned long long, long long);
    __declspec(dllexport) bool isUnicode();
}
// Shortcut key structure
struct ShortcutKey {
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    unsigned char _key = 0;
};

// FuncItem - plugin menu item
const int menuItemSize = 64;
struct FuncItem {
    wchar_t _itemName[menuItemSize] = { '\0' };
    PFUNCPLUGINCMD _pFunc = nullptr;
    int _cmdID = 0;
    bool _init2Check = false;
    ShortcutKey* _pShKey = nullptr;
};

// Exported plugin functions
    __declspec(dllexport) void setInfo(NppData);
    __declspec(dllexport) const wchar_t* getName();
    __declspec(dllexport) FuncItem* getFuncsArray(int*);
    __declspec(dllexport) void beNotified(const void*);
    __declspec(dllexport) long messageProc(unsigned int, unsigned long long, long long);
    __declspec(dllexport) bool isUnicode();
}
