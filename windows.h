#pragma once

#ifndef _WINDOWS_STUB_H
#define _WINDOWS_STUB_H

// If WindowsCompat.h was already included, skip redefinitions
#if defined(_WINDOWS_COMPAT_H_DEFINED)
#ifndef Sleep
#define Sleep(ms) ((void)0)
#endif
#ifndef GetTickCount
#define GetTickCount() (0)
#endif
#ifndef OutputDebugStringA
#define OutputDebugStringA(msg) ((void)0)
#endif
#ifndef OutputDebugStringW
#define OutputDebugStringW(msg) ((void)0)
#endif
#ifndef OutputDebugString
#define OutputDebugString OutputDebugStringA
#endif
#ifndef ReleaseDC
#define ReleaseDC(hwnd, hdc) (1)
#endif
#ifndef DeleteDC
#define DeleteDC(hdc) (1)
#endif
#ifndef DeleteObject
#define DeleteObject(obj) (1)
#endif
#ifndef RegCloseKey
#define RegCloseKey(hkey) (0)
#endif
#endif // _WINDOWS_COMPAT_H_DEFINED
#ifndef BYTE
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
typedef int32_t  LONG;
typedef int64_t  LONGLONG;
typedef uint64_t ULONG;
typedef int32_t  BOOL;
typedef int      INT;
typedef unsigned int UINT;
typedef intptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;
typedef uintptr_t UINT_PTR;
typedef intptr_t INT_PTR;
typedef uintptr_t ULONG_PTR;
#endif
#ifndef HANDLE
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HGLOBAL;
typedef void* HLOCAL;
typedef void* HDC;
typedef void* HGDIOBJ;
typedef void* HWND;
typedef void* HICON;
typedef void* HCURSOR;
typedef void* HBRUSH;
typedef void* HPEN;
typedef void* HFONT;
typedef void* HBITMAP;
typedef void* HMENU;
typedef void* HKEY;
typedef void* HRSRC;
typedef void* HDWP;
typedef void* HCRYPTPROV;
typedef void* HCRYPTKEY;
typedef void* HCRYPTHASH;
typedef void* HDEVINFO;
typedef const void* LPCVOID;
typedef void* LPVOID;
typedef void* PVOID;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef wchar_t WCHAR;
typedef BYTE* LPBYTE;
typedef DWORD* LPDWORD;
typedef LONG* PLONG;
typedef WORD* LPWORD;

// Handle constants
#ifdef NULL
#undef NULL
#endif
#define NULL 0
#define TRUE 1
#define FALSE 0
#define INFINITE 0xFFFFFFFF
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_ATOM 0

// File access modes
#define GENERIC_READ  0x80000000
#define GENERIC_WRITE 0x40000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_ALL   0x10000000
#define FILE_SHARE_READ  0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define CREATE_NEW      1
#define CREATE_ALWAYS   2
#define OPEN_EXISTING   3
#define OPEN_ALWAYS     4
#define TRUNCATE_EXISTING 5

// File attributes
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_FLAG_NO_BUFFERING 0x20000000

// Error codes
#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_ACCESS_DENIED 5
#define ERROR_INVALID_HANDLE 6
#define ERROR_NOT_ENOUGH_MEMORY 8
#define ERROR_INVALID_PARAMETER 87

// Window messages
#define WM_NULL              0x0000
#define WM_CREATE            0x0001
#define WM_DESTROY           0x0002
#define WM_MOVE              0x0003
#define WM_SIZE              0x0005
#define WM_ACTIVATE          0x0006
#define WM_SETFOCUS          0x0007
#define WM_KILLFOCUS         0x0008
#define WM_ENABLE            0x000A
#define WM_SETREDRAW         0x000B
#define WM_SETTEXT           0x000C
#define WM_GETTEXT           0x000D
#define WM_GETTEXTLENGTH     0x000E
#define WM_PAINT             0x000F
#define WM_CLOSE             0x0010
#define WM_QUIT              0x0012
#define WM_ERASEBKGND        0x0014
#define WM_SHOWWINDOW        0x0018
#define WM_CTLCOLOR          0x0019
#define WM_SETFONT           0x0030
#define WM_GETFONT           0x0031
#define WM_COMMAND           0x0111
#define WM_NOTIFY            0x004E
#define WM_HSCROLL           0x0114
#define WM_VSCROLL           0x0115
#define WM_INITDIALOG        0x0110
#define WM_CONTEXTMENU       0x007B
#define WM_CHAR              0x0102
#define WM_KEYDOWN           0x0100
#define WM_KEYUP             0x0101
#define WM_MOUSEMOVE         0x0200
#define WM_LBUTTONDOWN       0x0201
#define WM_LBUTTONUP         0x0202
#define WM_LBUTTONDBLCLK     0x0203
#define WM_RBUTTONDOWN       0x0204
#define WM_RBUTTONUP         0x0205
#define WM_MBUTTONDOWN       0x0207
#define WM_MBUTTONUP         0x0208
#define WM_MOUSEWHEEL        0x020A
#define WM_USER              0x0400
#define WM_REFLECT           0x2000

