// Semantic Lift: Win32 DoubleBuffer → Qt6 DoubleBufferHelper
// Original: PowerEditor/src/WinControls/DoubleBuffer/DoubleBuffer.cpp
// Target: npp-qt/src/WinControls/DoubleBuffer.h + .cpp

#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QRegion>

// DoubleBufferHelper provides automatic double-buffering for any QWidget.
// In Qt, double-buffering is the default, but this helper can be used to
// implement explicit offscreen buffering for custom paint handling or
// to replicate the Win32 BitBlt semantics where needed.
//
// Usage: subclass and override paintEvent(), then call beginDoubleBuffer()
// at the start and endDoubleBuffer() at the end. Or use the simpler
// subclassWidget() static helper to auto-subclass any QWidget.

class DoubleBufferHelper : public QObject
{
    Q_OBJECT

public:
    explicit DoubleBufferHelper(QWidget* widget);
    ~DoubleBufferHelper() override;

    // Call this to perform buffered painting.
    // Clears the buffer and returns a QPainter on it.
    // After painting, call endDoubleBuffer() to blit to screen.
    QPainter* beginDoubleBuffer();

    // Blits the offscreen buffer to the widget and returns the painter.
    QPainter* endDoubleBuffer();

    // Returns true if the last paint was fully buffered.
    bool isBuffered() const { return _isBuffered; }

    // Invalidate the buffer so the next paint rebuilds it.
    void invalidateBuffer() { _bufferValid = false; }

private:
    QWidget* _widget = nullptr;
    QPixmap _buffer;
    QPainter* _bufferPainter = nullptr;
    bool _isBuffered = false;
    bool _bufferValid = false;
    QRegion _paintRegion;
    QSize _bufferSize;

    void ensureBuffer(const QSize& requiredSize);
    bool eventFilter(QObject* watched, QEvent* event) override;
};

// Helper: install a DoubleBufferHelper on any QWidget.
// The helper is owned by the widget (deleted via deleteLater on cleanup).
DoubleBufferHelper* subclassWidgetForDoubleBuffer(QWidget* widget);
