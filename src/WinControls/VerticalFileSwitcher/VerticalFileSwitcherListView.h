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

#include <QListWidget>
#include <QString>

#include "Window.h"
#include "Window/Widget.h"

class VerticalFileSwitcherListView : public Widget< QListWidget >
{
    Q_OBJECT

public:
    VerticalFileSwitcherListView() = default;
    ~VerticalFileSwitcherListView() override = default;

    void init(QWidget* parent);
    void destroy();

    void setList(const QVector<QString>& entryNameArray);
    void getList(QVector<QString>& targetArray) const;

    QString getItemName(int index) const;
    void setCurrentIndex(int index);

    void setListItemTooltip(size_t index, const QString& tooltip);
    int getCurrentIndex() const { return _currentIndex; }

private:
    QVector<QString> _entryStrings;
    int _currentIndex = 0;
};