// Virtual keys
#define VK_BACK        0x08
#define VK_TAB         0x09
#define VK_RETURN      0x0D
#define VK_SHIFT       0x10
#define VK_CONTROL     0x11
#define VK_MENU        0x12
#define VK_ESCAPE      0x1B
#define VK_SPACE       0x20
#define VK_END         0x23
#define VK_HOME        0x24
#define VK_LEFT        0x25
#define VK_UP          0x26
#define VK_RIGHT       0x27
#define VK_DOWN        0x28
#define VK_DELETE      0x2E
#define VK_F1          0x70
#define VK_F2          0x71
#define VK_F3          0x72
#define VK_F4          0x73
#define VK_F5          0x74
#define VK_F6          0x75
#define VK_F7          0x76
#define VK_F8          0x77
#define VK_F9          0x78
#define VK_F10         0x79
#define VK_F11         0x7A
#define VK_F12         0x7B

// Colors
#define CLR_INVALID 0xFFFFFFFF
#define CLR_NONE 0xFFFFFFFF
#define COLOR_WINDOW 5
#define COLOR_WINDOWTEXT 8
#define COLOR_HIGHLIGHT 13
#define COLOR_HIGHLIGHTTEXT 14

// GDI objects
#define NULL_PEN        2
#define NULL_BRUSH      5

// Stock objects
#define OEM_FIXED_FONT  10
#define ANSI_FIXED_FONT 11
#define ANSI_VAR_FONT   12
#define SYSTEM_FONT     13
#define DEVICE_DEFAULT_FONT 14
#define DEFAULT_PALETTE 15
#define SYSTEM_FIXED_FONT  16
#define DEFAULT_GUI_FONT 17

// Button styles
#define BS_PUSHBUTTON       0x00000000
#define BS_DEFPUSHBUTTON    0x00000001
#define BS_CHECKBOX         0x00000002
#define BS_AUTOCHECKBOX     0x00000003
#define BS_RADIOBUTTON      0x00000004
#define BS_3STATE           0x00000005
#define BS_AUTO3STATE       0x00000006
#define BS_GROUPBOX         0x00000007
#define BS_USERBUTTON       0x00000008
#define BS_AUTORADIOBUTTON  0x00000009
#define BS_PUSHLIKE         0x00001000
#define BS_LEFTTEXT         0x00000020

// Window styles
#define WS_OVERLAPPED   0x00000000
#define WS_POPUP        0x80000000
#define WS_CHILD        0x40000000
#define WS_MINIMIZE     0x20000000
#define WS_VISIBLE      0x10000000
#define WS_DISABLED     0x08000000
#define WS_CLIPSIBLINGS 0x04000000
#define WS_CLIPCHILDREN 0x02000000
#define WS_MAXIMIZE     0x01000000
#define WS_CAPTION      0x00C00000
#define WS_BORDER       0x00800000
#define WS_DLGFRAME     0x00400000
#define WS_VSCROLL      0x00200000
#define WS_HSCROLL      0x00100000
#define WS_SYSMENU      0x00080000
#define WS_THICKFRAME   0x00040000
#define WS_MINIMIZEBOX  0x00020000
#define WS_MAXIMIZEBOX  0x00010000
#define WS_EX_DLGMODALFRAME 0x00000001
#define WS_EX_NOPARENTNOTIFY 0x00000004
#define WS_EX_TOPMOST   0x00000008
#define WS_EX_ACCEPTFILES 0x00000010
#define WS_EX_TRANSPARENT 0x00000020

