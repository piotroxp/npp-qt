#ifndef WINDOWS_COMPAT_H
#define WINDOWS_COMPAT_H

// =============================================================================
// Qt headers - must come BEFORE any typedefs using qint64/quint64
// =============================================================================
#include <QtCore>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <cstdint>

// =============================================================================
// Basic Windows types
// =============================================================================
#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned int DWORD;
#endif

#ifndef INT
typedef int INT;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef LONG
typedef long LONG;
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef WPARAM
typedef unsigned long WPARAM;
#endif

#ifndef LPARAM
typedef long LPARAM;
#endif

#ifndef LRESULT
typedef long LRESULT;
#endif

#ifndef HRESULT
typedef long HRESULT;
#endif

#ifndef COLORREF
typedef unsigned long COLORREF;
#endif

#ifndef ATOM
typedef unsigned long ATOM;
#endif

// Pointer-sized types
#ifndef INT_PTR
typedef intptr_t INT_PTR;
#endif

#ifndef UINT_PTR
typedef uintptr_t UINT_PTR;
#endif

#ifndef LONG_PTR
typedef long LONG_PTR;
#endif

// 8-byte integers
#ifndef QINT64
typedef long long QINT64;
#endif

#ifndef QUINT64
typedef unsigned long long QUINT64;
#endif

// =============================================================================
// Handle types
// =============================================================================
#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifndef HWND
typedef void* HWND;
#endif

#ifndef HDC
typedef void* HDC;
#endif

#ifndef HGDIOBJ
typedef void* HGDIOBJ;
#endif

#ifndef HBRUSH
typedef void* HBRUSH;
#endif

#ifndef HPEN
typedef void* HPEN;
#endif

#ifndef HFONT
typedef void* HFONT;
#endif

#ifndef HBITMAP
typedef void* HBITMAP;
#endif

#ifndef HCURSOR
typedef void* HCURSOR;
#endif

#ifndef HICON
typedef void* HICON;
#endif

#ifndef HMENU
typedef void* HMENU;
#endif

#ifndef HRGN
typedef void* HRGN;
#endif

#ifndef HMODULE
typedef void* HMODULE;
#endif

#ifndef HINSTANCE
typedef void* HINSTANCE;
#endif

#ifndef HLOCAL
typedef void* HLOCAL;
#endif

#ifndef HGLOBAL
typedef void* HGLOBAL;
#endif

#ifndef HACCEL
typedef void* HACCEL;
#endif

#ifndef HDWP
typedef void* HDWP;
#endif

#ifndef HPRINTER
typedef void* HPRINTER;
#endif

#ifndef HCOLORSPACE
typedef void* HCOLORSPACE;
#endif

#ifndef HIMAGELIST
typedef void* HIMAGELIST;
#endif

#ifndef HHOOK
typedef void* HHOOK;
#endif

#ifndef FARPROC
typedef void* FARPROC;
#endif

#ifndef HRSRC
typedef void* HRSRC;
#endif

#ifndef HTHEME
typedef void* HTHEME;
#endif

#ifndef HMONITOR
typedef void* HMONITOR;
#endif

#ifndef HTREEITEM
typedef void* HTREEITEM;
#endif

#ifndef HDROP
typedef void* HDROP;
#endif

#ifndef HKEY
typedef void* HKEY;
#endif

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef LPCVOID
typedef const void* LPCVOID;
#endif

// =============================================================================
// String types
// =============================================================================
#ifndef LPSTR
typedef char* LPSTR;
#endif

#ifndef LPCSTR
typedef const char* LPCSTR;
#endif

#ifndef LPWSTR
typedef wchar_t* LPWSTR;
#endif

#ifndef LPCWSTR
typedef const wchar_t* LPCWSTR;
#endif

#ifndef LPTSTR
typedef wchar_t* LPTSTR;
#endif

#ifndef LPCTSTR
typedef const wchar_t* LPCTSTR;
#endif

#ifndef TCHAR
typedef wchar_t TCHAR;
#endif

// =============================================================================
// Qt types aliased to Windows names (for code using Windows-style POINT/SIZE/RECT)
// =============================================================================
typedef QPoint                  POINT;
typedef QSize                   SIZE;
typedef POINT*                  PPOINT;
typedef SIZE*                    PSIZE;
struct RECT { long left, top, right, bottom; };
typedef RECT*                   PRECT;
typedef RECT*                   LPRECT;
typedef const RECT*             LPCRECT;

// =============================================================================
// Windows Structures
// =============================================================================
#ifndef _FILETIME_DEFINED
#define _FILETIME_DEFINED
struct FILETIME {
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
};
#endif

