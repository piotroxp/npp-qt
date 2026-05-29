#pragma once
#include "WindowsCompat.h"

#ifndef TCM_INSERTITEM
#define TCM_INSERTITEM (WM_USER + 62)
#define TCM_DELETEITEM (WM_USER + 8)
#define TCM_GETITEM (WM_USER + 60)
#define TCM_SETITEM (WM_USER + 61)
#define TCIF_PARAM 0x0008
#define TCIF_IMAGE 0x0002
#define TCN_SELCHANGE (0xFFFFFDDAu - 1u)
#endif

#ifndef CB_ERR
#define CB_ERR (-1)
#define CB_ERRSPACE (-2)
#endif
#ifndef LB_ERR
#define LB_ERR (-1)
#define LB_INSERTSTRING 0x0181
#define LB_DELETESTRING 0x0182
#define LB_ADDSTRING 0x0180
#define LB_FINDSTRING 0x018A
#define LB_GETTEXT 0x0189
#define LB_GETTEXTLEN 0x018A
#define LB_RESETCONTENT 0x0187
#define LB_GETITEMDATA 0x0199
#define LB_SETITEMDATA 0x019A
#define LBN_DBLCLK 2
#define LBN_SELCHANGE 1
#define LB_SETCURSEL 0x0186
#define LB_FINDSTRINGEXACT 0x018F
#endif
#ifndef BST_INDETERMINATE
#define BST_INDETERMINATE 2
#endif
#ifndef BS_CHECKBOX
#define BS_CHECKBOX 0x00000002
#define BS_AUTOCHECKBOX 0x00000003
#define BS_RADIOBUTTON 0x00000004
#define BS_AUTORADIOBUTTON 0x00000009
#define BS_MULTILINE 0x00002000
#define BS_TYPEMASK 0x0000000F
#endif
#ifndef MB_RIGHT
#define MB_RIGHT 0x00080000
#define MB_RTLREADING 0x00100000
#endif
#ifndef CB_FINDSTRING
#define CB_FINDSTRING 0x014C
#endif
#ifndef CB_GETCURSEL
#define CB_GETCURSEL 0x0147
#define CB_SETCURSEL 0x014E
#define CB_GETCOUNT 0x0146
#define CB_RESETCONTENT 0x014B
#define CB_ADDSTRING 0x0143
#define CB_INSERTSTRING 0x014A
#define CB_DELETESTRING 0x0144
#define CB_FINDSTRINGEXACT 0x0158
#define CB_GETDROPPEDSTATE 0x0157
#define CB_GETEDITSEL 0x0140
#define CB_SETEDITSEL 0x0142
#define CB_LIMITTEXT 0x0141
#define CB_SHOWDROPDOWN 0x014F
#endif
#ifndef CBN_SELCHANGE
#define CBN_SELCHANGE 1
#define CBN_EDITUPDATE 5
#define CBN_KILLFOCUS 4
#endif
#ifndef BCN_FIRST
#define BCN_FIRST (0U - 1250U)
#define BCN_DROPDOWN (BCN_FIRST + 8)
#endif
#ifndef EM_GETSEL
#define EM_GETSEL 0x00B0
#define EM_GETLIMITTEXT 0x00D5
#define EM_REPLACESEL 0x00C2
#endif
#ifndef CS_HREDRAW
#define CS_HREDRAW 0x0002
#define CS_VREDRAW 0x0001
#endif
#ifndef DT_LEFT
#define DT_LEFT 0x00000000
#endif
#ifndef DI_NORMAL
#define DI_NORMAL 0x0003
#endif
#ifndef DWLP_MSGRESULT
#define DWLP_MSGRESULT 0
#endif
#ifndef COLOR_3DFACE
#define COLOR_3DFACE 15
#define COLOR_3DSHADOW 16
#endif
#ifndef GW_CHILD
#define GW_CHILD 5
#endif
#ifndef MB_TASKMODAL
#define MB_TASKMODAL 0x00002000
#define MB_SETFOREGROUND 0x00010000
#endif
#ifndef PM_REMOVE
#define PM_REMOVE 0x0001
#endif
#ifndef ICC_PROGRESS_CLASS
#define ICC_PROGRESS_CLASS 0x00000020
#define ICC_STANDARD_CLASSES 0x00004000
#define ICC_BAR_CLASSES 0x00000400
#define ICC_COOL_CLASSES 0x00000400
#define ICC_USEREX_CLASSES 0x00000200
#define ICC_WIN95_CLASSES 0x000000FF
#endif
#ifndef CCS_TOP
#define CCS_TOP 0x00000001
#define CCS_NODIVIDER 0x00000040
#define CCS_NOPARENTALIGN 0x00000008
#define CCS_NORESIZE 0x00000004
#endif
#ifndef PBM_SETPOS
#define PBM_SETPOS (WM_USER + 2)
#endif
#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER)
#define TBM_SETPOS (WM_USER + 4)
#define TBM_SETRANGE (WM_USER + 6)
#endif
#ifndef RBBIM_CHILDSIZE
#define RBBIM_CHILDSIZE 0x00000020
#define RBBIM_ID 0x00000100
#define RBBIM_SIZE 0x00000040
#define RBBIM_STYLE 0x00000001
#define RBBIM_BACKGROUND 0x00000080
#define RBBIM_IDEALSIZE 0x00000200
#define RBBIM_CHEVRONLOCATION 0x00001000
#define RBBIM_CHEVRONSTATE 0x00002000
#define RBBS_HIDDEN 0x00000008
#define RBBS_NOGRIPPER 0x00000100
#define RBBS_GRIPPERALWAYS 0x00000080
#define RBBS_USECHEVRON 0x00000200
#define RBBS_VARIABLEHEIGHT 0x00000040
#define RBS_VARHEIGHT 0x00000200
#define RB_SETBANDINFO (WM_USER + 6)
#define RB_GETBANDINFO (WM_USER + 12)
#define RB_INSERTBAND (WM_USER + 1)
#define RB_DELETEBAND (WM_USER + 2)
#define RB_IDTOINDEX (WM_USER + 26)
#define RB_SETBARINFO (WM_USER + 4)
#define REBARCLASSNAMEW L"ReBarWindow32"
#define REBARCLASSNAME REBARCLASSNAMEW
struct REBARINFO {
	UINT cbSize = 0;
	UINT fMask = 0;
	UINT cyMinHeight = 0;
	UINT cyMaxHeight = 0;
	UINT cyIntegral = 0;
	UINT cxMinWidth = 0;
	UINT cxMaxWidth = 0;
	HIMAGELIST himl = nullptr;
};
#endif
#ifndef FLASHW_ALL
#define FLASHW_ALL 0x00000003
#endif
#ifndef WA_CLICKACTIVE
#define WA_CLICKACTIVE 2
#endif
#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS 0x00000400
#endif
#ifndef WM_CTLCOLORLISTBOX
#define WM_CTLCOLORLISTBOX 0x0134
#define WM_GETFONT 0x0031
#define WM_GETMINMAXINFO 0x0024
#define WM_GETTEXT 0x000D
#define WM_HSCROLL 0x0114
#define WM_KEYDOWN 0x0100
#define WM_NCHITTEST 0x0084
#define WM_NCLBUTTONDOWN 0x00A1
#define WM_NEXTDLGCTL 0x0028
#endif
#ifndef HTBORDER
#define HTBORDER 18
#define HTBOTTOM 15
#define HTBOTTOMLEFT 16
#define HTBOTTOMRIGHT 17
#define HTLEFT 10
#define HTRIGHT 11
#define HTTOP 12
#define HTTOPLEFT 13
#define HTTOPRIGHT 14
#endif

