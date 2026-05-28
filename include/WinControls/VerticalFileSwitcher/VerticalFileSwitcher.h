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

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include "../StaticDialog/StaticDialog.h"

// VerticalFileSwitcher - Ctrl+Tab file switcher panel
class VerticalFileSwitcher : public StaticDialog
{
    Q_OBJECT

public:
    VerticalFileSwitcher();
    ~VerticalFileSwitcher() override = default;

    void init(QWidget* parent);
    void destroy();
    void popUp() {
        display(true);
    }

    void addEntry(const QString& entryText);
    void removeCurrentEntry();
    void switchDown();
    void switchUp();
    void removeAllEntries();

    size_t getCurrentIndex() const { return _currentIndex; }
    size_t getNbEntry() const { return _entryStrings.size(); }

    bool isListOutOfSite() const { return !isVisible(); }
    void setListOutOfSite(bool isOutOfSite) {
        if (isOutOfSite) {
            hide();
        } else {
            popUp();
        }
    }

signals:
    void verticalFileSwitcherWantsToChange(int count);
    void closing(int currentIndex);
    void dropWholeList();
    void deleteCurrent();

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private:
    void selectEntry();
    void updateList();

    QListWidget* _pFileList = nullptr;
    QVector<QString> _entryStrings;
    size_t _currentIndex = 0;
    size_t _previousIndex = 0;
};

// Vertical file switcher direction
enum SwitcherMode {
    SwitcherDirection_down = -1,
    SwitcherDirection_up = 1
};