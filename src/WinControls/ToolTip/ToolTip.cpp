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

#include "ToolTip.h"

#include <QFont>
#include <QStyle>
#include <QApplication>

ToolTip::ToolTip(QWidget* parent)
    : QLabel(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setTextFormat(Qt::PlainText);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMargin(3);
    
    // Style similar to Windows tooltips
    setStyleSheet(R"(
        QLabel {
            background-color: #FFFFE1;
            color: #000000;
            border: 1px solid #808080;
            padding: 2px;
            font: 8pt 'MS Shell Dlg 2';
        }
    )");
}

void ToolTip::init(QWidget* parent)
{
    _hParent = parent;
    
    // Inherit font from parent
    if (parent) {
        setFont(parent->font());
    }
}

void ToolTip::destroy()
{
    hide();
    deleteLater();
}

void ToolTip::show(const QRect& rectTitle, const QString& pszTitleText, int iXOff, int iWidthOff)
{
    if (isVisible()) {
        hide();
    }
    
    if (pszTitleText.isEmpty()) return;
    
    setText(pszTitleText);
    
    // Position the tooltip below the title area
    int x = rectTitle.left() + iXOff;
    int y = rectTitle.top() + rectTitle.height() + iWidthOff;
    
    // Ensure tooltip stays on screen
    QPoint pos(x, y);
    QRect screenRect = QApplication::screenAt(pos)->geometry();
    
    // Adjust if tooltip would go off-screen
    QSize tipSize = sizeHint();
    if (pos.x() + tipSize.width() > screenRect.right()) {
        pos.setX(screenRect.right() - tipSize.width());
    }
    if (pos.y() + tipSize.height() > screenRect.bottom()) {
        pos.setY(rectTitle.top() - tipSize.height());
    }
    
    move(pos);
    QLabel::show();
}

void ToolTip::hide()
{
    QLabel::hide();
}