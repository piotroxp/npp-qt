// Semantic Lift: SizeableDlg — Win32 resizable dialog → Qt6 QDialog
// Original: PowerEditor/src/WinControls/WindowsDlg/SizeableDlg.cpp (78 lines)

#include "SizeableDlg.h"
#include <QResizeEvent>

SizeableDlg::SizeableDlg(QWidget* parent)
    : QDialog(parent)
{
    setSizeGripEnabled(true);
}

SizeableDlg::~SizeableDlg() {}

void SizeableDlg::setMinSize(int cx, int cy)
{
    _minCX = cx;
    _minCY = cy;
    setMinimumSize(cx, cy);
}

void SizeableDlg::setMaxSize(int cx, int cy)
{
    _maxCX = cx;
    _maxCY = cy;
    setMaximumSize(cx, cy);
}

void SizeableDlg::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    // Constrain to min/max
    int w = qBound(_minCX, event->size().width(), _maxCX);
    int h = qBound(_minCY, event->size().height(), _maxCY);
    if (w != event->size().width() || h != event->size().height())
        resize(w, h);
}
