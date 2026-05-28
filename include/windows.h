// Stub windows.h for Linux Qt6 compilation
#pragma once

// Minimal Windows API stubs for cross-platform compilation

#ifndef WIN32
#define WIN32
#endif

#ifndef _WINDOWS
#define _WINDOWS
#endif

// Basic Windows types are already defined in WindowsCompat.h
// These stubs are only for files that #include <windows.h>

// Window message constants
#ifndef WM_USER
#define WM_USER 0x0400
#endif

#ifndef WM_CLOSE
#define WM_CLOSE 0x0010
#endif

#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif

#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif

#ifndef WM_PAINT
#define WM_PAINT 0x000F
#endif

#ifndef WM_CREATE
#define WM_CREATE 0x0001
#endif

#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif

#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif

#ifndef WM_KEYDOWN
#define WM_KEYDOWN 0x0100
#endif

#ifndef WM_KEYUP
#define WM_KEYUP 0x0101
#endif

#ifndef WM_CHAR
#define WM_CHAR 0x0102
#endif

#ifndef WM_MOUSEMOVE
#define WM_MOUSEMOVE 0x0200
#endif

#ifndef WM_LBUTTONDOWN
#define WM_LBUTTONDOWN 0x0201
#endif

#ifndef WM_LBUTTONUP
#define WM_LBUTTONUP 0x0202
#endif

#ifndef WM_RBUTTONDOWN
#define WM_RBUTTONDOWN 0x0204
#endif

#ifndef WM_RBUTTONUP
#define WM_RBUTTONUP 0x0205
#endif

#ifndef WM_MBUTTONDOWN
#define WM_MBUTTONDOWN 0x0207
#endif

#ifndef WM_MBUTTONUP
#define WM_MBUTTONUP 0x0208
#endif

#ifndef EN_CHANGE
#define EN_CHANGE 0x0300
#endif

#ifndef EN_MSGFILTER
#define EN_MSGFILTER 0x0700
#endif

#ifndef EN_DROPFILES
#define EN_DROPFILES 0x0303
#endif

#ifndef EN_HSCROLL
#define EN_HSCROLL 0x0601
#endif

#ifndef EN_VSCROLL
#define EN_VSCROLL 0x0602
#endif

// WPARAM and LPARAM are defined in WindowsCompat.h
// Just ensure they're available
#ifndef WPARAM
typedef intptr_t WPARAM;
#endif

#ifndef LPARAM
typedef intptr_t LPARAM;
#endif

#ifndef LRESULT
typedef intptr_t LRESULT;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

// Color constants
#ifndef COLOR_WINDOW
#define COLOR_WINDOW 5
#endif

// CW_USEDEFAULT
#ifndef CW_USEDEFAULT
#define CW_USEDEFAULT ((int)0x80000000)
#endif

// HFILE_ERROR
#ifndef HFILE_ERROR
#define HFILE_ERROR ((int)-1)
#endif

// Make WPARAM and LPARAM available as macros for consistency
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif