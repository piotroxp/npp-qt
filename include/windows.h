#pragma once
/**
 * windows.h — Linux stub for Qt/Scintilla compilation
 * Maps common Win32 types to C++ standard equivalents.
 */
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>
#include <cerrno>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <sched.h>

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef int64_t  LONGLONG;
typedef uint64_t ULONGLONG;
typedef void*    HANDLE;
typedef void*    HWND;
typedef void*    HINSTANCE;
typedef void*    HMODULE;
typedef void*    HDC;
typedef void*    HBITMAP;
typedef void*    HBRUSH;
typedef void*    HPEN;
typedef void*    HFONT;
typedef void*    HRGN;
typedef void*    HCURSOR;
typedef void*    HICON;
typedef void*    HMENU;
typedef void*    HKL;
typedef void*    HACCEL;
typedef void*    HDWP;
typedef int      BOOL;
typedef unsigned int UINT;
typedef int      INT;
typedef char     CHAR;
typedef wchar_t  WCHAR;
typedef CHAR*     LPSTR;
typedef const CHAR* LPCSTR;
typedef WCHAR*    LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef void*    PVOID;
typedef void*    LPVOID;
typedef const void* LPCVOID;
typedef uintptr_t ULONG_PTR;
typedef intptr_t LONG_PTR;
typedef int64_t LPARAM;
typedef uint64_t WPARAM;
typedef int64_t LRESULT;
typedef unsigned int ATOM;
typedef void* HKEY;
typedef void* PHKEY;
typedef int64_t FILETIME;
typedef int64_t LARGE_INTEGER;

#define WINAPI
#define CALLBACK
#define CONST const
#define TRUE 1
#define FALSE 0
#define MAX_PATH 260
#define INFINITE 0xFFFFFFFF

// Common Windows macros — use function-like form to avoid C++ STL conflicts
// Use _max/_min for C++ code to avoid std::min/std::max conflicts
#define NOMINMAX 1

// Handle invalid values
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)(-1))
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_ATOM 0

