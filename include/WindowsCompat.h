#pragma once
// WindowsCompat.h - Clean Windows type shim for Qt6

#include <cstdint>
#include <QString>
#include <QWidget>
#include <QColor>
#include <QMenu>
#include <QPainter>
#include <QCursor>
#include <QBrush>
#include <QPen>
#include <QRegion>
#include <QList>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <QByteArray>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QVariant>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QInputMethodEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QFocusEvent>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QFileDialog>
#include <QTextCodec>

// === FUNDAMENTAL TYPES (order matters) ===
using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using LONG = std::int32_t;
using LONGLONG = std::int64_t;
using ULONG = std::uint32_t;
using ULONG_PTR = std::uintptr_t;
using DWORD_PTR = std::uintptr_t;
using BOOL = int;
using LONG_PTR = std::intptr_t;
using INT_PTR = std::intptr_t;
using UINT_PTR = std::uintptr_t;
using UINT = std::uint32_t;
using INT = int;
using WPARAM = std::uint64_t;
using LPARAM = std::uint64_t;
using LRESULT = std::int64_t;
using ATOM = std::uint16_t;
using LANGID = unsigned short;
using LCID = unsigned long;
using LCTYPE = unsigned long;
using SIZE_T = std::size_t;
using SSIZE_T = std::ptrdiff_t;
using LONG32 = std::int32_t;
using ULONG32 = std::uint32_t;
using DWORD32 = std::uint32_t;
using INT64 = std::int64_t;
using UINT64 = std::uint64_t;
using DWORD64 = std::uint64_t;
using LONG64 = std::int64_t;
using ULONG64 = std::uint64_t;

// === HANDLE TYPES ===
using HANDLE = void*;
using HMODULE = void*;
using HRSRC = void*;
using HGLOBAL = void*;
using HLOCAL = void*;
using HINSTANCE = void*;
using HDC = void*;
using HBITMAP = void*;
using HBRUSH = void*;
using HPEN = void*;
using HFONT = void*;
using HICON = void*;
using HCURSOR = void*;
using HACCEL = void*;
using HPALETTE = void*;
using HRGN = void*;
using HMENU = void*;
using HWND = void*;
using HKEY = void*;
using HGDIOBJ = void*;
using HENHMETAFILE = void*;
using HMETAFILE = void*;
using HDWP = void*;
using HHOOK = void*;
using HKL = void*;
using HRAWINPUT = void*;
using LPVOID = void*;
using LPCVOID = const void*;
using PVOID = void*;
using REGSAM = DWORD;
using PHKEY = HKEY*;
using DPI_AWARENESS_CONTEXT = void*;
using HRESULT = LONG;
using HALF_PTR = void*;
using UHALF_PTR = void*;

// === STRING TYPES ===
using LPWSTR = wchar_t*;
using LPCWSTR = const wchar_t*;
using PWSTR = wchar_t*;
using PCWSTR = const wchar_t*;
using LPSTR = char*;
using LPCSTR = const char*;
using TCHAR = wchar_t;
using LPBYTE = BYTE*;
using LPDWORD = DWORD*;
using LPWORD = WORD*;
using LPINT = INT*;
using LPUINT = UINT*;
using LPLONG = LONG*;
using LPBOOL = BOOL*;

