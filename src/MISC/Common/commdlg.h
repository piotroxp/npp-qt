#pragma once
#include "WindowsCompat.h"

typedef UINT_PTR (CALLBACK* LPCCHOOKPROC)(HWND, UINT, WPARAM, LPARAM);

#ifndef CC_FULLOPEN
#define CC_FULLOPEN 0x00000002
#define CC_RGBINIT 0x00000001
#define CC_ENABLEHOOK 0x00000010
#endif

struct CHOOSECOLORW {
	DWORD lStructSize = 0;
	HWND hwndOwner = nullptr;
	HWND hInstance = nullptr;
	COLORREF rgbResult = 0;
	COLORREF* lpCustColors = nullptr;
	DWORD Flags = 0;
	LPARAM lCustData = 0;
	LPCCHOOKPROC lpfnHook = nullptr;
	const wchar_t* lpTemplateName = nullptr;
};
typedef CHOOSECOLORW CHOOSECOLOR;

inline BOOL ChooseColorW(CHOOSECOLORW*) { return FALSE; }
#define ChooseColor ChooseColorW