// Access rights
#define GENERIC_READ  0x80000000
#define GENERIC_WRITE 0x40000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_ALL   0x10000000
#define FILE_SHARE_READ  0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define CREATE_NEW      1
#define CREATE_ALWAYS   2
#define OPEN_EXISTING   3
#define OPEN_ALWAYS     4
#define TRUNCATE_EXISTING 5

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
#define WM_WININICHANGE      0x001A
#define WM_SETTINGCHANGE     WM_WININICHANGE
#define WM_QUERYSENDSESSION  0x0032
#define WM_GETMINMAXINFO     0x0024
#define WM_PAINTICON         0x0026
#define WM_ICONERASEBKGND    0x0027
#define WM_NEXTDLGCTL        0x0028
#define WM_QUEUESYNC         0x0023
#define WM_GETDLGCODE        0x0087
#define WM_ENTERIDLE         0x0121
#define WM_COMPAREITEM       0x0039
#define WM_WINDOWPOSCHANGING  0x0046
#define WM_WINDOWPOSCHANGED   0x0047
#define WM_CONTEXTMENU       0x007B
#define WM_STYLECHANGED      0x007D
#define WM_SETICON           0x0080
#define WM_NCCREATE          0x0081
#define WM_NCDESTROY         0x0082
#define WM_NCCALCSIZE        0x0083
#define WM_NCHITTEST         0x0084
#define WM_NCPAINT           0x0085
#define WM_NCACTIVATE        0x0086
#define WM_GETICON           0x007F
#define WM_NCLBUTTONDOWN     0x00A1
#define WM_NCLBUTTONUP       0x00A2
#define WM_NCLBUTTONDBLCLK   0x00A3
#define WM_NCRBUTTONDOWN     0x00A4
#define WM_NCRBUTTONUP       0x00A5
#define WM_NCRBUTTONDBLCLK   0x00A6
#define WM_KEYDOWN           0x0100
#define WM_KEYUP             0x0101
#define WM_CHAR              0x0102
#define WM_DEADCHAR          0x0103
#define WM_SYSKEYDOWN        0x0104
#define WM_SYSKEYUP          0x0105
#define WM_SYSCHAR           0x0106
#define WM_IME_CHAR          0x0286
#define WM_INITDIALOG        0x0110
#define WM_COMMAND           0x0111
#define WM_SYSCOMMAND        0x0112
#define WM_TIMER             0x0113
#define WM_HSCROLL           0x0114
#define WM_VSCROLL           0x0115
#define WM_INITMENU          0x0116
#define WM_INITMENUPOPUP     0x0117
#define WM_MENUSELECT        0x011F
#define WM_MENUCHAR          0x0120
#define WM_ENTERIDLE         0x0121
#define WM_CTLCOLORMSGBOX    0x0132
#define WM_CTLCOLOREDIT      0x0133
#define WM_CTLCOLORLISTBOX   0x0134
#define WM_CTLCOLORBTN       0x0135
#define WM_CTLCOLORDLG       0x0136
#define WM_CTLCOLORSCROLLBAR 0x0137
#define WM_CTLCOLORSTATIC    0x0138
#define WM_MOUSEWHEEL        0x020A
#define WM_MOUSEHWHEEL       0x020E
#define WM_MOUSEMOVE         0x0200
#define WM_LBUTTONDOWN       0x0201
#define WM_LBUTTONUP         0x0202
#define WM_LBUTTONDBLCLK     0x0203
#define WM_RBUTTONDOWN       0x0204
#define WM_RBUTTONUP         0x0205
#define WM_RBUTTONDBLCLK     0x0206
#define WM_MBUTTONDOWN       0x0207
#define WM_MBUTTONUP         0x0208
#define WM_MBUTTONDBLCLK     0x0209
#define WM_XBUTTONDOWN       0x020B
#define WM_XBUTTONUP         0x020C
#define WM_XBUTTONDBLCLK     0x020D
#define WM_USER              0x0400

// Virtual key codes
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
#define VK_SNAPSHOT    0x2C
#define VK_INSERT      0x2D
#define VK_DELETE      0x2E
#define VK_HELP        0x2F
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
#define VK_F13         0x7C
#define VK_F14         0x7D
#define VK_F15         0x7E
#define VK_F16         0x7F
#define VK_F17         0x80
#define VK_F18         0x81
#define VK_F19         0x82
#define VK_F20         0x83
#define VK_F21         0x84
#define VK_F22         0x85
#define VK_F23         0x86
#define VK_F24         0x87
#define VK_NUMLOCK     0x90
#define VK_SCROLL      0x91

// Mouse buttons
#define MK_LBUTTON    0x0001
#define MK_RBUTTON    0x0002
#define MK_SHIFT      0x0004
#define MK_CONTROL    0x0008
#define MK_MBUTTON    0x0010
#define MK_XBUTTON1   0x0020
#define MK_XBUTTON2   0x0040

// System metrics indices
#define SM_CXSCREEN        0
#define SM_CYSCREEN        1
#define SM_CXVSCROLL       2
#define SM_CYHSCROLL       3
#define SM_CYCAPTION       4
#define SM_CXBORDER        5
#define SM_CYBORDER        6
#define SM_CXDLGFRAME      7
#define SM_CYDLGFRAME      8
#define SM_CXFRAME         32
#define SM_CYFRAME         33
#define SM_CXSIZE          30
#define SM_CYSIZE          31

