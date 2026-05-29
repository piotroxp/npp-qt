#ifndef WINDOWS_COMPAT_H
#define WINDOWS_COMPAT_H

// =============================================================================
// Qt headers - must come BEFORE any typedefs using qint64/quint64
// =============================================================================
#include <QtCore>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <cstdint>
#include <cstdarg>

// =============================================================================
// Basic Windows types
// =============================================================================
#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

#ifndef WORD
typedef unsigned short WORD;
typedef WORD* LPWORD;
#endif

#ifndef DWORD
typedef unsigned int DWORD;
#endif
typedef DWORD* LPDWORD;
typedef BYTE* LPBYTE;
#ifndef errno_t
typedef int errno_t;
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

#ifndef SHORT
typedef short SHORT;
#endif

#ifndef DWORD_PTR
typedef uintptr_t DWORD_PTR;
#endif
#ifndef ULONGLONG
typedef unsigned long long ULONGLONG;
#endif
#ifndef HIWORD
#define NPP_TO_DWORD_PTR(v) ((DWORD_PTR)(ULONG_PTR)(UINT_PTR)(v))
#define HIWORD(l) static_cast<WORD>((NPP_TO_DWORD_PTR(l) >> 16) & 0xFFFF)
#define LOWORD(l) static_cast<WORD>(NPP_TO_DWORD_PTR(l) & 0xFFFF)
#define HIBYTE(w) static_cast<BYTE>((static_cast<WORD>(w) >> 8) & 0xFF)
#define LOBYTE(w) static_cast<BYTE>(static_cast<WORD>(w) & 0xFF)
#endif

#ifndef SIZE_T
typedef size_t SIZE_T;
#endif

#ifndef COINIT_APARTMENTTHREADED
#define COINIT_APARTMENTTHREADED 0x2
#endif

#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ 0x00000001
#endif
#ifndef FILE_SHARE_WRITE
#define FILE_SHARE_WRITE 0x00000002
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef BOOL
typedef int BOOL;
#endif
typedef BOOL* PBOOL;

#ifndef WPARAM
typedef uintptr_t WPARAM;
#endif

#ifndef LPARAM
typedef intptr_t LPARAM;
#endif

#ifndef LRESULT
typedef intptr_t LRESULT;
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

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#endif

#ifndef NPP_WINDOWS_COMPAT_HWND
#define NPP_WINDOWS_COMPAT_HWND 1
#endif
#ifndef HWND
typedef QWidget* HWND;
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
// Geometry types (Win32 layout — do not alias QPoint; lifted code assigns .x/.y)
// =============================================================================
#ifndef POINT_DEFINED
#define POINT_DEFINED
struct POINT {
	LONG x;
	LONG y;
};
#endif
#ifndef SIZE_DEFINED
#define SIZE_DEFINED
struct SIZE {
	LONG cx;
	LONG cy;
};
#endif
typedef POINT* PPOINT;
typedef SIZE* PSIZE;
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
    unsigned long LowPart;
    unsigned long HighPart;
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
    HWND hwndFrom = nullptr;
    UINT_PTR idFrom = 0;
    UINT code = 0;
};
#endif

#ifndef _TOOLINFO_DEFINED
#define _TOOLINFO_DEFINED
struct TOOLINFO {
    unsigned int cbSize;
    unsigned int uFlags;
    HWND        hwnd;
    unsigned int uId;
    RECT        rect;
    HINSTANCE   hinst;
    wchar_t*    lpszText;
    LPARAM     lParam;
};
#endif

#ifndef _WIN32_FILE_ATTRIBUTE_DATA_DEFINED
#define _WIN32_FILE_ATTRIBUTE_DATA_DEFINED
struct WIN32_FILE_ATTRIBUTE_DATA {
    DWORD    dwFileAttributes = 0;
    FILETIME ftCreationTime{};
    FILETIME ftLastAccessTime{};
    FILETIME ftLastWriteTime{};
    DWORD    nFileSizeHigh = 0;
    DWORD    nFileSizeLow = 0;
};
#endif

#ifndef _WINDOWPLACEMENT_DEFINED
#define _WINDOWPLACEMENT_DEFINED
struct WINDOWPLACEMENT {
    UINT  length = 0;
    UINT  flags = 0;
    UINT  showCmd = 0;
    POINT ptMinPosition{};
    POINT ptMaxPosition{};
    RECT rcNormalPosition{};
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
#ifndef ERROR_OPERATION_ABORTED
#define ERROR_OPERATION_ABORTED 995L
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
#ifndef WS_CAPTION
#define WS_CAPTION            0x00C00000L
#endif
#ifndef WS_SYSMENU
#define WS_SYSMENU            0x00080000L
#endif
#ifndef WS_THICKFRAME
#define WS_THICKFRAME         0x00040000L
#endif
#ifndef WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW   (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#endif
#ifndef WS_EX_ACCEPTFILES
#define WS_EX_ACCEPTFILES 0x00000010L
#define WS_EX_LAYOUTRTL 0x00400000L
#endif
#ifndef WM_ACTIVATE
#define WM_ACTIVATE 0x0006
#endif

using NppWndProc = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);

struct WNDCLASSW {
	UINT style = 0;
	NppWndProc lpfnWndProc = nullptr;
	int cbClsExtra = 0;
	int cbWndExtra = 0;
	HINSTANCE hInstance = nullptr;
	HICON hIcon = nullptr;
	HCURSOR hCursor = nullptr;
	HBRUSH hbrBackground = nullptr;
	const wchar_t* lpszMenuName = nullptr;
	const wchar_t* lpszClassName = nullptr;
};
typedef WNDCLASSW WNDCLASS;

struct PRINTDLGW {
	DWORD lStructSize = 0;
	HWND hwndOwner = nullptr;
	HGLOBAL hDevMode = nullptr;
	HGLOBAL hDevNames = nullptr;
	HDC hDC = nullptr;
	DWORD Flags = 0;
	WORD nFromPage = 0;
	WORD nToPage = 0;
	WORD nMinPage = 0;
	WORD nMaxPage = 0;
	WORD nCopies = 0;
	HINSTANCE hInstance = nullptr;
	LPARAM lCustData = 0;
	void* lpfnPrintHook = nullptr;
	void* lpfnSetupHook = nullptr;
	const wchar_t* lpPrintTemplateName = nullptr;
	const wchar_t* lpSetupTemplateName = nullptr;
	HGLOBAL hPrintTemplate = nullptr;
	HGLOBAL hSetupTemplate = nullptr;
};
typedef PRINTDLGW PRINTDLG;
inline BOOL PrintDlgW(PRINTDLG*) { return FALSE; }
#define PrintDlg PrintDlgW

// GetWindowLong offsets
#ifndef GWL_STYLE
#define GWL_STYLE            (-16)
#endif

#ifndef GWL_WNDPROC
#define GWL_WNDPROC          (-4)
#define GWLP_USERDATA        (-21)
#define GWLP_WNDPROC         GWL_WNDPROC
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
#ifndef CB_ERR
#define CB_ERR (-1)
#endif
#ifndef CB_INSERTSTRING
#define CB_INSERTSTRING      0x014A
#define CB_DELETESTRING      0x0144
#define CB_FINDSTRINGEXACT   0x0158
#define CB_GETDROPPEDSTATE   0x0157
#define CB_GETEDITSEL        0x0140
#define CB_SETEDITSEL        0x0142
#define CB_LIMITTEXT         0x0141
#define CB_SHOWDROPDOWN      0x014F
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
struct tagNMTOOLBARW {
	NMHDR hdr{};
	int iItem = 0;
};
#ifndef LPNMTOOLBARW
#define LPNMTOOLBARW tagNMTOOLBARW*
#endif

// =============================================================================
// Win32 macros / types for dpiManager, dialogs, toolbars
// =============================================================================
#ifndef WINAPI
#define WINAPI
#endif
#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef APIENTRY
#define APIENTRY WINAPI
#endif
#ifndef LPVOID
#define LPVOID void*
#endif
#ifndef UINT_PTR
typedef uintptr_t UINT_PTR;
#endif
#ifndef MAKEINTRESOURCE
#define MAKEINTRESOURCE(i) (reinterpret_cast<wchar_t*>(static_cast<uintptr_t>(i)))
#endif
typedef LRESULT (CALLBACK *HOOKPROC)(int code, WPARAM wParam, LPARAM lParam);

#ifndef RGB
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#endif
#ifndef GetRValue
#define GetRValue(rgb) ((BYTE)(rgb))
#endif
#ifndef GetGValue
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#endif
#ifndef GetBValue
#define GetBValue(rgb) ((BYTE)((rgb) >> 16))
#endif

#ifndef LOWORD
#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b) ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif
#ifndef MulDiv
inline int MulDiv(int x, int y, int z) {
	return z ? static_cast<int>((static_cast<long long>(x) * y + z / 2) / z) : 0;
}
#endif

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef SWP_NOACTIVATE
#define SWP_NOACTIVATE 0x0010
#endif
#ifndef SWP_NOZORDER
#define SWP_NOZORDER 0x0004
#endif
#ifndef LR_DEFAULTCOLOR
#define LR_DEFAULTCOLOR 0x0000
#endif

#ifndef DPI_AWARENESS_CONTEXT
typedef void* DPI_AWARENESS_CONTEXT;
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)(INT_PTR)-4)
#define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED ((DPI_AWARENESS_CONTEXT)(INT_PTR)-5)
#endif

#ifndef LOGFONT_DEFINED
#define LOGFONT_DEFINED
struct LOGFONTW {
	LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	wchar_t lfFaceName[32];
};
typedef LOGFONTW LOGFONT;
struct ENUMLOGFONTEXW {
	LOGFONTW elfLogFont{};
	wchar_t elfFullName[64]{};
	wchar_t elfStyle[32]{};
};
typedef ENUMLOGFONTEXW ENUMLOGFONTEX;
#endif

