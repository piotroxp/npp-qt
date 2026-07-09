// FileBrowserPanel.h - File browser panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>
#include <QAction>

class FileBrowserPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FileBrowserPanel(QWidget* parent = nullptr);
    ~FileBrowserPanel() override;

    void setRootDirectory(const QString& dir);
    QString rootDirectory() const;
    void setCurrentProjectDirectory(const QString& dir);
    QString currentProjectDirectory() const { return _currentProjectDir; }

    void refresh();
    void navigateUp();
    void navigateHome();

signals:
    void fileDoubleClicked(const QString& path);

private slots:
    void onDoubleClicked(const QModelIndex& index);
    void onContextMenu(const QPoint& pos);
    void openSelectedFile();
    void copyPathToClipboard();
    void revealInFileManager();

private:
    QTreeView* _treeView = nullptr;
    QFileSystemModel* _model = nullptr;
    QToolBar* _toolBar = nullptr;
    QAction* _refreshAction = nullptr;
    QAction* _upAction = nullptr;
    QAction* _homeAction = nullptr;
    QString _currentProjectDir;
    QStringList _nameFilters;
};
