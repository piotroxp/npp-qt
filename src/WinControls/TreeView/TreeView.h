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

#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QVector>
#include <QPoint>
#include <QPixmap>

#include "../Window/Window.h"

// TreeView - Tree view control
// Replaces MFC CTreeCtrl with Qt QTreeWidget
class TreeView : public QTreeWidget
{
    Q_OBJECT

public:
    TreeView(QWidget* parent = nullptr);
    ~TreeView() override = default;

    void init(QWidget* parent, int treeViewID);
    void destroy() override;

    // Item operations
    QTreeWidgetItem* addItem(const QString& itemName, QTreeWidgetItem* hParentItem, int iImage, intptr_t lParam = 0);
    bool setItemParam(QTreeWidgetItem* item, intptr_t param);
    intptr_t getItemParam(QTreeWidgetItem* item) const;
    QString getItemDisplayName(QTreeWidgetItem* item) const;
    QTreeWidgetItem* searchSubItemByName(const QString& itemName, QTreeWidgetItem* hParentItem);
    void removeItem(QTreeWidgetItem* hTreeItem);
    void removeAllItems();
    bool renameItem(QTreeWidgetItem* item, const QString& newName);
    void makeLabelEditable(bool toBeEnabled);

    // Navigation
    QTreeWidgetItem* getChildFrom(QTreeWidgetItem* hTreeItem) const;
    QTreeWidgetItem* getSelection() const;
    bool selectItem(QTreeWidgetItem* hTreeItem2Select) const;
    QTreeWidgetItem* getRoot() const;
    QTreeWidgetItem* getParent(QTreeWidgetItem* hItem) const;
    QTreeWidgetItem* getNextSibling(QTreeWidgetItem* hItem) const;
    QTreeWidgetItem* getPrevSibling(QTreeWidgetItem* hItem) const;

    // Expansion
    void expand(QTreeWidgetItem* hItem) const;
    void fold(QTreeWidgetItem* hItem) const;
    void foldExpandRecursively(QTreeWidgetItem* hItem, bool isFold) const;
    void foldExpandAll(bool isFold) const;
    void foldAll() const;
    void expandAll() const;
    void toggleExpandCollapse(QTreeWidgetItem* hItem) const;

    // Images
    void setItemImage(QTreeWidgetItem* hTreeItem, int iImage, int iSelectedImage);

    // Drag and drop
    void beginDrag();
    void dragItem(const QPoint& pos);
    bool isDragging() const { return _isItemDragged; }
    bool dropItem();
    void setDropTarget(QTreeWidgetItem* target);

    void addCanNotDropInList(int val2set);
    void addCanNotDragOutList(int val2set);

    // Moving items
    bool moveDown(QTreeWidgetItem* itemToMove);
    bool moveUp(QTreeWidgetItem* itemToMove);

    // Image list
    void setImageList(const QVector<int>& imageIds, int imgSize = 0);

signals:
    void itemExpanded(QTreeWidgetItem* item);
    void itemCollapsed(QTreeWidgetItem* item);
    void itemRightClicked(QTreeWidgetItem* item, const QPoint& pos);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void cleanSubEntries(QTreeWidgetItem* hTreeItem);
    bool canBeDropped(QTreeWidgetItem* draggedItem, QTreeWidgetItem* targetItem);
    bool isParent(QTreeWidgetItem* targetItem, QTreeWidgetItem* draggedItem);

    QIcon _iconList;
    QVector<QPixmap> _imageList;
    QVector<int> _canNotDropInList;
    QVector<int> _canNotDragOutList;
    
    bool _isItemDragged = false;
    QTreeWidgetItem* _draggedItem = nullptr;
    QTreeWidgetItem* _dropTarget = nullptr;
};