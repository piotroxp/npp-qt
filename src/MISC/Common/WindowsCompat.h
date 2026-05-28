#ifndef WINDOWS_COMPAT_H
#define WINDOWS_COMPAT_H

#include <QtCore>
#include <QtWidgets>
#include <QtCore/private/qtextcodec_p.h>
#include <QString>
#include <QByteArray>
#include <QFile>

#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QWidget>
#include <cassert>

// =============================================================================
// Basic Windows Type Aliases - MUST come before anything else
// =============================================================================

typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef int BOOL;
typedef int INT;
typedef qint8 INT8;
typedef quint8 UINT8;
typedef qint16 INT16;
typedef quint16 UINT16;
typedef qint32 INT32;
typedef quint32 UINT32;
typedef qint64 INT64;
typedef quint64 UINT64;
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
typedef void* HIMAGELIST;
typedef void* HPRINTER;
typedef void* HCOLORSPACE;
typedef long LONG32;
typedef long* PLONG32;
typedef long LACCDWORD;
typedef unsigned long* PULARGE_INTEGER;
typedef unsigned int DWORD32;
typedef unsigned short WORD;
typedef unsigned char UCHAR;
typedef BYTE* LPBYTE;
typedef unsigned long DWORD_PTR;
typedef unsigned long U_LONG;
typedef void** LPVOID*;
typedef void* HPEN;
typedef int64_t INT_PTR;
typedef uint64_t UINT_PTR;
typedef int64_t LONG_PTR;
typedef uint64_t ULONG_PTR;
typedef void* HTHEME;

// Type pointers
typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;
typedef char* LPSTR;
typedef wchar_t* LPWSTR;
typedef long LRESULT;
typedef unsigned long WPARAM;
typedef long LPARAM;
typedef long HRESULT;
typedef unsigned long COLORREF;

// =============================================================================
// Windows Constants
// =============================================================================

#define HANDLE_NULL nullptr
#define INVALID_HANDLE_VALUE nullptr
#define MAX_PATH 260
#define MAXDWORD 0xFFFFFFFF
#define TRUE 1
#define FALSE 0

#define VOID void
#define CONST const
#define WINAPI

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
#define WM_PAINT 0x000F
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_GETTEXT 0x000D
#define WM_SETTEXT 0x000C
#define WM_SYSCOMMAND 0x0112
#define WM_USER 0x0400
#define WM_CTLCOLOREDIT 0x0133
#define WM_CTLCOLORSTATIC 0x0138
#define WM_DRAWITEM 0x002B
#define WM_MEASUREITEM 0x002C
#define CP_UTF8 65001

#define BN_CLICKED 0
#define EN_CHANGE 0x0300

#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
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
#define VK_DELETE 0x2E
#define VK_LEFT 0x25
#define VK_RIGHT 0x27
#define VK_UP 0x26
#define VK_DOWN 0x28

#define WS_OVERLAPPED 0x00000000
#define WS_POPUP 0x80000000
#define WS_CHILD 0x40000000
#define WS_MINIMIZE 0x20000000
#define WS_VISIBLE 0x10000000
#define WS_CAPTION 0x00C00000
#define WS_BORDER 0x00800000
#define WS_EX_LEFT 0x00000000
#define WS_EX_WINDOWEDGE 0x00000100
#define WS_EX_CLIENTEDGE 0x00000200

#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7
#define IDCLOSE 8
#define IDHELP 9

#define COLOR_WINDOW 5
#define COLOR_WINDOWTEXT 8
#define COLOR_HIGHLIGHT 13
#define CF_TEXT 1
#define CF_UNICODETEXT 13
#define MB_OK 0x00000000
#define MB_OKCANCEL 0x00000001
#define MB_YESNO 0x00000004
#define MB_ICONINFORMATION 0x00000040

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)
#define E_FAIL ((HRESULT)0x80000008L)
#define E_NOTIMPL ((HRESULT)0x80000001L)

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

// =============================================================================
// Windows Structures
// =============================================================================

struct POINT { LONG x; LONG y; POINT(long _x=0, long _y=0) : x(_x), y(_y) {} };
struct SIZE { LONG cx; LONG cy; SIZE(long _cx=0, long _cy=0) : cx(_cx), cy(_cy) {} };
struct RECT { LONG left; LONG top; LONG right; LONG bottom; RECT(long l=0, long t=0, long r=0, long b=0) : left(l), top(t), right(r), bottom(b) {} long Width() const { return right - left; } long Height() const { return bottom - top; } };

// Pointer aliases
typedef POINT* PPOINT;
typedef SIZE* PSIZE;
typedef RECT* PRECT;
typedef RECT* LPRECT;
typedef const RECT* LPCRECT;

// =============================================================================
// DRAWITEMSTRUCT / MEASUREITEMSTRUCT (for menu drawing)
// =============================================================================

