// PluginInterface.h - Qt port
#pragma once
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
typedef const wchar_t* (__cdecl * PFUNCGETNAME)();
struct NppData { HWND _nppHandle = nullptr; HWND _scintillaMainHandle = nullptr; HWND _scintillaSecondHandle = nullptr; };
typedef void (__cdecl * PFUNCSETINFO)(NppData);
typedef void (__cdecl * PFUNCPLUGINCMD)();
typedef void (__cdecl * PBENOTIFIED)(void*);
typedef long (__cdecl * PMESSAGEPROC)(unsigned int Message, unsigned long long wParam, long long lParam);
struct ShortcutKey { bool _isCtrl = false; bool _isAlt = false; bool _isShift = false; unsigned char _key = 0; };
const int menuItemSize = 64;
struct FuncItem { wchar_t _itemName[menuItemSize] = { '\0' }; PFUNCPLUGINCMD _pFunc = nullptr; int _cmdID = 0; bool _init2Check = false; ShortcutKey* _pShKey = nullptr; };
typedef FuncItem* (__cdecl * PFUNCGETFUNCSARRAY)(int*);
extern "C" __declspec(dllexport) void setInfo(NppData);
extern "C" __declspec(dllexport) const wchar_t* getName();
extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int*);
extern "C" __declspec(dllexport) void beNotified(void*);
extern "C" __declspec(dllexport) long messageProc(unsigned int Message, unsigned long long wParam, long long lParam);
extern "C" __declspec(dllexport) bool isUnicode();
