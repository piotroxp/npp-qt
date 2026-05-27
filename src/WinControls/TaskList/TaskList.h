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

#include <QTableWidget>
#include <QListWidget>
#include <QString>
#include <QVector>
#include <QRect>

#include "../Window.h"

// TaskList - Task list panel
// Replaces MFC ListView-based task list with Qt QTableWidget
class TaskList : public Window
{
    Q_OBJECT

public:
    TaskList();
    ~TaskList() override;

    void init(QWidget* parent, int nbItem, int index2set);
    void destroy() override;

    void setFont(int fontSize, const QString& fontName = QString());
    void destroyFont();
    QRect adjustSize();

    int getCurrentIndex() const { return _currentIndex; }
    int updateCurrentIndex();

signals:
    void taskSelected(int index);
    void taskActivated(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void moveSelection(int direction);

    QListWidget* _pListWidget = nullptr;
    int _nbItem = 0;
    int _currentIndex = 0;
    QRect _rc;
    
    QFont _font;
    QFont _fontSelected;
};