struct DRAWITEMSTRUCT {
    UINT  CtlType;
    UINT  CtlID;
    UINT  itemID;
    UINT  itemAction;
    UINT  itemState;
    HWND  hwndItem;
    HDC   hDC;
    RECT  rcItem;
    ULONG_PTR itemData;
};

struct MEASUREITEMSTRUCT {
    UINT  CtlType;
    UINT  CtlID;
    UINT  itemID;
    UINT  itemWidth;
    UINT  itemHeight;
    ULONG_PTR itemData;
};

// =============================================================================
// Macros
// =============================================================================

#define SetRect(r,l,t,rt,b) do { (r).left=l;(r).top=t;(r).right=rt;(r).bottom=b; } while(0)
#define IsRectEmpty(r) ((r).left >= (r).right || (r).top >= (r).bottom)
#define WidthRect(r) ((r).right - (r).left)
#define HeightRect(r) ((r).bottom - (r).top)
#define MAKELONG(a,b) ((LONG)(((DWORD)((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define HIWORD(x) ((WORD)((((DWORD)(x)) >> 16) & 0xFFFF))
#define LOWORD(x) ((WORD)(((DWORD)(x)) & 0xFFFF))

// =============================================================================
// MFC stubs
// =============================================================================

#define ASSERT(x) assert(x)
#define TRACE(...) ((void)0)
#define VERIFY(x) assert(x)
#define cbWndExtra 0
#define cbClsExtra 0

static inline void AfxThrowNotSupportedException() { throw "Not supported"; }

// =============================================================================
// CFile stub
// =============================================================================

class CFile {
public:
    enum OpenFlags { modeRead=0, modeWrite=1 };
    enum Attribute { normal=0 };
    CFile() : _file(nullptr) {}
    ~CFile() { if(_file) { _file->close(); delete _file; } }
    bool Open(const char* name, OpenFlags flags) {
        QIODevice::OpenMode m = (flags & modeWrite) ? QIODevice::WriteOnly : QIODevice::ReadOnly;
        _file = new QFile(QString::fromLocal8Bit(name));
        return _file->open(m);
    }
    ULONGLONG GetLength() const { return _file ? _file->size() : 0; }
    void Read(void* buf, UINT count) { if(_file) _file->read(static_cast<char*>(buf), count); }
    void Write(const void* buf, UINT count) { if(_file) _file->write(static_cast<const char*>(buf), count); }
    void SeekToBegin() { if(_file) _file->seek(0); }
    void Close() { if(_file) _file->close(); }
private:
    QFile* _file;
};

// =============================================================================
// CArchive stub
// =============================================================================

class CArchive {
public:
    enum Mode { load=0, store=1 };
    CArchive(CFile* f, Mode m) : _file(f), _mode(m) {}
    bool IsLoading() const { return _mode == load; }
    bool IsStoring() const { return _mode == store; }
private:
    CFile* _file;
    Mode _mode;
};

// =============================================================================
// CString stub
// =============================================================================

class CString : public QString {
public:
    CString() : QString() {}
    CString(const char* s) : QString(QString::fromLocal8Bit(s)) {}
    CString(const QString& s) : QString(s) {}
    CString& operator=(const char* s) { QString::operator=(QString::fromLocal8Bit(s)); return *this; }
    CString& operator=(const QString& s) { QString::operator=(s); return *this; }
    int GetLength() const { return length(); }
    bool IsEmpty() const { return isEmpty(); }
    void Empty() { clear(); }
    const char* c_str() const { return toLocal8Bit().constData(); }
    CString Mid(int n, int len=-1) const { return CString(mid(n, len)); }
    CString Left(int n) const { return CString(left(n)); }
    CString Right(int n) const { return CString(right(n)); }
    int Find(char c, int start=0) const { return indexOf(c, start); }
};

// =============================================================================
// CWnd stub
// =============================================================================

class CWnd {
public:
    CWnd() : m_hWnd(nullptr) {}
    virtual ~CWnd() {}
    HWND GetSafeHwnd() const { return m_hWnd; }
    void Attach(HWND h) { m_hWnd = h; }
    HWND Detach() { HWND h = m_hWnd; m_hWnd = nullptr; return h; }
    LRESULT SendMessage(UINT msg, WPARAM wp, LPARAM lp) { Q_UNUSED(msg); Q_UNUSED(wp); Q_UNUSED(lp); return 0; }
    BOOL ShowWindow(int cmd) { Q_UNUSED(cmd); return FALSE; }
    void SetWindowText(const char* txt) { Q_UNUSED(txt); }
    void GetWindowRect(LPRECT r) const { Q_UNUSED(r); }
    void GetClientRect(LPRECT r) const { Q_UNUSED(r); }
    void MoveWindow(int x, int y, int w, int h, BOOL repaint=TRUE) { Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(w); Q_UNUSED(h); Q_UNUSED(repaint); }
    void SetFocus() {}
    void Invalidate(BOOL erase=TRUE) { Q_UNUSED(erase); }
    void UpdateWindow() {}
    BOOL CreateEx(DWORD exStyle, const char* cls, const char* name, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, LPVOID param) {
        Q_UNUSED(exStyle); Q_UNUSED(cls); Q_UNUSED(name); Q_UNUSED(style);
        Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(w); Q_UNUSED(h); Q_UNUSED(parent); Q_UNUSED(menu); Q_UNUSED(param);
        return FALSE;
    }
    HWND m_hWnd;
};

