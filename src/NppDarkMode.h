// Semantic lift: NppDarkMode.h — Win32 dark mode theming → Qt6 dark mode palette
// Original: PowerEditor/src/NppDarkMode.h + .cpp (4542 lines)
// Target: npp-qt/src/NppDarkMode.h + .cpp
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3

#pragma once

// =============================================================================
// CRITICAL: All includes must be at GLOBAL scope (outside namespace NppDarkMode).
// Including Qt/system headers inside "namespace NppDarkMode { }" causes Qt symbols
// to become NppDarkMode::QToolBar, NppDarkMode::QIcon, etc.,
// which breaks class NppDarkMode : public QToolBar { } — circular inheritance.
// Fix: includes above, open namespace after.
// =============================================================================

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QPointer>
#include <QHash>
#include <QString>
#include <QAbstractButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QProgressBar>
#include <QProxyStyle>
#include <QAbstractItemView>
#include <QPaintDevice>

#include "WinControls/ToolBar.h"   // ToolBarStatusType, ToolBarIcons
#include "NppConstants.h"          // QRgb, qRgba
#include "WinControls/ImageListSet.h"  // FluentColor, IconList, DynamicIconCmd

// =============================================================================
// All types at GLOBAL scope (outside namespace).
// The class NppDarkMode lives inside namespace NppDarkMode, but the data
// structs and enums are at global scope so ::NppDarkModeColors etc. work.
// =============================================================================

struct NppDarkModeTbIconInfo
{
    ToolBarStatusType _tbIconSet = ToolBarStatusType::TB_STANDARD;
    FluentColor       _tbColor   = FluentColor::defaultColor;
    QRgb              _tbCustomColor = qRgba(0, 0, 0, 0);
    bool              _tbUseMono = false;
};

struct NppDarkModeColors
{
    QRgb background       = 0x303030;
    QRgb softerBackground = 0x383838;
    QRgb hotBackground    = 0x454545;
    QRgb pureBackground   = 0x202020;
    QRgb errorBackground  = 0xB00000;
    QRgb text             = 0xE0E0E0;
    QRgb darkerText       = 0xC0C0C0;
    QRgb disabledText     = 0x808080;
    QRgb linkText         = 0xFFFF00;
    QRgb edge             = 0x646464;
    QRgb hotEdge          = 0x9B9B9B;
    QRgb disabledEdge     = 0x484848;
};

struct NppDarkModeOptions
{
    bool enable = false;
    bool enablePlugin = false;
};

enum class NppDarkModeColorTone
{
    blackTone      = 0,
    redTone        = 1,
    greenTone      = 2,
    blueTone       = 3,
    purpleTone     = 4,
    cyanTone       = 5,
    oliveTone      = 6,
    customizedTone = 32
};

enum class NppDarkModeToolTipsType
{
    tooltip,
    toolbar,
    listview,
    treeview,
    tabbar
};

enum class NppDarkModeTreeViewStyle
{
    classic = 0,
    light   = 1,
    dark    = 2
};

enum class NppDarkModeAccentSource
{
    systemDefault,
    custom
};

// =============================================================================
// namespace NppDarkMode — contains class + convenience wrappers
// Types above are at global scope; namespace provides aliases + helpers
// =============================================================================
namespace NppDarkMode
{

// =============================================================================
// NppDarkMode — singleton palette + style controller
// =============================================================================

class NppDarkMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY darkModeChanged)

