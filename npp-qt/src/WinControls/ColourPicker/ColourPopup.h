// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

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

// Semantic Lift: Win32 ColourPopup → Qt6 ColourPopup
// Original: PowerEditor/src/WinControls/ColourPicker/ColourPopup.h/.cpp
// Target:   npp-qt/src/WinControls/ColourPicker/ColourPopup.h/.cpp
//
// ColourPopup is a modeless dialog showing a 6×8 grid (48 swatches) of
// preset colours plus a "Custom Colour..." button that opens QColorDialog.
// A colour preview bar at the top shows the currently hovered/selected colour.

#pragma once

#include <QDialog>
#include <QColor>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QColorDialog>
#include <QMouseEvent>
#include <QEvent>
#include <QToolTip>
#include <QPainter>
#include <QPixmap>

// The 48-colour NPP palette — mirrors the Win32 colorItems[] array exactly.
// 6 rows × 8 columns (48 swatches).
static const QColor nppColourItems[48] = {
    QColor(  0,   0,   0), QColor( 64,   0,   0), QColor(128,   0,   0), QColor(128,  64,  64),
    QColor(255,   0,   0), QColor(255, 128, 128), QColor(255, 255, 128), QColor(255, 255,   0),
    QColor(255, 128,  64), QColor(255, 128,   0), QColor(128,  64,   0), QColor(128, 128,   0),
    QColor(128, 128,  64), QColor(  0,  64,   0), QColor(  0, 128,   0), QColor(  0, 255,   0),
    QColor(128, 255,   0), QColor(128, 255, 128), QColor(  0, 255, 128), QColor(  0, 255,  64),
    QColor(  0, 128, 128), QColor(  0, 128,  64), QColor(  0,  64,  64), QColor(128, 128, 128),
    QColor( 64, 128, 128), QColor(  0,   0, 128), QColor(  0,   0, 255), QColor(  0,  64, 128),
    QColor(  0, 255, 255), QColor(128, 255, 255), QColor(  0, 128, 255), QColor(  0, 128, 192),
    QColor(128, 128, 255), QColor(  0,   0, 160), QColor(  0,   0,  64), QColor(192, 192, 192),
    QColor( 64,   0,  64), QColor(192,   0,  96), QColor(128,   0, 128), QColor(128,   0,  64),
    QColor(128, 128, 192), QColor(255, 128, 192), QColor(255, 128, 255), QColor(255,   0, 255),
    QColor(255,   0, 128), QColor(128,   0, 255), QColor( 64,   0, 128), QColor(255, 255, 255),
};


class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    // Construct a colour popup anchored to parent widget.
    // defaultColor: pre-selected colour shown as the initial preview.
    explicit ColourPopup(const QColor& defaultColor = Qt::white, QWidget* parent = nullptr);
    ~ColourPopup() override = default;

    // Current selected colour (last clicked or initial default)
    QColor selectedColor() const { return _colour; }

signals:
    // Emitted when user picks any colour (preset or custom)
    void colorSelected(const QColor& color);

private slots:
    void onCustomColorClicked();

private:
    // Event filter installed on colour cells to handle click
    bool eventFilter(QObject* watched, QEvent* event) override;

    // Select a colour from the 48-item grid
    void selectColourAtIndex(int index);

    // Update the preview bar
    void updatePreview(const QColor& colour);

    // Redraw a single colour cell (paint swatch + selection border)
    void redrawCell(int index, bool selected);

    // Colour cells: map from cell QLabel* → index
    int cellIndex(QLabel* cell) const;

    QColor _colour;
    QLabel* _previewLabel = nullptr;     // colour preview swatch
    QLabel* _previewTextLabel = nullptr; // "RGB(...)" text
    QVector<QLabel*> _cells;             // 48 colour cell labels
    int _selectedIndex = -1;              // -1 = none / custom
};
