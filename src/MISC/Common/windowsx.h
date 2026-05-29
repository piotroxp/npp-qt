#pragma once
#include "WindowsCompat.h"
#include "WindowsMessageStubs.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_WM_COMMAND_ID(wp, lp) LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp) (HWND)(lp)
#define GET_WM_COMMAND_CMD(wp, lp) HIWORD(wp)
#define Button_GetCheck(hwnd) ((int)SendMessage(hwnd, BM_GETCHECK, 0, 0))
#define Button_SetCheck(hwnd, check) SendMessage(hwnd, BM_SETCHECK, check, 0)
#define ComboBox_GetCurSel(hwnd) ((int)SendMessage(hwnd, CB_GETCURSEL, 0, 0))
#define ComboBox_SetCurSel(hwnd, index) SendMessage(hwnd, CB_SETCURSEL, index, 0)
#ifndef GET_KEYSTATE_WPARAM
#define GET_KEYSTATE_WPARAM(w) LOWORD(w)
#define GET_WHEEL_DELTA_WPARAM(w) ((short)HIWORD(w))
#define GET_WM_COMMAND_ID(wp, lp) LOWORD(wp)
#endif
#ifndef MK_SHIFT
#define MK_SHIFT 0x0004
#define MK_RBUTTON 0x0002
#endif
#ifndef SB_LINELEFT
#define SB_LINELEFT 0
#define SB_LINERIGHT 1
#endif
#ifndef VK_CONTROL
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_SHIFT 0x10
#endif
#ifndef WS_EX_RTLREADING
#define WS_EX_RTLREADING 0x00002000
#define WS_EX_CLIENTEDGE 0x00000200
#define WS_BORDER 0x00800000
#endif
#ifndef WM_KEYUP
#define WM_KEYUP 0x0101
#define WM_TIMER 0x0113
#define WM_IME_REQUEST 0x0288
#endif
#ifndef IMR_RECONVERTSTRING
#define IMR_RECONVERTSTRING 0x0004
#endif
struct POINTS { SHORT x = 0; SHORT y = 0; };
inline void POINTSTOPOINT(POINT& pt, POINTS pts) { pt.x = pts.x; pt.y = pts.y; }
struct RECONVERTSTRING {
	DWORD dwSize = 0;
	DWORD dwVersion = 0;
	DWORD dwStrLen = 0;
	DWORD dwStrOffset = 0;
	DWORD dwCompStrLen = 0;
	DWORD dwCompStrOffset = 0;
	DWORD dwTargetStrLen = 0;
	DWORD dwTargetStrOffset = 0;
};
inline bool Scintilla_RegisterClasses(void*) { return true; }
#define MAKEPOINTS(l) (*((POINTS*)&(l)))
inline wchar_t* CharUpperW(wchar_t* s) { return s; }
inline wchar_t* CharLowerW(wchar_t* s) { return s; }
inline errno_t memcpy_s(void* d, size_t dn, const void* s, size_t n) { memcpy(d, s, n < dn ? n : dn); return 0; }
#define itoa(v, buf, base) snprintf(buf, 32, "%d", v)
inline int vswprintf_s(wchar_t* buf, size_t n, const wchar_t* fmt, va_list ap) {
	return vswprintf(buf, n, fmt, ap);
}
inline int swprintf_s(wchar_t* buf, size_t n, const wchar_t* fmt, ...) {
	va_list ap; va_start(ap, fmt); const int r = vswprintf_s(buf, n, fmt, ap); va_end(ap); return r;
}
template<size_t N>
inline int swprintf_s(wchar_t (&buf)[N], const wchar_t* fmt, ...) {
	va_list ap; va_start(ap, fmt); const int r = vswprintf_s(buf, N, fmt, ap); va_end(ap); return r;
}
inline HWND FindWindowExW(HWND, HWND, const wchar_t*, const wchar_t*) { return nullptr; }
#define FindWindowEx FindWindowExW
#define TOOLTIPS_CLASS L"tooltips_class32"
inline UINT_PTR SetTimer(HWND, UINT_PTR, UINT, void*) { return 0; }
inline BOOL KillTimer(HWND, UINT_PTR) { return TRUE; }
#ifndef NPP_STUB_SETDLGITEMINT
#define NPP_STUB_SETDLGITEMINT
inline BOOL SetDlgItemInt(HWND, int, UINT, BOOL) { return TRUE; }
#endif
struct EDITBALLOONTIP { DWORD cbStruct = 0; LPCWSTR pszTitle = nullptr; LPCWSTR pszText = nullptr; INT ttiIcon = 0; };
#define EM_SHOWBALLOONTIP (WM_USER + 150)
#define EM_HIDEBALLOONTIP (WM_USER + 151)
#define TTI_ERROR_LARGE 2
