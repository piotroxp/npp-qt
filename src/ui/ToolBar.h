// ToolBar.h - Main toolbar
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QToolBar>
#include <QString>

class ToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit ToolBar(QWidget* parent = nullptr);
    ~ToolBar() override;

    void addAction(QAction* action);
    void setIconSize(int size);

public slots:
    void onNew();
    void onOpen();
    void onSave();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onFind();
    void onReplace();

signals:
    void toolBarCommand(const QString& cmd);

private:
    void setupActions();
};
