// DoubleBuffer.h — Qt6 translation
// Note: Qt handles double-buffering automatically via QWidget double-buffer
#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

// In Qt6, double-buffering is automatic via QWidget::setAttribute(Qt::WA_PaintOnScreen)
// For custom double-buffering, subclass QWidget and override paintEvent()
// Use QBackingStore or QImage for manual double-buffering when needed

class DoubleBufferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleBufferWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // Qt handles double-buffering automatically
        setAttribute(Qt::WA_PaintOnScreen, false);
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter p(this);
        // Draw with double-buffering - Qt handles this automatically
        p.fillRect(event->rect(), palette().window());
    }
};

// Utility to ensure double-buffering is disabled for specific widgets
inline void enableDoubleBuffer(QWidget* w)
{
    w->setAttribute(Qt::WA_PaintOnScreen, false);
    w->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

inline void disableDoubleBuffer(QWidget* w)
{
    w->setAttribute(Qt::WA_PaintOnScreen, true);
    w->setAttribute(Qt::WA_OpaquePaintEvent, true);
}
