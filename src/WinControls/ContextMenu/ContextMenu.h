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

#include <QMenu>
#include <QString>
#include <QVector>
#include <QPoint>

struct MenuItemUnit {
    unsigned long _cmdID = 0;
    QString _itemName;
    QString _parentFolderName;

    MenuItemUnit() = default;
    MenuItemUnit(unsigned long cmdID, const QString& itemName, const QString& parentFolderName = QString())
        : _cmdID(cmdID), _itemName(itemName), _parentFolderName(parentFolderName) {}
};

// ContextMenu - Context (right-click) menu
// Replaces Win32 popup menus with Qt QMenu
class ContextMenu : public QMenu
{
    Q_OBJECT

public:
    ~ContextMenu() override {
        destroy();
    }

    void create(QWidget* hParent, const QVector<MenuItemUnit>& menuItemArray, QMenu* mainMenuHandle = nullptr, bool copyLink = false);
    bool isCreated() const { return !_menuItems.isEmpty(); }

    void display(const QPoint& p) const {
        if (!_menuItems.isEmpty()) {
            QMenu::popup(p);
        }
    }

    void displayAtCursor() const {
        display(QCursor::pos());
    }

    void display(QWidget* hwnd) const {
        Q_UNUSED(hwnd);
        display(QCursor::pos());
    }

    void enableItem(int cmdID, bool doEnable) const {
        auto it = _actionMap.find(cmdID);
        if (it != _actionMap.end() && it.value()) {
            it.value()->setEnabled(doEnable);
        }
    }

    void checkItem(int cmdID, bool doCheck) const {
        auto it = _actionMap.find(cmdID);
        if (it != _actionMap.end() && it.value()) {
            it.value()->setChecked(doCheck);
        }
    }

    QMenu* getMenuHandle() const { return const_cast<QMenu*>(static_cast<const QMenu*>(this)); }

    void destroy() {
        clear();
        _menuItems.clear();
        _actionMap.clear();
        _subMenus.clear();
    }

signals:
    void menuItemClicked(int cmdID);
    void aboutToShow();
    void aboutToHide();

private:
    QWidget* _hParent = nullptr;
    QVector<MenuItemUnit> _menuItems;
    QMap<int, QAction*> _actionMap;
    QVector<QMenu*> _subMenus;
};