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

#include "TreeView.h"

#include <QHeaderView>
#include <QDrag>
#include <QMimeData>
#include <QMenu>

TreeView::TreeView(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    
    // Expand/collapse signals
    connect(this, &QTreeWidget::itemExpanded, this, &TreeView::itemExpanded);
    connect(this, &QTreeWidget::itemCollapsed, this, &TreeView::itemCollapsed);
}

void TreeView::init(QWidget* parent, int treeViewID)
{
    Q_UNUSED(treeViewID);
    Q_UNUSED(parent);
    // Already set up in constructor
}

void TreeView::destroy()
{
    clear();
}

QTreeWidgetItem* TreeView::addItem(const QString& itemName, QTreeWidgetItem* hParentItem, int iImage, intptr_t lParam)
{
    auto* item = new QTreeWidgetItem();
    item->setText(0, itemName);
    item->setData(0, Qt::UserRole, QVariant::fromValue(lParam));
    
    if (iImage >= 0 && iImage < _imageList.size()) {
        item->setIcon(0, QIcon(_imageList[iImage]));
    }
    
    if (hParentItem) {
        hParentItem->addChild(item);
    } else {
        addTopLevelItem(item);
    }
    
    return item;
}

bool TreeView::setItemParam(QTreeWidgetItem* item, intptr_t param)
{
    if (item) {
        item->setData(0, Qt::UserRole, QVariant::fromValue(param));
        return true;
    }
    return false;
}

intptr_t TreeView::getItemParam(QTreeWidgetItem* item) const
{
    if (item) {
        return item->data(0, Qt::UserRole).toLongLong();
    }
    return 0;
}

QString TreeView::getItemDisplayName(QTreeWidgetItem* item) const
{
    if (item) {
        return item->text(0);
    }
    return QString();
}

QTreeWidgetItem* TreeView::searchSubItemByName(const QString& itemName, QTreeWidgetItem* hParentItem)
{
    if (hParentItem) {
        for (int i = 0; i < hParentItem->childCount(); ++i) {
            QTreeWidgetItem* child = hParentItem->child(i);
            if (child->text(0) == itemName) {
                return child;
            }
        }
    } else {
        for (int i = 0; i < topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = topLevelItem(i);
            if (item->text(0) == itemName) {
                return item;
            }
            QTreeWidgetItem* found = searchSubItemByName(itemName, item);
            if (found) return found;
        }
    }
    return nullptr;
}

void TreeView::removeItem(QTreeWidgetItem* hTreeItem)
{
    if (hTreeItem) {
        cleanSubEntries(hTreeItem);
        delete hTreeItem;
    }
}

void TreeView::removeAllItems()
{
    clear();
}

bool TreeView::renameItem(QTreeWidgetItem* item, const QString& newName)
{
    if (item) {
        item->setText(0, newName);
        return true;
    }
    return false;
}

void TreeView::makeLabelEditable(bool toBeEnabled)
{
    setEditTriggers(toBeEnabled ? QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed 
                                 : QAbstractItemView::NoEditTriggers);
}

QTreeWidgetItem* TreeView::getChildFrom(QTreeWidgetItem* hTreeItem) const
{
    if (hTreeItem && hTreeItem->childCount() > 0) {
        return hTreeItem->child(0);
    }
    return nullptr;
}

QTreeWidgetItem* TreeView::getSelection() const
{
    return currentItem();
}

bool TreeView::selectItem(QTreeWidgetItem* hTreeItem2Select) const
{
    if (hTreeItem2Select) {
        setCurrentItem(hTreeItem2Select);
        scrollToItem(hTreeItem2Select);
        return true;
    }
    return false;
}

QTreeWidgetItem* TreeView::getRoot() const
{
    if (topLevelItemCount() > 0) {
        return topLevelItem(0);
    }
    return nullptr;
}

QTreeWidgetItem* TreeView::getParent(QTreeWidgetItem* hItem) const
{
    if (hItem) {
        return hItem->parent();
    }
    return nullptr;
}

QTreeWidgetItem* TreeView::getNextSibling(QTreeWidgetItem* hItem) const
{
    if (!hItem) return nullptr;
    QTreeWidgetItem* parent = hItem->parent();
    
    if (parent) {
        int index = parent->indexOfChild(hItem);
        if (index >= 0 && index < parent->childCount() - 1) {
            return parent->child(index + 1);
        }
    } else {
        int index = indexOfTopLevelItem(hItem);
        if (index >= 0 && index < topLevelItemCount() - 1) {
            return topLevelItem(index + 1);
        }
    }
    return nullptr;
}

QTreeWidgetItem* TreeView::getPrevSibling(QTreeWidgetItem* hItem) const
{
    if (!hItem) return nullptr;
    QTreeWidgetItem* parent = hItem->parent();
    
    if (parent) {
        int index = parent->indexOfChild(hItem);
        if (index > 0) {
            return parent->child(index - 1);
        }
    } else {
        int index = indexOfTopLevelItem(hItem);
        if (index > 0) {
            return topLevelItem(index - 1);
        }
    }
    return nullptr;
}

void TreeView::expand(QTreeWidgetItem* hItem) const
{
    if (hItem) {
        hItem->setExpanded(true);
    }
}

void TreeView::fold(QTreeWidgetItem* hItem) const
{
    if (hItem) {
        hItem->setExpanded(false);
    }
}

void TreeView::foldExpandRecursively(QTreeWidgetItem* hItem, bool isFold) const
{
    if (!hItem) return;
    hItem->setExpanded(!isFold);
    
    for (int i = 0; i < hItem->childCount(); ++i) {
        foldExpandRecursively(hItem->child(i), isFold);
    }
}

