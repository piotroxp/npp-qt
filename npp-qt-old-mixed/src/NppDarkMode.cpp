// Semantic lift: NppDarkMode.cpp — Win32 dark mode → Qt6 palette controller
// Original: PowerEditor/src/NppDarkMode.cpp (4542 lines, adzm / GPLv3)
// Target: npp-qt/src/NppDarkMode.cpp
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3

#include "NppDarkMode.h"
#include "Preference.h"
#include "WinControls/ToolBar.h"
#include "WinControls/ImageListSet.h"

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
#include <QGuiApplication>
#include <QStyleHints>

#ifdef _WIN32
// windows.h removed
#endif

// All implementation is inside NppDarkMode namespace (header is also inside)
// This mirrors the header layout so moc sees Q_OBJECT in one pass.

namespace NppDarkMode {

// Singleton instance
static NppDarkMode* s_instance = nullptr;

// DarkScrollBarProxyStyle
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

// ── NppDarkMode singleton ──────────────────────────────────────────────────

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

bool NppDarkMode::isExperimentalSupported()
{
    return false;
}

QRgb NppDarkMode::getDarkerTextColor()
{
    return instance().darkerTextColor();
}

void NppDarkMode::enableDarkScrollBarForWindowAndChildren(QWidget* w)
{
    if (w) {
        instance().setDarkScrollBar(w);
        instance().applyToChildWidgets(w);
    }
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

bool NppDarkMode::isWindowsModeEnabled()
{
    return false;
}

void NppDarkMode::setDarkTitleBar(QWidget* w)
{
    // Linux/X11: The window manager controls the native title bar color.
    // We can only darken the widget's content area below the title bar.
    if (!w || !_options.enable)
        return;
    // Apply dark palette to the widget itself (client area below title bar)
    w->setPalette(palette());
}

void NppDarkMode::setDarkTitleBar(QWidget* w, bool dark)
{
    // Linux/X11: The native title bar is controlled by the window manager.
    // We can only apply dark styling to the widget's client area.
    if (!w)
        return;
    if (dark && _options.enable) {
        w->setPalette(palette());
    } else {
        w->setPalette(QPalette());  // restore default palette (Qt6 has no unsetPalette())
    }
}

void NppDarkMode::allowDarkModeForApp(bool)
{
    // Linux: Qt6 color scheme preference API (setColorSchemePreference) is not
    // reliably available on all distributions. Dark mode is applied directly
    // via qApp->setPalette(palette()) — the Qt platform integration handles it.
    // No-op on Linux; dark palette is applied unconditionally when enabled.
}

void NppDarkMode::autoThemeChildControls(QWidget* w)
{
    // Linux: Recursively apply dark palette to all child widgets
    if (!w)
        return;
    applyToWidget(w);
    applyToChildWidgets(w);

    // Also theme specific control types with custom styles
    subclassComboBox(qobject_cast<QComboBox*>(w));
    subclassListWidget(qobject_cast<QListWidget*>(w));
    subclassTreeWidget(qobject_cast<QTreeWidget*>(w));
    subclassLineEdit(qobject_cast<QLineEdit*>(w));
    subclassTextEdit(qobject_cast<QTextEdit*>(w));
    subclassTableWidget(qobject_cast<QTableWidget*>(w));
    subclassButton(qobject_cast<QAbstractButton*>(w));
    subclassTabWidget(qobject_cast<QTabWidget*>(w));
    subclassGroupBox(qobject_cast<QGroupBox*>(w));
}

QString NppDarkMode::getThemeName()
{
    // Linux: Query the active Qt platform theme or X11 color scheme
#ifdef Q_OS_LINUX
    // Check Qt's reported color scheme first
    if (QGuiApplication::styleHints()) {
        Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
        if (scheme == Qt::ColorScheme::Dark)
            return QStringLiteral("dark");
        if (scheme == Qt::ColorScheme::Light)
            return QStringLiteral("light");
    }
    // Fallback: check XDG current theme or Qt platform theme
    QByteArray xdgTheme = qgetenv("XDG_CURRENT_DESKTOP");
    if (!xdgTheme.isEmpty()) {
        return QString::fromLatin1(xdgTheme.constData());
    }
    // Fallback: check Qt platform theme name (works for GTK/KDE on XCB)
    QByteArray qtTheme = qgetenv("QT_QPA_PLATFORMTHEME");
    if (!qtTheme.isEmpty()) {
        return QString::fromLatin1(qtTheme.constData());
    }
#endif
    return QString();
}

QString NppDarkMode::getTabIcon()
{
    return QString();
}

void NppDarkMode::setEnabled(bool on)
{
    if (_options.enable == on)
        return;
    _options.enable = on;
    _brushesValid = false;
    _pensValid = false;

    // ── Linux/Qt6 fix: propagate palette application-wide ──────────────────
    // On Linux, Qt widgets do not automatically inherit from child widget
    // palettes the way they do on Windows.  We must call qApp->setPalette()
    // to force the dark palette onto all widgets that inherit from
    // QPalette::Window / QPalette::Base.
    if (on) {
        qApp->setPalette(palette());
        allowDarkModeForApp(true);
    } else {
        qApp->setPalette(QPalette());  // restore default palette (Qt6 has no unsetPalette())
        allowDarkModeForApp(false);
    }
    // ── end Linux fix ─────────────────────────────────────────────────────

    // Emit darkModeChanged so listeners (including Notepad_plus) can
    // refresh their widgets and notify plugins via NPPN_DARKMODECHANGED
    Q_EMIT darkModeChanged(on);
}

void NppDarkMode::setEnabledForPlugins(bool on)
{
    _options.enablePlugin = on;
}

void NppDarkMode::initFromSettings()
{
    setEnabled(false);
    setEnabledForPlugins(false);
}

// ── Colour palette ─────────────────────────────────────────────────────────

NppDarkModeColors NppDarkMode::colors() const
{
    return _colors;
}

void NppDarkMode::setColors(const NppDarkModeColors& c)
{
    _colors = c;
    _brushesValid = false;
    _pensValid = false;
    Q_EMIT colorsChanged();
}

QRgb NppDarkMode::backgroundColor()     const { return _colors.background; }
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
    _brushBg          = QBrush(QColor::fromRgb(_colors.background));
    _brushCtrlBg       = QBrush(QColor::fromRgb(_colors.softerBackground));
    _brushHotBg        = QBrush(QColor::fromRgb(_colors.hotBackground));
    _brushPureBg       = QBrush(QColor::fromRgb(_colors.pureBackground));
    _brushErrorBg      = QBrush(QColor::fromRgb(_colors.errorBackground));
    _brushEdge         = QBrush(QColor::fromRgb(_colors.edge));
    _brushHotEdge      = QBrush(QColor::fromRgb(_colors.hotEdge));
    _brushDisabledEdge = QBrush(QColor::fromRgb(_colors.disabledEdge));
    _brushesValid = true;
}

void NppDarkMode::rebuildPens()
{
    _penEdge         = QPen(QColor::fromRgb(_colors.edge), 1);
    _penHotEdge      = QPen(QColor::fromRgb(_colors.hotEdge), 1);
    _penDisabledEdge = QPen(QColor::fromRgb(_colors.disabledEdge), 1);
    _penDarkerText  = QPen(QColor::fromRgb(_colors.darkerText), 1);
    _pensValid = true;
}

void NppDarkMode::emitChanged()
{
    _brushesValid = false;
    _pensValid = false;
    Q_EMIT colorsChanged();
}

// ── Brushes ────────────────────────────────────────────────────────────────

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

// ── Pens ─────────────────────────────────────────────────────────────────

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

// ── Colour setters ────────────────────────────────────────────────────────

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

// ── Accent colour ─────────────────────────────────────────────────────────

QRgb NppDarkMode::accentColor() const
{
    if (_accentSource == NppDarkModeAccentSource::custom)
        return _accent;
    return hexRgb(0x0078D7);
}

void NppDarkMode::setAccentColor(QRgb c)
{
    if (_accent == c && _accentSource == NppDarkModeAccentSource::custom)
        return;
    _accent = c;
    _accentSource = NppDarkModeAccentSource::custom;
    Q_EMIT accentColorChanged(c);
}

void NppDarkMode::setAccentSource(NppDarkModeAccentSource src)
{
    if (_accentSource == src)
        return;
    _accentSource = src;
    Q_EMIT accentColorChanged(accentColor());
}

// ── Tone ─────────────────────────────────────────────────────────────────

void NppDarkMode::setTone(NppDarkModeColorTone tone)
{
    if (_tone == tone)
        return;
    _tone = tone;
    setColors(defaultColors(tone));
}

NppDarkModeColorTone NppDarkMode::tone() const
{
    return _tone;
}

NppDarkModeColors NppDarkMode::getDarkModeDefaultColors(NppDarkModeColorTone colorTone)
{
    switch (colorTone) {
        case NppDarkModeColorTone::redTone:    return makeRedColors();
        case NppDarkModeColorTone::greenTone:  return makeGreenColors();
        case NppDarkModeColorTone::blueTone:   return makeBlueColors();
        case NppDarkModeColorTone::purpleTone: return makePurpleColors();
        case NppDarkModeColorTone::cyanTone:   return makeCyanColors();
        case NppDarkModeColorTone::oliveTone:  return makeOliveColors();
        case NppDarkModeColorTone::blackTone:
        default:                               return k_colorsBlack;
    }
}

NppDarkModeColors NppDarkMode::defaultColors(NppDarkModeColorTone tone) const
{
    switch (tone) {
        case NppDarkModeColorTone::redTone:    return makeRedColors();
        case NppDarkModeColorTone::greenTone:  return makeGreenColors();
        case NppDarkModeColorTone::blueTone:   return makeBlueColors();
        case NppDarkModeColorTone::purpleTone: return makePurpleColors();
        case NppDarkModeColorTone::cyanTone:   return makeCyanColors();
        case NppDarkModeColorTone::oliveTone:  return makeOliveColors();
        case NppDarkModeColorTone::blackTone:
        default:                               return k_colorsBlack;
    }
}

// ── Tree-view style ──────────────────────────────────────────────────────

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
    if (lightness < 50.0 - 2.0) {
        _treeViewStyle = NppDarkModeTreeViewStyle::dark;
        setTone(NppDarkModeColorTone::blackTone);
    } else if (lightness > 50.0 + 2.0) {
        _treeViewStyle = NppDarkModeTreeViewStyle::light;
    } else {
        _treeViewStyle = NppDarkModeTreeViewStyle::classic;
    }
}

