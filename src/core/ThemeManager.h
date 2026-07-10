// ThemeManager.h - Theme loading and management for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

// ============================================================================
// Theme colors structure
// ============================================================================
struct ThemeColors {
    QString foreground;
    QString background;
    QString selectionBackground;
    QString currentLineBackground;
    QString marginBackground;
    QString caretColor;
    QString lineNumberForeground;
    QString lineNumberBackground;
    QString keywordColor;
    QString stringColor;
    QString commentColor;
    QString numberColor;

    ThemeColors() = default;
};

// ============================================================================
// ThemeManager — loads and manages editor themes
// ============================================================================
class ThemeManager {
public:
    ThemeManager();
    ~ThemeManager();

    // Available themes
    QStringList availableThemes() const;

    // Theme loading
    bool loadTheme(const QString& themeName);
    bool loadThemeFromPath(const QString& path);

    // Theme colors (for applying to Scintilla)
    ThemeColors getThemeColors(const QString& theme) const;
    ThemeColors currentThemeColors() const { return _currentColors; }

    // Theme QSS (for toolbar, statusbar, etc.)
    QString getThemeQss(const QString& theme, const QString& component) const;
    QString currentThemeQss(const QString& component) const;

    // Scintilla style string (for lexer/editor)
    QString getScintillaStyle(const QString& theme, const QString& lexerClass) const;
    QString currentScintillaStyle(const QString& lexerClass) const;

    // Current theme
    QString currentTheme() const { return _currentTheme; }

private:
    void resetToDefaults();
    void applyDefaultDark();
    void applyDefaultLight();

    QString _currentTheme;
    ThemeColors _currentColors;
    QMap<QString, QString> _themeQss;
    QMap<QString, QMap<QString, QString>> _scintillaStyles;
};

