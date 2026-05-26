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

// Qt6 Translation - NppDarkMode.h
// Dark mode support using Qt Palette and Stylesheet

#pragma once

#include <QtWidgets/QWidget>
#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QStyleFactory>

// Forward declarations
struct TbIconInfo;

// Dark mode namespace
namespace NppDarkMode {
    
// Color tone options
enum class ColorTone {
    blackTone = 0,
    redTone = 1,
    greenTone = 2,
    blueTone = 3,
    purpleTone = 4,
    cyanTone = 5,
    oliveTone = 6,
    customizedTone = 32
};

// Colors structure
struct Colors {
    QColor background;
    QColor softerBackground;
    QColor hotBackground;
    QColor pureBackground;
    QColor errorBackground;
    QColor text;
    QColor darkerText;
    QColor disabledText;
    QColor linkText;
    QColor edge;
    QColor hotEdge;
    QColor disabledEdge;
};

// Options structure
struct Options {
    bool enable = false;
    bool enablePlugin = false;
};

// Theme colors
struct ThemeColors {
    // Black (default dark)
    static Colors darkColors() {
        Colors c;
        c.background = QColor(32, 32, 32);
        c.softerBackground = QColor(56, 56, 56);
        c.hotBackground = QColor(69, 69, 69);
        c.pureBackground = QColor(32, 32, 32);
        c.errorBackground = QColor(176, 0, 0);
        c.text = QColor(224, 224, 224);
        c.darkerText = QColor(192, 192, 192);
        c.disabledText = QColor(128, 128, 128);
        c.linkText = QColor(255, 255, 0);
        c.edge = QColor(100, 100, 100);
        c.hotEdge = QColor(155, 155, 155);
        c.disabledEdge = QColor(72, 72, 72);
        return c;
    }
    
    // Red tone
    static Colors darkRedColors() {
        Colors c = darkColors();
        c.background = QColor(32 + 16, 32, 32);
        c.softerBackground = QColor(56 + 16, 56, 56);
        c.hotBackground = QColor(69 + 16, 69, 69);
        return c;
    }
    
    // Green tone
    static Colors darkGreenColors() {
        Colors c = darkColors();
        c.background = QColor(32, 32 + 16, 32);
        c.softerBackground = QColor(56, 56 + 16, 56);
        c.hotBackground = QColor(69, 69 + 16, 69);
        return c;
    }
    
    // Blue tone
    static Colors darkBlueColors() {
        Colors c = darkColors();
        c.background = QColor(32, 32, 32 + 32);
        c.softerBackground = QColor(56, 56, 56 + 32);
        c.hotBackground = QColor(69, 69, 69 + 32);
        return c;
    }
    
    // Light mode colors
    static Colors lightColors() {
        Colors c;
        c.background = QColor(255, 255, 255);
        c.softerBackground = QColor(240, 240, 240);
        c.hotBackground = QColor(230, 230, 230);
        c.pureBackground = QColor(255, 255, 255);
        c.errorBackground = QColor(255, 200, 200);
        c.text = QColor(0, 0, 0);
        c.darkerText = QColor(64, 64, 64);
        c.disabledText = QColor(128, 128, 128);
        c.linkText = QColor(0, 0, 255);
        c.edge = QColor(192, 192, 192);
        c.hotEdge = QColor(128, 128, 128);
        c.disabledEdge = QColor(224, 224, 224);
        return c;
    }
};

// Initialize dark mode
void initDarkMode();

// Refresh dark mode application
void refreshDarkMode(QWidget* mainWindow, bool forceRefresh = false);

// Check if dark mode is enabled
bool isEnabled();

// Check if dark mode is enabled for plugins
bool isEnabledForPlugins();

// Check if experimental dark mode is active
bool isExperimentalActive();

// Check if experimental dark mode is supported
bool isExperimentalSupported();

// Get current palette based on dark mode
QPalette getPalette();

// Get current stylesheet
QString getStyleSheet();

// Get background color
QColor getBackgroundColor();

// Get text color
QColor getTextColor();

// Get edge color
QColor getEdgeColor();

// Get disabled text color
QColor getDisabledTextColor();

// Get link text color
QColor getLinkTextColor();

// Get accent color
QColor getAccentColor();

// Get current color tone
ColorTone getColorTone();

// Set color tone
void setColorTone(ColorTone tone);

// Get dark mode default colors
Colors getDarkModeDefaultColors(ColorTone colorTone = ColorTone::blackTone);

// Apply dark mode to widget
void applyDarkMode(QWidget* widget, bool recursive = true);

// Apply dark mode to all child widgets
void applyDarkModeToChildren(QWidget* parent);

// Setup dark mode for specific widget types
void setupDarkModeTabBar(QTabBar* tabBar);
void setupDarkModeMenu(QMenu* menu);
void setupDarkModeToolBar(QToolBar* toolBar);
void setupDarkModeScrollBar(QScrollBar* scrollBar);
void setupDarkModeComboBox(QComboBox* comboBox);
void setupDarkModeLineEdit(QLineEdit* lineEdit);
void setupDarkModeSpinBox(QSpinBox* spinBox);
void setupDarkModeCheckBox(QCheckBox* checkBox);
void setupDarkModeRadioButton(QRadioButton* radioButton);
void setupDarkModeTextEdit(QTextEdit* textEdit);
void setupDarkModeTreeView(QTreeView* treeView);
void setupDarkModeListView(QListView* listView);
void setupDarkModeTableView(QTableView* tableView);
void setupDarkModeSplitter(QSplitter* splitter);
void setupDarkModeGroupBox(QGroupBox* groupBox);
void setupDarkModeLabel(QLabel* label);
void setupDarkModeProgressBar(QProgressBar* progressBar);

// Get custom theme colors
void setCustomColors(const Colors& colors);
Colors getCustomColors();

// Apply color tuning to a base color
QColor adjustColorLightness(const QColor& color, int lightnessDelta, int saturationDelta);

// Calculate perceived lightness
double calculatePerceivedLightness(const QColor& color);

// Invert lightness
QColor invertLightness(const QColor& color);

// Apply theme to application
void applyTheme(QApplication* app);

// Check if dark mode setting changed
bool hasSettingChanged();

} // namespace NppDarkMode
