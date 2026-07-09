// TabBar.h - Tab bar for file tabs
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QTabBar>

class QTabWidget;

class TabBar : public QTabBar {
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);
    ~TabBar() override;

    void updateFrom(QTabWidget* tabWidget);
    void setCurrentTab(int index);

signals:
    void tabMovedSignal(int from, int to);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    int _dragStartTab = -1;
    QPoint _dragStartPos;
};
