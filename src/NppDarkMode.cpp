// Semantic lift: NppDarkMode.cpp — Win32 dark mode → Qt6 palette controller
// Original: PowerEditor/src/NppDarkMode.cpp (4542 lines, adzm / GPLv3)
// Target: npp-qt/src/NppDarkMode.cpp
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3
// Win32→Qt6: HBRUSH/HPEN → QBrush/QPen; HWND → QWidget*; SendMessage → signal/slot

#include "NppDarkMode.h"
#include "Preference.h"

#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QPalette>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QPainter>
#include <QChildEvent>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QMainWindow>
#include <QHeaderView>
#include <QProxyStyle>
#include <QFontDatabase>

#ifdef _WIN32
// windows.h removed
#endif

namespace NppDarkMode
{

// =============================================================================
// Default colour tables (from Win32 originals)
// =============================================================================

static constexpr int k_offsetEdge   = hexRgb(0x1C1C1C);
static constexpr int k_offsetRed   = hexRgb(0x100000);
static constexpr int k_offsetGreen = hexRgb(0x001000);
static constexpr int k_offsetBlue  = hexRgb(0x000020);
static constexpr int k_offsetPurple = hexRgb(0x100020);
static constexpr int k_offsetCyan  = hexRgb(0x001020);
static constexpr int k_offsetOlive = hexRgb(0x101000);

static constexpr Colors k_colorsBlack =
{
    k_darkBg,
    k_darkCtrlBg,
    k_darkHotBg,
    k_darkPureBg,
    k_darkErrorBg,
    k_darkText,
    k_darkDarkerText,
    k_darkDisabledText,
    k_darkLink,
    k_darkEdge,
    k_darkHotEdge,
    k_darkDisabledEdge
};

static constexpr Colors makeRedColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetRed;
    c.softerBackground += k_offsetRed;
    c.hotBackground    += k_offsetRed;
    c.pureBackground   += k_offsetRed;
    c.edge             += k_offsetEdge + k_offsetRed;
    c.hotEdge          += k_offsetRed;
    c.disabledEdge     += k_offsetRed;
    return c;
}

static constexpr Colors makeGreenColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetGreen;
    c.softerBackground += k_offsetGreen;
    c.hotBackground    += k_offsetGreen;
    c.pureBackground   += k_offsetGreen;
    c.edge             += k_offsetEdge + k_offsetGreen;
    c.hotEdge          += k_offsetGreen;
    c.disabledEdge     += k_offsetGreen;
    return c;
}

static constexpr Colors makeBlueColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetBlue;
    c.softerBackground += k_offsetBlue;
    c.hotBackground    += k_offsetBlue;
    c.pureBackground   += k_offsetBlue;
    c.edge             += k_offsetEdge + k_offsetBlue;
    c.hotEdge          += k_offsetBlue;
    c.disabledEdge     += k_offsetBlue;
    return c;
}

static constexpr Colors makePurpleColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetPurple;
    c.softerBackground += k_offsetPurple;
    c.hotBackground    += k_offsetPurple;
    c.pureBackground   += k_offsetPurple;
    c.edge             += k_offsetEdge + k_offsetPurple;
    c.hotEdge          += k_offsetPurple;
    c.disabledEdge     += k_offsetPurple;
    return c;
}

static constexpr Colors makeCyanColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetCyan;
    c.softerBackground += k_offsetCyan;
    c.hotBackground    += k_offsetCyan;
    c.pureBackground   += k_offsetCyan;
    c.edge             += k_offsetEdge + k_offsetCyan;
    c.hotEdge          += k_offsetCyan;
    c.disabledEdge     += k_offsetCyan;
    return c;
}

static constexpr Colors makeOliveColors()
{
    Colors c = k_colorsBlack;
    c.background       += k_offsetOlive;
    c.softerBackground += k_offsetOlive;
    c.hotBackground    += k_offsetOlive;
    c.pureBackground   += k_offsetOlive;
    c.edge             += k_offsetEdge + k_offsetOlive;
    c.hotEdge          += k_offsetOlive;
    c.disabledEdge     += k_offsetOlive;
    return c;
}

