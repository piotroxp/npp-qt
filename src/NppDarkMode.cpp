// This file is part of Notepad++ project
// Copyright (c) 2021 adzm / Adam D. Walling

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 Translation - NppDarkMode.cpp
// Dark mode support using Qt Palette and Stylesheet

#include "NppDarkMode.h"
#include "Parameters.h"

#include <QApplication>
#include <QWidget>
#include <QPalette>
#include <QColor>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QCommonStyle>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabBar>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <cmath>

namespace NppDarkMode {

// Static state
static bool g_enabled = false;
static bool g_enabledForPlugins = true;
static ColorTone g_colorTone = ColorTone::blackTone;
static Colors g_customColors;
static Colors g_currentColors;
static bool g_settingChanged = false;

// Initialize dark mode
void initDarkMode() {
    NppParameters& nppParam = NppParameters::getInstance();
    const NppGUI& nppGUI = nppParam.getNppGUI();
    
    g_enabled = nppGUI._darkmode._isEnabled;
    g_enabledForPlugins = nppGUI._darkmode._isEnabledPlugin;
    g_colorTone = static_cast<ColorTone>(nppGUI._darkmode._colorTone);
    
    if (g_enabled) {
        g_currentColors = getDarkModeDefaultColors(g_colorTone);
    } else {
        g_currentColors = ThemeColors::lightColors();
    }
}

// Refresh dark mode application
void refreshDarkMode(QWidget* mainWindow, bool forceRefresh) {
    if (!mainWindow) return;
    
    NppParameters& nppParam = NppParameters::getInstance();
    const NppGUI& nppGUI = nppParam.getNppGUI();
    
    bool newEnabled = nppGUI._darkmode._isEnabled;
    
    if (newEnabled != g_enabled || forceRefresh) {
        g_enabled = newEnabled;
        
        if (g_enabled) {
            g_currentColors = getDarkModeDefaultColors(g_colorTone);
        } else {
            g_currentColors = ThemeColors::lightColors();
        }
        
        // Apply new palette to main window
        QPalette palette = getPalette();
        mainWindow->setPalette(palette);
        
        // Apply stylesheet
        mainWindow->setStyleSheet(getStyleSheet());
        
        // Recursively apply to children
        applyDarkModeToChildren(mainWindow);
    }
}

// Check if dark mode is enabled
bool isEnabled() {
    return g_enabled;
}

// Check if dark mode is enabled for plugins
bool isEnabledForPlugins() {
    return g_enabledForPlugins;
}

// Check if experimental dark mode is active
bool isExperimentalActive() {
    return g_enabled;
}

// Check if experimental dark mode is supported
bool isExperimentalSupported() {
    return true; // Always supported in Qt
}

// Get current palette
QPalette getPalette() {
    QPalette palette;
    
    if (g_enabled) {
        // Dark palette
        palette.setColor(QPalette::Window, g_currentColors.background);
        palette.setColor(QPalette::WindowText, g_currentColors.text);
        palette.setColor(QPalette::Base, g_currentColors.pureBackground);
        palette.setColor(QPalette::AlternateBase, g_currentColors.softerBackground);
        palette.setColor(QPalette::ToolTipBase, g_currentColors.hotBackground);
        palette.setColor(QPalette::ToolTipText, g_currentColors.text);
        palette.setColor(QPalette::Text, g_currentColors.text);
        palette.setColor(QPalette::Button, g_currentColors.softerBackground);
        palette.setColor(QPalette::ButtonText, g_currentColors.text);
        palette.setColor(QPalette::BrightText, Qt::white);
        palette.setColor(QPalette::Highlight, g_currentColors.hotBackground);
        palette.setColor(QPalette::HighlightedText, g_currentColors.text);
        palette.setColor(QPalette::Link, g_currentColors.linkText);
        palette.setColor(QPalette::LinkVisited, g_currentColors.disabledText);
        
        // Disabled states
        palette.setColor(QPalette::Disabled, QPalette::WindowText, g_currentColors.disabledText);
        palette.setColor(QPalette::Disabled, QPalette::Text, g_currentColors.disabledText);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, g_currentColors.disabledText);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, g_currentColors.edge);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, g_currentColors.disabledText);
    } else {
        // Light palette
        palette = QPalette(Qt::lightGray);
    }
    
    return palette;
}

// Get current stylesheet
QString getStyleSheet() {
    QString style;
    
    if (g_enabled) {
        // Dark mode stylesheet
        style = R"(
            QMainWindow, QWidget {
                background-color: %1;
                color: %2;
            }
            
            QMenuBar {
                background-color: %1;
                color: %2;
            }
            
            QMenuBar::item:selected {
                background-color: %3;
            }
            
            QMenu {
                background-color: %1;
                color: %2;
                border: 1px solid %4;
            }
            
            QMenu::item:selected {
                background-color: %3;
            }
            
            QToolBar {
                background-color: %1;
                border: none;
            }
            
            QStatusBar {
                background-color: %5;
                color: %2;
            }
            
            QTabBar::tab {
                background-color: %5;
                color: %2;
                padding: 4px 8px;
            }
            
            QTabBar::tab:selected {
                background-color: %1;
            }
            
            QTabBar::tab:hover {
                background-color: %3;
            }
            
            QPushButton {
                background-color: %5;
                color: %2;
                border: 1px solid %4;
                padding: 4px 12px;
            }
            
            QPushButton:hover {
                background-color: %3;
            }
            
            QPushButton:pressed {
                background-color: %1;
            }
            
            QPushButton:disabled {
                background-color: %5;
                color: %6;
            }
            
            QLineEdit, QTextEdit, QPlainTextEdit {
                background-color: %1;
                color: %2;
                border: 1px solid %4;
                selection-background-color: %3;
            }
            
            QComboBox {
                background-color: %5;
                color: %2;
                border: 1px solid %4;
            }
            
            QComboBox:hover {
                background-color: %3;
            }
            
            QComboBox::drop-down {
                border: none;
            }
            
            QComboBox QAbstractItemView {
                background-color: %1;
                color: %2;
                selection-background-color: %3;
            }
            
            QSpinBox {
                background-color: %1;
                color: %2;
                border: 1px solid %4;
            }
            
            QCheckBox, QRadioButton {
                color: %2;
            }
            
            QCheckBox::indicator:checked, QRadioButton::indicator:checked {
                background-color: %3;
            }
            
            QTreeView, QListView, QTableView {
                background-color: %1;
                color: %2;
                alternate-background-color: %5;
                selection-background-color: %3;
            }
            
            QHeaderView::section {
                background-color: %5;
                color: %2;
            }
            
            QScrollBar:vertical {
                background: %1;
                width: 12px;
            }
            
            QScrollBar::handle:vertical {
                background: %4;
                min-height: 20px;
            }
            
            QScrollBar::handle:vertical:hover {
                background: %3;
            }
            
            QScrollBar:horizontal {
                background: %1;
                height: 12px;
            }
            
            QScrollBar::handle:horizontal {
                background: %4;
                min-width: 20px;
            }
            
            QScrollBar::handle:horizontal:hover {
                background: %3;
            }
            
            QSplitter::handle {
                background-color: %4;
            }
            
            QGroupBox {
                color: %2;
                border: 1px solid %4;
                margin-top: 8px;
            }
            
            QGroupBox::title {
                background-color: transparent;
            }
            
            QLabel {
                color: %2;
            }
            
            QProgressBar {
                background-color: %1;
                color: %2;
                border: 1px solid %4;
            }
            
            QProgressBar::chunk {
                background-color: %3;
            }
            
            QToolTip {
                background-color: %3;
                color: %2;
            }
            
            QStatusBar::item {
                border: none;
            }
            
            /* Scintilla editor */
            QScintilla {
                background-color: %1;
                color: %2;
            }
        )";
        
        style = style
            .arg(g_currentColors.background.name())
            .arg(g_currentColors.text.name())
            .arg(g_currentColors.hotBackground.name())
            .arg(g_currentColors.edge.name())
            .arg(g_currentColors.softerBackground.name())
            .arg(g_currentColors.disabledText.name());
    }
    
    return style;
}

// Get background color
QColor getBackgroundColor() {
    return g_currentColors.background;
}

// Get text color
QColor getTextColor() {
    return g_currentColors.text;
}

// Get edge color
QColor getEdgeColor() {
    return g_currentColors.edge;
}

// Get disabled text color
QColor getDisabledTextColor() {
    return g_currentColors.disabledText;
}

// Get link text color
QColor getLinkTextColor() {
    return g_currentColors.linkText;
}

// Get accent color
QColor getAccentColor() {
    // Default accent color for dark mode
    if (g_enabled) {
        return QColor(0, 120, 215); // Blue
    }
    return QColor(0, 120, 215);
}

// Get current color tone
ColorTone getColorTone() {
    return g_colorTone;
}

// Set color tone
void setColorTone(ColorTone tone) {
    g_colorTone = tone;
    if (g_enabled) {
        g_currentColors = getDarkModeDefaultColors(tone);
    }
}

// Get dark mode default colors
Colors getDarkModeDefaultColors(ColorTone colorTone) {
    switch (colorTone) {
        case ColorTone::redTone:
            return ThemeColors::darkRedColors();
        case ColorTone::greenTone:
            return ThemeColors::darkGreenColors();
        case ColorTone::blueTone:
            return ThemeColors::darkBlueColors();
        case ColorTone::blackTone:
        default:
            return ThemeColors::darkColors();
    }
}

// Apply dark mode to widget
void applyDarkMode(QWidget* widget, bool recursive) {
    if (!widget) return;
    
    widget->setPalette(getPalette());
    widget->setStyleSheet(getStyleSheet());
    
    if (recursive) {
        applyDarkModeToChildren(widget);
    }
}

// Apply dark mode to children
void applyDarkModeToChildren(QWidget* parent) {
    if (!parent) return;
    
    for (QObject* child : parent->children()) {
        QWidget* widget = qobject_cast<QWidget*>(child);
        if (widget && !widget->isWindow()) {
            // Apply appropriate dark mode styling based on widget type
            if (auto tabBar = qobject_cast<QTabBar*>(widget)) {
                setupDarkModeTabBar(tabBar);
            } else if (auto menu = qobject_cast<QMenu*>(widget)) {
                setupDarkModeMenu(menu);
            } else if (auto toolBar = qobject_cast<QToolBar*>(widget)) {
                setupDarkModeToolBar(toolBar);
            } else if (auto scrollBar = qobject_cast<QScrollBar*>(widget)) {
                setupDarkModeScrollBar(scrollBar);
            } else if (auto comboBox = qobject_cast<QComboBox*>(widget)) {
                setupDarkModeComboBox(comboBox);
            } else if (auto lineEdit = qobject_cast<QLineEdit*>(widget)) {
                setupDarkModeLineEdit(lineEdit);
            } else if (auto spinBox = qobject_cast<QSpinBox*>(widget)) {
                setupDarkModeSpinBox(spinBox);
            } else if (auto checkBox = qobject_cast<QCheckBox*>(widget)) {
                setupDarkModeCheckBox(checkBox);
            } else if (auto radioButton = qobject_cast<QRadioButton*>(widget)) {
                setupDarkModeRadioButton(radioButton);
            } else if (auto textEdit = qobject_cast<QTextEdit*>(widget)) {
                setupDarkModeTextEdit(textEdit);
            } else if (auto treeView = qobject_cast<QTreeView*>(widget)) {
                setupDarkModeTreeView(treeView);
            } else if (auto listView = qobject_cast<QListView*>(widget)) {
                setupDarkModeListView(listView);
            } else if (auto tableView = qobject_cast<QTableView*>(widget)) {
                setupDarkModeTableView(tableView);
            } else if (auto splitter = qobject_cast<QSplitter*>(widget)) {
                setupDarkModeSplitter(splitter);
            } else if (auto groupBox = qobject_cast<QGroupBox*>(widget)) {
                setupDarkModeGroupBox(groupBox);
            } else if (auto label = qobject_cast<QLabel*>(widget)) {
                setupDarkModeLabel(label);
            } else if (auto progressBar = qobject_cast<QProgressBar*>(widget)) {
                setupDarkModeProgressBar(progressBar);
            } else {
                // Generic widget - just apply palette
                widget->setPalette(getPalette());
            }
            
            // Recursively apply to children
            applyDarkModeToChildren(widget);
        }
    }
}

// Widget-specific setup functions
void setupDarkModeTabBar(QTabBar* tabBar) {
    if (!tabBar) return;
    tabBar->setStyleSheet(getStyleSheet());
}

void setupDarkModeMenu(QMenu* menu) {
    if (!menu) return;
    menu->setStyleSheet(getStyleSheet());
}

void setupDarkModeToolBar(QToolBar* toolBar) {
    if (!toolBar) return;
    toolBar->setStyleSheet(getStyleSheet());
}

