// DocTabView.cpp - Advanced document tab widget
// Copyright (C) 2026 Agent Army
// GPL v3

#include "DocTabView.h"
#include "ScintillaEditor.h"
#include <QAbstractItemView>
#include <QTabBar>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include <QStyle>
#include <QStyleOptionTab>
#include <QPushButton>

// ============================================================================
// Construction
// ============================================================================

DocTabView::DocTabView(QWidget* parent)
    : QTabWidget(parent)
{
    // Use a movable, closable tab bar
    tabBar()->setMovable(true);
    tabBar()->setTabsClosable(true);
    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    tabBar()->setAcceptDrops(true);
    // Context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTabWidget::customContextMenuRequested,
            this, &DocTabView::onContextMenu);

    // Close button signal
    connect(tabBar(), &QTabBar::tabCloseRequested,
            this, &DocTabView::onTabCloseRequested);

    // Current changed signal
    connect(this, &QTabWidget::currentChanged,
            this, &DocTabView::onCurrentChanged);

    // Tab moved (drag reorder)
    connect(tabBar(), &QTabBar::tabMoved,
            this, &DocTabView::onTabMovedWhileDragging);

    setupContextMenu();
    setupNewTabButton();
}

DocTabView::~DocTabView() = default;

// ============================================================================
// Context menu
// ============================================================================

void DocTabView::setupContextMenu() {
    _contextMenu = new QMenu(this);

    _closeAct = new QAction(tr("Close"), this);
    _closeOthersAct = new QAction(tr("Close Other Tabs"), this);
    _closeAllAct = new QAction(tr("Close All Tabs"), this);
    _closeToLeftAct = new QAction(tr("Close Tabs to the Left"), this);
    _closeToRightAct = new QAction(tr("Close Tabs to the Right"), this);
    _moveLeftAct = new QAction(tr("Move Tab Left"), this);
    _moveRightAct = new QAction(tr("Move Tab Right"), this);
    _pinAct = new QAction(tr("Pin Tab"), this);
    _newTabAct = new QAction(tr("New Tab"), this);

    connect(_closeAct, &QAction::triggered, this, &DocTabView::onCloseTriggered);
    connect(_closeOthersAct, &QAction::triggered, this, &DocTabView::onCloseOthersTriggered);
    connect(_closeAllAct, &QAction::triggered, this, &DocTabView::onCloseAllTriggered);
    connect(_closeToLeftAct, &QAction::triggered, this, &DocTabView::onCloseToLeftTriggered);
    connect(_closeToRightAct, &QAction::triggered, this, &DocTabView::onCloseToRightTriggered);
    connect(_moveLeftAct, &QAction::triggered, this, &DocTabView::onMoveLeftTriggered);
    connect(_moveRightAct, &QAction::triggered, this, &DocTabView::onMoveRightTriggered);
    connect(_pinAct, &QAction::triggered, this, &DocTabView::onPinTabTriggered);
    connect(_newTabAct, &QAction::triggered, this, &DocTabView::onNewTabTriggered);

    _contextMenu->addAction(_newTabAct);
    _contextMenu->addSeparator();
    _contextMenu->addAction(_closeAct);
    _contextMenu->addAction(_closeOthersAct);
    _contextMenu->addAction(_closeAllAct);
    _contextMenu->addAction(_closeToLeftAct);
    _contextMenu->addAction(_closeToRightAct);
    _contextMenu->addSeparator();
    _contextMenu->addAction(_moveLeftAct);
    _contextMenu->addAction(_moveRightAct);
    _contextMenu->addSeparator();
    _contextMenu->addAction(_pinAct);
}

void DocTabView::onContextMenu(const QPoint& pos) {
    int idx = tabBar()->tabAt(tabBar()->mapFrom(this, pos));
    if (idx < 0) idx = currentIndex();

    bool hasTabs = count() > 0;
    bool hasOthers = count() > 1;
    _closeAct->setEnabled(hasTabs && idx >= 0);
    _closeOthersAct->setEnabled(hasOthers);
    _closeAllAct->setEnabled(hasTabs);
    _closeToLeftAct->setEnabled(idx > 0);
    _closeToRightAct->setEnabled(idx >= 0 && idx < count() - 1);
    _moveLeftAct->setEnabled(idx > 0);
    _moveRightAct->setEnabled(idx >= 0 && idx < count() - 1);

    bool pinned = (idx >= 0) ? isTabPinned(idx) : false;
    _pinAct->setText(pinned ? tr("Unpin Tab") : tr("Pin Tab"));
    _pinAct->setEnabled(idx >= 0);

    _contextMenu->popup(mapToGlobal(pos));
}

