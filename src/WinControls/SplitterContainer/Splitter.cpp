// Semantic Lift: Win32 Splitter → Qt6 Splitter widget implementation
// Original: PowerEditor/src/WinControls/SplitterContainer/Splitter.cpp
// Target: npp-qt/src/WinControls/SplitterContainer/Splitter.cpp
//
// This file provides the concrete Qt6 implementation of the Splitter widget.
// It is included by SplitterContainer.cpp; the Splitter class is exposed
// via SplitterContainer.h which includes Splitter.h.
//
// Key Win32 → Qt6 mappings:
//   WNDCLASSEX + RegisterClassEx     → QWidget constructor + setAttribute
//   CreateWindowEx                  → new Splitter()
//   SetCapture() / ReleaseCapture()  → implicit via mouse grab
//   GetClientRect()                 → rect()
//   MoveWindow()                    → setGeometry() / resize() + move()
//   FillRect() + CreateSolidBrush   → QPainter fillRect()
//   BeginPaint() / EndPaint()       → paintEvent() + QPainter
//   SendMessage(WM_RESIZE_CONTAINER) → signal splitterMoved
//   SendMessage(WM_GETSPLITTER_X/Y) → query via parent callback

#include "SplitterContainer/Splitter.h"
#include "SplitterContainer.h"
#include "NppDarkMode.h"
#include "MISC/Common/dpiManagerV2.h"
#include <QPainter>
#include <QApplication>
#include <QCursor>
#include <QScreen>
#include <QDebug>

// =============================================================================
// Splitter — Qt6 resize handle widget
// =============================================================================

// Static registration flags (not needed in Qt — each instance is unique)
bool Splitter::_isHorizontalRegistered = false;
bool Splitter::_isVerticalRegistered = false;
bool Splitter::_isHorizontalFixedRegistered = false;
bool Splitter::_isVerticalFixedRegistered = false;

constexpr int HEIGHT_MINIMAL = 15;

Splitter::Splitter(Qt::Orientation orient, QWidget* parent)
    : QWidget(parent)
    , _orientation(orient)
{
    // Set cursor style based on orientation
    setCursor(orient == Qt::Horizontal ? Qt::SizeHorCursor : Qt::SizeVerCursor);
    setMouseTracking(true);
}

void Splitter::mousePressEvent(QMouseEvent* event)
{
    if (_isFixed) return;

    QPoint pt = event->pos();
    if ((isInLeftTopZone(pt)) && (event->button() == Qt::LeftButton)) {
        gotoTopLeft();
        return;
    }
    if ((isInRightBottomZone(pt)) && (event->button() == Qt::LeftButton)) {
        gotoRightBottom();
        return;
    }

    setCursor(Qt::ClosedHandCursor);
    _isDraged = true;
    _isLeftButtonDown = true;
    _dragStartPos = event->globalPos();
    _dragStartRatio = _splitRatio;
    update();
}

void Splitter::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (!_isFixed && _isLeftButtonDown) {
        unsetCursor();
        _isLeftButtonDown = false;
    }
}

void Splitter::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pt = event->pos();

    // Show hand cursor when hovering over collapse zones
    if (isInLeftTopZone(pt) || isInRightBottomZone(pt)) {
        setCursor(Qt::PointingHandCursor);
        return;
    }

    if (_isFixed || (!_isDraged) || (!_isLeftButtonDown)) {
        if (!_isDraged) {
            setCursor(_orientation == Qt::Horizontal ? Qt::SizeHorCursor : Qt::SizeVerCursor);
        }
        return;
    }

    // Dragging — compute new ratio from mouse delta
    QWidget* parent = parentWidget();
    if (!parent) return;

    QRect rt = parent->rect();

    if (_orientation == Qt::Horizontal) {
        // Vertical splitter (drag up/down to resize)
        int dx = event->globalPos().x() - _dragStartPos.x();
        double delta = static_cast<double>(dx) / rt.width();
        double newRatio = _dragStartRatio + delta;
        newRatio = qBound(0.01, newRatio, 0.99);
        _splitRatio = newRatio;
    } else {
        // Horizontal splitter (drag left/right to resize)
        int dy = event->globalPos().y() - _dragStartPos.y();
        double delta = static_cast<double>(dy) / rt.height();
        double newRatio = _dragStartRatio + delta;
        newRatio = qBound(0.01, newRatio, 0.99);
        _splitRatio = newRatio;
    }

    // Notify container to resize
    emit splitterMoved(_splitRatio);
    update();
}