NppDarkModeTreeViewStyle NppDarkMode::treeViewStyle() const
{
    return _treeViewStyle;
}

void NppDarkMode::setTreeViewStyle(QWidget* treeView, NppDarkModeTreeViewStyle style)
{
    if (!treeView)
        return;
    if (style == NppDarkModeTreeViewStyle::dark)
        setDarkScrollBar(treeView);
    else
        setLightScrollBar(treeView);
}

// ── Qt palette ────────────────────────────────────────────────────────────

QPalette NppDarkMode::palette() const
{
    QPalette pal;
    if (!_options.enable) {
        for (int role = 0; role < QPalette::NColorRoles; ++role) {
            QColor invalid;
            pal.setColor(static_cast<QPalette::ColorRole>(role), invalid);
        }
        return pal;
    }
    pal.setColor(QPalette::Window,         QColor::fromRgb(_colors.background));
    pal.setColor(QPalette::WindowText,     QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Base,           QColor::fromRgb(_colors.pureBackground));
    pal.setColor(QPalette::AlternateBase,  QColor::fromRgb(_colors.softerBackground));
    pal.setColor(QPalette::ToolTipBase,    QColor::fromRgb(_colors.pureBackground));
    pal.setColor(QPalette::ToolTipText,    QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Text,           QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Button,         QColor::fromRgb(_colors.softerBackground));
    pal.setColor(QPalette::ButtonText,     QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::BrightText,     Qt::white);
    pal.setColor(QPalette::Highlight,      QColor::fromRgb(_colors.hotBackground));
    pal.setColor(QPalette::HighlightedText, QColor::fromRgb(_colors.text));
    pal.setColor(QPalette::Link,          QColor::fromRgb(_colors.linkText));
    pal.setColor(QPalette::LinkVisited,   QColor::fromRgb(_colors.darkerText));
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
    if (!parent)
        return;
    const auto children = parent->children();
    for (QObject* child : children) {
        QWidget* w = qobject_cast<QWidget*>(child);
        if (w)
            applyToWidget(w);
    }
}

