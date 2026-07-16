// TabBar.cpp - Tab bar for file tabs with full Notepad++ functionality
// Copyright (C) 2026 Agent Army
// GPL v3

#include "TabBar.h"
#include "DpiManager.h"
#include <QTabWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QStyle>
#include <QPainter>
#include <QToolTip>

TabBar::TabBar(QWidget* parent) : QTabBar(parent) {
    setAcceptDrops(true);
    setMovable(true);
    setTabsClosable(true);
    setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    setDocumentMode(true);
    setElideMode(Qt::ElideMiddle);
    setUsesScrollButtons(true);
    
    // Scroll timer
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setInterval(ScrollInterval);
    connect(m_scrollTimer, &QTimer::timeout, this, &TabBar::onScrollTimer);
    
    // Drag scroll timer
    m_dragScrollTimer = new QTimer(this);
    m_dragScrollTimer->setInterval(ScrollInterval);
    connect(m_dragScrollTimer, &QTimer::timeout, this, &TabBar::onDragScrollTimer);
    
    // Connect close button signal
    connect(this, &QTabBar::tabCloseRequested, this, &TabBar::onTabCloseRequested);
}

TabBar::~TabBar() = default;

void TabBar::setupTabs() {
    // Additional setup if needed
}

void TabBar::updateFrom(QTabWidget* tabWidget) {
    if (!tabWidget) return;

    int currentCount = count();
    int widgetCount = tabWidget->count();

    // Remove excess tabs
    while (currentCount > widgetCount) {
        removeTab(currentCount - 1);
        currentCount--;
    }

    // Add missing tabs
    while (currentCount < widgetCount) {
        insertTab(currentCount, QString());
        currentCount++;
    }

    // Update tab texts and icons
    for (int i = 0; i < widgetCount; ++i) {
        QString text = tabWidget->tabText(i);
        QIcon icon = tabWidget->tabIcon(i);
        QString tooltip = tabWidget->tabToolTip(i);
        setTabText(i, text);
        setTabIcon(i, icon);
        setTabToolTip(i, tooltip);
        updateTabAppearance(i);
    }

    // Sync current index
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < count()) {
        setCurrentIndex(currentIndex);
    }
}

void TabBar::setCurrentTab(int index) {
    setCurrentIndex(index);
}

void TabBar::onTabCloseRequested(int index) {
    if (index >= 0 && index < count()) {
        if (isTabLocked(index)) {
            QToolTip::showText(QCursor::pos(), "This tab is locked and cannot be closed");
            return;
        }
        emit tabClosed(index);
        emit closeTabRequested(index);
    }
}

void TabBar::closeTab(int index) {
    if (index >= 0 && index < count() && !isTabLocked(index)) {
        removeTab(index);
    }
}

void TabBar::closeOtherTabs(int index) {
    emit closeOtherTabsRequested(index);
}

void TabBar::closeAllTabs() {
    emit closeAllTabsRequested();
}

void TabBar::pinTab(int index) {
    if (index >= 0 && index < count()) {
        m_pinnedTabs.insert(index);
        updateTabAppearance(index);
        emit tabPinned(index);
    }
}

void TabBar::unpinTab(int index) {
    if (index >= 0 && index < count()) {
        m_pinnedTabs.remove(index);
        updateTabAppearance(index);
        emit tabUnpinned(index);
    }
}

bool TabBar::isTabPinned(int index) const {
    return m_pinnedTabs.contains(index);
}

void TabBar::lockTab(int index, bool lock) {
    if (index >= 0 && index < count()) {
        if (lock) {
            m_lockedTabs.insert(index);
        } else {
            m_lockedTabs.remove(index);
        }
        updateTabAppearance(index);
        emit tabLocked(index, lock);
    }
}

bool TabBar::isTabLocked(int index) const {
    return m_lockedTabs.contains(index);
}

void TabBar::setTabModified(int index, bool modified) {
    if (index >= 0 && index < count()) {
        if (modified) {
            m_modifiedTabs.insert(index);
        } else {
            m_modifiedTabs.remove(index);
        }
        updateTabAppearance(index);
    }
}

bool TabBar::isTabModified(int index) const {
    return m_modifiedTabs.contains(index);
}