public:

    // Nested type aliases — allow NppDarkMode::TbIconInfo etc. from old nested layout
    using TbIconInfo       = ::NppDarkModeTbIconInfo;
    using Colors           = ::NppDarkModeColors;
    using Options          = ::NppDarkModeOptions;
    using ColorTone        = ::NppDarkModeColorTone;
    using ToolTipsType     = ::NppDarkModeToolTipsType;
    using TreeViewStyle    = ::NppDarkModeTreeViewStyle;
    using AccentSource     = ::NppDarkModeAccentSource;

    // ── Enable / query ────────────────────────────────────────────────────
    bool isEnabled() const;
    static bool isEnabled_Static() { return instance().isEnabled(); }
    static QBrush getDlgBackgroundBrush() { return instance().pureBackgroundBrush(); }
    bool isWindowsModeEnabled();
    bool isEnabledForPlugins() const;
    void setEnabled(bool on);
    void setEnabledForPlugins(bool on);
    static int getTabIconSet(bool isDarkMode);
    static QString getTabIcon();
    static NppDarkModeTbIconInfo getToolbarIconInfo();

    // ── Color palette ───────────────────────────────────────────────────────
    NppDarkModeColors colors() const;
    void setColors(const NppDarkModeColors& c);

    QRgb backgroundColor() const;
    QRgb ctrlBackgroundColor() const;
    QRgb hotBackgroundColor() const;
    QRgb pureBackgroundColor() const;
    QRgb errorBackgroundColor() const;
    QRgb textColor() const;
    QRgb darkerTextColor() const;
    QRgb disabledTextColor() const;
    QRgb linkTextColor() const;
    QRgb edgeColor() const;
    QRgb hotEdgeColor() const;
    QRgb disabledEdgeColor() const;

    QBrush backgroundBrush() const;
    QBrush ctrlBackgroundBrush() const;
    QBrush hotBackgroundBrush() const;
    QBrush pureBackgroundBrush() const;
    QBrush errorBackgroundBrush() const;

    QBrush edgeBrush() const;
    QBrush hotEdgeBrush() const;
    QBrush disabledEdgeBrush() const;

    QPen edgePen() const;
    QPen hotEdgePen() const;
    QPen disabledEdgePen() const;
    QPen darkerTextPen() const;

    // ── Color setters (live-update theme) ─────────────────────────────────
    void setBackgroundColor(QRgb c);
    void setCtrlBackgroundColor(QRgb c);
    void setHotBackgroundColor(QRgb c);
    void setPureBackgroundColor(QRgb c);
    void setErrorBackgroundColor(QRgb c);
    void setTextColor(QRgb c);
    void setDarkerTextColor(QRgb c);
    void setDisabledTextColor(QRgb c);
    void setLinkTextColor(QRgb c);
    void setEdgeColor(QRgb c);
    void setHotEdgeColor(QRgb c);
    void setDisabledEdgeColor(QRgb c);

    // ── Accent colour ──────────────────────────────────────────────────────
    QRgb accentColor() const;
    void setAccentColor(QRgb c);
    void setAccentSource(NppDarkModeAccentSource src);

    // ── Tone ────────────────────────────────────────────────────────────────
    void setTone(NppDarkModeColorTone tone);
    NppDarkModeColorTone tone() const;
    NppDarkModeColors defaultColors(NppDarkModeColorTone tone) const;
    static NppDarkModeColors getDarkModeDefaultColors(NppDarkModeColorTone colorTone = NppDarkModeColorTone::blackTone);

    // ── Tree-view style ────────────────────────────────────────────────────
    NppDarkModeTreeViewStyle treeViewStyle() const;
    void setTreeViewStyle(QWidget* treeView, NppDarkModeTreeViewStyle style);
    void calculateTreeViewStyle(QRgb bgColor);

    // ── Qt palette integration ─────────────────────────────────────────────
    QPalette palette() const;
    void applyToWidget(QWidget* w) const;
    void applyToChildWidgets(QWidget* parent) const;

    // ── Control theming helpers ───────────────────────────────────────────
    void subclassButton(QAbstractButton* btn);
    void subclassGroupBox(QGroupBox* gb);
    void subclassTabWidget(QTabWidget* tw);
    void subclassComboBox(QComboBox* cb);
    void subclassListWidget(QListWidget* lw);
    void subclassTreeWidget(QTreeWidget* tw);
    void subclassLineEdit(QLineEdit* le);
    void subclassTextEdit(QTextEdit* te);
    void subclassTableWidget(QTableWidget* tw);
    void subclassProgressBar(QProgressBar* pb);

    // ── Scroll bar theming ─────────────────────────────────────────────────
    void setDarkScrollBar(QWidget* w);
    void setLightScrollBar(QWidget* w);

    // ── Title bar ──────────────────────────────────────────────────────────
    void setTitleBarDark(QWidget* w);
    void setTitleBarLight(QWidget* w);

    // ── Init ───────────────────────────────────────────────────────────────
    void initFromSettings();