#ifndef _SYSTEMTIME_DEFINED
#define _SYSTEMTIME_DEFINED
struct SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
};
#endif

#ifndef _ULARGE_INTEGER_DEFINED
#define _ULARGE_INTEGER_DEFINED
union ULARGE_INTEGER {
    struct { unsigned long LowPart; unsigned long HighPart; } u;
    unsigned long long QuadPart;
};
#endif

#ifndef _MSG_DEFINED
#define _MSG_DEFINED
struct MSG {
    HWND       hwnd;
    UINT       message;
    WPARAM     wParam;
    LPARAM     lParam;
    unsigned long time;
    POINT      pt;
};
#endif

#ifndef _NMHDR_DEFINED
#define _NMHDR_DEFINED
struct NMHDR {
    void* hwndFrom;
    UINT  idFrom;
    UINT  code;
};
#endif

#ifndef _TOOLINFO_DEFINED
#define _TOOLINFO_DEFINED
struct TOOLINFO {
    unsigned int cbSize;
    unsigned int uFlags;
    HWND        hwndOwner;
    unsigned int uId;
    RECT        rect;
    HINSTANCE  lpReserved;
    wchar_t*    lpszText;
    LPARAM     lParam;
    void*      hinst;
};
#endif

#ifndef _WIN32_FILE_ATTRIBUTE_DATA_DEFINED
#define _WIN32_FILE_ATTRIBUTE_DATA_DEFINED
struct WIN32_FILE_ATTRIBUTE_DATA {
    DWORD         dwFileAttributes;
    FILETIME      ftCreationTime;
    FILETIME      ftLastAccessTime;
    FILETIME      ftLastWriteTime;
    ULARGE_INTEGER nFileSize;
};
#endif

#ifndef _WINDOWPLACEMENT_DEFINED
#define _WINDOWPLACEMENT_DEFINED
struct WINDOWPLACEMENT {
    UINT  length;
    UINT  flags;
    UINT  showCmd;
    long  ptMinPosition[2];
    long  ptMaxPosition[2];
    long  rcNormalPosition[4];
};
#endif

// =============================================================================
// COM / GUID types
// =============================================================================
#ifndef _GUID_DEFINED
#define _GUID_DEFINED
typedef struct { unsigned long Data1; unsigned short Data2; unsigned short Data3; unsigned char Data4[8]; } GUID;
#endif

#ifndef IID
typedef GUID IID;
#endif

#ifndef CLSID
typedef GUID CLSID;
#endif

// =============================================================================
// Windows Constants
// =============================================================================
#ifndef S_OK
#define S_OK                  ((HRESULT)0L)
#endif

#ifndef S_FALSE
#define S_FALSE               ((HRESULT)1L)
#endif

#ifndef E_FAIL
#define E_FAIL                ((HRESULT)0x80000008L)
#endif

#ifndef E_NOTIMPL
#define E_NOTIMPL             ((HRESULT)0x80000001L)
#endif

#ifndef E_NOINTERFACE
#define E_NOINTERFACE         ((HRESULT)0x80004002L)
#endif

#ifndef NO_ERROR
#define NO_ERROR              0
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS        0
#endif

#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND  2
#endif

#ifndef TRUE
#define TRUE                  1
#endif

#ifndef FALSE
#define FALSE                 0
#endif

#ifndef MAX_PATH
#define MAX_PATH              260
#endif

// Message box styles
#ifndef MB_OK
#define MB_OK                 0x00000000L
#endif

#ifndef MB_OKCANCEL
#define MB_OKCANCEL           0x00000001L
#endif

#ifndef MB_YESNO
#define MB_YESNO              0x00000004L
#endif

#ifndef MB_ICONINFORMATION
#define MB_ICONINFORMATION    0x00000040L
#endif

#ifndef MB_ICONSTOP
#define MB_ICONSTOP           0x00000010L
#endif

// Window messages
#ifndef WM_COMMAND
#define WM_COMMAND            0x0111
#endif

#ifndef WM_NOTIFY
#define WM_NOTIFY             0x004E
#endif

#ifndef WM_CLOSE
#define WM_CLOSE              0x0010
#endif

#ifndef WM_DESTROY
#define WM_DESTROY            0x0002
#endif

#ifndef WM_SIZE
#define WM_SIZE               0x0005
#endif

#ifndef WM_PAINT
#define WM_PAINT              0x000F
#endif

#ifndef WM_ERASEBKGND
#define WM_ERASEBKGND         0x0014
#endif

