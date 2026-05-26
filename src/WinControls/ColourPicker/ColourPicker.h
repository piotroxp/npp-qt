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

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QColorDialog>
#include <QRgb>
#include <QColor>

// Colour picker control - color button
#define CPN_COLOURPICKED (BN_CLICKED)

// ColourPicker - Button that shows current color and opens color picker
class ColourPicker : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QRgb colour READ colour WRITE setColour)
    Q_PROPERTY(bool enabled READ isPickerEnabled WRITE setPickerEnabled)

public:
    explicit ColourPicker(QWidget* parent = nullptr);
    ~ColourPicker() override;

    void init(QWidget* parent);
    void destroy() override;

    QRgb colour() const { return _currentColour; }
    void setColour(QRgb c) {
        _currentColour = c;
        updateColorDisplay();
    }

    bool isPickerEnabled() const { return _isEnabled; }
    void setPickerEnabled(bool enabled) {
        _isEnabled = enabled;
        setDisabled(!enabled);
        update();
    }
    
    void disableRightClick() { _disableRightClick = true; }

signals:
    void colorChanged(QRgb color);
    void colorPicked(QRgb color);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void updateColorDisplay();
    void showColorPopup();
    void destroyColorPopup();

    QRgb _currentColour = qRgb(0xFF, 0x00, 0x00);
    bool _isEnabled = true;
    bool _disableRightClick = false;
    
    QColorDialog* _pColorDialog = nullptr;
};