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

// Semantic Lift: Win32 URLCtrl → Qt6 QLabel-based hyperlink widget

#include "URLCtrl.h"

#include <QApplication>
#include <QDesktopServices>
#include <QEvent>
#include <QPalette>
#include <QFont>
#include <QCursor>

// Static cursor cache
QCursor URLCtrl::s_handCursor(Qt::PointingHandCursor);
bool URLCtrl::s_cursorInitialized = false;

// static
void URLCtrl::create(QLabel* label, const QString& link, const QColor& linkColor)
{
    if (!label)
        return;

    // Ensure URLCtrl is the actual widget (it inherits QLabel)
    URLCtrl* ctrl = qobject_cast<URLCtrl*>(label);
    if (!ctrl) {
        // Replace the label's parent with URLCtrl by reparenting
        QWidget* parent = label->parentWidget();
        ctrl = new URLCtrl(link, label->text(), parent);
        ctrl->_url = link;
        ctrl->_linkColor = linkColor;
        ctrl->_visitedColor = linkColor.darker(150);
        ctrl->_clicking = false;

        // Style: underlined, colored text
        QFont f = ctrl->font();
        f.setUnderline(true);
        ctrl->setFont(f);
        ctrl->setStyleSheet(QString(
            "URLCtrl { color: %1; }"
            "URLCtrl:hover { color: %2; }"
        ).arg(linkColor.name()).arg(linkColor.darker(120).name()));

        // Cursor
        ctrl->setCursor(Qt::PointingHandCursor);

        // Move and resize to match original label position
        ctrl->setGeometry(label->geometry());
        ctrl->setText(label->text());
        ctrl->setAlignment(label->alignment());

        // URL mode: clicking calls action() which opens URL directly; no signal needed

        label->hide();
        // label will be deleted by parent if owned
    } else {
        ctrl->_url = link;
        ctrl->_linkColor = linkColor;
    }
}

// static
void URLCtrl::create(QLabel* label, int cmd, QWidget* msgDest)
{
    if (!label)
        return;

    URLCtrl* ctrl = qobject_cast<URLCtrl*>(label);
    if (!ctrl) {
        QWidget* parent = label->parentWidget();
        ctrl = new URLCtrl(QString(), label->text(), parent);
        ctrl->_cmdID = cmd;
        ctrl->_msgDest = msgDest;
        ctrl->_linkColor = QColor(0, 0, 255);
        ctrl->_visitedColor = QColor(128, 0, 128);
        ctrl->_clicking = false;

        QFont f = ctrl->font();
        f.setUnderline(true);
        ctrl->setFont(f);
        ctrl->setStyleSheet(QString(
            "URLCtrl { color: %1; }"
            "URLCtrl:hover { color: %2; }"
        ).arg(ctrl->_linkColor.name()).arg(ctrl->_linkColor.darker(120).name()));

        ctrl->setCursor(Qt::PointingHandCursor);
        ctrl->setGeometry(label->geometry());
        ctrl->setText(label->text());
        ctrl->setAlignment(label->alignment());

        // Connect click signal to send command
        QObject::connect(ctrl, &URLCtrl::linkClicked,
                         ctrl, [msgDest, cmd](int /*clickedCmd*/) {
                             if (msgDest) {
                                 QEvent e(QEvent::Type(QEvent::User + cmd));
                                 QApplication::sendEvent(msgDest, &e);
                             }
                         });

        label->hide();
    } else {
        ctrl->_cmdID = cmd;
        ctrl->_msgDest = msgDest;
    }
}

QCursor URLCtrl::loadHandCursor()
{
    return QCursor(Qt::PointingHandCursor);
}

void URLCtrl::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _clicking = true;
        QLabel::mousePressEvent(event);
    } else {
        QLabel::mousePressEvent(event);
    }
}

void URLCtrl::action()
{
    if (!_url.isEmpty()) {
        // Open URL in default browser
        QDesktopServices::openUrl(QUrl(_url));
    } else if (_cmdID != 0 && _msgDest) {
        // Send command notification to destination widget
        QEvent* evt = new QEvent(QEvent::Type(QEvent::User + _cmdID));
        QApplication::postEvent(_msgDest, evt);
    }
}