Q_SIGNALS:
    void darkModeChanged(bool enabled);
    void colorsChanged();
    void accentColorChanged(QRgb color);

private:
    NppDarkMode();
    ~NppDarkMode();
    NppDarkMode(const NppDarkMode&) = delete;
    NppDarkMode& operator=(const NppDarkMode&) = delete;

    void rebuildBrushes();
    void rebuildPens();
    void emitChanged();

    // ── Internal palette ──────────────────────────────────────────────────
    NppDarkModeColors _colors;
    NppDarkModeColorTone _tone = NppDarkModeColorTone::blackTone;
    NppDarkModeOptions _options;
    NppDarkModeAccentSource _accentSource = NppDarkModeAccentSource::systemDefault;
    QRgb _accent = 0;

    // Pre-built brushes (lazy)
    mutable QBrush _brushBg;
    mutable QBrush _brushCtrlBg;
    mutable QBrush _brushHotBg;
    mutable QBrush _brushPureBg;
    mutable QBrush _brushErrorBg;
    mutable QBrush _brushEdge;
    mutable QBrush _brushHotEdge;
    mutable QBrush _brushDisabledEdge;
    mutable bool _brushesValid = false;

    // Pre-built pens
    mutable QPen _penEdge;
    mutable QPen _penHotEdge;
    mutable QPen _penDisabledEdge;
    mutable QPen _penDarkerText;
    mutable bool _pensValid = false;

    // Cached tree-view style
    mutable NppDarkModeTreeViewStyle _treeViewStyle = NppDarkModeTreeViewStyle::classic;

public:
    // Exposed via second public section so MOC-generated members stay isolated
    static NppDarkMode& instance();

    // Dark mode feature flags (Win32-specific behaviour stubs for Linux)
    static bool isExperimentalSupported();
    static void enableDarkScrollBarForWindowAndChildren(QWidget*);
    static void setDarkTitleBar(QWidget*);
    static void setDarkTitleBar(QWidget* w, bool dark);
    static void autoSubclassWindowMenuBar(QWidget*);
    static void autoSubclassCtlColor(QWidget*);
    static QRgb getDarkerTextColor();
    static void allowDarkModeForApp(bool on);
    static void autoThemeChildControls(QWidget* w);
    static QString getThemeName();
};

// =============================================================================
// Convenience type aliases (so existing code NppDarkMode::Colors still works)
// These are INSIDE namespace so NppDarkMode::Colors = ::NppDarkModeColors
// =============================================================================

// ── Namespace-scope convenience wrappers (mirror class statics) ───────────
inline void enableDarkScrollBarForWindowAndChildren(QWidget* w)
    { NppDarkMode::enableDarkScrollBarForWindowAndChildren(w); }

using Colors       = ::NppDarkModeColors;
using Options      = ::NppDarkModeOptions;
using ColorTone    = ::NppDarkModeColorTone;
using ToolTipsType      = ::NppDarkModeToolTipsType;
using TreeViewStyle     = ::NppDarkModeTreeViewStyle;
using AccentSource      = ::NppDarkModeAccentSource;
using TbIconInfo       = ::NppDarkModeTbIconInfo;

