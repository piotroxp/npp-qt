#ifndef WINDOWS_STUBS_H
#define WCHAR wchar_t
#define WINDOWS_STUBS_H

// Include WindowsCompat first - it includes Qt headers and all Windows types
#include "WindowsCompat.h"
#include "WindowsCommCtrlStubs.h"

// Additional POSIX/stdlib includes
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <cwchar>

// =============================================================================
// shlwapi.h stubs
// =============================================================================

#define PathFindExtensionA(x) ((char*)strrchr(x, '.'))
inline wchar_t* PathFindExtensionW(wchar_t* x) { return const_cast<wchar_t*>(wcsrchr(x, L'.')); }
inline const wchar_t* PathFindExtensionW(const wchar_t* x) { return wcsrchr(x, L'.'); }
#define PathFindExtension PathFindExtensionW
#define PathFindFileNameA(x) ((char*)(strrchr(x, '/') ? strrchr(x, '/') + 1 : x))
#define PathRemoveExtensionA(x) ((void)0)
#define PathRemoveFileSpecA(x) ((void)0)
#define PathAddBackslashA(x) ((void)0)
#define PathAppendA(x, y) ((void)0)
#define PathIsRelativeA(x) ((x)[0] != '/')
#define PathIsDirectoryA(x) false
#define PathCanonicalizeA(x, y) strcpy(x, y)
#define PathCombineA(x, y, z) ((void)0)

#define PathRemoveArgs(x) ((void)0)
inline BOOL PathCompactPathExW(wchar_t*, const wchar_t*, UINT, DWORD) { return FALSE; }
inline BOOL PathRemoveFileSpecW(wchar_t* path) {
	if (!path) return FALSE;
	wchar_t* slash = wcsrchr(path, L'\\');
	if (!slash) slash = wcsrchr(path, L'/');
	if (slash) { *slash = L'\0'; return TRUE; }
	return FALSE;
}
#define PathRemoveFileSpec PathRemoveFileSpecW
#define PathIsRelative PathIsRelativeW
inline BOOL PathIsRelativeW(const wchar_t* path) {
	if (!path || path[0] == L'\0') return TRUE;
	return path[0] != L'/' && path[1] != L':';
}
#define PathCompactPathEx PathCompactPathExW
inline void PathRemoveExtensionW(wchar_t* pszPath) {
	if (!pszPath) return;
	wchar_t* dot = wcsrchr(pszPath, L'.');
	wchar_t* slash = wcsrchr(pszPath, L'\\');
	if (!slash) slash = wcsrchr(pszPath, L'/');
	if (dot && (!slash || dot > slash)) *dot = L'\0';
}
#define PathRemoveExtension PathRemoveExtensionW
inline void PathStripPathW(wchar_t* pszPath) {
	if (!pszPath) return;
	const wchar_t* name = PathFindFileNameW(pszPath);
	if (name && name != pszPath)
		wcscpy(pszPath, name);
}
#define PathStripPath PathStripPathW

#define PathGetArgs(x) ""

#define SHBrowseForFolderA(x) nullptr
#define SHGetPathFromIDListA(x, y) false
#define SHARD_PATH 0x00000003
typedef void* LPCITEMIDLIST;

static inline int SHOpenFolderAndSelectItems(void* pidl, int cidl, void**, int flags) {
    Q_UNUSED(pidl); Q_UNUSED(cidl); Q_UNUSED(flags);
    return 0;
}

#define StrStrIA(s1,s2) strstr(s1,s2)
#define StrCmpIA(s1,s2) strcasecmp(s1,s2)
#define StrCmpNIA(s1,s2,n) strncmp(s1,s2,n)
#define StrCmpNA(s1,s2,n) strncmp(s1,s2,n)

// =============================================================================
// commctrl.h stubs
// =============================================================================

#ifndef HHOOK
typedef void* HHOOK;
#endif

#define ICC_LISTVIEW_CLASSES 0x00000001
#define ICC_TREEVIEW_CLASSES 0x00000002
#define ICC_TAB_CLASSES 0x00000008

#define LVIF_TEXT 0x0001
#define LVCF_TEXT 0x0004
#define LVIS_FOCUSED 0x0001
#define LVIS_SELECTED 0x0002
#define TVIF_TEXT 0x0001

struct LVCOLUMNW {
	UINT mask;
	int fmt;
	int cx;
	LPWSTR pszText;
	int cchTextMax;
	int iSubItem;
	int iImage;
	int iOrder;
};
typedef LVCOLUMNW LVCOLUMN;

