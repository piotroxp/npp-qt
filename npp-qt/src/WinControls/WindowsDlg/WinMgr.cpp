// Semantic Lift: WinMgr — Win32 window rect management → Qt6 QLayout + QRect
// Original: PowerEditor/src/WinControls/WindowsDlg/WinMgr.cpp (496 lines)

#include "WinMgr.h"
#include <QWidget>

WinMgr::WinMgr() {}
WinMgr::~WinMgr() {}

void WinMgr::setWndRect(QWidget* wnd, const QRect& rect)
{
    if (_locked || !wnd)
        return;
    _wndRects[wnd] = rect;
    wnd->setGeometry(rect);
}

QRect WinMgr::getWndRect(QWidget* wnd) const
{
    if (!wnd)
        return QRect();
    return _wndRects.value(wnd, wnd->geometry());
}

void WinMgr::removeWnd(QWidget* wnd)
{
    _wndRects.remove(wnd);
}

void WinMgr::saveLayout()
{
    // Layout is automatically tracked via _wndRects
}

void WinMgr::restoreLayout()
{
    for (auto it = _wndRects.begin(); it != _wndRects.end(); ++it) {
        QWidget* wnd = it.key();
        if (wnd) {
            wnd->setGeometry(it.value());
        }
    }
}

void WinMgr::reset()
{
    _wndRects.clear();
}