typedef WORD CLIPFORMAT;
typedef const wchar_t* PCTSTR;

struct COMBOBOXINFO {
	DWORD cbSize = 0;
	RECT rcItem{};
	RECT rcButton{};
	DWORD stateButton = 0;
	HWND hwndCombo = nullptr;
	HWND hwndItem = nullptr;
	HWND hwndList = nullptr;
};

struct FLASHWINFO {
	UINT cbSize = 0;
	HWND hwnd = nullptr;
	DWORD dwFlags = 0;
	UINT uCount = 0;
	DWORD dwTimeout = 0;
};

struct INITCOMMONCONTROLSEX {
	DWORD dwSize = 0;
	DWORD dwICC = 0;
};

#ifndef WNDPROC_DEFINED
#define WNDPROC_DEFINED
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
#endif

struct WNDCLASSEXW {
	UINT cbSize = 0;
	UINT style = 0;
	WNDPROC lpfnWndProc = nullptr;
	int cbClsExtra = 0;
	int cbWndExtra = 0;
	HINSTANCE hInstance = nullptr;
	HICON hIcon = nullptr;
	HCURSOR hCursor = nullptr;
	HBRUSH hbrBackground = nullptr;
	const wchar_t* lpszMenuName = nullptr;
	const wchar_t* lpszClassName = nullptr;
	HICON hIconSm = nullptr;
};
typedef WNDCLASSEXW WNDCLASSEX;

inline ATOM RegisterClassExW(const WNDCLASSEXW*) { return 1; }
#define RegisterClassEx RegisterClassExW
inline BOOL UnregisterClassW(const wchar_t*, HINSTANCE) { return TRUE; }
#define UnregisterClass UnregisterClassW
inline BOOL InitCommonControlsEx(INITCOMMONCONTROLSEX*) { return TRUE; }
inline LONG InterlockedIncrement(volatile long* v) { return ++(*v); }
inline LONG InterlockedDecrement(volatile long* v) { return --(*v); }
inline LONG InterlockedIncrement(volatile DWORD* v) { return ++(*reinterpret_cast<volatile long*>(v)); }
inline LONG InterlockedDecrement(volatile DWORD* v) { return --(*reinterpret_cast<volatile long*>(v)); }
inline BOOL OffsetWindowOrgEx(HDC, int, int, POINT*) { return TRUE; }
inline BOOL SetWindowOrgEx(HDC, int, int, POINT*) { return TRUE; }
inline BOOL TranslateMessage(const void*) { return TRUE; }
inline BOOL PeekMessageW(void*, HWND, UINT, UINT, UINT) { return FALSE; }
#define PeekMessage PeekMessageW
inline BOOL DrawIconEx(HDC, int, int, HICON, int, int, UINT, HBRUSH, UINT) { return TRUE; }
inline BOOL FlashWindowEx(FLASHWINFO*) { return TRUE; }
inline HWND GetWindow(HWND, UINT) { return nullptr; }
inline COLORREF SetBkColor(HDC, COLORREF) { return 0; }

#ifndef PD_RETURNDC
#define PD_ALLPAGES 0x00000000
#define PD_SELECTION 0x00000001
#define PD_PAGENUMS 0x00000002
#define PD_NOSELECTION 0x00000004
#define PD_RETURNDC 0x00000100
#define PD_RETURNDEFAULT 0x00000400
#define PD_USEDEVMODECOPIES 0x00040000
#endif
#ifndef HORZRES
#define HORZRES 8
#define VERTRES 10
#define LOGPIXELSX 88
#define LOGPIXELSY 90
#define PHYSICALWIDTH 110
#define PHYSICALHEIGHT 111
#define PHYSICALOFFSETX 112
#define PHYSICALOFFSETY 113
#endif
#ifndef FW_NORMAL
#define FW_NORMAL 400
#define FW_BOLD 700
#endif
#ifndef TA_TOP
#define TA_TOP 0
#define TA_BOTTOM 8
#define TA_RTLREADING 256
#endif
#ifndef ETO_CLIPPED
#define ETO_CLIPPED 0x00000004
#define ETO_OPAQUE 0x00000002
#endif
#ifndef DATE_SHORTDATE
#define DATE_SHORTDATE 0x00000001
#define DATE_LONGDATE 0x00000002
#define TIME_NOSECONDS 0x00000002
#endif
struct DOCINFOW {
	int cbSize = 0;
	const wchar_t* lpszDocName = nullptr;
	const wchar_t* lpszOutput = nullptr;
	const wchar_t* lpszDatatype = nullptr;
	DWORD fwType = 0;
};
typedef DOCINFOW DOCINFO;
inline int GetDeviceCaps(HDC, int) { return 100; }
inline BOOL DPtoLP(HDC, POINT*, int) { return TRUE; }
inline HFONT CreateFontW(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, const wchar_t*) { return nullptr; }
#define CreateFont CreateFontW
inline HPEN CreatePen(int, int, COLORREF) { return nullptr; }
inline COLORREF SetTextColor(HDC, COLORREF) { return 0; }
inline BOOL MoveToEx(HDC, int, int, POINT*) { return TRUE; }
inline BOOL LineTo(HDC, int, int) { return TRUE; }
inline UINT SetTextAlign(HDC, UINT) { return 0; }
inline BOOL GetTextMetricsW(HDC, TEXTMETRIC*) { return FALSE; }
#define GetTextMetrics GetTextMetricsW
inline int ExtTextOutW(HDC, int, int, UINT, const RECT*, const wchar_t*, UINT, const int*) { return 0; }
#define ExtTextOut ExtTextOutW
inline int StartDocW(HDC, const DOCINFOW*) { return 1; }
#define StartDoc StartDocW
inline int StartPage(HDC) { return 1; }
inline int EndPage(HDC) { return 1; }
inline int EndDoc(HDC) { return 1; }
inline void GetLocalTime(void*) {}