void DocTabView::onCloseTriggered() {
    int idx = tabBar()->currentIndex();
    if (idx >= 0)
        closeTab(idx);
}

void DocTabView::onCloseOthersTriggered() {
    int keep = tabBar()->currentIndex();
    if (keep < 0) return;
    for (int i = count() - 1; i >= 0; --i) {
        if (i != keep && !isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::onCloseAllTriggered() {
    for (int i = count() - 1; i >= 0; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
    emit allTabsClosed();
}

void DocTabView::onCloseToLeftTriggered() {
    int keep = tabBar()->currentIndex();
    if (keep <= 0) return;
    for (int i = keep - 1; i >= 0; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::onCloseToRightTriggered() {
    int keep = tabBar()->currentIndex();
    if (keep < 0) return;
    for (int i = count() - 1; i > keep; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::onMoveLeftTriggered() {
    int idx = tabBar()->currentIndex();
    if (idx > 0)
        moveTab(idx, idx - 1);
}

void DocTabView::onMoveRightTriggered() {
    int idx = tabBar()->currentIndex();
    if (idx >= 0 && idx < count() - 1)
        moveTab(idx, idx + 1);
}

void DocTabView::onPinTabTriggered() {
    int idx = tabBar()->currentIndex();
    if (idx < 0) return;
    bool pinned = isTabPinned(idx);
    setTabPinned(idx, !pinned);
}

void DocTabView::onNewTabTriggered() {
    emit newTabRequested();
}

// ============================================================================
// Tab management
// ============================================================================

int DocTabView::addTab(ScintillaEditor* editor, const QString& title) {
    return QTabWidget::addTab(editor, title);
}

int DocTabView::addTab(ScintillaEditor* editor, const QIcon& icon,
                       const QString& title) {
    return QTabWidget::addTab(editor, icon, title);
}

int DocTabView::insertTab(int index, ScintillaEditor* editor,
                          const QString& title) {
    return QTabWidget::insertTab(index, editor, title);
}

int DocTabView::insertTab(int index, ScintillaEditor* editor,
                          const QIcon& icon, const QString& title) {
    return QTabWidget::insertTab(index, editor, icon, title);
}

void DocTabView::closeTab(int index) {
    if (index < 0 || index >= count())
        return;

    // Pinned tabs cannot be closed
    if (isTabPinned(index))
        return;

    // Notify before removal so listeners can save state
    emit tabClosed(index);

    // Remove auxiliary data
    _tabData.remove(index);

    // Remove the tab; widget is not deleted automatically
    QWidget* w = widget(index);
    removeTab(index);

    // Note: we don't delete the editor here — ownership may be transferred
    // to a "recently closed" stack or handled by FileManager
    Q_UNUSED(w);
}

void DocTabView::moveTab(int fromIndex, int toIndex) {
    if (fromIndex == toIndex) return;
    if (fromIndex < 0 || fromIndex >= count()) return;
    if (toIndex < 0 || toIndex >= count()) return;

    // Preserve auxiliary data
    TabData data = tabData(fromIndex);

    QString text = tabText(fromIndex);
    QIcon icon = tabIcon(fromIndex);
    QString tooltip = tabToolTip(fromIndex);
    QWidget* w = widget(fromIndex);

    // Remove from source position
    removeTab(fromIndex);

    // Re-insert at destination
    int actualTo = (fromIndex < toIndex) ? qMax(0, toIndex - 1) : toIndex;
    insertTab(actualTo, static_cast<ScintillaEditor*>(w), icon, text);
    setTabToolTip(actualTo, tooltip);

    // Restore auxiliary data
    if (auto* ed = qobject_cast<ScintillaEditor*>(w)) {
        // Editor-specific restore if needed
    }
    _tabData[actualTo] = data;

    // Apply pinned appearance if needed
    if (data.pinned)
        updateTabPinnedAppearance(actualTo);

    // Apply color if needed
    if (data.color.isValid())
        setTabColor(actualTo, data.color);

    // Set current tab to the moved tab
    setCurrentIndex(actualTo);

    emit tabMoved(fromIndex, actualTo);
}

ScintillaEditor* DocTabView::editorAt(int index) const {
    if (index < 0 || index >= count())
        return nullptr;
    return qobject_cast<ScintillaEditor*>(widget(index));
}

void DocTabView::updateTabTitle(int index, const QString& title) {
    if (index < 0 || index >= count())
        return;
    setTabText(index, title);
}

void DocTabView::setTabIcon(int index, const QIcon& icon) {
    if (index < 0 || index >= count())
        return;
    QTabWidget::setTabIcon(index, icon);
}

// ============================================================================
// Pinned tabs
// ============================================================================

void DocTabView::setTabPinned(int index, bool pinned) {
    if (index < 0 || index >= count())
        return;
    tabData(index).pinned = pinned;
    updateTabPinnedAppearance(index);
    emit tabPinnedChanged(index, pinned);
}

bool DocTabView::isTabPinned(int index) const {
    if (index < 0 || index >= count())
        return false;
    return tabData(index).pinned;
}

void DocTabView::updateTabPinnedAppearance(int index) {
    if (index < 0 || index >= count())
        return;
    QString text = tabText(index);
    if (tabData(index).pinned && !text.startsWith(QStringLiteral("\U0001F4CC "))) {
        // Add a pin emoji prefix to indicate pinned state
        setTabText(index, QStringLiteral("\U0001F4CC ") + text);
    } else if (!tabData(index).pinned && text.startsWith(QStringLiteral("\U0001F4CC "))) {
        setTabText(index, text.mid(3));  // Remove pin emoji
    }
}

// ============================================================================
// Tab color indicators
// ============================================================================

void DocTabView::setTabColor(int index, const QColor& color) {
    if (index < 0 || index >= count())
        return;
    tabData(index).color = color;
    if (color.isValid()) {
        // Set tab bar background color hint
        tabBar()->setTabData(index, color);
        // Repaint the tab bar
        tabBar()->update();
    }
    emit tabColorChanged(index, color);
}

QColor DocTabView::tabColor(int index) const {
    if (index < 0 || index >= count())
        return QColor();
    return tabData(index).color;
}

// ============================================================================
// "New tab" button
// ============================================================================

void DocTabView::setupNewTabButton() {
    // Create a "+" button as the corner widget (after close buttons)
    // We use the tab bar's corner widget area
    _newTabBtn = new QPushButton("+", this);
    _newTabBtn->setObjectName("NewTabButton");
    _newTabBtn->setToolTip(tr("New Tab (Ctrl+N)"));
    _newTabBtn->setFixedSize(24, 24);
    _newTabBtn->setFlat(true);
    connect(_newTabBtn, &QPushButton::clicked, this, &DocTabView::onNewTabTriggered);
    setCornerWidget(_newTabBtn, Qt::TopRightCorner);
}

void DocTabView::setNewTabButtonVisible(bool visible) {
    if (_newTabBtn) {
        _newTabBtn->setVisible(visible);
        // Adjust corner widget size hint
        setCornerWidget(visible ? _newTabBtn : nullptr, Qt::TopRightCorner);
    }
}

// ============================================================================
// Modified indicator
// ============================================================================

void DocTabView::setTabModified(int index, bool modified) {
    if (index < 0 || index >= count())
        return;
    tabData(index).modified = modified;
    QString text = tabText(index);
    // Remove leading "•" if present
    if (text.startsWith(QStringLiteral("• ")))
        text = text.mid(2);
    if (modified)
        text = QStringLiteral("• ") + text;
    setTabText(index, text);
}

bool DocTabView::isTabModified(int index) const {
    if (index < 0 || index >= count())
        return false;
    return tabData(index).modified;
}

// ============================================================================
// Buffer-to-tab synchronization
// ============================================================================

void DocTabView::setTabBufferId(int index, const QString& bufferId) {
    if (index < 0 || index >= count())
        return;
    tabData(index).bufferId = bufferId;
}

QString DocTabView::tabBufferId(int index) const {
    if (index < 0 || index >= count())
        return QString();
    return tabData(index).bufferId;
}

int DocTabView::findTabByBufferId(const QString& bufferId) const {
    for (int i = 0; i < count(); ++i) {
        if (tabData(i).bufferId == bufferId)
            return i;
    }
    return -1;
}

// ============================================================================
// Tab state persistence
// ============================================================================

QStringList DocTabView::tabOrder() const {
    QStringList order;
    for (int i = 0; i < count(); ++i) {
        order.append(tabData(i).bufferId);
    }
    return order;
}

void DocTabView::restoreTabOrder(const QStringList& bufferIds) {
    // This requires coordination with the document buffer manager.
    // For now, just a hint that tabs should be ordered per bufferIds.
    Q_UNUSED(bufferIds);
    // Full implementation would:
    // 1. Find each buffer ID in bufferIds in order
    // 2. Move its tab to the matching position
}

// ============================================================================
// Context menu customization
// ============================================================================

void DocTabView::addContextMenuAction(QAction* action) {
    if (_contextMenu)
        _contextMenu->addAction(action);
}

void DocTabView::addContextMenuSeparator() {
    if (_contextMenu)
        _contextMenu->addSeparator();
}

// ============================================================================
// QTabWidget overrides
// ============================================================================

void DocTabView::tabRemoved(int index) {
    QTabWidget::tabRemoved(index);
    // Remove tab data for this index and shift subsequent indices down
    _tabData.remove(index);
    // Re-index remaining tab data
    QMap<int, TabData> newData;
    for (auto it = _tabData.constBegin(); it != _tabData.constEnd(); ++it) {
        int key = it.key();
        if (key > index)
            newData[key - 1] = it.value();
        else if (key < index)
            newData[key] = it.value();
    }
    _tabData = newData;
    Q_UNUSED(index);
}

void DocTabView::tabInserted(int index) {
    QTabWidget::tabInserted(index);
    // Re-index tab data for insertion
    QMap<int, TabData> newData;
    for (auto it = _tabData.constBegin(); it != _tabData.constEnd(); ++it) {
        int key = it.key();
        if (key >= index)
            newData[key + 1] = it.value();
        else
            newData[key] = it.value();
    }
    _tabData = newData;
    Q_UNUSED(index);
}

void DocTabView::onTabCloseRequested(int index) {
    closeTab(index);
}

void DocTabView::onCurrentChanged(int index) {
    emit currentChanged(index);
}

void DocTabView::onTabBarClicked(int index) {
    Q_UNUSED(index);
}

// ============================================================================
// Mouse events — middle-click to close, double-click to close, drag
// ============================================================================

void DocTabView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        int idx = tabBar()->tabAt(event->position().toPoint());
        if (idx >= 0 && !isTabPinned(idx)) {
            closeTab(idx);
            event->accept();
            return;
        }
    }
    QTabWidget::mousePressEvent(event);
}

void DocTabView::mouseReleaseEvent(QMouseEvent* event) {
    QTabWidget::mouseReleaseEvent(event);
}

void DocTabView::mouseDoubleClickEvent(QMouseEvent* event) {
    // Double-click on tab bar → close tab (unless pinned)
    if (event->button() == Qt::LeftButton) {
        int idx = tabBar()->tabAt(event->position().toPoint());
        if (idx >= 0 && !isTabPinned(idx)) {
            closeTab(idx);
            event->accept();
            return;
        }
    }
    QTabWidget::mouseDoubleClickEvent(event);
}

// Drag & drop between tab views

void DocTabView::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/tab-index")) {
        event->acceptProposedAction();
        return;
    }
    QTabWidget::dragEnterEvent(event);
}

void DocTabView::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasFormat("application/tab-index")) {
        _dropIndex = tabBar()->tabAt(event->position().toPoint());
        event->acceptProposedAction();
        return;
    }
    QTabWidget::dragMoveEvent(event);
}

void DocTabView::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/tab-index")) {
        QByteArray data = event->mimeData()->data("application/tab-index");
        int fromIdx = QString::fromUtf8(data).toInt();
        int toIdx = tabBar()->tabAt(event->position().toPoint());
        if (toIdx < 0) toIdx = count() - 1;

        if (fromIdx != toIdx) {
            moveTab(fromIdx, toIdx);
        }
        event->acceptProposedAction();
        return;
    }
    QTabWidget::dropEvent(event);
}

