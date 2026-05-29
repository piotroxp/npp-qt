#pragma once
#include "WindowsCompat.h"

#ifndef WM_ENDSESSION
#define WM_ENDSESSION 0x0016
#define WM_QUERYENDSESSION 0x0011
#define WM_SYNCPAINT 0x0088
#define WM_DROPFILES 0x0233
#endif
#ifndef WM_INITMENUPOPUP
#define WM_INITMENUPOPUP 0x0117
#define WM_ENTERMENULOOP 0x0211
#define WM_EXITMENULOOP 0x0212
#define WM_MENURBUTTONUP 0x0122
#define WM_APPCOMMAND 0x0319
#define WM_COPYDATA 0x004A
#define WM_SYSCOMMAND 0x0118
#define WM_MOUSEWHEEL 0x020A
#define WM_MOUSEHWHEEL 0x020E
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP 0x020C
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
#define GWLP_WNDPROC (-4)
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_APPCOMMAND_LPARAM(lp) HIWORD(lp)
#endif

struct COPYDATASTRUCT {
	ULONG_PTR dwData = 0;
	DWORD cbData = 0;
	void* lpData = nullptr;
};

#ifndef APPCOMMAND_BROWSER_BACKWARD
#define APPCOMMAND_BROWSER_BACKWARD 1
#define APPCOMMAND_BROWSER_FORWARD 2
#endif

#ifndef CDDS_PREPAINT
#define CDDS_PREPAINT 0x00000001
#define CDDS_ITEMPREPAINT 0x00010001
#define CDDS_ITEMPOSTPAINT 0x00010002
#define CDRF_DODEFAULT 0x00000000
#define CDRF_SKIPDEFAULT 0x00000004
#define CDRF_NOTIFYITEMDRAW 0x00000020
#define CDRF_NOTIFYPOSTPAINT 0x00000010
#define CDIS_SELECTED 0x0001
#define CDIS_CHECKED 0x0008
#define CDIS_HOT 0x0040
#endif

#ifndef BTNS_DROPDOWN
#define BTNS_DROPDOWN 0x0008
#endif

#ifndef DT_CENTER
#define DT_CENTER 0x00000001
#define DT_TOP 0x00000000
#define DT_VCENTER 0x00000004
#define DT_SINGLELINE 0x00000020
#define DT_NOPREFIX 0x00000800
#define DT_NOCLIP 0x00000100
#endif

#ifndef ENDSESSION_CRITICAL
#define ENDSESSION_CRITICAL 0x40000000
#endif

#ifndef ERROR_NOT_FOUND
#define ERROR_NOT_FOUND 1168
#define ERROR_OPERATION_ABORTED 995L
#endif

#ifndef ETDT_ENABLETAB
#define ETDT_ENABLETAB 0x00000001
#define ETDT_USETABTEXTURE 0x00000002
#endif

inline int GetClassNameA(HWND, char*, int) { return 0; }
inline HANDLE GetCurrentProcess() { return nullptr; }
inline HWND GetLastActivePopup(HWND h) { return h; }
inline int GetWindowTextA(HWND, char*, int) { return 0; }
inline HMENU LoadMenuW(HINSTANCE, const wchar_t*) { return nullptr; }
#define LoadMenu LoadMenuW
#ifndef NPP_STUB_REGISTERWINDOWMESSAGE
#define NPP_STUB_REGISTERWINDOWMESSAGE
inline UINT RegisterWindowMessageW(const wchar_t*) { return 0xC000; }
#define RegisterWindowMessage RegisterWindowMessageW
#endif
inline BOOL RoundRect(HDC, int, int, int, int, int, int) { return TRUE; }
inline HRESULT RegisterApplicationRestart(const wchar_t*, DWORD) { return S_OK; }
inline HRESULT UnregisterApplicationRestart() { return S_OK; }
inline HRESULT GetApplicationRestartSettings(HANDLE, wchar_t*, DWORD*, DWORD*) { return S_FALSE; }
inline void EnableThemeDialogTexture(HWND, DWORD) {}
inline BOOL strcpy_s(char* d, size_t n, const char* s) {
	if (!d || n == 0) return FALSE;
	strncpy(d, s, n - 1);
	d[n - 1] = '\0';
	return TRUE;
}