typedef INT_PTR (CALLBACK *DLGPROC)(HWND, UINT, WPARAM, LPARAM);

#ifndef BST_CHECKED
#define BST_CHECKED 1
#define BST_UNCHECKED 0
#endif
#ifndef BM_GETCHECK
#define BM_GETCHECK 0x00F0
#define BM_SETCHECK 0x00F1
#endif

struct DRAWITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	UINT itemID;
	UINT itemAction;
	UINT itemState;
	HWND hwndItem;
	HDC hDC;
	RECT rcItem;
	DWORD_PTR itemData;
};

inline LRESULT SendMessage(HWND, UINT, WPARAM, LPARAM) { return 0; }
inline LRESULT SendDlgItemMessage(HWND, int, UINT, WPARAM, LPARAM) { return 0; }
inline HWND GetDlgItem(HWND parent, int) { return parent; }
inline BOOL ImageList_Destroy(HIMAGELIST) { return TRUE; }
inline BOOL ImageList_RemoveAll(HIMAGELIST) { return TRUE; }

struct TBBUTTON {
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];
	DWORD_PTR dwData;
	INT_PTR iString;
};

struct REBARBANDINFO {
	UINT cbSize;
	UINT fMask;
	UINT fStyle;
	COLORREF clrFore;
	COLORREF clrBack;
	wchar_t lpText[256];
	UINT cch;
	int iImage;
	HWND hwndChild;
	UINT cxMinChild;
	UINT cyMinChild;
	UINT cx;
	HBITMAP hbmBack;
	UINT wID;
	UINT cyChild;
	UINT cyMaxChild;
	UINT cyIntegral;
	UINT cxIdeal;
	LPARAM lParam;
	UINT cxHeader;
	RECT rcChevronLocation{};
	UINT uChevronState = 0;
};

#ifndef TB_ENABLEBUTTON
#define TB_ENABLEBUTTON 0x0401
#define TB_CHECKBUTTON 0x0402
#define TB_GETSTATE 0x040C
#define TB_SETIMAGELIST 0x0430
#define TB_SETDISABLEDIMAGELIST 0x0436
#define TBSTATE_CHECKED 0x01
#endif

#ifndef IMAGE_FILE_MACHINE_I386
#define IMAGE_FILE_MACHINE_I386 0x014c
#endif
#ifndef IMAGE_FILE_MACHINE_AMD64
#define IMAGE_FILE_MACHINE_AMD64 0x8664
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xaa64
#endif
#ifndef IMAGE_FILE_MACHINE_UNKNOWN
#define IMAGE_FILE_MACHINE_UNKNOWN 0
#endif
#ifndef GENERIC_READ
#define GENERIC_READ 0x80000000
#endif
#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY 0x00000001
#endif
#ifndef FILE_MAP_READ
#define FILE_MAP_READ 4
#endif
#ifndef PAGE_READONLY
#define PAGE_READONLY 0x02
#endif
#ifndef SEC_IMAGE
#define SEC_IMAGE 0x01000000
#endif
#ifndef IDYES
#define IDYES 6
#endif
#ifndef MF_BYPOSITION
#define MF_BYPOSITION 0x00000400
#endif
#ifndef MF_POPUP
#define MF_POPUP 0x00000010
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef WIN32_FIND_DATA_DEFINED
#define WIN32_FIND_DATA_DEFINED
struct WIN32_FIND_DATAW {
	DWORD dwFileAttributes = 0;
	FILETIME ftCreationTime{};
	FILETIME ftLastAccessTime{};
	FILETIME ftLastWriteTime{};
	DWORD nFileSizeHigh = 0;
	DWORD nFileSizeLow = 0;
	DWORD dwReserved0 = 0;
	DWORD dwReserved1 = 0;
	wchar_t cFileName[MAX_PATH]{};
	wchar_t cAlternateFileName[14]{};
};
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
#endif

struct IMAGE_NT_HEADERS;
typedef IMAGE_NT_HEADERS* PIMAGE_NT_HEADERS;

extern bool g_darkModeSupported;
extern bool g_darkModeEnabled;

inline HWND GetParent(HWND hwnd) {
	return hwnd ? hwnd->parentWidget() : nullptr;
}

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
inline HBITMAP CreateCompatibleBitmap(HDC, int, int) { return nullptr; }
inline HDC CreateCompatibleDC(HDC) { return nullptr; }
inline BOOL IsZoomed(HWND) { return FALSE; }
inline UINT GetMenuItemID(HMENU, int) { return 0; }
inline BOOL ReleaseMutex(HANDLE) { return TRUE; }
inline HGDIOBJ SelectObject(HDC, HGDIOBJ) { return nullptr; }
inline BOOL SetMenuItemBitmaps(HMENU, UINT, UINT, HBITMAP, HBITMAP) { return TRUE; }
inline BOOL TabCtrl_SetItemSize(HWND, int, int) { return TRUE; }
inline int lstrlenW(const wchar_t* s) { return s ? static_cast<int>(wcslen(s)) : 0; }
#define lstrlen lstrlenW
inline int wcscpy_s(wchar_t* d, size_t n, const wchar_t* s) {
	if (!d || n == 0) return -1;
	wcsncpy(d, s, n - 1);
	d[n - 1] = L'\0';
	return 0;
}
template<size_t N>
inline int wcscpy_s(wchar_t (&d)[N], const wchar_t* s) {
	return wcscpy_s(d, N, s);
}
inline int _itow(int v, wchar_t* buf, int radix) { return swprintf(buf, 32, radix == 10 ? L"%d" : L"%x", v); }
inline int wcsnicmp(const wchar_t* a, const wchar_t* b, size_t n) { return wcsncasecmp(a, b, n); }

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
inline BOOL SetWindowPos(HWND hwnd, HWND, int x, int y, int cx, int cy, UINT) {
	if (hwnd)
		hwnd->setGeometry(x, y, cx, cy);
	return TRUE;
}
inline HWND CreateWindowExW(DWORD, const wchar_t*, const wchar_t*, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void*) { return nullptr; }
#define CreateWindowEx CreateWindowExW
inline ATOM RegisterClassW(const WNDCLASSW*) { return 1; }
#define RegisterClass RegisterClassW
inline HICON LoadIconW(HINSTANCE, const wchar_t*) { return nullptr; }
inline HICON LoadIcon(HINSTANCE inst, const wchar_t* name) { return LoadIconW(inst, name); }
inline HICON LoadIcon(HINSTANCE inst, int id) { return LoadIconW(inst, MAKEINTRESOURCE(id)); }
inline HCURSOR LoadCursorW(HINSTANCE, const wchar_t*) { return nullptr; }
inline HCURSOR LoadCursor(HINSTANCE inst, const wchar_t* name) { return LoadCursorW(inst, name); }
inline HCURSOR LoadCursor(HINSTANCE inst, int id) { return LoadCursorW(inst, MAKEINTRESOURCE(id)); }
inline BOOL SetMenu(HWND, HMENU) { return TRUE; }
inline BOOL LockWindowUpdate(HWND) { return TRUE; }
inline HDC GetDCEx(HWND, HRGN, DWORD) { return nullptr; }
#ifndef COLOR_MENU
#define COLOR_MENU 4
#endif
#ifndef CS_DBLCLKS
#define CS_DBLCLKS 0x0008
#define CS_BYTEALIGNWINDOW 0x2000
#endif
#ifndef DCX_CACHE
#define DCX_CACHE 0x00000002
#define DCX_LOCKWINDOWUPDATE 0x00000400
#endif
#ifndef IDC_ARROW
#define IDC_ARROW 32512
#endif
#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#define SW_MAXIMIZE 3
#endif
#ifndef WA_ACTIVE
#define WA_ACTIVE 1
#endif
inline BOOL IsDialogMessageW(HWND, void*) { return FALSE; }
#define IsDialogMessage IsDialogMessageW
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
#ifndef HKEY_CLASSES_ROOT
#define HKEY_CLASSES_ROOT ((HKEY)(ULONG_PTR)0x80000000)
#endif
#ifndef KEY_ALL_ACCESS
#define KEY_ALL_ACCESS 0xF003F
#endif
#ifndef REG_OPENED_EXISTING_KEY
#define REG_OPENED_EXISTING_KEY 0x00000002
#endif
inline LONG RegOpenKeyExW(HKEY, const wchar_t*, unsigned long, unsigned long, HKEY*) { return ERROR_SUCCESS; }
inline LONG RegCreateKeyExW(HKEY, const wchar_t*, DWORD, wchar_t*, DWORD, DWORD, void*, HKEY*, DWORD*) { return ERROR_SUCCESS; }
inline LONG RegDeleteKeyW(HKEY, const wchar_t*) { return ERROR_SUCCESS; }
inline LONG RegDeleteValueW(HKEY, const wchar_t*) { return ERROR_SUCCESS; }
inline LONG RegEnumKeyExW(HKEY, DWORD, wchar_t*, LPDWORD, LPDWORD, wchar_t*, LPDWORD, void*) { return 259L; }
inline LONG RegQueryValueExW(HKEY, const wchar_t*, LPDWORD, LPDWORD, LPBYTE, LPDWORD) { return ERROR_SUCCESS; }
inline LONG RegSetValueExW(HKEY, const wchar_t*, unsigned long, unsigned long, const unsigned char*, unsigned long) { return ERROR_SUCCESS; }
inline LONG RegCloseKey(HKEY) { return ERROR_SUCCESS; }
#define RegOpenKeyEx RegOpenKeyExW
#define RegCreateKeyEx RegCreateKeyExW
#define RegDeleteKey RegDeleteKeyW
#define RegDeleteValue RegDeleteValueW
#define RegEnumKeyEx RegEnumKeyExW
#define RegQueryValueEx RegQueryValueExW
#define RegSetValueEx RegSetValueExW

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
#define MoveFileEx MoveFileExW
inline HLOCAL LocalFree(HLOCAL) { return nullptr; }
inline DWORD GetTickCount() { return 0; }

