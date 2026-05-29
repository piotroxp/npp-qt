// This file is part of Notepad++ project
// Copyright (C)2021 adzm / Adam D. Walling
// Ported to Qt/Linux for npp-qt

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "../NppDarkMode.h"

#include <memory>
#include "../Parameters.h"
#include "dpiManagerV2.h"
#include "resource.h"

#include <QPainter>
#include <QPalette>
#include <QApplication>
#include <QWidget>
#include <QEvent>
#include <QDebug>

bool g_darkModeSupported = false;
bool g_darkModeEnabled = false;

// =============================================================================
// COLORREF helper - uses 0xRRGGBB format (same as HEXRGB output)
// =============================================================================

static inline QColor colorRefToQColor(COLORREF c) {
    return QColor(GetRValue(c), GetGValue(c), GetBValue(c));
}

static inline COLORREF qColorToColorRef(const QColor& c) {
    return RGB(c.red(), c.green(), c.blue());
}

static constexpr COLORREF HEXRGB(DWORD rrggbb) {
    return
        ((rrggbb & 0xFF0000) >> 16) |
        ((rrggbb & 0x00FF00)) |
        ((rrggbb & 0x0000FF) << 16);
}

namespace NppDarkMode
{
    // =============================================================================
    // Qt Color/Brush/Pen structures - replace Windows GDI objects
    // =============================================================================

    struct Brushes
    {
        QBrush background;
        QBrush ctrlBackground;
        QBrush hotBackground;
        QBrush dlgBackground;
        QBrush errorBackground;

        QBrush edgeBrush;
        QBrush hotEdgeBrush;
        QBrush disabledEdgeBrush;

        Brushes(const Colors& colors)
            : background(colorRefToQColor(colors.background))
            , ctrlBackground(colorRefToQColor(colors.softerBackground))
            , hotBackground(colorRefToQColor(colors.hotBackground))
            , dlgBackground(colorRefToQColor(colors.pureBackground))
            , errorBackground(colorRefToQColor(colors.errorBackground))
            , edgeBrush(colorRefToQColor(colors.edge))
            , hotEdgeBrush(colorRefToQColor(colors.hotEdge))
            , disabledEdgeBrush(colorRefToQColor(colors.disabledEdge))
        {}

        void change(const Colors& colors)
        {
            background = colorRefToQColor(colors.background);
            ctrlBackground = colorRefToQColor(colors.softerBackground);
            hotBackground = colorRefToQColor(colors.hotBackground);
            dlgBackground = colorRefToQColor(colors.pureBackground);
            errorBackground = colorRefToQColor(colors.errorBackground);

            edgeBrush = colorRefToQColor(colors.edge);
            hotEdgeBrush = colorRefToQColor(colors.hotEdge);
            disabledEdgeBrush = colorRefToQColor(colors.disabledEdge);
        }
    };

    struct Pens
    {
        QPen darkerTextPen;
        QPen edgePen;
        QPen hotEdgePen;
        QPen disabledEdgePen;

        Pens(const Colors& colors)
            : darkerTextPen(QPen(colorRefToQColor(colors.darkerText), 1))
            , edgePen(QPen(colorRefToQColor(colors.edge), 1))
            , hotEdgePen(QPen(colorRefToQColor(colors.hotEdge), 1))
            , disabledEdgePen(QPen(colorRefToQColor(colors.disabledEdge), 1))
        {}

        void change(const Colors& colors)
        {
            darkerTextPen = QPen(colorRefToQColor(colors.darkerText), 1);
            edgePen = QPen(colorRefToQColor(colors.edge), 1);
            hotEdgePen = QPen(colorRefToQColor(colors.hotEdge), 1);
            disabledEdgePen = QPen(colorRefToQColor(colors.disabledEdge), 1);
        }
    };

    // =============================================================================
    // Color Definitions
    // =============================================================================

    // black (default)
    static constexpr Colors darkColors{
        HEXRGB(0x202020),   // background
        HEXRGB(0x383838),   // softerBackground
        HEXRGB(0x454545),   // hotBackground
        HEXRGB(0x202020),   // pureBackground
        HEXRGB(0xB00000),   // errorBackground
        HEXRGB(0xE0E0E0),   // textColor
        HEXRGB(0xC0C0C0),   // darkerTextColor
        HEXRGB(0x808080),   // disabledTextColor
        HEXRGB(0xFFFF00),   // linkTextColor
        HEXRGB(0x646464),   // edgeColor
        HEXRGB(0x9B9B9B),   // hotEdgeColor
        HEXRGB(0x484848)    // disabledEdgeColor
    };

