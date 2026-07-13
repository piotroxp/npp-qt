// Ported to Qt6: DPI-aware font and scaling utilities — implementation
#include "MISC/Common/dpiManagerV2.h"
#include <QApplication>
#include <QScreen>
#include <QFontDatabase>

// USER_DEFAULT_SCREEN_DPI = 96 (Windows default)
static constexpr int USER_DEFAULT_SCREEN_DPI = 96;

qreal DPIManagerV2::getScreenDpi(QWidget* widget)
{
    if (widget) {
        QScreen* screen = widget->screen();
        if (screen)
            return screen->logicalDotsPerInchY();
        // Fallback: physical DPI via devicePixelRatio
        return widget->devicePixelRatio() * 96.0;
    }
    QScreen* primary = QApplication::primaryScreen();
    if (primary)
        return primary->logicalDotsPerInchY();
    return 96.0;
}

qreal DPIManagerV2::getScaleFactor(QWidget* widget)
{
    return getScreenDpi(widget) / static_cast<qreal>(USER_DEFAULT_SCREEN_DPI);
}

int DPIManagerV2::scale(int x, int toDpi, int fromDpi)
{
    if (fromDpi == 0) fromDpi = USER_DEFAULT_SCREEN_DPI;
    return qRound(x * static_cast<qreal>(toDpi) / static_cast<qreal>(fromDpi));
}

int DPIManagerV2::scale(int x, int dpi)
{
    return scale(x, dpi, USER_DEFAULT_SCREEN_DPI);
}

int DPIManagerV2::scale(int x, QWidget* widget)
{
    return qRound(x * getScaleFactor(widget));
}

int DPIManagerV2::unscale(int x, int dpi)
{
    return scale(x, USER_DEFAULT_SCREEN_DPI, dpi);
}

int DPIManagerV2::unscale(int x, QWidget* widget)
{
    return qRound(x / getScaleFactor(widget));
}

int DPIManagerV2::scaleFont(int pt, int dpi)
{
    // Win32: return -(MulDiv(pt, dpi, 72)) → negative = height units
    // 72 is points-per-inch reference (1 pt = 1/72 inch)
    return -(scale(pt, dpi, 72));
}

int DPIManagerV2::scaleFont(int pt, QWidget* widget)
{
    return scaleFont(pt, qRound(getScreenDpi(widget)));
}

int DPIManagerV2::scaleFontForFactor(int pt, unsigned int textScaleFactor)
{
    static constexpr unsigned int defaultFactor = 100;
    return qRound(pt * static_cast<qreal>(textScaleFactor) / static_cast<qreal>(defaultFactor));
}

int DPIManagerV2::getTextScaleFactor()
{
    // Win32 reads HKEY_CURRENT_USER\Software\Microsoft\Accessibility\TextScaleFactor
    // Qt6/Linux: system DPI scaling is handled by the desktop environment (KDE, GNOME, etc.)
    // via Qt's platform integration — font sizes are already scaled by the window manager.
    // On X11/Wayland, QFont already respects the desktop scaling factor. Default to 100%.
    return 100;
}

void DPIManagerV2::applyFontScaling(QFont& font, QWidget* widget)
{
    int currentPt = font.pointSize();
    if (currentPt > 0)
        font.setPointSizeF(static_cast<qreal>(scaleFontForFactor(currentPt)));
}

