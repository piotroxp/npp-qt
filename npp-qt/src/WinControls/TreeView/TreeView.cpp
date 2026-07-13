// Semantic Lift: TreeView — Win32 TreeView_* messages → Qt6 QTreeWidget
// Original: PowerEditor/src/WinControls/TreeView/TreeView.cpp (823 lines)
// Win32 → Qt6: TVM_INSERTITEM + TVM_DELETEITEM + TVM_GETITEM → QTreeWidget + QTreeWidgetItem

#include "TreeView.h"
#include "NppDarkMode.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>

TreeView::TreeView(QWidget* parent)
    : QWidget(parent)
    , _treeView(new QTreeWidget(this))
    , _wndHandle(nullptr)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(_treeView);

    _treeView->setHeaderHidden(true);
    _treeView->setAlternatingRowColors(false);
    _treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    _treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(_treeView, &QTreeWidget::itemClicked,
            this, &TreeView::onItemClicked);
    connect(_treeView, &QTreeWidget::itemDoubleClicked,
            this, &TreeView::onItemDoubleClicked);
    connect(_treeView, &QTreeWidget::itemExpanded,
            this, &TreeView::onItemExpanded);
    connect(_treeView, &QTreeWidget::itemCollapsed,
            this, &TreeView::onItemCollapsed);
    connect(_treeView, &QTreeWidget::itemChanged,
            this, &TreeView::onItemChanged);
    connect(_treeView, &QTreeWidget::itemSelectionChanged,
            this, &TreeView::onItemSelectionChanged);
}

TreeView::~TreeView()
{
}

void TreeView::init(HWNDWrapper hwnd)
{
    _wndHandle = hwnd;
}

void TreeView::destroy()
{
    _itemMap.clear();
    _itemIdMap.clear();
    _itemLParamMap.clear();
    _treeView->clear();
}

void* TreeView::insertItem(void* hParent, int id, const QString& text, int iconIndex, void* hInsertAfter)
{
    QTreeWidgetItem* parent = voidToItem(hParent);
    QTreeWidgetItem* afterItem = voidToItem(hInsertAfter);

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setData(0, Qt::UserRole, id);

    if (parent) {
        if (afterItem) {
            parent->insertChild(parent->indexOfChild(afterItem), item);
        } else {
            parent->addChild(item);
        }
    } else {
        if (afterItem) {
            int idx = _treeView->indexOfTopLevelItem(afterItem);
            _treeView->insertTopLevelItem(idx + 1, item);
        } else {
            _treeView->addTopLevelItem(item);
        }
    }

    _itemIdMap[item] = id;
    return itemToVoid(item);
}

void* TreeView::insertItemSorted(void* hParent, int id, const QString& text, int iconIndex)
{
    QTreeWidgetItem* parent = voidToItem(hParent);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setData(0, Qt::UserRole, id);
    _itemIdMap[item] = id;

    if (parent) {
        // Insert in sorted order
        for (int i = 0; i < parent->childCount(); ++i) {
            QTreeWidgetItem* child = parent->child(i);
            if (child->text(0) > text) {
                parent->insertChild(i, item);
                return itemToVoid(item);
            }
        }
        parent->addChild(item);
    } else {
        for (int i = 0; i < _treeView->topLevelItemCount(); ++i) {
            QTreeWidgetItem* child = _treeView->topLevelItem(i);
            if (child->text(0) > text) {
                _treeView->insertTopLevelItem(i, item);
                return itemToVoid(item);
            }
        }
        _treeView->addTopLevelItem(item);
    }

    return itemToVoid(item);
}

void TreeView::deleteItem(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item) return;

    _itemIdMap.remove(item);
    _itemLParamMap.remove(item);
    _itemMap.remove(hItem);

    delete item;
}

void* TreeView::getChild(void* hItem, int index)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item || index < 0 || index >= item->childCount())
        return nullptr;
    return itemToVoid(item->child(index));
}

void* TreeView::getChildWithName(void* hParent, const QString& name)
{
    QTreeWidgetItem* parent = voidToItem(hParent);
    if (!parent) {
        // Search top-level items
        for (int i = 0; i < _treeView->topLevelItemCount(); ++i) {
            QTreeWidgetItem* child = _treeView->topLevelItem(i);
            if (child->text(0) == name)
                return itemToVoid(child);
        }
        return nullptr;
    }

    for (int i = 0; i < parent->childCount(); ++i) {
        QTreeWidgetItem* child = parent->child(i);
        if (child->text(0) == name)
            return itemToVoid(child);
    }
    return nullptr;
}

int TreeView::getChildCount(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    return item ? item->childCount() : 0;
}

void TreeView::deleteAllChildren(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item) return;

    while (item->childCount() > 0) {
        QTreeWidgetItem* child = item->child(0);
        _itemIdMap.remove(child);
        _itemLParamMap.remove(child);
        item->removeChild(child);
        delete child;
    }
}

