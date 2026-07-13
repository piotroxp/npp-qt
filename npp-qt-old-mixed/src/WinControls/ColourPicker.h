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

// Semantic Lift: Win32 ColourPicker → Qt6 ColourPicker
// Original: PowerEditor/src/WinControls/ColourPicker/ColourPicker.h
// Target:   npp-qt/src/WinControls/ColourPicker.h
//
// ColourPicker is a small button-like swatch control (25×25 px) that shows
// the currently selected colour. Left-click opens ColourPopup.
// Right-click toggles the enabled/disabled state (crossed-out swatch).
//
// The 48-colour palette is defined as nppColourItems[] in ColourPopup.h
// and shared with ColourPopup.

#pragma once

#include "WinControls/Window.h"
#include "ColourPicker/ColourPopup.h"

#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>

// Win32 BN_CLICKED forwarded as CPN_COLOURPICKED to parent
#define CPN_COLOURPICKED 1

class ColourPopup;  // forward (ColourPopup.h included above)


// =============================================================================
// ColourPicker — button-like colour swatch control
// =============================================================================
class ColourPicker : public Window
{
    Q_OBJECT

public:
    explicit ColourPicker(QWidget* parent = nullptr);
    ~ColourPicker() override = default;

    void init(void* hInst, QWidget* hParent) override;
    void destroy() override;

    void setColour(const QColor& c) { _currentColour = c; update(); }
    QColor getColour() const { return _currentColour; }

    bool isEnabled() const { return _isEnabled; }
    void setEnabledPicker(bool enabled) {
        _isEnabled = enabled;
        update();
    }

    void disableRightClick() { _disableRightClick = true; }

signals:
    // Emitted when a colour is picked from the popup
    void colorPicked(const QColor& color);
    // Emitted on right-click (toggles enabled state)
    void rightClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void showColorPopup();
    void destroyColorPopup();

    QColor _currentColour = QColor(255, 0, 0); // red default
    bool _isEnabled = true;
    bool _disableRightClick = false;
    ColourPopup* _pColourPopup = nullptr;
};