#ifndef SC_MINIMIZE
#define SC_MINIMIZE 0xF020
#define SC_MAXIMIZE 0xF030
#define SC_CLOSE 0xF060
#define SC_KEYMENU 0xF100
#define SC_MOUSEMENU 0xF090
#define SC_RESTORE 0xF120
#endif
#ifndef HTCAPTION
#define HTCAPTION 2
#endif
#ifndef WM_NCRBUTTONDOWN
#define WM_NCRBUTTONDOWN 0x00A4
#define WM_LBUTTONDBLCLK 0x0203
#define WM_MBUTTONUP 0x0208
#define WM_RBUTTONUP 0x0205
#define WM_NULL 0x0000
#endif
#ifndef SIZE_RESTORED
#define SIZE_RESTORED 0
#endif
#ifndef NIM_DELETE
#define NIM_DELETE 0x00000002
#define NIM_MODIFY 0x00000001
#endif
#ifndef WM_DRAWCLIPBOARD
#define WM_DRAWCLIPBOARD 0x0308
#define WM_CHANGECBCHAIN 0x030D
#endif
#ifndef CF_TEXT
#define CF_TEXT 1
#define CF_UNICODETEXT 13
#endif
#ifndef LB_ERR
#define LB_ERR (-1)
#define LB_INSERTSTRING 0x0181
#define LB_DELETESTRING 0x0182
#define LB_ADDSTRING 0x0180
#define LB_FINDSTRING 0x018A
#define LB_GETTEXT 0x0189
#define LB_GETTEXTLEN 0x018A
#define LB_RESETCONTENT 0x0187
#define LB_GETITEMDATA 0x0199
#define LB_SETITEMDATA 0x019A
#define LBN_DBLCLK 2
#define LBN_SELCHANGE 1
#endif
#ifndef BST_INDETERMINATE
#define BST_INDETERMINATE 2
#endif
#ifndef CB_FINDSTRING
#define CB_FINDSTRING 0x014C
#endif
#ifndef WM_MEASUREITEM
#define WM_MEASUREITEM 0x002C
#endif
#ifndef ODA_DRAWENTIRE
#define ODA_DRAWENTIRE 0x0001
#define ODA_SELECT 0x0002
#define ODA_FOCUS 0x0004
#define ODS_SELECTED 0x0001
#endif
#ifndef PS_SOLID
#define PS_SOLID 0
#endif
#ifndef COLOR_BTNHIGHLIGHT
#define COLOR_BTNHIGHLIGHT 20
#define COLOR_BTNSHADOW 21
#endif
struct MEASUREITEMSTRUCT {
	UINT CtlType = 0;
	UINT CtlID = 0;
	UINT itemID = 0;
	UINT itemWidth = 0;
	UINT itemHeight = 0;
	ULONG_PTR itemData = 0;
};
typedef MEASUREITEMSTRUCT* LPMEASUREITEMSTRUCT;
#ifndef NPP_STUB_DRAWFOCUSRECT
#define NPP_STUB_DRAWFOCUSRECT
inline BOOL DrawFocusRect(HDC, const RECT*) { return TRUE; }
inline BOOL FrameRect(HDC, const RECT*, HBRUSH) { return TRUE; }
#endif
inline BOOL ChangeClipboardChain(HWND, HWND) { return TRUE; }
inline HWND SetClipboardViewer(HWND) { return nullptr; }

inline wchar_t* lstrcpyW(wchar_t* d, const wchar_t* s) { return wcscpy(d, s); }
#define lstrcpy lstrcpyW
inline int lstrcmpiA(const char* a, const char* b) { return strcasecmp(a, b); }
#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#endif
struct SHELLEXECUTEINFOW {
	DWORD cbSize = 0;
	ULONG fMask = 0;
	HWND hwnd = nullptr;
	const wchar_t* lpVerb = nullptr;
	const wchar_t* lpFile = nullptr;
	const wchar_t* lpParameters = nullptr;
	const wchar_t* lpDirectory = nullptr;
	int nShow = 0;
	HINSTANCE hInstApp = nullptr;
	void* lpIDList = nullptr;
	const wchar_t* lpClass = nullptr;
	HKEY hkeyClass = nullptr;
	DWORD dwHotKey = 0;
	union { HANDLE hIcon; HANDLE hMonitor; } DUMMYUNIONNAME;
	HANDLE hProcess = nullptr;
};
typedef SHELLEXECUTEINFOW SHELLEXECUTEINFO;
#ifndef SEE_MASK_NOCLOSEPROCESS
#define SEE_MASK_NOCLOSEPROCESS 0x00000040
#endif
inline BOOL ShellExecuteExW(SHELLEXECUTEINFO*) { return FALSE; }
inline BOOL GetExitCodeProcess(HANDLE, DWORD*) { return TRUE; }
#ifndef CF_TEXT
#define CF_TEXT 1
#define CF_UNICODETEXT 13
#endif
inline BOOL EmptyClipboard() { return TRUE; }
inline HANDLE SetClipboardData(UINT, HANDLE) { return nullptr; }
inline HGLOBAL GlobalFree(HGLOBAL h) { return h; }

