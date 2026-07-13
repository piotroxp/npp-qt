// Semantic Lift: Win32 TreeView → Qt6 QTreeWidget
// Original: PowerEditor/src/WinControls/TreeView/*.{h,cpp}
// Target: npp-qt/src/WinControls/TreeView.h

#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVector>
#include <QString>
#include <QIcon>
#include <QPoint>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>

// =============================================================================
// TreeStateNode — serialized tree state for save/restore (lifted from TreeView.h)
// =============================================================================

struct TreeStateNode
{
    QString _label;
    QString _extraData;
    bool _isExpanded = false;
    bool _isSelected = false;
    QVector<TreeStateNode> _children;
};

// =============================================================================
// TreeView — tree view widget with drag-drop support (lifted from TreeView.h)
// Translates TreeView_* macros / TVM_* messages → QTreeWidget signals
// =============================================================================

class TreeView : public QTreeWidget
{
    Q_OBJECT

public:
    explicit TreeView(QWidget* parent = nullptr);
    ~TreeView() override = default;

    // Initialize with a parent dialog and control ID
    void init(QWidget* parent);

    // Add item — mirrors Win32 TV_INSERTSTRUCT + TreeView_InsertItem
    QTreeWidgetItem* addItem(const QString& itemName, QTreeWidgetItem* parentItem = nullptr,
                               int imageIndex = -1, QVariant lParam = QVariant());

    // Set/get item data
    bool setItemData(QTreeWidgetItem* item, const QVariant& data);
    QVariant getItemData(QTreeWidgetItem* item) const;

    // Get item display name
    QString getItemText(QTreeWidgetItem* item) const;

    // Rename item
    bool renameItem(QTreeWidgetItem* item, const QString& newName);

    // Search for item by name
    QTreeWidgetItem* searchSubItemByName(const QString& itemName, QTreeWidgetItem* parentItem = nullptr);

    // Remove items
    void removeItem(QTreeWidgetItem* item);
    void removeAllItems();

    // Make labels editable
    void makeLabelEditable(bool toBeEnabled);

    // Image list — pass vector of icon IDs
    bool setImageList(const QVector<QIcon>& icons);

    // Expand/collapse
    void expandItem(QTreeWidgetItem* item) const;
    void collapseItem(QTreeWidgetItem* item) const;
    void expandAllItems(QTreeWidgetItem* item);
    void collapseAllItems(QTreeWidgetItem* item);
    void toggleExpand(QTreeWidgetItem* item) const;

    // Set item icon
    void setItemIcon(QTreeWidgetItem* item, int imageIndex, int selectedImageIndex);

    // Folding state
    bool saveFoldingState(QTreeWidgetItem* item, TreeStateNode& state) const;
    bool restoreFoldingState(const TreeStateNode& state, QTreeWidgetItem* item);

    // Search and build tree
    bool searchLeafAndBuildTree(QTreeWidget* sourceTree, const QString& text2Search, int imageIndex);

    // Sorting
    void sort(QTreeWidgetItem* item, bool isRecursive);
    void sortAll(QTreeWidgetItem* item);

    // Drag and drop
    void beginDrag(QTreeWidgetItem* item);
    bool dropItem(QTreeWidgetItem* targetItem);
    bool isDragging() const { return _isDragging; }

    // Item navigation
    QTreeWidgetItem* getChildFrom(QTreeWidgetItem* parent) const;
    QTreeWidgetItem* getSelection() const;
    QTreeWidgetItem* getRoot() const;
    QTreeWidgetItem* getParent(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getNextSibling(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getPrevSibling(QTreeWidgetItem* item) const;

    // Movement
    bool moveDown(QTreeWidgetItem* item);
    bool moveUp(QTreeWidgetItem* item);
    bool swapItems(QTreeWidgetItem* itemA, QTreeWidgetItem* itemB);

    // Drop restrictions
    void addCannotDropInList(int imageVal);
    void addCannotDragOutList(int imageVal);
    bool canBeDropped(QTreeWidgetItem* draggedItem, QTreeWidgetItem* targetItem) const;

signals:
    void itemExpanded(QTreeWidgetItem* item);
    void itemCollapsed(QTreeWidgetItem* item);
    void itemRenamed(QTreeWidgetItem* item, const QString& newName);
    void itemDragged(QTreeWidgetItem* item);
    void itemDropped(QTreeWidgetItem* source, QTreeWidgetItem* target);

protected:
    // Override for drag-drop behavior
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const override;
    QStringList mimeTypes() const override;

private:
    bool canDragOut(QTreeWidgetItem* item) const;
    bool canDropIn(QTreeWidgetItem* item) const;
    void duplicateSubTree(QTreeWidgetItem* source, QTreeWidgetItem* parentDest);
    bool isAncestor(QTreeWidgetItem* possibleAncestor, QTreeWidgetItem* item) const;

    QVector<QIcon> _icons;
    QTreeWidgetItem* _draggedItem = nullptr;
    bool _isDragging = false;
    QVector<int> _cannotDropInList;
    QVector<int> _cannotDragOutList;
};
