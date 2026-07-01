// Ported to Qt6: DPI-aware font and scaling utilities
#pragma once
#include <QtCore>
#include <QFont>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QFontDatabase>

// Win32 LOGFONT structure + required constants (lifted for non-Windows builds)
#ifndef LOGFONT
#ifndef FW_NORMAL
#define FW_NORMAL 400
#endif
#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET 1
#endif
#ifndef LF_FACESIZE
#define LF_FACESIZE 32
#endif
struct LOGFONT {
    long lfHeight;
    long lfWidth;
    long lfEscapement;
    long lfOrientation;
    long lfWeight;
    unsigned char lfItalic;
    unsigned char lfUnderline;
    unsigned char lfStrikeOut;
    unsigned char lfCharSet;
    unsigned char lfOutPrecision;
    unsigned char lfClipPrecision;
    unsigned char lfQuality;
    unsigned char lfPitchAndFamily;
    char lfFaceName[32];
};
#endif

class DPIManagerV2
{
public:
    DPIManagerV2() { }
    virtual ~DPIManagerV2() = default;

    static qreal getScreenDpi(QWidget* widget = nullptr);
    static qreal getScaleFactor(QWidget* widget = nullptr);
    static int scale(int x, QWidget* widget = nullptr);

    // Overload: scale by raw DPI int (used by callers that already have dpi value)
    static int scale(int x, int dpi) {
        return qRound(x * static_cast<qreal>(dpi) / 96.0);
    }

    static int unscale(int x, QWidget* widget = nullptr);
    static QFont scaleFont(const QString& fontName, int pointSize, QWidget* widget = nullptr);
    static int scaleFontPointSize(int pointSize, QWidget* widget = nullptr);
    static int scaleFontForFactor(int pt, unsigned int textScaleFactor = 100);
    static void applyFontScaling(QFont& font, QWidget* widget = nullptr);
    static int getTextScaleFactor();

    // Win32 SystemParametersInfo(SPI_GETNONCLIENTMETRICS) equivalent —
    // returns a LOGFONT for the default GUI font at the given DPI.
    // Translates: SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ..., &nm) → logfont
    static LOGFONT getDefaultGUIFontForDpi(unsigned int dpi);

    void setDpiWithScreen(QWidget* widget);
    void setDpi(int dpi) { _dpi = dpi; }
    int getDpi() const { return _dpi; }
    int scale(int x) const { return scale(x, nullptr); }
    int unscale(int x) const { return unscale(x, nullptr); }
    int scaleFont(int pt) const { return scaleFontForFactor(pt); }
    static int getDpiForWindow(QWidget* w) { return w ? w->logicalDpiY() : 96; }

    // Win32 SM_CXMINTRACK equivalent — minimum window track width in pixels
    // Base value 112 from Windows default; scaled by DPI factor.
    static int getSystemMetricsForDpi(int /*metric*/, QWidget* widget = nullptr) {
        return scale(112, widget);  // SM_CXMINTRACK = 112 (Windows default minimum width)
    }

    // No-op stub — Qt6 handles DPI initialization automatically via QApplication
    static void initDpiAPI() { }

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

// Win32 SystemParametersInfo(SPI_GETNONCLIENTMETRICS) → QFontDatabase translation
inline LOGFONT DPIManagerV2::getDefaultGUIFontForDpi(unsigned int dpi)
{
    LOGFONT lf = {};
    lf.lfHeight = -qRound(8 * static_cast<qreal>(dpi) / 96.0); // 8pt at current DPI
    lf.lfWeight = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;

    // Try to get the system GUI font name via Qt
    QFont guiFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    QString faceName = guiFont.family();
    // Qt family may be locale-dependent; fall back to Segoe UI (Windows default)
    if (faceName.isEmpty() || faceName == QStringLiteral("Sans Serif")) {
        faceName = QStringLiteral("Segoe UI");
    }
    qstrncpy(lf.lfFaceName, faceName.toLocal8Bit().constData(), LF_FACESIZE - 1);
    lf.lfFaceName[LF_FACESIZE - 1] = '\0';
    return lf;
}

inline void DPIManagerV2::setDpiWithScreen(QWidget* widget)
{
    if (widget)
        _dpi = qRound(getScreenDpi(widget));
    else
        _dpi = qRound(getScreenDpi());
}
