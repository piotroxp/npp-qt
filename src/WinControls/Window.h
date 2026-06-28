// Semantic Lift: Win32 Window class → Qt6 QWidget base
// Original: PowerEditor/src/WinControls/Window.h
// Target: npp-qt/src/WinControls/Window.h

#ifndef WINDOW_H
#define WINDOW_H

// Window class definition
#define WINDOW_H_INCLUDED

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QSize>
#include <string>

// WideCharToMultiByte stub
inline int WideCharToMultiByte(unsigned int CodePage, unsigned long dwFlags, const wchar_t* lpWideCharStr, int cchWideChar, char* lpMultiByteStr, int cbMultiByte, const char* lpDefaultChar, int* lpUsedDefaultChar) {
    std::wstring ws(lpWideCharStr ? lpWideCharStr : L"");
    std::string s = QString::fromWCharArray(lpWideCharStr).toUtf8().constData();
    if (lpMultiByteStr && cbMultiByte > 0) { strncpy(lpMultiByteStr, s.c_str(), cbMultiByte - 1); lpMultiByteStr[cbMultiByte - 1] = 0; }
    return static_cast<int>(s.size());
}

// IWindow interface - pure virtual base for Window-like objects
// Does NOT inherit from QWidget to avoid diamond inheritance with classes
// that already inherit from QWidget (TabBarPlus -> QTabWidget)
class Window
{
public:
    virtual ~Window() = default;

    // Core Window interface
    virtual QWidget* getHSelf() = 0;
    virtual void display(bool show = true) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual int getHeight() const = 0;
    virtual QRect getClientRect() const = 0;
    virtual void destroy() = 0;
    virtual void init(void* hInst, QWidget* hParent) = 0;
    virtual void redraw(bool forceUpdate = false) = 0;

    // Static factory
    static Window* fromWidget(QWidget* w) { return qobject_cast<Window*>(w); }
};

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
    QWidget* getHSelf() override { return nullptr; }

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
    void* _hInst = nullptr;
    QWidget* _hParent = nullptr;
    QWidget* _hSelf = nullptr;
};

// Type aliases for Win32 → Qt translation layer
using HWND_ = QWidget*;
using HINSTANCE_ = QApplication*;
#endif // WINDOW_H
