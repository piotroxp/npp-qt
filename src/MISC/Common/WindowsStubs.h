#ifndef WINDOWS_STUBS_H
#define WINDOWS_STUBS_H

// =============================================================================
// Windows API Stub Headers for Linux Port
// =============================================================================

#include <QtCore>
#include <QString>
#include <QByteArray>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>

#ifdef _MSC_VER
    #include "MISC/Common/WindowsStubs.h"
    #include <commctrl.h>
    #include <shlobj.h>
    #include <tchar.h>
    #include <dwmapi.h>
    #include <uxtheme.h>
#else
    #include <cwchar>

// =============================================================================
// shlwapi.h stubs (path functions)
// =============================================================================

#define PathFindExtensionA(x) strrchr(x, '.')
#define PathFindFileNameA(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x
#define PathRemoveExtensionA(x) do {} while(0)
#define PathRemoveFileSpecA(x) do {} while(0)
#define PathAddBackslashA(x) do {} while(0)
#define PathAppendA(x, y) do {} while(0)
#define PathIsRelativeA(x) ((x)[0] != '/')
#define PathIsDirectoryA(x) false
#define PathCanonicalizeA(x, y) strcpy(x, y)
#define PathCombineA(x, y, z) do {} while(0)

#define PathRemoveArgs(x) do {} while(0)
#define PathGetArgs(x) ""
#define PathParseIconLocationA(x) 0

static inline int StrCmpNA(const char* s1, const char* s2, int n) {
    return strncmp(s1, s2, n);
}
#define StrCmpNIA StrCmpNA

static inline int StrCmpIA(const char* s1, const char* s2) {
    return strcasecmp(s1, s2);
}
#define StrCmpI StrCmpIA

#define StrStrIA strstr
#define StrStrNA strstr

#define SHBrowseForFolderA(x) nullptr
#define SHGetPathFromIDListA(x, y) false
#define SHGetMalloc(x) nullptr

#define SHARD_PATH 0x00000003

typedef void* SHFILEOPSTRUCTA;
typedef void* LPCITEMIDLIST;

static inline int SHOpenFolderAndSelectItems(void* pidl, int cidl, void**, int flags) {
    Q_UNUSED(pidl); Q_UNUSED(cidl); Q_UNUSED(flags);
    return 0;
}

// =============================================================================
// commctrl.h stubs (common controls)
// =============================================================================

#define ICC_LISTVIEW_CLASSES 0x00000001
#define ICC_TREEVIEW_CLASSES 0x00000002
#define ICC_BAR_CLASSES 0x00000004
#define ICC_TAB_CLASSES 0x00000008
#define ICC_ANIMATE_CLASS 0x00000080
#define ICC_DATE_CLASSES 0x00000100

#define LVM_FIRST 0x1000
#define TV_FIRST 0x1100
#define HDM_FIRST 0x1200

#define LVM_GETITEMCOUNT (LVM_FIRST + 4)
#define LVM_GETITEM (LVM_FIRST + 5)
#define LVM_SETITEM (LVM_FIRST + 6)
#define LVM_INSERTITEM (LVM_FIRST + 7)
#define LVM_DELETEITEM (LVM_FIRST + 8)
#define LVM_DELETEALLITEMS (LVM_FIRST + 9)
#define LVM_GETNEXTITEM (LVM_FIRST + 10)
#define LVM_FINDITEM (LVM_FIRST + 13)
#define LVM_ENSUREVISIBLE (LVM_FIRST + 19)
#define LVM_SCROLL (LVM_FIRST + 20)
#define LVM_GETITEMRECT (LVM_FIRST + 10)
#define LVM_GETSUBITEMRECT (LVM_FIRST + 42)
#define LVM_GETCOLUMN (LVM_FIRST + 25)
#define LVM_SETCOLUMN (LVM_FIRST + 26)
#define LVM_INSERTCOLUMN (LVM_FIRST + 27)
#define LVM_DELETECOLUMN (LVM_FIRST + 28)
#define LVM_SETCOLUMNWIDTH (LVM_FIRST + 30)
#define LVM_GETITEMTEXT (LVM_FIRST + 45)
#define LVM_SETITEMTEXT (LVM_FIRST + 46)
#define LVM_SETITEMCOUNT (LVM_FIRST + 47)
#define LVM_SORTITEMS (LVM_FIRST + 48)
#define LVM_GETSELECTEDCOUNT (LVM_FIRST + 50)
#define LVM_GETITEMSTATE (LVM_FIRST + 44)
#define LVM_GETSTRINGWIDTH (LVM_FIRST + 37)

#define TVGN_ROOT 0x0000
#define TVGN_NEXT 0x0001
#define TVGN_PREVIOUS 0x0002
#define TVGN_PARENT 0x0003
#define TVGN_CHILD 0x0004
#define TVGN_CARET 0x0009

#define TVM_SELECTITEM (TV_FIRST + 11)
#define TVM_GETITEM (TV_FIRST + 12)
#define TVM_SETITEM (TV_FIRST + 13)
#define TVM_GETNEXTITEM (TV_FIRST + 10)
#define TVM_INSERTITEM (TV_FIRST + 12)
#define TVM_DELETEITEM (TV_FIRST + 1)
#define TVM_EXPAND (TV_FIRST + 2)
#define TVM_GETITEMRECT (TV_FIRST + 4)
#define TVM_GETINDENT (TV_FIRST + 5)
#define TVM_SETINDENT (TV_FIRST + 6)
#define TVM_GETIMAGELIST (TV_FIRST + 8)
#define TVM_SETIMAGELIST (TV_FIRST + 9)

