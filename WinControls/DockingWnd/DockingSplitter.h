// Semantic Lift: Win32 DockingSplitter → Qt6 QSplitter
// Original: PowerEditor/src/WinControls/DockingWnd/DockingSplitter.h
// Target: npp-qt/WinControls/DockingWnd/DockingSplitter.h

#pragma once

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QCursor>
#include <QPaintEngine>

#include "Docking.h"
#include "dockingResource.h"

// =============================================================================
// DockingSplitter — draggable splitter between docked panels.
// Replaces the Win32 WNDPROC-based splitter with a QWidget that emits Qt signals.
// DMS_VERTICAL   → horizontal splitter (drag top↔bottom, NS cursor)
// DMS_HORIZONTAL → vertical splitter   (drag left↔right, WE cursor)
// =============================================================================

constexpr int DMS_VERTICAL   = 0x00000001;
constexpr int DMS_HORIZONTAL = 0x00000002;

class DockingSplitter : public QWidget
{
    Q_OBJECT

public:
    DockingSplitter() = default;
    ~DockingSplitter() override = default;

    // ── Initialisation ─────────────────────────────────────────────────────

    // Initialise the splitter.  hMessage is the widget that receives
    // DMM_MOVE_SPLITTER notifications.  flags is DMS_VERTICAL|HORIZONTAL.
    void init(QWidget* hWnd, QWidget* hMessage, unsigned int flags);

    // ── Drag state ──────────────────────────────────────────────────────────

    bool isDragging() const { return _isLeftButtonDown; }
    void cancelDrag() { _isLeftButtonDown = false; releaseMouse(); }

    // ── Splitter orientation ────────────────────────────────────────────────

    bool isHorizontal() const { return (_flags & DMS_HORIZONTAL) != 0; }
    bool isVertical() const   { return (_flags & DMS_VERTICAL) != 0; }

    // Convenience: map Qt::CursorShape to Win32 IDC_* cursor names
    static const char* cursorName(bool isHorizontal) {
        return isHorizontal ? "IDC_SIZENS" : "IDC_SIZEWE";
    }

public Q_SIGNALS:
    // Emitted when the splitter is dragged by `delta` pixels.
    // Positive delta: panel shrinks; negative: panel grows.
    void splitterMoved(int delta, QWidget* splitter);

    // Convenience signal: raw DMM_MOVE_SPLITTER integer (for shim compatibility)
    void moveSplitter(int delta);

protected:
    // ── Qt event handlers (replaces WNDPROC messages) ───────────────────────

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QWidget* _hMessage = nullptr;
    QPoint   _ptOldPos;
    unsigned int _flags = 0;
    bool     _isLeftButtonDown = false;
    bool     _isRTL = false;  // true if layout direction is RTL
};
