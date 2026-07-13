// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: Splitter.cpp — Win32 Splitter WNDPROC → Qt6 QWidget event handlers
// Translates: HWND APIs, PAINTSTRUCT/FillRect, SetCapture/ReleaseCapture,
//              MoveWindow, SendMessage → signals, LOWORD/HIWORD → QPoint

#include "Splitter.h"
#include "SplitterContainer.h"
#include <QApplication>
#include <QEnterEvent>
#include "NppDarkMode.h"
#include <stdexcept>

// Static flag initializations (no-op in Qt; kept for binary compatibility)
bool Splitter::_isHorizontalRegistered     = false;
bool Splitter::_isVerticalRegistered        = false;
bool Splitter::_isHorizontalFixedRegistered = false;
bool Splitter::_isVerticalFixedRegistered   = false;

// =============================================================================
// Constants (lifted from original)
// =============================================================================
namespace {
constexpr int HEIGHT_MINIMAL = 15;
}

// =============================================================================
// Construction — mirrors original default constructor
// =============================================================================

Splitter::Splitter(QWidget* parent)
    : QWidget(parent)
{
    // Enable mouse tracking for cursor changes without requiring a click
    setMouseTracking(true);

    // Qt flag: accept focus so mouseDoubleClickEvent fires
    setFocusPolicy(Qt::StrongFocus);
}

// =============================================================================
// init — mirrors Win32 RegisterClassEx + CreateWindowEx
// =============================================================================

void Splitter::init(QWidget* hParent, int splitterSize, double splitRatio, unsigned int flags)
{
    if (!hParent)
        throw std::runtime_error("Splitter::init: parent widget is null");

    if (splitRatio < 0.0 || splitRatio >= 100.0)
        throw std::runtime_error("Splitter::init: splitRatio must be 0 < ratio < 100");

    _hParent = hParent;
    _splitterSize = splitterSize;
    _splitPercent = splitRatio;
    _flags = flags;

    if (flags & SV_FIXED)
        _isFixed = true;

    // Register with parent (Win32: setParent in CreateWindowEx)
    setParent(hParent);

    // Compute initial geometry from parent rect
    QRect parentRect = hParent->rect();

    if (flags & SV_HORIZONTAL) {
        // Horizontal splitter: divider runs left-to-right, splits top/bottom
        _rect.setTop(static_cast<int>((parentRect.height() * _splitPercent) / 100.0) - splitterSize / 2);
        _rect.setLeft(0);
        _rect.setBottom(splitterSize);
        _rect.setRight(parentRect.width());
    } else {
        // Vertical splitter: divider runs top-to-bottom, splits left/right
        _rect.setTop(0);
        _rect.setLeft(static_cast<int>((parentRect.width() * _splitPercent) / 100.0) - splitterSize / 2);
        _rect.setBottom(parentRect.height());
        _rect.setRight(splitterSize);
    }

    // Move and resize to initial position
    move(_rect.left(), _rect.top());
    resize(_rect.width(), _rect.height());

    // Set cursor based on orientation and fixed state
    if (_isFixed) {
        setCursor(Qt::ArrowCursor);
    } else {
        if (flags & SV_HORIZONTAL)
            setCursor(Qt::SizeVerCursor);
        else
            setCursor(Qt::SizeHorCursor);
    }

    // Enable double-click messages if needed
    if ((flags & SV_ENABLERDBLCLK) || (flags & SV_ENABLELDBLCLK))
        setAttribute(Qt::WA_MouseTracking);

    // Build click zones
    QRect rc = hParent->rect();
    int czW = getClickZone(WH::width);
    int czH = getClickZone(WH::height);

    _clickZone2TL.setLeft(rc.left());
    _clickZone2TL.setTop(rc.top());
    _clickZone2TL.setRight(czW);
    _clickZone2TL.setBottom(czH);

    _clickZone2BR.setLeft(rc.right() - czW);
    _clickZone2BR.setTop(rc.bottom() - czH);
    _clickZone2BR.setRight(czW);
    _clickZone2BR.setBottom(czH);

    show();

    // Notify parent of initial position (lifted from WM_RESIZE_CONTAINER)
    emit splitterRatioChanged(_splitPercent);
}

// =============================================================================
// destroy — mirrors ::DestroyWindow(_hSelf)
// =============================================================================

