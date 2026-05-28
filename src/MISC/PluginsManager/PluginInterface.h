// This file is part of Notepad++ project
// Copyright (C)2025 Don HO <don.h@free.fr>

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



// For more comprehensive information on plugin communication, please refer to the following resource:
// https://npp-user-manual.org/docs/plugin-communication/

#pragma once

#include "scintilla/include/Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "MISC/Common/WindowsCompat.h"
#include <cstdint>

// Stub Windows calling conventions (no-op on Linux)
#define __cdecl
#define __declspec(x)

#ifndef UCHAR
#define UCHAR unsigned char
#endif

struct NppData
{
	HWND _nppHandle = nullptr;
	HWND _scintillaMainHandle = nullptr;
	HWND _scintillaSecondHandle = nullptr;
};

typedef const wchar_t* (*PFUNCGETNAME)();
typedef void (*PFUNCSETINFO)(NppData);
typedef void (*PFUNCPLUGINCMD)();
typedef void (*PBENOTIFIED)(SCNotification*);
typedef long (*PMESSAGEPROC)(unsigned int, unsigned long, long);
typedef FuncItem* (*PFUNCGETFUNCSARRAY)(int*);
typedef bool (*PFUNCISUNICODE)();

struct ShortcutKey
{
	bool _isCtrl = false;
	bool _isAlt = false;
	bool _isShift = false;
	UCHAR _key = 0;
};

const int menuItemSize = 64;

struct FuncItem
{
	wchar_t _itemName[menuItemSize] = { '\0' };
	PFUNCPLUGINCMD _pFunc = nullptr;
	int _cmdID = 0;
	bool _init2Check = false;
	ShortcutKey *_pShKey = nullptr;
};

// Plugin API functions (stub for Linux)
extern "C" __declspec(dllexport) void setInfo(NppData);
extern "C" __declspec(dllexport) const wchar_t* getName();
extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int*);
extern "C" __declspec(dllexport) void beNotified(SCNotification*);
extern "C" __declspec(dllexport) long messageProc(unsigned int Message, unsigned long wParam, long lParam);
extern "C" __declspec(dllexport) BOOL isUnicode();