void Splitter::mouseDoubleClickEvent(QMouseEvent* event)
{
    if ((_flags & SV_ENABLELDBLCLK) && (event->button() == Qt::LeftButton)) {
        gotoTopLeft();
    } else if ((_flags & SV_ENABLERDBLCLK) && (event->button() == Qt::RightButton)) {
        gotoRightBottom();
    }
}

void Splitter::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    drawSplitter(painter);
}

void Splitter::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event);
    setCursor(_orientation == Qt::Horizontal ? Qt::SizeHorCursor : Qt::SizeVerCursor);
}

void Splitter::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    if (!_isDraged) {
        unsetCursor();
    }
}

int Splitter::getClickZone(WH which) const
{
    if (_splitterSize <= 8) {
        return isVertical()
            ? (which == WH::width  ? _splitterSize  : HEIGHT_MINIMAL)
            : (which == WH::width  ? HEIGHT_MINIMAL : _splitterSize);
    } else {
        return isVertical()
            ? ((which == WH::width)  ? 8  : 15)
            : ((which == WH::width) ? 15 : 8);
    }
}

void Splitter::drawSplitter(QPainter& painter)
{
    QRect rc = rect();
    bool isDarkMode = NppDarkMode::isEnabled_Static();

    // Background
    if (isDarkMode) {
        painter.fillRect(rc, NppDarkMode::getDlgBackgroundBrush());
    } else {
        painter.fillRect(rc, palette().color(QPalette::Button));
    }

    // DPI-scaled sizing
    const unsigned int dpi = DPIManagerV2::getDpiForWindow(parentWidget());
    const int scaled1 = DPIManagerV2::scale(1, nullptr);
    const int scaled2 = DPIManagerV2::scale(2, nullptr);
    const int scaled3 = DPIManagerV2::scale(3, nullptr);
    const int scaled4 = DPIManagerV2::scale(4, nullptr);

    // Draw grip dots
    QColor highlight = palette().color(QPalette::Normal, QPalette::Midlight);
    QColor shadow = palette().color(QPalette::Normal, QPalette::Shadow);

    int bottom = (_flags & SV_RESIZEWTHPERCNT)
        ? (isVertical() ? rc.bottom() - _clickZone2BR.bottom() : rc.bottom())
        : rc.bottom();

    if (isVertical()) {
        // Draw vertical grip dots (horizontal splitter)
        int cx = rc.center().x();
        for (int y = scaled1; y < bottom - scaled1; y += scaled4) {
            painter.fillRect(cx - scaled1, y, scaled2, scaled2,
                             isDarkMode ? shadow : highlight);
        }
    } else {
        // Draw horizontal grip dots (vertical splitter)
        int rightEdge = (_flags & SV_RESIZEWTHPERCNT)
            ? rc.right() - _clickZone2BR.right()
            : rc.right();
        int cy = rc.center().y();
        for (int x = _clickZone2TL.right() + scaled1; x < rightEdge - scaled1; x += scaled4) {
            painter.fillRect(x, cy - scaled1, scaled2, scaled2,
                             isDarkMode ? shadow : highlight);
        }
    }

    // Draw arrows at ends if large enough
    if ((_splitterSize >= scaled4) && (_flags & SV_RESIZEWTHPERCNT)) {
        QRect tlRc, brRc;
        adjustZoneToDraw(tlRc, ZONE_TYPE::topLeft);
        adjustZoneToDraw(brRc, ZONE_TYPE::bottomRight);
        drawArrow(painter, tlRc, isVertical() ? Arrow::left  : Arrow::up);
        drawArrow(painter, brRc, isVertical() ? Arrow::right : Arrow::down);
    }
}

