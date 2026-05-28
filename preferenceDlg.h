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
#include <QTabWidget>
#include <QString>
#include <QMap>
#include <QVector>

#include "../StaticDialog/StaticDialog.h"

// PreferenceDlg - Preferences/Settings dialog
// Replaces MFC CPropertySheet with Qt QTabWidget
class PreferenceDlg : public StaticDialog
{
    Q_OBJECT

public:
    PreferenceDlg();
    ~PreferenceDlg() override = default;

    void doDialog(bool isRTL = false);
    void destroy();
    bool renameDialogTitle(const QString& internalName, const QString& newName);
    
    int getListSelectedIndex() const;
    void showDialogByName(const QString& name) const;
    bool setListSelection(size_t currentSel) const;
    bool goToSection(size_t iPage);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private:
    void makeCategoryList();
    int getIndexFromName(const QString& name) const;
    void showDialogByIndex(size_t index) const;

    QTabWidget* _pTabWidget = nullptr;
    QMap<QString, int> _categoryMap;
    QVector<QString> _categoryNames;
    
    // Sub-dialogs would be added here
    // GeneralSubDlg, EditingSubDlg, etc.
};