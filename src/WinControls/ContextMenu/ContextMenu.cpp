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

#include "ContextMenu.h"

#include <QAction>
#include <QMenuBar>
#include <QApplication>

void ContextMenu::create(QWidget* hParent, const QVector<MenuItemUnit>& menuItemArray, QMenu* mainMenuHandle, bool copyLink)
{
    Q_UNUSED(mainMenuHandle);
    Q_UNUSED(copyLink);

    _hParent = hParent;
    _menuItems = menuItemArray;

    clear();
    _actionMap.clear();
    _subMenus.clear();

    bool lastIsSep = false;
    QMenu* hParentFolder = nullptr;
    QString currentParentFolderStr;
    int folderIndex = 0;

    for (int i = 0; i < menuItemArray.size(); ++i) {
        const MenuItemUnit& item = menuItemArray[i];

        if (item._parentFolderName.isEmpty()) {
            // No parent folder
            currentParentFolderStr.clear();
            hParentFolder = nullptr;
            folderIndex = 0;
        } else {
            // Check if we're starting a new folder
            if (item._parentFolderName != currentParentFolderStr) {
                currentParentFolderStr = item._parentFolderName;
                hParentFolder = new QMenu(currentParentFolderStr, this);
                _subMenus.append(hParentFolder);
                addMenu(hParentFolder);
                folderIndex = 0;
            }
        }

        QAction* action = nullptr;

        if (item._cmdID == 0) {
            // Separator
            if ((i == 0 || i == menuItemArray.size() - 1)) {
                lastIsSep = true;
            } else if (!lastIsSep) {
                QAction* sep = addSeparator();
                Q_UNUSED(sep);
                lastIsSep = true;
            }
            continue;
        }

        lastIsSep = false;

        if (hParentFolder) {
            action = hParentFolder->addAction(item._itemName);
        } else {
            action = addAction(item._itemName);
        }

        if (action) {
            action->setData(QVariant::fromValue(static_cast<int>(item._cmdID)));
            _actionMap[item._cmdID] = action;

            connect(action, &QAction::triggered, this, [this, item]() {
                emit menuItemClicked(item._cmdID);
            });
        }
    }

    // Add "Copy link" option if requested
    Q_UNUSED(copyLink);
}

// Note: display(const QPoint&) is defined inline in ContextMenu.h