void TabBar::setTabReadOnly(int index, bool readOnly) {
    if (index >= 0 && index < count()) {
        if (readOnly) {
            m_readOnlyTabs.insert(index);
        } else {
            m_readOnlyTabs.remove(index);
        }
        updateTabAppearance(index);
    }
}

bool TabBar::isTabReadOnly(int index) const {
    return m_readOnlyTabs.contains(index);
}

void TabBar::setExternalDropEnabled(bool enabled) {
    m_externalDropEnabled = enabled;
}

void TabBar::setScrollEnabled(bool enabled) {
    m_scrollEnabled = enabled;
    setUsesScrollButtons(enabled);
}

void TabBar::scrollToTab(int index) {
    if (index >= 0 && index < count()) {
        ensureTabVisible(index);
    }
}

void TabBar::showTabListMenu() {
    emit tabListRequested();
}

void TabBar::saveTabOrder(QSettings& settings) const {
    QStringList order;
    for (int i = 0; i < count(); ++i) {
        order.append(tabData(i).toString());
    }
    settings.setValue("TabBar/TabOrder", order);
}

void TabBar::restoreTabOrder(const QSettings& settings) {
    QStringList order = settings.value("TabBar/TabOrder").toStringList();
    if (order.isEmpty()) return;
    
    // Reorder tabs based on saved order
    QMap<QString, int> indexMap;
    for (int i = 0; i < count(); ++i) {
        indexMap[tabData(i).toString()] = i;
    }
    
    QList<int> newOrder;
    for (const QString& id : order) {
        if (indexMap.contains(id)) {
            newOrder.append(indexMap[id]);
        }
    }
    
    // Move tabs to new positions
    for (int i = 0; i < newOrder.size() && i < count(); ++i) {
        if (newOrder[i] != i) {
            moveTab(newOrder[i], i);
        }
    }
}

void TabBar::updateTabAppearance(int index) {
    if (index < 0 || index >= count()) return;
    
    QString text = tabText(index);
    
    // Add indicators for special states
    if (m_modifiedTabs.contains(index)) {
        if (!text.endsWith(" *")) {
            text += " *";
        }
    }
    
    if (m_pinnedTabs.contains(index)) {
        if (!text.startsWith("📌 ")) {
            text = "📌 " + text;
        }
    }
    
    if (m_readOnlyTabs.contains(index)) {
        if (!text.endsWith(" [RO]")) {
            text += " [RO]";
        }
    }
    
    setTabText(index, text);
    
    // Update icon for locked tabs
    if (m_lockedTabs.contains(index)) {
        // Could set a lock icon here
    }
}

void TabBar::onMiddleMouseClicked(int index) {
    if (index >= 0 && index < count()) {
        if (isTabLocked(index)) {
            return;
        }
        emit closeTabRequested(index);
    }
}

void TabBar::setCloseButtonVisible(bool visible) {
    m_showCloseButtons = visible;
    setTabsClosable(visible);
}

void TabBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        int tabIndex = tabAt(event->pos());
        if (tabIndex >= 0) {
            onMiddleMouseClicked(tabIndex);
            event->accept();
            return;
        }
    }
    
    if (event->button() == Qt::LeftButton) {
        m_dragStartTab = tabAt(event->pos());
        m_dragStartPos = event->pos();
    }
    
    QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton && m_dragStartTab >= 0) {
        QPoint diff = event->pos() - m_dragStartPos;
        
        // Start drag after moving a few pixels
        if (QLineF(event->pos(), m_dragStartPos).length() > QApplication::startDragDistance()) {
            if (!m_isDragging) {
                m_isDragging = true;
                performDrag();
            }
        }
    }
    
    // Update close button hover state
    int hoverTab = tabAt(event->pos());
    if (hoverTab != m_closeButtonHoverTab) {
        m_closeButtonHoverTab = hoverTab;
        update();
    }
    
    QTabBar::mouseMoveEvent(event);
}

void TabBar::mouseReleaseEvent(QMouseEvent* event) {
    m_dragStartTab = -1;
    m_isDragging = false;
    m_isDraggingExternal = false;
    clearDropHighlight();
    QTabBar::mouseReleaseEvent(event);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int tabIndex = tabAt(event->pos());
        if (tabIndex >= 0) {
            // Double-click on tab could open file properties or show context menu
            contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, event->pos(), event->globalPos()));
        } else {
            // Double-click on empty area creates new tab
            emit newTabRequested();
        }
    }
}