// Virtual keys and accelerator flags
#ifndef VK_BACK
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_ADD 0x6B
#define VK_SUBTRACT 0x6D
#define VK_DIVIDE 0x6F
#define VK_CAPITAL 0x14
#define VK_OEM_2 0xBF
#define VK_OEM_1 0xBA
#define VK_OEM_PLUS 0xBB
#define VK_OEM_COMMA 0xBC
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_7 0xDE
#define VK_OEM_8 0xDF
#define VK_OEM_102 0xE2
#define VK_MULTIPLY 0x6A
#define VK_SEPARATOR 0x6C
#define VK_DECIMAL 0x6E
#define VK_OEM_3 0xC0
#define VK_OEM_4 0xDB
#define VK_OEM_5 0xDC
#define VK_OEM_6 0xDD
#define VK_LCONTROL 0xA2
#define VK_CONTROL VK_LCONTROL
#define VK_MENU 0x12
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87
#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#endif
#ifndef FVIRTKEY
#define FVIRTKEY 0x01
#define FCONTROL 0x02
#define FALT 0x04
#define FSHIFT 0x10
#endif

#ifndef CP_ACP
#define CP_ACP 0
#endif
#ifndef CREATE_ALWAYS
#define CREATE_ALWAYS 2
#endif
#ifndef TRUNCATE_EXISTING
#define TRUNCATE_EXISTING 5
#endif
#ifndef OPEN_EXISTING
#define OPEN_EXISTING 3
#endif
#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#endif
#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND 2
#endif
#ifndef NO_ERROR
#define NO_ERROR 0
#endif
#ifndef FILE_NAME_OPENED
#define FILE_NAME_OPENED 0x8
#endif
#ifndef VOLUME_NAME_NT
#define VOLUME_NAME_NT 0x2
#endif
#ifndef FindStreamInfoStandard
#define FindStreamInfoStandard 0
#endif
#ifndef MB_ICONWARNING
#define MB_ICONWARNING 0x30
#endif
#ifndef MB_ICONERROR
#define MB_ICONERROR 0x10
#endif
#ifndef MB_SYSTEMMODAL
#define MB_SYSTEMMODAL 0x1000
#endif
#ifndef WM_SETREDRAW
#define WM_SETREDRAW 0x000B
#endif
#ifndef LCMAP_SORTKEY
#define LCMAP_SORTKEY 0x0400
#endif
#ifndef LOCALE_NAME_USER_DEFAULT
#define LOCALE_NAME_USER_DEFAULT nullptr
#endif
#ifndef LINGUISTIC_IGNORECASE
#define LINGUISTIC_IGNORECASE 0x10
#endif
#ifndef LINGUISTIC_IGNOREDIACRITIC
#define LINGUISTIC_IGNOREDIACRITIC 0x20
#endif
#ifndef NORM_IGNORESYMBOLS
#define NORM_IGNORESYMBOLS 0x800
#endif
#ifndef NORM_LINGUISTIC_CASING
#define NORM_LINGUISTIC_CASING 0x10000000
#endif
#ifndef SORT_DIGITSASNUMBERS
#define SORT_DIGITSASNUMBERS 0x8
#endif

#ifndef LARGE_INTEGER_DEFINED
#define LARGE_INTEGER_DEFINED
typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	long long QuadPart;
} LARGE_INTEGER;
#endif

struct WIN32_FIND_STREAM_DATA {
	LARGE_INTEGER StreamSize;
	wchar_t cStreamName[296];
};

#ifndef TCN_FIRST
#define TCN_FIRST (-550)
#endif
#ifndef TCM_GETCURSEL
#define TCM_GETCURSEL (WM_USER + 11)
#define TCM_SETCURSEL (WM_USER + 12)
#define TCM_SETCURFOCUS (WM_USER + 15)
#define TCM_GETIMAGELIST (WM_USER + 2)
#define TCM_SETIMAGELIST (WM_USER + 3)
#define TCM_SETTOOLTIPS (WM_USER + 9)
#define TCS_TABS 0x0000
#define TCS_BUTTONS 0x0100
#define TCS_MULTILINE 0x0200
#define TCS_VERTICAL 0x0080
#define TCS_RIGHTJUSTIFY 0x0001
#define TCS_FOCUSNEVER 0x8000
#define WC_TABCONTROL L"SysTabControl32"
#define TOOLTIPS_CLASS L"tooltips_class32"
#define TTS_ALWAYSTIP 0x01
#define TTS_NOPREFIX 0x02
#define WM_APP 0x8000
#ifndef VK_LBUTTON
#define VK_LBUTTON 0x01
#endif
#define TabCtrl_GetRowCount(hwnd) static_cast<int>(SendMessage(hwnd, TCM_GETROWCOUNT, 0, 0))
#define NM_CLICK (0U - 2U)
#define MK_CONTROL 0x0008
#define MK_SHIFT 0x0004
#define SB_ENDSCROLL 8
#define ILD_SELECTED 0x00000004
#define COLOR_BTNTEXT 18
struct IMAGEINFO {
	HBITMAP hbmImage = nullptr;
	HBITMAP hbmMask = nullptr;
	int Unused1 = 0;
	int Unused2 = 0;
	RECT rcImage{};
};
inline BOOL ImageList_Draw(HIMAGELIST, int, HDC, int, int, UINT) { return TRUE; }
inline BOOL ImageList_GetImageInfo(HIMAGELIST, int, IMAGEINFO*) { return FALSE; }
inline BOOL SetRectEmpty(RECT*) { return TRUE; }
inline HWND GetCapture() { return nullptr; }
inline BOOL GetTextExtentPointW(HDC, const wchar_t*, int, SIZE*) { return FALSE; }
#define GetTextExtentPoint GetTextExtentPointW
#define TCM_GETITEMCOUNT (WM_USER + 4)
#define TCM_GETROWCOUNT (WM_USER + 40)
#define TCM_HITTEST (WM_USER + 13)
#define TCM_DELETEALLITEMS (WM_USER + 19)
#define TCM_SETMINTABWIDTH (WM_USER + 49)
#define TCM_SETPADDING (WM_USER + 43)
#define TCM_GETITEMRECT (WM_USER + 10)
#define TCM_GETFOCUS (WM_USER + 14)
#endif
#ifndef TCS_OWNERDRAWFIXED
#define TCS_OWNERDRAWFIXED 0x0020
#endif
#ifndef TCN_GETOBJECT
#define TCN_GETOBJECT (TCN_FIRST - 3)
#endif
#define TabCtrl_GetCurSel(hwnd) static_cast<int>(SendMessage(hwnd, TCM_GETCURSEL, 0, 0))
#define TabCtrl_SetCurSel(hwnd, i) SendMessage(hwnd, TCM_SETCURSEL, static_cast<WPARAM>(i), 0)
#define TabCtrl_GetItemCount(hwnd) static_cast<int>(SendMessage(hwnd, TCM_GETITEMCOUNT, 0, 0))
#define TabCtrl_GetItemRect(hwnd, i, prc) SendMessage(hwnd, TCM_GETITEMRECT, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(prc))
#define TabCtrl_GetCurFocus(hwnd) static_cast<int>(SendMessage(hwnd, TCM_GETFOCUS, 0, 0))
#define TabCtrl_SetPadding(hwnd, cx, cy) SendMessage(hwnd, TCM_SETPADDING, 0, MAKELPARAM(cx, cy))
struct TCHITTESTINFO {
	POINT pt;
	UINT flags;
};
typedef TCHITTESTINFO TC_HITTESTINFO;

#ifndef ACCEL_DEFINED
#define ACCEL_DEFINED
struct ACCEL {
	BYTE fVirt;
	WORD key;
	WORD cmd;
};
#endif

#ifndef __inout
#define __inout
#define __in
#define __out
#define __in_opt
#define __inout_opt
#endif

#define LOCALE_IDEFAULTANSICODEPAGE 0x1004
#define LOCALE_NAME_SYSTEM_DEFAULT L""
#define LOCALE_RETURN_NUMBER 0x20000000
#define lstrcmp wcscmp
#define lstrcmpi _wcsicmp
inline int _wtoi(const wchar_t* s) { return s ? static_cast<int>(wcstol(s, nullptr, 10)) : 0; }
#define MB_ICONHAND MB_ICONERROR

inline HANDLE CreateEvent(void*, BOOL, BOOL, const wchar_t*) { return nullptr; }
inline BOOL SetEvent(HANDLE) { return TRUE; }
inline void GetSystemTimeAsFileTime(FILETIME*) {}
inline void ColorRGBToHLS(COLORREF, WORD*, WORD*, WORD*) {}
inline COLORREF ColorHLSToRGB(WORD, WORD, WORD) { return 0; }
inline int GetLocaleInfoEx(const wchar_t*, DWORD, wchar_t*, int) { return 0; }
inline void Scintilla_ReleaseResources() {}
inline BOOL DestroyAcceleratorTable(HACCEL) { return TRUE; }
inline HACCEL CreateAcceleratorTableW(ACCEL*, int) { return nullptr; }
#define CreateAcceleratorTable CreateAcceleratorTableW
#ifndef EM_SETREADONLY
#define EM_SETREADONLY 0x00CF
#endif
#ifndef LANGID
typedef unsigned short LANGID;
#define MAKELANGID(p, s) (static_cast<LANGID>((static_cast<WORD>(s) << 10) | static_cast<WORD>(p)))
#endif
#ifndef MAPVK_VK_TO_CHAR
#define MAPVK_VK_TO_CHAR 2
#endif
inline UINT MapVirtualKeyW(UINT, UINT) { return 0; }
#define MapVirtualKey MapVirtualKeyW
inline BOOL FileTimeToLocalFileTime(const FILETIME*, FILETIME*) { return FALSE; }
#ifndef WM_NULL
#define WM_NULL 0x0000
#endif
#ifndef __int64
typedef long long __int64;
#endif
inline int _istdigit(wint_t c) { return iswdigit(c); }
inline int _istascii(wint_t c) { return (c & ~0x7F) == 0; }
inline int _istupper(wint_t c) { return iswupper(c); }

