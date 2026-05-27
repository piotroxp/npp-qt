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

#include "ColourPicker.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QColorDialog>
#include <QColor>
#include <QHBoxLayout>

ColourPicker::ColourPicker(QWidget* parent)
    : QPushButton(parent)
    , _currentColour(qRgb(0xFF, 0x00, 0x00))
    , _isEnabled(true)
    , _disableRightClick(false)
    , _pColorDialog(nullptr)
{
    setMinimumSize(25, 25);
    setMaximumSize(25, 25);
    setText("F");
    
    connect(this, &QPushButton::clicked, this, &ColourPicker::showColorPopup);
}

ColourPicker::~ColourPicker()
{
    destroyColorPopup();
}

void ColourPicker::init(QWidget* parent)
{
    Q_UNUSED(parent);
    // Already initialized in constructor
}

void ColourPicker::destroy()
{
    destroyColorPopup();
    deleteLater();
}

void ColourPicker::updateColorDisplay()
{
    QColor c = QColor::fromRgb(_currentColour);
    setStyleSheet(QString(
        "QPushButton { "
        "   background-color: rgb(%1, %2, %3); "
        "   border: 1px solid #888888; "
        "   min-width: 25px; "
        "   max-width: 25px; "
        "   min-height: 25px; "
        "   max-height: 25px; "
        "}"
    ).arg(c.red()).arg(c.green()).arg(c.blue()));
}

void ColourPicker::showColorPopup()
{
    destroyColorPopup();
    
    QColor initialColor = QColor::fromRgb(_currentColour);
    QColorDialog* dialog = new QColorDialog(initialColor, this);
    _pColorDialog = dialog;
    
    connect(dialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {
        _currentColour = color.rgb();
        updateColorDisplay();
        emit colorPicked(_currentColour);
        emit colorChanged(_currentColour);
    });
    
    dialog->setWindowTitle("Pick a Color");
    dialog->open();
}

void ColourPicker::destroyColorPopup()
{
    if (_pColorDialog) {
        _pColorDialog->deleteLater();
        _pColorDialog = nullptr;
    }
}

void ColourPicker::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        showColorPopup();
    }
    QPushButton::mouseDoubleClickEvent(event);
}

void ColourPicker::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        if (!_disableRightClick) {
            _isEnabled = !_isEnabled;
            setDisabled(!_isEnabled);
            update();
            emit colorChanged(_currentColour);
        }
    } else if (event->button() == Qt::LeftButton) {
        showColorPopup();
    }
}

void ColourPicker::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    
    // Draw the color rectangle
    QRect rect = contentsRect();
    QColor c = QColor::fromRgb(_currentColour);
    p.fillRect(rect, c);
    
    // Draw border
    p.setPen(Qt::darkGray);
    p.drawRect(rect.adjusted(0, 0, -1, -1));
    
    // If disabled, draw strikethrough
    if (!_isEnabled) {
        // Calculate luminance
        int luminance = (c.red() * 299 + c.green() * 587 + c.blue() * 114) / 1000;
        QColor strikeColor = (luminance < 128) ? Qt::white : Qt::black;
        
        p.setPen(QPen(strikeColor, 2));
        p.drawLine(rect.topLeft(), rect.bottomRight());
    }
    
    event->accept();
}