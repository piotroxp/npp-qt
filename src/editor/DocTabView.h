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
#include <QDropEvent>
#include <QMimeData>

class ScintillaEditor;

class DocTabView : public QTabWidget {
    Q_OBJECT

public:
    explicit DocTabView(QWidget* parent = nullptr);
    ~DocTabView() override;

    // Add an editor tab with title; returns tab index
    int addTab(ScintillaEditor* editor, const QString& title);

    // Convenience: add tab with icon
    int addTab(ScintillaEditor* editor, const QIcon& icon, const QString& title);

    // Close tab at index
    void closeTab(int index);

    // Move tab from one position to another (for drag-reorder)
    void moveTab(int fromIndex, int toIndex);

    // Close all tabs
    void closeAllTabs();

    // Close all tabs except the given index
    void closeOtherTabs(int keepIndex);

    // Get editor at tab index
    ScintillaEditor* editorAt(int index) const;

    // Reload tab title (e.g., after file rename)
    void updateTabTitle(int index, const QString& title);

    // Tab bar access
    QTabBar* tabBar() const { return QTabWidget::tabBar(); }

signals:
    void tabClosed(int index);
    void tabMoved(int from, int to);
    void currentChanged(int index);
    void allTabsClosed();

protected:
    // Handle tab removal
    void tabRemoved(int index) override;

    // Handle tab insertion
    void tabInserted(int index) override;

    // Mouse events for middle-click close and drag-reorder
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onTabCloseRequested(int index);
    void onContextMenu(const QPoint& pos);
    void onCloseTriggered();
    void onCloseOthersTriggered();
    void onCloseAllTriggered();
    void onMoveLeftTriggered();
    void onMoveRightTriggered();

private:
    void setupContextMenu();
    int tabIndexAt(const QPoint& pos) const;

    // Drop index during drag
    int _dropIndex = -1;
    int _dragSourceIndex = -1;

    QMenu* _contextMenu = nullptr;
    QAction* _closeAct = nullptr;
    QAction* _closeOthersAct = nullptr;
    QAction* _closeAllAct = nullptr;
    QAction* _moveLeftAct = nullptr;
    QAction* _moveRightAct = nullptr;
    QAction* _pinAct = nullptr;
};