#include <cstdarg>
#include <cstdio>
inline FILE* _wfopen(const wchar_t* path, const wchar_t* mode) {
	char pathMb[4096];
	char modeMb[16];
	if (wcstombs(pathMb, path, sizeof(pathMb) - 1) == static_cast<size_t>(-1))
		return nullptr;
	pathMb[sizeof(pathMb) - 1] = '\0';
	if (wcstombs(modeMb, mode, sizeof(modeMb) - 1) == static_cast<size_t>(-1))
		return nullptr;
	modeMb[sizeof(modeMb) - 1] = '\0';
	return fopen(pathMb, modeMb);
}

inline int wsprintf(wchar_t* buf, const wchar_t* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int n = vswprintf(buf, 4096, fmt, args);
	va_end(args);
	return n < 0 ? 0 : n;
}

inline HMENU CreateMenu() { return nullptr; }
inline BOOL DeleteFileW(const wchar_t*) { return TRUE; }
#define DeleteFile DeleteFileW
inline HANDLE FindFirstFileW(const wchar_t*, WIN32_FIND_DATA*) { return INVALID_HANDLE_VALUE; }
#define FindFirstFile FindFirstFileW
inline BOOL FindNextFileW(HANDLE, WIN32_FIND_DATA*) { return FALSE; }
#define FindNextFile FindNextFileW
inline BOOL InsertMenuW(HMENU, UINT, UINT, UINT_PTR, const wchar_t*) { return TRUE; }
#define InsertMenu InsertMenuW
inline HMODULE LoadLibraryExW(const wchar_t*, HANDLE, DWORD) { return nullptr; }
#define LoadLibraryEx LoadLibraryExW
inline int MessageBoxA(void*, const char*, const char*, unsigned int) { return 0; }
inline wchar_t* PathFindFileNameW(wchar_t* path) {
	wchar_t* slash = wcsrchr(path, L'/');
	wchar_t* back = wcsrchr(path, L'\\');
	wchar_t* base = slash > back ? slash : back;
	return base ? base + 1 : path;
}
inline const wchar_t* PathFindFileNameW(const wchar_t* path) {
	const wchar_t* slash = wcsrchr(path, L'/');
	const wchar_t* back = wcsrchr(path, L'\\');
	const wchar_t* base = slash > back ? slash : back;
	return base ? base + 1 : path;
}
#define PathFindFileName PathFindFileNameW
inline HANDLE CreateFileW(const wchar_t*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE) {
	return INVALID_HANDLE_VALUE;
}
#define CreateFile CreateFileW
inline HANDLE CreateFileMapping(HANDLE, void*, DWORD, DWORD, DWORD, const wchar_t*) { return nullptr; }
inline void* MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, size_t) { return nullptr; }
inline BOOL UnmapViewOfFile(const void*) { return TRUE; }
inline DWORD GetFullPathNameW(const wchar_t* lpFileName, DWORD nBufferLength, wchar_t* lpBuffer, wchar_t**) {
	if (!lpFileName) return 0;
	if (lpBuffer && nBufferLength > 0) {
		wcsncpy(lpBuffer, lpFileName, nBufferLength - 1);
		lpBuffer[nBufferLength - 1] = L'\0';
	}
	return static_cast<DWORD>(wcslen(lpFileName));
}
#define GetFullPathName GetFullPathNameW
inline DWORD GetModuleFileNameW(HMODULE, wchar_t*, DWORD) { return 0; }
#define GetModuleFileName GetModuleFileNameW
inline HMODULE GetModuleHandleW(const wchar_t*) { return nullptr; }
#define GetModuleHandle GetModuleHandleW
inline PIMAGE_NT_HEADERS ImageNtHeader(void*) { return nullptr; }

#ifndef CHAR
typedef char CHAR;
#endif
typedef NMHDR* LPNMHDR;
typedef DRAWITEMSTRUCT* LPDRAWITEMSTRUCT;

#ifndef TCIF_TEXT
#define TCIF_TEXT 0x0001
#endif
struct TCITEMW {
	UINT mask;
	DWORD dwState;
	DWORD dwStateMask;
	LPWSTR pszText;
	int cchTextMax;
	int iImage;
	LPARAM lParam;
};
typedef TCITEMW TCITEM;
#define TabCtrl_SetItem(hwnd, index, item) 0

#ifndef WM_SETTEXT
#define WM_SETTEXT 0x000C
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef DM_SETDEFID
#define DM_SETDEFID (WM_USER + 1)
#endif
#ifndef DM_REPOSITION
#define DM_REPOSITION (WM_USER + 5)
#endif
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0
#endif
#ifndef LB_GETCURSEL
#define LB_GETCURSEL 0x0188
#endif
#ifndef WH_GETMESSAGE
#define WH_GETMESSAGE 3
#endif

struct MINMAXINFO {
	POINT ptReserved;
	POINT ptMaxSize;
	POINT ptMaxPosition;
	POINT ptMinTrackSize;
	POINT ptMaxTrackSize;
};
typedef MINMAXINFO* LPMINMAXINFO;

struct BITMAP {
	LONG bmType = 0;
	LONG bmWidth = 0;
	LONG bmHeight = 0;
	LONG bmWidthBytes = 0;
	WORD bmPlanes = 0;
	WORD bmBitsPixel = 0;
	LPVOID bmBits = nullptr;
};

struct RGBQUAD {
	BYTE rgbBlue = 0;
	BYTE rgbGreen = 0;
	BYTE rgbRed = 0;
	BYTE rgbReserved = 0;
};

struct BITMAPINFOHEADER {
	DWORD biSize = 0;
	LONG biWidth = 0;
	LONG biHeight = 0;
	WORD biPlanes = 0;
	WORD biBitCount = 0;
	DWORD biCompression = 0;
	DWORD biSizeImage = 0;
	LONG biXPelsPerMeter = 0;
	LONG biYPelsPerMeter = 0;
	DWORD biClrUsed = 0;
	DWORD biClrImportant = 0;
};

struct BITMAPINFO {
	BITMAPINFOHEADER bmiHeader{};
	RGBQUAD bmiColors[1]{};
};

#ifndef BI_RGB
#define BI_RGB 0
#endif
#ifndef DIB_RGB_COLORS
#define DIB_RGB_COLORS 0
#endif

struct ICONINFO {
	BOOL fIcon = FALSE;
	DWORD xHotspot = 0;
	DWORD yHotspot = 0;
	HBITMAP hbmMask = nullptr;
	HBITMAP hbmColor = nullptr;
};

#ifndef SRCCOPY
#define SRCCOPY 0x00CC0020
#endif
#ifndef PS_SOLID
#define PS_SOLID 0
#endif
#ifndef ANSI_CHARSET
#define ANSI_CHARSET 0
#define DEFAULT_PITCH 0
#define FF_ROMAN 16
#endif
#ifndef COLOR_BTNFACE
#define COLOR_BTNFACE 15
#define COLOR_BTNSHADOW 16
#define COLOR_ACTIVECAPTION 2
#define COLOR_CAPTIONTEXT 9
#endif
#ifndef DT_BOTTOM
#define DT_BOTTOM 0x00000008
#define DT_END_ELLIPSIS 0x00008000
#endif
#ifndef MK_LBUTTON
#define MK_LBUTTON 0x0001
#endif
#ifndef ODA_DRAWENTIRE
#define ODA_DRAWENTIRE 0x0001
#define ODA_SELECT 0x0002
#define ODA_FOCUS 0x0004
#define ODS_DEFAULT 0x0000
#define ODS_FOCUS 0x0010
#define ODS_NOFOCUSRECT 0x0200
#define ODS_SELECTED 0x0001
#define ODT_TAB 5
#endif
#ifndef VK_SHIFT
#define VK_SHIFT 0x10
#endif
#ifndef SC_MOVE
#define SC_MOVE 0xF010
#endif
#ifndef ILC_COLOR32
#define ILC_COLOR32 0x00000020
#define ILC_MASK 0x00000001
#define ILD_TRANSPARENT 0x00000001
#endif

inline HWND GetActiveWindow() { return nullptr; }
inline BOOL BitBlt(HDC, int, int, int, int, HDC, int, int, DWORD) { return TRUE; }
inline int ExcludeClipRect(HDC, int, int, int, int) { return 0; }
inline BOOL OffsetRect(LPRECT, int, int) { return TRUE; }
inline BOOL RestoreDC(HDC, int) { return TRUE; }
inline int SaveDC(HDC) { return 1; }
inline int SetBkMode(HDC, int) { return 0; }
inline HRGN CreateRectRgn(int, int, int, int) { return nullptr; }
inline HRGN CreateRectRgnIndirect(const RECT*) { return nullptr; }
inline int GetClipRgn(HDC, HRGN) { return 0; }
inline BOOL IntersectRect(LPRECT, const RECT*, const RECT*) { return TRUE; }
inline BOOL Polyline(HDC, const POINT*, int) { return TRUE; }
inline int SelectClipRgn(HDC, HRGN) { return 0; }
inline HIMAGELIST ImageList_Create(int, int, UINT, int, int) { return nullptr; }
inline int ImageList_AddIcon(HIMAGELIST, HICON) { return 0; }
inline HICON ImageList_GetIcon(HIMAGELIST, int, UINT) { return nullptr; }
inline int ImageList_AddMasked(HIMAGELIST, HBITMAP, COLORREF) { return 0; }
inline int ImageList_ReplaceIcon(HIMAGELIST, int, HICON) { return 0; }
inline int GetDIBits(HDC, HBITMAP, UINT, UINT, void*, BITMAPINFO*, UINT) { return 0; }
inline int SetDIBits(HDC, HBITMAP, UINT, UINT, const void*, const BITMAPINFO*, UINT) { return 0; }
inline BOOL GetIconInfo(HICON, ICONINFO*) { return FALSE; }
inline HICON CreateIconIndirect(ICONINFO*) { return nullptr; }
inline void InitCommonControls() {}
inline BOOL IsWindowEnabled(HWND) { return TRUE; }
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p) (void)(p)
#endif
inline BOOL PtInRect(const RECT*, POINT) { return FALSE; }
inline BOOL BringWindowToTop(HWND) { return TRUE; }
inline HBITMAP CreateBitmap(int, int, UINT, UINT, const void*) { return nullptr; }
inline HBRUSH CreatePatternBrush(HBITMAP) { return nullptr; }
inline BOOL PatBlt(HDC, int, int, int, int, DWORD) { return TRUE; }
inline BOOL SetBrushOrgEx(HDC, int, int, POINT*) { return TRUE; }
inline void mouse_event(DWORD, DWORD, DWORD, DWORD, uintptr_t) {}
inline HRESULT CoInitialize(void*) { return S_OK; }