    static constexpr int offsetEdge = HEXRGB(0x1C1C1C);

    // red tone
    static constexpr int offsetRed = HEXRGB(0x100000);
    static constexpr Colors darkRedColors{
        darkColors.background + offsetRed,
        darkColors.softerBackground + offsetRed,
        darkColors.hotBackground + offsetRed,
        darkColors.pureBackground + offsetRed,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetRed,
        darkColors.hotEdge + offsetRed,
        darkColors.disabledEdge + offsetRed
    };

    // green tone
    static constexpr int offsetGreen = HEXRGB(0x001000);
    static constexpr Colors darkGreenColors{
        darkColors.background + offsetGreen,
        darkColors.softerBackground + offsetGreen,
        darkColors.hotBackground + offsetGreen,
        darkColors.pureBackground + offsetGreen,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetGreen,
        darkColors.hotEdge + offsetGreen,
        darkColors.disabledEdge + offsetGreen
    };

    // blue tone
    static constexpr int offsetBlue = HEXRGB(0x000020);
    static constexpr Colors darkBlueColors{
        darkColors.background + offsetBlue,
        darkColors.softerBackground + offsetBlue,
        darkColors.hotBackground + offsetBlue,
        darkColors.pureBackground + offsetBlue,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetBlue,
        darkColors.hotEdge + offsetBlue,
        darkColors.disabledEdge + offsetBlue
    };

    // purple tone
    static constexpr int offsetPurple = HEXRGB(0x100020);
    static constexpr Colors darkPurpleColors{
        darkColors.background + offsetPurple,
        darkColors.softerBackground + offsetPurple,
        darkColors.hotBackground + offsetPurple,
        darkColors.pureBackground + offsetPurple,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetPurple,
        darkColors.hotEdge + offsetPurple,
        darkColors.disabledEdge + offsetPurple
    };

    // cyan tone
    static constexpr int offsetCyan = HEXRGB(0x001020);
    static constexpr Colors darkCyanColors{
        darkColors.background + offsetCyan,
        darkColors.softerBackground + offsetCyan,
        darkColors.hotBackground + offsetCyan,
        darkColors.pureBackground + offsetCyan,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetCyan,
        darkColors.hotEdge + offsetCyan,
        darkColors.disabledEdge + offsetCyan
    };

    // olive tone
    static constexpr int offsetOlive = HEXRGB(0x101000);
    static constexpr Colors darkOliveColors{
        darkColors.background + offsetOlive,
        darkColors.softerBackground + offsetOlive,
        darkColors.hotBackground + offsetOlive,
        darkColors.pureBackground + offsetOlive,
        darkColors.errorBackground,
        darkColors.text,
        darkColors.darkerText,
        darkColors.disabledText,
        darkColors.linkText,
        darkColors.edge + offsetEdge + offsetOlive,
        darkColors.hotEdge + offsetOlive,
        darkColors.disabledEdge + offsetOlive
    };

    // customized
    static Colors darkCustomizedColors{ darkColors };

    static ColorTone g_colorToneChoice = blackTone;

    void setDarkTone(ColorTone colorToneChoice)
    {
        g_colorToneChoice = colorToneChoice;
    }

    // =============================================================================
    // Theme management
    // =============================================================================

    struct Theme
    {
        Colors _colors;
        Brushes _brushes;
        Pens _pens;

        Theme(const Colors& colors)
            : _colors(colors)
            , _brushes(colors)
            , _pens(colors)
        {}

        void change(const Colors& colors)
        {
            _colors = colors;
            _brushes.change(colors);
            _pens.change(colors);
        }
    };

    static Theme tDefault(darkColors);
    static Theme tR(darkRedColors);
    static Theme tG(darkGreenColors);
    static Theme tB(darkBlueColors);
    static Theme tP(darkPurpleColors);
    static Theme tC(darkCyanColors);
    static Theme tO(darkOliveColors);
    static Theme tCustom(darkCustomizedColors);