// ── Control theming ───────────────────────────────────────────────────────

void NppDarkMode::subclassButton(QAbstractButton* btn)
{
    if (!btn)
        return;
    btn->setStyle(new DarkScrollBarProxyStyle(btn->style()));
}

void NppDarkMode::subclassGroupBox(QGroupBox* gb)
{
    if (!gb)
        return;
    gb->setStyle(new DarkScrollBarProxyStyle(gb->style()));
}

void NppDarkMode::subclassTabWidget(QTabWidget* tw)
{
    if (!tw)
        return;
    tw->setStyle(new DarkScrollBarProxyStyle(tw->style()));
}

void NppDarkMode::subclassComboBox(QComboBox* cb)
{
    if (!cb)
        return;
    cb->setStyle(new DarkScrollBarProxyStyle(cb->style()));
}

void NppDarkMode::subclassListWidget(QListWidget* lw)
{
    if (!lw)
        return;
    lw->setStyle(new DarkScrollBarProxyStyle(lw->style()));
}

void NppDarkMode::subclassTreeWidget(QTreeWidget* tw)
{
    if (!tw)
        return;
    tw->setStyle(new DarkScrollBarProxyStyle(tw->style()));
}

void NppDarkMode::subclassLineEdit(QLineEdit* le)
{
    if (!le)
        return;
    le->setStyle(new DarkScrollBarProxyStyle(le->style()));
}

