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

// Qt6 port: URLCtrl - hyperlink static control
// Win32: Static control with SS_NOTIFY + window subclassing + custom WM_PAINT
// Qt6:  QLabel subclass with linkActivated signal + overridden paintEvent()

#include "URLCtrl.h"

#include <QUrl>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QCursor>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QPalette>
#include <QApplication>
#include <QTimer>

URLCtrl::URLCtrl(QWidget* parent)
    : QLabel(parent)
    , _cmdID(0)
    , _msgDest(nullptr)
    , _linkColor(Qt::blue)
    , _visitedColor(QColor(128, 0, 128))
    , _clicking(false)
    , _hfUnderlined(nullptr)
    , _hCursor(nullptr)
{
    setTextFormat(Qt::RichText);
    setTextInteractionFlags(Qt::TextBrowserInteraction);
    setCursor(Qt::PointingHandCursor);
}

URLCtrl::~URLCtrl() {
    destroy();
}

void URLCtrl::create(QWidget* itemHandle, const QString& link, const QColor& linkColor) {
    // Set the URL text
    if (!link.isEmpty()) {
        _URL = link;
    }

    // Set the hyperlink color
    _linkColor = linkColor.isValid() ? linkColor : Qt::blue;

    // Set the visited color
    _visitedColor = QColor(128, 0, 128);

    // Set display text as rich text link if not already set
    if (text().isEmpty() && !_URL.isEmpty()) {
        setLinkText(_URL);
    }

    // Connect linkActivated signal to open URL
    connect(this, &QLabel::linkActivated, this, &URLCtrl::onLinkActivated);
}

void URLCtrl::create(QWidget* itemHandle, int cmd, QWidget* msgDest) {
    _cmdID = cmd;
    _msgDest = msgDest;

    // Set the hyperlink colour
    _linkColor = Qt::blue;

    // Connect linkActivated to emit command
    connect(this, &QLabel::linkActivated, this, &URLCtrl::onLinkActivated);
}

void URLCtrl::setLinkText(const QString& linkText) {
    // Create rich text with underline styling
    QString styled = QString("<a href=\"%1\" style=\"text-decoration: underline; color: %2\">%1</a>")
        .arg(linkText, _linkColor.name());
    setText(styled);
}

void URLCtrl::onLinkActivated(const QString& link) {
    if (_cmdID) {
        // Emit command signal instead of SendMessage
        emit commandTriggered(_cmdID);
    } else {
        _linkColor = _visitedColor;

        // Open URL in browser
        QUrl url(link);
        if (url.isValid()) {
            QDesktopServices::openUrl(url);
        } else {
            // Try opening as a URL from window text
            QDesktopServices::openUrl(QUrl(link));
        }
    }
}

void URLCtrl::destroy() {
    if (_hfUnderlined) {
        delete _hfUnderlined;
        _hfUnderlined = nullptr;
    }
}

void URLCtrl::action() {
    if (_cmdID) {
        emit commandTriggered(_cmdID);
    } else {
        _linkColor = _visitedColor;
        update();

        // Open URL
        if (!_URL.isEmpty()) {
            QDesktopServices::openUrl(QUrl(_URL));
        } else {
            QDesktopServices::openUrl(QUrl(text()));
        }
    }
}

// Mouse press tracking for space-bar activation compatibility
void URLCtrl::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        _clicking = true;
    }
    QLabel::mousePressEvent(event);
}

void URLCtrl::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && _clicking) {
        _clicking = false;
        action();
    }
    QLabel::mouseReleaseEvent(event);
}

// Support using space to activate this object
void URLCtrl::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        _clicking = true;
        event->accept();
    } else {
        QLabel::keyPressEvent(event);
    }
}

void URLCtrl::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && _clicking) {
        _clicking = false;
        action();
        event->accept();
    } else {
        QLabel::keyReleaseEvent(event);
    }
}

// Provide link color from parent (dark mode support)
QColor URLCtrl::getCtrlBgColor(QWidget* parent) const {
    if (parent) {
        return parent->palette().color(parent->backgroundRole());
    }
    return QColor(); // transparent
}

// Static helper: open URL in desktop services
// Replaces ShellExecute in Win32
/* static */ void URLCtrl::openUrl(const QString& url) {
    QDesktopServices::openUrl(QUrl(url));
}