// Window styles
#define WS_OVERLAPPED     0x00000000
#define WS_POPUP          0x80000000
#define WS_CHILD          0x40000000
#define WS_MINIMIZE       0x20000000
#define WS_VISIBLE        0x10000000
#define WS_DISABLED       0x08000000
#define WS_CLIPSIBLINGS   0x04000000
#define WS_CLIPCHILDREN   0x02000000
#define WS_MAXIMIZE       0x01000000
#define WS_CAPTION        0x00C00000
#define WS_BORDER         0x00800000
#define WS_DLGFRAME       0x00400000
#define WS_VSCROLL        0x00200000
#define WS_HSCROLL        0x00100000
#define WS_SYSMENU        0x00080000
#define WS_THICKFRAME     0x00040000
#define WS_MINIMIZEBOX    0x00020000
#define WS_MAXIMIZEBOX    0x00010000
#define WS_EX_TOOLWINDOW  0x00000080
#define WS_EX_APPWINDOW   0x00040000
#define WS_EX_CLIENTEDGE  0x00000200
#define WS_EX_WINDOWEDGE  0x00000100

// Clipboard formats
#define CF_TEXT           1
#define CF_BITMAP         2
#define CF_METAFILEPICT   3
#define CF_SYLK           4
#define CF_DIF            5
#define CF_TIFF           6
#define CF_OEMTEXT        7
#define CF_DIB            8
#define CF_PALETTE        9
#define CF_PENDATA        10
#define CF_RIFF           11
#define CF_WAVE           12
#define CF_UNICODETEXT    13
#define CF_ENHMETAFILE    14
#define CF_HDROP          15

// File attributes
#define FILE_ATTRIBUTE_READONLY     0x00000001
#define FILE_ATTRIBUTE_HIDDEN       0x00000002
#define FILE_ATTRIBUTE_SYSTEM        0x00000004
#define FILE_ATTRIBUTE_DIRECTORY     0x00000010
#define FILE_ATTRIBUTE_ARCHIVE      0x00000020
#define FILE_ATTRIBUTE_NORMAL       0x00000080
#define FILE_ATTRIBUTE_TEMPORARY    0x00000100
#define FILE_ATTRIBUTE_COMPRESSED   0x00000800

// Registry
#define HKEY_CLASSES_ROOT     ((HKEY)(void*)(intptr_t)0x80000000)
#define HKEY_CURRENT_USER     ((HKEY)(void*)(intptr_t)0x80000001)
#define HKEY_LOCAL_MACHINE    ((HKEY)(void*)(intptr_t)0x80000002)
#define HKEY_USERS            ((HKEY)(void*)(intptr_t)0x80000003)
#define HKEY_PERFORMANCE_DATA ((HKEY)(void*)(intptr_t)0x80000004)
#define HKEY_CURRENT_CONFIG   ((HKEY)(void*)(intptr_t)0x80000005)

#define REG_SZ               1
#define REG_EXPAND_SZ        2
#define REG_BINARY           3
#define REG_DWORD            4
#define REG_DWORD_LITTLE_ENDIAN 4
#define REG_DWORD_BIG_ENDIAN    5
#define REG_LINK              6
#define REG_MULTI_SZ          7

#define KEY_READ             0x20019
#define KEY_WRITE            0x20006
#define KEY_ALL_ACCESS       0xF003F

#define REG_CREATED_NEW_KEY     1
#define REG_OPENED_EXISTING_KEY 2

#define ERROR_SUCCESS           0L
#define ERROR_FILE_NOT_FOUND    2L
#define ERROR_PATH_NOT_FOUND    3L
#define ERROR_ACCESS_DENIED     5L
#define ERROR_INVALID_HANDLE     6L
#define ERROR_NOT_ENOUGH_MEMORY  8L
#define ERROR_INVALID_PARAMETER 87L

// LoadLibrary / GetProcAddress
#define DONT_RESOLVE_DLL_REFERENCES         0x00000001
#define LOAD_LIBRARY_AS_DATAFILE            0x00000002
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE       0x00000020