    static Theme& getTheme()
    {
        switch (g_colorToneChoice)
        {
            case redTone:    return tR;
            case greenTone:  return tG;
            case blueTone:   return tB;
            case purpleTone: return tP;
            case cyanTone:   return tC;
            case oliveTone:  return tO;
            case customizedTone: return tCustom;
            default:         return tDefault;
        }
    }

    // =============================================================================
    // Options and state
    // =============================================================================

    static Options _options;
    static AdvancedOptions g_advOptions;

    static Options configuredOptions()
    {
        const NppGUI& nppGui = NppParameters::getInstance().getNppGUI();
        Options opt;
        opt.enable = nppGui._darkmode._isEnabled;
        opt.enablePlugin = nppGui._darkmode._isEnabledPlugin;
        g_colorToneChoice = nppGui._darkmode._colorTone;
        tCustom.change(nppGui._darkmode._customColors);
        return opt;
    }

    static COLORREF cAccentDark = g_cDefaultSecondaryDark;
    static COLORREF cAccentLight = g_cDefaultSecondaryLight;

    // =============================================================================
    // Global dark mode state - g_darkModeSupported and g_darkModeEnabled
    // are declared in DarkMode.h (extern)
    // =============================================================================

    static bool g_isAtLeastWindows10 = false;
    static bool g_isWine = false;

    void initDarkMode()
    {
        _options = configuredOptions();
        initExperimentalDarkMode();
        initAdvancedOptions();

        // On Linux, we always support dark mode
        g_darkModeSupported = true;

        g_isAtLeastWindows10 = false;  // Not applicable on Linux

        if (NppDarkMode::isWindowsModeEnabled())
        {
            NppParameters& nppParam = NppParameters::getInstance();
            NppGUI& nppGUI = nppParam.getNppGUI();
            nppGUI._darkmode._isEnabled = NppDarkMode::isDarkModeReg() && !NppDarkMode::isHighContrast();
            _options.enable = nppGUI._darkmode._isEnabled;
        }

        setDarkMode(_options.enable, true);
        g_isWine = false;
    }

    void refreshDarkMode(HWND hwnd, bool forceRefresh)
    {
        Q_UNUSED(hwnd);
        bool supportedChanged = false;

        auto config = configuredOptions();

        if (_options.enable != config.enable)
        {
            supportedChanged = true;
            _options.enable = config.enable;
            setDarkMode(_options.enable, _options.enable);
        }

        if (!supportedChanged && !forceRefresh)
        {
            return;
        }

        // On Linux, we don't have HWND messaging, so just trigger repaints
        // The actual refresh will happen via Qt's event system
    }

    void initAdvancedOptions()
    {
        const NppGUI& nppGui = NppParameters::getInstance().getNppGUI();
        g_advOptions = nppGui._darkmode._advOptions;
    }

    bool isEnabled() { return _options.enable; }
    bool isEnabledForPlugins() { return _options.enablePlugin; }
    bool isExperimentalActive() { return g_darkModeEnabled; }
    bool isExperimentalSupported() { return g_darkModeSupported; }

    bool isWindowsModeEnabled() { return g_advOptions._enableWindowsMode; }
    void setWindowsMode(bool enable) { g_advOptions._enableWindowsMode = enable; }

    void setThemeName(const std::wstring& newThemeName)
    {
        if (NppDarkMode::isEnabled())
            g_advOptions._darkDefaults._xmlFileName = newThemeName;
        else
            g_advOptions._lightDefaults._xmlFileName = newThemeName;
    }

    std::wstring getThemeName()
    {
        auto& theme = NppDarkMode::isEnabled() ? g_advOptions._darkDefaults._xmlFileName : g_advOptions._lightDefaults._xmlFileName;
        return (lstrcmp(theme.c_str(), L"stylers.xml") == 0) ? L"" : theme;
    }

    TbIconInfo getToolbarIconInfo(bool useDark)
    {
        auto& toolbarInfo = useDark ? g_advOptions._darkDefaults._tbIconInfo : g_advOptions._lightDefaults._tbIconInfo;
        if (toolbarInfo._tbCustomColor == 0)
            toolbarInfo._tbCustomColor = NppDarkMode::getAccentColor(useDark);
        return toolbarInfo;
    }

    TbIconInfo getToolbarIconInfo() { return getToolbarIconInfo(NppDarkMode::isEnabled()); }