#ifndef PATINVERT
#define PATINVERT 0x005A0049
#endif
#ifndef MOUSEEVENTF_LEFTUP
#define MOUSEEVENTF_LEFTUP 0x0004
#endif
#ifndef CW_USEDEFAULT
#define CW_USEDEFAULT ((int)0x80000000)
#endif
#ifndef IDC_SIZENS
#define IDC_SIZENS 32645
#define IDC_SIZEWE 32644
#endif
#ifndef LWA_COLORKEY
#define LWA_COLORKEY 0x00000001
#endif
#ifndef SM_CXVIRTUALSCREEN
#define SM_CXVIRTUALSCREEN 78
#define SM_CYVIRTUALSCREEN 79
#define SM_XVIRTUALSCREEN 76
#define SM_YVIRTUALSCREEN 77
#endif
#ifndef SW_SHOWNOACTIVATE
#define SW_SHOWNOACTIVATE 4
#endif
#ifndef WS_EX_TOPMOST
#define WS_EX_TOPMOST 0x00000008
#define WS_EX_TRANSPARENT 0x00000020
#endif

struct CREATESTRUCTW {
	DWORD dwExStyle = 0;
	const wchar_t* lpszClass = nullptr;
	const wchar_t* lpszName = nullptr;
	long x = 0, y = 0, cx = 0, cy = 0;
	HWND hwndParent = nullptr;
	HMENU hMenu = nullptr;
	HINSTANCE hInstance = nullptr;
	LPVOID lpCreateParams = nullptr;
};
typedef CREATESTRUCTW CREATESTRUCT;
typedef CREATESTRUCTW* LPCREATESTRUCT;

struct NOTIFYICONDATAW {
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	wchar_t szTip[128];
	DWORD dwState;
	DWORD dwStateMask;
	wchar_t szInfo[256];
	UINT uTimeoutOrVersion;
	wchar_t szInfoTitle[64];
	DWORD dwInfoFlags;
};
typedef NOTIFYICONDATAW NOTIFYICONDATA;
#ifndef NIM_ADD
#define NIM_ADD 0x00000000
#define NIM_MODIFY 0x00000001
#define NIM_DELETE 0x00000002
#define NIF_ICON 0x00000002
#define NIF_MESSAGE 0x00000001
#define NIF_TIP 0x00000004
#endif
inline BOOL Shell_NotifyIconW(DWORD, NOTIFYICONDATAW*) { return TRUE; }
#define Shell_NotifyIcon Shell_NotifyIconW
inline HBITMAP LoadBitmapW(HINSTANCE, const wchar_t*) { return nullptr; }
#define LoadBitmap LoadBitmapW
inline HWND GetAncestor(HWND, UINT) { return nullptr; }
#ifndef GA_ROOTOWNER
#define GA_ROOTOWNER 3
#endif
#ifndef WH_MOUSE_LL
#define WH_MOUSE_LL 14
#endif
#ifndef WS_EX_PALETTEWINDOW
#define WS_EX_PALETTEWINDOW 0x00000188
#endif
#ifndef LVN_GETDISPINFO
#define LVN_GETDISPINFO (0U - 177U)
#define NM_RCLICK (0U - 5U)
#endif
struct MSLLHOOKSTRUCT {
	POINT pt{};
	DWORD mouseData = 0;
	DWORD flags = 0;
	DWORD time = 0;
	uintptr_t dwExtraInfo = 0;
};

inline BOOL FillRect(HDC, const RECT*, HBRUSH) { return TRUE; }
inline BOOL SetWindowTextW(HWND, const wchar_t*) { return TRUE; }
#define SetWindowText SetWindowTextW
inline DWORD WaitForSingleObject(HANDLE, DWORD) { return WAIT_OBJECT_0; }
inline HWND GetFocus() { return nullptr; }
inline BOOL SetFocus(HWND) { return TRUE; }
inline UINT GetDlgItemTextW(HWND, int, wchar_t*, int) { return 0; }
inline UINT GetDlgItemTextA(HWND, int, char*, int) { return 0; }
#define GetDlgItemText GetDlgItemTextW
inline BOOL SetDlgItemTextW(HWND, int, const wchar_t*) { return TRUE; }
inline BOOL SetDlgItemTextA(HWND, int, const char*) { return TRUE; }
#define SetDlgItemText SetDlgItemTextW
inline UINT GetDlgCtrlID(HWND) { return 0; }
inline LRESULT SendDlgItemMessageW(HWND, int, UINT, WPARAM, LPARAM) { return 0; }
#define SendDlgItemMessage SendDlgItemMessageW
using NppSubclassProc = LRESULT (*)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
inline LRESULT DefSubclassProc(HWND, UINT, WPARAM, LPARAM) { return 0; }
inline BOOL SetWindowSubclass(HWND, NppSubclassProc, UINT_PTR, DWORD_PTR) { return TRUE; }
inline BOOL RemoveWindowSubclass(HWND, NppSubclassProc, UINT_PTR) { return TRUE; }
inline BOOL GetWindowSubclass(HWND, NppSubclassProc, UINT_PTR, DWORD_PTR*) { return FALSE; }
inline HINSTANCE ShellExecuteW(HWND, const wchar_t*, const wchar_t*, const wchar_t*, const wchar_t*, int) { return nullptr; }
#define ShellExecute ShellExecuteW
#ifndef WM_INITDIALOG
#define WM_INITDIALOG 0x0110
#define WM_GETDLGCODE 0x0087
#define WM_CTLCOLORDLG 0x0136
#define WM_CTLCOLOREDIT 0x0133
#define WM_CTLCOLORSTATIC 0x0138
#define WM_GETTEXTLENGTH 0x000E
#define WM_NCDESTROY 0x0082
#define WM_PRINTCLIENT 0x0318
#define WM_CHAR 0x0102
#endif
#ifndef EM_SETSEL
#define EM_SETSEL 0x00B1
#endif
#ifndef EN_CHANGE
#define EN_CHANGE 0x0300
#endif
#ifndef DLGC_WANTALLKEYS
#define DLGC_WANTALLKEYS 0x0004
#endif
#ifndef IDOK
#define IDOK 1
#endif
inline LRESULT SendMessageW(HWND, UINT, WPARAM, LPARAM) { return 0; }
#ifndef SendMessage
#define SendMessage SendMessageW
#endif
inline int ImageList_SetIconSize(void*, int, int) { return 0; }
inline BOOL ImageList_BeginDrag(HIMAGELIST, int, int, int) { return TRUE; }
inline BOOL ImageList_DragEnter(HWND, int, int) { return TRUE; }
inline BOOL ImageList_DragMove(int, int) { return TRUE; }
inline void ImageList_DragShowNolock(BOOL) {}
inline void ImageList_EndDrag() {}
inline BOOL ImageList_GetIconSize(HIMAGELIST, int*, int*) { return FALSE; }
inline int ShowCursor(BOOL) { return 0; }
#ifndef NPP_STUB_REGISTERWINDOWMESSAGE
#define NPP_STUB_REGISTERWINDOWMESSAGE
inline UINT RegisterWindowMessageW(const wchar_t*) { return 0xC000; }
#define RegisterWindowMessage RegisterWindowMessageW
#endif
#ifndef SM_CYMENU
#define SM_CYMENU 3
#define SM_CXSIZEFRAME 32
#define SM_CXBORDER 5
#endif
#ifndef NPP_STUB_DRAWFOCUSRECT
#define NPP_STUB_DRAWFOCUSRECT
inline BOOL DrawFocusRect(HDC, const RECT*) { return TRUE; }
inline BOOL FrameRect(HDC, const RECT*, HBRUSH) { return TRUE; }
#endif
#ifndef NPP_STUB_LSTRCPYN
#define NPP_STUB_LSTRCPYN
inline wchar_t* lstrcpynW(wchar_t* d, const wchar_t* s, int n) {
	if (!d || n <= 0) return d;
	wcsncpy(d, s ? s : L"", static_cast<size_t>(n - 1));
	d[n - 1] = L'\0';
	return d;
}
#define lstrcpyn lstrcpynW
inline char* lstrcpynA(char* d, const char* s, int n) {
	if (!d || n <= 0) return d;
	strncpy(d, s ? s : "", static_cast<size_t>(n - 1));
	d[n - 1] = '\0';
	return d;
}
#endif
#ifndef HKL
typedef void* HKL;
#endif
inline HKL GetKeyboardLayout(DWORD) { return reinterpret_cast<HKL>(static_cast<DWORD_PTR>(0x0409)); }
#ifndef NPP_STUB_SETDLGITEMINT
#define NPP_STUB_SETDLGITEMINT
inline BOOL SetDlgItemInt(HWND, int, UINT, BOOL) { return TRUE; }
#endif
inline void _wsplitpath_s(const wchar_t*, wchar_t*, size_t, wchar_t*, size_t, wchar_t*, size_t, wchar_t*, size_t) {}
inline HHOOK SetWindowsHookExW(int, HOOKPROC, HINSTANCE, DWORD) { return nullptr; }
#define SetWindowsHookEx SetWindowsHookExW
inline BOOL UnhookWindowsHookEx(HHOOK) { return TRUE; }
inline LRESULT CallNextHookEx(HHOOK, int nCode, WPARAM wParam, LPARAM lParam) {
	(void)wParam;
	(void)lParam;
	return nCode;
}

