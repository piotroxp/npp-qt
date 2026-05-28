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

#include "WindowsDlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#ifdef _WIN32
#include <Windows.h>
#else
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
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif
#endif // !_WIN32

WindowsDlg::WindowsDlg()
{
    setCaption("Windows");
}

void WindowsDlg::init(QWidget* parent)
{
    StaticDialog::init(parent);
}

void WindowsDlg::run()
{
    if (!isCreated()) {
        create(0);
        setWindowTitle("Windows");
    }

    populateWindowList();
    display(true);
}

void WindowsDlg::populateWindowList()
{
    _pWindowList->clear();

#ifdef _WIN32
    // Enumerate windows using EnumWindows
    // For each visible window, add it to the list
    // This would require Windows-specific code
    Q_UNUSED(_hCurrentWindow);
#endif

    // Would scan desktop windows
    _pWindowList->addItem("Untitled - Notepad++");
}

void WindowsDlg::onItemDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    restoreSelectedWindow();
}

void WindowsDlg::restoreSelectedWindow()
{
    // Restore the selected window
}

void WindowsDlg::minimizeSelectedWindow()
{
    // Minimize the selected window
}

void WindowsDlg::closeAllOtherWindows()
{
    // Close all windows except the selected one
}

intptr_t WindowsDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);

            QLabel* infoLabel = new QLabel("Select a window:", this);
            mainLayout->addWidget(infoLabel);

            _pWindowList = new QListWidget(this);
            _pWindowList->setSelectionMode(QAbstractItemView::SingleSelection);
            connect(_pWindowList, &QListWidget::itemDoubleClicked,
                    this, &WindowsDlg::onItemDoubleClicked);
            mainLayout->addWidget(_pWindowList);

            QHBoxLayout* btnLayout = new QHBoxLayout();

            QPushButton* restoreBtn = new QPushButton("Restore", this);
            restoreBtn->connect(restoreBtn, &QPushButton::clicked,
                               this, &WindowsDlg::restoreSelectedWindow);
            btnLayout->addWidget(restoreBtn);

            QPushButton* minimizeBtn = new QPushButton("Minimize", this);
            minimizeBtn->connect(minimizeBtn, &QPushButton::clicked,
                                this, &WindowsDlg::minimizeSelectedWindow);
            btnLayout->addWidget(minimizeBtn);

            QPushButton* closeOthersBtn = new QPushButton("Close Others", this);
            closeOthersBtn->connect(closeOthersBtn, &QPushButton::clicked,
                                this, &WindowsDlg::closeAllOtherWindows);
            btnLayout->addWidget(closeOthersBtn);

            QPushButton* closeBtn = new QPushButton("Close", this);
            connect(closeBtn, &QPushButton::clicked,
                   this, [this]() { display(false); });
            btnLayout->addWidget(closeBtn);

            mainLayout->addLayout(btnLayout);

            populateWindowList();
            return TRUE;
        }
    }

    return StaticDialog::run_dlgProc(message, wParam, lParam);
}