void NppDarkMode::subclassTextEdit(QTextEdit* te)
{
    if (!te)
        return;
    te->setStyle(new DarkScrollBarProxyStyle(te->style()));
}

void NppDarkMode::subclassTableWidget(QTableWidget* tw)
{
    if (!tw)
        return;
    tw->setStyle(new DarkScrollBarProxyStyle(tw->style()));
}

void NppDarkMode::subclassProgressBar(QProgressBar* pb)
{
    if (!pb)
        return;
    pb->setStyle(new DarkScrollBarProxyStyle(pb->style()));
}

// ── Scroll bar ───────────────────────────────────────────────────────────

void NppDarkMode::setDarkScrollBar(QWidget* w)
{
    if (!w)
        return;
    w->setStyleSheet(
        "QScrollBar:vertical { background: #2a2a2a; width: 14px; }"
        "QScrollBar::handle:vertical { background: #4a4a4a; min-height: 20px; border-radius: 6px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar:horizontal { background: #2a2a2a; height: 14px; }"
        "QScrollBar::handle:horizontal { background: #4a4a4a; min-width: 20px; border-radius: 6px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
    );
}

void NppDarkMode::setLightScrollBar(QWidget* w)
{
    if (!w)
        return;
    w->setStyleSheet(QString());
}

// ── Title bar ────────────────────────────────────────────────────────────

void NppDarkMode::setTitleBarDark(QWidget* w)
{
    Q_UNUSED(w);
    // On Linux/X11 title bar colour is handled by the window manager
}

void NppDarkMode::setTitleBarLight(QWidget* w)
{
    Q_UNUSED(w);
}

int NppDarkMode::getTabIconSet(bool /*isDarkMode*/)
{
    return 0;  // No alternate icon set on Linux
}

NppDarkModeTbIconInfo NppDarkMode::getToolbarIconInfo()
{
    NppDarkModeTbIconInfo info;
    info._tbIconSet  = instance().isEnabled() ? ToolBarDisplayType::TB_SMALL : ToolBarDisplayType::TB_STANDARD;
    info._tbColor    = FluentColor::defaultColor;
    info._tbCustomColor = 0;
    info._tbUseMono  = false;
    return info;
}

} // namespace NppDarkMode