#ifndef SB_LINEUP
#define SB_LINEUP 0
#define SB_LINEDOWN 1
#define SB_PAGEUP 2
#define SB_PAGEDOWN 3
#define SIF_RANGE 0x0001
#define SIF_PAGE 0x0002
#define WM_VSCROLL 0x0115
#define WM_SHOWWINDOW 0x0018
#endif
#ifndef EM_SETLIMITTEXT
#define EM_SETLIMITTEXT 0x00C5
#define EM_LIMITTEXT 0x00C5
#endif
#ifndef CB_SETITEMDATA
#define CB_GETITEMDATA 0x0153
#define CB_SETITEMDATA 0x015A
#endif
#ifndef DS_MODALFRAME
#define DS_MODALFRAME 0x0080
#define WS_EX_CONTROLPARENT 0x00010000
#endif
#ifndef SWP_NOREDRAW
#define SWP_NOREDRAW 0x0008
#define SWP_NOSENDCHANGING 0x0400
#endif
#ifndef IS_TEXT_UNICODE_STATISTICS
#define IS_TEXT_UNICODE_STATISTICS 0x0008
#endif
inline BOOL IsTextUnicode(const void*, int, int*) { return FALSE; }
inline unsigned short _byteswap_ushort(unsigned short x) {
	return static_cast<unsigned short>((x >> 8) | (x << 8));
}
inline int SetScrollPos(HWND, int, int, BOOL) { return 0; }
inline BOOL SetScrollInfo(HWND, int, const void*, BOOL) { return TRUE; }
inline BOOL ScrollWindow(HWND, int, int, const RECT*, const RECT*) { return TRUE; }
inline BOOL ScreenToClient(HWND, LPPOINT) { return TRUE; }
inline HWND SetParent(HWND h, HWND p) { Q_UNUSED(p); return h; }
inline HANDLE GetPropW(HWND, const wchar_t*) { return nullptr; }
#define GetProp GetPropW
inline BOOL SetPropW(HWND, const wchar_t*, HANDLE) { return TRUE; }
#define SetProp SetPropW
inline HANDLE RemovePropW(HWND, const wchar_t*) { return nullptr; }
#define RemoveProp RemovePropW

