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

#include "trayIconControler.h"

#include <QWidget>

trayIconControler::trayIconControler(QWidget* hwnd, unsigned int uID, unsigned int uCBMsg, 
                                      const QIcon& icon, const QString& tip)
    : QSystemTrayIcon(icon, hwnd)
    , _uID(uID)
    , _uCBMsg(uCBMsg)
    , _isIconShown(false)
    , _parentWindow(hwnd)
{
    setToolTip(tip);
    
    // Connect activation signal
    connect(this, &QSystemTrayIcon::activated, this, &trayIconControler::onActivated);
}

trayIconControler::~trayIconControler()
{
    // Remove icon if still shown
    if (_isIconShown) {
        hide();
    }
}

int trayIconControler::doTrayIcon(unsigned int op)
{
    if (op != ADD && op != REMOVE) {
        return INCORRECT_OPERATION;
    }

    if ((_isIconShown && op == ADD) || (!_isIconShown && op == REMOVE)) {
        return OPERATION_INCOHERENT;
    }

    if (op == ADD) {
        show();
        _isIconShown = true;
    } else {
        hide();
        _isIconShown = false;
    }

    return 0;
}

int trayIconControler::reAddTrayIcon()
{
    if (!_isIconShown) {
        return -1;
    }

    // Hide first to ensure no duplication
    hide();
    _isIconShown = false;
    
    // Re-add the tray icon
    return doTrayIcon(ADD);
}

void trayIconControler::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
            emit trayIconClicked(reason);
            break;
        case QSystemTrayIcon::DoubleClick:
            emit trayIconDoubleClicked();
            break;
        default:
            break;
    }
}