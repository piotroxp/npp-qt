// Semantic Lift: Win32 DockingSplitter → Qt6 QSplitter event handlers
// Original: PowerEditor/src/WinControls/DockingWnd/DockingSplitter.cpp
// Target: npp-qt/WinControls/DockingWnd/DockingSplitter.cpp

#include "DockingSplitter.h"
#include "NppDarkMode.h"
#include <QPainter>
#include <QApplication>

// =============================================================================
// init — replaces Win32 RegisterClass + CreateWindowEx
// =============================================================================
void DockingSplitter::init(QWidget* hWnd, QWidget* hMessage, unsigned int flags)
{
    // Set parent — Qt6 equivalent of WS_CHILD | WS_VISIBLE
    setParent(hWnd);
    setAttribute(Qt::WA_StyledBackground);

    _hMessage = hMessage;
    _flags    = flags;
    _isRTL    = qApp->layoutDirection() == Qt::RightToLeft;

    // ── Cursor (replaces LoadCursor(NULL, IDC_SIZENS/IDC_SIZEWE)) ─────────
    Qt::CursorShape cur = (flags & DMS_HORIZONTAL)
        ? Qt::SizeVerCursor    // NS resize — nsdockspliter
        : Qt::SizeHorCursor;   // WE resize — wedockspliter
    setCursor(cur);

    // ── Splitter width (SPLITTER_WIDTH = 4px) ─────────────────────────────
    // Horizontal splitter: fixed height, full width from parent
    // Vertical splitter:   fixed width,  full height from parent
    if (flags & DMS_HORIZONTAL) {
        setFixedHeight(SPLITTER_WIDTH);
    } else {
        setFixedWidth(SPLITTER_WIDTH);
    }

    // ── Initial position (CW_USEDEFAULT → let parent lay out) ─────────────
    // Qt6: size already set by setFixedHeight/Width; resize(0,0) to be
    // placeholder; the parent's layout controls geometry.
    resize(0, 0);

    // ── Background colour ─────────────────────────────────────────────────
    // Win32: GetSysColorBrush(COLOR_3DFACE) → Qt: use palette button face
    // Dark mode handled in paintEvent.
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, pal.color(QPalette::Button));
    setPalette(pal);

    show();
}

// =============================================================================
// mousePressEvent — replaces WM_LBUTTONDOWN + SetCapture
// =============================================================================
void DockingSplitter::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _isLeftButtonDown = true;
        _ptOldPos = event->globalPosition().toPoint();
        grabMouse();  // Qt equivalent of SetCapture()
        event->accept();
    }
}

// =============================================================================
// mouseReleaseEvent — replaces WM_LBUTTONUP + ReleaseCapture
// =============================================================================
void DockingSplitter::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _isLeftButtonDown = false;
        releaseMouse();  // Qt equivalent of ReleaseCapture()
        event->accept();
    }
}

// =============================================================================
// mouseMoveEvent — replaces WM_MOUSEMOVE + SendMessage(DMM_MOVE_SPLITTER)
// =============================================================================
void DockingSplitter::mouseMoveEvent(QMouseEvent* event)
{
    if (!_isLeftButtonDown) {
        return;
    }

    QPoint ptNew = event->globalPosition().toPoint();
    int delta = 0;

    if (_flags & DMS_HORIZONTAL) {
        // North-South: vertical movement changes panel size
        delta = _ptOldPos.y() - ptNew.y();  // positive = up = shrink top panel
    } else {
        // West-East: horizontal movement
        delta = _isRTL
            ? ptNew.x() - _ptOldPos.x()
            : _ptOldPos.x() - ptNew.x();
    }

    if (delta != 0) {
        // Emit Qt signals (primary path)
        emit splitterMoved(delta, this);
        emit moveSplitter(delta);

        // Also notify the Win32-style message target (for shim compatibility)
        if (_hMessage) {
            // Create a custom Qt event with type DMM_MOVE_SPLITTER
            // that the message target can process via customEvent()
            QEvent* fakeEvent = new QEvent(static_cast<QEvent::Type>(DMM_MOVE_SPLITTER));
            QApplication::postEvent(_hMessage, fakeEvent);
        }
    }

    _ptOldPos = ptNew;
}

// =============================================================================
// paintEvent — replaces WM_ERASEBKGND + FillRect
// =============================================================================
void DockingSplitter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    if (NppDarkMode::isEnabled_Static()) {
        painter.fillRect(rect(), NppDarkMode::getDlgBackgroundBrush());
    } else {
        // Win32: COLOR_3DFACE (button face colour)
        painter.fillRect(rect(), palette().color(QPalette::Button));
    }
}
