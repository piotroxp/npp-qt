// Window.h — Qt6 translation of Window utilities
#pragma once

#include <QWidget>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QColor>

// Common window utilities shim for Win32 → Qt6

// ─── Rect helpers ────────────────────────────────────────────────

inline QRect makeRect(int left, int top, int right, int bottom)
{
    return QRect(left, top, right - left, bottom - top);
}

inline void setRect(QRect& r, int x1, int y1, int x2, int y2)
{
    r = QRect(x1, y1, x2 - x1, y2 - y1);
}

inline void setRectEmpty(QRect& r) { r = QRect(); }
inline bool isRectEmpty(const QRect& r) { return r.isEmpty(); }

inline void copyRect(QRect& dest, const QRect& src) { dest = src; }

inline QRect intersectRect(const QRect& a, const QRect& b)
{
    return a.intersected(b);
}

// ─── Point helpers ───────────────────────────────────────────────

inline QPoint makePoint(long x, long y) { return QPoint(x, y); }

// ─── Color helpers ──────────────────────────────────────────────

inline QRgb RGB(int r, int g, int b) { return qRgb(r, g, b); }
inline int GetRValue(QRgb c) { return qRed(c); }
inline int GetGValue(QRgb c) { return qGreen(c); }
inline int GetBValue(QRgb c) { return qBlue(c); }

// ─── HICON → QIcon ───────────────────────────────────────────

inline QIcon iconFromResource(int id, int size = 16)
{
    // Load from Qt resource system
    Q_UNUSED(id);
    Q_UNUSED(size);
    return QIcon();
}

inline QPixmap pixmapFromResource(int id, int width = 16, int height = 16)
{
    Q_UNUSED(id);
    Q_UNUSED(width);
    Q_UNUSED(height);
    return QPixmap();
}

// ─── Window style helpers ──────────────────────────────────────

inline Qt::WindowFlags makeWindowFlags(unsigned long style)
{
    Qt::WindowFlags flags = Qt::Widget;
    if (style & 0x10000000) flags |= Qt::Window; // WS_VISIBLE
    if (style & 0x08000000) flags |= Qt::Window; // Not disabled - handled separately
    if (style & 0x00C00000) flags |= Qt::WindowTitleHint; // Has caption
    if (style & 0x00800000) flags |= Qt::FramelessWindowHint; // Border
    if (style & 0x00080000) flags |= Qt::WindowSystemMenuHint; // Sys menu
    if (style & 0x00040000) flags |= Qt::MSWindowsFixedSizeDialogHint; // Thick frame (resize)
    if (style & 0x00020000) flags |= Qt::WindowMinimizeButtonHint;
    if (style & 0x00010000) flags |= Qt::WindowMaximizeButtonHint;
    return flags;
}

// ─── Client/Window rect ────────────────────────────────────────

inline QRect getClientRect(QWidget* w)
{
    return w->rect();
}

inline QRect getWindowRect(QWidget* w)
{
    return QRect(w->mapToGlobal(QPoint(0, 0)), w->size());
}

// ─── Move/Resize ──────────────────────────────────────────────

inline void moveWindow(QWidget* w, int x, int y, int cx = -1, int cy = -1)
{
    if (cx < 0 || cy < 0)
        w->move(x, y);
    else
        w->setGeometry(x, y, cx, cy);
}

inline void resizeTo(QWidget* w, const QRect& rc)
{
    w->setGeometry(rc);
}

// ─── Visibility ────────────────────────────────────────────────

inline bool isWindowVisible(QWidget* w) { return w->isVisible(); }
inline bool isWindowEnabled(QWidget* w) { return w->isEnabled(); }
inline void enableWindow(QWidget* w, bool enable) { w->setEnabled(enable); }
inline void showWindow(QWidget* w, int cmd)
{
    switch (cmd)
    {
        case 0: w->hide(); break; // SW_HIDE
        case 1: w->show(); break; // SW_SHOWNORMAL
        case 3: w->showMaximized(); break; // SW_SHOWMAXIMIZED
        case 6: w->showMinimized(); break; // SW_MINIMIZE
        case 9: w->showNormal(); break; // SW_RESTORE
        default: w->show(); break;
    }
}

// ─── Focus ───────────────────────────────────────────────────

inline void setFocus(QWidget* w) { w->setFocus(); }
inline QWidget* getFocus() { return QApplication::focusWidget(); }
inline QWidget* getActiveWindow() { return QApplication::activeWindow(); }

// ─── Cursor ──────────────────────────────────────────────────

inline Qt::CursorShape loadCursor(int id)
{
    switch (id)
    {
        case 1: case 32512: return Qt::ArrowCursor;
        case 32513: return Qt::CrossCursor;
        case 32514: return Qt::SizeAllCursor;
        case 32515: return Qt::WaitCursor;
        case 32516: return Qt::IBeamCursor;
        case 32645: return Qt::SizeAllCursor;
        case 32649: return Qt::ForbiddenCursor;
        case 32648: return Qt::PointingHandCursor;
        default: return Qt::ArrowCursor;
    }
}

// ─── Screen info ─────────────────────────────────────────────

inline QSize getScreenSize()
{
    if (QWidget* w = QApplication::primaryScreen())
        return w->size();
    return QSize(1920, 1080);
}

inline QRect getWorkArea()
{
    if (QWidget* w = QApplication::primaryScreen())
        return w->availableGeometry();
    return QRect(0, 0, 1920, 1080);
}