void Splitter::destroy()
{
    hide();
    deleteLater();
}

// =============================================================================
// getClickZone — determines hit-test zone size from splitter size
// =============================================================================

int Splitter::getClickZone(WH which) const
{
    if (_splitterSize <= 8) {
        return isVertical()
            ? (which == WH::width  ? _splitterSize  : HEIGHT_MINIMAL)
            : (which == WH::width  ? HEIGHT_MINIMAL : _splitterSize);
    } else {
        return isVertical()
            ? (which == WH::width  ? 8  : 15)
            : (which == WH::width  ? 15 : 8);
    }
}

// =============================================================================
// resizeSpliter — parent-size changed; recompute our rect and redraw
// Lifted from original resizeSpliter(RECT*)
// =============================================================================

void Splitter::resizeSpliter(QRect* pRect)
{
    QRect rect;
    if (pRect)
        rect = *pRect;
    else
        rect = _hParent ? _hParent->rect() : QRect{};

    if (_flags & SV_HORIZONTAL) {
        _rect.setRight(rect.width());

        if (_flags & SV_RESIZEWTHPERCNT)
            _rect.setTop(static_cast<int>((rect.height() * _splitPercent) / 100.0) - _splitterSize / 2);
        else
            _rect.setTop(getSplitterFixPosY());
    } else {
        _rect.setBottom(rect.height());

        if (_flags & SV_RESIZEWTHPERCNT)
            _rect.setLeft(static_cast<int>((rect.width() * _splitPercent) / 100.0) - _splitterSize / 2);
        else
            _rect.setLeft(getSplitterFixPosX());
    }

    move(_rect.left(), _rect.top());
    resize(_rect.width(), _rect.height());

    emit splitterRatioChanged(_splitPercent);

    // Update bottom-right click zone
    QRect rc = rect;
    _clickZone2BR.setRight(getClickZone(WH::width));
    _clickZone2BR.setBottom(getClickZone(WH::height));
    _clickZone2BR.setLeft(rc.right() - _clickZone2BR.width());
    _clickZone2BR.setTop(rc.bottom() - _clickZone2BR.height());

    update();  // force repaint
}

// =============================================================================
// gotoTopLeft — collapse splitter to top/left edge (SV_ENABLELDBLCLK)
// =============================================================================

void Splitter::gotoTopLeft()
{
    if ((_flags & SV_ENABLELDBLCLK) && !_isFixed && _splitPercent > 1) {
        if (_flags & SV_HORIZONTAL)
            _rect.setTop(1);
        else
            _rect.setLeft(1);

        _splitPercent = 1;

        move(_rect.left(), _rect.top());
        emit splitterRatioChanged(_splitPercent);
        update();
    }
}

// =============================================================================
// gotoRightBottom — collapse splitter to bottom/right edge (SV_ENABLERDBLCLK)
// =============================================================================

void Splitter::gotoRightBottom()
{
    if ((_flags & SV_ENABLERDBLCLK) && !_isFixed && _splitPercent < 99) {
        QRect rt = _hParent->rect();

        if (_flags & SV_HORIZONTAL)
            _rect.setTop(rt.bottom() - _splitterSize);
        else
            _rect.setLeft(rt.right() - _splitterSize);

        _splitPercent = 99;

        move(_rect.left(), _rect.top());
        emit splitterRatioChanged(_splitPercent);
        update();
    }
}

// =============================================================================
// rotate — switch between horizontal and vertical orientation
// =============================================================================

void Splitter::rotate()
{
    if (_isFixed)
        return;

    // Save current state before destroy
    double savedRatio = _splitPercent;
    int savedSize = _splitterSize;

    destroy();

    // Toggle flags
    if (_flags & SV_HORIZONTAL) {
        _flags &= ~SV_HORIZONTAL;
        _flags |= SV_VERTICAL;
    } else {
        _flags &= ~SV_VERTICAL;
        _flags |= SV_HORIZONTAL;
    }

    _splitterSize = savedSize;
    init(_hParent, _splitterSize, savedRatio, _flags);
}

// =============================================================================
// updateCursor — set cursor based on zone hit-test
// =============================================================================