// Color constants
#define CLR_INVALID    0xFFFFFFFF
#define COLOR_SCROLLBAR           0
#define COLOR_BACKGROUND          1
#define COLOR_ACTIVECAPTION       2
#define COLOR_INACTIVECAPTION     3
#define COLOR_MENU                 4
#define COLOR_WINDOW              5
#define COLOR_WINDOWFRAME         6
#define COLOR_MENUTEXT            7
#define COLOR_WINDOWTEXT           8
#define COLOR_CAPTIONTEXT          9
#define COLOR_ACTIVEBORDER        10
#define COLOR_INACTIVEBORDER      11
#define COLOR_APPWORKSPACE        12
#define COLOR_HIGHLIGHT           13
#define COLOR_HIGHLIGHTTEXT       14
#define COLOR_BTNFACE             15
#define COLOR_BTNSHADOW           16
#define COLOR_GRAYTEXT             17
#define COLOR_BTNTEXT              18
#define COLOR_INACTIVECAPTIONTEXT  19
#define COLOR_BTNHIGHLIGHT        20
#define COLOR_3DDKSHADOW          21
#define COLOR_3DLIGHT             22
#define COLOR_INFOBK              24
#define COLOR_INFOTEXT            23
#define COLOR_HOTTRACKING         26
#define COLOR_GRADIENTACTIVECAPTION 27
#define COLOR_GRADIENTINACTIVECAPTION 28

// Rectangle
struct RECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};
typedef RECT* LPRECT;
typedef const RECT* LPCRECT;

// Point
struct POINT {
    LONG x;
    LONG y;
};
typedef POINT* LPPOINT;
typedef const POINT* LPCPOINT;

// Size
struct SIZE {
    LONG cx;
    LONG cy;
};
typedef SIZE* LPSIZE;

// Msg
struct MSG {
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
};
typedef MSG* LPMSG;

// HINSTANCE/HMODULE
typedef int (*FARPROC)();

// DllMain stub signature
#define DllMain DllMain_win32
typedef int (*PDLLMAIN)(HINSTANCE, DWORD, LPVOID);

// Sleep
inline void Sleep(DWORD ms) { ::usleep(ms * 1000); }
inline DWORD GetTickCount() { struct timespec ts; ::clock_gettime(CLOCK_MONOTONIC, &ts); return ts.tv_sec * 1000 + ts.tv_nsec / 1000000; }
inline DWORD GetCurrentThreadId() { return (DWORD)(uintptr_t)pthread_self(); }
inline DWORD GetCurrentProcessId() { return (DWORD)getpid(); }
inline void OutputDebugStringA(const char* s) { fprintf(stderr, "%s", s); }
inline void OutputDebugStringW(const wchar_t* s) { fwprintf(stderr, L"%ls", s); }
#define OutputDebugString OutputDebugStringA
inline HMODULE GetModuleHandleA(const char* m) { return nullptr; }
#define GetModuleHandle GetModuleHandleA
inline DWORD GetLastError() { return errno; }
inline int MulDiv(int a, int b, int c) { return c ? (a * b) / c : -1; }

// itoa/utoa equivalents
inline char* itoa(int val, char* buf, int base) {
    if (base == 10) { snprintf(buf, 64, "%d", val); }
    else if (base == 16) { snprintf(buf, 64, "%x", val); }
    else if (base == 8) { snprintf(buf, 64, "%o", val); }
    return buf;
}
inline char* utoa(unsigned int val, char* buf, int base) {
    if (base == 10) { snprintf(buf, 64, "%u", val); }
    else if (base == 16) { snprintf(buf, 64, "%x", val); }
    else if (base == 8) { snprintf(buf, 64, "%o", val); }
    return buf;
}
inline wchar_t* itow(int val, wchar_t* buf, int base) {
    if (base == 10) { swprintf(buf, 64, L"%d", val); }
    else if (base == 16) { swprintf(buf, 64, L"%x", val); }
    else if (base == 8) { swprintf(buf, 64, L"%o", val); }
    return buf;
}
#define _itoa itoa
#define _itoa_s itoa
#define _utoa utoa
#define _ultoa utoa
#define _itow itow

