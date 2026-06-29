// Semantic Lift: Win32 DockingSplitter → Qt6 QSplitter drag handle
// Original: PowerEditor/src/WinControls/DockingWnd/DockingSplitter.h
// Target: npp-qt/src/WinControls/DockingWnd/DockingSplitter.h

#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QCursor>
#include <QPainter>
#include <QCoreApplication>

#include "Window.h"

// =============================================================================
// DockingSplitter flags — matches original DMS_* constants
// =============================================================================
constexpr unsigned int DMS_VERTICAL   = 0x00000001;  // East-west resize
constexpr unsigned int DMS_HORIZONTAL = 0x00000002;  // North-south resize

// =============================================================================
// DockingSplitter — draggable splitter between docked panels
//
// Win32: Custom WNDCLASS with CS_HREDRAW|CS_VREDRAW, IDC_SIZEWE/IDC_SIZENS cursor,
//        WM_LBUTTONDOWN/UP, WM_MOUSEMOVE handling. Sends DMM_MOVE_SPLITTER to parent.
// Qt6:   QWidget subclass with mouse tracking. Emits splitterMoved(delta) signal
//        consumed by DockingManager for panel resize.
// =============================================================================
class DockingSplitter : public Window
{
    Q_OBJECT

public:
    DockingSplitter() = default;
    ~DockingSplitter() override = default;

    void destroy() override {}

    // =======================================================================
    // init — initialize splitter widget
    //
    // Win32: RegisterClass (if needed), CreateWindowEx for "nsdockspliter"/"wedockspliter"
    // Qt6:   Set up widget properties, cursor, and mouse tracking
    // =======================================================================
    void init(QWidget* parent, QWidget* messageTarget, unsigned int flags) {
        _hParent = parent;
        _hMessage = messageTarget;
        _flags = flags;

        setParent(parent);
        setAttribute(Qt::WA_OpaquePaintEvent, false);

        // Detect RTL layout (Win32: GWL_EXSTYLE & WS_EX_LAYOUTRTL)
        _isRTL = (layoutDirection() == Qt::RightToLeft);

        // Set cursor based on orientation (Win32: IDC_SIZENS / IDC_SIZEWE)
        if (flags & DMS_HORIZONTAL) {
            setCursor(Qt::SizeVerCursor);   // North-South = vertical resize
        } else {
            setCursor(Qt::SizeHorCursor);   // East-West = horizontal resize
        }

        // Make widget visible, thin sizing
        setMinimumSize(SPLITTER_WIDTH, SPLITTER_WIDTH);
        setMaximumSize(16, 16);
        show();
    }

signals:
    // Emitted when user drags the splitter — delta is pixels moved
    // Win32: SendMessage(_hMessage, DMM_MOVE_SPLITTER, delta, (LPARAM)_hSelf)
    void splitterMoved(int delta, QWidget* splitter);

protected:
    // Win32: WM_LBUTTONDOWN → SetCapture, record position
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            _isLeftButtonDown = true;
            _ptOldPos = QCursor::pos();
            event->accept();
        }
    }

    // Win32: WM_LBUTTONUP → ReleaseCapture
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            _isLeftButtonDown = false;
            event->accept();
        }
    }

    // Win32: WM_MOUSEMOVE → if button down, send DMM_MOVE_SPLITTER
    void mouseMoveEvent(QMouseEvent* event) override {
        Q_UNUSED(event);
        if (_isLeftButtonDown) {
            QPoint pt = QCursor::pos();

            if ((_flags & DMS_HORIZONTAL) && (_ptOldPos.y() != pt.y())) {
                int delta = _ptOldPos.y() - pt.y();
                emit splitterMoved(delta, this);
            } else if (_ptOldPos.x() != pt.x()) {
                int delta = _isRTL ? (pt.x() - _ptOldPos.x()) : (_ptOldPos.x() - pt.x());
                emit splitterMoved(delta, this);
            }
            _ptOldPos = pt;
        }
    }

    // Win32: WM_ERASEBKGND → FillRect with COLOR_3DFACE brush (or NppDarkMode)
    // Qt6:   fill with themed background color
    void paintEvent(QPaintEvent* event) override;

private:
    QWidget* _hMessage = nullptr;           // Target widget for splitterMoved signal

    bool _isLeftButtonDown = false;
    QPoint _ptOldPos{0, 0};
    unsigned int _flags = 0;

    // RTL layout direction (Win32: GWL_EXSTYLE & WS_EX_LAYOUTRTL)
    bool _isRTL = false;
};