    void setToolbarIconSet(int state2Set, bool useDark)
    {
        if (useDark)
            g_advOptions._darkDefaults._tbIconInfo._tbIconSet = static_cast<toolBarStatusType>(state2Set);
        else
            g_advOptions._lightDefaults._tbIconInfo._tbIconSet = static_cast<toolBarStatusType>(state2Set);
    }

    void setToolbarIconSet(int state2Set) { setToolbarIconSet(state2Set, NppDarkMode::isEnabled()); }

    void setToolbarFluentColor(FluentColor color2Set, bool useDark)
    {
        if (useDark)
            g_advOptions._darkDefaults._tbIconInfo._tbColor = color2Set;
        else
            g_advOptions._lightDefaults._tbIconInfo._tbColor = color2Set;
    }

    void setToolbarFluentColor(FluentColor color2Set) { setToolbarFluentColor(color2Set, NppDarkMode::isEnabled()); }

    void setToolbarFluentMonochrome(bool setMonochrome, bool useDark)
    {
        if (useDark)
            g_advOptions._darkDefaults._tbIconInfo._tbUseMono = setMonochrome;
        else
            g_advOptions._lightDefaults._tbIconInfo._tbUseMono = setMonochrome;
    }

    void setToolbarFluentMonochrome(bool setMonochrome) { setToolbarFluentMonochrome(setMonochrome, NppDarkMode::isEnabled()); }

    void setToolbarFluentCustomColor(COLORREF color, bool useDark)
    {
        if (useDark)
            g_advOptions._darkDefaults._tbIconInfo._tbCustomColor = color;
        else
            g_advOptions._lightDefaults._tbIconInfo._tbCustomColor = color;
    }

    void setToolbarFluentCustomColor(COLORREF color) { setToolbarFluentCustomColor(color, NppDarkMode::isEnabled()); }

    void setTabIconSet(bool useAltIcons, bool useDark)
    {
        if (useDark)
            g_advOptions._darkDefaults._tabIconSet = useAltIcons ? 1 : 2;
        else
            g_advOptions._lightDefaults._tabIconSet = useAltIcons ? 1 : 0;
    }

    int getTabIconSet(bool useDark) { return useDark ? g_advOptions._darkDefaults._tabIconSet : g_advOptions._lightDefaults._tabIconSet; }
    bool useTabTheme() { return NppDarkMode::isEnabled() ? g_advOptions._darkDefaults._tabUseTheme : g_advOptions._lightDefaults._tabUseTheme; }

    void setAdvancedOptions()
    {
        NppGUI& nppGui = NppParameters::getInstance().getNppGUI();
        nppGui._darkmode._advOptions = g_advOptions;
    }

    bool isWindows10() { return false; }
    bool isWindows11() { return false; }
    DWORD getWindowsBuildNumber() { return 0; }

    COLORREF invertLightness(COLORREF c)
    {
        WORD h = 0, s = 0, l = 0;
        ColorRGBToHLS(c, &h, &l, &s);
        l = 240 - l;
        return ColorHLSToRGB(h, l, s);
    }

    static TreeViewStyle g_treeViewStylePrev = TreeViewStyle::classic;
    static TreeViewStyle g_treeViewStyle = TreeViewStyle::classic;
    static COLORREF g_treeViewBg = NppParameters::getInstance().getCurrentDefaultBgColor();
    static double g_lightnessTreeView = 50.0;

    double calculatePerceivedLightness(COLORREF c)
    {
        auto linearValue = [](double colorChannel) -> double {
            colorChannel /= 255.0;
            if (colorChannel <= 0.04045)
                return colorChannel / 12.92;
            return std::pow(((colorChannel + 0.055) / 1.055), 2.4);
        };

        double r = linearValue(static_cast<double>(GetRValue(c)));
        double g = linearValue(static_cast<double>(GetGValue(c)));
        double b = linearValue(static_cast<double>(GetBValue(c)));

        double luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        double lightness = (luminance <= 216.0 / 24389.0) ? (luminance * 24389.0 / 27.0) : (std::pow(luminance, (1.0 / 3.0)) * 116.0 - 16.0);
        return lightness;
    }

    COLORREF getAccentColor(bool useDark) { return useDark ? cAccentDark : cAccentLight; }
    COLORREF getAccentColor() { return getAccentColor(NppDarkMode::isEnabled()); }

