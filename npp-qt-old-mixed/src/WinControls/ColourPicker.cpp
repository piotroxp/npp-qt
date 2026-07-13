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

// Semantic Lift: Win32 ColourPicker → Qt6 ColourPicker implementation
// Original: PowerEditor/src/WinControls/ColourPicker/ColourPicker.cpp
// Target:   npp-qt/src/WinControls/ColourPicker.cpp
//
// ColourPicker is a small 25×25 button showing a colour swatch.
// Left-click opens ColourPopup; right-click toggles enabled/disabled.

#include "ColourPicker.h"
#include <QPainter>
#include <QPainterPath>

// =============================================================================
// ColourPicker
// =============================================================================

ColourPicker::ColourPicker(QWidget* parent)
    : Window(parent)
{
    setFixedSize(25, 25);
    setCursor(Qt::PointingHandCursor);
}

void ColourPicker::init(void* hInst, QWidget* hParent)
{
    WindowBase::init(hInst, hParent);
}

void ColourPicker::destroy()
{
    destroyColorPopup();
    deleteLater();
}

void ColourPicker::destroyColorPopup()
{
    if (_pColourPopup) {
        _pColourPopup->hide();
        delete _pColourPopup;
        _pColourPopup = nullptr;
    }
}

void ColourPicker::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    QRect r = rect().adjusted(1, 1, -2, -2);

    if (_isEnabled) {
        // Background — the colour swatch
        p.fillRect(r, _currentColour);
        // Light border
        p.setPen(QColor(136, 136, 136));
        p.drawRect(r.adjusted(0, 0, -1, -1));
    } else {
        // Disabled: hatching overlay (mirrors Win32 drawForeground)
        p.fillRect(r, Qt::lightGray);

        // Diagonal hatch lines
        QPainterPath clip;
        clip.addRect(r);
        p.setClipPath(clip);

        int luminance = _currentColour.red() * 0.299
                      + _currentColour.green() * 0.587
                      + _currentColour.blue() * 0.114;
        QColor strikeOut = (luminance < 200) ? Qt::white : Qt::black;

        p.setPen(QPen(strikeOut, 1));
        for (int i = -r.height(); i < r.width(); i += 4) {
            p.drawLine(r.left() + i, r.top(), r.left(), r.top() + i);
        }

        p.setClipping(false);
        p.setPen(QColor(136, 136, 136));
        p.drawRect(r.adjusted(0, 0, -1, -1));
    }
}

void ColourPicker::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Single left-click: show colour popup (mirrors Win32 WM_LBUTTONDOWN → ColourPopup)
        showColorPopup();
    } else if (event->button() == Qt::RightButton) {
        if (!_disableRightClick) {
            _isEnabled = !_isEnabled;
            emit rightClicked();
            update();
        }
    }
}

void ColourPicker::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        showColorPopup();
    }
}

void ColourPicker::showColorPopup()
{
    destroyColorPopup();

    _pColourPopup = new ColourPopup(_currentColour, this);
    connect(_pColourPopup, &ColourPopup::colorSelected,
            this, [this](const QColor& color) {
                if (_currentColour != color) {
                    _currentColour = color;
                    emit colorPicked(color);
                    update();
                }
            });

    // Position below the ColourPicker button
    QPoint pos = mapToGlobal(QPoint(0, height()));
    _pColourPopup->move(pos);
    _pColourPopup->show();
}