#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG_PTR)0x80000002)
#define KEY_READ 0x20019
#define KEY_QUERY_VALUE 0x0001
#define KEY_ENUMERATE_SUB_KEYS 0x0008
#define REG_SZ 1
#endif
typedef LONG LSTATUS;
inline LSTATUS RegQueryInfoKeyW(HKEY, wchar_t*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, FILETIME*) { return 0; }
#define RegQueryInfoKey RegQueryInfoKeyW
inline DWORD GetVersion() { return 0; }
#ifndef WM_DRAWITEM
#define WM_DRAWITEM 0x002B
#endif
#ifndef SPI_GETNONCLIENTMETRICS
#define SPI_GETNONCLIENTMETRICS 0x0029
#endif
inline BOOL SystemParametersInfoW(UINT, UINT, void*, UINT) { return TRUE; }
#define SystemParametersInfo SystemParametersInfoW
#ifndef SM_CMONITORS
#define SM_CMONITORS 80
#define SM_CXCURSOR 13
#define SM_CYCURSOR 14
#endif
#ifndef FIXED_PITCH
#define FIXED_PITCH 0x01
#define FF_MODERN 0x30
#define DEFAULT_QUALITY 0
#define CLIP_DEFAULT_PRECIS 0
#define OUT_DEFAULT_PRECIS 0
#endif
struct NONCLIENTMETRICS {
	UINT cbSize = 0;
	int iBorderWidth = 0;
	int iScrollWidth = 0;
	int iScrollHeight = 0;
	int iCaptionWidth = 0;
	int iCaptionHeight = 0;
	LOGFONTW lfCaptionFont{};
	LOGFONTW lfSmCaptionFont{};
	LOGFONTW lfMenuFont{};
	LOGFONTW lfStatusFont{};
	LOGFONTW lfMessageFont{};
};
#ifndef DT_CENTER
#define DT_CENTER 0x00000001
#define DT_RIGHT 0x00000002
#define DT_SINGLELINE 0x00000020
#define DT_VCENTER 0x00000004
#define DT_NOPREFIX 0x00000800
#define DT_NOCLIP 0x00000100
#define DT_TOP 0x00000000
#endif
#ifndef HTCLIENT
#define HTCLIENT 1
#define OCR_HAND 32649
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif
#ifndef SS_CENTER
#define SS_CENTER 0x00000001
#define SS_CENTERIMAGE 0x00000200
#define SS_NOTIFY 0x00000100
#endif
#ifndef WS_BORDER
#define WS_BORDER 0x00800000
#define WS_TABSTOP 0x00010000
#endif
struct PAINTSTRUCT {
	HDC hdc = nullptr;
	BOOL fErase = FALSE;
	RECT rcPaint{};
	BOOL fRestore = FALSE;
	BOOL fIncUpdate = FALSE;
	BYTE rgbReserved[32]{};
};
inline int GetWindowTextW(HWND, wchar_t*, int) { return 0; }
#define GetWindowText GetWindowTextW
inline HCURSOR SetCursor(HCURSOR) { return nullptr; }
#ifndef WM_ENABLE
#define WM_ENABLE 0x000A
#define WM_KEYUP 0x0101
#define WM_CREATE 0x0001
#define WM_NCCREATE 0x0081
#define WM_SETTINGCHANGE 0x001A
#define WM_MOVING 0x0216
#define WM_SIZING 0x0214
#define WM_CONTEXTMENU 0x007B
#define WM_LBUTTONDBLCLK 0x0203
#define WM_MBUTTONUP 0x0208
#define WM_RBUTTONUP 0x0205
#define WM_MOUSEHOVER 0x02A1
#define WM_MOUSELEAVE 0x02A3
#define WM_NCACTIVATE 0x0086
#define WM_NCCALCSIZE 0x0083
#define WM_NCLBUTTONDBLCLK 0x00A3
#define WM_PARENTNOTIFY 0x0210
#define WM_SYSCOMMAND 0x0118
#define WM_CLEAR 0x0303
#define WM_PRINT 0x0317
#define WM_TIMER 0x0113
#define WM_CAPTURECHANGED 0x0215
#define WM_CHANGEUISTATE 0x0127
#define WM_IME_REQUEST 0x0288
#endif
#ifndef EN_SETFOCUS
#define EN_SETFOCUS 0x0100
#define EN_KILLFOCUS 0x0200
#endif
#ifndef CB_SELECTSTRING
#define CB_SELECTSTRING 0x014D
#endif
#ifndef LB_SELECTSTRING
#define LB_SELECTSTRING 0x018C
#endif
#ifndef BTNS_AUTOSIZE
#define BTNS_AUTOSIZE 0x0010
#define TBSTYLE_AUTOSIZE 0x0004
#define CCS_ADJUSTABLE 0x00000080
#define I_IMAGENONE (-2)
#endif
#ifndef TB_GETITEMRECT
#define TB_GETITEMRECT (WM_USER + 29)
#endif
#ifndef TBM_SETPAGESIZE
#define TBM_SETPAGESIZE (WM_USER + 5)
#define TBM_SETRANGEMIN (WM_USER + 7)
#define TBM_SETRANGEMAX (WM_USER + 8)
#endif
#ifndef TTM_SETMAXTIPWIDTH
#define TTM_SETMAXTIPWIDTH (WM_USER + 24)
#endif
#ifndef NM_FIRST
#define NM_FIRST (0U - 0U)
#endif
#ifndef WS_EX_WINDOWEDGE
#define WS_EX_WINDOWEDGE 0x00000100
#endif
#ifndef TME_HOVER
#define TME_HOVER 0x00000001
#define TME_LEAVE 0x00000002
#endif
struct TRACKMOUSEEVENT {
	DWORD cbSize = sizeof(TRACKMOUSEEVENT);
	DWORD dwFlags = 0;
	HWND hwndTrack = nullptr;
	DWORD dwHoverTime = 0;
};
inline BOOL _TrackMouseEvent(TRACKMOUSEEVENT*) { return TRUE; }
#define TrackMouseEvent _TrackMouseEvent
#ifndef TRANSPARENT
#define TRANSPARENT 1
#endif
#ifndef HS_FDIAGONAL
#define HS_FDIAGONAL 2
#endif
#ifndef MAKEWPARAM
#define MAKEWPARAM(l, h) static_cast<WPARAM>(MAKELPARAM(l, h))
#endif
inline BOOL Rectangle(HDC, int, int, int, int) { return TRUE; }
inline HBRUSH CreateHatchBrush(int, COLORREF) { return nullptr; }
inline BOOL InflateRect(RECT*, int, int) { return TRUE; }

#ifndef WS_EX_CLIENTEDGE
#define WS_EX_CLIENTEDGE 0x00000200
#endif
#ifndef WC_EDIT
#define WC_EDIT L"Edit"
#define WC_TREEVIEW L"SysTreeView32"
#define TOOLBARCLASSNAMEW L"ToolbarWindow32"
#define TOOLBARCLASSNAME TOOLBARCLASSNAMEW
#endif
#ifndef ES_AUTOVSCROLL
#define ES_AUTOVSCROLL 0x0040
#endif
#ifndef FILE_NOTIFY_CHANGE_DIR_NAME
#define FILE_NOTIFY_CHANGE_DIR_NAME 0x00000002
#define FILE_NOTIFY_CHANGE_CREATION 0x00000040
#endif
#ifndef SIF_POS
#define SIF_POS 0x0004
#endif
#ifndef PRF_CLIENT
#define PRF_CLIENT 0x00000004
#define PRF_ERASEBKGND 0x00000008
#endif
inline BOOL GetScrollInfo(HWND, int, void*) { return TRUE; }
inline int IntersectClipRect(HDC, int, int, int, int) { return 0; }
inline UINT GetDlgItemInt(HWND, int, BOOL*, BOOL) { return 0; }