    // Color getters
    COLORREF getBackgroundColor()        { return getTheme()._colors.background; }
    COLORREF getCtrlBackgroundColor()    { return getTheme()._colors.softerBackground; }
    COLORREF getHotBackgroundColor()     { return getTheme()._colors.hotBackground; }
    COLORREF getDlgBackgroundColor()     { return getTheme()._colors.pureBackground; }
    COLORREF getErrorBackgroundColor()   { return getTheme()._colors.errorBackground; }
    COLORREF getTextColor()              { return getTheme()._colors.text; }
    COLORREF getDarkerTextColor()        { return getTheme()._colors.darkerText; }
    COLORREF getDisabledTextColor()      { return getTheme()._colors.disabledText; }
    COLORREF getLinkTextColor()          { return getTheme()._colors.linkText; }
    COLORREF getEdgeColor()              { return getTheme()._colors.edge; }
    COLORREF getHotEdgeColor()           { return getTheme()._colors.hotEdge; }
    COLORREF getDisabledEdgeColor()      { return getTheme()._colors.disabledEdge; }

    // Brush/Pen access - return nullptr on Linux (colors accessed via get*Color())
    HBRUSH getBackgroundBrush()       { return nullptr; }
    HBRUSH getCtrlBackgroundBrush()  { return nullptr; }
    HBRUSH getHotBackgroundBrush()   { return nullptr; }
    HBRUSH getDlgBackgroundBrush()   { return nullptr; }
    HBRUSH getErrorBackgroundBrush() { return nullptr; }
    HBRUSH getEdgeBrush()            { return nullptr; }
    HBRUSH getHotEdgeBrush()         { return nullptr; }
    HBRUSH getDisabledEdgeBrush()    { return nullptr; }
    HPEN getDarkerTextPen()          { return nullptr; }
    HPEN getEdgePen()                { return nullptr; }
    HPEN getHotEdgePen()             { return nullptr; }
    HPEN getDisabledEdgePen()        { return nullptr; }