LOGFONT DPIManagerV2::getDefaultGUIFontForDpi(unsigned int dpi, FontType type)
{
    LOGFONT lf = {};
    lf.lfHeight = -scaleFont(8, static_cast<int>(dpi)); // 8pt at target DPI
    lf.lfWeight = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = 3; // OUT_DEFAULT_PRECIS
    lf.lfQuality = 1;     // DEFAULT_QUALITY

    // Map FontType to the appropriate Qt system font
    // Qt6 QFontDatabase::SystemFont has: GeneralFont, FixedFont, TitleFont, SmallestReadableFont
    QFontDatabase::SystemFont qtFontRole;
    switch (type) {
        case FontType::caption:   qtFontRole = QFontDatabase::SystemFont::TitleFont;        break;
        case FontType::smcaption: qtFontRole = QFontDatabase::SystemFont::SmallestReadableFont; break;
        case FontType::menu:
        case FontType::status:
        case FontType::message:
        default:                 qtFontRole = QFontDatabase::SystemFont::GeneralFont;     break;
    }
    QFont guiFont = QFontDatabase::systemFont(qtFontRole);
    QString faceName = guiFont.family();
    if (faceName.isEmpty() || faceName == QStringLiteral("Sans Serif")) {
        faceName = QStringLiteral("Segoe UI");
    }
    qstrncpy(lf.lfFaceName, faceName.toLocal8Bit().constData(), LF_FACESIZE - 1);
    lf.lfFaceName[LF_FACESIZE - 1] = '\0';
    return lf;
}

void DPIManagerV2::setDpiWithScreen(QWidget* widget)
{
    if (widget)
        _dpi = qRound(getScreenDpi(widget));
    else
        _dpi = qRound(getScreenDpi());
}

void DPIManagerV2::setDpiWithParent(QWidget* w)
{
    if (w) {
        QWidget* parent = w->parentWidget();
        _dpi = getDpiForWindow(parent ? parent : w);
    } else {
        _dpi = qRound(getScreenDpi());
    }
}

void DPIManagerV2::setDpiWP(WPARAM wParam)
{
    // Win32 WM_DPICHANGED wParam: LOWORD = new DPI
    // Cast handles any Qt signal/slot that passes an integer DPI value
    _dpi = static_cast<int>(wParam & 0xFFFF);
}

int DPIManagerV2::getSystemMetricsForDpi(int metric, QWidget* widget)
{
    // Common SM_* values and their Qt equivalents:
    // SM_CXMINTRACK  = 34  → minimum window drag width
    // SM_CYMINTRACK  = 35  → minimum window drag height
    // SM_CXSMICON    = 49  → small icon width (16 typical)
    // SM_CYSMICON    = 50  → small icon height
    // SM_CXICON      = 11  → icon width (32 typical)
    // SM_CYICON      = 12  → icon height
    // SM_CXBORDER    = 5   → border width
    // SM_CYBORDER    = 6   → border height
    // SM_CXEDGE      = 45  → 3D border width
    // SM_CYEDGE      = 46  → 3D border height
    // Default fallback: scale base value by DPI
    int baseValue = 112; // generic fallback
    switch (metric) {
        case 34: baseValue = 112; break;  // SM_CXMINTRACK
        case 35: baseValue = 27;  break;  // SM_CYMINTRACK
        case 49: baseValue = 16;  break;  // SM_CXSMICON
        case 50: baseValue = 16;  break;  // SM_CYSMICON
        case 11: baseValue = 32;  break;  // SM_CXICON
        case 12: baseValue = 32;  break;  // SM_CYICON
        case 5:  baseValue = 1;   break;  // SM_CXBORDER
        case 6:  baseValue = 1;  break;  // SM_CYBORDER
        case 45: baseValue = 1;   break;  // SM_CXEDGE
        case 46: baseValue = 1;   break;  // SM_CYEDGE
        default: baseValue = 112; break;  // fallback: SM_CXMINTRACK
    }
    return scale(baseValue, widget);
}

void DPIManagerV2::setPositionDpi(long lParam, QWidget* w, unsigned flags)
{
    // Win32: SetWindowPos(hWnd, nullptr, prc->left, prc->top, prc->right-prc->left, prc->bottom-prc->top, flags)
    // Qt: use QWidget geometry setters
    if (!w)
        return;
    const QRect* prc = reinterpret_cast<const QRect*>(lParam);
    if (!prc)
        return;
    Qt::WindowFlags wf(flags);
    w->setGeometry(prc->left(), prc->top(), prc->width(), prc->height());
    Q_UNUSED(wf);
}