#ifndef TV_FIRST
#define TV_FIRST 0x1100
#define TVN_FIRST (0U - 400U)
#endif
#ifndef TVIF_PARAM
#define TVIF_TEXT 0x0001
#define TVIF_IMAGE 0x0002
#define TVIF_PARAM 0x0004
#define TVIF_SELECTEDIMAGE 0x000020
#define TVI_ROOT ((HTREEITEM)(ULONG_PTR)0xFFFF0000)
#define TVM_INSERTITEM (TV_FIRST + 0)
#define TVM_DELETEITEM (TV_FIRST + 1)
#define TVM_EXPAND (TV_FIRST + 2)
#define TVM_SELECTITEM (TV_FIRST + 11)
#define TVM_HITTEST (TV_FIRST + 17)
#define TVM_EDITLABEL (TV_FIRST + 15)
#define TVM_ENSUREVISIBLE (TV_FIRST + 20)
#define TVM_GETNEXTITEM (TV_FIRST + 10)
#define TVGN_CARET 0x0009
#define TVGN_CHILD 0x0004
#define TVGN_NEXT 0x0001
#define TVGN_PARENT 0x0003
#define TVGN_ROOT 0x0000
#define TVGN_PREVIOUS 0x0002
#define TVM_GETITEM (TV_FIRST + 12)
#define TVM_SETITEM (TV_FIRST + 13)
#define TVM_GETITEMRECT (TV_FIRST + 4)
#define TVM_GETCOUNT (TV_FIRST + 5)
#define TVM_GETLINECOLOR (TV_FIRST + 16)
#define TVM_SETLINECOLOR (TV_FIRST + 29)
#define TVN_SELCHANGED (TVN_FIRST - 2U)
#define TVN_ITEMEXPANDED (TVN_FIRST - 6U)
#define TVN_ENDLABELEDIT (TVN_FIRST - 8U)
#define TVN_GETINFOTIP (TVN_FIRST - 14U)
#define TVN_KEYDOWN (TVN_FIRST - 12U)
#define TVN_BEGINDRAG (TVN_FIRST - 7U)
#define TVN_BEGINRDRAG (TVN_FIRST - 9U)
#define TVN_BEGINLABELEDIT (TVN_FIRST - 10U)
#define TreeView_SetItem(hwnd, item) SendMessage(hwnd, TVM_SETITEM, 0, reinterpret_cast<LPARAM>(item))
#define TreeView_HitTest(hwnd, info) reinterpret_cast<HTREEITEM>(SendMessage(hwnd, TVM_HITTEST, 0, reinterpret_cast<LPARAM>(info)))
#define TreeView_EditLabel(hwnd, item) reinterpret_cast<HTREEITEM>(SendMessage(hwnd, TVM_EDITLABEL, 0, reinterpret_cast<LPARAM>(item)))
#define TreeView_GetItemRect(hwnd, item, prc, b) SendMessage(hwnd, TVM_GETITEMRECT, static_cast<WPARAM>(b), reinterpret_cast<LPARAM>(prc))
#define TVIF_HANDLE 0x0010
#define TVIF_STATE 0x0008
#define TVIS_SELECTED 0x0002
#define TVIS_EXPANDED 0x0020
#define TVIS_EXPANDEDONCE 0x0040
#define TVI_FIRST ((HTREEITEM)(ULONG_PTR)-65535)
#define TVI_LAST ((HTREEITEM)(ULONG_PTR)-65534)
#define TVGN_DROPHILITE 0x0008
#define TVM_SETIMAGELIST (TV_FIRST + 9)
#define TVM_SETITEMHEIGHT (TV_FIRST + 27)
#define TVM_SORTCHILDREN (TV_FIRST + 19)
#define TVM_SORTCHILDRENCB (TV_FIRST + 21)
#define TVM_CREATEDRAGIMAGE (TV_FIRST + 18)
#define TVSIL_NORMAL 0
#define TVS_HASBUTTONS 0x0001
#define TVS_HASLINES 0x0002
#define TVS_LINESATROOT 0x0004
#define TVS_EDITLABELS 0x0008
#define TVS_SHOWSELALWAYS 0x0020
#define TVS_INFOTIP 0x0400
#define TreeView_DeleteAllItems(hwnd) SendMessage(hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(TVI_ROOT))
#define TreeView_DeleteItem(hwnd, hItem) SendMessage(hwnd, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hItem))
#define TreeView_GetItem(hwnd, item) SendMessage(hwnd, TVM_GETITEM, 0, reinterpret_cast<LPARAM>(item))
#define TreeView_SetImageList(hwnd, himl, type) SendMessage(hwnd, TVM_SETIMAGELIST, type, reinterpret_cast<LPARAM>(himl))
#define TreeView_SetItemHeight(hwnd, cy) SendMessage(hwnd, TVM_SETITEMHEIGHT, cy, 0)
#endif
struct TVITEMW {
	UINT mask = 0;
	HTREEITEM hItem = nullptr;
	UINT state = 0;
	UINT stateMask = 0;
	LPWSTR pszText = nullptr;
	int cchTextMax = 0;
	int iImage = 0;
	int iSelectedImage = 0;
	int cChildren = 0;
	LPARAM lParam = 0;
};
typedef TVITEMW TVITEM;
typedef TVITEMW* LPTVITEM;
struct TVINSERTSTRUCTW {
	HTREEITEM hParent = nullptr;
	HTREEITEM hInsertAfter = nullptr;
	TVITEMW item{};
};
typedef TVINSERTSTRUCTW TVINSERTSTRUCT;
#ifndef PFNTVCOMPARE_DEFINED
#define PFNTVCOMPARE_DEFINED
typedef int (CALLBACK *PFNTVCOMPARE)(LPARAM, LPARAM, LPARAM);
#endif
struct TVSORTCB {
	HTREEITEM hParent = nullptr;
	PFNTVCOMPARE lpfnCompare = nullptr;
	LPARAM lParam = 0;
};
struct TVHITTESTINFO {
	POINT pt{};
	UINT flags = 0;
	HTREEITEM hItem = nullptr;
};
struct NMTREEVIEW {
	NMHDR hdr{};
	UINT action = 0;
	TVITEMW itemOld{};
	TVITEMW itemNew{};
	POINT ptDrag{};
};
typedef NMTREEVIEW* LPNMTREEVIEW;
struct NMTVDISPINFOW {
	NMHDR hdr{};
	TVITEMW item{};
	LPWSTR pszText = nullptr;
	int cchTextMax = 0;
};
typedef NMTVDISPINFOW NMTVDISPINFO;
typedef NMTVDISPINFOW* LPNMTVDISPINFO;
struct NMTVGETINFOTIPW {
	NMHDR hdr{};
	LPWSTR pszText = nullptr;
	int cchTextMax = 0;
	HTREEITEM hItem = nullptr;
	LPARAM lParam = 0;
};
typedef NMTVGETINFOTIPW NMTVGETINFOTIP;
typedef NMTVGETINFOTIPW* LPNMTVGETINFOTIP;
struct NMTVKEYDOWN {
	NMHDR hdr{};
	WORD wVKey = 0;
	UINT flags = 0;
};
typedef NMTVKEYDOWN* LPNMTVKEYDOWN;
typedef NMTREEVIEW* LPNMTREEVIEWW;
typedef NMTVDISPINFOW* LPNMTVDISPINFOW;
typedef NMTVGETINFOTIPW* LPNMTVGETINFOTIPW;