    // Color setters
    void setBackgroundColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.background = c; getTheme().change(clrs); }
    void setCtrlBackgroundColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.softerBackground = c; getTheme().change(clrs); }
    void setHotBackgroundColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.hotBackground = c; getTheme().change(clrs); }
    void setDlgBackgroundColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.pureBackground = c; getTheme().change(clrs); }
    void setErrorBackgroundColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.errorBackground = c; getTheme().change(clrs); }
    void setTextColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.text = c; getTheme().change(clrs); }
    void setDarkerTextColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.darkerText = c; getTheme().change(clrs); }
    void setDisabledTextColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.disabledText = c; getTheme().change(clrs); }
    void setLinkTextColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.linkText = c; getTheme().change(clrs); }
    void setEdgeColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.edge = c; getTheme().change(clrs); }
    void setHotEdgeColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.hotEdge = c; getTheme().change(clrs); }
    void setDisabledEdgeColor(COLORREF c) { Colors clrs = getTheme()._colors; clrs.disabledEdge = c; getTheme().change(clrs); }

    Colors getDarkModeDefaultColors(ColorTone colorTone)
    {
        switch (colorTone)
        {
            case ColorTone::redTone:    return darkRedColors;
            case ColorTone::greenTone:  return darkGreenColors;
            case ColorTone::blueTone:   return darkBlueColors;
            case ColorTone::purpleTone: return darkPurpleColors;
            case ColorTone::cyanTone:   return darkCyanColors;
            case ColorTone::oliveTone:  return darkOliveColors;
            case ColorTone::customizedTone:
            case ColorTone::blackTone:
            default:                    return darkColors;
        }
    }

    void changeCustomTheme(const Colors& colors) { tCustom.change(colors); }

    void handleSettingChange(HWND hwnd, LPARAM lParam, bool isFromBtn)
    {
        Q_UNUSED(hwnd); Q_UNUSED(lParam); Q_UNUSED(isFromBtn);
        if (!isExperimentalSupported()) return;
        g_darkModeEnabled = NppDarkMode::isDarkModeReg() && !NppDarkMode::isHighContrast();
    }

    bool isHighContrast() { return false; }

    bool isDarkModeReg()
    {
        // On Linux, check system theme via Qt
        QPalette palette = qApp->palette();
        QColor bg = palette.window().color();
        return (bg.redF() * 0.299 + bg.greenF() * 0.587 + bg.blueF() * 0.114) < 0.5;
    }

    void initExperimentalDarkMode()
    {
        g_darkModeSupported = true;
        g_darkModeEnabled = _options.enable;
    }

    void setDarkMode(bool useDark, bool fixDarkScrollbar)
    {
        Q_UNUSED(fixDarkScrollbar);
        g_darkModeEnabled = useDark;
        _options.enable = useDark;

        if (useDark)
        {
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window, colorRefToQColor(getBackgroundColor()));
            darkPalette.setColor(QPalette::WindowText, colorRefToQColor(getTextColor()));
            darkPalette.setColor(QPalette::Base, colorRefToQColor(getCtrlBackgroundColor()));
            darkPalette.setColor(QPalette::Text, colorRefToQColor(getTextColor()));
            darkPalette.setColor(QPalette::Button, colorRefToQColor(getCtrlBackgroundColor()));
            darkPalette.setColor(QPalette::ButtonText, colorRefToQColor(getTextColor()));
            darkPalette.setColor(QPalette::Highlight, colorRefToQColor(getHotBackgroundColor()));
            darkPalette.setColor(QPalette::HighlightedText, colorRefToQColor(getTextColor()));
            darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, colorRefToQColor(getDisabledTextColor()));
            darkPalette.setColor(QPalette::Disabled, QPalette::Text, colorRefToQColor(getDisabledTextColor()));
            qApp->setPalette(darkPalette);
        }
        else
        {
            qApp->setPalette(QApplication::palette());
        }
    }

    void allowDarkModeForApp(bool allow) { Q_UNUSED(allow); }
    bool allowDarkModeForWindow(HWND hWnd, bool allow) { Q_UNUSED(hWnd); Q_UNUSED(allow); return true; }
    void setTitleBarThemeColor(HWND hWnd) { Q_UNUSED(hWnd); }
    void enableDarkScrollBarForWindowAndChildren(HWND hwnd) { Q_UNUSED(hwnd); }

    // Drawing utilities
    void paintRoundRect(HDC hdc, const RECT rect, const HPEN hpen, const HBRUSH hBrush, int width, int height)
    {
        QPainter* painter = reinterpret_cast<QPainter*>(hdc);
        if (!painter) return;
        QRect qrect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(colorRefToQColor(getBackgroundColor()));
        int rx = qMin(width, qrect.width() / 2);
        int ry = qMin(height, qrect.height() / 2);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(qrect, rx, ry);
        painter->restore();
    }

    inline void paintRoundFrameRect(HDC hdc, const RECT rect, const HPEN hpen, int width, int height)
    {
        QPainter* painter = reinterpret_cast<QPainter*>(hdc);
        if (!painter) return;
        QRect qrect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        painter->save();
        painter->setPen(getTheme()._pens.edgePen);
        painter->setBrush(Qt::NoBrush);
        int rx = qMin(width, qrect.width() / 2);
        int ry = qMin(height, qrect.height() / 2);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(qrect, rx, ry);
        painter->restore();
    }

    // Control theming stubs
    void subclassButtonControl(HWND hwnd) { Q_UNUSED(hwnd); }
    void subclassGroupboxControl(HWND hwnd) { Q_UNUSED(hwnd); }
    void subclassTabControl(HWND hWnd) { Q_UNUSED(hWnd); }
    void subclassComboBoxControl(HWND hWnd) { Q_UNUSED(hWnd); }
    bool subclassTabUpDownControl(HWND hwnd) { Q_UNUSED(hwnd); return false; }

    void subclassAndThemeButton(HWND hwnd, NppDarkModeParams p) { Q_UNUSED(hwnd); Q_UNUSED(p); }
    void subclassAndThemeComboBox(HWND hWnd, NppDarkModeParams p) { Q_UNUSED(hWnd); Q_UNUSED(p); }
    void subclassAndThemeListBoxOrEditControl(HWND hwnd, NppDarkModeParams p, bool isListBox) { Q_UNUSED(hwnd); Q_UNUSED(p); Q_UNUSED(isListBox); }
    void subclassAndThemeListView(HWND hwnd, NppDarkModeParams p) { Q_UNUSED(hwnd); Q_UNUSED(p); }
    void themeTreeView(HWND hwnd, NppDarkModeParams p) { Q_UNUSED(hwnd); Q_UNUSED(p); }
    void themeToolbar(HWND hwnd, NppDarkModeParams p) { Q_UNUSED(hwnd); Q_UNUSED(p); }
    void themeRichEdit(HWND hwnd, NppDarkModeParams p) { Q_UNUSED(hwnd); Q_UNUSED(p); }

    void autoSubclassAndThemeChildControls(HWND hwndParent, bool subclass, bool theme) { Q_UNUSED(hwndParent); Q_UNUSED(subclass); Q_UNUSED(theme); }
    void autoThemeChildControls(HWND hwndParent) { Q_UNUSED(hwndParent); }
    void autoSubclassAndThemePluginDockWindow(HWND hwnd) { Q_UNUSED(hwnd); }
    ULONG autoSubclassAndThemePlugin(HWND hwnd, ULONG dmFlags) { Q_UNUSED(hwnd); Q_UNUSED(dmFlags); return 0; }
    void autoSubclassCtlColor(HWND hWnd) { Q_UNUSED(hWnd); }
    void autoSubclassAndThemeWindowNotify(HWND hwnd) { Q_UNUSED(hwnd); }
    void autoSubclassWindowMenuBar(HWND hWnd) { Q_UNUSED(hWnd); }

    void setDarkTitleBar(HWND hwnd) { Q_UNUSED(hwnd); }
    void setDarkExplorerTheme(HWND hwnd) { Q_UNUSED(hwnd); }
    void setDarkScrollBar(HWND hwnd) { Q_UNUSED(hwnd); }
    void setDarkTooltips(HWND hwnd, ToolTipsType type) { Q_UNUSED(hwnd); Q_UNUSED(type); }
    void setDarkLineAbovePanelToolbar(HWND hwnd) { Q_UNUSED(hwnd); }
    void setDarkListView(HWND hwnd) { Q_UNUSED(hwnd); }
    void disableVisualStyle(HWND hwnd, bool doDisable) { Q_UNUSED(hwnd); Q_UNUSED(doDisable); }

    constexpr double g_middleGrayRange = 2.0;

    void calculateTreeViewStyle()
    {
        COLORREF bgColor = NppParameters::getInstance().getCurrentDefaultBgColor();
        if (g_treeViewBg != bgColor || g_lightnessTreeView == 50.0)
        {
            g_lightnessTreeView = calculatePerceivedLightness(bgColor);
            g_treeViewBg = bgColor;
        }
        if (g_lightnessTreeView < (50.0 - g_middleGrayRange))
            g_treeViewStyle = TreeViewStyle::dark;
        else if (g_lightnessTreeView > (50.0 + g_middleGrayRange))
            g_treeViewStyle = TreeViewStyle::light;
        else
            g_treeViewStyle = TreeViewStyle::classic;
    }

    void updateTreeViewStylePrev() { g_treeViewStylePrev = g_treeViewStyle; }
    TreeViewStyle getTreeViewStyle() { return g_treeViewStyle; }
    void setTreeViewStyle(HWND hWnd, bool force) { Q_UNUSED(hWnd); Q_UNUSED(force); }
    bool isThemeDark() { return g_treeViewStyle == TreeViewStyle::dark; }
    void setBorder(HWND hwnd, bool border) { Q_UNUSED(hwnd); Q_UNUSED(border); }
    void setDarkAutoCompletion() {}

    LRESULT onCtlColor(HDC hdc) { Q_UNUSED(hdc); return NppDarkMode::isEnabled() ? 0 : FALSE; }
    LRESULT onCtlColorCtrl(HDC hdc) { Q_UNUSED(hdc); return NppDarkMode::isEnabled() ? 0 : FALSE; }
    LRESULT onCtlColorDlg(HDC hdc) { Q_UNUSED(hdc); return NppDarkMode::isEnabled() ? 0 : FALSE; }
    LRESULT onCtlColorError(HDC hdc) { Q_UNUSED(hdc); return NppDarkMode::isEnabled() ? 0 : FALSE; }
    LRESULT onCtlColorDlgStaticText(HDC hdc, bool isTextEnabled) { Q_UNUSED(hdc); Q_UNUSED(isTextEnabled); return FALSE; }
    LRESULT onCtlColorDlgLinkText(HDC hdc, bool isTextEnabled) { Q_UNUSED(hdc); Q_UNUSED(isTextEnabled); return FALSE; }
    LRESULT onCtlColorListbox(WPARAM wParam, LPARAM lParam) { Q_UNUSED(wParam); Q_UNUSED(lParam); return FALSE; }
}