#ifndef WM_LBUTTONDOWN
#define WM_LBUTTONDOWN        0x0201
#endif

#ifndef WM_LBUTTONUP
#define WM_LBUTTONUP          0x0202
#endif

#ifndef WM_RBUTTONDOWN
#define WM_RBUTTONDOWN        0x0204
#endif

#ifndef WM_MOUSEMOVE
#define WM_MOUSEMOVE          0x0200
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL         0x020A
#endif

#ifndef WM_USER
#define WM_USER               0x0400
#endif

// File attributes
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL       0x00000080
#endif

#ifndef FILE_ATTRIBUTE_HIDDEN
#define FILE_ATTRIBUTE_HIDDEN       0x00000002
#endif

#ifndef FILE_ATTRIBUTE_SYSTEM
#define FILE_ATTRIBUTE_SYSTEM       0x00000004
#endif

#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY     0x00000001
#endif

#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY     0x00000010
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES     0xFFFFFFFF
#endif

// GENERIC constants
#ifndef GENERIC_READ
#define GENERIC_READ         0x80000000
#endif

#ifndef GENERIC_WRITE
#define GENERIC_WRITE        0x40000000
#endif

#ifndef GENERIC_EXECUTE
#define GENERIC_EXECUTE      0x20000000
#endif

#ifndef GENERIC_ALL
#define GENERIC_ALL          0x10000000
#endif

// System metrics
#ifndef SM_CXSMICON
#define SM_CXSMICON          49
#endif

#ifndef SM_CYSMICON
#define SM_CYSMICON          50
#endif

// Code page
#ifndef CP_UTF8
#define CP_UTF8              65001
#endif

// File move flags
#ifndef MOVEFILE_COPY_ALLOWED
#define MOVEFILE_COPY_ALLOWED  0x00000002
#endif

#ifndef MOVEFILE_DELAY_IF_BUSY
#define MOVEFILE_DELAY_IF_BUSY 0x00000010
#endif

// Scroll bar
#ifndef SB_VERT
#define SB_VERT              0x0001
#endif

#ifndef SB_THUMBPOSITION
#define SB_THUMBPOSITION     4
#endif

#ifndef SB_THUMBTRACK
#define SB_THUMBTRACK        5
#endif

// Show window commands
#ifndef SW_HIDE
#define SW_HIDE              0x0000
#endif

#ifndef SW_SHOW
#define SW_SHOW               0x0005
#endif

#ifndef SW_SHOWMAXIMIZED
#define SW_SHOWMAXIMIZED      3
#endif

// SetWindowPos flags
#ifndef SWP_NOZORDER
#define SWP_NOZORDER         0x0004
#endif

#ifndef SWP_NOSIZE
#define SWP_NOSIZE           0x0001
#endif

#ifndef SWP_NOMOVE
#define SWP_NOMOVE           0x0002
#endif

#ifndef SWP_SHOWWINDOW
#define SWP_SHOWWINDOW       0x0040
#endif

// Window styles
#ifndef WS_VISIBLE
#define WS_VISIBLE           0x10000000L
#endif

#ifndef WS_DISABLED
#define WS_DISABLED          0x08000000L
#endif

#ifndef WS_CAPTION
#define WS_CAPTION           0x00C00000L
#endif

#ifndef WS_THICKFRAME
#define WS_THICKFRAME         0x00040000L
#endif

#ifndef WS_HSCROLL
#define WS_HSCROLL            0x00100000L
#endif

#ifndef WS_VSCROLL
#define WS_VSCROLL           0x00200000L
#endif

#ifndef WS_OVERLAPPED
#define WS_OVERLAPPED        0x00000000L
#endif

#ifndef WS_CLIPCHILDREN
#define WS_CLIPCHILDREN      0x02000000L
#endif

#ifndef WS_MINIMIZEBOX
#define WS_MINIMIZEBOX       0x00020000L
#endif

#ifndef WS_MAXIMIZEBOX
#define WS_MAXIMIZEBOX        0x00010000L
#endif

// GetWindowLong offsets
#ifndef GWL_STYLE
#define GWL_STYLE            (-16)
#endif

#ifndef GWL_WNDPROC
#define GWL_WNDPROC          (-4)
#endif

// Menu item flags
#ifndef MF_BYCOMMAND
#define MF_BYCOMMAND         0x00000000
#endif

#ifndef MF_CHECKED
#define MF_CHECKED           0x00000008
#endif

#ifndef MF_UNCHECKED
#define MF_UNCHECKED        0x00000000
#endif

#ifndef MF_ENABLED
#define MF_ENABLED          0x00000000
#endif