#ifndef BTNS_BUTTON
#define BTNS_BUTTON 0x0000
#define BTNS_CHECK 0x0002
#define BTNS_SEP 0x0001
#define BTNS_DROPDOWN 0x0008
#define TBSTYLE_FLAT 0x0800
#define TBSTYLE_LIST 0x1000
#define TBSTYLE_TOOLTIPS 0x0400
#define TBSTYLE_TRANSPARENT 0x8000
#define TBSTYLE_CUSTOMERASE 0x00002000
#define TBSTYLE_EX_DOUBLEBUFFER 0x00000080
#define TBSTATE_ENABLED 0x0004
#define TBIF_STATE 0x0004
#define TBIF_STYLE 0x0001
#define TB_COMMANDTOINDEX (WM_USER + 25)
#define TB_GETITEMDROPDOWNRECT (WM_USER + 75)
#define TBN_DROPDOWN (0U - 10U)
#define TB_ADDBUTTONS (WM_USER + 20)
#define TB_AUTOSIZE (WM_USER + 33)
#define TB_BUTTONSTRUCTSIZE (WM_USER + 30)
#define TB_GETBUTTONINFO (WM_USER + 63)
#define TB_SETBUTTONINFO (WM_USER + 64)
#define TB_GETEXTENDEDSTYLE (WM_USER + 27)
#define TB_SETEXTENDEDSTYLE (WM_USER + 36)
#define TB_SETBUTTONSIZE (WM_USER + 52)
#define TB_ADDBITMAP (WM_USER + 23)
#define TB_GETBUTTON (WM_USER + 44)
#define TB_GETBUTTONSIZE (WM_USER + 58)
#define TB_GETPADDING (WM_USER + 86)
#define TB_ISBUTTONENABLED (WM_USER + 53)
#define TB_SETBITMAPSIZE (WM_USER + 56)
#define TBSTATE_HIDDEN 0x0008
#define TBSTYLE_EX_DRAWDDARROWS 0x00000001
#define TBSTYLE_EX_HIDECLIPPEDBUTTONS 0x00000010
struct TBADDBITMAP {
	HINSTANCE hInst = nullptr;
	UINT_PTR nID = 0;
};
#endif
#ifndef TTM_ADDTOOL
#define TTM_ADDTOOL (WM_USER + 50)
#define TTM_TRACKACTIVATE (WM_USER + 17)
#define TTM_TRACKPOSITION (WM_USER + 18)
#define TTF_TRACK 0x0020
#define TTF_ABSOLUTE 0x0800
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
struct NMLVDISPINFOW {
	NMHDR hdr{};
	LVITEMW item{};
};
typedef NMLVDISPINFOW NMLVDISPINFO;
typedef NMLVDISPINFOW* LPNMLVDISPINFOW;
#endif
struct TBBUTTONINFOW {
	UINT cbSize = 0;
	DWORD dwMask = 0;
	int idCommand = 0;
	int iImage = 0;
	BYTE fsState = 0;
	BYTE fsStyle = 0;
	WORD cx = 0;
	LPARAM lParam = 0;
	wchar_t* pszText = nullptr;
	int cchText = 0;
};
typedef TBBUTTONINFOW TBBUTTONINFO;

#ifndef NM_RETURN
#define NM_RETURN (NM_FIRST - 4)
#endif
struct TOOLTIPTEXTW {
	NMHDR hdr{};
	wchar_t* lpszText = nullptr;
	wchar_t szText[80]{};
	HINSTANCE hinst = nullptr;
	UINT uFlags = 0;
};
typedef TOOLTIPTEXTW TOOLTIPTEXT;
typedef TOOLTIPTEXTW* LPTOOLTIPTEXT;
#ifndef TTN_GETDISPINFO
#define TTN_GETDISPINFO (0U - 520U)
#endif

