// Semantic Lift: Win32 SplitterContainer → Qt6 implementation
// Original: PowerEditor/src/WinControls/SplitterContainer/
// Target: npp-qt/src/WinControls/SplitterContainer.cpp

#include "SplitterContainer.h"
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>

// =============================================================================
// Splitter
// =============================================================================

Splitter::Splitter(Qt::Orientation orient, QWidget* parent)
    : QWidget(parent)
    , _orientation(orient)
{
    setMinimumSize(4, 4);
    if (orient == Qt::Vertical) {
        setMaximumHeight(4);
        setCursor(Qt::SizeVerCursor);
    } else {
        setMaximumWidth(4);
        setCursor(Qt::SizeHorCursor);
    }
    setMouseTracking(true);
}

void Splitter::mousePressEvent(QMouseEvent* event) {
    if (!_draggable || _isFixed) return;
    if (event->button() == Qt::LeftButton) {
        _isDragging = true;
        _dragStartPos = event->globalPos();
        _dragStartRatio = _splitRatio;
        update();
    }
}

void Splitter::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    _isDragging = false;
    update();
}

void Splitter::mouseMoveEvent(QMouseEvent* event) {
    if (!_draggable || _isFixed) return;
    if (_isDragging) {
        QWidget* parent = parentWidget();
        if (!parent) return;

        QRect parentRect = parent->rect();
        double delta = 0.0;

        if (_orientation == Qt::Horizontal) {
            int dx = event->globalPos().x() - _dragStartPos.x();
            delta = static_cast<double>(dx) / parentRect.width();
        } else {
            int dy = event->globalPos().y() - _dragStartPos.y();
            delta = static_cast<double>(dy) / parentRect.height();
        }

        double newRatio = _dragStartRatio + delta;
        newRatio = qBound(0.05, newRatio, 0.95);
        _splitRatio = newRatio;
        emit splitterMoved(newRatio);
    }
}

void Splitter::mouseDoubleClickEvent(QMouseEvent* event) {
    if ((_flags & SV_ENABLELDBLCLK) && event->button() == Qt::LeftButton) {
        emit doubleClicked(Qt::LeftButton);
    } else if ((_flags & SV_ENABLERDBLCLK) && event->button() == Qt::RightButton) {
        emit doubleClicked(Qt::RightButton);
    }
}

void Splitter::enterEvent(QEnterEvent* event) {
    Q_UNUSED(event);
    if (_draggable && !_isFixed) {
        setCursor(_orientation == Qt::Vertical ? Qt::SizeVerCursor : Qt::SizeHorCursor);
    }
}

void Splitter::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    if (!_isDragging) {
        unsetCursor();
    }
}

void Splitter::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    drawSplitter(painter);
}

void Splitter::drawSplitter(QPainter& painter) {
    QColor highlight = palette().color(QPalette::Normal, QPalette::Midlight);
    QColor shadow = palette().color(QPalette::Normal, QPalette::Dark);

    if (_isDragging) {
        painter.fillRect(rect(), highlight);
    } else {
        // 3D border effect
        painter.fillRect(rect(), palette().color(QPalette::Normal, QPalette::Button));
    }

    // Draw grip dots
    QRect r = rect();
    painter.setPen(shadow);
    if (_orientation == Qt::Horizontal) {
        int cx = r.center().x();
        for (int y = r.top() + 3; y < r.bottom() - 2; y += 4) {
            painter.drawPoint(cx, y);
        }
    } else {
        int cy = r.center().y();
        for (int x = r.left() + 3; x < r.right() - 2; x += 4) {
            painter.drawPoint(x, cy);
        }
    }
}

void Splitter::drawArrow(QPainter& painter, const QRect& rect, Arrow direction) {
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    Q_UNUSED(direction);
    // Arrow drawing for future expansion
}

// =============================================================================
// SplitterContainer
// =============================================================================

SplitterContainer::SplitterContainer(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

void SplitterContainer::create(QWidget* pWin0, QWidget* pWin1, int splitterSize,
                                SplitterMode mode, int ratio, bool isVertical) {
    _pWin0 = pWin0;
    _pWin1 = pWin1;
    _splitterSize = splitterSize;
    _splitterMode = mode;
    _ratio = ratio / 100.0;

    // Remove old splitter if any
    if (_splitter) {
        layout()->removeWidget(_splitter);
        delete _splitter;
    }

    // Create new splitter
    _splitter = new QSplitter(isVertical ? Qt::Vertical : Qt::Horizontal, this);
    _splitter->setChildrenCollapsible(false);
    _splitter->setHandleWidth(splitterSize);

    if (pWin0) _splitter->addWidget(pWin0);
    if (pWin1) _splitter->addWidget(pWin1);

    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 1);

    layout()->addWidget(_splitter);

    // Set initial ratio
    QList<int> sizes;
    int total = isVertical ? height() : width();
    if (total <= 0) total = 500;
    int s0 = static_cast<int>(total * _ratio);
    int s1 = total - s0;
    sizes << s0 << s1;
    _splitter->setSizes(sizes);

    connect(_splitter, &QSplitter::splitterMoved, this, &SplitterContainer::onSplitterMoved);
}

void SplitterContainer::setSplitterSize(int size) {
    _splitterSize = size;
    if (_splitter) {
        _splitter->setHandleWidth(size);
    }
}

void SplitterContainer::setRatio(double ratio) {
    _ratio = qBound(0.05, ratio, 0.95);
    if (_splitter) {
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2) {
            int total = sizes[0] + sizes[1];
            if (total > 0) {
                sizes[0] = static_cast<int>(total * _ratio);
                sizes[1] = total - sizes[0];
                _splitter->setSizes(sizes);
            }
        }
    }
}

double SplitterContainer::ratio() const {
    if (_splitter) {
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2 && (sizes[0] + sizes[1]) > 0) {
            return static_cast<double>(sizes[0]) / (sizes[0] + sizes[1]);
        }
    }
    return _ratio;
}

void SplitterContainer::show() {
    QWidget::show();
    if (_splitter) _splitter->show();
}

void SplitterContainer::hide() {
    if (_pWin0) _pWin0->hide();
    if (_pWin1) _pWin1->hide();
    if (_splitter) _splitter->hide();
    QWidget::hide();
}

void SplitterContainer::rotate() {
    if (!_splitter) return;
    Qt::Orientation newOrient = (_splitter->orientation() == Qt::Vertical) ? Qt::Horizontal : Qt::Vertical;
    _splitter->setOrientation(newOrient);
    relayout();
}

void SplitterContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayout();
}

void SplitterContainer::relayout() {
    // QSplitter handles its own layout, this is for future custom handling
}

void SplitterContainer::onSplitterMoved(double newRatio) {
    _ratio = newRatio;
    emit splitterMoved(newRatio);
}