// Singleton instance
static NppDarkMode* s_instance = nullptr;

// DarkScrollBarProxyStyle implementation (class declared in NppDarkMode.h)
DarkScrollBarProxyStyle::DarkScrollBarProxyStyle(QStyle* base)
    : QProxyStyle(base)
{}

int DarkScrollBarProxyStyle::styleHint(StyleHint hint, const QStyleOption* option,
                                        const QWidget* widget,
                                        QStyleHintReturn* returnData) const
{
    if (hint == QStyle::SH_ScrollBar_ContextMenu)
        return 0;
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

// =============================================================================
// NppDarkMode
// =============================================================================

NppDarkMode::NppDarkMode()
{
    _colors = k_colorsBlack;
    s_instance = this;
}

NppDarkMode::~NppDarkMode()
{
    s_instance = nullptr;
}

NppDarkMode& NppDarkMode::instance()
{
    if (!s_instance)
        s_instance = new NppDarkMode();
    return *s_instance;
}

// ── Windows-specific dark mode helpers (no-ops on Linux) ─────────────────────

bool NppDarkMode::isExperimentalSupported()
{
    return false;  // Windows 10 dark mode explorer experimental flag — not applicable on Linux
}

void NppDarkMode::enableDarkScrollBarForWindowAndChildren(QWidget* w)
{
    // Windows: EnableMenuShadow / dark scrollbar for taskbar.  No-op on Linux.
    if (w) {
        instance().setDarkScrollBar(w);
        instance().applyToChildWidgets(w);
    }
}

void NppDarkMode::setDarkTitleBar(QWidget*)
{
    // Windows: setDarkTitleBar via DwmSetWindowAttribute.  N/A on Linux.
}

void NppDarkMode::autoSubclassWindowMenuBar(QWidget*)
{
    // Windows: subclass menu bar for dark mode.  N/A on Linux.
}

void NppDarkMode::autoSubclassCtlColor(QWidget*)
{
    // Windows: WM_CTLCOLOR* subclassing for dark mode.  N/A on Linux.
}

// ── Enable ─────────────────────────────────────────────────────────────────

bool NppDarkMode::isEnabled() const
{
    return _options.enable;
}

bool NppDarkMode::isEnabledForPlugins() const
{
    return _options.enablePlugin;
}

void NppDarkMode::setEnabled(bool on)
{
    if (_options.enable == on)
        return;
    _options.enable = on;
    _brushesValid = false;
    _pensValid = false;
    Q_EMIT darkModeChanged(on);
}

void NppDarkMode::setEnabledForPlugins(bool on)
{
    _options.enablePlugin = on;
}

// ── Init ─────────────────────────────────────────────────────────────────────

void NppDarkMode::initFromSettings()
{
    // In a full lift this would read from NppSettings / Preference
    // For now use sensible defaults
    setEnabled(false);
    setEnabledForPlugins(false);
}

// ── Colour palette ───────────────────────────────────────────────────────────

Colors NppDarkMode::colors() const
{
    return _colors;
}

void NppDarkMode::setColors(const Colors& c)
{
    _colors = c;
    _brushesValid = false;
    _pensValid = false;
    Q_EMIT colorsChanged();
}

QRgb NppDarkMode::backgroundColor()    const { return _colors.background; }
QRgb NppDarkMode::ctrlBackgroundColor() const { return _colors.softerBackground; }
QRgb NppDarkMode::hotBackgroundColor()  const { return _colors.hotBackground; }
QRgb NppDarkMode::pureBackgroundColor() const { return _colors.pureBackground; }
QRgb NppDarkMode::errorBackgroundColor() const { return _colors.errorBackground; }
QRgb NppDarkMode::textColor()           const { return _colors.text; }
QRgb NppDarkMode::darkerTextColor()     const { return _colors.darkerText; }
QRgb NppDarkMode::disabledTextColor()   const { return _colors.disabledText; }
QRgb NppDarkMode::linkTextColor()       const { return _colors.linkText; }
QRgb NppDarkMode::edgeColor()           const { return _colors.edge; }
QRgb NppDarkMode::hotEdgeColor()        const { return _colors.hotEdge; }
QRgb NppDarkMode::disabledEdgeColor()   const { return _colors.disabledEdge; }

void NppDarkMode::rebuildBrushes()
{
    _brushBg        = QBrush(QColor::fromRgb(_colors.background));
    _brushCtrlBg    = QBrush(QColor::fromRgb(_colors.softerBackground));
    _brushHotBg     = QBrush(QColor::fromRgb(_colors.hotBackground));
    _brushPureBg    = QBrush(QColor::fromRgb(_colors.pureBackground));
    _brushErrorBg   = QBrush(QColor::fromRgb(_colors.errorBackground));
    _brushEdge      = QBrush(QColor::fromRgb(_colors.edge));
    _brushHotEdge   = QBrush(QColor::fromRgb(_colors.hotEdge));
    _brushDisabledEdge = QBrush(QColor::fromRgb(_colors.disabledEdge));
    _brushesValid = true;
}

void NppDarkMode::rebuildPens()
{
    _penEdge           = QPen(QColor::fromRgb(_colors.edge), 1);
    _penHotEdge        = QPen(QColor::fromRgb(_colors.hotEdge), 1);
    _penDisabledEdge   = QPen(QColor::fromRgb(_colors.disabledEdge), 1);
    _penDarkerText     = QPen(QColor::fromRgb(_colors.darkerText), 1);
    _pensValid = true;
}

void NppDarkMode::emitChanged()
{
    _brushesValid = false;
    _pensValid = false;
    Q_EMIT colorsChanged();
}

// ── Brushes ──────────────────────────────────────────────────────────────────

QBrush NppDarkMode::backgroundBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushBg;
}

