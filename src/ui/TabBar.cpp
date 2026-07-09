// TabBar.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "TabBar.h"
#include <QTabWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

TabBar::TabBar(QWidget* parent) : QTabBar(parent) {
    setAcceptDrops(true);
    setMovable(true);
    setTabsClosable(true);
    setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
}

TabBar::~TabBar() = default;

void TabBar::updateFrom(QTabWidget* tabWidget) {
    if (!tabWidget) return;

    // Sync tab count and titles from QTabWidget
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

void TabBar::mousePressEvent(QMouseEvent* event) {
    _dragStartTab = tabAt(event->pos());
    _dragStartPos = event->pos();
    QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton && _dragStartTab >= 0) {
        QDrag* drag = new QDrag(this);
        QMimeData* mime = new QMimeData;
        mime->setData("application/tab-index", QByteArray::number(_dragStartTab));
        drag->setMimeData(mime);
        drag->exec();
    }
    QTabBar::mouseMoveEvent(event);
}

void TabBar::dragEnterEvent(QDragEnterEvent* event) {
    event->acceptProposedAction();
}

void TabBar::dropEvent(QDropEvent* event) {
    QTabBar::dropEvent(event);
}
