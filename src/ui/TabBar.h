// TabBar.h - Tab bar for file tabs with full Notepad++ functionality
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QTabBar>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QString>

class QTabWidget;
class QDrag;
class QMimeData;

class TabBar : public QTabBar {
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);
    ~TabBar() override;

    // Basic functionality
    void updateFrom(QTabWidget* tabWidget);
    void setCurrentTab(int index);
    
    // Tab management
    void closeTab(int index);
    void closeOtherTabs(int index);
    void closeAllTabs();
    void pinTab(int index);
    void unpinTab(int index);
    bool isTabPinned(int index) const;
    void lockTab(int index, bool lock);
    bool isTabLocked(int index) const;
    
    // Tab modification
    void setTabModified(int index, bool modified);
    bool isTabModified(int index) const;
    void setTabReadOnly(int index, bool readOnly);
    bool isTabReadOnly(int index) const;
    
    // Drag and drop
    void setExternalDropEnabled(bool enabled);
    bool isExternalDropEnabled() const { return m_externalDropEnabled; }
    
    // Scroll behavior
    void setScrollEnabled(bool enabled);
    bool isScrollEnabled() const { return m_scrollEnabled; }
    void scrollToTab(int index);
    
    // Tab list dropdown
    void showTabListMenu();
    
    // Persistence
    void saveTabOrder(QSettings& settings) const;
    void restoreTabOrder(const QSettings& settings);

Q_SIGNALS:
    void tabMovedSignal(int from, int to);
    void tabClosed(int index);
    void tabPinned(int index);
    void tabUnpinned(int index);
    void tabLocked(int index, bool locked);
    void tabListRequested();
    void newTabRequested();
    void closeTabRequested(int index);
    void closeOtherTabsRequested(int index);
    void closeAllTabsRequested();
    void tabDroppedOutside(int index, const QPoint& pos);
    void tabReordered(int from, int to);

public slots:
    void onTabCloseRequested(int index);
    void onMiddleMouseClicked(int index);
    void setCloseButtonVisible(bool visible);
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void onScrollTimer();
    void onDragScrollTimer();
    void onCloseOtherTabs();
    void onCloseAllTabs();
    void onPinTab();
    void onUnpinTab();
    void onLockTab();
    void onDuplicateTab();
    void onOpenInNewWindow();

private:
    void setupTabs();
    void updateTabAppearance(int index);
    void startDrag(int tabIndex);
    void performDrag();
    int hitTestTab(const QPoint& pos) const;
    QRect tabRectFromIndex(int index) const;
    void scrollTabs(int direction);
    void showTabContextMenu(const QPoint& pos, int tabIndex);
    void animateTabClose(int index);
    void highlightDropTarget(int index);
    void clearDropHighlight();
    
    // Drag state
    int m_dragStartTab = -1;
    QPoint m_dragStartPos;
    bool m_isDragging = false;
    bool m_isDraggingExternal = false;
    QDrag* m_currentDrag = nullptr;
    
    // Scroll state
    bool m_scrollEnabled = true;
    QTimer* m_scrollTimer = nullptr;
    int m_scrollDirection = 0;
    
    // Drag scroll
    QTimer* m_dragScrollTimer = nullptr;
    int m_dropTargetIndex = -1;
    
    // Tab state
    QSet<int> m_pinnedTabs;
    QSet<int> m_lockedTabs;
    QSet<int> m_modifiedTabs;
    QSet<int> m_readOnlyTabs;
    
    // Close button hover
    int m_closeButtonHoverTab = -1;
    
    // External drag/drop
    bool m_externalDropEnabled = true;
    
    // Visual feedback
    bool m_showCloseButtons = true;
    QRect m_dropIndicatorRect;
    
    // Constants
    static constexpr int ScrollAmount = 20;
    static constexpr int ScrollInterval = 50;
    static constexpr int DragScrollThreshold = 30;
};
