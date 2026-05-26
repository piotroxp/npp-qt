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

#include "ansiCharPanel.h"
#include "ListView.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QHeaderView>

AnsiCharPanel::AnsiCharPanel()
    : DockingDlgInterface(IDD_ANSIASCII_PANEL)
{
    setWindowTitle(AI_PROJECTPANELTITLE);
}

void AnsiCharPanel::init(QWidget* parent, ScintillaEditView** ppEditView)
{
    DockingDlgInterface::init(parent);
    _ppEditView = ppEditView;
}

void AnsiCharPanel::setParent(QWidget* parent2set)
{
    _hParent = parent2set;
}

void AnsiCharPanel::switchEncoding()
{
    // Get current encoding from scintilla and update display
    // if (_ppEditView && *_ppEditView) {
    //     _currentCodepage = (*_ppEditView)->getCurrentBuffer()->getEncoding();
    // }
    updateCharDisplay();
}

void AnsiCharPanel::insertChar(unsigned char char2insert) const
{
    QString ch = QString(QChar(char2insert));
    insertString(ch);
    
    if (_ppEditView && *_ppEditView) {
        //(*_ppEditView)->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(ch.toUtf8().constData()));
    }
    
    emit insertCharacter(QChar(char2insert));
}

void AnsiCharPanel::insertString(const QString& string2insert) const
{
    if (_ppEditView && *_ppEditView) {
        //(*_ppEditView)->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(string2insert.toUtf8().constData()));
    }
    
    emit insertText(string2insert);
}

void AnsiCharPanel::setBackgroundColor(QRgb bgColour)
{
    _backgroundColor = bgColour;
    if (_pCharList) {
        QPalette pal = _pCharList->palette();
        pal.setColor(QPalette::Base, QColor::fromRgb(bgColour));
        _pCharList->setPalette(pal);
    }
}

void AnsiCharPanel::setForegroundColor(QRgb fgColour)
{
    _foregroundColor = fgColour;
    if (_pCharList) {
        QPalette pal = _pCharList->palette();
        pal.setColor(QPalette::Text, QColor::fromRgb(fgColour));
        _pCharList->setPalette(pal);
    }
}

void AnsiCharPanel::setupCharGrid()
{
    // Create a list widget for character display
    _pCharList = new QListWidget(this);
    _pCharList->setViewMode(QListWidget::IconMode);
    _pCharList->setGridSize(QSize(50, 60));
    _pCharList->setResizeMode(QListWidget::Adjust);
    _pCharList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    connect(_pCharList, &QListWidget::itemDoubleClicked, this, &AnsiCharPanel::onItemDoubleClicked);
    connect(_pCharList, &QListWidget::itemClicked, this, &AnsiCharPanel::onItemClicked);
    
    updateCharDisplay();
}

void AnsiCharPanel::updateCharDisplay()
{
    if (!_pCharList) return;
    
    _pCharList->clear();
    
    // Add all printable ASCII characters (32-126)
    for (unsigned char c = 32; c < 127; ++c) {
        QString label = QString("%1\n0x%2\n%3")
            .arg(c)
            .arg(c, 2, 16, QChar('0')).toUpper()
            .arg(QChar(c).isPrint() ? QString(QChar(c)) : ".");
        
        auto* item = new QListWidgetItem(label, _pCharList);
        item->setData(Qt::UserRole, c);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
    
    // Add extended ASCII (128-255) in a separate section
    for (unsigned char c = 128; c < 255; ++c) {
        QString label = QString("%1\n0x%2\n%3")
            .arg(c)
            .arg(c, 2, 16, QChar('0')).toUpper()
            .arg(".");
        
        auto* item = new QListWidgetItem(label, _pCharList);
        item->setData(Qt::UserRole, c);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void AnsiCharPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;
    
    unsigned char charValue = static_cast<unsigned char>(item->data(Qt::UserRole).toInt());
    
    // Insert the character
    insertChar(charValue);
}

void AnsiCharPanel::onItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    // Could show character info in status bar
}

intptr_t AnsiCharPanel::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            setupCharGrid();
            return TRUE;
        }
        
        case WM_SIZE: {
            if (_pCharList) {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                _pCharList->resize(width, height);
            }
            return TRUE;
        }
        
        case WM_NOTIFY: {
            // Handle dock close notification
            return TRUE;
        }
        
        default:
            break;
    }
    
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}