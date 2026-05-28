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
#include "MISC/Common/WindowsCompat.h"
#include "menuCmdID.h"
#include "../Parameters.h"
#include "localization.h"
#include <QApplication>

MenuItemUnit::MenuItemUnit(unsigned long cmdID, const wchar_t*     itemName, const wchar_t*     parentFolderName) : _cmdID(cmdID)
{
	if (!itemName)
		_itemName.clear();
	else
		_itemName = itemName;

	if (!parentFolderName)
		_parentFolderName.clear();
	else
		_parentFolderName = parentFolderName;
}

QAction* ContextMenu::findAction(int cmdID) const
{
	if (!_menu)
		return nullptr;
	const auto actions = _menu->actions();
	for (QAction* a : actions) {
		if (a->data().toLongLong() == static_cast<long long>(cmdID))
			return a;
		// check submenus
		QMenu* sm = a->menu();
		if (sm) {
			for (QAction* sa : sm->actions()) {
				if (sa->data().toLongLong() == static_cast<long long>(cmdID))
					return sa;
			}
		}
	}
	return nullptr;
}

void ContextMenu::create(QWidget* hParent, const std::vector<MenuItemUnit> & menuItemArray, QMenu* mainMenuHandle, bool copyLink)
{
	_hParent = hParent;
	_menu = new QMenu(hParent);

	bool lastIsSep = false;
	QMenu* hParentFolder = nullptr;
	std::wstring currentParentFolderStr;
	int j = 0;

	for (size_t i = 0, len = menuItemArray.size(); i < len; ++i)
	{
		const MenuItemUnit & item = menuItemArray[i];
		if (item._parentFolderName.empty())
		{
			currentParentFolderStr.clear();
			hParentFolder = nullptr;
			j = 0;
		}
		else
		{
			if (item._parentFolderName != currentParentFolderStr)
			{
				currentParentFolderStr = item._parentFolderName;
				hParentFolder = new QMenu(QString::fromWCharArray(currentParentFolderStr.c_str()), _menu);
				j = 0;

				_subMenus.push_back(hParentFolder);
				QAction* act = _menu->addMenu(hParentFolder);
				Q_UNUSED(act);
			}
		}

		if (item._cmdID == 0)
		{
			// separator
			if (hParentFolder)
			{
				if (!lastIsSep)
					hParentFolder->addSeparator();
			}
			else if ((i == 0 || i == menuItemArray.size() - 1) || !lastIsSep)
			{
				if (!lastIsSep)
					_menu->addSeparator();
			}
			lastIsSep = true;
		}
		else
		{
			QAction* act = nullptr;
			if (hParentFolder)
			{
				act = hParentFolder->addAction(QString::fromWCharArray(item._itemName.c_str()));
				lastIsSep = false;
			}
			else
			{
				act = _menu->addAction(QString::fromWCharArray(item._itemName.c_str()));
				lastIsSep = false;
			}
			act->setData(QVariant::fromValue<long long>(item._cmdID));
			lastIsSep = false;
		}

		if (mainMenuHandle)
		{
			// Mirror enabled/checked state from main menu
			QAction* mainAct = nullptr;
			for (QAction* a : mainMenuHandle->actions()) {
				if (a->data().toLongLong() == static_cast<long long>(item._cmdID)) {
					mainAct = a;
					break;
				}
				if (a->menu()) {
					for (QAction* sa : a->menu()->actions()) {
						if (sa->data().toLongLong() == static_cast<long long>(item._cmdID)) {
							mainAct = sa;
							break;
						}
					}
					if (mainAct) break;
				}
			}
			if (mainAct) {
				QAction* ourAct = findAction(item._cmdID);
				if (ourAct) {
					ourAct->setEnabled(mainAct->isEnabled());
					ourAct->setCheckable(true);
					ourAct->setChecked(mainAct->isChecked());
				}
			}
		}

		if (copyLink && (item._cmdID == IDM_EDIT_COPY))
		{
			NativeLangSpeaker* nativeLangSpeaker = NppParameters::getInstance().getNativeLangSpeaker();
			std::wstring localized = nativeLangSpeaker->getNativeLangMenuString(IDM_EDIT_COPY_LINK);
			if (localized.length() == 0)
				localized = L"Copy link";
			QAction* lastAct = _menu->actions().isEmpty() ? nullptr : _menu->actions().back();
			if (lastAct) {
				lastAct->setText(QString::fromWCharArray(localized.c_str()));
				lastAct->setData(QVariant::fromValue<long long>(IDM_EDIT_COPY_LINK));
			}
		}
	}
}

void ContextMenu::display(QWidget* hwnd) const
{
	if (!_menu)
		return;
	QWidget* parent = _hParent ? _hParent : hwnd;
	if (!parent)
		parent = QApplication::activeWindow();
	if (!parent)
		return;

	QRect rc = parent->rect();
	QPoint p = parent->mapToGlobal(QPoint(rc.left(), rc.bottom()));

	const NativeLangSpeaker* nativeLangSpeaker = NppParameters::getInstance().getNativeLangSpeaker();
	if (nativeLangSpeaker->isRTL())
	{
		p.setX(parent->mapToGlobal(QPoint(rc.right(), 0)).x());
		_menu->popup(p);
	}
	else
	{
		_menu->popup(p);
	}
}

void ContextMenu::enableItem(int cmdID, bool doEnable) const
{
	QAction* act = findAction(static_cast<int>(cmdID));
	if (act)
		act->setEnabled(doEnable);
}

bool ContextMenu::isItemEnabled(int cmdID) const
{
	QAction* act = findAction(static_cast<int>(cmdID));
	return act ? act->isEnabled() : false;
}

void ContextMenu::checkItem(int cmdID, bool doCheck) const
{
	QAction* act = findAction(static_cast<int>(cmdID));
	if (act) {
		act->setCheckable(true);
		act->setChecked(doCheck);
	}
}

bool ContextMenu::isItemChecked(int cmdID) const
{
	QAction* act = findAction(static_cast<int>(cmdID));
	return act ? act->isChecked() : false;
}

void ContextMenu::destroy()
{
	if (isCreated())
	{
		for (size_t i = 0, len = _subMenus.size(); i < len; ++i)
		{
			delete _subMenus[i];
			_subMenus[i] = nullptr;
		}
		_subMenus.clear();
		delete _menu;
		_menu = nullptr;
	}
}
