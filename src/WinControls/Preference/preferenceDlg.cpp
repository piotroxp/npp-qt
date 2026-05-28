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

#include "preferenceDlg.h"

// Win32 macro stubs for Linux Qt6 port
#ifndef WM_INITDIALOG
#define WM_INITDIALOG 0x0110
#endif
#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif
#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif
#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef IDOK
#define IDOK 1
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef IDYES
#define IDYES 6
#endif
#ifndef IDNO
#define IDNO 7
#endif
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

PreferenceDlg::PreferenceDlg()
    : StaticDialog()
{
    setWindowTitle("Preferences");
}

void PreferenceDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(0);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Create tab widget
        _pTabWidget = new QTabWidget(this);
        mainLayout->addWidget(_pTabWidget);
        
        // Add preference categories
        makeCategoryList();
        
        // Button row
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        
        QPushButton* closeBtn = new QPushButton("Close", this);
        connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
        btnLayout->addWidget(closeBtn);
        
        mainLayout->addLayout(btnLayout);
    }
    
    display(true);
}

void PreferenceDlg::destroy()
{
    // Would clean up sub-dialogs
}

void PreferenceDlg::makeCategoryList()
{
    // Add preference category tabs
    _pTabWidget->addTab(new QWidget(), "General");
    _pTabWidget->addTab(new QWidget(), "Editing");
    _pTabWidget->addTab(new QWidget(), "Preferences");
    _pTabWidget->addTab(new QWidget(), "Language");
    _pTabWidget->addTab(new QWidget(), "Integration");
    _pTabWidget->addTab(new QWidget(), "Cloud & Link");
    _pTabWidget->addTab(new QWidget(), "Search Engine");
    _pTabWidget->addTab(new QWidget(), "Backup");
    _pTabWidget->addTab(new QWidget(), "Auto-Completion");
    _pTabWidget->addTab(new QWidget(), "Multi-Instance");
    _pTabWidget->addTab(new QWidget(), "Delimiter");
    _pTabWidget->addTab(new QWidget(), "Print");
    _pTabWidget->addTab(new QWidget(), "Performance");
    _pTabWidget->addTab(new QWidget(), "Recent Files History");
    
    // Build category map
    _categoryNames = {
        "General", "Editing", "Preferences", "Language", "Integration",
        "Cloud & Link", "Search Engine", "Backup", "Auto-Completion",
        "Multi-Instance", "Delimiter", "Print", "Performance", "Recent Files History"
    };
    
    for (int i = 0; i < _categoryNames.size(); ++i) {
        _categoryMap[_categoryNames[i]] = i;
    }
}

int PreferenceDlg::getIndexFromName(const QString& name) const
{
    return _categoryMap.value(name, -1);
}

void PreferenceDlg::showDialogByName(const QString& name) const
{
    int index = getIndexFromName(name);
    if (index >= 0) {
        _pTabWidget->setCurrentIndex(index);
    }
}

void PreferenceDlg::showDialogByIndex(size_t index) const
{
    if (index < static_cast<size_t>(_pTabWidget->count())) {
        _pTabWidget->setCurrentIndex(static_cast<int>(index));
    }
}

bool PreferenceDlg::setListSelection(size_t currentSel) const
{
    if (currentSel < static_cast<size_t>(_pTabWidget->count())) {
        _pTabWidget->setCurrentIndex(static_cast<int>(currentSel));
        return true;
    }
    return false;
}

int PreferenceDlg::getListSelectedIndex() const
{
    return _pTabWidget->currentIndex();
}

bool PreferenceDlg::renameDialogTitle(const QString& internalName, const QString& newName)
{
    Q_UNUSED(internalName);
    Q_UNUSED(newName);
    // Would rename a category tab
    return true;
}

bool PreferenceDlg::goToSection(size_t iPage)
{
    if (iPage < static_cast<size_t>(_pTabWidget->count())) {
        _pTabWidget->setCurrentIndex(static_cast<int>(iPage));
        return true;
    }
    return false;
}

intptr_t PreferenceDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
            
        case WM_COMMAND:
            if (wParam == IDOK || wParam == IDCANCEL) {
                display(false);
                return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}