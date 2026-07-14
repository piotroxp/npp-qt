// DocTabView.cpp - Advanced document tab widget
// Copyright (C) 2026 Agent Army
// GPL v3

#include "DocTabView.h"
#include "ScintillaEditor.h"
#include <QTabBar>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDebug>

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

    setupContextMenu();
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
    _moveLeftAct = new QAction(tr("Move Tab Left"), this);
    _moveRightAct = new QAction(tr("Move Tab Right"), this);
    _pinAct = new QAction(tr("Pin Tab"), this);

    connect(_closeAct, &QAction::triggered, this, &DocTabView::onCloseTriggered);
    connect(_closeOthersAct, &QAction::triggered, this, &DocTabView::onCloseOthersTriggered);
    connect(_closeAllAct, &QAction::triggered, this, &DocTabView::onCloseAllTriggered);
    connect(_moveLeftAct, &QAction::triggered, this, &DocTabView::onMoveLeftTriggered);
    connect(_moveRightAct, &QAction::triggered, this, &DocTabView::onMoveRightTriggered);
    connect(_pinAct, &QAction::triggered, this, [this] {
        int idx = tabBar()->currentIndex();
        if (idx >= 0) {
            ScintillaEditor* ed = editorAt(idx);
            // TODO: toggle pin state on buffer
            Q_UNUSED(ed);
        }
    });

    _contextMenu->addAction(_closeAct);
    _contextMenu->addAction(_closeOthersAct);
    _contextMenu->addAction(_closeAllAct);
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
    _closeAct->setEnabled(hasTabs && idx >= 0);
    _closeOthersAct->setEnabled(count() > 1);
    _closeAllAct->setEnabled(hasTabs);
    _moveLeftAct->setEnabled(idx > 0);
    _moveRightAct->setEnabled(idx >= 0 && idx < count() - 1);

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
    // Close in reverse to preserve indices
    for (int i = count() - 1; i >= 0; --i) {
        if (i != keep)
            closeTab(i);
    }
}

void DocTabView::onCloseAllTriggered() {
    for (int i = count() - 1; i >= 0; --i) {
        closeTab(i);
    }
    emit allTabsClosed();
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

void DocTabView::closeTab(int index) {
    if (index < 0 || index >= count())
        return;

    // Notify before removal so listeners can save state
    emit tabClosed(index);

    // Remove the tab; the widget (editor) is not deleted automatically
    // Caller is responsible for the editor lifetime
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

    QString text = tabText(fromIndex);
    QIcon icon = tabIcon(fromIndex);
    QString tooltip = tabToolTip(fromIndex);
    QWidget* w = widget(fromIndex);

    // Remove from source position
    removeTab(fromIndex);

    // Re-insert at destination
    // Account for the shift when source was before destination
    int actualTo = (fromIndex < toIndex) ? toIndex - 1 : toIndex;
    insertTab(actualTo, w, icon, text);
    setTabToolTip(actualTo, tooltip);

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

// ============================================================================
// QTabWidget overrides
// ============================================================================

void DocTabView::tabRemoved(int index) {
    QTabWidget::tabRemoved(index);
    // Tab index may be used for tracking; subclasses can hook here
    Q_UNUSED(index);
}

void DocTabView::tabInserted(int index) {
    QTabWidget::tabInserted(index);
    // Hook for post-insertion processing
    Q_UNUSED(index);
}

void DocTabView::onTabCloseRequested(int index) {
    closeTab(index);
}

void DocTabView::onCurrentChanged(int index) {
    emit currentChanged(index);
}

// ============================================================================
// Mouse events — middle-click to close, drag tab between views
// ============================================================================

void DocTabView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        int idx = tabBar()->tabAt(event->pos());
        if (idx >= 0) {
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
        _dropIndex = tabBar()->tabAt(event->pos());
        event->acceptProposedAction();
        return;
    }
    QTabWidget::dragMoveEvent(event);
}

void DocTabView::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/tab-index")) {
        QByteArray data = event->mimeData()->data("application/tab-index");
        int fromIdx = QString::fromUtf8(data).toInt();
        int toIdx = tabBar()->tabAt(event->pos());
        if (toIdx < 0) toIdx = count() - 1;

        // Move the tab from the source DocTabView to this one
        // This requires coordination with the source view — for now, move
        // within the same widget
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
        closeTab(i);
    }
}

void DocTabView::closeOtherTabs(int keepIndex) {
    if (keepIndex < 0 || keepIndex >= count())
        return;
    for (int i = count() - 1; i >= 0; --i) {
        if (i != keepIndex)
            closeTab(i);
    }
}