#ifndef DATE_SHORTDATE
#define DATE_SHORTDATE 0x00000001
#define DATE_LONGDATE 0x00000002
#define TIME_NOSECONDS 0x00000002
#endif
inline void GetLocalTime(SYSTEMTIME*) {}
inline int GetDateFormatEx(const wchar_t*, DWORD, const SYSTEMTIME*, const wchar_t*, wchar_t*, int, const wchar_t*) { return 0; }
inline int GetTimeFormatEx(const wchar_t*, DWORD, const SYSTEMTIME*, const wchar_t*, wchar_t*, int) { return 0; }

#ifndef WA_INACTIVE
#define WA_INACTIVE 0
#define WA_ACTIVE 1
#endif
#ifndef WM_ACTIVATEAPP
#define WM_ACTIVATEAPP 0x001C
#endif
#ifndef TRANSPARENT
#define TRANSPARENT 1
#endif
#ifndef GMEM_MOVEABLE
#define GMEM_MOVEABLE 0x0002
#endif
inline HGLOBAL GlobalAlloc(UINT, size_t) { return nullptr; }

#ifndef WM_UNDO
#define WM_UNDO 0x0304
#define WM_CUT 0x0300
#define WM_COPY 0x0301
#define WM_PASTE 0x0302
#endif
#ifndef HRESULT_FROM_WIN32
#define HRESULT_FROM_WIN32(x) static_cast<HRESULT>(x)
#endif
#ifndef RESTART_MAX_CMD_LINE
#define RESTART_MAX_CMD_LINE 1024
#define RESTART_NO_CRASH 0x00000001
#define RESTART_NO_HANG 0x00000002
#define RESTART_NO_PATCH 0x00000004
#endif
typedef void* ITEMIDLIST;
inline HRESULT SHParseDisplayName(const wchar_t*, void*, ITEMIDLIST**, unsigned long, void*) { return E_FAIL; }
inline void CoTaskMemFree(void*) {}
inline UINT RegisterClipboardFormatW(const wchar_t*) { return 0xC000; }
#define RegisterClipboardFormat RegisterClipboardFormatW
#define ShellExecuteEx ShellExecuteExW
#ifndef WM_CREATE
#define WM_CREATE 0x0001
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_CONTEXTMENU 0x007B
#define WM_DRAWITEM 0x002B
#define WM_MOVING 0x0216
#define WM_SIZING 0x0214
#define WM_NCCREATE 0x0081
#define WM_NCACTIVATE 0x0086
#define WM_SETTINGCHANGE 0x001A
#define WM_CLEAR 0x0303
#endif
#ifndef VK_SHIFT
#define VK_SHIFT 0x10
#endif
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG_PTR)0x80000002)
#define KEY_READ 0x20019
#endif
#define RegOpenKeyEx RegOpenKeyExW
#define RegQueryValueEx RegQueryValueExW
typedef BYTE* LPBYTE;
#define PathAppend PathAppendW
inline BOOL PathAppendW(wchar_t*, const wchar_t*) { return TRUE; }
#ifndef NPP_STUB_LSTRCPYN
#define NPP_STUB_LSTRCPYN
inline wchar_t* lstrcpynW(wchar_t* d, const wchar_t* s, int n) {
	if (!d || !s || n <= 0) return d;
	wcsncpy(d, s, static_cast<size_t>(n) - 1);
	d[n - 1] = L'\0';
	return d;
}
#define lstrcpyn lstrcpynW
#endif
inline SIZE_T GlobalSize(HGLOBAL) { return 0; }
inline UINT GetWindowsDirectoryW(wchar_t*, UINT) { return 0; }
#define GetWindowsDirectory GetWindowsDirectoryW
#define _TUCHAR wchar_t
inline int _istspace(wchar_t c) { return iswspace(c); }

#ifndef NMCUSTOMDRAW_DEFINED
#define NMCUSTOMDRAW_DEFINED
struct NMCUSTOMDRAW {
	NMHDR hdr{};
	DWORD dwDrawStage = 0;
	HDC hdc = nullptr;
	RECT rc{};
	DWORD_PTR dwItemSpec = 0;
	UINT uItemState = 0;
	LPARAM lItemlParam = 0;
};
typedef NMCUSTOMDRAW* LPNMCUSTOMDRAW;
#endif