// MessageBox flags
#define MB_OK               0x00000000
#define MB_OKCANCEL         0x00000001
#define MB_ABORTRETRYIGNORE 0x00000002
#define MB_YESNOCANCEL      0x00000003
#define MB_YESNO            0x00000004
#define MB_RETRYCANCEL      0x00000005
#define MB_ICONHAND         0x00000010
#define MB_ICONQUESTION     0x00000020
#define MB_ICONEXCLAMATION  0x00000030
#define MB_ICONASTERISK     0x00000040
#define MB_ICONINFORMATION  MB_ICONASTERISK
#define MB_ICONSTOP         MB_ICONHAND
#define MB_DEFBUTTON1       0x00000000
#define MB_DEFBUTTON2       0x00000100
#define MB_DEFBUTTON3       0x00000200

// Dialog IDs
#define IDOK        1
#define IDCANCEL    2
#define IDABORT     3
#define IDRETRY     4
#define IDIGNORE    5
#define IDYES       6
#define IDNO        7
#define IDCLOSE     8
#define IDHELP      9

// Calling conventions
#define WINAPI
#define CALLBACK
#define CONST const

// Include unistd.h for usleep() on POSIX systems
#include <unistd.h>

// Win32 function stubs (with guards against redefinition)
#ifndef Sleep
inline void Sleep(DWORD ms) { ::usleep(ms * 1000); }
#endif
#ifndef GetTickCount
inline DWORD GetTickCount() { 
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts); 
    return (DWORD)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000); 
}
#endif
#ifndef OutputDebugStringA
inline void OutputDebugStringA(const char* msg) { fprintf(stderr, "%s", msg); }
#endif
#ifndef OutputDebugStringW
inline void OutputDebugStringW(const wchar_t* msg) { fwprintf(stderr, L"%ls", msg); }
#endif
#ifndef OutputDebugString
#define OutputDebugString OutputDebugStringA
#endif

// GDI stub functions (with guards)
#ifndef ReleaseDC
inline int ReleaseDC(HWND, HDC) { return 1; }
#endif
#ifndef DeleteDC
inline int DeleteDC(HDC) { return 1; }
#endif
#ifndef DeleteObject
inline int DeleteObject(HGDIOBJ) { return 1; }
#endif

// Registry stubs (with guards)
inline LONG RegOpenKeyExW(HKEY, LPCWSTR, DWORD, DWORD, HKEY*) { return ERROR_SUCCESS; }
#ifndef RegCloseKey
inline LONG RegCloseKey(HKEY) { return ERROR_SUCCESS; }
#endif
inline LONG RegQueryValueExW(HKEY, LPCWSTR, DWORD*, DWORD*, LPBYTE, DWORD*) { return ERROR_SUCCESS; }
inline LONG RegSetValueExW(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD) { return ERROR_SUCCESS; }

// Dialog constants
#define DS_SETFOREGROUND 0x0002
#define DS_MODALFRAME 0x80
#define DS_CENTER 0x0800
#define DS_3DLOOK 0x0004
#define DS_FIXEDSYS 0x0008

// Scintilla types (forward declared)
namespace Scintilla::Internal {
    class ScintillaQt;
}
using ScintillaQt = Scintilla::Internal::ScintillaQt;

// NppPosition structure (renamed to avoid conflict with INVALID_POSITION)
#ifndef NPPPOSITION_DEFINED
struct NppPosition {
    intptr_t _firstVisibleLine = 0;
    intptr_t _startPos = 0;
    intptr_t _endPos = 0;
    intptr_t _xOffset = 0;
    intptr_t _selMode = 0;
    intptr_t _scrollWidth = 1;
    intptr_t _wrapCount = 0;
    intptr_t _offset = 0;
};
#define NPPPOSITION_DEFINED
#endif

// MAX_PATH constant
#define MAX_PATH 260

#endif // _WINDOWS_STUB_H

// Additional list view and control constants
#define LVS_EX_FULLROWSELECT 0x00000020
#define LBN_DBLCLK 4

// Remove RGB macro conflict - we'll undef it
#undef RGB
#endif // _WINDOWS_STUB_H