void Splitter::updateCursor(const QPoint& pos)
{
    if (_isFixed) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (isInLeftTopZone(pos) || isInRightBottomZone(pos)) {
        setCursor(Qt::PointingHandCursor);
    } else if (_flags & SV_HORIZONTAL) {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::SizeHorCursor);
    }
}

// =============================================================================
// mousePressEvent — WM_LBUTTONDOWN + WM_RBUTTONDOWN
// =============================================================================

void Splitter::mousePressEvent(QMouseEvent* event)
{
    const QPoint p = event->position().toPoint();

    if (event->button() == Qt::LeftButton) {
        if (isInLeftTopZone(p) && _isLeftButtonDown) {
            gotoTopLeft();
            return;
        }
        if (isInRightBottomZone(p) && _isLeftButtonDown) {
            gotoRightBottom();
            return;
        }

        if (!_isFixed) {
            grabMouse();  // ::SetCapture equivalent
            _isDraged = true;
            _isLeftButtonDown = true;
            _dragStartPos = QCursor::pos();
            _dragStartRatio = _splitPercent;
            _dragStartTopLeft = isVertical() ? _rect.left() : _rect.top();
        }
    } else if (event->button() == Qt::RightButton) {
        // Lift WM_DOPOPUPMENU → signal
        emit popupMenuRequested(event);
    }

    QWidget::mousePressEvent(event);
}

// =============================================================================
// mouseMoveEvent — WM_MOUSEMOVE: cursor tracking + drag resizing
// =============================================================================

void Splitter::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint p = event->position().toPoint();
    const QPoint globalPt = event->globalPosition().toPoint();

    // Update cursor in zone
    if (isInLeftTopZone(p) || isInRightBottomZone(p)) {
        if (!_isInZone) {
            _isInZone = true;
            updateCursor(p);
        }
    } else {
        _isInZone = false;
        updateCursor(p);
    }

    // Drag-resize: WM_LBUTTON held and mouse captured
    if (!_isFixed && (event->buttons() & Qt::LeftButton) && _isLeftButtonDown) {
        QRect rt = _hParent->rect();

        // Map global cursor to parent-local coords
        QPoint pt = _hParent->mapFromGlobal(globalPt);

        if (_flags & SV_HORIZONTAL) {
            if (pt.y() <= 1) {
                _rect.setTop(1);
                _splitPercent = 1;
            } else if (pt.y() >= rt.height() - 5) {
                _rect.setTop(rt.height() - 5);
                _splitPercent = 99;
            } else {
                _rect.setTop(pt.y());
                _splitPercent = (pt.y() * 100.0 / rt.height());
            }
        } else {
            if (pt.x() <= 1) {
                _rect.setLeft(1);
                _splitPercent = 1;
            } else if (pt.x() >= rt.width() - 5) {
                _rect.setLeft(rt.width() - 5);
                _splitPercent = 99;
            } else {
                _rect.setLeft(pt.x());
                _splitPercent = (pt.x() * 100.0 / rt.width());
            }
        }

        move(_rect.left(), _rect.top());
        emit splitterRatioChanged(_splitPercent);
        update();
    }
}

// =============================================================================
// mouseDoubleClickEvent — WM_LBUTTONDBLCLK: reset to 50%
// =============================================================================

void Splitter::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    QRect rc = _hParent->rect();

    if (_flags & SV_HORIZONTAL)
        _rect.setTop((rc.height() - _splitterSize) / 2);
    else
        _rect.setLeft((rc.width() - _splitterSize) / 2);

    _splitPercent = 50.0;

    move(_rect.left(), _rect.top());
    emit splitterRatioChanged(_splitPercent);
    update();
}

// =============================================================================
// mouseReleaseEvent — WM_LBUTTONUP: release mouse capture
// =============================================================================

void Splitter::mouseReleaseEvent(QMouseEvent* event)
{
    if (!_isFixed && _isLeftButtonDown) {
        releaseMouse();  // ::ReleaseCapture equivalent
        _isLeftButtonDown = false;
    }
    QWidget::mouseReleaseEvent(event);
}

// =============================================================================
// paintEvent — WM_PAINT: custom splitter bar rendering
// Lifts: BeginPaint/EndPaint, FillRect, CreateSolidBrush, SelectObject, DPIManagerV2
// =============================================================================

