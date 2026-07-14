// DpiManager.h - High-DPI awareness and scaling for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QObject>
#include <QScreen>
#include <QWidget>
#include <QWindow>
#include <QRect>

// ============================================================================
// DpiManager — Handles DPI-aware scaling across the application
// ============================================================================
// All UI sizes should go through this manager to be DPI-aware.
// Uses logical pixels that are scaled by the current DPI factor.
// Base DPI is 96 (standard Windows default). Scales proportionally.
//

class DpiManager : public QObject {
    Q_OBJECT

public:
    static DpiManager& instance();

    // === DPI Information ===

    // Get the current DPI scale factor (1.0 = 96 DPI, 1.25 = 120 DPI, 1.5 = 144 DPI, etc.)
    qreal scaleFactor() const { return _scaleFactor; }

    // Get the raw DPI value
    int dpi() const { return _dpi; }

    // Get the base DPI (always 96)
    static constexpr int baseDpi() { return 96; }

    // === Scaling Utilities ===

    // Scale a value from logical pixels to physical pixels
    int scale(int logicalValue) const { return qRound(logicalValue * _scaleFactor); }
    qreal scaleF(qreal logicalValue) const { return logicalValue * _scaleFactor; }

    // Unscale from physical to logical pixels
    int unscale(int physicalValue) const { return qRound(physicalValue / _scaleFactor); }
    qreal unscaleF(qreal physicalValue) const { return physicalValue / _scaleFactor; }

    // Scale a point
    QPoint scale(const QPoint& logicalPoint) const {
        return QPoint(scale(logicalPoint.x()), scale(logicalPoint.y()));
    }

    // Scale a size
    QSize scale(const QSize& logicalSize) const {
        return QSize(scale(logicalSize.width()), scale(logicalSize.height()));
    }

    // Scale a rect (scales x, y, width, height)
    QRect scale(const QRect& logicalRect) const {
        return QRect(
            scale(logicalRect.x()),
            scale(logicalRect.y()),
            scale(logicalRect.width()),
            scale(logicalRect.height())
        );
    }

    // === Font Scaling ===

    // Scale a font size for the current DPI
    int scaleFontSize(int logicalFontSize) const {
        // Font sizes scale with DPI but not as aggressively
        // Use 1.0 + (scaleFactor - 1.0) * 0.5 to avoid overly large fonts
        qreal fontScale = 1.0 + (_scaleFactor - 1.0) * 0.5;
        return qRound(logicalFontSize * fontScale);
    }

    qreal scaleFontSizeF(qreal logicalFontSize) const {
        qreal fontScale = 1.0 + (_scaleFactor - 1.0) * 0.5;
        return logicalFontSize * fontScale;
    }

    // === Standard Sizes ===

    // Get scaled standard sizes (all in logical pixels)
    int standardMargin() const { return scale(_logicalMargins); }
    int standardSpacing() const { return scale(_logicalSpacing); }
    int standardPadding() const { return scale(_logicalPadding); }
    int iconSize() const { return scale(_logicalIconSize); }
    int buttonHeight() const { return scale(_logicalButtonHeight); }
    int toolbarHeight() const { return scale(_logicalToolbarHeight); }
    int tabHeight() const { return scale(_logicalTabHeight); }
    int statusBarHeight() const { return scale(_logicalStatusBarHeight); }

    // === Icon Loading ===

    // Load an icon with automatic DPI scaling
    // Looks for @2x and @3x variants when available
    QString findScaledIcon(const QString& baseName) const;

    // === Widget Scaling ===

    // Apply DPI scaling to a widget's stylesheet (converts logical to physical sizes)
    QString scaleStyleSheet(const QString& styleSheet) const;

    // Apply minimum size hints scaled for DPI
    void applyScaledMinimumSize(QWidget* widget, int minW, int minH);

    // Retranslate strings on all widgets (for font scaling changes)
    void retranslateUi(QWidget* widget);

    // === DPI Change Handling ===

    // Force recalculation of DPI from a specific screen
    void updateFromScreen(QScreen* screen);

    // Set the base logical size for a window (used for restoreGeometry)
    void setLogicalDpi(int dpi) { _logicalDpi = dpi; }
    int logicalDpi() const { return _logicalDpi; }

signals:
    void dpiChanged(int newDpi, qreal newScaleFactor);

private slots:
    void onScreenChanged();
    void onScreensChanged();

private:
    explicit DpiManager(QObject* parent = nullptr);
    ~DpiManager() override;

    void recalculateDpi();
    int detectScreenDpi() const;

    qreal _scaleFactor = 1.0;
    int _dpi = baseDpi();
    int _logicalDpi = baseDpi();  // DPI at which the UI was designed
    QScreen* _currentScreen = nullptr;

    // Standard logical sizes (design at 96 DPI)
    static constexpr int _logicalMargins = 6;
    static constexpr int _logicalSpacing = 6;
    static constexpr int _logicalPadding = 4;
    static constexpr int _logicalIconSize = 24;
    static constexpr int _logicalButtonHeight = 28;
    static constexpr int _logicalToolbarHeight = 40;
    static constexpr int _logicalTabHeight = 28;
    static constexpr int _logicalStatusBarHeight = 22;
};

// ============================================================================
// Inline Helpers for Convenience
// ============================================================================

inline int dp(int logical) { return DpiManager::instance().scale(logical); }
inline qreal dpf(qreal logical) { return DpiManager::instance().scaleF(logical); }
inline int iconSize() { return DpiManager::instance().iconSize(); }
inline int margin() { return DpiManager::instance().standardMargin(); }
inline int spacing() { return DpiManager::instance().standardSpacing(); }

// ============================================================================
// Additional DPI utilities
// ============================================================================
inline qreal getScaleFactor() { return DpiManager::instance().scaleFactor(); }
inline QSize scaleSize(const QSize& size) { return DpiManager::instance().scale(size); }
inline int scaleInt(int value) { return DpiManager::instance().scale(value); }

// ============================================================================
// Cache invalidation for theme/size changes
// ============================================================================
inline void invalidateDpiCaches() { DpiManager::instance().recalculateDpi(); }
