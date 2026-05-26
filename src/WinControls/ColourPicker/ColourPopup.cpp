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

#include "ColourPopup.h"
#include "../StaticDialog/StaticDialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>

const QRgb ColourPopup::g_ColourArray[NUM_COLORS] = {
    qRgb(0x00, 0x00, 0x00), qRgb(0x80, 0x00, 0x00), qRgb(0x00, 0x80, 0x00), qRgb(0x80, 0x80, 0x00),
    qRgb(0x00, 0x00, 0x80), qRgb(0x80, 0x00, 0x80), qRgb(0x00, 0x80, 0x80), qRgb(0xC0, 0xC0, 0xC0),
    qRgb(0x80, 0x80, 0x80), qRgb(0xFF, 0x00, 0x00), qRgb(0x00, 0xFF, 0x00), qRgb(0xFF, 0xFF, 0x00),
    qRgb(0x00, 0x00, 0xFF), qRgb(0xFF, 0x00, 0xFF), qRgb(0x00, 0xFF, 0xFF), qRgb(0xFF, 0xFF, 0xFF),
    qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x5F), qRgb(0x00, 0x00, 0x7F), qRgb(0x00, 0x00, 0x9F),
    qRgb(0x00, 0x00, 0xBF), qRgb(0x00, 0x00, 0xDF), qRgb(0x00, 0x00, 0xFF), qRgb(0x3F, 0x3F, 0x3F),
    qRgb(0x00, 0x5F, 0x00), qRgb(0x00, 0x7F, 0x00), qRgb(0x00, 0x9F, 0x00), qRgb(0x00, 0xBF, 0x00),
    qRgb(0x00, 0xDF, 0x00), qRgb(0x00, 0xFF, 0x00), qRgb(0x5F, 0x5F, 0x00), qRgb(0x7F, 0x7F, 0x00),
    qRgb(0x9F, 0x9F, 0x00), qRgb(0xBF, 0xBF, 0x00), qRgb(0xDF, 0xDF, 0x00), qRgb(0xFF, 0xFF, 0x00),
    qRgb(0x00, 0x00, 0x5F), qRgb(0x00, 0x00, 0x7F), qRgb(0x00, 0x00, 0x9F), qRgb(0x00, 0x00, 0xBF)
};

ColourPopup::ColourPopup()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setObjectName("ColorPopup");
}

ColourPopup::ColourPopup(QRgb defaultColor)
    : _colour(defaultColor)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setObjectName("ColorPopup");
}

ColourPopup::~ColourPopup()
{
}

void ColourPopup::createColorPopup()
{
    if (_pGridLayout) return;
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    
    _pGridLayout = new QGridLayout();
    mainLayout->addLayout(_pGridLayout);
    
    int rows = (NUM_COLORS + 8) / 8;
    for (int i = 0; i < NUM_COLORS; ++i) {
        int row = i / 8;
        int col = i % 8;
        
        QPushButton* btn = new QPushButton(this);
        btn->setFixedSize(18, 18);
        btn->setStyleSheet(QString(
            "QPushButton { "
            "   background-color: rgb(%1, %2, %3); "
            "   border: 1px solid #000000; "
            "}"
            "QPushButton:hover { "
            "   border: 2px solid #FFFFFF; "
            "}"
        ).arg(qRed(g_ColourArray[i]))
         .arg(qGreen(g_ColourArray[i]))
         .arg(qBlue(g_ColourArray[i])));
        
        int colorIndex = i;
        connect(btn, &QPushButton::clicked, this, [this, colorIndex]() {
            QRgb selected = g_ColourArray[colorIndex];
            emit colorSelected(selected);
            accept();
        });
        
        _pGridLayout->addWidget(btn, row, col);
        _colorButtons.append(btn);
    }
}

void ColourPopup::doDialog(QPoint p)
{
    createColorPopup();
    move(p);
    show();
    
    // Use event loop to keep popup visible
    exec();
}

QColor ColourPopup::getColorFromArray(int index)
{
    if (index >= 0 && index < NUM_COLORS) {
        return QColor::fromRgb(g_ColourArray[index]);
    }
    return QColor();
}