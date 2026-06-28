// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: ColourPopup replaces the Win32 owner-drawn colour grid popup.
// Win32: owner-drawn ListBox with custom WM_MEASUREITEM/WM_DRAWITEM.
// Qt6:   QListWidget with custom item delegates + QColorDialog for custom colours.

#pragma once

#include <QColor>
#include <QDialog>
#include <QListWidget>
#include <QPoint>
#include <QWidget>

#include "ColourPopupResource.h"

// Custom message: sent when user picks a colour (replaces WM_PICKUP_COLOR)
#define WM_PICKUP_COLOR  (0xFFFF0000U + 1)  // NPP internal message range

class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    ColourPopup();
    explicit ColourPopup(const QColor &defaultColor);

    void doPopup(const QPoint &p);
    void createColorPopup();

Q signals:
    void colorPicked(const QColor &color);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onListCurrentRowChanged(int row);
    void onCustomColorClicked();
    void onChooseColorDialogAccepted();

private:
    void populateColorList();
    void applyDarkModeIfEnabled();
    QColor currentColor() const;

    QListWidget *_colorList = nullptr;
    QColor _colour = Qt::white;
    QWidget *_parent = nullptr;
    QColorDialog *_colorDialog = nullptr;
};