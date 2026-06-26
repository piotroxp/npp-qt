// Semantic Lift: Win32 DoubleBuffer → Qt6 DoubleBufferHelper
// Original: PowerEditor/src/WinControls/DoubleBuffer/DoubleBuffer.cpp
// Target: npp-qt/src/WinControls/DoubleBuffer.cpp

#include "DoubleBuffer.h"
#include <QPainter>
#include <QPaintDevice>
#include <QCoreApplication>

DoubleBufferHelper::DoubleBufferHelper(QWidget* widget)
    : QObject(widget)
    , _widget(widget)
{
    if (_widget) {
        _widget->setAttribute(Qt::WA_PaintOnScreen, false);
        _widget->installEventFilter(this);
    }
}

DoubleBufferHelper::~DoubleBufferHelper()
{
    if (_widget) {
        _widget->removeEventFilter(this);
    }
    delete _bufferPainter;
}

void DoubleBufferHelper::ensureBuffer(const QSize& requiredSize)
{
    if (_buffer.isNull() || _buffer.size() != requiredSize) {
        _buffer = QPixmap(requiredSize);
        _buffer.fill(Qt::transparent);
        _bufferSize = requiredSize;
        _bufferValid = true;
    } else if (!_bufferValid) {
        _buffer.fill(Qt::transparent);
        _bufferValid = true;
    }
}

QPainter* DoubleBufferHelper::beginDoubleBuffer()
{
    if (!_widget) {
        return nullptr;
    }

    delete _bufferPainter;
    _bufferPainter = nullptr;
    _isBuffered = false;

    const QSize requiredSize = _widget->size();
    ensureBuffer(requiredSize);

    _bufferPainter = new QPainter(&_buffer);
    _bufferPainter->setClipping(true);
    return _bufferPainter;
}

QPainter* DoubleBufferHelper::endDoubleBuffer()
{
    if (!_widget || !_bufferPainter) {
        return nullptr;
    }

    if (!_bufferPainter->isActive()) {
        delete _bufferPainter;
        _bufferPainter = nullptr;
        return nullptr;
    }

    _bufferPainter->end();
    delete _bufferPainter;
    _bufferPainter = nullptr;

    // Blit to screen — replicate BitBlt(hPaintDC, rcPaint, _hMemoryDC, rcPaint, SRCCOPY)
    QPainter screenPainter(_widget);
    if (_paintRegion.isEmpty()) {
        screenPainter.drawPixmap(0, 0, _buffer);
    } else {
        // Only blit the painted region
        const QRect clipRect = _paintRegion.boundingRect();
        screenPainter.drawPixmap(clipRect.topLeft(), _buffer, clipRect);
    }

    _isBuffered = true;
    _paintRegion = QRegion();
    return nullptr; // painter is done
}

bool DoubleBufferHelper::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != _widget) {
        return QObject::eventFilter(watched, event);
    }

    if (event->type() == QEvent::Paint) {
        // Store the paint region for the blit step
        auto* pe = static_cast<QPaintEvent*>(event);
        _paintRegion = pe->region();
    }

    return QObject::eventFilter(watched, event);
}

DoubleBufferHelper* subclassWidgetForDoubleBuffer(QWidget* widget)
{
    if (!widget) {
        return nullptr;
    }
    // Ownership: widget takes ownership via Qt::WA_DeleteOnClose or manual delete
    auto* helper = new DoubleBufferHelper(widget);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);
    return helper;
}