void Splitter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QRect rc = rect();
    bool isDark = NppDarkMode::instance().isEnabled();

    // DPI scaling — maps DPIManagerV2::scale() to devicePixelRatio()
    // Use 96 DPI as baseline; scale proportionally
    const qreal dpiScale = devicePixelRatio();

    auto scale = [dpiScale](int v) -> int { return static_cast<int>(v * dpiScale); };

    const int s1 = scale(1);
    const int s2 = scale(2);
    const int s3 = scale(3);
    const int s4 = scale(4);

    // Choose brushes based on dark/light mode
    if (isDark) {
        painter.fillRect(rc, NppDarkMode::getDlgBackgroundBrush());
        painter.setPen(NppDarkMode::getDarkerTextColor());
    } else {
        painter.fillRect(rc, QBrush(QColor(0xFF, 0xFF, 0xFF)));
    }

    // Compute zone rects
    QRect rcToDraw1, rcToDraw2, TLrc, BRrc;

    if ((_splitterSize >= s4) && (_flags & SV_RESIZEWTHPERCNT)) {
        adjustZoneToDraw(TLrc, ZONE_TYPE::topLeft);
        adjustZoneToDraw(BRrc, ZONE_TYPE::bottomRight);
        drawArrow(painter, TLrc, isVertical() ? Arrow::left : Arrow::up);
    }

    if (isVertical()) {
        // Vertical splitter: stripes run top-to-bottom
        rcToDraw2.setTop((_flags & SV_RESIZEWTHPERCNT) ? _clickZone2TL.bottom() : 0);
        rcToDraw2.setBottom(rcToDraw2.top() + s2);
        rcToDraw1.setTop(rcToDraw2.top() + s1);
        rcToDraw1.setBottom(rcToDraw1.top() + s2);
    } else {
        // Horizontal splitter: stripes run left-to-right
        rcToDraw2.setTop(s1);
        rcToDraw2.setBottom(s3);
        rcToDraw1.setTop(s2);
        rcToDraw1.setBottom(s4);
    }

    // Determine painting extent
    int bottom = (_flags & SV_RESIZEWTHPERCNT)
        ? (isVertical() ? rc.bottom() - _clickZone2BR.bottom() : rc.bottom())
        : rc.bottom();

    // Primary fill brush
    QBrush primaryBrush = isDark
        ? NppDarkMode::getDlgBackgroundBrush()
        : QBrush(QColor(0xFF, 0xFF, 0xFF));
    // Secondary (shadow) brush — uses COLOR_3DSHADOW → Qt equivalent
    QBrush shadowBrush = isDark
        ? NppDarkMode::getDlgBackgroundBrush()
        : QBrush(palette().shadow().color());

    // Draw the striped pattern
    while (rcToDraw1.bottom() <= bottom) {
        if (isVertical()) {
            rcToDraw2.setLeft(s1);
            rcToDraw2.setRight(s3);
            rcToDraw1.setLeft(s2);
            rcToDraw1.setRight(s4);
        } else {
            rcToDraw2.setLeft(_clickZone2TL.right());
            rcToDraw2.setRight(rcToDraw2.left() + s2);
            rcToDraw1.setLeft(rcToDraw2.left());
            rcToDraw1.setRight(rcToDraw1.left() + s2);
        }

        while (rcToDraw1.right() <= (isVertical() ? rc.right() : rc.right() - _clickZone2BR.right())) {
            painter.fillRect(rcToDraw1, primaryBrush);
            painter.fillRect(rcToDraw2, shadowBrush);

            rcToDraw2.translate(s4, 0);
            rcToDraw1.translate(s4, 0);
        }

        rcToDraw2.translate(0, s4);
        rcToDraw2.setBottom(rcToDraw2.bottom() + s4);
        rcToDraw1.translate(0, s4);
        rcToDraw1.setBottom(rcToDraw1.bottom() + s4);
    }

    // Draw collapse arrows at edges
    if ((_splitterSize >= s4) && (_flags & SV_RESIZEWTHPERCNT))
        drawArrow(painter, BRrc, isVertical() ? Arrow::right : Arrow::down);
}

// =============================================================================
// drawArrow — draws a diagonal arrow using painter lines
// Lifts: ::MoveToEx / ::LineTo → QPainter::drawLine
// =============================================================================