// =============================================================================
// CMenu stub
// =============================================================================

class CMenu {
public:
    CMenu() : _menu(nullptr) {}
    ~CMenu() { if(_menu) delete _menu; }
    BOOL CreatePopupMenu() { _menu = new QMenu(); return TRUE; }
    BOOL AppendMenu(UINT flags, UINT id=0, const char* text=nullptr) {
        if(_menu) { _menu->addAction(QString::fromLocal8Bit(text ? text : "")); }
        Q_UNUSED(flags); Q_UNUSED(id);
        return TRUE;
    }
    BOOL TrackPopupMenu(UINT flags, int x, int y, CWnd* parent) {
        Q_UNUSED(flags); Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(parent);
        if(_menu) { _menu->exec(); return TRUE; }
        return FALSE;
    }
    void DestroyMenu() { if((_menu)) { delete _menu; _menu = nullptr; } }
    QMenu* _menu;
};

// =============================================================================
// CDC stub
// =============================================================================

class CDC {
public:
    CDC() : _textColor(0), _bkMode(0) {}
    void SetTextColor(COLORREF c) { _textColor = c; }
    void SetBkMode(int mode) { _bkMode = mode; }
    COLORREF _textColor;
    int _bkMode;
};

class CClientDC : public CDC {
public:
    CClientDC(CWnd* w) { Q_UNUSED(w); }
};

class CWindowDC : public CDC {
public:
    CWindowDC(CWnd* w) { Q_UNUSED(w); }
};

class CMemDC : public CDC {
public:
    CMemDC() {}
};

class CPaintDC : public CDC {
public:
    CPaintDC(CWnd* w) { Q_UNUSED(w); }
};

// =============================================================================
// CFont / CBitmap stubs
// =============================================================================

class CFont {
public:
    CFont() {}
    BOOL CreateFont(int h, int w, int esc, int ori, int weight, BYTE italic, BYTE underline, BYTE strikeout, BYTE charset, BYTE outprec, BYTE clipprec, BYTE quality, BYTE pitch, const char* facename) {
        Q_UNUSED(h); Q_UNUSED(w); Q_UNUSED(esc); Q_UNUSED(ori); Q_UNUSED(weight);
        Q_UNUSED(italic); Q_UNUSED(underline); Q_UNUSED(strikeout); Q_UNUSED(charset);
        Q_UNUSED(outprec); Q_UNUSED(clipprec); Q_UNUSED(quality); Q_UNUSED(pitch); Q_UNUSED(facename);
        return TRUE;
    }
};

class CBitmap {
public:
    CBitmap() {}
    BOOL LoadBitmap(const char* name) { Q_UNUSED(name); return TRUE; }
    void* _data = nullptr;
};

struct BITMAP {
    int bmType = 0;
    int bmWidth = 0;
    int bmHeight = 0;
    int bmWidthBytes = 0;
    WORD bmPlanes = 0;
    WORD bmBitsPixel = 0;
    void* bmBits = nullptr;
};

// =============================================================================
// CDocTemplate / CDocument stubs
// =============================================================================

class CDocTemplate { public: CDocTemplate() {} };
class CDocument { public: CDocument() {} virtual ~CDocument() {} };

// =============================================================================
// CRectTracker / CSocket stubs
// =============================================================================

class CRectTracker {
public:
    CRectTracker() {}
    void Draw(CDC*) {}
    int HitTest(POINT) { return 0; }
};

struct SOCKADDR { short sa_family; char sa_data[14]; };

class CSocket {
public:
    CSocket() {}
    virtual ~CSocket() {}
    virtual BOOL Create(UINT, int, int) { return FALSE; }
    virtual BOOL Listen(int) { return FALSE; }
    virtual void Close() {}
    virtual int Send(const void*, int, int) { return 0; }
    virtual int Receive(void*, int, int) { return 0; }
};

// =============================================================================
// ATL / GDI+ stubs
// =============================================================================

#define USES_CONVERSION
#define A2W(x) ((WCHAR*)nullptr)
#define W2A(x) ((char*)nullptr)

static inline int GdiplusStartup(void** token, void*, void*) { return 0; }
static inline void GdiplusShutdown(void* token) { Q_UNUSED(token); }

#endif // WINDOWS_COMPAT_H