// Menu
#define MF_ENABLED         0x00000000L
#define MF_GRAYED         0x00000001L
#define MF_DISABLED       0x00000002L
#define MF_BITMAP         0x00000004L
#define MF_STRING         0x00000000L
#define MF_POPUP          0x00000010L
#define MF_SEPARATOR      0x00000800L
#define MF_CHECKED        0x00000008L
#define MF_UNCHECKED      0x00000000L
#define MF_OWNERDRAW      0x00000100L
#define MF_RIGHTJUSTIFY   0x00004000L

// TrackPopupMenu
#define TPM_LEFTALIGN      0x0000L
#define TPM_CENTERALIGN    0x0004L
#define TPM_RIGHTALIGN     0x0008L
#define TPM_TOPALIGN       0x0000L
#define TPM_VCENTERALIGN   0x0010L
#define TPM_BOTTOMALIGN    0x0020L
#define TPM_NONOTIFY       0x0080L
#define TPM_RETURNCMD      0x0100L
#define TPM_LEFTBUTTON     0x0000L
#define TPM_RIGHTBUTTON    0x0002L

// Scroll bar
#define SB_HORZ 0
#define SB_VERT 1
#define SB_CTL  2
#define SB_BOTH 3

// System cursor IDs
#define IDC_ARROW     MAKEINTRESOURCE(32512)
#define IDC_IBEAM     MAKEINTRESOURCE(32513)
#define IDC_WAIT      MAKEINTRESOURCE(32514)
#define IDC_CROSS     MAKEINTRESOURCE(32515)
#define IDC_UPARROW   MAKEINTRESOURCE(32516)
#define IDC_SIZEWE    MAKEINTRESOURCE(32644)
#define IDC_SIZENS    MAKEINTRESOURCE(32645)
#define IDC_SIZEALL   MAKEINTRESOURCE(32646)
#define IDC_NO        MAKEINTRESOURCE(32649)
#define IDC_APPSTARTING MAKEINTRESOURCE(32650)
#define IDC_HELP      MAKEINTRESOURCE(32651)
#define IDC_HAND      MAKEINTRESOURCE(32649)
#define IDC_SIZEALL2  MAKEINTRESOURCE(32647)

#define MAKEINTRESOURCE(i) ((LPSTR)(intptr_t)(i))

// Dialog codes
#define DLGC_WANTARROWS      0x0001
#define DLGC_WANTTAB         0x0002
#define DLGC_WANTALLKEYS     0x0004
#define DLGC_WANTCHARS        0x0008
#define DLGC_DEFPUSHBUTTON   0x0010
#define DLGC_UNDEFPUSHBUTTON 0x0020
#define DLGC_WANTMESSAGE     0x0040
#define DLGC_WANTTAB         0x0002
#define DS_ABSALIGN          0x01L
#define DS_SYSMODAL          0x02L
#define DS_3DLOOK            0x0004L
#define DS_FIXEDSYS          0x0008L
#define DS_NOFAILCREATE       0x0010L
#define DS_LOCALHOUSE         0x0020L
#define DS_SETFONT           0x0040L
#define DS_MODALFRAME        0x0080L
#define DS_CONTEXTHELP       0x0200L
#define DS_SETFOREGROUND     0x0300L
#define DS_NOIDLEMSG         0x0100L

// GetStockObject
#define WHITE_BRUSH     0
#define GRAY_BRUSH      1
#define LTGRAY_BRUSH    2
#define DKGRAY_BRUSH    3
#define BLACK_BRUSH     4
#define NULL_BRUSH      5
#define HOLLOW_BRUSH    NULL_BRUSH
#define WHITE_PEN       0
#define BLACK_PEN       1
#define NULL_PEN        2
#define OEM_FIXED_FONT  10
#define ANSI_FIXED_FONT 11
#define ANSI_VAR_FONT   12
#define SYSTEM_FONT     13
#define DEVICE_DEFAULT_FONT 14
#define DEFAULT_PALETTE 15
#define SYSTEM_FIXED_FONT  16
#define DEFAULT_GUI_FONT 17
