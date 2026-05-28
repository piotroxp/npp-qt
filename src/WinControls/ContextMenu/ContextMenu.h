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
#include "MISC/Common/Common.h"
#include <QMenu>
#include <QAction>
#include <QWidget>

struct MenuItemUnit final
{
	unsigned long _cmdID = 0;
	std::wstring _itemName;
	std::wstring _parentFolderName;

	MenuItemUnit() = default;
	MenuItemUnit(unsigned long cmdID, const std::wstring& itemName, const std::wstring& parentFolderName = std::wstring())
		: _cmdID(cmdID), _itemName(itemName), _parentFolderName(parentFolderName) {}
	MenuItemUnit(unsigned long cmdID, const wchar_t* itemName, const wchar_t* parentFolderName = nullptr);
};


class ContextMenu final
{
public:
	~ContextMenu() {
		destroy();
	}

	void create(QWidget* hParent, const std::vector<MenuItemUnit> & menuItemArray, QMenu* mainMenuHandle = nullptr, bool copyLink = false);
	bool isCreated() const { return _menu != nullptr; }

	void display(const QPoint& p) const {
		if (_menu) {
			_menu->popup(p);
		}
	}

	void display(QWidget* hwnd) const;

	void enableItem(int cmdID, bool doEnable) const;
	bool isItemEnabled(int cmdID) const;
	void checkItem(int cmdID, bool doCheck) const;
	bool isItemChecked(int cmdID) const;

	QMenu* getMenuHandle() const { return _menu; }

	void destroy();

	// Helper: find action by ID
	QAction* findAction(int cmdID) const;

private:
	QWidget* _hParent = nullptr;
	QMenu* _menu = nullptr;
	std::vector<QMenu*> _subMenus;
};