void Splitter::drawArrow(QPainter& painter, const QRect& rect, Arrow direction)
{
    QRect rc = rect;
    painter.save();
    painter.setPen(painter.pen());

    switch (direction) {
    case Arrow::left: {
        int x = rc.right();
        int y = rc.top();
        while (x > rc.left() && y != rc.bottom()) {
            painter.drawLine(QPoint(x, y), QPoint(x, rc.bottom()));
            --x;
            ++y;
        }
        break;
    }
    case Arrow::right: {
        int x = rc.left();
        int y = rc.top();
        while (x < rc.right() && y != rc.bottom()) {
            painter.drawLine(QPoint(x, y), QPoint(x, rc.bottom()));
            ++x;
            ++y;
        }
        break;
    }
    case Arrow::up: {
        int x = rc.left();
        int y = rc.bottom();
        while (y > rc.top() && x != rc.right()) {
            painter.drawLine(QPoint(x, y), QPoint(rc.right(), y));
            ++x;
            --y;
        }
        break;
    }
    case Arrow::down: {
        int x = rc.left();
        int y = rc.top();
        while (y < rc.bottom() && x != rc.right()) {
            painter.drawLine(QPoint(x, y), QPoint(rc.right(), y));
            ++x;
            ++y;
        }
        break;
    }
    }

    painter.restore();
}

// =============================================================================
// adjustZoneToDraw — computes arrow hit-test zone geometry
// =============================================================================

void Splitter::adjustZoneToDraw(QRect& rc2def, ZONE_TYPE whichZone) const
{
    if (_splitterSize < 4)
        return;

    int w, h;
    if (_splitterSize <= 8) {
        w = isVertical() ? 4  : 7;
        h = isVertical() ? 7  : 4;
    } else {
        w = isVertical() ? 6  : 11;
        h = isVertical() ? 11 : 6;
    }

    int x0 = 0, y0 = 0;

    if (isVertical()) {
        if (whichZone == ZONE_TYPE::topLeft) {
            x0 = 0;
            y0 = (_clickZone2TL.bottom() - h) / 2;
        } else {
            x0 = _clickZone2BR.left() + _clickZone2BR.width() - w;
            y0 = (_clickZone2BR.bottom() - h) / 2 + _clickZone2BR.top();
        }
    } else {
        if (whichZone == ZONE_TYPE::topLeft) {
            x0 = (_clickZone2TL.right() - w) / 2;
            y0 = 0;
        } else {
            x0 = ((_clickZone2BR.right() - w) / 2) + _clickZone2BR.left();
            y0 = _clickZone2BR.top() + _clickZone2BR.bottom() - h;
        }
    }

    rc2def.setRect(x0, y0, w, h);
}

// =============================================================================
// isInLeftTopZone / isInRightBottomZone — zone hit-test
// =============================================================================

bool Splitter::isInLeftTopZone(const QPoint& p) const
{
    return (p.x() >= _clickZone2TL.left()
         && p.x() <= _clickZone2TL.left() + _clickZone2TL.width()
         && p.y() >= _clickZone2TL.top()
         && p.y() <= _clickZone2TL.top() + _clickZone2TL.height());
}

bool Splitter::isInRightBottomZone(const QPoint& p) const
{
    return (p.x() >= _clickZone2BR.left()
         && p.x() <= _clickZone2BR.left() + _clickZone2BR.width()
         && p.y() >= _clickZone2BR.top()
         && p.y() <= _clickZone2BR.top() + _clickZone2BR.height());
}

// =============================================================================
// getSplitterFixPosX / getSplitterFixPosY
// Lifts: ::SendMessage(_hParent, WM_GETSPLITTER_X/Y) → parent query
// Uses a qobject_cast to call directly into SplitterContainer if available,
// otherwise emits a signal for loosely-coupled parents.
// =============================================================================

int Splitter::getSplitterFixPosX() const
{
    // Stub: return half the parent width when fixed position is needed
    return _hParent ? _hParent->size().width() / 2 : 0;
}

int Splitter::getSplitterFixPosY() const
{
    // Stub: return half the parent height when fixed position is needed
    return _hParent ? _hParent->size().height() / 2 : 0;
}

void Splitter::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
}

void Splitter::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
}