void TabBar::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/tab-index")) {
        event->acceptProposedAction();
        return;
    }
    
    if (m_externalDropEnabled && (event->mimeData()->hasUrls() || event->mimeData()->hasText())) {
        event->acceptProposedAction();
        m_isDraggingExternal = true;
        return;
    }
    
    QTabBar::dragEnterEvent(event);
}

void TabBar::dragMoveEvent(QDragMoveEvent* event) {
    int targetIndex = tabAt(event->pos());
    
    if (targetIndex >= 0) {
        highlightDropTarget(targetIndex);
    } else {
        clearDropHighlight();
    }
    
    event->acceptProposedAction();
    QTabBar::dragMoveEvent(event);
}

void TabBar::dragLeaveEvent(QDragLeaveEvent* event) {
    clearDropHighlight();
    QTabBar::dragLeaveEvent(event);
}

void TabBar::dropEvent(QDropEvent* event) {
    clearDropHighlight();
    
    if (event->mimeData()->hasFormat("application/tab-index")) {
        QTabBar::dropEvent(event);
        return;
    }
    
    if (m_externalDropEnabled && event->mimeData()->hasUrls()) {
        // Handle file drop - emit signal
        const QList<QUrl>& urls = event->mimeData()->urls();
        for (const QUrl& url : urls) {
            if (url.isLocalFile()) {
                emit tabDroppedOutside(-1, event->pos()); // For opening in new tab
            }
        }
    }
    
    QTabBar::dropEvent(event);
}

void TabBar::wheelEvent(QWheelEvent* event) {
    if (m_scrollEnabled) {
        int delta = event->angleDelta().y();
        if (delta != 0) {
            scrollTabs(delta > 0 ? -1 : 1);
        }
    }
    QTabBar::wheelEvent(event);
}

void TabBar::contextMenuEvent(QContextMenuEvent* event) {
    int tabIndex = tabAt(event->pos());
    showTabContextMenu(event->globalPos(), tabIndex);
}

bool TabBar::event(QEvent* event) {
    return QTabBar::event(event);
}

void TabBar::startDrag(int tabIndex) {
    m_dragStartTab = tabIndex;
    m_isDragging = true;
    performDrag();
}

void TabBar::performDrag() {
    if (m_dragStartTab < 0 || m_dragStartTab >= count()) return;
    
    auto* drag = new QDrag(this);
    auto* mime = new QMimeData;
    
    mime->setData("application/tab-index", QByteArray::number(m_dragStartTab));
    mime->setData("application/tab-text", tabText(m_dragStartTab).toUtf8());
    
    drag->setMimeData(mime);
    drag->setPixmap(QIcon().pixmap(16, 16));
    
    Qt::DropAction action = drag->exec(Qt::MoveAction | Qt::CopyAction);
    
    if (action == Qt::MoveAction) {
        // Tab was moved
    } else if (action == Qt::CopyAction) {
        // Tab was duplicated (dragged to another window)
    }
    
    m_isDragging = false;
}

int TabBar::hitTestTab(const QPoint& pos) const {
    return tabAt(pos);
}

QRect TabBar::tabRectFromIndex(int index) const {
    if (index < 0 || index >= count()) return QRect();
    return QTabBar::tabRect(index);
}

void TabBar::scrollTabs(int direction) {
    if (!m_scrollEnabled) return;
    
    QScrollBar* scrollBar = this->findChild<QScrollBar*>();
    if (scrollBar) {
        scrollBar->setValue(scrollBar->value() - direction * ScrollAmount);
    }
}