void TreeView::setItemText(void* hItem, const QString& text)
{
    if (auto* item = voidToItem(hItem))
        item->setText(0, text);
}

void TreeView::setItemImage(void* hItem, int iconIndex)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item) return;
    // Icon would come from an image list — use placeholder
    Q_UNUSED(iconIndex);
}

void TreeView::setItemParam(void* hItem, int param)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        item->setData(0, Qt::UserRole, param);
}

void TreeView::setItemLParam(void* hItem, long lParam)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        _itemLParamMap[item] = lParam;
}

void TreeView::setItemExpanded(void* hItem, bool expand)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        item->setExpanded(expand);
}

void TreeView::setItemSelected(void* hItem, bool select)
{
    if (QTreeWidgetItem* item = voidToItem(hItem)) {
        if (select)
            _treeView->setCurrentItem(item);
        else if (_treeView->currentItem() == item)
            _treeView->setCurrentItem(nullptr);
    }
}

void TreeView::setItemChecked(void* hItem, bool check)
{
    if (QTreeWidgetItem* item = voidToItem(hItem)) {
        item->setCheckState(0, check ? Qt::Checked : Qt::Unchecked);
    }
}

QString TreeView::getItemText(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return item->text(0);
    return QString();
}

int TreeView::getItemParam(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return item->data(0, Qt::UserRole).toInt();
    return 0;
}

long TreeView::getItemLParam(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return _itemLParamMap.value(item, 0);
    return 0;
}

bool TreeView::getItemExpanded(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return item->isExpanded();
    return false;
}

bool TreeView::getItemSelected(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return _treeView->currentItem() == item;
    return false;
}

bool TreeView::getItemChecked(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        return item->checkState(0) == Qt::Checked;
    return false;
}

void* TreeView::getParent(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item || !item->parent())
        return nullptr;
    return itemToVoid(item->parent());
}

void* TreeView::getRootItem()
{
    return nullptr; // No single root
}

void* TreeView::getSelection()
{
    return itemToVoid(_treeView->currentItem());
}

void* TreeView::getFirstVisibleItem()
{
    return itemToVoid(_treeView->topLevelItem(0));
}

void* TreeView::getNextVisibleItem(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item) return nullptr;
    return itemToVoid(item->parent()
        ? item->parent()->child(item->parent()->indexOfChild(item) + 1)
        : _treeView->topLevelItem(_treeView->indexOfTopLevelItem(item) + 1));
}

void* TreeView::getNextItem(void* hItem)
{
    return getNextVisibleItem(hItem);
}

void TreeView::ensureItemVisible(void* hItem)
{
    if (QTreeWidgetItem* item = voidToItem(hItem))
        _treeView->scrollToItem(item);
}

void TreeView::selectItem(void* hItem)
{
    setItemSelected(hItem, true);
}

void TreeView::sortChildren(void* hItem, bool recurse)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (!item) return;
    item->sortChildren(0, Qt::AscendingOrder);
    if (recurse) {
        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* child = item->child(i);
            child->sortChildren(0, Qt::AscendingOrder);
        }
    }
}

void TreeView::startLabelEdit(void* hItem)
{
    QTreeWidgetItem* item = voidToItem(hItem);
    if (item)
        _treeView->editItem(item, 0);
}

void TreeView::doColorNotify(int color)
{
    _currentColor = color;
}

void TreeView::changeTGg所使用的()
{
    // Placeholder for Win32-specific notification
}

void TreeView::setBgColor(int r, int g, int b)
{
    QPalette pal = _treeView->palette();
    pal.setColor(QPalette::Base, QColor(r, g, b));
    _treeView->setPalette(pal);
}

void TreeView::setFgColor(int r, int g, int b)
{
    QPalette pal = _treeView->palette();
    pal.setColor(QPalette::Text, QColor(r, g, b));
    _treeView->setPalette(pal);
}

// --- Private helpers ---

QTreeWidgetItem* TreeView::voidToItem(void* h) const
{
    return static_cast<QTreeWidgetItem*>(h);
}

void* TreeView::itemToVoid(QTreeWidgetItem* item) const
{
    return static_cast<void*>(item);
}

// --- Slots ---

void TreeView::onItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    emit treeItemSelected(id, itemToVoid(item));
}

void TreeView::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    emit treeItemDoubleClicked(id, itemToVoid(item));
}

void TreeView::onItemExpanded(QTreeWidgetItem* item)
{
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    emit treeItemExpanded(id, itemToVoid(item));
}

void TreeView::onItemCollapsed(QTreeWidgetItem* item)
{
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    emit treeItemCollapsed(id, itemToVoid(item));
}

void TreeView::onItemChanged(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    bool checked = item->checkState(0) == Qt::Checked;
    emit treeItemChecked(id, checked, itemToVoid(item));
}

void TreeView::onItemSelectionChanged()
{
    QTreeWidgetItem* item = _treeView->currentItem();
    if (!item) return;
    int id = _itemIdMap.value(item, 0);
    emit treeItemSelected(id, itemToVoid(item));
}
