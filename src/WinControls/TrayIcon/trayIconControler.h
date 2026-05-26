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

#include <QSystemTrayIcon>
#include <QString>
#include <QIcon>
#include <QMenu>
#include <QAction>

// trayIconControler - System tray icon management
// Replaces Win32 Shell_NotifyIcon with Qt QSystemTrayIcon
class trayIconControler : public QSystemTrayIcon
{
    Q_OBJECT

public:
    trayIconControler(QWidget* hwnd, unsigned int uID, unsigned int uCBMsg, const QIcon& icon, const QString& tip);
    ~trayIconControler() override;

    int doTrayIcon(unsigned int op);
    bool isInTray() const { return _isIconShown; }
    int reAddTrayIcon();

signals:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void trayIconDoubleClicked();

public slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    unsigned int _uID = 0;
    unsigned int _uCBMsg = 0;
    bool _isIconShown = false;
    QWidget* _parentWindow = nullptr;
    
    // Error codes
    enum {
        INCORRECT_OPERATION = 1,
        OPERATION_INCOHERENT = 2
    };
};

// Add/Remove operations
#define ADD NIM_ADD
#define REMOVE NIM_DELETE