void setupDarkModeScrollBar(QScrollBar* scrollBar) {
    if (!scrollBar) return;
    scrollBar->setStyleSheet(getStyleSheet());
}

void setupDarkModeComboBox(QComboBox* comboBox) {
    if (!comboBox) return;
    comboBox->setStyleSheet(getStyleSheet());
}

void setupDarkModeLineEdit(QLineEdit* lineEdit) {
    if (!lineEdit) return;
    lineEdit->setStyleSheet(getStyleSheet());
}

void setupDarkModeSpinBox(QSpinBox* spinBox) {
    if (!spinBox) return;
    spinBox->setStyleSheet(getStyleSheet());
}

void setupDarkModeCheckBox(QCheckBox* checkBox) {
    if (!checkBox) return;
    checkBox->setStyleSheet(getStyleSheet());
}

void setupDarkModeRadioButton(QRadioButton* radioButton) {
    if (!radioButton) return;
    radioButton->setStyleSheet(getStyleSheet());
}

void setupDarkModeTextEdit(QTextEdit* textEdit) {
    if (!textEdit) return;
    textEdit->setStyleSheet(getStyleSheet());
}

void setupDarkModeTreeView(QTreeView* treeView) {
    if (!treeView) return;
    treeView->setStyleSheet(getStyleSheet());
}

void setupDarkModeListView(QListView* listView) {
    if (!listView) return;
    listView->setStyleSheet(getStyleSheet());
}

void setupDarkModeTableView(QTableView* tableView) {
    if (!tableView) return;
    tableView->setStyleSheet(getStyleSheet());
}

void setupDarkModeSplitter(QSplitter* splitter) {
    if (!splitter) return;
    splitter->setStyleSheet(getStyleSheet());
}

void setupDarkModeGroupBox(QGroupBox* groupBox) {
    if (!groupBox) return;
    groupBox->setStyleSheet(getStyleSheet());
}

void setupDarkModeLabel(QLabel* label) {
    if (!label) return;
    label->setStyleSheet(getStyleSheet());
}

void setupDarkModeProgressBar(QProgressBar* progressBar) {
    if (!progressBar) return;
    progressBar->setStyleSheet(getStyleSheet());
}

// Set custom colors
void setCustomColors(const Colors& colors) {
    g_customColors = colors;
}

// Get custom colors
Colors getCustomColors() {
    return g_customColors;
}

// Adjust color lightness
QColor adjustColorLightness(const QColor& color, int lightnessDelta, int saturationDelta) {
    QColor hsl = color.toHsl();
    int h = hsl.hue();
    int s = hsl.saturation();
    int l = hsl.lightness();
    
    l = qBound(0, l + lightnessDelta, 255);
    s = qBound(0, s + saturationDelta, 255);
    
    hsl.setHsl(h, s, l);
    return hsl;
}

// Calculate perceived lightness
double calculatePerceivedLightness(const QColor& color) {
    // Convert to perceived lightness using sRGB coefficients
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();
    
    auto linearValue = [](double c) -> double {
        if (c <= 0.04045) {
            return c / 12.92;
        }
        return std::pow(((c + 0.055) / 1.055, 2.4);
    };
    
    double rLinear = linearValue(r);
    double gLinear = linearValue(g);
    double bLinear = linearValue(b);
    
    double luminance = 0.2126 * rLinear + 0.7152 * gLinear + 0.0722 * bLinear;
    
    // Convert to lightness
    if (luminance <= 216.0 / 24389.0) {
        return luminance * 24389.0 / 27.0;
    }
    return std::pow(luminance, 1.0 / 3.0) * 116.0 - 16.0;
}

// Invert lightness
QColor invertLightness(const QColor& color) {
    double lightness = calculatePerceivedLightness(color);
    int newLightness = qRound(240 - lightness);
    
    QColor hsl = color.toHsl();
    hsl.setLightness(newLightness);
    return hsl;
}

// Apply theme to application
void applyTheme(QApplication* app) {
    if (!app) return;
    
    // Set application-wide palette
    app->setPalette(getPalette());
    
    // Set application-wide stylesheet
    app->setStyleSheet(getStyleSheet());
    
    // Set fusion style for consistency
    app->setStyle(QStringLiteral("Fusion"));
}

// Check if setting changed
bool hasSettingChanged() {
    if (g_settingChanged) {
        g_settingChanged = false;
        return true;
    }
    return false;
}

} // namespace NppDarkMode
