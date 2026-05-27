// Widget.h - Qt6 translation: base widget wrapper (no Q_OBJECT - not supported in templates)
#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

// Widget template - base for WinForms-like controls
// Note: No Q_OBJECT - MOC doesn't support templated classes with Q_OBJECT
// For signals/slots in derived classes, add Q_OBJECT in those classes directly
template<typename BaseWidget>
class Widget : public BaseWidget
{
public:
    template<typename... Args>
    Widget(Args... args) : BaseWidget(args...) {}

    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.fillRect(event->rect(), BaseWidget::palette().window());
        BaseWidget::paintEvent(event);
    }
};