QBrush NppDarkMode::ctrlBackgroundBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushCtrlBg;
}

QBrush NppDarkMode::hotBackgroundBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushHotBg;
}

QBrush NppDarkMode::pureBackgroundBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushPureBg;
}

QBrush NppDarkMode::errorBackgroundBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushErrorBg;
}

QBrush NppDarkMode::edgeBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushEdge;
}

QBrush NppDarkMode::hotEdgeBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushHotEdge;
}

QBrush NppDarkMode::disabledEdgeBrush() const
{
    if (!_brushesValid) const_cast<NppDarkMode*>(this)->rebuildBrushes();
    return _brushDisabledEdge;
}

// ── Pens ──────────────────────────────────────────────────────────────────────

QPen NppDarkMode::edgePen() const
{
    if (!_pensValid) const_cast<NppDarkMode*>(this)->rebuildPens();
    return _penEdge;
}

QPen NppDarkMode::hotEdgePen() const
{
    if (!_pensValid) const_cast<NppDarkMode*>(this)->rebuildPens();
    return _penHotEdge;
}

QPen NppDarkMode::disabledEdgePen() const
{
    if (!_pensValid) const_cast<NppDarkMode*>(this)->rebuildPens();
    return _penDisabledEdge;
}

QPen NppDarkMode::darkerTextPen() const
{
    if (!_pensValid) const_cast<NppDarkMode*>(this)->rebuildPens();
    return _penDarkerText;
}

// ── Colour setters ────────────────────────────────────────────────────────────

void NppDarkMode::setBackgroundColor(QRgb c)
{
    if (_colors.background == c) return;
    _colors.background = c;
    emitChanged();
}

void NppDarkMode::setCtrlBackgroundColor(QRgb c)
{
    if (_colors.softerBackground == c) return;
    _colors.softerBackground = c;
    emitChanged();
}

void NppDarkMode::setHotBackgroundColor(QRgb c)
{
    if (_colors.hotBackground == c) return;
    _colors.hotBackground = c;
    emitChanged();
}