// ── Namespace-scope inline wrappers for class static methods ─────────────────
// Allow callers that already use NppDarkMode::isEnabled_Static() etc. to
// continue without changes.
inline bool isEnabled_Static()             { return ::NppDarkMode::isEnabled_Static(); }
inline bool isExperimentalSupported()      { return ::NppDarkMode::isExperimentalSupported(); }
inline QBrush getDlgBackgroundBrush()     { return ::NppDarkMode::getDlgBackgroundBrush(); }
inline void setDarkTitleBar(QWidget* w)   { ::NppDarkMode::setDarkTitleBar(w); }
inline QRgb getDarkerTextColor()          { return ::NppDarkMode::getDarkerTextColor(); }
inline QString getThemeName()             { return ::NppDarkMode::getThemeName(); }
inline void allowDarkModeForApp(bool v)   { ::NppDarkMode::allowDarkModeForApp(v); }
inline int getTabIconSet(bool v)         { return ::NppDarkMode::getTabIconSet(v); }
inline NppDarkModeTbIconInfo getToolbarIconInfo() { return ::NppDarkMode::getToolbarIconInfo(); }
inline void autoThemeChildControls(QWidget* w) { ::NppDarkMode::autoThemeChildControls(w); }

// Delegate to the singleton
inline NppDarkMode& instance() { return ::NppDarkMode::instance(); }

// Helper: return a QRgb as-is. Callers in this codebase already use
// 0x00RRGGBB (fully-opaque ARGB) so no conversion is needed.
constexpr inline QRgb hexRgb(unsigned int rgb) { return rgb; }

// ── Convenience colours for the default dark theme ──────────────────────────
// inline so header + NppDarkMode.cpp can both define them (ODR-safe)
inline static constexpr QRgb k_darkBg            = 0x303030;
inline static constexpr QRgb k_darkCtrlBg        = 0x383838;
inline static constexpr QRgb k_darkHotBg         = 0x454545;
inline static constexpr QRgb k_darkPureBg        = 0x202020;
inline static constexpr QRgb k_darkErrorBg       = 0xB00000;
inline static constexpr QRgb k_darkText          = 0xE0E0E0;
inline static constexpr QRgb k_darkDarkerText    = 0xC0C0C0;
inline static constexpr QRgb k_darkDisabledText  = 0x808080;
inline static constexpr QRgb k_darkLink          = 0xFFFF00;
inline static constexpr QRgb k_darkEdge          = 0x646464;
inline static constexpr QRgb k_darkHotEdge       = 0x9B9B9B;
inline static constexpr QRgb k_darkDisabledEdge  = 0x484848;

// ── Default color tables (mirrors NppDarkMode.cpp namespace-scope defs) ─────
// Offset values for tone variants
inline static constexpr int k_offsetEdge    = 0x1C1C1C;
inline static constexpr int k_offsetRed     = 0x100000;
inline static constexpr int k_offsetGreen  = 0x001000;
inline static constexpr int k_offsetBlue   = 0x000020;
inline static constexpr int k_offsetPurple = 0x100020;
inline static constexpr int k_offsetCyan   = 0x001020;
inline static constexpr int k_offsetOlive  = 0x101000;

// k_colorsBlack — aggregate of k_dark* constants
inline static constexpr NppDarkModeColors k_colorsBlack = {
    k_darkBg, k_darkCtrlBg, k_darkHotBg, k_darkPureBg, k_darkErrorBg,
    k_darkText, k_darkDarkerText, k_darkDisabledText, k_darkLink,
    k_darkEdge, k_darkHotEdge, k_darkDisabledEdge
};