#define ListView_SetColumn(hwnd, iCol, pcol) 0
#define ListView_GetItemCount(hwnd) 0
#define ListView_GetSelectionMark(hwnd) (-1)
#define ListView_SetItemState(hwnd, i, state, mask) 0
#define ListView_EnsureVisible(hwnd, i, partial) 0
#define ListView_SetSelectionMark(hwnd, i) 0
#define ListView_DeleteItem(hwnd, i) 0
#define ListView_DeleteColumn(hwnd, i) 0
#define ListView_GetImageList(hwnd, i) nullptr
#define ListView_SetBkColor(hwnd, c) 0
#define ListView_SetTextBkColor(hwnd, c) 0
#define ListView_SetTextColor(hwnd, c) 0
#define LVSIL_SMALL 1
#define LVM_GETSELECTEDCOUNT (LVM_FIRST + 50)
#ifndef LVM_GETITEMCOUNT
#define LVM_GETITEMCOUNT (LVM_FIRST + 4)
#endif
#define LVCF_WIDTH 0x0001
#define LVIF_PARAM 0x0004
#define LVIS_STATEIMAGEMASK 0xF000
#define INDEXTOSTATEIMAGEMASK(i) ((i) << 12)
#define ListView_InsertColumn(hwnd, i, pcol) 0
#define ListView_InsertItem(hwnd, pitem) 0
#define ListView_SetItemText(hwnd, i, sub, text) TRUE
#define ListView_GetItemText(hwnd, i, sub, buf, len) 0
#define ListView_GetItem(hwnd, pitem) TRUE
#define ListView_GetItemState(hwnd, i, mask) 0
#define ListView_SubItemHitTest(hwnd, p) 0
#ifndef LVM_GETEXTENDEDLISTVIEWSTYLE
#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)
#endif
#define ListView_GetExtendedListViewStyle(hwnd) static_cast<DWORD>(SendMessage(hwnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0))
#define WC_LISTVIEW L"SysListView32"
#define LVS_REPORT 0x0001
#define LVS_SINGLESEL 0x0004
#define LVS_SHOWSELALWAYS 0x0008
#define LVS_NOSORTHEADER 0x8000
#define LVS_AUTOARRANGE 0x0100
#define LVS_SHAREIMAGELISTS 0x0040
#define LVS_EX_FULLROWSELECT 0x00000020
#define LVS_EX_BORDERSELECT 0x00008000
#define LVS_EX_DOUBLEBUFFER 0x00010000
#define ListView_DeleteAllItems(hwnd) SendMessage(hwnd, LVM_DELETEALLITEMS, 0, 0)
#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)
#endif
#define ListView_SetExtendedListViewStyle(hwnd, style) \
	SendMessage(hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, (style), (style))
#define LVS_NOCOLUMNHEADER 0x4000
#define LVS_OWNERDATA 0x1000
#define LVS_OWNERDRAWFIXED 0x0400
#define LVSICF_NOSCROLL 0x0002
#define LVIR_ICON 1
#define LVM_GETSTRINGWIDTH (LVM_FIRST + 17)
#define LVM_SETITEMCOUNT (LVM_FIRST + 47)
#define LVM_SETITEMCOUNTEX (LVM_FIRST + 57)
#define LVM_SETIMAGELIST (LVM_FIRST + 3)
#define LVM_GETITEMRECT (LVM_FIRST + 14)
#define LVM_REDRAWITEMS (LVM_FIRST + 21)
#define ListView_GetStringWidth(hwnd, s) static_cast<int>(SendMessage(hwnd, LVM_GETSTRINGWIDTH, 0, reinterpret_cast<LPARAM>(s)))
#define ListView_SetItemCountEx(hwnd, c, f) SendMessage(hwnd, LVM_SETITEMCOUNTEX, c, f)
#define ListView_SetImageList(hwnd, h, i) SendMessage(hwnd, LVM_SETIMAGELIST, i, reinterpret_cast<LPARAM>(h))
#define ListView_GetItemRect(hwnd, i, prc, code) SendMessage(hwnd, LVM_GETITEMRECT, i, reinterpret_cast<LPARAM>(prc))
#define ListView_RedrawItems(hwnd, f, l) SendMessage(hwnd, LVM_REDRAWITEMS, f, l)
#ifndef LF_FACESIZE
#define LF_FACESIZE 32
#endif
#define CreateWindowW(lpClass, lpName, style, x, y, w, h, parent, menu, inst, param) \
	CreateWindowExW(0, lpClass, lpName, style, x, y, w, h, parent, menu, inst, param)
