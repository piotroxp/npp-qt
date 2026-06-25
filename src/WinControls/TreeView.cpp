// Semantic Lift: Win32 TreeView → Qt6 QTreeWidget
// Original: PowerEditor/src/WinControls/TreeView/*.{h,cpp}
// Target: npp-qt/src/WinControls/TreeView.cpp

#include "TreeView.h"
#include <QHeaderView>
#include <QDrag>
#include <QMimeData>
#include <QDataStream>
#include <QByteArray>

TreeView::TreeView(QWidget* parent)
    : QTreeWidget(parent)
{
    // Mirror Win32 tree view styles:
    // WS_HSCROLL | WS_TABSTOP | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS | TVS_INFOTIP
    setHeaderHidden(true);
    setAlternatingRowColors(false);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    // Enable internal drag-drop
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    // Default connection: emit renamed signal
    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        if (item) item->setFlags(item->flags() | Qt::ItemIsEditable);
    });

    connect(this, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int) {
        // Mirror TVN_ENDLABELEDIT
        if (item) {
            QString newText = item->text(0);
            emit itemRenamed(item, newText);
        }
    });
}

void TreeView::init(QWidget* parent)
{
    Q_UNUSED(parent);
    // In Qt, the parent is set in constructor; this is called for Win32 compatibility
}

QTreeWidgetItem* TreeView::addItem(const QString& itemName, QTreeWidgetItem* parentItem,
                                    int imageIndex, QVariant lParam)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, itemName);
    item->setData(0, Qt::UserRole, lParam);

    if (imageIndex >= 0 && imageIndex < _icons.size()) {
        item->setIcon(0, _icons[imageIndex]);
    }

    if (parentItem) {
        parentItem->addChild(item);
    } else {
        addTopLevelItem(item);
    }

    return item;
}

bool TreeView::setItemData(QTreeWidgetItem* item, const QVariant& data)
{
    if (!item) return false;
    item->setData(0, Qt::UserRole, data);
    return true;
}

QVariant TreeView::getItemData(QTreeWidgetItem* item) const
{
    if (!item) return QVariant();
    return item->data(0, Qt::UserRole);
}

QString TreeView::getItemText(QTreeWidgetItem* item) const
{
    if (!item) return QString();
    return item->text(0);
}

bool TreeView::renameItem(QTreeWidgetItem* item, const QString& newName)
{
    if (!item) return false;
    item->setText(0, newName);
    return true;
}

QTreeWidgetItem* TreeView::searchSubItemByName(const QString& itemName, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* root = parentItem ? parentItem : invisibleRootItem();
    if (!root) return nullptr;

    for (int i = 0; i < root->childCount(); ++i) {
        QTreeWidgetItem* child = root->child(i);
        if (child->text(0) == itemName)
            return child;
    }
    return nullptr;
}

void TreeView::removeItem(QTreeWidgetItem* item)
{
    if (!item) return;
    delete item; // QTreeWidget takes ownership
}

void TreeView::removeAllItems()
{
    // Remove all top-level items
    while (topLevelItemCount() > 0) {
        delete takeTopLevelItem(0);
    }
}

void TreeView::makeLabelEditable(bool toBeEnabled)
{
    if (toBeEnabled) {
        setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    } else {
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

bool TreeView::setImageList(const QVector<QIcon>& icons)
{
    _icons = icons;
    return true;
}

void TreeView::expandItem(QTreeWidgetItem* item) const
{
    if (item) expandItem(item);
}

void TreeView::collapseItem(QTreeWidgetItem* item) const
{
    if (item) collapseItem(item);
}

void TreeView::expandAllItems(QTreeWidgetItem* item)
{
    if (!item) item = invisibleRootItem();
    if (!item) return;
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        child->setExpanded(true);
        expandAllItems(child);
    }
}

void TreeView::collapseAllItems(QTreeWidgetItem* item)
{
    if (!item) item = invisibleRootItem();
    if (!item) return;
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        child->setExpanded(false);
        collapseAllItems(child);
    }
}

void TreeView::toggleExpand(QTreeWidgetItem* item) const
{
    if (!item) return;
    item->setExpanded(!item->isExpanded());
}

void TreeView::setItemIcon(QTreeWidgetItem* item, int imageIndex, int selectedImageIndex)
{
    Q_UNUSED(selectedImageIndex);
    if (!item) return;
    if (imageIndex >= 0 && imageIndex < _icons.size()) {
        item->setIcon(0, _icons[imageIndex]);
    }
}