// Inline helpers for tone color tables (constexpr so usable as constant expressions)
inline constexpr NppDarkModeColors makeRedColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetRed;
    c.softerBackground += k_offsetRed;
    c.hotBackground    += k_offsetRed;
    c.pureBackground   += k_offsetRed;
    c.edge             += k_offsetEdge + k_offsetRed;
    c.hotEdge          += k_offsetRed;
    c.disabledEdge     += k_offsetRed;
    return c;
}
inline constexpr NppDarkModeColors makeGreenColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetGreen;
    c.softerBackground += k_offsetGreen;
    c.hotBackground    += k_offsetGreen;
    c.pureBackground   += k_offsetGreen;
    c.edge             += k_offsetEdge + k_offsetGreen;
    c.hotEdge          += k_offsetGreen;
    c.disabledEdge     += k_offsetGreen;
    return c;
}
inline constexpr NppDarkModeColors makeBlueColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetBlue;
    c.softerBackground += k_offsetBlue;
    c.hotBackground    += k_offsetBlue;
    c.pureBackground   += k_offsetBlue;
    c.edge             += k_offsetEdge + k_offsetBlue;
    c.hotEdge          += k_offsetBlue;
    c.disabledEdge     += k_offsetBlue;
    return c;
}
inline constexpr NppDarkModeColors makePurpleColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetPurple;
    c.softerBackground += k_offsetPurple;
    c.hotBackground    += k_offsetPurple;
    c.pureBackground   += k_offsetPurple;
    c.edge             += k_offsetEdge + k_offsetPurple;
    c.hotEdge          += k_offsetPurple;
    c.disabledEdge     += k_offsetPurple;
    return c;
}
inline constexpr NppDarkModeColors makeCyanColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetCyan;
    c.softerBackground += k_offsetCyan;
    c.hotBackground    += k_offsetCyan;
    c.pureBackground   += k_offsetCyan;
    c.edge             += k_offsetEdge + k_offsetCyan;
    c.hotEdge          += k_offsetCyan;
    c.disabledEdge     += k_offsetCyan;
    return c;
}
inline constexpr NppDarkModeColors makeOliveColors() {
    NppDarkModeColors c = k_colorsBlack;
    c.background       += k_offsetOlive;
    c.softerBackground += k_offsetOlive;
    c.hotBackground    += k_offsetOlive;
    c.pureBackground   += k_offsetOlive;
    c.edge             += k_offsetEdge + k_offsetOlive;
    c.hotEdge          += k_offsetOlive;
    c.disabledEdge     += k_offsetOlive;
    return c;
}


// =============================================================================
// Dark scrollbar proxy style (must be in header for Q_OBJECT / moc)
// =============================================================================
class DarkScrollBarProxyStyle : public QProxyStyle
{
    Q_OBJECT
public:
    DarkScrollBarProxyStyle(QStyle* base);

    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;

    // ── Static convenience shims (Win32 API names → instance calls) ──────────
    static QBrush getBackgroundBrush()     { return NppDarkMode::instance().backgroundBrush(); }
    static QBrush getCtrlBackgroundBrush() { return NppDarkMode::instance().ctrlBackgroundBrush(); }
    static QBrush getHotBackgroundBrush()  { return NppDarkMode::instance().hotBackgroundBrush(); }
    static QBrush getPureBackgroundBrush()  { return NppDarkMode::instance().pureBackgroundBrush(); }
    static QBrush getDlgBackgroundBrush()   { return NppDarkMode::instance().pureBackgroundBrush(); }
    static QBrush getEdgeBrush()           { return NppDarkMode::instance().edgeBrush(); }
    static QPen   getEdgePen()             { return NppDarkMode::instance().edgePen(); }
    static QPen   getHotEdgePen()          { return NppDarkMode::instance().hotEdgePen(); }
    static QPen   getDarkerTextPen()       { return NppDarkMode::instance().darkerTextPen(); }
    static QRgb   getTextColor()           { return NppDarkMode::instance().textColor(); }
    static QRgb   getDarkerTextColor()     { return NppDarkMode::instance().darkerTextColor(); }
    static QRgb   getBackgroundColor()     { return NppDarkMode::instance().backgroundColor(); }
    static QRgb   getCtrlBackgroundColor() { return NppDarkMode::instance().ctrlBackgroundColor(); }
    static QRgb   getHotBackgroundColor()   { return NppDarkMode::instance().hotBackgroundColor(); }
    static bool   isWindows11()            { return false; }
};

} // namespace NppDarkMode
