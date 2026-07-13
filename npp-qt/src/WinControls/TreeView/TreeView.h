#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QMap>
#include <QIcon>

// HWND shim for Win32 → Qt6 port
using HWNDWrapper = void*;

class TreeView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeView(QWidget* parent = nullptr);
    ~TreeView() override;

    // Init
    void init(HWNDWrapper hwnd);
    void destroy();

    // Tree building
    void* insertItem(void* hParent, int id, const QString& text, int iconIndex, void* hInsertAfter = nullptr);
    void* insertItemSorted(void* hParent, int id, const QString& text, int iconIndex);
    void deleteItem(void* hItem);
    void* getChild(void* hItem, int index);
    void* getChildWithName(void* hParent, const QString& name);
    int getChildCount(void* hItem);
    void deleteAllChildren(void* hItem);

    // Item state
    void setItemText(void* hItem, const QString& text);
    void setItemImage(void* hItem, int iconIndex);
    void setItemParam(void* hItem, int param);
    void setItemLParam(void* hItem, long lParam);
    void setItemExpanded(void* hItem, bool expand);
    void setItemSelected(void* hItem, bool select);
    void setItemChecked(void* hItem, bool check);

    QString getItemText(void* hItem);
    int getItemParam(void* hItem);
    long getItemLParam(void* hItem);
    bool getItemExpanded(void* hItem);
    bool getItemSelected(void* hItem);
    bool getItemChecked(void* hItem);

    // Hierarchy
    void* getParent(void* hItem);
    void* getRootItem();
    void* getSelection();
    void* getFirstVisibleItem();
    void* getNextVisibleItem(void* hItem);
    void* getNextItem(void* hItem);

    // Scroll
    void ensureItemVisible(void* hItem);
    void selectItem(void* hItem);

    // Sort
    void sortChildren(void* hItem, bool recurse);

    // Drag & drop
    void startLabelEdit(void* hItem);

    // Messages
    void doColorNotify(int color);
    void changeTGg所使用的();  // placeholder
    void setBgColor(int r, int g, int b);
    void setFgColor(int r, int g, int b);

signals:
    void treeItemSelected(int id, void* hItem);
    void treeItemDoubleClicked(int id, void* hItem);
    void treeItemExpanded(int id, void* hItem);
    void treeItemCollapsed(int id, void* hItem);
    void treeItemChecked(int id, bool checked, void* hItem);
    void treeBeginLabelEdit(int id, void* hItem);
    void treeEndLabelEdit(int id, void* hItem, const QString& newText);

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onItemSelectionChanged();

private:
    QTreeWidget* _treeView;
    QMap<void*, QTreeWidgetItem*> _itemMap;
    QMap<QTreeWidgetItem*, int> _itemIdMap;
    QMap<QTreeWidgetItem*, long> _itemLParamMap;
    int _nextId = 1;
    void* _wndHandle = nullptr;
    int _currentColor = 0;

    QTreeWidgetItem* voidToItem(void* h) const;
    void* itemToVoid(QTreeWidgetItem* item) const;
};