bool TreeView::saveFoldingState(QTreeWidgetItem* item, TreeStateNode& state) const
{
    if (!item) return false;
    state._label = item->text(0);
    state._isExpanded = item->isExpanded();
    state._isSelected = item->isSelected();
    state._extraData = item->data(0, Qt::UserRole).toString();
    state._children.clear();
    for (int i = 0; i < item->childCount(); ++i) {
        state._children.append(TreeStateNode());
        saveFoldingState(item->child(i), state._children.last());
    }
    return true;
}

bool TreeView::restoreFoldingState(const TreeStateNode& state, QTreeWidgetItem* item)
{
    if (!item) return false;
    if (state._isExpanded)
        item->setExpanded(true);
    if (state._isSelected)
        setCurrentItem(item);

    int childCount = qMin(item->childCount(), state._children.size());
    for (int i = 0; i < childCount; ++i) {
        restoreFoldingState(state._children[i], item->child(i));
    }
    return true;
}

bool TreeView::searchLeafAndBuildTree(QTreeWidget* sourceTree, const QString& text2Search, int imageIndex)
{
    Q_UNUSED(sourceTree);
    Q_UNUSED(text2Search);
    Q_UNUSED(imageIndex);
    // Simplified implementation
    return false;
}

void TreeView::sort(QTreeWidgetItem* item, bool isRecursive)
{
    if (!item) return;
    item->sortChildren(0, Qt::AscendingOrder);
    if (isRecursive) {
        for (int i = 0; i < item->childCount(); ++i) {
            sort(item->child(i), true);
        }
    }
}

void TreeView::sortAll(QTreeWidgetItem* item)
{
    sort(item ? item : invisibleRootItem(), true);
}

void TreeView::beginDrag(QTreeWidgetItem* item)
{
    if (!item) return;
    if (!canDragOut(item)) return;

    _draggedItem = item;
    _isDragging = true;
    emit itemDragged(item);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData(QList<QTreeWidgetItem*>() << item));
    drag->exec(Qt::MoveAction);
    _isDragging = false;
}

bool TreeView::dropItem(QTreeWidgetItem* targetItem)
{
    if (!targetItem || !_draggedItem) return false;
    if (!canBeDropped(_draggedItem, targetItem)) return false;

    // Move item to new parent
    QTreeWidgetItem* parent = targetItem;
    QTreeWidgetItem* dragged = _draggedItem;

    // Clone tree structure
    duplicateSubTree(dragged, parent);

    // Remove original
    delete dragged;

    emit itemDropped(dragged, targetItem);
    return true;
}

void TreeView::duplicateSubTree(QTreeWidgetItem* source, QTreeWidgetItem* parentDest)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(0, source->text(0));
    newItem->setData(0, Qt::UserRole, source->data(0, Qt::UserRole));
    newItem->setIcon(0, source->icon(0));
    parentDest->addChild(newItem);

    for (int i = 0; i < source->childCount(); ++i) {
        duplicateSubTree(source->child(i), newItem);
    }
}

QTreeWidgetItem* TreeView::getChildFrom(QTreeWidgetItem* parent) const
{
    if (!parent) return nullptr;
    if (parent->childCount() == 0) return nullptr;
    return parent->child(0);
}

QTreeWidgetItem* TreeView::getSelection() const
{
    return currentItem();
}

QTreeWidgetItem* TreeView::getRoot() const
{
    if (topLevelItemCount() == 0) return nullptr;
    return topLevelItem(0);
}

QTreeWidgetItem* TreeView::getParent(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    return item->parent();
}

QTreeWidgetItem* TreeView::getNextSibling(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    QTreeWidgetItem* parent = item->parent();
    if (!parent) {
        int idx = indexOfTopLevelItem(item);
        if (idx >= 0 && idx < topLevelItemCount() - 1)
            return topLevelItem(idx + 1);
        return nullptr;
    }
    int idx = parent->indexOfChild(item);
    if (idx >= 0 && idx < parent->childCount() - 1)
        return parent->child(idx + 1);
    return nullptr;
}

QTreeWidgetItem* TreeView::getPrevSibling(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    QTreeWidgetItem* parent = item->parent();
    if (!parent) {
        int idx = indexOfTopLevelItem(item);
        if (idx > 0)
            return topLevelItem(idx - 1);
        return nullptr;
    }
    int idx = parent->indexOfChild(item);
    if (idx > 0)
        return parent->child(idx - 1);
    return nullptr;
}

