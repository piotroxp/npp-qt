// Semantic Lift: Win32 Window class → Qt6 QWidget base
// Original: PowerEditor/src/WinControls/Window.h
// Target: npp-qt/src/WinControls/Window.h

#pragma once

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QSize>

// =============================================================================
// IWindow — abstract interface matching Window's Win32 API surface
// Used by callers that need the Window protocol without coupling to QWidget.
// =============================================================================
class IWindow
{
public:
    virtual ~IWindow() = default;

    virtual QWidget* getHSelf() = 0;
    virtual void display(bool show = true) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual int getHeight() const = 0;
    virtual QRect getClientRect() const = 0;
    virtual void destroy() = 0;
    virtual void init(void* hInst, QWidget* hParent) = 0;
    virtual void redraw(bool forceUpdate = false) = 0;
    virtual int getWidth() const = 0;
    virtual bool isVisible() const = 0;
    virtual QWidget* getHParent() const = 0;

    // Static factory — for any QWidget* that also implements IWindow
    static IWindow* fromWidget(QWidget* w) {
        return dynamic_cast<IWindow*>(w);
    }
};

// =============================================================================
// WindowBase — concrete QWidget base class that implements IWindow
// All Qt widgets in npp-qt that expose the Window API should inherit from this.
// =============================================================================
class WindowBase : public QWidget, virtual public IWindow
{
    Q_OBJECT

public:
    using QWidget::QWidget;  // inherit constructors

    QWidget* getHSelf() override { return this; }
    void display(bool toShow = true) override { toShow ? show() : hide(); }
    void show() override { QWidget::show(); }
    void hide() override { QWidget::hide(); }
    int getHeight() const override { return rect().height(); }
    int getWidth() const override { return rect().width(); }

    QRect getClientRect() const override {
        return rect();
    }

    QRect getWindowRect() const {
        return geometry();
    }

    void destroy() override { deleteLater(); }

    void init(void* /*hInst*/, QWidget* hParent) override {
        if (hParent) setParent(hParent);
    }

    void redraw(bool forceUpdate = false) override {
        update();
        if (forceUpdate) repaint();
    }

    bool isVisible() const override { return QWidget::isVisible(); }

    QWidget* getHParent() const { return parentWidget(); }

    void grabFocus() { setFocus(); }

    // Convenience: reSizeTo maps RECT → QWidget resize + move
    void reSizeTo(QRect& rc) {
        resize(rc.width(), rc.height());
        move(rc.left(), rc.top());
        update();
    }

    void reSizeToWH(QRect& rc) {
        resize(rc.width(), rc.height());
        update();
    }

protected:
    void* _hInst = nullptr;
    QWidget* _hParent = nullptr;
};

// Backwards-compat alias — code references "Window" as a concrete base
using Window = WindowBase;

// Win32 type aliases for compatibility layer (renamed to avoid conflict with NppConstants.h void* versions)
// Use NPP_HWND/QWidget* and NPP_HINSTANCE/QApplication* for Qt-layer window handles.
// The void* HWND/HINSTANCE from NppConstants.h are used for Win32 API stubs.
using NPP_HWND_  = QWidget*;
using NPP_HWND   = QWidget*;
using NPP_HINSTANCE_ = QApplication*;
using NPP_HINSTANCE  = QApplication*;
