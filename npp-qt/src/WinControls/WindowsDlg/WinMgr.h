#pragma once

#include <QRect>
#include <QSize>
#include <QWidget>
#include <QMap>

class WinMgr
{
public:
    WinMgr();
    ~WinMgr();

    // Window rectangle management
    void setWndRect(QWidget* wnd, const QRect& rect);
    QRect getWndRect(QWidget* wnd) const;
    void removeWnd(QWidget* wnd);

    // Layout
    void saveLayout();
    void restoreLayout();
    void reset();

    // Lock/unlock
    void lock() { _locked = true; }
    void unlock() { _locked = false; }

private:
    QMap<QWidget*, QRect> _wndRects;
    bool _locked = false;
};