#ifndef HC_ACTION
#define HC_ACTION 0
#endif
#ifndef WH_CALLWNDPROC
#define WH_CALLWNDPROC 4
#endif
struct CWPSTRUCT {
	LPARAM lParam = 0;
	WPARAM wParam = 0;
	UINT message = 0;
	HWND hwnd = nullptr;
};

#ifndef CF_UNICODETEXT
#define CF_UNICODETEXT 13
#endif
#ifndef COLOR_WINDOW
#define COLOR_WINDOW 5
#define COLOR_WINDOWTEXT 8
#define COLOR_HIGHLIGHT 13
#define COLOR_HIGHLIGHTTEXT 14
#endif
#ifndef CWP_SKIPINVISIBLE
#define CWP_SKIPINVISIBLE 0x0001
#endif
#ifndef MONITOR_DEFAULTTONEAREST
#define MONITOR_DEFAULTTONEAREST 2
#endif
#ifndef SW_SHOWMINIMIZED
#define SW_SHOWMINIMIZED 2
#endif
#ifndef SW_RESTORE
#define SW_RESTORE 9
#endif
#ifndef TPM_RETURNCMD
#define TPM_RETURNCMD 0x0100
#define TPM_LEFTALIGN 0x0000
#define TPM_LEFTBUTTON 0x0000
#define TPM_RIGHTALIGN 0x0008
#define TPM_RIGHTBUTTON 0x0002
#define TPM_VERTICAL 0x0040
#define TPM_LAYOUTRTL 0x8000
#endif
struct TPMPARAMS {
	UINT cbSize = 0;
	RECT rcExclude{};
};
#ifndef RDW_INVALIDATE
#define RDW_INVALIDATE 0x0001
#define RDW_ERASE 0x0004
#define RDW_FRAME 0x0400
#define RDW_ALLCHILDREN 0x0080
#define RDW_UPDATENOW 0x0100
#endif
#ifndef WS_POPUP
#define WS_POPUP 0x80000000L
#endif
#ifndef WM_SETFONT
#define WM_SETFONT 0x0030
#endif
#ifndef WPF_RESTORETOMAXIMIZED
#define WPF_RESTORETOMAXIMIZED 0x0002
#endif
#ifndef SWP_FRAMECHANGED
#define SWP_FRAMECHANGED 0x0020
#define SWP_DRAWFRAME SWP_FRAMECHANGED
#endif
#ifndef SM_CXSCREEN
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXMINTRACK 34
#endif
#ifndef TB_GETRECT
#define TB_GETRECT (WM_USER + 51)
#endif
#ifndef SECURITY_NT_AUTHORITY
#define SECURITY_NT_AUTHORITY {0,0,0,0,0,5}
#define SECURITY_BUILTIN_DOMAIN_RID 0x00000020
#endif
#include <cstring>
#ifndef ZeroMemory
#define ZeroMemory(dst, len) std::memset((dst), 0, (len))
#endif
#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
struct URL_COMPONENTSW {
	DWORD dwStructSize = 0;
	wchar_t* lpszScheme = nullptr;
	DWORD dwSchemeLength = 0;
	wchar_t* lpszHostName = nullptr;
	DWORD dwHostNameLength = 0;
	wchar_t* lpszUrlPath = nullptr;
	DWORD dwUrlPathLength = 0;
	wchar_t* lpszExtraInfo = nullptr;
	DWORD dwExtraInfoLength = 0;
};
typedef URL_COMPONENTSW URL_COMPONENTS;

inline BOOL InternetCrackUrlW(const wchar_t*, DWORD, DWORD, URL_COMPONENTSW* comp, DWORD) {
	if (comp) comp->dwStructSize = sizeof(URL_COMPONENTSW);
	return TRUE;
}
inline BOOL InternetCrackUrlW(const wchar_t* url, DWORD urlLen, DWORD flags, URL_COMPONENTSW* comp) {
	return InternetCrackUrlW(url, urlLen, flags, comp, 0);
}
#define InternetCrackUrl InternetCrackUrlW

#ifndef HWND_TOP
#define HWND_TOP ((HWND)0)
#define HWND_BOTTOM ((HWND)1)
#define HWND_TOPMOST ((HWND)-1)
#define HWND_NOTOPMOST ((HWND)-2)
#define HWND_DESKTOP ((HWND)0)
#endif
#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif
#ifndef IDNO
#define IDNO 7
#endif
#ifndef MB_APPLMODAL
#define MB_APPLMODAL 0
#define MB_DEFBUTTON2 0x00000100
#define MB_ICONEXCLAMATION MB_ICONWARNING
#define MB_ICONQUESTION 0x00000020
#define MB_YESNOCANCEL 0x00000003
#endif
#ifndef MF_STRING
#define MF_STRING 0x00000000
#define MF_DISABLED 0x00000002
#define MFS_GRAYED 0x00000003
#define MFT_SEPARATOR 0x00000800
#define MFT_STRING 0x00000000
#define MFT_RADIOCHECK 0x00000200
#endif
#ifndef MIIM_STATE
#define MIIM_STATE 0x00000001
#define MIIM_ID 0x00000002
#define MIIM_SUBMENU 0x00000004
#define MIIM_STRING 0x00000040
#define MIIM_FTYPE 0x00000100
#define MIIM_CHECKMARKS 0x00000008
#endif
#ifndef MFT_BITMAP
#define MFT_BITMAP 0x00000004
#endif
#ifndef MFS_ENABLED
#define MFS_ENABLED 0x00000000
#endif
#ifndef LR_LOADMAP3DCOLORS
#define LR_LOADMAP3DCOLORS 0x00002000
#define LR_LOADTRANSPARENT 0x00000020
#endif
#ifndef MAKELONG
#define MAKELONG(l, h) ((LONG)(((WORD)(DWORD_PTR)(l)) | ((DWORD)((WORD)(DWORD_PTR)(h)) << 16)))
#endif
#ifndef MAKELPARAM
#define MAKELPARAM(l, h) ((LPARAM)MAKELONG(l, h))
#endif
typedef POINT* LPPOINT;
typedef void* PSID;
#ifndef ULONG_PTR
typedef uintptr_t ULONG_PTR;
#endif
#ifndef DOMAIN_ALIAS_RID_ADMINS
#define DOMAIN_ALIAS_RID_ADMINS 0x00000220
#endif

struct MENUITEMINFOW {
	UINT cbSize;
	UINT fMask;
	UINT fType;
	UINT fState;
	UINT wID;
	HMENU hSubMenu;
	HBITMAP hbmpChecked;
	HBITMAP hbmpUnchecked;
	ULONG_PTR dwItemData;
	LPWSTR dwTypeData;
	UINT cch;
	HBITMAP hbmpItem;
};
typedef MENUITEMINFOW MENUITEMINFO;
inline BOOL InsertMenuItemW(HMENU, UINT, BOOL, MENUITEMINFOW*) { return TRUE; }
#define InsertMenuItem InsertMenuItemW

struct MONITORINFO {
	DWORD cbSize;
	RECT rcMonitor;
	RECT rcWork;
	DWORD dwFlags;
};

