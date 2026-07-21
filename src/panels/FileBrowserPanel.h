// FileBrowserPanel.h - File browser panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>
#include <QAction>
#include <QFileSystemWatcher>
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QComboBox>
#include <QLineEdit>

class FileBrowserPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FileBrowserPanel(QWidget* parent = nullptr);
    ~FileBrowserPanel() override;

    void setRootDirectory(const QString& dir);
    QString rootDirectory() const;
    void setCurrentProjectDirectory(const QString& dir);
    QString currentProjectDirectory() const { return _currentProjectDir; }
    void followActiveDocument(const QString& filePath);

    void refresh();
    void navigateUp();
    void navigateHome();

    // Recent directories
    void addRecentDirectory(const QString& dir);
    QStringList recentDirectories() const { return _recentDirs; }

signals:
    void fileDoubleClicked(const QString& path);
    void directoryChanged(const QString& path);

private slots:
    void onDoubleClicked(const QModelIndex& index);
    void onContextMenu(const QPoint& pos);
    void openSelectedFile();
    void openReadOnly();
    void openInNewInstance();
    void copyPathToClipboard();
    void revealInFileManager();
    void openInTerminal();
    void renameSelected();
    void deleteSelected();
    void newFileHere();
    void newFolderHere();
    void addToWorkspace();
    void onFilterChanged(const QString& text);
    void onSortModeChanged(int index);
    void expandAll();
    void collapseAll();
    void toggleHiddenFiles();
    void toggleSystemFiles();
    void followCurrentDocument();
    void onRecentDirTriggered(QAction* action);
    void onDirectoryChanged(const QString& path);
    void onFileRenamed(const QString& path);

private:
    void setupToolbar();
    void setupBreadcrumbs();
    void setupCompleter();
    void updateBreadcrumbs();
    void syncFollowState();
    QStringList getSelectedPaths() const;
    void applySortMode();

    QTreeView* _treeView = nullptr;
    QFileSystemModel* _model = nullptr;
    QToolBar* _toolBar = nullptr;
    QFileSystemWatcher* _watcher = nullptr;
    QCompleter* _completer = nullptr;
    QStringListModel* _completerModel = nullptr;

    // Toolbar actions
    QAction* _refreshAction = nullptr;
    QAction* _upAction = nullptr;
    QAction* _homeAction = nullptr;
    QAction* _newFileAction = nullptr;
    QAction* _newFolderAction = nullptr;
    QAction* _collapseAllAction = nullptr;
    QAction* _expandAllAction = nullptr;
    QAction* _hiddenFilesAction = nullptr;
    QAction* _systemFilesAction = nullptr;
    QAction* _followAction = nullptr;

    // Filter bar
    QLineEdit* _filterEdit = nullptr;
    QComboBox* _sortCombo = nullptr;

    // Breadcrumb navigation
    QWidget* _breadcrumbBar = nullptr;
    QList<QAction*> _breadcrumbActions;

    // State
    QString _currentProjectDir;
    QStringList _nameFilters;
    bool _showHidden = false;
    bool _showSystem = false;
    int _sortMode = 0;  // 0=name, 1=date, 2=size, 3=type
    QString _lastFollowedPath;
    QStringList _recentDirs;
    static constexpr int MaxRecentDirs = 10;

    // Internal file path tracking for follow-active-document
    QString _activeFilePath;
    void openInFileManagerAt(const QString& path, bool selectFile = false);

    bool _initialized = false;  // guard: block interactions until fully constructed
};
