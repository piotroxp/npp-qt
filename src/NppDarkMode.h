// Semantic lift: NppDarkMode.h — Win32 dark mode theming → Qt6 dark mode palette
// Original: PowerEditor/src/NppDarkMode.h + .cpp (4542 lines)
// Target: npp-qt/src/NppDarkMode.h + .cpp
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3

#pragma once

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

// Forward declarations
class QPaintDevice;
class QPalette;

// =============================================================================
// Colour palette — maps Win32 COLORREF → Qt QRgb
// =============================================================================

namespace NppDarkMode
{

struct Colors
{
    QRgb background      = 0x202020;  // main background
    QRgb softerBackground = 0x383838; // control background
    QRgb hotBackground   = 0x454545;  // hover highlight
    QRgb pureBackground  = 0x202020;  // dialog background
    QRgb errorBackground = 0xB00000; // error/warning bg
    QRgb text            = 0xE0E0E0;  // primary text
    QRgb darkerText      = 0xC0C0C0;  // secondary text
    QRgb disabledText    = 0x808080;  // disabled text
    QRgb linkText        = 0xFFFF00;  // hyperlink text
    QRgb edge            = 0x646464;  // borders
    QRgb hotEdge         = 0x9B9B9B;  // hover border
    QRgb disabledEdge    = 0x484848;  // disabled border
};

struct Options
{
    bool enable = false;
    bool enablePlugin = false;
};

enum class ColorTone
{
    blackTone     = 0,
    redTone       = 1,
    greenTone     = 2,
    blueTone      = 3,
    purpleTone    = 4,
    cyanTone      = 5,
    oliveTone     = 6,
    customizedTone = 32
};

enum class ToolTipsType
{
    tooltip,
    toolbar,
    listview,
    treeview,
    tabbar
};

enum class TreeViewStyle
{
    classic = 0,
    light   = 1,
    dark    = 2
};

// Accent colour source
enum class AccentSource
{
    systemDefault,
    custom
};

// =============================================================================
// NppDarkMode — singleton palette + style controller
// =============================================================================

class NppDarkMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY darkModeChanged)

public:
    // ── Enable / query ────────────────────────────────────────────────────
    bool isEnabled() const;
    bool isEnabledForPlugins() const;
    void setEnabled(bool on);
    void setEnabledForPlugins(bool on);

    // ── Color palette ───────────────────────────────────────────────────────
    Colors colors() const;
    void setColors(const Colors& c);

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
    void setAccentSource(AccentSource src);

    // ── Tone ────────────────────────────────────────────────────────────────
    void setTone(ColorTone tone);
    ColorTone tone() const;
    Colors defaultColors(ColorTone tone) const;
    static Colors getDarkModeDefaultColors(ColorTone colorTone = ColorTone::blackTone);

    // ── Tree-view style ────────────────────────────────────────────────────
    TreeViewStyle treeViewStyle() const;
    void setTreeViewStyle(QWidget* treeView, TreeViewStyle style);
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
    Colors _colors;
    ColorTone _tone = ColorTone::blackTone;
    Options _options;
    AccentSource _accentSource = AccentSource::systemDefault;
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

public:
    // Exposed via second public section so MOC-generated members stay isolated
    static NppDarkMode& instance();
};

// Namespace-level delegate so code can use instance() without qualification.
// Placed after the class definition so NppDarkMode is complete.
inline NppDarkMode& instance()
{
    return NppDarkMode::instance();
}

// Helper: build a QRgb from 0xRRGGBB (big-endian hex literal)
constexpr inline QRgb hexRgb(unsigned int rrggbb)
{
    return ((rrggbb & 0xFF0000) >> 16)
         |  (rrggbb & 0x00FF00)
         | ((rrggbb & 0x0000FF) << 16);
}

// ── Convenience colours for the default dark theme ──────────────────────────
static constexpr QRgb k_darkBg        = 0x202020;
static constexpr QRgb k_darkCtrlBg    = 0x383838;
static constexpr QRgb k_darkHotBg      = 0x454545;
static constexpr QRgb k_darkPureBg    = 0x202020;
static constexpr QRgb k_darkErrorBg   = 0xB00000;
static constexpr QRgb k_darkText      = 0xE0E0E0;
static constexpr QRgb k_darkDarkerText = 0xC0C0C0;
static constexpr QRgb k_darkDisabledText = 0x808080;
static constexpr QRgb k_darkLink      = 0xFFFF00;
static constexpr QRgb k_darkEdge      = 0x646464;
static constexpr QRgb k_darkHotEdge    = 0x9B9B9B;
static constexpr QRgb k_darkDisabledEdge = 0x484848;

// Dark scrollbar proxy style (must be in header for Q_OBJECT / moc)
class DarkScrollBarProxyStyle : public QProxyStyle
{
    Q_OBJECT
public:
    DarkScrollBarProxyStyle(QStyle* base);

    // Qt6: QProxyStyle::styleHint returns int (was QStyle::StyleHint in Qt5)
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;
};

} // namespace NppDarkMode
