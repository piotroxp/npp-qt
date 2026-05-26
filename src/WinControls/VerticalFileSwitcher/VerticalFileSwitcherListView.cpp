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

#include "VerticalFileSwitcherListView.h"

void VerticalFileSwitcherListView::init(QWidget* parent)
{
    _hwnd = new QListWidget(parent);
    _hwnd->setSelectionMode(QAbstractItemView::SingleSelection);
    
    connect(_hwnd, &QListWidget::currentRowChanged, this, [this]() {
        _currentIndex = _hwnd->currentRow();
    });
}

void VerticalFileSwitcherListView::destroy()
{
    _entryStrings.clear();
}

void VerticalFileSwitcherListView::setList(const QVector<QString>& entryNameArray)
{
    _entryStrings = entryNameArray;
    
    if (_hwnd) {
        _hwnd->clear();
        
        for (const QString& entry : entryNameArray) {
            _hwnd->addItem(entry);
        }
    }
}

void VerticalFileSwitcherListView::getList(QVector<QString>& targetArray) const
{
    targetArray = _entryStrings;
}

QString VerticalFileSwitcherListView::getItemName(int index) const
{
    if (index >= 0 && index < _entryStrings.size()) {
        return _entryStrings[index];
    }
    return QString();
}

void VerticalFileSwitcherListView::setCurrentIndex(int index)
{
    _currentIndex = index;
    if (_hwnd && index >= 0 && index < _hwnd->count()) {
        _hwnd->setCurrentRow(index);
    }
}

void VerticalFileSwitcherListView::setListItemTooltip(size_t index, const QString& tooltip)
{
    if (index < static_cast<size_t>(_hwnd->count())) {
        QListWidgetItem* item = _hwnd->item(static_cast<int>(index));
        if (item) {
            item->setToolTip(tooltip);
        }
    }
}