void NppDarkMode::setPureBackgroundColor(QRgb c)
{
    if (_colors.pureBackground == c) return;
    _colors.pureBackground = c;
    emitChanged();
}

void NppDarkMode::setErrorBackgroundColor(QRgb c)
{
    if (_colors.errorBackground == c) return;
    _colors.errorBackground = c;
    emitChanged();
}

void NppDarkMode::setTextColor(QRgb c)
{
    if (_colors.text == c) return;
    _colors.text = c;
    emitChanged();
}

void NppDarkMode::setDarkerTextColor(QRgb c)
{
    if (_colors.darkerText == c) return;
    _colors.darkerText = c;
    emitChanged();
}

void NppDarkMode::setDisabledTextColor(QRgb c)
{
    if (_colors.disabledText == c) return;
    _colors.disabledText = c;
    emitChanged();
}

void NppDarkMode::setLinkTextColor(QRgb c)
{
    if (_colors.linkText == c) return;
    _colors.linkText = c;
    emitChanged();
}

void NppDarkMode::setEdgeColor(QRgb c)
{
    if (_colors.edge == c) return;
    _colors.edge = c;
    emitChanged();
}

void NppDarkMode::setHotEdgeColor(QRgb c)
{
    if (_colors.hotEdge == c) return;
    _colors.hotEdge = c;
    emitChanged();
}

void NppDarkMode::setDisabledEdgeColor(QRgb c)
{
    if (_colors.disabledEdge == c) return;
    _colors.disabledEdge = c;
    emitChanged();
}

// ── Accent colour ─────────────────────────────────────────────────────────────

QRgb NppDarkMode::accentColor() const
{
    if (_accentSource == AccentSource::custom)
        return _accent;
#ifdef _WIN32
    // Try to read Windows accent color (requires platform code)
    // For cross-platform, default to blue accent
    return hexRgb(0x0078D7);
#else
    return hexRgb(0x0078D7);
#endif
}

void NppDarkMode::setAccentColor(QRgb c)
{
    if (_accent == c && _accentSource == AccentSource::custom)
        return;
    _accent = c;
    _accentSource = AccentSource::custom;
    Q_EMIT accentColorChanged(c);
}

void NppDarkMode::setAccentSource(AccentSource src)
{
    if (_accentSource == src)
        return;
    _accentSource = src;
    Q_EMIT accentColorChanged(accentColor());
}

// ── Tone ─────────────────────────────────────────────────────────────────────

void NppDarkMode::setTone(ColorTone tone)
{
    if (_tone == tone)
        return;
    _tone = tone;
    setColors(defaultColors(tone));
}

ColorTone NppDarkMode::tone() const
{
    return _tone;
}

Colors NppDarkMode::getDarkModeDefaultColors(ColorTone colorTone)
{
    // Delegate to static default colors (instance-free stub for init)
    switch (colorTone) {
        case ColorTone::redTone:    return {};
        case ColorTone::greenTone:  return {};
        case ColorTone::blueTone:   return {};
        case ColorTone::purpleTone: return {};
        case ColorTone::cyanTone:   return {};
        case ColorTone::oliveTone:  return {};
        case ColorTone::blackTone:
        default:                    return {};
    }
}

Colors NppDarkMode::defaultColors(ColorTone tone) const
{
    switch (tone)
    {
        case ColorTone::redTone:      return makeRedColors();
        case ColorTone::greenTone:    return makeGreenColors();
        case ColorTone::blueTone:     return makeBlueColors();
        case ColorTone::purpleTone:   return makePurpleColors();
        case ColorTone::cyanTone:     return makeCyanColors();
        case ColorTone::oliveTone:    return makeOliveColors();
        case ColorTone::blackTone:
        default:                      return k_colorsBlack;
    }
}

// ── Tree-view style ──────────────────────────────────────────────────────────