void TreeView::foldExpandAll(bool isFold) const
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        foldExpandRecursively(topLevelItem(i), isFold);
    }
}

void TreeView::foldAll() const
{
    foldExpandAll(true);
}

void TreeView::expandAll() const
{
    foldExpandAll(false);
}

void TreeView::toggleExpandCollapse(QTreeWidgetItem* hItem) const
{
    if (hItem) {
        hItem->setExpanded(!hItem->isExpanded());
    }
}

void TreeView::setItemImage(QTreeWidgetItem* hTreeItem, int iImage, int iSelectedImage)
{
    Q_UNUSED(iSelectedImage);
    if (hTreeItem && iImage >= 0 && iImage < _imageList.size()) {
        hTreeItem->setIcon(0, QIcon(_imageList[iImage]));
    }
}

void TreeView::beginDrag()
{
    _isItemDragged = true;
    _draggedItem = currentItem();
}

void TreeView::dragItem(const QPoint& pos)
{
    Q_UNUSED(pos);
    // Drag is handled by Qt's built-in drag-drop mechanism
}

bool TreeView::dropItem()
{
    if (!_draggedItem || !_dropTarget) return false;
    
    if (!canBeDropped(_draggedItem, _dropTarget)) return false;
    
    // Move the item
    QTreeWidgetItem* parent = _dropTarget->parent();
    int row = parent ? parent->indexOfChild(_dropTarget) : indexOfTopLevelItem(_dropTarget);
    
    QTreeWidgetItem* item = _draggedItem;
    QTreeWidgetItem* oldParent = item->parent();
    if (oldParent) {
        oldParent->removeChild(item);
    } else {
        takeTopLevelItem(indexOfTopLevelItem(item));
    }
    
    if (parent) {
        parent->insertChild(row, item);
    } else {
        insertTopLevelItem(row, item);
    }
    
    _isItemDragged = false;
    _draggedItem = nullptr;
    _dropTarget = nullptr;
    
    return true;
}

void TreeView::setDropTarget(QTreeWidgetItem* target)
{
    _dropTarget = target;
}

void TreeView::addCanNotDropInList(int val2set)
{
    if (!_canNotDropInList.contains(val2set)) {
        _canNotDropInList.append(val2set);
    }
}

void TreeView::addCanNotDragOutList(int val2set)
{
    if (!_canNotDragOutList.contains(val2set)) {
        _canNotDragOutList.append(val2set);
    }
}

bool TreeView::moveDown(QTreeWidgetItem* itemToMove)
{
    if (!itemToMove) return false;
    
    QTreeWidgetItem* parent = itemToMove->parent();
    int row = parent ? parent->indexOfChild(itemToMove) : indexOfTopLevelItem(itemToMove);
    int maxRow = parent ? parent->childCount() - 1 : topLevelItemCount() - 1;
    
    if (row < maxRow) {
        QTreeWidgetItem* below = parent ? parent->child(row + 1) : topLevelItem(row + 1);
        if (below) {
            parent ? parent->removeChild(itemToMove) : takeTopLevelItem(row);
            parent ? parent->insertChild(row + 1, itemToMove) : insertTopLevelItem(row + 1, itemToMove);
            return true;
        }
    }
    return false;
}

bool TreeView::moveUp(QTreeWidgetItem* itemToMove)
{
    if (!itemToMove) return false;
    
    QTreeWidgetItem* parent = itemToMove->parent();
    int row = parent ? parent->indexOfChild(itemToMove) : indexOfTopLevelItem(itemToMove);
    
    if (row > 0) {
        QTreeWidgetItem* above = parent ? parent->child(row - 1) : topLevelItem(row - 1);
        if (above) {
            parent ? parent->removeChild(itemToMove) : takeTopLevelItem(row);
            parent ? parent->insertChild(row - 1, itemToMove) : insertTopLevelItem(row - 1, itemToMove);
            return true;
        }
    }
    return false;
}

void TreeView::setImageList(const QVector<int>& imageIds, int imgSize)
{
    Q_UNUSED(imgSize);
    _imageList.clear();
    
    // Would load icons from resources
    for (int id : imageIds) {
        Q_UNUSED(id);
        _imageList.append(QPixmap());
    }
}

void TreeView::cleanSubEntries(QTreeWidgetItem* hTreeItem)
{
    if (!hTreeItem) return;
    
    for (int i = hTreeItem->childCount() - 1; i >= 0; --i) {
        QTreeWidgetItem* child = hTreeItem->child(i);
        cleanSubEntries(child);
        delete child;
    }
}

bool TreeView::canBeDropped(QTreeWidgetItem* draggedItem, QTreeWidgetItem* targetItem)
{
    if (!draggedItem || !targetItem) return false;
    if (draggedItem == targetItem) return false;
    
    // Check if dragged item is an ancestor of target
    if (isParent(targetItem, draggedItem)) return false;
    
    return true;
}

bool TreeView::isParent(QTreeWidgetItem* targetItem, QTreeWidgetItem* draggedItem)
{
    if (!targetItem || !draggedItem) return false;
    
    QTreeWidgetItem* parent = targetItem->parent();
    while (parent) {
        if (parent == draggedItem) return true;
        parent = parent->parent();
    }
    return false;
}

void TreeView::dragEnterEvent(QDragEnterEvent* event)
{
    QTreeWidget::dragEnterEvent(event);
}

void TreeView::dragMoveEvent(QDragMoveEvent* event)
{
    QTreeWidget::dragMoveEvent(event);
}

void TreeView::dropEvent(QDropEvent* event)
{
    QTreeWidget::dropEvent(event);
}

void TreeView::mousePressEvent(QMouseEvent* event)
{
    QTreeWidget::mousePressEvent(event);
}