#ifndef MF_GRAYED
#define MF_GRAYED           0x00000001
#endif

#ifndef MF_POPUP
#define MF_POPUP            0x00000010
#endif

#ifndef MF_SEPARATOR
#define MF_SEPARATOR        0x00000800
#endif

// TreeView messages
#ifndef TVM_GETITEM
#define TVM_GETITEM          0x100D
#endif

#ifndef TVM_SETITEM
#define TVM_SETITEM          0x100E
#endif

#ifndef TVM_DELETEITEM
#define TVM_DELETEITEM       0x1009
#endif

// TreeView notifications
#ifndef TVN_SELCHANGED
#define TVN_SELCHANGED       0xFFFFFD02
#endif

#ifndef TVN_BEGINDRAG
#define TVN_BEGINDRAG        0xFFFFFD03
#endif

#ifndef TVN_ROOT
#define TVN_ROOT            0x0000
#endif

#ifndef TVN_CARET
#define TVN_CARET           0x0009
#endif

// ListView messages
#ifndef LVM_GETITEMCOUNT
#define LVM_GETITEMCOUNT     0x1004
#endif

#ifndef LVM_GETITEM
#define LVM_GETITEM          0x1005
#endif

#ifndef LVM_SETITEM
#define LVM_SETITEM          0x1006
#endif

#ifndef LVM_INSERTITEM
#define LVM_INSERTITEM        0x1007
#endif

#ifndef LVM_DELETEITEM
#define LVM_DELETEITEM       0x1008
#endif

#ifndef LVM_DELETEALLITEMS
#define LVM_DELETEALLITEMS  0x1009
#endif

#ifndef LVM_GETITEMTEXT
#define LVM_GETITEMTEXT      0x102D
#endif

#ifndef LVM_SETITEMTEXT
#define LVM_SETITEMTEXT      0x102E
#endif

// ComboBox messages
#ifndef CB_GETCOUNT
#define CB_GETCOUNT          0x0147
#endif

#ifndef CB_GETLBTEXT
#define CB_GETLBTEXT         0x0148
#endif

#ifndef CB_GETLBTEXTLEN
#define CB_GETLBTEXTLEN      0x0149
#endif

#ifndef CB_GETCURSEL
#define CB_GETCURSEL         0x0140
#endif

#ifndef CB_SETCURSEL
#define CB_SETCURSEL         0x014E
#endif

#ifndef CB_ADDSTRING
#define CB_ADDSTRING         0x0143
#endif

#ifndef CB_RESETCONTENT
#define CB_RESETCONTENT      0x014B
#endif

// Button notifications
#ifndef BN_CLICKED
#define BN_CLICKED           0x00F0
#endif

// GDI / Image constants
#ifndef IMAGE_ICON
#define IMAGE_ICON           1
#endif

#ifndef IMAGE_BITMAP
#define IMAGE_BITMAP         0
#endif

#ifndef IMAGE_CURSOR
#define IMAGE_CURSOR         2
#endif

#ifndef LR_LOADFROMFILE
#define LR_LOADFROMFILE      0x00000010
#endif

#ifndef LR_SHARED
#define LR_SHARED            0x00000008
#endif

#ifndef LR_DEFAULTSIZE
#define LR_DEFAULTSIZE       0x00000040
#endif

// Helper macros - NOTE: Already using inline functions below instead of macros
#ifndef SUCCEEDED
#define SUCCEEDED(hr)       (((HRESULT)(hr)) >= 0)
#endif

// Stub type aliases for toolbar (compatibility with original code)
struct tagNMTOOLBARW { int unused; };
#ifndef LPNMTOOLBARW
#define LPNMTOOLBARW tagNMTOOLBARW*
#endif

// =============================================================================
// Globals for dark mode (always false on Linux)
// =============================================================================
#ifndef g_darkModeSupported
#define g_darkModeSupported (*(bool*)nullptr)
#endif

#ifndef g_darkModeEnabled
#define g_darkModeEnabled (*(bool*)nullptr)
#endif

// =============================================================================
// No-op inline function stubs for Windows API
// =============================================================================

// COM stubs
inline void CoInitializeEx(void*, unsigned long) {}
inline void CoUninitialize() {}

// Message box stubs
inline int MessageBoxW(void*, const wchar_t*, const wchar_t*, unsigned int) { return 0; }
inline int MessageBox(void*, const wchar_t*, const wchar_t*, unsigned int) { return 0; }
inline BOOL DestroyMenu(HMENU) { return TRUE; }
inline BOOL DestroyIcon(HICON) { return TRUE; }
inline BOOL DestroyWindow(HWND) { return TRUE; }