#define CreateWindow CreateWindowW
#ifndef NM_DBLCLK
#define NM_DBLCLK (0U - 3U)
#endif
#ifndef LVN_KEYDOWN
#define LVN_KEYDOWN (0U - 255U)
#endif
#ifndef LVITEMW_DEFINED
#define LVITEMW_DEFINED
struct LVITEMW {
	UINT mask = 0;
	int iItem = 0;
	int iSubItem = 0;
	UINT state = 0;
	UINT stateMask = 0;
	LPWSTR pszText = nullptr;
	int cchTextMax = 0;
	int iImage = 0;
	int iGroupId = 0;
	LPARAM lParam = 0;
};
typedef LVITEMW LVITEM;
#endif
struct NMITEMACTIVATE {
	NMHDR hdr{};
	int iItem = 0;
	int iSubItem = 0;
	UINT uNewState = 0;
	UINT uOldState = 0;
	UINT uChanged = 0;
	POINT ptAction{};
	LPARAM lParam = 0;
};
typedef NMITEMACTIVATE* LPNMITEMACTIVATE;
struct LVHITTESTINFO {
	POINT pt{};
	UINT flags = 0;
	int iItem = 0;
	int iSubItem = 0;
};
struct NMLVKEYDOWN {
	NMHDR hdr{};
	WORD wVKey = 0;
	UINT flags = 0;
};
typedef NMLVKEYDOWN* LPNMLVKEYDOWN;

#define TVGN_CARET 0x0009
#define TVE_EXPAND 0x0002
#define TVE_COLLAPSE 0x0001
#define TVE_TOGGLE 0x0003
#define TreeView_GetChild(hwnd, item) nullptr
#define TreeView_GetSelection(hwnd) nullptr
#define TreeView_GetRoot(hwnd) nullptr
#define TreeView_GetParent(hwnd, item) nullptr
#define TreeView_GetNextSibling(hwnd, item) nullptr
#define TreeView_GetPrevSibling(hwnd, item) nullptr
#define TreeView_SelectItem(hwnd, item) TRUE
#define TreeView_Expand(hwnd, item, flag) 0
#define TreeView_SetBkColor(hwnd, c) 0
#define TreeView_SetTextColor(hwnd, c) 0

#define NIM_ADD 0x00000000

#ifndef PFNTVCOMPARE_DEFINED
#define PFNTVCOMPARE_DEFINED
typedef int (CALLBACK *PFNTVCOMPARE)(LPARAM, LPARAM, LPARAM);
#endif

struct SCROLLINFO {
	UINT cbSize = 0;
	UINT fMask = 0;
	int nMin = 0;
	int nMax = 0;
	UINT nPage = 0;
	int nPos = 0;
	int nTrackPos = 0;
};

inline COLORREF ColorAdjustLuma(COLORREF, int, int) { return 0; }

// =============================================================================
// shlobj.h stubs
// =============================================================================

#define CSIDL_PROGRAMS 0x0002
#define CSIDL_APPDATA 0x001A
#define CSIDL_LOCAL_APPDATA 0x001C
#define CSIDL_MYDOCUMENTS 0x000C
#define CSIDL_PROFILE 0x0028
#define CSIDL_WINDOWS 0x0024
#define CSIDL_SYSTEM 0x0025

#define SHGetFolderPathA(x, y, z, w, v) -1

// =============================================================================
// tchar.h stubs
// =============================================================================

#define _T(x) x
#define TEXT(x) x
using TBUFFER = char;

// =============================================================================
// dwmapi.h stubs
// =============================================================================

#define DWM_EC_DISABLECOMPOSITION 0

static inline HRESULT DwmEnableBlurBehindWindow(HWND hWnd, void* pBlurBehind) {
    Q_UNUSED(hWnd); Q_UNUSED(pBlurBehind);
    return S_OK;
}

static inline HRESULT DwmGetColorizationColor(DWORD* pcrColorization, BOOL* pfOpaque) {
    Q_UNUSED(pcrColorization); Q_UNUSED(pfOpaque);
    return S_OK;
}

static inline HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, void* Marinset) {
    Q_UNUSED(hWnd); Q_UNUSED(Marinset);
    return S_OK;
}

// =============================================================================
// uxtheme.h stubs
// =============================================================================

typedef void* HTHEME;

static inline HTHEME OpenThemeData(HWND hwnd, const WCHAR* pszClassList) {
    Q_UNUSED(hwnd); Q_UNUSED(pszClassList);
    return nullptr;
}

static inline HRESULT CloseThemeData(HTHEME hTheme) {
    Q_UNUSED(hTheme);
    return S_OK;
}
static inline HRESULT DrawThemeBackground(HTHEME, HDC, int, int, const RECT*, const RECT*) { return S_OK; }
static inline HRESULT GetThemePartSize(HTHEME, HDC, int, int, const RECT*, int, SIZE*) { return S_OK; }
static inline BOOL RectVisible(HDC, const RECT*) { return FALSE; }

static inline BOOL IsAppThemed() { return FALSE; }
static inline BOOL IsThemeActive() { return FALSE; }

#endif // WINDOWS_STUBS_H