void TabBar::showTabContextMenu(const QPoint& pos, int tabIndex) {
    QMenu menu(this);
    
    if (tabIndex >= 0 && tabIndex < count()) {
        // Tab-specific actions
        QAction* closeAction = menu.addAction("Close");
        closeAction->setEnabled(!isTabLocked(tabIndex));
        
        QAction* closeOthersAction = menu.addAction("Close Other Tabs");
        closeOthersAction->setEnabled(count() > 1);
        
        QAction* closeAllAction = menu.addAction("Close All Tabs");
        
        menu.addSeparator();
        
        if (isTabPinned(tabIndex)) {
            menu.addAction("Unpin Tab")->setData("unpin");
        } else {
            menu.addAction("Pin Tab")->setData("pin");
        }
        
        if (isTabLocked(tabIndex)) {
            menu.addAction("Unlock Tab")->setData("unlock");
        } else {
            menu.addAction("Lock Tab")->setData("lock");
        }
        
        menu.addSeparator();
        
        menu.addAction("Duplicate Tab")->setData("duplicate");
        menu.addAction("Open in New Window")->setData("newWindow");
        
        menu.addSeparator();
        
        menu.addAction("Copy Full Path to Clipboard")->setData("copyPath");
        menu.addAction("Copy File Name to Clipboard")->setData("copyName");
        
    } else {
        // Empty area context menu
        menu.addAction("New Tab")->setData("newTab");
        menu.addAction("New Window")->setData("newWindow");
    }
    
    menu.addSeparator();
    menu.addAction("Close Tab Bar")->setData("closeBar");
    
    QAction* action = menu.exec(pos);
    if (!action) return;
    
    QString data = action->data().toString();
    
    if (tabIndex >= 0) {
        if (data == "pin") pinTab(tabIndex);
        else if (data == "unpin") unpinTab(tabIndex);
        else if (data == "lock") lockTab(tabIndex, true);
        else if (data == "unlock") lockTab(tabIndex, false);
        else if (data == "duplicate") emit tabClosed(-tabIndex - 1); // Negative means duplicate
        else if (data == "newWindow") emit tabDroppedOutside(tabIndex, pos);
        else if (data == "copyPath") {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(tabToolTip(tabIndex));
        }
        else if (data == "copyName") {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(tabText(tabIndex));
        }
    }
    
    if (data == "newTab") emit newTabRequested();
}

void TabBar::onScrollTimer() {
    scrollTabs(m_scrollDirection);
}

void TabBar::onDragScrollTimer() {
    QPoint pos = QCursor::pos();
    QRect rect = geometry();
    rect.moveTo(mapToGlobal(QPoint(0, 0)));
    
    if (pos.x() < rect.left() + DragScrollThreshold) {
        scrollTabs(-1);
    } else if (pos.x() > rect.right() - DragScrollThreshold) {
        scrollTabs(1);
    }
}

void TabBar::onCloseOtherTabs() {
    if (currentIndex() >= 0) {
        emit closeOtherTabsRequested(currentIndex());
    }
}

void TabBar::onCloseAllTabs() {
    emit closeAllTabsRequested();
}

void TabBar::onPinTab() {
    int idx = currentIndex();
    if (idx >= 0) pinTab(idx);
}

void TabBar::onUnpinTab() {
    int idx = currentIndex();
    if (idx >= 0) unpinTab(idx);
}

void TabBar::onLockTab() {
    int idx = currentIndex();
    if (idx >= 0) lockTab(idx, !isTabLocked(idx));
}

void TabBar::onDuplicateTab() {
    // Emit signal for duplicating current tab
}

void TabBar::onOpenInNewWindow() {
    int idx = currentIndex();
    if (idx >= 0) {
        emit tabDroppedOutside(idx, QCursor::pos());
    }
}

void TabBar::animateTabClose(int index) {
    // Could add animation here
    removeTab(index);
}

void TabBar::highlightDropTarget(int index) {
    m_dropTargetIndex = index;
    update();
}

void TabBar::clearDropHighlight() {
    if (m_dropTargetIndex >= 0) {
        m_dropTargetIndex = -1;
        update();
    }
}

// Stub implementations for methods declared in header but not yet defined
void TabBar::setTabToolTip(int index, const QString& tip) {
    QTabBar::setTabToolTip(index, tip);
}

void TabBar::moveTab(int from, int to) {
    QTabBar::moveTab(from, to);
}

int TabBar::tabAt(const QPoint& pos) const {
    return QTabBar::tabAt(pos);
}

void TabBar::ensureTabVisible(int index) {
    // For basic implementation, just ensure the tab is visible
    if (index >= 0 && index < count()) {
        setCurrentIndex(index);
    }
}
