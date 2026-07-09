// ToolBar.cpp - Main toolbar implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ToolBar.h"
#include <QAction>
#include <QIcon>
#include <QStyle>
#include <QApplication>
#include <QSize>

ToolBar::ToolBar(QWidget* parent) : QToolBar(parent) {
    setMovable(true);
    setFloatable(true);
    setIconSize(16);
    setupActions();
}

ToolBar::~ToolBar() = default;

void ToolBar::setupActions() {
    // File actions
    QAction* newAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "New", this);
    newAction->setData("file.new");
    newAction->setToolTip("New (Ctrl+N)");
    addAction(newAction);
    
    QAction* openAction = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), "Open", this);
    openAction->setData("file.open");
    openAction->setToolTip("Open (Ctrl+O)");
    addAction(openAction);
    
    QAction* saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
    saveAction->setData("file.save");
    saveAction->setToolTip("Save (Ctrl+S)");
    addAction(saveAction);
    
    addSeparator();
    
    // Edit actions
    QAction* undoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), "Undo", this);
    undoAction->setData("edit.undo");
    undoAction->setToolTip("Undo (Ctrl+Z)");
    addAction(undoAction);
    
    QAction* redoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), "Redo", this);
    redoAction->setData("edit.redo");
    redoAction->setToolTip("Redo (Ctrl+Y)");
    addAction(redoAction);
    
    addSeparator();
    
    // Clipboard actions
    QAction* cutAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarMinButton), "Cut", this);
    cutAction->setData("edit.cut");
    cutAction->setToolTip("Cut (Ctrl+X)");
    addAction(cutAction);
    
    QAction* copyAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarMaxButton), "Copy", this);
    copyAction->setData("edit.copy");
    copyAction->setToolTip("Copy (Ctrl+C)");
    addAction(copyAction);
    
    QAction* pasteAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarNormalButton), "Paste", this);
    pasteAction->setData("edit.paste");
    pasteAction->setToolTip("Paste (Ctrl+V)");
    addAction(pasteAction);
    
    addSeparator();
    
    // Search actions
    QAction* findAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), "Find", this);
    findAction->setData("edit.find");
    findAction->setToolTip("Find (Ctrl+F)");
    addAction(findAction);
    
    QAction* replaceAction = new QAction(style()->standardIcon(QStyle::SP_DesktopIcon), "Replace", this);
    replaceAction->setData("edit.replace");
    replaceAction->setToolTip("Replace (Ctrl+H)");
    addAction(replaceAction);
    
    // Connect all actions to emit toolBarCommand signal
    for (QAction* action : actions()) {
        if (!action->data().isNull()) {
            connect(action, &QAction::triggered, this, [this, action]() {
                QString cmd = action->data().toString();
                if (!cmd.isEmpty()) {
                    emit toolBarCommand(cmd);
                }
            });
        }
    }
}

void ToolBar::addAction(QAction* action) {
    QToolBar::addAction(action);
}

void ToolBar::setIconSize(int size) {
    QToolBar::setIconSize(QSize(size, size));
}

void ToolBar::onNew() {
    emit toolBarCommand("file.new");
}

void ToolBar::onOpen() {
    emit toolBarCommand("file.open");
}

void ToolBar::onSave() {
    emit toolBarCommand("file.save");
}

void ToolBar::onUndo() {
    emit toolBarCommand("edit.undo");
}

void ToolBar::onRedo() {
    emit toolBarCommand("edit.redo");
}

void ToolBar::onCut() {
    emit toolBarCommand("edit.cut");
}

void ToolBar::onCopy() {
    emit toolBarCommand("edit.copy");
}

void ToolBar::onPaste() {
    emit toolBarCommand("edit.paste");
}

void ToolBar::onFind() {
    emit toolBarCommand("edit.find");
}

void ToolBar::onReplace() {
    emit toolBarCommand("edit.replace");
}
