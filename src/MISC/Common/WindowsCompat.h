#ifndef WINDOWS_COMPAT_H
#define WINDOWS_COMPAT_H

// Windows compatibility layer for Linux port

#include <QtCore>
#include <QtWidgets>
#include <QString>
#include <QByteArray>
#include <QFile>

#include "WindowsStubs.h"

#ifdef _MSC_VER
    #include "MISC/Common/WindowsCompat.h"
#else

// Basic type aliases
typedef quint8 BYTE;
typedef qint32 LONG;
typedef quint32 DWORD;
typedef qint64 LONGLONG;
typedef quint64 ULONGLONG;
typedef wchar_t WCHAR;
typedef void* HANDLE;
typedef void* HWND;
typedef void* HGDIOBJ;
typedef void* HBRUSH;
typedef void* HFONT;
typedef void* HBITMAP;
typedef void* HCURSOR;
typedef void* HICON;
typedef void* HMENU;
typedef void* HDC;
typedef void* HRGN;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HACCEL;
typedef void* HDWP;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* LPSECURITY_ATTRIBUTES;

#define HANDLE_NULL nullptr
#define INVALID_HANDLE_VALUE nullptr
#define MAX_PATH 260
#define TRUE 1
#define FALSE 0
#define MAXDWORD 0xFFFFFFFF

// Windows message types
#define WM_COMMAND 0x0111
#define WM_NOTIFY 0x004E
#define WM_INITDIALOG 0x0110
#define WM_CLOSE 0x0010
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_MOVE 0x0003
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_MOUSEMOVE 0x0200
#define WM_MOUSEWHEEL 0x020A
#define WM_HSCROLL 0x0114
#define WM_VSCROLL 0x0115
#define WM_PAINT 0x000F
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_GETTEXT 0x000D
#define WM_SETTEXT 0x000C
#define WM_GETTEXTLENGTH 0x000E
#define WM_CUT 0x0300
#define WM_COPY 0x0301
#define WM_PASTE 0x0302
#define WM_CLEAR 0x0303
#define WM_UNDO 0x0304
#define WM_CONTEXTMENU 0x007B
#define WM_SYSCOMMAND 0x0112
#define WM_USER 0x0400
#define WM_DPICHANGED 0x02E0

// Control notifications
#define BN_CLICKED 0
#define EN_CHANGE 0x0300
#define EN_UPDATE 0x0400

// Virtual key codes
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
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

// Window styles
#define WS_OVERLAPPED 0x00000000
#define WS_POPUP 0x80000000
#define WS_CHILD 0x40000000
#define WS_MINIMIZE 0x20000000
#define WS_VISIBLE 0x10000000
#define WS_DISABLED 0x08000000
#define WS_CLIPSIBLINGS 0x04000000
#define WS_CLIPCHILDREN 0x02000000
#define WS_MAXIMIZE 0x01000000
#define WS_CAPTION 0x00C00000
#define WS_BORDER 0x00800000
#define WS_DLGFRAME 0x00400000
#define WS_VSCROLL 0x00200000
#define WS_HSCROLL 0x00100000
#define WS_SYSMENU 0x00080000
#define WS_THICKFRAME 0x00040000
#define WS_MINIMIZEBOX 0x00020000
#define WS_MAXIMIZEBOX 0x00010000
#define WS_EX_LEFT 0x00000000
#define WS_EX_TOOLWINDOW 0x00000080
#define WS_EX_WINDOWEDGE 0x00000100
#define WS_EX_CLIENTEDGE 0x00000200

// Button styles
#define BS_PUSHBUTTON 0x00000000
#define BS_DEFPUSHBUTTON 0x00000001
#define BS_CHECKBOX 0x00000002
#define BS_RADIOBUTTON 0x00000004
#define BS_GROUPBOX 0x00000007
#define BS_NOTIFY 0x00004000
#define BS_CENTER 0x00000300
#define BS_LEFT 0x00000100
#define BS_RIGHT 0x00000200

// Edit styles
#define ES_LEFT 0x0000
#define ES_CENTER 0x0001
#define ES_RIGHT 0x0002
#define ES_MULTILINE 0x0004
#define ES_UPPERCASE 0x0008
#define ES_LOWERCASE 0x0010
#define ES_PASSWORD 0x0020
#define ES_AUTOVSCROLL 0x0040
#define ES_AUTOHSCROLL 0x0080
#define ES_READONLY 0x0800
#define ES_WANTRETURN 0x1000
#define ES_NUMBER 0x2000

// Static styles  
#define SS_LEFT 0x0000L
#define SS_CENTER 0x0001L
#define SS_RIGHT 0x0002L
#define SS_ICON 0x0003L
#define SS_BLACKRECT 0x0004L
#define SS_WHITERECT 0x0005L
#define SS_GRAYRECT 0x0006L
#define SS_BLACKFRAME 0x0007L
#define SS_WHITEFRAME 0x0009L

// Dialog codes
#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDRETRY 4
#define IDIGNORE 5
#define IDYES 6
#define IDNO 7
#define IDCLOSE 8
#define IDHELP 9

// System metrics
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXVSCROLL 2
#define SM_CYVSCROLL 3
#define SM_CYCAPTION 4
#define SM_CXBORDER 5
#define SM_CYBORDER 6

// MessageBox types
#define MB_OK 0x00000000
#define MB_OKCANCEL 0x00000001
#define MB_YESNO 0x00000004
#define MB_YESNOCANCEL 0x00000003
#define MB_ICONSTOP 0x00000010
#define MB_ICONQUESTION 0x00000020
#define MB_ICONEXCLAMATION 0x00000030
#define MB_ICONINFORMATION 0x00000040

// File constants
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

// Rect and point
typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *PPOINT;

typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE;

typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT;

#define SetRect(r,l,t,rt,b) do { (r).left=l;(r).top=t;(r).right=rt;(r).bottom=b; } while(0)
#define IsRectEmpty(r) ((r).left >= (r).right || (r).top >= (r).bottom)
#define WidthRect(r) ((r).right - (r).left)
#define HeightRect(r) ((r).bottom - (r).top)
#define OffsetRect(r,dx,dy) do { (r).left+=(dx);(r).right+=(dx);(r).top+=(dy);(r).bottom+=(dy); } while(0)
#define MAKELONG(a,b) ((LONG)(((DWORD)((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define HIWORD(x) ((WORD)((((DWORD)(x)) >> 16) & 0xFFFF))
#define LOWORD(x) ((WORD)(((DWORD)(x)) & 0xFFFF))

// Clipboard
#define CF_TEXT 1
#define CF_UNICODETEXT 13

// Colors
#define COLOR_WINDOW 5
#define COLOR_WINDOWTEXT 8
#define COLOR_HIGHLIGHT 13
#define COLOR_HIGHLIGHTTEXT 14

#endif

#endif // WINDOWS_COMPAT_H
