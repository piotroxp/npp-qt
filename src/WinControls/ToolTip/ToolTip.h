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

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QRect>

// ToolTip - Custom tooltip control
// Replaces Win32 tooltip with Qt-based custom tooltip
class ToolTip : public QLabel
{
    Q_OBJECT

public:
    ToolTip(QWidget* parent = nullptr);
    ~ToolTip() override = default;

    void init(QWidget* parent);
    void destroy();

    void show(const QRect& rectTitle, const QString& pszTitleText, int iXOff = 0, int iWidthOff = 0);
    void hide();

private:
    bool _bTrackMouse = false;
    QWidget* _hParent = nullptr;
};