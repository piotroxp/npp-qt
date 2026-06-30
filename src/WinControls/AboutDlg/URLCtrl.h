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

#pragma once

#include <QWidget>
#include <QString>
#include <QColor>
#include <QLabel>
#include <QFont>
#include <QMouseEvent>
#include <QCursor>
#include <QDesktopServices>
#include <QUrl>

// Forward declaration
class URLCtrl;

// URLCtrl mimics Win32 static-link behavior: underlined text that opens a URL on click.
// In Qt6 this is cleanly implemented as a QLabel subclass with linkActivated.
// Supports both direct URL strings and command-based notification to a target widget.
class URLCtrl : public QLabel
{
    Q_OBJECT

public:
    // Constructor: creates a URLCtrl label widget
    explicit URLCtrl(const QString& link, const QString& text = QString(), QWidget* parent = nullptr);

    // Create URLCtrl attached to an existing QLabel widget (parent owns it)
    // link: the URL string to open
    // linkColor: text color for the link (default blue)
    static void create(QLabel* label, const QString& link, const QColor& linkColor = QColor(0, 0, 255));

    // Create URLCtrl that sends a command ID to a target widget via custom event
    // label: the QLabel to turn into a URLCtrl
    // cmd: command ID to send to msgDest
    // msgDest: widget to receive the notification
    static void create(QLabel* label, int cmd, QWidget* msgDest = nullptr);

    // Destroy: URLCtrl is owned by parent label, no explicit destroy needed
    void destroy() { /* nothing needed — parent QLabel manages lifetime */ }

Q_SIGNALS:
    // Emitted when user clicks the link (for command-based mode)
    void linkClicked(int cmdId);

protected:
    // Handle mouse press for click tracking
    void mousePressEvent(QMouseEvent* event) override;

private:
    // Load hand cursor for link hover (system standard link cursor)
    static QCursor loadHandCursor();

    // Perform the action: open URL or send command
    void action();

    QString _url;
    int _cmdID = 0;
    QWidget* _msgDest = nullptr;
    QColor _linkColor = QColor(255, 255, 255);
    QColor _visitedColor = QColor(255, 255, 255);
    bool _clicking = false;

    // Cached hand cursor (shared)
    static QCursor* s_handCursor;  // lazy — null until first handCursor() call
    static bool s_cursorInitialized;
};