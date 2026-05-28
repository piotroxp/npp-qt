#pragma once

#ifndef _WINDOWS_COMPAT_H
#define _WINDOWS_COMPAT_H
#define _WINDOWS_COMPAT_H_DEFINED  // Signal to windows.h that types are already defined

// Basic types
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <unistd.h>  // For ::usleep() on glibc systems


typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef int64_t LONGLONG;
typedef uint64_t ULONG;
typedef int32_t BOOL;
typedef int INT;
typedef unsigned int UINT;
typedef intptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;
typedef uintptr_t UINT_PTR;
typedef intptr_t INT_PTR;
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

// Constants (guard against redefinition)
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
// MAX_PATH and INFINITE constants (guard against redefinition)
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

// Structures
#ifndef RECT
struct RECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    RECT() : left(0), top(0), right(0), bottom(0) {}
    RECT(LONG l, LONG t, LONG r, LONG b) : left(l), top(t), right(r), bottom(b) {}
};
#endif

#ifndef POINT
struct POINT {
    LONG x;
    LONG y;
    POINT() : x(0), y(0) {}
    POINT(LONG x_, LONG y_) : x(x_), y(y_) {}
};
#endif

#ifndef SIZE
struct SIZE {
    LONG cx;
    LONG cy;
    SIZE() : cx(0), cy(0) {}
    SIZE(LONG cx_, LONG cy_) : cx(cx_), cy(cy_) {}
};
// FILETIME structure
struct FILETIME {
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
};

#endif

#ifndef LPRECT
typedef RECT* LPRECT;
typedef const RECT* LPCRECT;
typedef POINT* LPPOINT;
typedef SIZE* LPSIZE;
#endif

// Macros
#define LOWORD(l) ((WORD)((DWORD)(l) & 0xFFFF))
#define HIWORD(l) ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w) ((BYTE)((DWORD)(w) & 0xFF))
#define HIBYTE(w) ((BYTE)((DWORD)(w) >> 8))
#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))

// Undefine any existing RGB macro and redefine
#ifdef RGB
#undef RGB
#endif
#define RGB(r,g,b) ((DWORD)((r) | ((g) << 8) | ((b) << 16)))

// Stub WINAPI definition
#ifdef WINAPI
#undef WINAPI
#endif
#define WINAPI

// ListView extended styles
#ifndef LVS_EX_FULLROWSELECT
#define LVS_EX_FULLROWSELECT 0x00000020
#endif

// Stub functions
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
#ifndef OutputDebugString
#define OutputDebugString OutputDebugStringA
#endif
#ifndef ReleaseDC
inline int ReleaseDC(HWND, HDC) { return 1; }
#endif
#ifndef DeleteDC
inline int DeleteDC(HDC) { return 1; }
#endif
#ifndef DeleteObject
inline int DeleteObject(HGDIOBJ) { return 1; }
#endif
#ifndef RegCloseKey
inline LONG RegCloseKey(HKEY) { return 0; }
#endif

// File attributes (with redefinition guards)
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0x80
#endif
#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#endif
#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY 0x1
#endif
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF

// Scintilla forward declaration
namespace Scintilla::Internal { class ScintillaQt; }
using ScintillaQt = Scintilla::Internal::ScintillaQt;

// Note: NppPosition is defined in Parameters.h (included later)
// SavingStatus, LangType, etc. are also in Parameters.h

// Qt includes
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
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QSplitter>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QScrollArea>
#include <QDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QFont>
#include <QFontMetrics>
#include <QPalette>
#include <QIcon>
#include <QClipboard>
#include <QKeySequence>
#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSlider>
#include <QProgressBar>
#include <QDoubleSpinBox>
#include <QDate>
#include <QTime>
#include <QDateTimeEdit>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>

// Helper functions
inline QWidget* asWidget(HWND h) { return reinterpret_cast<QWidget*>(h); }
inline HWND asHWND(QWidget* w) { return reinterpret_cast<HWND>(w); }
inline QRect toQRect(const RECT& r) { return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top); }
inline RECT fromQRect(const QRect& qr) { return RECT(qr.left(), qr.top(), qr.right(), qr.bottom()); }
inline bool GetWindowRect(HWND h, RECT* r) { if(QWidget* w=asWidget(h)) { QRect qr=w->geometry(); r->left=qr.left(); r->top=qr.top(); r->right=qr.right(); r->bottom=qr.bottom(); return true; } return false; }
inline bool GetClientRect(HWND h, RECT* r) { if(QWidget* w=asWidget(h)) { QSize s=w->size(); r->left=0; r->top=0; r->right=s.width(); r->bottom=s.height(); return true; } return false; }
inline void setEnabled(QWidget* w, bool enabled) { if (w) w->setEnabled(enabled); }
inline void setVisible(QWidget* w, bool visible) { if (w) visible ? w->show() : w->hide(); }
inline QWidget* getFocus() { return QApplication::focusWidget(); }
inline void setFocus(QWidget* w) { if (w) w->setFocus(); }
inline bool SetForegroundWindow(QWidget* w) { if(w) { w->activateWindow(); w->raise(); return true; } return false; }

#endif // _WINDOWS_COMPAT_H
