#ifndef WINDOWS_STUBS_H
#define WCHAR wchar_t
#define WINDOWS_STUBS_H

// Include WindowsCompat first - it includes Qt headers and all Windows types
#include "WindowsCompat.h"

// Additional POSIX/stdlib includes
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <cwchar>

// =============================================================================
// shlwapi.h stubs
// =============================================================================

#define PathFindExtensionA(x) ((char*)strrchr(x, '.'))
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

#define ICC_LISTVIEW_CLASSES 0x00000001
#define ICC_TREEVIEW_CLASSES 0x00000002
#define ICC_TAB_CLASSES 0x00000008

#define LVM_FIRST 0x1000
#define TV_FIRST 0x1100

#define LVN_FIRST (-100)
#define TVN_FIRST (-400)

#define LVIF_TEXT 0x0001
#define TVIF_TEXT 0x0001
#define TVGN_CARET 0x0009

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

static inline BOOL IsAppThemed() { return FALSE; }
static inline BOOL IsThemeActive() { return FALSE; }

#endif // WINDOWS_STUBS_H