// === STRUCTS (must be after types) ===
struct POINT { long x=0, y=0; POINT()=default; POINT(long px,long py):x(px),y(py){} };
struct RECT { long left=0, top=0, right=0, bottom=0; RECT()=default; RECT(long l,long t,long r,long b):left(l),top(t),right(r),bottom(b){} };
struct SIZE { long cx=0, cy=0; SIZE()=default; SIZE(long w,long h):cx(w),cy(h){} };
struct MSG { void* hwnd=nullptr; unsigned message=0; WPARAM wParam=0; LPARAM lParam=0; unsigned time=0; POINT pt{0,0}; };
struct FILETIME { DWORD dwLowDateTime=0; DWORD dwHighDateTime=0; };
struct SYSTEMTIME { WORD wYear=0,wMonth=0,wDayOfWeek=0,wDay=0,wHour=0,wMinute=0,wSecond=0,wMilliseconds=0; };
struct WINDOWPLACEMENT { UINT length=0; UINT flags=0; UINT showCmd=0; POINT ptMinPosition{0,0},ptMaxPosition{0,0}; RECT rcNormalPosition; };
struct MINMAXINFO { POINT ptReserved{0,0},ptMaxSize{0,0},ptMaxPosition{0,0},ptMinTrackSize{0,0},ptMaxTrackSize{0,0}; };
struct SECURITY_ATTRIBUTES { DWORD nLength=0; LPVOID lpSecurityDescriptor=nullptr; BOOL bInheritHandle=0; };
struct OVERLAPPED { ULONG_PTR Internal=0; ULONG_PTR InternalHigh=0; DWORD Offset=0; DWORD OffsetHigh=0; HANDLE hEvent=nullptr; };
struct WIN32_FIND_DATAW { DWORD dwFileAttributes=0; FILETIME ftCreationTime{}; FILETIME ftLastAccessTime{}; FILETIME ftLastWriteTime{}; DWORD nFileSizeHigh=0; DWORD nFileSizeLow=0; DWORD dwReserved0=0; DWORD dwReserved1=0; wchar_t cFileName[260]={}; wchar_t cAlternateFileName[14]={}; };
struct HTREEITEM__ { int unused; };
using HTREEITEM = HTREEITEM__*;
struct HDITEMW { DWORD mask=0; int cxy=0; LPWSTR pszText=nullptr; HBITMAP hbm=nullptr; int cchTextMax=0; int fmt=0; LPARAM lParam=0; int iImage=0; int iOrder=0; UINT type=0; };
struct LVITEMW { DWORD mask=0; int iItem=0; int iSubItem=0; UINT state=0; UINT stateMask=0; LPWSTR pszText=nullptr; int cchTextMax=0; int iImage=0; LPARAM lParam=0; int iIndent=0; };
struct TVITEMW { DWORD mask=0; HTREEITEM hItem=nullptr; UINT state=0; UINT stateMask=0; LPWSTR pszText=nullptr; int cchTextMax=0; int iImage=0; int iSelectedImage=0; int cChildren=0; LPARAM lParam=0; };
struct NMHDR { HWND hwndFrom=nullptr; UINT_PTR idFrom=0; UINT code=0; };
struct NMITEMACTIVATE { NMHDR hdr{}; int iItem=-1; int iSubItem=0; UINT uNewState=0; UINT uOldState=0; UINT uChanged=0; POINT ptAction{0,0}; LPARAM lParam=0; UINT uKeyFlags=0; };
struct NMTVDISPINFOW { NMHDR hdr{}; TVITEMW item; };
struct LVDISPINFOW { NMHDR hdr{}; LVITEMW item; };
struct NMLVCUSTOMDRAW { NMHDR hdr{}; DWORD dwDrawStage=0; HDC hdc=nullptr; RECT rc{}; DWORD_PTR dwItemSpec=0; UINT uItemState=0; LPARAM lItemlParam=0; int iSubItem=0; };
struct NMLVGETINFOTIPW { NMHDR hdr{}; DWORD dwFlags=0; int iItem=0; int iSubItem=0; LPWSTR pszText=nullptr; int cchTextMax=0; LPARAM lParam=0; POINT ptAction{0,0}; UINT uKeyFlags=0; };
struct NMTBHOTITEM { NMHDR hdr{}; int idOld=0; int idNew=0; UINT dwFlags=0; };
struct NMPGINFO { NMHDR hdr{}; DWORD dwContext=0; DWORD dwSize=0; RECT rcParent{}; int iActive=0; int iCol=-1; };
struct PSHNOTIFY { NMHDR hdr{}; LPARAM lParam=0; };
struct MEASUREITEMSTRUCT { UINT CtlType=0; UINT CtlID=0; UINT itemID=0; UINT itemWidth=0; UINT itemHeight=0; ULONG_PTR itemData=0; };
struct DRAWITEMSTRUCT { UINT CtlType=0; UINT CtlID=0; UINT itemID=0; UINT itemAction=0; UINT itemState=0; HWND hwndItem=nullptr; HDC hDC=nullptr; RECT rcItem{}; ULONG_PTR itemData=0; };
struct NCCALCSIZE_PARAMS { RECT rgrc[3]; void* lppos=nullptr; };
struct COPYDATASTRUCT { ULONG_PTR dwData=0; DWORD cbData=0; PVOID lpData=nullptr; };

// === CONSTANTS ===
#define FALSE 0
#define TRUE 1
#define NULL 0
#define MAX_PATH 260
#define MAXUINT ((UINT)-1)
#define MAXULONG ((ULONG)-1)
#define MAXDWORD ((DWORD)-1)
#define MAXLONG ((LONG)(0x7FFFFFFF))
#define INFINITE 0xFFFFFFFF
#define LBN_DBLCLK 4
#define LBN_SELCHANGE 1
#define CBN_DBLCLK 4
#define CBN_SELCHANGE 1

#define WS_EX_LEFT 0x00000000L
#define WS_EX_RIGHT 0x00001000L
#define ES_LEFT 0x0000L
#define ES_CENTER 0x0001L
#define ES_RIGHT 0x0002L
#define ES_MULTILINE 0x0004L
#define ES_READONLY 0x0800L
#define ES_NUMBER 0x2000L
#define SS_LEFT 0x0000L
#define SS_CENTER 0x0001L
#define SS_RIGHT 0x0002L
#define BS_PUSHBUTTON 0x00000000L
#define BS_DEFPUSHBUTTON 0x00000001L
#define BS_CHECKBOX 0x00000002L
#define BS_AUTOCHECKBOX 0x00000003L
#define BS_RADIOBUTTON 0x00000004L
#define BS_3STATE 0x00000005L
#define BS_GROUPBOX 0x00000007L
#define CBS_DROPDOWN 0x0002L
#define CBS_DROPDOWNLIST 0x0003L
#define LVS_REPORT 0x0001L
#define LVS_SINGLESEL 0x0004L
#define LVS_SHOWSELALWAYS 0x0008L
#define LVS_NOCOLUMNHEADER 0x4000L
#define LVS_EX_FULLROWSELECT 0x00000020L
#define LVS_EX_GRIDLINES 0x00000001L
#define TVS_HASBUTTONS 0x0001L
#define TVS_HASLINES 0x0002L
#define TVS_EDITLABELS 0x0008L
#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7