bool TreeView::moveDown(QTreeWidgetItem* item)
{
    QTreeWidgetItem* sibling = getNextSibling(item);
    if (!sibling) return false;
    return swapItems(item, sibling);
}

bool TreeView::moveUp(QTreeWidgetItem* item)
{
    QTreeWidgetItem* sibling = getPrevSibling(item);
    if (!sibling) return false;
    return swapItems(sibling, item);
}

bool TreeView::swapItems(QTreeWidgetItem* itemA, QTreeWidgetItem* itemB)
{
    // Simple swap: exchange their order within parent
    QTreeWidgetItem* parentA = itemA->parent();
    QTreeWidgetItem* parentB = itemB->parent();
    if (parentA != parentB) return false;

    QTreeWidgetItem* parent = parentA;
    int idxA, idxB;
    if (parent) {
        idxA = parent->indexOfChild(itemA);
        idxB = parent->indexOfChild(itemB);
    } else {
        idxA = indexOfTopLevelItem(itemA);
        idxB = indexOfTopLevelItem(itemB);
    }
    if (idxA < 0 || idxB < 0) return false;

    // Detach both
    if (parent) {
        parent->removeChild(itemA);
        parent->removeChild(itemB);
        // Re-insert in swapped order
        parent->insertChild(qMin(idxA, idxB), itemB);
        parent->insertChild(qMax(idxA, idxB), itemA);
    } else {
        takeTopLevelItem(idxA);
        takeTopLevelItem(idxB > idxA ? idxB - 1 : idxB);
        insertTopLevelItem(qMin(idxA, idxB), itemB);
        insertTopLevelItem(qMax(idxA, idxB), itemA);
    }
    return true;
}

void TreeView::addCannotDropInList(int imageVal)
{
    if (!_cannotDropInList.contains(imageVal))
        _cannotDropInList.append(imageVal);
}

void TreeView::addCannotDragOutList(int imageVal)
{
    if (!_cannotDragOutList.contains(imageVal))
        _cannotDragOutList.append(imageVal);
}

bool TreeView::canBeDropped(QTreeWidgetItem* draggedItem, QTreeWidgetItem* targetItem) const
{
    if (!targetItem || !draggedItem) return false;
    if (!canDropIn(targetItem)) return false;
    if (isAncestor(targetItem, draggedItem)) return false;
    if (targetItem == draggedItem) return false;
    return true;
}

bool TreeView::canDropIn(QTreeWidgetItem* item) const
{
    if (!item) return false;
    int img = item->data(0, Qt::UserRole + 1).toInt(); // Simplified: check user data
    Q_UNUSED(img);
    for (int v : _cannotDropInList) {
        Q_UNUSED(v);
        // In full implementation, compare with item image index
    }
    return true;
}

bool TreeView::canDragOut(QTreeWidgetItem* item) const
{
    if (!item) return false;
    for (int v : _cannotDragOutList) {
        Q_UNUSED(v);
        // Compare with item image index
    }
    return true;
}

bool TreeView::isAncestor(QTreeWidgetItem* possibleAncestor, QTreeWidgetItem* item) const
{
    if (!item) return false;
    QTreeWidgetItem* parent = item->parent();
    while (parent) {
        if (parent == possibleAncestor) return true;
        parent = parent->parent();
    }
    return false;
}

void TreeView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->source() == this) {
        event->acceptProposedAction();
    }
}

void TreeView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->source() == this) {
        event->acceptProposedAction();
    }
}

void TreeView::dropEvent(QDropEvent* event)
{
    if (event->source() != this) return;

    QTreeWidgetItem* target = itemAt(event->pos());
    if (!target) return;

    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* dragged = items.first();
    if (canBeDropped(dragged, target)) {
        // Clone and delete original
        duplicateSubTree(dragged, target);
        delete dragged;
        emit itemDropped(dragged, target);
    }

    event->acceptProposedAction();
}

QMimeData* TreeView::mimeData(const QList<QTreeWidgetItem*> items) const
{
    if (items.isEmpty()) return nullptr;
    QMimeData* mime = new QMimeData();
    mime->setText(items.first()->text(0));
    return mime;
}

QStringList TreeView::mimeTypes() const
{
    return QStringList() << QStringLiteral("text/plain");
}