#define LVIF_TEXT 0x0001
#define LVIF_IMAGE 0x0002
#define LVIF_PARAM 0x0004
#define LVIF_STATE 0x0008
#define LVIF_INDENT 0x0010
#define LVIF_NOPACKETHANDLING 0x80000000

#define TVIF_TEXT 0x0001
#define TVIF_IMAGE 0x0002
#define TVIF_PARAM 0x0004
#define TVIF_STATE 0x0008
#define TVIF_SELECTEDIMAGE 0x0020
#define TVIF_CHILDREN 0x0040

#define TVIS_SELECTED 0x0002
#define TVIS_EXPANDED 0x0020
#define TVIS_STATEIMAGEMASK 0xF000
#define TVIS_BOLD 0x0010

#define TVE_COLLAPSE 0x0001
#define TVE_EXPAND 0x0002
#define TVE_TOGGLE 0x0003

#define LVIS_SELECTED 0x0002
#define LVIS_FOCUSED 0x0001
#define LVIS_STATEIMAGEMASK 0xF000

#define LVN_FIRST (-100)
#define LVN_ITEMCHANGED (LVN_FIRST - 1)
#define LVN_ENDLABELEDIT (LVN_FIRST - 110)
#define LVN_BEGINLABELEDIT (LVN_FIRST - 105)

#define TVN_FIRST (-400)
#define TVN_ITEMEXPANDING (TVN_FIRST - 5)
#define TVN_ITEMEXPANDED (TVN_FIRST - 6)
#define TVN_SELCHANGED (TVN_FIRST - 2)
#define TVN_BEGINLABELEDIT (TVN_FIRST - 3)
#define TVN_ENDLABELEDIT (TVN_FIRST - 4)

#define PBS_SMOOTH 0x01
#define PBS_VERTICAL 0x04

#define PBM_SETPOS (WM_USER + 2)
#define PBM_DELTAPOS (WM_USER + 3)
#define PBM_SETRANGE (WM_USER + 1)
#define PBM_SETSTEP (WM_USER + 4)
#define PBM_STEPIT (WM_USER + 5)
#define PBM_SETBARSTATE (WM_USER + 4)

#define TBMF_BOTTOM 0x00000001

// =============================================================================
// shlobj.h stubs (shell functions)
// =============================================================================

// IShellLink, IPCNetCfg, etc. - just stubs for now
#define CSIDL_PROGRAMS 0x0002
#define CSIDL_COMMON_PROGRAMS 0x0017
#define CSIDL_APPDATA 0x001A
#define CSIDL_LOCAL_APPDATA 0x001C
#define CSIDL_MYDOCUMENTS 0x000C
#define CSIDL_PROFILE 0x0028
#define CSIDL_WINDOWS 0x0024
#define CSIDL_SYSTEM 0x0025
#define CSIDL_PROGRAM_FILES 0x0026
#define CSIDL_PROGRAM_FILES_COMMON 0x002B

#define SHGetFolderPathA(x, y, z, w, v) -1
#define SHGetSpecialFolderPathA(x, y, z, w) false

// =============================================================================
// tchar.h stubs
// =============================================================================

#ifdef UNICODE
using TBUFFER = wchar_t;
#else
using TBUFFER = char;
#endif

#ifdef UNICODE
#define _T(x) L##x
#define _TEXT(x) L##x
#define TEXT(x) L##x
#else
#define _T(x) x
#define _TEXT(x) x
#define TEXT(x) x
#endif

//================================================================================
// dwmapi.h stubs (desktop window management)
//================================================================================

#define DWM_EC_DISABLECOMPOSITION 0
#define DWM_EC_ENABLECOMPOSITION 1

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

//================================================================================
// uxtheme.h stubs
//================================================================================

typedef void* HTHEME;

#define TMT_FILLLOL 3601
#define TMT_FILLCOLORHINT 3802
#define TMT_TRANSPARENT 3803
#define TMT_BORDERSIZE 3804

static inline HTHEME OpenThemeData(HWND hwnd, const WCHAR* pszClassList) {
    Q_UNUSED(hwnd); Q_UNUSED(pszClassList);
    return nullptr;
}

static inline HRESULT CloseThemeData(HTHEME hTheme) {
    Q_UNUSED(hTheme);
    return S_OK;
}

static inline HRESULT GetThemeColor(HTHEME hTheme, int iPartId, int iStateId, 
                                    int iColorId, COLORREF* pColor) {
    Q_UNUSED(hTheme); Q_UNUSED(iPartId); Q_UNUSED(iStateId); Q_UNUSED(iColorId);
    if (pColor) *pColor = 0;
    return S_OK;
}

static inline BOOL IsAppThemed() { return FALSE; }
static inline BOOL IsThemeActive() { return FALSE; }

// =============================================================================
// More stubs
// =============================================================================

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)
#define E_FAIL ((HRESULT)0x80000008L)
#define E_NOTIMPL ((HRESULT)0x80000001L)
#define E_OUTOFMEMORY ((HRESULT)0x80000002L)
#define E_INVALIDARG ((HRESULT)0x80000003L)
#define E_POINTER ((HRESULT)0x80000004L)

typedef long HRESULT;
typedef unsigned long COLORREF;
typedef long LPARAM;
typedef unsigned long WPARAM;
typedef long LRESULT;

#endif

#endif // WINDOWS_STUBS_H
