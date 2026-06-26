// Ported to Qt6: DPI-aware font and scaling utilities
#pragma once
#include <QtCore>
#include <QFont>
#include <QWidget>
#include <QApplication>
#include <QScreen>

class DPIManagerV2
{
public:
    DPIManagerV2() { }
    virtual ~DPIManagerV2() = default;

    static qreal getScreenDpi(QWidget* widget = nullptr);
    static qreal getScaleFactor(QWidget* widget = nullptr);
    static int scale(int x, QWidget* widget = nullptr);
    static int unscale(int x, QWidget* widget = nullptr);
    static QFont scaleFont(const QString& fontName, int pointSize, QWidget* widget = nullptr);
    static int scaleFontPointSize(int pointSize, QWidget* widget = nullptr);
    static int scaleFontForFactor(int pt, unsigned int textScaleFactor = 100);
    static void applyFontScaling(QFont& font, QWidget* widget = nullptr);
    static int getTextScaleFactor();
    void setDpiWithScreen(QWidget* widget);
    void setDpi(int dpi) { _dpi = dpi; }
    int getDpi() const { return _dpi; }
    int scale(int x) const { return scale(x, nullptr); }
    int unscale(int x) const { return unscale(x, nullptr); }
    int scaleFont(int pt) const { return scaleFontForFactor(pt); }
    static int getDpiForWindow(QWidget* w) { return w ? w->logicalDpiY() : 96; }

private:
    int _dpi = 96;
};

inline qreal DPIManagerV2::getScreenDpi(QWidget* widget)
{
    if (widget)
        return widget->devicePixelRatio() * 96.0;
    if (QApplication::primaryScreen())
        return QApplication::primaryScreen()->logicalDotsPerInchX();
    return 96.0;
}

inline qreal DPIManagerV2::getScaleFactor(QWidget* widget)
{
    return getScreenDpi(widget) / 96.0;
}

inline int DPIManagerV2::scale(int x, QWidget* widget)
{
    qreal factor = getScaleFactor(widget);
    return qRound(x * factor);
}

inline int DPIManagerV2::unscale(int x, QWidget* widget)
{
    qreal factor = getScaleFactor(widget);
    return qRound(x / factor);
}

inline QFont DPIManagerV2::scaleFont(const QString& fontName, int pointSize, QWidget*)
{
    QFont font(fontName);
    font.setPointSizeF(scaleFontForFactor(pointSize));
    return font;
}

inline int DPIManagerV2::scaleFontPointSize(int pointSize, QWidget*)
{
    return scaleFontForFactor(pointSize);
}

inline int DPIManagerV2::scaleFontForFactor(int pt, unsigned int textScaleFactor)
{
    static constexpr unsigned int defaultFactor = 100;
    return qRound(pt * static_cast<qreal>(textScaleFactor) / defaultFactor);
}

inline void DPIManagerV2::applyFontScaling(QFont& font, QWidget*)
{
    int currentPt = font.pointSize();
    if (currentPt > 0)
        font.setPointSizeF(scaleFontForFactor(currentPt));
}

inline int DPIManagerV2::getTextScaleFactor()
{
    return 100;
}

inline void DPIManagerV2::setDpiWithScreen(QWidget* widget)
{
    if (widget)
        _dpi = qRound(getScreenDpi(widget));
    else
        _dpi = qRound(getScreenDpi());
}
