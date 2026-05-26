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

#include "TaskList.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>

TaskList::TaskList()
    : Window()
{
    _rc.setRect(0, 0, 150, 0);
}

TaskList::~TaskList()
{
    destroyFont();
}

void TaskList::init(QWidget* parent, int nbItem, int index2set)
{
    Window::init(parent);
    _nbItem = nbItem;
    _currentIndex = index2set;
    
    _pListWidget = new QListWidget(parent);
    _pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _pListWidget->setAlternatingRowColors(true);
    
    // Populate with placeholder items
    for (int i = 0; i < nbItem; ++i) {
        _pListWidget->addItem(QString("Document %1").arg(i + 1));
    }
    
    if (index2set >= 0 && index2set < nbItem) {
        _pListWidget->setCurrentRow(index2set);
    }
    
    // Connect signals
    connect(_pListWidget, &QListWidget::itemClicked, this, [this]() {
        emit taskSelected(_pListWidget->currentRow());
    });
    connect(_pListWidget, &QListWidget::itemDoubleClicked, this, [this]() {
        emit taskActivated(_pListWidget->currentRow());
    });
}

void TaskList::destroy()
{
    destroyFont();
    if (_pListWidget) {
        _pListWidget->deleteLater();
        _pListWidget = nullptr;
    }
}

void TaskList::setFont(int fontSize, const QString& fontName)
{
    QString name = fontName.isEmpty() ? "MS Sans Serif" : fontName;
    _font = QFont(name, fontSize);
    _fontSelected = QFont(name, fontSize, QFont::Bold);
    
    if (_pListWidget) {
        _pListWidget->setFont(_font);
    }
}

void TaskList::destroyFont()
{
    _font = QFont();
    _fontSelected = QFont();
}

QRect TaskList::adjustSize()
{
    if (!_pListWidget) return _rc;
    
    // Calculate required size based on content
    int maxWidth = 150;
    QRect rect = _pListWidget->visualRect(_pListWidget->model()->index(0, 0));
    
    _rc.setRect(0, 0, maxWidth, _nbItem * rect.height());
    
    return _rc;
}

int TaskList::updateCurrentIndex()
{
    if (_pListWidget) {
        _currentIndex = _pListWidget->currentRow();
    }
    return _currentIndex;
}

void TaskList::moveSelection(int direction)
{
    if (!_pListWidget) return;
    
    int newIndex = _currentIndex + direction;
    if (newIndex < 0) newIndex = _nbItem - 1;
    if (newIndex >= _nbItem) newIndex = 0;
    
    _pListWidget->setCurrentRow(newIndex);
    _currentIndex = newIndex;
}

void TaskList::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Up:
            moveSelection(-1);
            event->accept();
            break;
        case Qt::Key_Down:
            moveSelection(1);
            event->accept();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit taskActivated(_currentIndex);
            event->accept();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}

void TaskList::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}