#define WM_INITDIALOG 0x0110
#define WM_SIZE 0x0005
#define WM_COMMAND 0x0111
#define WM_NOTIFY 0x004E
#define WM_DESTROY 0x0002
#define WM_CLOSE 0x0010
#define WM_PAINT 0x000F
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_MOUSEMOVE 0x0200
#define WM_MOUSEWHEEL 0x020A
#define WM_TIMER 0x0113
#define WM_VSCROLL 0x0115
#define WM_HSCROLL 0x0114
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_GETTEXTLENGTH 0x000E
#define WM_GETTEXT 0x000D
#define WM_SETTEXT 0x000C
#define WM_CLEAR 0x0303
#define WM_COPY 0x0301
#define WM_CUT 0x0300
#define WM_PASTE 0x0302
#define WM_USER 0x0400
#define WM_APP 0x8000

// === Qt/Windows interoperability ===
inline QRect toQRect(const RECT& r) { return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top); }
inline RECT fromQRect(const QRect& qr) { return RECT(qr.left(), qr.top(), qr.right(), qr.bottom()); }
inline QPoint toQPoint(const POINT& p) { return QPoint(p.x, p.y); }
inline POINT fromQPoint(const QPoint& qp) { return POINT(qp.x(), qp.y()); }
inline QWidget* asWidget(HWND h) { return reinterpret_cast<QWidget*>(h); }
inline HWND asHWND(QWidget* w) { return reinterpret_cast<HWND>(w); }
inline QRect getWorkArea() { if (QScreen* s = QApplication::primaryScreen()) return s->availableGeometry(); return QRect(0,0,1920,1080); }
inline int MessageBox(HWND, const QString& text, const QString& caption, unsigned) { QMessageBox::StandardButtons b = QMessageBox::Ok; QMessageBox mb(QMessageBox::Information, caption, text, b); auto r = mb.exec(); if(r==QMessageBox::Ok) return IDOK; if(r==QMessageBox::Cancel) return IDCANCEL; if(r==QMessageBox::Yes) return IDYES; if(r==QMessageBox::No) return IDNO; return IDCANCEL; }
inline bool PostMessage(HWND, unsigned, WPARAM, LPARAM) { return true; }
inline LRESULT SendMessage(HWND, unsigned, WPARAM, LPARAM) { return 0; }
inline bool SetForegroundWindow(HWND h) { if(QWidget* w=asWidget(h)) { w->activateWindow(); w->raise(); return true; } return false; }
inline bool SetWindowPos(HWND h, HWND, int x, int y, int cx, int cy, unsigned) { if(QWidget* w=asWidget(h)) { w->move(x,y); w->resize(cx,cy); return true; } return false; }
inline bool GetWindowRect(HWND h, RECT* r) { if(QWidget* w=asWidget(h)) { QRect qr=w->geometry(); r->left=qr.left(); r->top=qr.top(); r->right=qr.right(); r->bottom=qr.bottom(); return true; } return false; }
inline bool GetClientRect(HWND h, RECT* r) { if(QWidget* w=asWidget(h)) { QSize s=w->size(); r->left=0; r->top=0; r->right=s.width(); r->bottom=s.height(); return true; } return false; }
inline bool SetWindowText(HWND h, const QString& t) { if(QWidget* w=asWidget(h)) { w->setWindowTitle(t); return true; } return false; }
inline void setEnabled(QWidget* w, bool enabled) { if (w) w->setEnabled(enabled); }
inline void setVisible(QWidget* w, bool visible) { if (w) visible ? w->show() : w->hide(); }
inline QWidget* getFocus() { return QApplication::focusWidget(); }
inline void setFocus(QWidget* w) { if (w) w->setFocus(); }
inline bool SetForegroundWindow(QWidget* w) { if(w) { w->activateWindow(); w->raise(); return true; } return false; }
inline LONG RegOpenKeyEx(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY) { return 0; }
inline LONG RegCloseKey(HKEY) { return 0; }
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define LOWORD(x) ((unsigned short)((unsigned long)(x) & 0xFFFF))
#define HIWORD(x) ((unsigned short)((unsigned long)(x) >> 16))
#define LOBYTE(x) ((unsigned char)((unsigned short)(x) & 0xFF))
#define HIBYTE(x) ((unsigned char)((unsigned short)(x) >> 8))
#define WINAPI