inline HMENU AppendMenuW(HMENU, UINT, UINT_PTR, const wchar_t*) { return nullptr; }
#define AppendMenu AppendMenuW
inline BOOL CheckMenuRadioItem(HMENU, UINT, UINT, UINT, UINT) { return TRUE; }
inline HWND ChildWindowFromPointEx(HWND, POINT, UINT) { return nullptr; }
inline BOOL CloseClipboard() { return TRUE; }
inline BOOL CopyFileW(const wchar_t*, const wchar_t*, BOOL) { return TRUE; }
#define CopyFile CopyFileW
inline BOOL CreateDirectoryW(const wchar_t*, void*) { return TRUE; }
#define CreateDirectory CreateDirectoryW
inline HANDLE CreateMutexW(void*, BOOL, const wchar_t*) { return nullptr; }
#define CreateMutex CreateMutexW
inline HMENU CreatePopupMenu() { return nullptr; }
typedef DWORD (*LPTHREAD_START_ROUTINE)(void*);
inline HANDLE CreateThread(void*, size_t, LPTHREAD_START_ROUTINE, void*, DWORD, DWORD*) { return nullptr; }
inline BOOL DeleteMenu(HMENU, UINT, UINT) { return TRUE; }
inline BOOL DragQueryPoint(HDROP, POINT*) { return FALSE; }
inline BOOL EnableMenuItem(HMENU, UINT, UINT) { return TRUE; }
inline HANDLE GetClipboardData(UINT) { return nullptr; }
inline HMENU GetMenu(HWND) { return nullptr; }
inline int GetMenuItemCount(HMENU) { return 0; }
inline BOOL GetMenuItemInfoW(HMENU, UINT, BOOL, void*) { return FALSE; }
#define GetMenuItemInfo GetMenuItemInfoW
inline UINT GetMenuState(HMENU, UINT, UINT) { return 0; }
inline int GetMenuStringW(HMENU, UINT, wchar_t*, int, UINT) { return 0; }
#define GetMenuString GetMenuStringW
inline BOOL GetMonitorInfoW(HMONITOR, MONITORINFO*) { return FALSE; }
#define GetMonitorInfo GetMonitorInfoW
inline COLORREF GetSysColor(int) { return 0; }
inline BOOL GetWindowPlacement(HWND, void*) { return TRUE; }
inline void* GlobalLock(HANDLE) { return nullptr; }
inline BOOL GlobalUnlock(HANDLE) { return TRUE; }
inline BOOL IsChild(HWND, HWND) { return FALSE; }
inline BOOL IsClipboardFormatAvailable(UINT) { return FALSE; }
inline BOOL IsIconic(HWND) { return FALSE; }
inline BOOL IsWindowVisible(HWND) { return TRUE; }
inline int MapWindowPoints(HWND, HWND, POINT*, int) { return 0; }
inline BOOL MessageBeep(UINT) { return TRUE; }
inline HMONITOR MonitorFromWindow(HWND, DWORD) { return nullptr; }
inline BOOL MoveWindow(HWND hwnd, int x, int y, int w, int h, BOOL) {
	if (hwnd) hwnd->setGeometry(x, y, w, h);
	return TRUE;
}
inline BOOL OpenClipboard(HWND) { return TRUE; }
inline BOOL PostMessageW(HWND, UINT, WPARAM, LPARAM) { return TRUE; }
#ifndef PostMessage
#define PostMessage PostMessageW
#endif
inline BOOL RemoveMenu(HMENU, UINT, UINT) { return TRUE; }
inline BOOL SetMenuItemInfoW(HMENU, UINT, BOOL, void*) { return TRUE; }
#define SetMenuItemInfo SetMenuItemInfoW
inline LONG_PTR SetWindowLongPtrW(HWND, int, LONG_PTR) { return 0; }
#define SetWindowLongPtr SetWindowLongPtrW
inline BOOL SetWindowPlacement(HWND, const void*) { return TRUE; }
inline void Sleep(DWORD) {}
inline BOOL TrackPopupMenu(HMENU, UINT, int, int, int, HWND, void*) { return FALSE; }
inline UINT TrackPopupMenuEx(HMENU, UINT, int, int, HWND, void*) { return 0; }
inline int TranslateAcceleratorW(HWND, void*, void*) { return 0; }
#define TranslateAccelerator TranslateAcceleratorW
inline int WideCharToMultiByte(UINT, DWORD, const wchar_t*, int, char*, int, const char*, BOOL*) { return 0; }
struct SID_IDENTIFIER_AUTHORITY { BYTE Value[6]; };
inline BOOL AllocateAndInitializeSid(void*, BYTE, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, void**) { return FALSE; }
inline BOOL CheckTokenMembership(HANDLE, void*, BOOL*) { return FALSE; }
inline void FreeSid(void*) {}
inline SID_IDENTIFIER_AUTHORITY AdministratorsGroup = {};

inline DWORD GetLastError() { return 0; }
inline BOOL CloseHandle(HANDLE) { return TRUE; }
inline BOOL FlushFileBuffers(HANDLE) { return TRUE; }
inline BOOL WriteFile(HANDLE, const void*, DWORD, DWORD*, void*) { return TRUE; }
inline BOOL SetFileTime(HANDLE, const FILETIME*, const FILETIME*, const FILETIME*) { return TRUE; }
inline BOOL SetFileAttributesW(const wchar_t*, DWORD) { return TRUE; }
#define SetFileAttributes SetFileAttributesW
inline DWORD GetFinalPathNameByHandle(HANDLE, wchar_t*, DWORD, DWORD) { return 0; }
inline HANDLE FindFirstStreamW(const wchar_t*, int, WIN32_FIND_STREAM_DATA*, DWORD) {
	return INVALID_HANDLE_VALUE;
}
inline BOOL FindClose(HANDLE) { return TRUE; }
inline BOOL PathIsNetworkPath(const wchar_t*) { return FALSE; }
inline int MultiByteToWideChar(UINT, DWORD, const char*, int, wchar_t*, int) { return 0; }
inline int LCMapStringEx(const wchar_t*, DWORD, const wchar_t*, int, wchar_t*, int, void*, void*, void*) {
	return 0;
}

// String stubs - use standard library on Linux
#ifdef __linux__
#define _wcsicmp wcscasecmp
#define _wcsnicmp wcsncasecmp
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define strnicmp strncasecmp
#else
inline int _wcsicmp(const wchar_t*, const wchar_t*) { return 0; }
inline int _wcsnicmp(const wchar_t*, const wchar_t*, size_t) { return 0; }
#endif

#ifndef IDIGNORE
#define IDIGNORE 10
#define IDRETRY 4
#endif
typedef size_t rsize_t;
#ifndef _TRUNCATE
#define _TRUNCATE (static_cast<rsize_t>(-1))
#endif
inline errno_t strncpy_s(char* dest, rsize_t destsz, const char* src, rsize_t count) {
	if (!dest || destsz == 0) return EINVAL;
	if (!src) { dest[0] = '\0'; return EINVAL; }
	const rsize_t n = (count == _TRUNCATE || count >= destsz) ? destsz - 1 : count;
	strncpy(dest, src, n);
	dest[n] = '\0';
	return 0;
}
inline errno_t wcsncpy_s(wchar_t* dest, rsize_t destsz, const wchar_t* src, rsize_t count) {
	if (!dest || destsz == 0) return EINVAL;
	if (!src) { dest[0] = L'\0'; return EINVAL; }
	const rsize_t n = (count == _TRUNCATE || count >= destsz) ? destsz - 1 : count;
	wcsncpy(dest, src, n);
	dest[n] = L'\0';
	return 0;
}
template<size_t N>
inline errno_t wcsncpy_s(wchar_t (&dest)[N], const wchar_t* src, rsize_t count) {
	return wcsncpy_s(dest, N, src, count);
}
#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT 258L
#define WAIT_IO_COMPLETION 192L
#endif
inline DWORD WaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD) { return WAIT_TIMEOUT; }
#define COPY_FILE_NO_BUFFERING 0x00001000
#define MOVEFILE_REPLACE_EXISTING 0x00000001
#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002
#define REPLACEFILE_IGNORE_ACL_ERRORS 0x00000004
#ifndef VOID
typedef void VOID;
#endif
#ifndef FILE_FLAG_OVERLAPPED
#define FILE_FLAG_OVERLAPPED 0x40000000
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#define FILE_SHARE_DELETE 0x00000004
#define FILE_LIST_DIRECTORY 0x0001
#endif
struct OVERLAPPED {
	ULONG_PTR Internal = 0;
	ULONG_PTR InternalHigh = 0;
	DWORD Offset = 0;
	DWORD OffsetHigh = 0;
	HANDLE hEvent = nullptr;
};
typedef OVERLAPPED* LPOVERLAPPED;
struct FILE_NOTIFY_INFORMATION {
	DWORD NextEntryOffset = 0;
	DWORD Action = 0;
	DWORD FileNameLength = 0;
	wchar_t FileName[1];
};
typedef VOID (CALLBACK* LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD, DWORD, LPOVERLAPPED);
inline BOOL ReadDirectoryChangesW(HANDLE, void*, DWORD, BOOL, DWORD, LPDWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE) { return FALSE; }
inline BOOL CancelIo(HANDLE) { return TRUE; }
inline DWORD QueueUserAPC(void (*)(ULONG_PTR), HANDLE, ULONG_PTR) { return 0; }
inline DWORD SleepEx(DWORD, BOOL) { return 0; }
inline DWORD WaitForSingleObjectEx(HANDLE, DWORD, BOOL) { return 0; }
inline DWORD GetLongPathNameW(const wchar_t*, wchar_t*, DWORD) { return 0; }
#define GetLongPathName GetLongPathNameW
#ifndef FILE_ACTION_MODIFIED
#define FILE_ACTION_ADDED 1
#define FILE_ACTION_REMOVED 2
#define FILE_ACTION_MODIFIED 3
#define FILE_ACTION_RENAMED_OLD_NAME 4
#define FILE_ACTION_RENAMED_NEW_NAME 5
#define FILE_NOTIFY_CHANGE_FILE_NAME 0x00000001
#define FILE_NOTIFY_CHANGE_SIZE 0x00000002
#define FILE_NOTIFY_CHANGE_LAST_WRITE 0x00000010
#endif
inline BOOL CopyFileExW(const wchar_t*, const wchar_t*, void*, void*, BOOL*, DWORD) { return FALSE; }
#define CopyFileEx CopyFileExW
inline BOOL ReplaceFileW(const wchar_t*, const wchar_t*, const wchar_t*, DWORD, void*, void*) { return FALSE; }
#define ReplaceFile ReplaceFileW
inline DWORD ExpandEnvironmentStringsW(const wchar_t*, wchar_t*, DWORD) { return 0; }
#define ExpandEnvironmentStrings ExpandEnvironmentStringsW
inline int SHCreateDirectory(HWND, const wchar_t*) { return 0; }
inline BOOL EnableWindow(HWND, BOOL) { return TRUE; }
inline void PostQuitMessage(int) {}
inline LRESULT DefWindowProcW(HWND, UINT, WPARAM, LPARAM) { return 0; }
#define DefWindowProc DefWindowProcW
inline HDWP BeginDeferWindowPos(int) { return nullptr; }
inline HDWP DeferWindowPos(HDWP, HWND, HWND, int, int, int, int, UINT) { return nullptr; }
inline BOOL EndDeferWindowPos(HDWP) { return TRUE; }
inline BOOL ClientToScreen(HWND, LPPOINT) { return TRUE; }
inline HFONT CreateFontIndirectW(const void*) { return nullptr; }
#define CreateFontIndirect CreateFontIndirectW
inline BOOL DispatchMessageW(const void*) { return TRUE; }
#define DispatchMessage DispatchMessageW
inline int DrawTextW(HDC, const wchar_t*, int, LPRECT, UINT) { return 0; }
#define DrawText DrawTextW
inline BOOL EndDialog(HWND, INT_PTR) { return TRUE; }
inline BOOL GetComboBoxInfo(HWND, void*) { return FALSE; }
inline BOOL GetCursorPos(LPPOINT) { return TRUE; }
inline BOOL GetMessageW(void*, HWND, UINT, UINT) { return FALSE; }
#define GetMessage GetMessageW
inline int GetObjectW(HANDLE, int, void*) { return 0; }
#define GetObject GetObjectW
inline HBRUSH GetSysColorBrush(int) { return nullptr; }
inline BOOL GetTextExtentPoint32W(HDC, const wchar_t*, int, void*) { return FALSE; }
#define GetTextExtentPoint32 GetTextExtentPoint32W
inline LONG_PTR GetWindowLongPtrW(HWND, int) { return 0; }
#define GetWindowLongPtr GetWindowLongPtrW
inline int GetWindowTextLengthW(HWND) { return 0; }
#define GetWindowTextLength GetWindowTextLengthW
inline BOOL ResetEvent(HANDLE) { return TRUE; }
#ifndef WS_EX_OVERLAPPEDWINDOW
#define WS_EX_OVERLAPPEDWINDOW 0x00000300
#define WS_EX_APPWINDOW 0x00040000
#define WS_EX_TOOLWINDOW 0x00000080
#define WS_CHILD 0x40000000
#define WS_CLIPSIBLINGS 0x04000000
#define SS_PATHELLIPSIS 0x00008000
#define SS_RIGHT 0x00000002
#define BS_DEFPUSHBUTTON 0x00000001
#define PBS_SMOOTH 0x00000001
#define PBM_SETRANGE (WM_USER + 1)
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOOWNERZORDER 0x0200
#define SWP_NOCOPYBITS 0x0100
#define SM_CXPADDEDBORDER 92
#define SM_CXFRAME 32
#define SM_CYFRAME 33
#define SM_CYCAPTION 4
#define NULL_BRUSH 5
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_MOVE 0x0003
#define WM_NCMOUSEMOVE 0x00A0
#define WM_NCLBUTTONUP 0x00A2
#define WC_STATIC L"Static"
#define WC_BUTTON L"Button"
#define PROGRESS_CLASS L"msctls_progress32"
#endif

