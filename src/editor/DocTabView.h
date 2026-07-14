// DocTabView.h - Advanced document tab widget with drag-reorder and split
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QTabWidget>
#include <QTabBar>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMap>
#include <QColor>
#include <QTimer>

class ScintillaEditor;

// ============================================================================
// TabData — per-tab auxiliary state
// ============================================================================
struct TabData {
    bool pinned = false;
    QColor color;           // Tab color indicator (empty = default)
    QString bufferId;       // Buffer identifier for buffer→tab mapping
    bool modified = false;   // Unsaved changes indicator
};

// ============================================================================
// DocTabView — Advanced tab management for document views
// ============================================================================
class DocTabView : public QTabWidget {
    Q_OBJECT

public:
    explicit DocTabView(QWidget* parent = nullptr);
    ~DocTabView() override;

    // ---- Tab management ----

    /// Add an editor tab with title; returns tab index
    int addTab(ScintillaEditor* editor, const QString& title);

    /// Convenience: add tab with icon
    int addTab(ScintillaEditor* editor, const QIcon& icon, const QString& title);

    /// Insert a tab at a specific position
    int insertTab(int index, ScintillaEditor* editor, const QString& title);
    int insertTab(int index, ScintillaEditor* editor, const QIcon& icon,
                  const QString& title);

    /// Close tab at index
    void closeTab(int index);

    /// Move tab from one position to another (for drag-reorder)
    void moveTab(int fromIndex, int toIndex);

    /// Close all tabs
    void closeAllTabs();

    /// Close all tabs except the given index
    void closeOtherTabs(int keepIndex);

    /// Close all tabs to the left of the given index
    void closeTabsToLeft(int index);

    /// Close all tabs to the right of the given index
    void closeTabsToRight(int index);

    /// Get editor at tab index
    ScintillaEditor* editorAt(int index) const;

    /// Reload tab title (e.g., after file rename)
    void updateTabTitle(int index, const QString& title);

    /// Set tab icon (e.g., for language icons)
    void setTabIcon(int index, const QIcon& icon);

    /// Tab bar access
    QTabBar* tabBar() const { return QTabWidget::tabBar(); }

    // ---- Pinned tabs ----

    /// Pin or unpin a tab (pinned tabs cannot be closed via middle-click)
    void setTabPinned(int index, bool pinned);
    bool isTabPinned(int index) const;

    // ---- Tab color indicators ----

    /// Set the color indicator for a tab (0 = no color)
    void setTabColor(int index, const QColor& color);
    QColor tabColor(int index) const;

    /// Clear the color indicator for a tab
    void clearTabColor(int index) { setTabColor(index, QColor()); }

    // ---- "New tab" button ----

    /// Show/hide the "+" new tab button
    void setNewTabButtonVisible(bool visible);
    bool isNewTabButtonVisible() const { return _newTabBtn != nullptr; }

    /// Set the tab that shows an unsaved/modiifed dot indicator
    void setTabModified(int index, bool modified);
    bool isTabModified(int index) const;

    // ---- Buffer-to-tab synchronization ----

    /// Register a buffer ID for a tab (for buffer→tab mapping)
    void setTabBufferId(int index, const QString& bufferId);
    QString tabBufferId(int index) const;

    /// Find the tab index for a given buffer ID
    int findTabByBufferId(const QString& bufferId) const;

    // ---- Tab state persistence ----

    /// Get the order of tabs as a list of buffer IDs (for session saving)
    QStringList tabOrder() const;

    /// Restore tab order from a list of buffer IDs (for session loading)
    void restoreTabOrder(const QStringList& bufferIds);

    // ---- Context menu actions ----

    /// Get the tab context menu
    QMenu* contextMenu() const { return _contextMenu; }

    /// Add a custom action to the context menu
    void addContextMenuAction(QAction* action);
    void addContextMenuSeparator();

signals:
    void tabClosed(int index);
    void tabMoved(int from, int to);
    void currentChanged(int index);
    void allTabsClosed();
    void newTabRequested();
    void tabPinnedChanged(int index, bool pinned);
    void tabColorChanged(int index, const QColor& color);

protected:
    // Handle tab removal
    void tabRemoved(int index) override;

    // Handle tab insertion
    void tabInserted(int index) override;

    // Mouse events for middle-click close and drag-reorder
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
    void onContextMenu(const QPoint& pos);
    void onCloseTriggered();
    void onCloseOthersTriggered();
    void onCloseAllTriggered();
    void onCloseToLeftTriggered();
    void onCloseToRightTriggered();
    void onMoveLeftTriggered();
    void onMoveRightTriggered();
    void onPinTabTriggered();
    void onNewTabTriggered();
    void onTabBarClicked(int index);
    void onTabMovedWhileDragging(int from, int to);

private:
    void setupContextMenu();
    void setupNewTabButton();
    int tabIndexAt(const QPoint& pos) const;
    TabData& tabData(int index);
    const TabData& tabData(int index) const;
    void updateTabPinnedAppearance(int index);

    // Drop index during drag
    int _dropIndex = -1;
    int _dragSourceIndex = -1;

    QMenu* _contextMenu = nullptr;
    QAction* _closeAct = nullptr;
    QAction* _closeOthersAct = nullptr;
    _closeAllAct = nullptr;
    QAction* _closeToLeftAct = nullptr;
    QAction* _closeToRightAct = nullptr;
    QAction* _moveLeftAct = nullptr;
    QAction* _moveRightAct = nullptr;
    QAction* _pinAct = nullptr;
    QAction* _newTabAct = nullptr;

    QWidget* _newTabBtn = nullptr;    // The "+" button
    QTimer* _tabReorderTimer = nullptr;

    QMap<int, TabData> _tabData;     // Per-tab auxiliary state
};