struct NMTBCUSTOMDRAW {
	NMCUSTOMDRAW nmcd{};
	HBRUSH hbrMonoDither = nullptr;
	HBRUSH hbrLines = nullptr;
	HPEN hpenLines = nullptr;
	COLORREF clrText = 0;
	COLORREF clrMark = 0;
	COLORREF clrTextHighlight = 0;
	COLORREF clrBtnFace = 0;
	COLORREF clrBtnHighlight = 0;
	COLORREF clrHighlightHotTrack = 0;
	RECT rcText{};
	int nStringBkMode = 0;
	int nHLStringBkMode = 0;
};
typedef NMTBCUSTOMDRAW* LPNMTBCUSTOMDRAW;

#ifndef NM_FIRST
#define NM_FIRST (0U - 0U)
#endif
#ifndef NM_CUSTOMDRAW
#define NM_CUSTOMDRAW (NM_FIRST - 12)
#define NM_RCLICK (NM_FIRST - 5)
#endif
#ifndef MK_RBUTTON
#define MK_RBUTTON 0x0002
#endif
#ifndef ODT_TAB
#define ODT_TAB 5
#endif
#ifndef RBBIM_STYLE
#define RBBIM_STYLE 0x00000001
#define RBBIM_CHEVRONLOCATION 0x00001000
#define RBBIM_CHEVRONSTATE 0x00002000
#define RBBS_USECHEVRON 0x00000200
#endif
#ifndef RB_GETBANDINFO
#define RB_GETBANDINFO (WM_USER + 5)
#endif
#ifndef TB_GETBUTTONINFO
#define TB_GETBUTTONINFO (WM_USER + 63)
#define TB_COMMANDTOINDEX (WM_USER + 25)
#define TB_GETITEMDROPDOWNRECT (WM_USER + 75)
#define TBIF_STYLE 0x00000001
#define TBN_DROPDOWN (NM_FIRST - 10)
#endif
#ifndef TBCDRF_NOBACKGROUND
#define TBCDRF_NOBACKGROUND 0x00010000
#define TBCDRF_USECDCOLORS 0x00020000
#define TBDDRET_DEFAULT 0
#define TBDDRET_NODEFAULT 1
#endif
#ifndef STATE_SYSTEM_HOTTRACKED
#define STATE_SYSTEM_HOTTRACKED 0x00000080
#define STATE_SYSTEM_PRESSED 0x00000008
#endif
#ifndef TCIF_IMAGE
#define TCIF_IMAGE 0x0002
#define TCIF_PARAM 0x0008
#define TCM_GETITEM (WM_USER + 60)
#define TCM_SETITEM (WM_USER + 61)
#define TCM_SETCURSEL (WM_USER + 12)
#endif
#ifndef TPM_LEFTALIGN
#define TPM_LEFTALIGN 0x0000
#endif

#ifndef NM_CLICK
#define NM_CLICK (NM_FIRST - 2)
#define NM_DBLCLK (NM_FIRST - 3)
#define TCN_SELCHANGE (0xFFFFFDDAu - 1u)
#define TTN_GETDISPINFO (0U - 520U)
#define TTM_SETMAXTIPWIDTH (WM_USER + 24)
#define RBN_CHEVRONPUSHED (RBN_FIRST - 1)
#define RBN_HEIGHTCHANGE (RBN_FIRST - 3)
#define RBBIM_CHILD 0x00000010
#endif
#ifndef RBN_FIRST
#define RBN_FIRST (0U - 831U)
#endif

struct NMMOUSE {
	NMHDR hdr{};
	DWORD_PTR dwItemSpec = 0;
	DWORD_PTR dwItemData = 0;
	POINT pt{};
	LPARAM dwHitInfo = 0;
};
typedef NMMOUSE* LPNMMOUSE;

struct NMREBARCHEVRON {
	NMHDR hdr{};
	UINT uBand = 0;
	UINT wID = 0;
	LPARAM lParam = 0;
	RECT rc = {};
	LPARAM lParamNM = 0;
};

#ifndef TabCtrl_GetItemRect
#define TabCtrl_GetItemRect(hwnd, i, prc) SendMessage(hwnd, TCM_GETITEMRECT, i, reinterpret_cast<LPARAM>(prc))
#define TCM_GETITEMRECT (WM_USER + 10)
#endif