#ifndef GWL_EXSTYLE
#define GWL_EXSTYLE (-20)
#define WS_EX_LAYERED 0x00080000
#define LWA_ALPHA 0x00000002
#endif
#ifndef MOVEFILE_WRITE_THROUGH
#define MOVEFILE_WRITE_THROUGH 0x00000008
#endif
#ifndef ERROR_ACCESS_DENIED
#define ERROR_ACCESS_DENIED 5
#endif
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
#ifndef LOCALE_USER_DEFAULT
#define LOCALE_USER_DEFAULT 0x0400
#endif
#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET 1
#endif
#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 0x0026
#define SHGFP_TYPE_CURRENT 0
#endif
#ifndef GetFileExInfoStandard
#define GetFileExInfoStandard 0
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32s 0
#define VER_PLATFORM_WIN32_WINDOWS 1
#define VER_PLATFORM_WIN32_NT 2
#define VER_NT_WORKSTATION 0x00000001
#endif
#ifndef PROCESSOR_ARCHITECTURE_INTEL
#define PROCESSOR_ARCHITECTURE_INTEL 0
#define PROCESSOR_ARCHITECTURE_AMD64 9
#define PROCESSOR_ARCHITECTURE_IA64 6
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif
struct OSVERSIONINFO {
	DWORD dwOSVersionInfoSize = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuildNumber = 0;
	DWORD dwPlatformId = 0;
	wchar_t szCSDVersion[128]{};
};
struct OSVERSIONINFOEX : OSVERSIONINFO {
	WORD wServicePackMajor = 0;
	WORD wServicePackMinor = 0;
	WORD wSuiteMask = 0;
	BYTE wProductType = 0;
};
struct SYSTEM_INFO {
	WORD wProcessorArchitecture = 0;
	DWORD dwPageSize = 0;
	void* lpMinimumApplicationAddress = nullptr;
	void* lpMaximumApplicationAddress = nullptr;
	DWORD_PTR dwActiveProcessorMask = 0;
	DWORD dwNumberOfProcessors = 0;
	DWORD dwProcessorType = 0;
	DWORD dwAllocationGranularity = 0;
	WORD wProcessorLevel = 0;
	WORD wProcessorRevision = 0;
};
typedef SYSTEM_INFO* LPSYSTEM_INFO;
struct TEXTMETRIC {
	LONG tmHeight = 0;
	LONG tmAscent = 0;
	LONG tmDescent = 0;
	LONG tmExternalLeading = 0;
};
struct SHFILEOPSTRUCTW {
	HWND hwnd = nullptr;
	UINT wFunc = 0;
	const wchar_t* pFrom = nullptr;
	const wchar_t* pTo = nullptr;
	WORD fFlags = 0;
	BOOL fAnyOperationsAborted = FALSE;
	void* hNameMappings = nullptr;
	const wchar_t* lpszProgressTitle = nullptr;
};
typedef SHFILEOPSTRUCTW SHFILEOPSTRUCT;
#define FO_DELETE 0x0003
#define FOF_ALLOWUNDO 0x0040
#define FOF_NOCONFIRMATION 0x0010
inline int SHFileOperationW(SHFILEOPSTRUCTW*) { return 0; }
#define SHFileOperation SHFileOperationW
inline BOOL GetFileAttributesExW(const wchar_t*, int, WIN32_FILE_ATTRIBUTE_DATA*) { return FALSE; }
#define GetFileAttributesEx GetFileAttributesExW
inline BOOL GetVersionExW(OSVERSIONINFO*) { return FALSE; }
#define GetVersionEx GetVersionExW
inline void GetSystemInfo(SYSTEM_INFO*) {}
inline BOOL SetLayeredWindowAttributes(HWND, COLORREF, BYTE, DWORD) { return TRUE; }
inline int GetACP() { return 65001; }
inline HWND FindWindowW(const wchar_t*, const wchar_t*) { return nullptr; }
#define FindWindow FindWindowW
inline DWORD GetCurrentDirectoryW(DWORD, wchar_t*) { return 0; }
#define GetCurrentDirectory GetCurrentDirectoryW
inline int GetClassNameW(HWND, wchar_t*, int) { return 0; }
#define GetClassName GetClassNameW
inline BOOL ModifyMenuW(HMENU, UINT, UINT, UINT_PTR, const wchar_t*) { return TRUE; }
#define ModifyMenu ModifyMenuW
inline HWND WindowFromPoint(POINT) { return nullptr; }
inline DWORD GetTempPathW(DWORD, wchar_t*) { return 0; }
#define GetTempPath GetTempPathW
inline void SetLastError(DWORD) {}
typedef int (CALLBACK* FONTENUMPROCEXW)(const LOGFONTW*, const TEXTMETRIC*, DWORD, LPARAM);
inline int EnumFontFamiliesExW(HDC, LOGFONTW*, FONTENUMPROCEXW, LPARAM, DWORD) { return 0; }
#define EnumFontFamiliesEx EnumFontFamiliesExW
inline HRESULT SHGetFolderPathW(HWND, int, HANDLE, DWORD, wchar_t*) { return E_FAIL; }
#define SHGetFolderPath SHGetFolderPathW
inline int CompareFileTime(const FILETIME*, const FILETIME*) { return 0; }
inline BOOL FileTimeToSystemTime(const FILETIME*, void*) { return FALSE; }
inline BOOL SystemTimeToTzSpecificLocalTime(void*, const void*, void*) { return FALSE; }
inline int GetDateFormatW(DWORD, DWORD, const void*, const wchar_t*, wchar_t*, int) { return 0; }
#define GetDateFormat GetDateFormatW
inline int GetTimeFormatW(DWORD, DWORD, const void*, const wchar_t*, wchar_t*, int) { return 0; }
#define GetTimeFormat GetTimeFormatW
inline int _stprintf_s(wchar_t* buf, size_t n, const wchar_t* fmt, ...) {
	if (!buf || n == 0) return -1;
	va_list ap;
	va_start(ap, fmt);
	const int r = vswprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}
inline errno_t wcscat_s(wchar_t* dest, size_t destsz, const wchar_t* src) {
	if (!dest || destsz == 0) return EINVAL;
	if (!src) { dest[0] = L'\0'; return EINVAL; }
	const size_t len = wcsnlen(dest, destsz);
	if (len >= destsz) return EINVAL;
	wcsncat(dest, src, destsz - len - 1);
	return 0;
}
template<size_t N>
inline errno_t wcscat_s(wchar_t (&dest)[N], const wchar_t* src) {
	return wcscat_s(dest, N, src);
}
inline int sprintf_s(char* buf, size_t n, const char* fmt, ...) {
	if (!buf || n == 0) return -1;
	va_list ap;
	va_start(ap, fmt);
	const int r = vsnprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}

#ifndef _locale_t
typedef __locale_t _locale_t;
#endif
inline _locale_t _create_locale(int, const char*) { return nullptr; }
inline void _free_locale(_locale_t) {}

#ifndef PVOID
typedef void* PVOID;
#endif
#ifndef HKEY_CURRENT_USER
#define HKEY_CURRENT_USER ((HKEY)(ULONG_PTR)0x80000001)
#endif
#ifndef RRF_RT_REG_DWORD
#define RRF_RT_REG_DWORD 0x00000018
#endif
#ifndef LSTATUS
typedef LONG LSTATUS;
#endif
inline LSTATUS RegGetValueW(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD) { return ERROR_FILE_NOT_FOUND; }
#define RegGetValue RegGetValueW
#ifndef DEFAULT_GUI_FONT
#define DEFAULT_GUI_FONT 17
#endif
inline HRESULT LoadIconWithScaleDown(HINSTANCE, LPCWSTR, int, int, HICON*) { return E_FAIL; }

#include "WindowsCommCtrlStubs.h"

#endif // WINDOWS_COMPAT_H
