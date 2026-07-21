// Window.h - Win32 Window base class stub for Qt6
// INTENT: source is the Win32 Window class (HWND wrapper). In Qt6, QWidget handles this.
// This file exists only for include compatibility.
#pragma once
#include <QWidget>

// Window was the base MFC-like class in NPP for all UI elements.
// In Qt6, QWidget provides all the same functionality.
// This is a compatibility shim that maps Window methods to QWidget.
class Window {
public:
    virtual ~Window() = default;
    virtual void init(QWidget* parent) { _hSelf = parent; }
    virtual void display(bool show = true) { if (_hSelf) show ? _hSelf->show() : _hSelf->hide(); }
    virtual void destroy() {}

    QWidget* getHSelf() const { return _hSelf; }

protected:
    QWidget* _hSelf = nullptr;
};