// Perceived lightness: ITU-R BT.709 luminance coefficients
static double perceivedLightness(QRgb c)
{
    auto linearChannel = [](double v) -> double {
        v /= 255.0;
        return (v <= 0.04045) ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4);
    };
    double r = linearChannel(qRed(c));
    double g = linearChannel(qGreen(c));
    double b = linearChannel(qBlue(c));
    double luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    return (luminance <= 216.0 / 24389.0)
        ? (luminance * 24389.0 / 27.0)
        : (std::pow(luminance, 1.0 / 3.0) * 116.0 - 16.0);
}

void NppDarkMode::calculateTreeViewStyle(QRgb bgColor)
{
    double lightness = perceivedLightness(bgColor);
    if (lightness < 50.0 - 2.0)
        setTone(ColorTone::blackTone); // dark bg → dark theme
    else if (lightness > 50.0 + 2.0)
        ; // light bg → light theme (default)
}

TreeViewStyle NppDarkMode::treeViewStyle() const
{
    // Heuristic based on bg lightness
    double lightness = perceivedLightness(_colors.background);
    if (lightness < 50.0 - 2.0)
        return TreeViewStyle::dark;
    if (lightness > 50.0 + 2.0)
        return TreeViewStyle::light;
    return TreeViewStyle::classic;
}

void NppDarkMode::setTreeViewStyle(QWidget* treeView, TreeViewStyle style)
{
    if (!treeView)
        return;
    // In Qt, style is applied via the widget's palette.
    // For "dark" style we use dark palette, for "light" the system palette.
    if (style == TreeViewStyle::dark)
    {
        setDarkScrollBar(treeView);
    }
    else
    {
        setLightScrollBar(treeView);
    }
}

// ── Qt palette ────────────────────────────────────────────────────────────────

QPalette NppDarkMode::palette() const
{
    QPalette pal;
    if (!_options.enable)
        return pal; // return null/empty palette → use default

    pal.setColor(QPalette::Window,          QColor::fromRgb(_colors.background));
    pal.setColor(QPalette::WindowText,      QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Base,            QColor::fromRgb(_colors.pureBackground));
    pal.setColor(QPalette::AlternateBase,   QColor::fromRgb(_colors.softerBackground));
    pal.setColor(QPalette::ToolTipBase,     QColor::fromRgb(_colors.pureBackground));
    pal.setColor(QPalette::ToolTipText,     QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Text,            QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Button,          QColor::fromRgb(_colors.softerBackground));
    pal.setColor(QPalette::ButtonText,      QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::BrightText,      Qt::white);
    pal.setColor(QPalette::Highlight,       QColor::fromRgb(_colors.hotBackground));
    pal.setColor(QPalette::HighlightedText, QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Link,           QColor::fromRgb(_colors.linkText));
    pal.setColor(QPalette::LinkVisited,    QColor::fromRgb(_colors.darkerText));
    pal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor::fromRgb(_colors.disabledText));
    pal.setColor(QPalette::Disabled, QPalette::Text,        QColor::fromRgb(_colors.disabledText));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,  QColor::fromRgb(_colors.disabledText));
    pal.setColor(QPalette::Disabled, QPalette::Light,       QColor::fromRgb(_colors.disabledEdge));
    pal.setColor(QPalette::Disabled, QPalette::Midlight,    QColor::fromRgb(_colors.disabledEdge));
    pal.setColor(QPalette::Disabled, QPalette::Dark,        QColor::fromRgb(_colors.disabledEdge));
    pal.setColor(QPalette::Disabled, QPalette::Mid,         QColor::fromRgb(_colors.disabledEdge));
    pal.setColor(QPalette::Disabled, QPalette::Shadow,      QColor::fromRgb(_colors.disabledEdge));
    return pal;
}

void NppDarkMode::applyToWidget(QWidget* w) const
{
    if (!w || !_options.enable)
        return;
    w->setPalette(palette());
}

