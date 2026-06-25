// Semantic Lift: Win32 Window class → Qt6 QWidget base
// Original: PowerEditor/src/WinControls/Window.h
// Target: npp-qt/src/WinControls/Window.h

#pragma once

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QSize>

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr) : QWidget(parent) {}

    virtual ~Window() = default;

    // Semantic lift: init() maps to constructor + setWindowFlags
    virtual void init(QApplication* app, QWidget* parent) {
        _hInst = reinterpret_cast<HINSTANCE>(app);
        _hParent = parent;
        if (parent) {
            setParent(parent);
        }
    }

    // Semantic lift: destroy() → destructor cleanup
    virtual void destroy() {
        deleteLater();
    }

    // Semantic lift: display(bool) → show/hide
    virtual void display(bool toShow = true) {
        if (toShow) {
            show();
        } else {
            hide();
        }
    }

    // Semantic lift: reSizeTo(RECT&) → resize()
    virtual void reSizeTo(QRect& rc) {
        resize(rc.width(), rc.height());
        move(rc.left(), rc.top());
        update();
    }

    // Semantic lift: reSizeToWH(RECT&) → resize(w, h)
    virtual void reSizeToWH(QRect& rc) {
        resize(rc.width(), rc.height());
        update();
    }

    // Semantic lift: redraw() → update()
    virtual void redraw(bool forceUpdate = false) {
        update();
        if (forceUpdate) {
            repaint();
        }
    }

    // Semantic lift: getClientRect() → rect()
    virtual QRect getClientRect() const {
        return rect();
    }

    // Semantic lift: getWindowRect() → QWidget::geometry()
    virtual QRect getWindowRect() const {
        return geometry();
    }

    // Semantic lift: getWidth() → width()
    virtual int getWidth() const {
        return rect().width();
    }

    // Semantic lift: getHeight() → height()
    virtual int getHeight() const {
        if (isVisible()) {
            return rect().height();
        }
        return 0;
    }

    virtual bool isVisible() const {
        return QWidget::isVisible();
    }

    // Semantic lift: getHSelf() → this QWidget*
    QWidget* getHSelf() {
        return this;
    }

    // Semantic lift: getHParent() → parentWidget()
    QWidget* getHParent() const {
        return parentWidget();
    }

    // Semantic lift: grabFocus() → setFocus()
    void grabFocus() {
        setFocus();
    }

    // Semantic lift: getHinst() → qApp
    QApplication* getHinst() const {
        return qApp;
    }

    // Win32 handle types for compatibility
    using HINSTANCE = QApplication*;
    using HWND = QWidget*;

protected:
    HINSTANCE _hInst = nullptr;
    QWidget* _hParent = nullptr;
    QWidget* _hSelf = nullptr;
};

// Type aliases for Win32 → Qt translation layer
using HWND_ = QWidget*;
using HINSTANCE_ = QApplication*;