#ifndef LVN_FIRST
#define LVM_FIRST 0x1000
#define LVN_FIRST (0U - 100U)
#define LVN_ITEMCHANGED (LVN_FIRST - 1U)
#define LVN_GETINFOTIP (0U - 158U)
#define LVN_COLUMNCLICK (0U - 108U)
#define LVIF_STATE 0x0008
#define LVIF_IMAGE 0x0002
#define LVIF_GROUPID 0x0100
#define LVS_EX_CHECKBOXES 0x00000004
#define LVS_EX_INFOTIP 0x00000400
#define LVS_ALIGNTOP 0x0000
#define LVCF_FMT 0x0001
#define LVM_SETCOLUMNWIDTH (LVM_FIRST + 30)
#define LVM_GETHEADER (LVM_FIRST + 31)
#define LVM_HITTEST (LVM_FIRST + 14)
#define LVM_GETCOLUMN (LVM_FIRST + 95)
#define LVM_SETCOLUMN (LVM_FIRST + 96)
#define LVM_SORTITEMSEX (LVM_FIRST + 81)
#define LVM_ENABLEGROUPVIEW (LVM_FIRST + 157)
#define LVM_INSERTGROUP (LVM_FIRST + 145)
#define LVM_GETGROUPRECT (LVM_FIRST + 147)
#define ListView_SetColumnWidth(hwnd, col, cx) SendMessage(hwnd, LVM_SETCOLUMNWIDTH, col, MAKELPARAM(cx, 0))
#define ListView_GetHeader(hwnd) reinterpret_cast<HWND>(SendMessage(hwnd, LVM_GETHEADER, 0, 0))
#define ListView_SortItemsEx(hwnd, pfn, lParam) SendMessage(hwnd, LVM_SORTITEMSEX, 0, reinterpret_cast<LPARAM>(pfn))
#define ListView_HitTest(hwnd, info) static_cast<int>(SendMessage(hwnd, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(info)))
#define ListView_SetItem(hwnd, pitem) SendMessage(hwnd, LVM_SETITEM, 0, reinterpret_cast<LPARAM>(pitem))
#define ListView_EnableGroupView(hwnd, f) SendMessage(hwnd, LVM_ENABLEGROUPVIEW, f, 0)
#define ListView_InsertGroup(hwnd, idx, pg) SendMessage(hwnd, LVM_INSERTGROUP, idx, reinterpret_cast<LPARAM>(pg))
#define ListView_GetGroupRect(hwnd, id, flags, prc) SendMessage(hwnd, LVM_GETGROUPRECT, id, reinterpret_cast<LPARAM>(prc))
#define LVGF_HEADER 0x00000001
#define LVGF_GROUPID 0x00000010
#define LVGF_STATE 0x00000004
#define LVGS_COLLAPSIBLE 0x00000008
#define LVCDI_GROUP 0x00000001
#define LVGGR_HEADER 0x00000003
#define NM_CUSTOMDRAW (NM_FIRST - 12)
#define CDDS_PREPAINT 0x00000001
#define CDDS_ITEMPREPAINT 0x00000002
#define CDRF_NOTIFYITEMDRAW 0x00000010
#define CDRF_NEWFONT 0x00000002
#define CDRF_DODEFAULT 0x00000000
#define CDRF_SKIPDEFAULT 0x00000004
#define CDRF_NOTIFYPOSTPAINT 0x00000010
#define CDDS_ITEMPOSTPAINT 0x00000003
#define CDIS_CHECKED 0x00000008
#define CDIS_SELECTED 0x00000001
#define CDIS_HOT 0x00000200
#define HDM_FIRST 0x1200
#define HDM_GETITEMCOUNT (HDM_FIRST + 0)
#define HDM_GETITEM (HDM_FIRST + 11)
#define HDM_GETITEMRECT (HDM_FIRST + 7)
#define HDI_TEXT 0x00000002
#define HDI_WIDTH 0x00000001
#define HDF_SORTUP 0x0400
#define HDF_SORTDOWN 0x0200
#define HDN_FIRST (0U - 300U)
#define HDN_DIVIDERDBLCLICK (HDN_FIRST - 25)
#define HDN_ENDTRACK (HDN_FIRST - 26)
#define Header_GetItem(hwnd, idx, phdi) SendMessage(hwnd, HDM_GETITEM, idx, reinterpret_cast<LPARAM>(phdi))
#define Header_GetItemCount(hwnd) static_cast<int>(SendMessage(hwnd, HDM_GETITEMCOUNT, 0, 0))
#define Header_GetItemRect(hwnd, idx, prc) SendMessage(hwnd, HDM_GETITEMRECT, idx, reinterpret_cast<LPARAM>(prc))
#define LVCF_SUBITEM 0x0008
#define LVCFMT_LEFT 0x0000
#define LVCFMT_CENTER 0x0002
#define LVM_INSERTCOLUMN (LVM_FIRST + 97)
#define LVM_GETCOLUMNWIDTH (LVM_FIRST + 29)
#define LVM_GETSELECTEDCOUNT (LVM_FIRST + 50)
#define LVM_SETITEMCOUNT (LVM_FIRST + 47)
#define LVM_GETNEXTITEM (LVM_FIRST + 58)
#define LVNI_ALL 0x0000
#define LVNI_SELECTED 0x0002
#define LVN_ITEMACTIVATE (0U - 114U)
#define LVN_ODSTATECHANGED (0U - 115U)
#define LVS_EX_HEADERDRAGDROP 0x02000000
#define LVSICF_NOINVALIDATEALL 0x00000001
#define ListView_GetNextItem(hwnd, i, f) static_cast<int>(SendMessage(hwnd, LVM_GETNEXTITEM, i, f))
#define ListView_GetSelectedCount(hwnd) static_cast<int>(SendMessage(hwnd, LVM_GETSELECTEDCOUNT, 0, 0))
#define ListView_SetItemCount(hwnd, c) SendMessage(hwnd, LVM_SETITEMCOUNT, c, 0)
#endif
#ifndef HDITEMW_DEFINED
#define HDITEMW_DEFINED
struct HDITEMW {
	UINT mask = 0;
	int cxy = 0;
	LPWSTR pszText = nullptr;
	HBITMAP hbm = nullptr;
	int cchTextMax = 0;
	int fmt = 0;
	LPARAM lParam = 0;
	int iImage = 0;
	int iOrder = 0;
};
typedef HDITEMW HDITEM;
struct NMHEADERW {
	NMHDR hdr{};
	int iItem = 0;
	int iButton = 0;
	HDITEMW* pitem = nullptr;
};
typedef NMHEADERW NMHEADER;
typedef NMHEADERW* LPNMHEADER;
#endif
#ifndef LVGROUP_DEFINED
#define LVGROUP_DEFINED
struct LVGROUP {
	UINT cbSize = 0;
	UINT mask = 0;
	LPWSTR pszHeader = nullptr;
	int cchHeader = 0;
	LPWSTR pszFooter = nullptr;
	int cchFooter = 0;
	int iGroupId = 0;
	UINT stateMask = 0;
	UINT state = 0;
	UINT uAlign = 0;
};
#endif
#ifndef NMCUSTOMDRAW_DEFINED
#define NMCUSTOMDRAW_DEFINED
struct NMCUSTOMDRAW {
	NMHDR hdr{};
	DWORD dwDrawStage = 0;
	HDC hdc = nullptr;
	RECT rc{};
	DWORD_PTR dwItemSpec = 0;
	UINT uItemState = 0;
	LPARAM lItemlParam = 0;
};
typedef NMCUSTOMDRAW* LPNMCUSTOMDRAW;
struct NMLVCUSTOMDRAW {
	NMCUSTOMDRAW nmcd{};
	COLORREF clrText = 0;
	COLORREF clrTextBk = 0;
	int iSubItem = 0;
	DWORD dwItemType = 0;
	COLORREF clrFace = 0;
	int iIconEffect = 0;
	int iPartId = 0;
	int iStateId = 0;
	RECT rcText{};
	UINT uAlign = 0;
};
typedef NMLVCUSTOMDRAW* LPNMLVCUSTOMDRAW;
struct NMLVGETINFOTIPW {
	NMHDR hdr{};
	DWORD dwFlags = 0;
	LPWSTR pszText = nullptr;
	int cchTextMax = 0;
	int iItem = 0;
	int iSubItem = 0;
	LPARAM lParam = 0;
};
typedef NMLVGETINFOTIPW NMLVGETINFOTIP;
typedef NMLVGETINFOTIPW* LPNMLVGETINFOTIP;
#endif
struct NMLISTVIEW {
	NMHDR hdr{};
	int iItem = 0;
	int iSubItem = 0;
	UINT uNewState = 0;
	UINT uOldState = 0;
	UINT uChanged = 0;
	POINT ptAction{};
	LPARAM lParam = 0;
};
typedef NMLISTVIEW* LPNMLISTVIEW;

