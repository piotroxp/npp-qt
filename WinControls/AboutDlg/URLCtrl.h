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
// Win32: Subclassed static control with SS_NOTIFY for clickable hyperlinks
// Qt6:  QLabel subclass with linkActivated signal and command support

#pragma once

#include <QLabel>
#include <QColor>
#include <QFont>
#include <QWidget>
#include <QString>

class URLCtrl : public QLabel {
    Q_OBJECT

public:
    // Subclass ID for multiple URLCtrl instances
    enum class SubclassID { first = 0 };

    explicit URLCtrl(QWidget* parent = nullptr);
    ~URLCtrl();

    // Create with URL and custom color (Win32: create(HWND, wchar_t*, COLORREF))
    void create(QWidget* itemHandle, const QString& link, const QColor& linkColor = QColor());

    // Create with command ID and message destination (Win32: create(HWND, int, HWND))
    void create(QWidget* itemHandle, int cmd, QWidget* msgDest);

    // Set the display text with link styling
    void setLinkText(const QString& linkText);

    // Perform the link action
    void action();

    // Clean up resources
    void destroy();

    // Get current link color
    QColor linkColor() const { return _linkColor; }
    void setLinkColor(const QColor& color) { _linkColor = color; }

    // Get visited color
    QColor visitedColor() const { return _visitedColor; }

    // Load hand cursor (Qt6: always available via Qt::PointingHandCursor)
    QCursor& loadHandCursor();

    // Helper to open URL via desktop services (replaces ShellExecute)
    static void openUrl(const QString& url);

Q_SIGNALS:
    // Emitted when link is activated with command ID
    void commandTriggered(int cmdID);

protected:
    // Mouse press/release tracking for space-bar compatibility
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // Keyboard support for space-bar activation
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // Handle link activation from Qt's rich text
    void onLinkActivated(const QString& link);

    // Get background color from parent (dark mode support)
    QColor getCtrlBgColor(QWidget* parent) const;

private:
    QString _URL;                 // The hyperlink URL
    int _cmdID = 0;               // Command ID to emit when clicked
    QWidget* _msgDest = nullptr;  // Message destination widget
    QColor _linkColor;            // Hyperlink color
    QColor _visitedColor;         // Visited hyperlink color
    bool _clicking = false;       // Tracking mouse press state
    QFont* _hfUnderlined = nullptr; // Underline font (owner-drawn)
    QCursor* _hCursor = nullptr;  // Hand cursor
};