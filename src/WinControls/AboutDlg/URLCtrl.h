// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QLabel>
#include <QWidget>
#include <QString>

// URLCtrl - Clickable URL control
// In Qt, this is replaced with QLabel that has linkActivated signal
// or a simple QLabel with openExternalLinks enabled

class URLCtrl : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString linkColor READ linkColor WRITE setLinkColor)
    Q_PROPERTY(QString visitedColor READ visitedColor WRITE setVisitedColor)

public:
    explicit URLCtrl(QWidget* parent = nullptr)
        : QLabel(parent)
        , _linkColor("#0000FF")
        , _visitedColor("#800080")
        , _clicking(false)
    {
        setTextFormat(Qt::RichText);
        setOpenExternalLinks(false);
        connect(this, &QLabel::linkActivated, this, &URLCtrl::onLinkActivated);
    }

    // Initialize with parent window handle (for Qt shim)
    void init(QWidget* parent, QWidget* msgDest = nullptr)
    {
        Q_UNUSED(parent);
        _msgDest = msgDest ? msgDest : parent;
    }

    // Create URL link on a label widget
    void create(QLabel* label, const QString& url)
    {
        _URL = url;
        label->setText(QString("<a href=\"%1\" style=\"color: %2;\">%1</a>")
                       .arg(url)
                       .arg(_linkColor));
        label->setOpenExternalLinks(false);
        
        // Disconnect default and connect to our handler
        disconnect(label, &QLabel::linkActivated, nullptr, nullptr);
        connect(label, &QLabel::linkActivated, this, &URLCtrl::onLinkActivated);
    }

    // Create URL link with command ID
    void create(QLabel* label, int cmd, QWidget* msgDest = nullptr)
    {
        _cmdID = cmd;
        _msgDest = msgDest ? msgDest : label->window();
        
        // For command-based links, we still need a URL for display
        QString displayText = label->text().isEmpty() ? _URL : label->text();
        create(label, displayText);
    }

    void destroy()
    {
        // Nothing to destroy - QLabel is managed by Qt
    }

    QString linkColor() const { return _linkColor; }
    void setLinkColor(const QString& color) {
        _linkColor = color;
        updateStyle();
    }

    QString visitedColor() const { return _visitedColor; }
    void setVisitedColor(const QString& color) {
        _visitedColor = color;
    }

    void updateStyle() {
        setStyleSheet(QString("QLabel { color: %1; }").arg(_linkColor));
    }

signals:
    void linkClicked(const QString& url);
    void commandTriggered(int cmd);

private slots:
    void onLinkActivated(const QString& link)
    {
        action();
        Q_UNUSED(link);
    }

private:
    void action()
    {
        if (_cmdID != 0 && _msgDest) {
            // Emit command signal for command-based links
            emit commandTriggered(_cmdID);
        } else if (!_URL.isEmpty()) {
            // Open URL in browser
            emit linkClicked(_URL);
        }
    }

    QString _URL;
    QString _linkColor;
    QString _visitedColor;
    bool _clicking;
    
    QWidget* _msgDest = nullptr;
    int _cmdID = 0;
};