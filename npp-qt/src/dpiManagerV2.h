#pragma once

#include <QPoint>
#include <QSize>
#include <QString>

class DpiManagerV2
{
public:
    DpiManagerV2();
    void init();
    void updateDpi();
    void notifyAllControls(void*);

    int getDpi() const;
    int getOriginalDpi() const;
    int getZoomFactor() const;
    void setZoomFactor(int zoomFactor);
    float getScale() const;

    // Scaling helpers
    int scale(int value) const;
    int scaleByZoom(int value, int zoom) const;
    QPoint scalePoint(const QPoint& pt) const;
    QSize scaleSize(const QSize& sz) const;
    void reInit();

private:
    int _dpi;
    int _dpiOriginal;
    int _zoomFactors;
};

// Compatibility typedef
using HWNDWrapper = void*;

