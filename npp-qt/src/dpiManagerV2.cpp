// Semantic Lift: dpiManagerV2 — Win32 DPI awareness → Qt6 QApplication::setAttribute
// Original: PowerEditor/src/dpiManagerV2.cpp (248 lines)
// Win32 → Qt6: SetProcessDPIAware + WM_DPICHANGED → Qt::AA_EnableHighDpiScaling + QScreen::logicalDpiY

#include "dpiManagerV2.h"
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QWidget>

DpiManagerV2::DpiManagerV2()
    : _dpi(96), _dpiOriginal(96), _zoomFactors(100)
{
    init();
}

void DpiManagerV2::init()
{
    // Qt6 handles DPI automatically via Qt::AA_EnableHighDpiScaling
    // This class manages custom zoom factors beyond system DPI

    // Get primary screen DPI
    if (QApplication::primaryScreen()) {
        int dpiY = QApplication::primaryScreen()->logicalDpiY();
        _dpiOriginal = dpiY;
        _dpi = dpiY;
        _zoomFactors = (dpiY * 100) / 96; // 100% = 96 DPI
    }
}

void DpiManagerV2::updateDpi()
{
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        _dpi = screen->logicalDpiY();
        _zoomFactors = (_dpi * 100) / 96;
    }
}

void DpiManagerV2::notifyAllControls(HWNDWrapper)
{
    // In Qt6, DPI changes are handled automatically via event filters
    // This method is kept for API compatibility
    QApplication::processEvents();
}

int DpiManagerV2::getDpi() const
{
    return _dpi;
}

int DpiManagerV2::getOriginalDpi() const
{
    return _dpiOriginal;
}

int DpiManagerV2::getZoomFactor() const
{
    return _zoomFactors;
}

void DpiManagerV2::setZoomFactor(int zoomFactor)
{
    _zoomFactors = zoomFactor;
    _dpi = (_dpiOriginal * zoomFactor) / 100;
}

float DpiManagerV2::getScale() const
{
    return static_cast<float>(_zoomFactors) / 100.0f;
}

// Scale a value based on current DPI
int DpiManagerV2::scale(int value) const
{
    return (value * _zoomFactors) / 100;
}

// Scale a value based on specific zoom
int DpiManagerV2::scaleByZoom(int value, int zoom) const
{
    return (value * zoom) / 100;
}

QPoint DpiManagerV2::scalePoint(const QPoint& pt) const
{
    float scale = getScale();
    return QPoint(static_cast<int>(pt.x() * scale),
                  static_cast<int>(pt.y() * scale));
}

QSize DpiManagerV2::scaleSize(const QSize& sz) const
{
    float scale = getScale();
    return QSize(static_cast<int>(sz.width() * scale),
                 static_cast<int>(sz.height() * scale));
}

void DpiManagerV2::reInit()
{
    init();
}
