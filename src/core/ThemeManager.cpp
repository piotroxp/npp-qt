// ThemeManager.cpp - Theme loading and management for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ThemeManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QApplication>

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager() {
    // Initialize with default dark theme
    applyDefaultDark();
}

ThemeManager::~ThemeManager() = default;

QStringList ThemeManager::availableThemes() const {
    return QStringList{"default", "dark", "light"};
}

bool ThemeManager::loadTheme(const QString& themeName) {
    _currentTheme = themeName;

    if (themeName == "dark" || themeName == "default") {
        applyDefaultDark();
        return true;
    } else if (themeName == "light") {
        applyDefaultLight();
        return true;
    }

    // Try to load from file
    return loadThemeFromPath(themeName);
}

bool ThemeManager::loadThemeFromPath(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();

    // Load colors
    QJsonObject colors = root["colors"].toObject();
    _currentColors.foreground = colors["foreground"].toString("#abb2bf");
    _currentColors.background = colors["background"].toString("#1e1e1e");
    _currentColors.selectionBackground = colors["selectionBackground"].toString("#3e4451");
    _currentColors.currentLineBackground = colors["currentLineBackground"].toString("#2c313a");
    _currentColors.marginBackground = colors["marginBackground"].toString("#1e1e1e");
    _currentColors.caretColor = colors["caretColor"].toString("#528bff");
    _currentColors.lineNumberForeground = colors["lineNumberForeground"].toString("#636d83");
    _currentColors.lineNumberBackground = colors["lineNumberBackground"].toString("#1e1e1e");
    _currentColors.keywordColor = colors["keywordColor"].toString("#c678dd");
    _currentColors.stringColor = colors["stringColor"].toString("#98c379");
    _currentColors.commentColor = colors["commentColor"].toString("#5c6370");
    _currentColors.numberColor = colors["numberColor"].toString("#d19a66");

    // Load QSS
    QJsonObject qss = root["qss"].toObject();
    for (auto it = qss.begin(); it != qss.end(); ++it) {
        _themeQss[it.key()] = it.value().toString();
    }

    return true;
}

ThemeColors ThemeManager::getThemeColors(const QString& theme) const {
    if (theme == _currentTheme) {
        return _currentColors;
    }
    // Return defaults for other themes (lazy load)
    ThemeColors colors;
    colors.background = "#1e1e1e";
    colors.foreground = "#abb2bf";
    return colors;
}

QString ThemeManager::getThemeQss(const QString& theme, const QString& component) const {
    Q_UNUSED(theme);
    return _themeQss.value(component);
}

QString ThemeManager::currentThemeQss(const QString& component) const {
    return _themeQss.value(component);
}

QString ThemeManager::getScintillaStyle(const QString& theme, const QString& lexerClass) const {
    Q_UNUSED(theme);
    return _scintillaStyles.value(_currentTheme).value(lexerClass);
}

QString ThemeManager::currentScintillaStyle(const QString& lexerClass) const {
    return _scintillaStyles.value(_currentTheme).value(lexerClass);
}

void ThemeManager::resetToDefaults() {
    _currentColors = ThemeColors();
    _themeQss.clear();
    _scintillaStyles.clear();
}

void ThemeManager::applyDefaultDark() {
    _currentColors.foreground = "#abb2bf";
    _currentColors.background = "#1e1e1e";
    _currentColors.selectionBackground = "#3e4451";
    _currentColors.currentLineBackground = "#2c313a";
    _currentColors.marginBackground = "#1e1e1e";
    _currentColors.caretColor = "#528bff";
    _currentColors.lineNumberForeground = "#636d83";
    _currentColors.lineNumberBackground = "#1e1e1e";
    _currentColors.keywordColor = "#c678dd";
    _currentColors.stringColor = "#98c379";
    _currentColors.commentColor = "#5c6370";
    _currentColors.numberColor = "#d19a66";

    // Dark QSS for toolbar and statusbar
    _themeQss["toolbar"] = "QToolBar { background: #1e1e1e; border: none; }";
    _themeQss["tabs"] = "QTabWidget::pane { border: 1px solid #3e4451; background: #1e1e1e; }"
                       "QTabBar::tab { background: #2c313a; color: #abb2bf; padding: 6px 12px; }"
                       "QTabBar::tab:selected { background: #1e1e1e; border-bottom: 2px solid #528bff; }"
                       "QTabBar::tab:hover { background: #3e4451; }";
    _themeQss["statusbar"] = "QStatusBar { background: #1e1e1e; color: #abb2bf; }";
    _themeQss["menu"] = "QMenuBar { background: #1e1e1e; color: #abb2bf; }"
                        "QMenuBar::item:selected { background: #3e4451; }"
                        "QMenu { background: #1e1e1e; color: #abb2bf; border: 1px solid #3e4451; }"
                        "QMenu::item:selected { background: #3e4451; }";
}

void ThemeManager::applyDefaultLight() {
    _currentColors.foreground = "#383a42";
    _currentColors.background = "#fafafa";
    _currentColors.selectionBackground = "#e5ebf4";
    _currentColors.currentLineBackground = "#f0f0f0";
    _currentColors.marginBackground = "#fafafa";
    _currentColors.caretColor = "#4078f2";
    _currentColors.lineNumberForeground = "#999999";
    _currentColors.lineNumberBackground = "#fafafa";
    _currentColors.keywordColor = "#a626a4";
    _currentColors.stringColor = "#50a14f";
    _currentColors.commentColor = "#a0a1a7";
    _currentColors.numberColor = "#986801";

    // Light QSS for toolbar and statusbar
    _themeQss["toolbar"] = "QToolBar { background: #fafafa; border: none; }";
    _themeQss["tabs"] = "QTabWidget::pane { border: 1px solid #e5ebf4; background: #fafafa; }"
                       "QTabBar::tab { background: #f0f0f0; color: #383a42; padding: 6px 12px; }"
                       "QTabBar::tab:selected { background: #fafafa; border-bottom: 2px solid #4078f2; }"
                       "QTabBar::tab:hover { background: #e5ebf4; }";
    _themeQss["statusbar"] = "QStatusBar { background: #fafafa; color: #383a42; }";
    _themeQss["menu"] = "QMenuBar { background: #fafafa; color: #383a42; }"
                        "QMenuBar::item:selected { background: #e5ebf4; }"
                        "QMenu { background: #fafafa; color: #383a42; border: 1px solid #e5ebf4; }"
                        "QMenu::item:selected { background: #e5ebf4; }";
}