#ifndef LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE 0x00000040
#endif
#ifndef DS_SETFONT
#define DS_SETFONT 0x0040
#endif
#ifndef RT_DIALOG
#define RT_DIALOG MAKEINTRESOURCE(5)
#endif
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif
#ifndef STATUSCLASSNAME
#define STATUSCLASSNAMEW L"msctls_statusbar32"
#define STATUSCLASSNAME STATUSCLASSNAMEW
#define SB_SETTEXT (WM_USER + 1)
#define SB_GETTEXT (WM_USER + 2)
#define SB_GETTEXTLENGTH (WM_USER + 3)
#define SB_SETPARTS (WM_USER + 4)
#define SB_GETPARTS (WM_USER + 6)
#define SB_GETBORDERS (WM_USER + 7)
#define SB_GETRECT (WM_USER + 10)
#define SBT_OWNERDRAW 0x1000
#define SBT_NOBORDERS 0x0100
#define SBT_POPOUT 0x0200
#define SBT_RTLREADING 0x0400
#define SBARS_SIZEGRIP 0x0100
#endif
#ifndef RBN_FIRST
#define RBN_FIRST (0U - 831U)
#define RBN_CHEVRONPUSHED (RBN_FIRST - 1)
#define RBN_HEIGHTCHANGE (RBN_FIRST - 3)
#define RBBIM_CHILD 0x00000010
#endif
#ifndef SPI_GETWORKAREA
#define SPI_GETWORKAREA 0x0030
#endif
#ifndef SM_CYSMCAPTION
#define SM_CYSMCAPTION 51
#define SM_CYSIZEFRAME 33
#define SM_CYBORDER 6
#endif
inline HMONITOR MonitorFromRect(const RECT*, DWORD) { return nullptr; }
#ifndef MONITOR_DEFAULTTONULL
#define MONITOR_DEFAULTTONULL 0
#endif
inline HRSRC FindResourceW(HMODULE, const wchar_t*, const wchar_t*) { return nullptr; }
#define FindResource FindResourceW
inline HGLOBAL LoadResource(HMODULE, HRSRC) { return nullptr; }
inline void* LockResource(HGLOBAL) { return nullptr; }
inline DWORD SizeofResource(HMODULE, HRSRC) { return 0; }

#ifndef GetWindowLong
#define GetWindowLong GetWindowLongW
#endif
#ifndef BF_RECT
#define BF_RECT 0x0000000F
#define BF_MIDDLE 0x00000800
#define BF_ADJUST 0x00002000
#define EDGE_ETCHED 0x00000006
#endif
#ifndef DFC_BUTTON
#define DFC_BUTTON 4
#define DFCS_BUTTONCHECK 0x0000
#define DFCS_CHECKED 0x0000400
#endif
#ifndef DLGC_WANTARROWS
#define DLGC_WANTARROWS 0x0001
#define DLGC_WANTCHARS 0x0080
#define DLGC_DEFPUSHBUTTON 0x0010
#endif
#ifndef DT_EDITCONTROL
#define DT_EDITCONTROL 0x00002000
#define DT_WORDBREAK 0x00000010
#endif
#ifndef FW_HEAVY
#define FW_HEAVY 700
#endif
#ifndef GRAY_BRUSH
#define GRAY_BRUSH 2
#define NULL_PEN 8
#endif
#ifndef LBS_STANDARD
#define LBS_STANDARD 0x00A00003
#define LB_GETCOUNT 0x018B
#endif
#ifndef R2_XORPEN
#define R2_XORPEN 7
#endif
#ifndef SB_HORZ
#define SB_HORZ 0
#define SB_LINELEFT 0
#define SB_LINERIGHT 1
#define SB_PAGELEFT 2
#define SB_PAGERIGHT 3
#endif
#ifndef SIZE_MAXIMIZED
#define SIZE_MAXIMIZED 2
#endif
#ifndef SM_CYEDGE
#define SM_CYEDGE 46
#define SM_CXEDGE 45
#define SM_CYHSCROLL 3
#define SM_CXVSCROLL 2
#endif
inline BOOL CreateCaret(HWND, HBITMAP, int, int) { return TRUE; }
inline BOOL DestroyCaret() { return TRUE; }
inline BOOL HideCaret(HWND) { return TRUE; }
inline BOOL SetCaretPos(int, int) { return TRUE; }
inline BOOL DrawEdge(HDC, LPRECT, UINT, UINT) { return TRUE; }
inline BOOL DrawFrameControl(HDC, LPRECT, UINT, UINT) { return TRUE; }
inline int DrawTextExW(HDC, wchar_t*, int, LPRECT, UINT, void*) { return 0; }
#define DrawTextEx DrawTextExW
inline int GetROP2(HDC) { return 0; }
inline int SetROP2(HDC, int) { return 0; }
inline int GetScrollPos(HWND, int) { return 0; }
inline BOOL GetScrollRange(HWND, int, int*, int*) { return TRUE; }
inline BOOL SetScrollRange(HWND, int, int, int, BOOL) { return TRUE; }
inline BOOL ShowScrollBar(HWND, int, BOOL) { return TRUE; }
inline BOOL ShowCaret(HWND) { return TRUE; }
inline int ToAscii(UINT, UINT, const BYTE*, LPWORD, UINT) { return 0; }
inline BOOL GetKeyboardState(BYTE*) { return TRUE; }
inline HGDIOBJ GetStockObject(int) { return nullptr; }
