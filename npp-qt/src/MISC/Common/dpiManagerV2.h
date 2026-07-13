// Ported to Qt6: DPI-aware font and scaling utilities
#pragma once
#include "NppConstants.h"
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

    // Win32 FontType enum (maps to Qt font categories)
    enum class FontType { menu, status, message, caption, smcaption };

    // DPI accessors
    static qreal getScreenDpi(QWidget* widget = nullptr);
    static qreal getScaleFactor(QWidget* widget = nullptr);

    // Static scale/unscale (base 96 DPI → target DPI)
    static int scale(int x, int toDpi, int fromDpi);
    static int scale(int x, int dpi);                          // from USER_DEFAULT_SCREEN_DPI (96)
    static int scale(int x, QWidget* widget);                  // from widget's DPI
    static int unscale(int x, int dpi);                       // to USER_DEFAULT_SCREEN_DPI
    static int unscale(int x, QWidget* widget);

    // Instance scale/unscale (using stored _dpi)
    int scale(int x) const { return scale(x, _dpi); }
    int unscale(int x) const { return unscale(x, _dpi); }

    // Font scaling
    static int scaleFont(int pt, int dpi);                    // pt at 72pt/inch ref
    static int scaleFont(int pt, QWidget* widget);
    int scaleFont(int pt) const { return scaleFont(pt, _dpi); }
    static int scaleFontForFactor(int pt, unsigned int textScaleFactor = 100);
    static int getTextScaleFactor();

    // Font helpers
    static LOGFONT getDefaultGUIFontForDpi(unsigned int dpi, FontType type = FontType::message);
    LOGFONT getDefaultGUIFontForDpi(FontType type = FontType::message) const {
        return getDefaultGUIFontForDpi(static_cast<unsigned int>(_dpi), type);
    }
    static void applyFontScaling(QFont& font, QWidget* widget = nullptr);

    // getDpiForWindow — Win32 GetDpiForWindow mapped to Qt QWidget
    static int getDpiForWindow(QWidget* w) {
        if (w) {
            // Prefer logical DPI (DPI-aware scaling); fallback to physical / devicePixelRatio
            qreal dpi = w->logicalDpiY();
            if (dpi <= 0)
                dpi = w->devicePixelRatio() * 96.0;
            return qRound(dpi);
        }
        return qRound(getScreenDpi());
    }

    // No-op: Qt6 handles DPI automatically via QApplication / QScreen
    static void initDpiAPI() { }

    // DPI setters
    void setDpi(int dpi) { _dpi = dpi; }
    void setDpi(QWidget* w) { setDpi(getDpiForWindow(w)); }
    void setDpiWithParent(QWidget* w);                         // uses parentWidget()
    void setDpiWithSystem() { _dpi = qRound(getScreenDpi()); }
    void setDpiWithScreen(QWidget* widget);
    void setDpiWP(WPARAM wParam);                             // LOWORD(wParam) on WM_DPICHANGED

    int getDpi() const { return _dpi; }

    // System metrics (Win32 SM_* mapped via DPI scaling)
    static int getSystemMetricsForDpi(int metric, QWidget* widget);

    // Static scale by HWND-free QWidget path (fallback for callers with DPI int)
    static int getDpiForSystem() {
        return qRound(getScreenDpi());
    }

    // Stub for Win32 DPI awareness — Qt6 is always DPI-aware
    static bool isValidDpiAwarenessContext(void*) { return true; }
    static void* setThreadDpiAwarenessContext(void*) { return nullptr; }
    static bool adjustWindowRectExForDpi(void*, long, bool, long, int) { return false; }

    // Stub: setPositionDpi — Qt equivalent is QWidget::setGeometry/move
    static void setPositionDpi(long lParam, QWidget* w, unsigned flags);

    // Icon loading stub (Qt uses QIcon/QPixmap from resources)
    static void loadIcon(const QString& /*resourcePath*/, const QSize& /*size*/, QIcon& /*outIcon*/) { }

private:
    int _dpi = 96;   // USER_DEFAULT_SCREEN_DPI = 96
};