// Library stubs
inline BOOL FreeLibrary(HMODULE) { return TRUE; }
inline FARPROC GetProcAddress(HMODULE, const char*) { return nullptr; }
inline HMODULE LoadLibraryW(const wchar_t*) { return nullptr; }

// Image / icon stubs
inline HICON LoadImage(HINSTANCE, const wchar_t*, unsigned int, int, int, unsigned int) { return nullptr; }
inline HICON CreateIconFromResource(unsigned char*, DWORD, BOOL, DWORD) { return nullptr; }

// GDI stubs
inline HBRUSH CreateSolidBrush(COLORREF) { return nullptr; }
inline HDC GetDC(HWND) { return nullptr; }
inline int ReleaseDC(HWND, HDC) { return 0; }
inline BOOL DeleteDC(HDC) { return TRUE; }
inline BOOL DeleteObject(HGDIOBJ) { return TRUE; }
inline HDC BeginPaint(HWND, void*) { return nullptr; }
inline BOOL EndPaint(HWND, void*) { return TRUE; }

// System / window stubs
inline int GetSystemMetrics(int) { return 32; }
inline LONG GetWindowLongW(HWND, int) { return 0; }
inline LONG SetWindowLongW(HWND, int, LONG) { return 0; }
inline BOOL SetWindowPos(HWND, HWND, int, int, int, int, UINT) { return TRUE; }
inline BOOL ShowWindow(HWND, int) { return TRUE; }
inline BOOL UpdateWindow(HWND) { return TRUE; }
inline BOOL InvalidateRect(HWND, const RECT*, BOOL) { return TRUE; }
inline BOOL RedrawWindow(HWND, const RECT*, HRGN, UINT) { return TRUE; }
inline BOOL GetWindowRect(HWND, RECT*) { return TRUE; }
inline BOOL SetForegroundWindow(HWND) { return TRUE; }
inline HWND SetCapture(HWND) { return nullptr; }
inline BOOL ReleaseCapture() { return TRUE; }
inline BOOL GetClientRect(HWND, RECT*) { return TRUE; }

// Menu helpers
inline UINT CheckMenuItem(HMENU, UINT, UINT) { return 0; }
inline BOOL DrawMenuBar(HWND) { return TRUE; }
inline HMENU GetSubMenu(HMENU, int) { return nullptr; }

// Drag & drop / file stubs
inline UINT DragQueryFileW(HDROP, UINT, wchar_t*, UINT) { return 0; }
inline void DragFinish(HDROP) {}

// Registry stubs
inline LONG RegOpenKeyExW(HKEY, const wchar_t*, unsigned long, unsigned long, HKEY*) { return ERROR_SUCCESS; }
inline LONG RegQueryValueExW(HKEY, const wchar_t*, unsigned long*, unsigned char*, unsigned long*) { return ERROR_SUCCESS; }
inline LONG RegSetValueExW(HKEY, const wchar_t*, unsigned long, unsigned long, const unsigned char*, unsigned long) { return ERROR_SUCCESS; }
inline LONG RegCloseKey(HKEY) { return ERROR_SUCCESS; }

// Keyboard / locale stubs
inline int GetKeyState(int) { return 0; }
inline SHORT VkKeyScanW(wchar_t) { return 0; }

// Character classification
inline BOOL IsCharAlphaNumericW(wchar_t) { return TRUE; }
inline BOOL IsCharAlphaW(wchar_t) { return TRUE; }
inline BOOL IsCharLowerW(wchar_t) { return TRUE; }
inline BOOL IsCharUpperW(wchar_t) { return TRUE; }

// Misc stubs
inline DWORD GetCurrentThreadId() { return 1; }
inline DWORD GetCurrentProcessId() { return 1; }
inline BOOL MoveFileExW(const wchar_t*, const wchar_t*, DWORD) { return TRUE; }
inline HLOCAL LocalFree(HLOCAL) { return nullptr; }
inline DWORD GetTickCount() { return 0; }

// String stubs - use standard library on Linux
#ifdef __linux__
#define _wcsicmp wcscasecmp
#define _wcsnicmp wcsncasecmp
#else
inline int _wcsicmp(const wchar_t*, const wchar_t*) { return 0; }
inline int _wcsnicmp(const wchar_t*, const wchar_t*, size_t) { return 0; }
#endif

#endif // WINDOWS_COMPAT_H#define SHORT short
// Window API stubs for Window.h (Qt widget methods)
// Duplicate
#define SHORT short