int DocTabView::tabIndexAt(const QPoint& pos) const {
    return tabBar()->tabAt(mapFromParent(pos));
}

// ============================================================================
// Convenience: close all / other
// ============================================================================

void DocTabView::closeAllTabs() {
    for (int i = count() - 1; i >= 0; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::closeOtherTabs(int keepIndex) {
    if (keepIndex < 0 || keepIndex >= count())
        return;
    for (int i = count() - 1; i >= 0; --i) {
        if (i != keepIndex && !isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::closeTabsToLeft(int index) {
    if (index <= 0) return;
    for (int i = index - 1; i >= 0; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
}

void DocTabView::closeTabsToRight(int index) {
    if (index < 0 || index >= count())
        return;
    for (int i = count() - 1; i > index; --i) {
        if (!isTabPinned(i))
            closeTab(i);
    }
}

// ============================================================================
// Tab data helpers
// ============================================================================

TabData& DocTabView::tabData(int index) {
    // Ensure an entry exists
    if (!_tabData.contains(index))
        _tabData[index] = TabData();
    return _tabData[index];
}

TabData DocTabView::tabData(int index) const {
    return _tabData.value(index);
}

void DocTabView::onTabMovedWhileDragging(int from, int to) {
    // Re-index tab data after a move
    Q_UNUSED(from);
    Q_UNUSED(to);
    // The built-in tab bar handles the move; our tab data is synced via
    // tabInserted/tabRemoved which are called by Qt's QTabWidget.
}