void NppDarkMode::applyToChildWidgets(QWidget* parent) const
{
    if (!parent || !_options.enable)
        return;
    parent->setPalette(palette());
    for (QObject* child : parent->children())
    {
        if (auto* w = qobject_cast<QWidget*>(child))
            w->setPalette(palette());
    }
}

// ── Control theming ───────────────────────────────────────────────────────────

void NppDarkMode::subclassButton(QAbstractButton* btn)
{
    if (!btn || !_options.enable)
        return;
    btn->setPalette(palette());
}

void NppDarkMode::subclassGroupBox(QGroupBox* gb)
{
    if (!gb || !_options.enable)
        return;
    gb->setPalette(palette());
}

void NppDarkMode::subclassTabWidget(QTabWidget* tw)
{
    if (!tw || !_options.enable)
        return;
    tw->setPalette(palette());
    // Tab bar background
    if (tw->tabBar())
        tw->tabBar()->setPalette(palette());
}

void NppDarkMode::subclassComboBox(QComboBox* cb)
{
    if (!cb || !_options.enable)
        return;
    cb->setPalette(palette());
}

void NppDarkMode::subclassLineEdit(QLineEdit* le)
{
    if (!le || !_options.enable)
        return;
    le->setPalette(palette());
}

void NppDarkMode::subclassTextEdit(QTextEdit* te)
{
    if (!te || !_options.enable)
        return;
    te->setPalette(palette());
}

void NppDarkMode::subclassListWidget(QListWidget* lw)
{
    if (!lw || !_options.enable)
        return;
    lw->setPalette(palette());
    setDarkScrollBar(lw);
}

void NppDarkMode::subclassTreeWidget(QTreeWidget* tw)
{
    if (!tw || !_options.enable)
        return;
    tw->setPalette(palette());
    setDarkScrollBar(tw);
}

void NppDarkMode::subclassTableWidget(QTableWidget* tw)
{
    if (!tw || !_options.enable)
        return;
    tw->setPalette(palette());
    setDarkScrollBar(tw);
    if (tw->horizontalHeader())
        tw->horizontalHeader()->setPalette(palette());
    if (tw->verticalHeader())
        tw->verticalHeader()->setPalette(palette());
}

void NppDarkMode::subclassProgressBar(QProgressBar* pb)
{
    if (!pb || !_options.enable)
        return;
    pb->setPalette(palette());
}

// ── Scroll bar theming ────────────────────────────────────────────────────────

void NppDarkMode::setDarkScrollBar(QWidget* w)
{
    if (!w)
        return;
    if (auto* av = qobject_cast<QAbstractItemView*>(w))
    {
        av->setStyleSheet(
            "QScrollBar:vertical { background: #2a2a2a; width: 14px; }"
            "QScrollBar::handle:vertical { background: #4a4a4a; min-height: 20px; border-radius: 6px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
            "QScrollBar:horizontal { background: #2a2a2a; height: 14px; }"
            "QScrollBar::handle:horizontal { background: #4a4a4a; min-width: 20px; border-radius: 6px; }"
            "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
        );
    }
    else
    {
        w->setStyleSheet(
            "QScrollBar:vertical { background: #2a2a2a; width: 14px; }"
            "QScrollBar::handle:vertical { background: #4a4a4a; min-height: 20px; border-radius: 6px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
            "QScrollBar:horizontal { background: #2a2a2a; height: 14px; }"
            "QScrollBar::handle:horizontal { background: #4a4a4a; min-width: 20px; border-radius: 6px; }"
            "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
        );
    }
}

void NppDarkMode::setLightScrollBar(QWidget* w)
{
    if (!w)
        return;
    w->setStyleSheet(QString());
}

// ── Title bar ─────────────────────────────────────────────────────────────────

void NppDarkMode::setTitleBarDark(QWidget* w)
{
    Q_UNUSED(w);
    // On Linux/X11 this is a no-op; title bar colour is handled by the window manager
}

void NppDarkMode::setTitleBarLight(QWidget* w)
{
    Q_UNUSED(w);
}

} // namespace NppDarkMode

