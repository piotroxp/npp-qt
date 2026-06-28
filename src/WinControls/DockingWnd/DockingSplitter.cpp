// Semantic Lift: Win32 DockingSplitter → Qt6 splitter widget
// Original: PowerEditor/src/WinControls/DockingWnd/DockingSplitter.cpp
// Target: npp-qt/src/WinControls/DockingWnd/DockingSplitter.cpp

#include "DockingSplitter.h"
#include "Docking.h"

#include <QPainter>
#include <QPalette>

// =============================================================================
// DockingSplitter — draggable panel separator
//
// Win32:
//   - Custom WNDPROC handles WM_LBUTTONDOWN/UP, WM_MOUSEMOVE, WM_ERASEBKGND
//   - Sends DMM_MOVE_SPLITTER to _hMessage on drag
//   - Uses IDC_SIZENS / IDC_SIZEWE cursor based on DMS_HORIZONTAL flag
//
// Qt6:
//   - QWidget with mouse tracking (mousePressEvent, mouseReleaseEvent, mouseMoveEvent)
//   - Emits splitterMoved(delta, this) signal on drag
//   - DockingManager connects and adjusts panel sizes
//
// Win32 → Qt6 message mapping:
//   WM_LBUTTONDOWN  → mousePressEvent + QCursor::pos()
//   WM_LBUTTONUP    → mouseReleaseEvent
//   WM_MOUSEMOVE    → mouseMoveEvent + QCursor::pos()
//   WM_ERASEBKGND   → paintEvent (NppDarkMode via stylesheet)
//   SendMessage(DMM_MOVE_SPLITTER, ...) → emit splitterMoved(delta, this)
// =============================================================================

// =============================================================================
// paintEvent — WM_ERASEBKGND equivalent
// =============================================================================
void DockingSplitter::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // Win32: GetSysColorBrush(COLOR_3DFACE) — themed 3D face color
    // Qt6:   use QPalette for theming; NppDarkMode overrides via stylesheet
    QPalette::ColorRole bgRole = QPalette::Window;
    if (_flags & DMS_HORIZONTAL) {
        // Horizontal splitter: thin vertical bar — slightly darker
        painter.fillRect(rect(), palette().color(bgRole).darker(110));
    } else {
        // Vertical splitter: thin horizontal bar
        painter.fillRect(rect(), palette().color(bgRole).darker(110));
    }
}