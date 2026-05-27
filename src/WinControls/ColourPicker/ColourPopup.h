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
#include <QDialog>
#include <QGridLayout>
#include <QRgb>
#include <QPoint>

#define WM_PICKUP_COLOR (COLOURPOPUP_USER + 1)

// Resource IDs
#define IDD_COLOUR_POPUP 2100
#define IDC_COLOUR_LIST (IDD_COLOUR_POPUP + 1)

// ColourPopup - Popup color grid dialog
class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    ColourPopup();
    explicit ColourPopup(QRgb defaultColor);
    ~ColourPopup() override;

    void createColorPopup();
    void doDialog(QPoint p);

    void destroy() {
        reject();
    }

signals:
    void colorSelected(QRgb color);

private:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);
    
    static const int NUM_COLORS = 40;
    static const QRgb g_ColourArray[NUM_COLORS];

    QRgb _colour = qRgb(0xFF, 0xFF, 0xFF);
    
    QVector<QPushButton*> _colorButtons;
    QGridLayout* _pGridLayout = nullptr;
    
    static QColor getColorFromArray(int index);
};