void Splitter::drawArrow(QPainter& painter, const QRect& r, Arrow arrowDir)
{
    QPen pen(painter.pen());
    pen.setColor(NppDarkMode::isEnabled_Static()
                     ? NppDarkMode::getDarkerTextColor()
                     : palette().color(QPalette::Text));
    painter.setPen(pen);

    switch (arrowDir) {
    case Arrow::left:
        for (int x = r.right(), y = r.top(); (x > r.left()) && (y != r.bottom()); --x) {
            painter.drawPoint(x, y++);
        }
        break;
    case Arrow::right:
        for (int x = r.left(), y = r.top(); (x < r.right()) && (y != r.bottom()); ++x) {
            painter.drawPoint(x, y++);
        }
        break;
    case Arrow::up:
        for (int x = r.left(), y = r.bottom(); (y > r.top()) && (x != r.right()); --y) {
            painter.drawPoint(x++, y);
        }
        break;
    case Arrow::down:
        for (int x = r.left(), y = r.top(); (y < r.bottom()) && (x != r.right()); ++y) {
            painter.drawPoint(x++, y);
        }
        break;
    }
}

void Splitter::adjustZoneToDraw(QRect& rc2def, ZONE_TYPE whichZone) const
{
    if (_splitterSize < 4) return;

    int w = 0, h = 0;
    if (_splitterSize <= 8) {
        w = isVertical() ? 4  : 7;
        h = isVertical() ? 7  : 4;
    } else {
        w = isVertical() ? 6  : 11;
        h = isVertical() ? 11 : 6;
    }

    if (isVertical()) {
        if (whichZone == ZONE_TYPE::topLeft) {
            rc2def.setRect(0, (_clickZone2TL.top() + _clickZone2TL.height() - h) / 2, w, h);
        } else {
            rc2def.setRect(
                _clickZone2BR.left() + _clickZone2BR.width() - w,
                (_clickZone2BR.top() + _clickZone2BR.height() - h) / 2,
                w, h);
        }
    } else {
        if (whichZone == ZONE_TYPE::topLeft) {
            rc2def.setRect((_clickZone2TL.right() + _clickZone2TL.width() - w) / 2, 0, w, h);
        } else {
            rc2def.setRect(
                (_clickZone2BR.left() + _clickZone2BR.width() - w) / 2,
                _clickZone2BR.top() + _clickZone2BR.height() - h,
                w, h);
        }
    }
}

bool Splitter::isInLeftTopZone(const QPoint& p) const
{
    return (p.x() >= _clickZone2TL.left())
        && (p.x() <= _clickZone2TL.left() + _clickZone2TL.width())
        && (p.y() >= _clickZone2TL.top())
        && (p.y() <= _clickZone2TL.top() + _clickZone2TL.height());
}

bool Splitter::isInRightBottomZone(const QPoint& p) const
{
    return (p.x() >= _clickZone2BR.left())
        && (p.x() <= _clickZone2BR.left() + _clickZone2BR.width())
        && (p.y() >= _clickZone2BR.top())
        && (p.y() <= _clickZone2BR.top() + _clickZone2BR.height());
}

void Splitter::gotoTopLeft()
{
    if (((_flags & SV_ENABLELDBLCLK) == 0) || _isFixed || (_splitRatio <= 0.01)) return;
    _splitRatio = 0.01;
    emit splitterMoved(_splitRatio);
    update();
}

void Splitter::gotoRightBottom()
{
    if (((_flags & SV_ENABLERDBLCLK) == 0) || _isFixed || (_splitRatio >= 0.99)) return;
    _splitRatio = 0.99;
    emit splitterMoved(_splitRatio);
    update();
}
