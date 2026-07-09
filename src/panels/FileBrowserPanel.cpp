// FileBrowserPanel.cpp - File browser panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FileBrowserPanel.h"
#include "../core/Application.h"
#include <QVBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDir>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QHeaderView>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>

FileBrowserPanel::FileBrowserPanel(QWidget* parent)
    : QDockWidget("File Browser", parent)
{
    setObjectName("FileBrowserPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    QWidget* content = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create toolbar
    _toolBar = new QToolBar(content);
    _toolBar->setMovable(false);
    _toolBar->setFloatable(false);
    _toolBar->setIconSize(QSize(16, 16));

    // Refresh action
    _refreshAction = new QAction(QIcon::fromTheme("view-refresh", QIcon(":/icons/refresh")), tr("Refresh"), this);
    _refreshAction->setToolTip(tr("Refresh (F5)"));
    _toolBar->addAction(_refreshAction);

    // Up directory action
    _upAction = new QAction(QIcon::fromTheme("go-up", QIcon(":/icons/up")), tr("Up"), this);
    _upAction->setToolTip(tr("Go to parent directory"));
    _toolBar->addAction(_upAction);

    // Home directory action
    _homeAction = new QAction(QIcon::fromTheme("go-home", QIcon(":/icons/home")), tr("Home"), this);
    _homeAction->setToolTip(tr("Go to home directory"));
    _toolBar->addAction(_homeAction);

    layout->addWidget(_toolBar);

    // Create file system model
    _model = new QFileSystemModel(this);
    _model->setRootPath("");

    // Set name filters for source code files
    _nameFilters = QStringList()
        << "*.cpp" << "*.h" << "*.hpp" << "*.cxx" << "*.c" << "*.cc"
        << "*.py" << "*.pyw" << "*.js" << "*.jsx" << "*.ts" << "*.tsx"
        << "*.java" << "*.cs" << "*.go" << "*.rs" << "*.swift"
        << "*.txt" << "*.md" << "*.markdown" << "*.rst"
        << "*.xml" << "*.xaml" << "*.ui"
        << "*.json" << "*.yaml" << "*.yml" << "*.toml"
        << "*.html" << "*.htm" << "*.css" << "*.scss" << "*.sass" << "*.less"
        << "*.sql" << "*.sh" << "*.bat" << "*.ps1"
        << "*.ini" << "*.cfg" << "*.conf" << "*.properties"
        << "*.glsl" << "*.vert" << "*.frag"
        << "CMakeLists.txt" << "Makefile*" << "*.pro" << "*.pri";
    _model->setNameFilters(_nameFilters);
    _model->setNameFilterDisables(false);  // Hide filtered files instead of disabling

    // Create tree view
    _treeView = new QTreeView(content);
    _treeView->setModel(_model);
    _treeView->setRootIndex(_model->index(QDir::homePath()));
    _treeView->setHeaderHidden(false);
    _treeView->setAlternatingRowColors(true);
    _treeView->setSortingEnabled(true);
    _treeView->sortByColumn(0, Qt::AscendingOrder);
    _treeView->setAnimated(true);
    _treeView->setIndentation(20);
    _treeView->setExpandsOnDoubleClick(true);

    // Set column widths
    _treeView->header()->resizeSection(0, 200);  // Name
    _treeView->header()->resizeSection(1, 80);   // Size
    _treeView->header()->resizeSection(2, 120);  // Type
    _treeView->header()->resizeSection(3, 140);  // Date Modified

    layout->addWidget(_treeView);
    setWidget(content);

    // Connect signals
    connect(_treeView, &QTreeView::doubleClicked, this, &FileBrowserPanel::onDoubleClicked);
    connect(_refreshAction, &QAction::triggered, this, &FileBrowserPanel::refresh);
    connect(_upAction, &QAction::triggered, this, &FileBrowserPanel::navigateUp);
    connect(_homeAction, &QAction::triggered, this, &FileBrowserPanel::navigateHome);
    connect(_treeView, &QTreeView::customContextMenuRequested, this, &FileBrowserPanel::onContextMenu);

    // Initial directory
    QString initialDir = QDir::homePath();
    if (qApp->applicationDirPath().contains("/workspace")) {
        // Likely running in dev environment, use workspace as root
        initialDir = qApp->applicationDirPath();
        int wsIdx = initialDir.indexOf("/workspace");
        if (wsIdx > 0) {
            initialDir = initialDir.left(wsIdx + 10);
        }
    }
    setRootDirectory(initialDir);
}

FileBrowserPanel::~FileBrowserPanel() = default;

void FileBrowserPanel::setRootDirectory(const QString& dir) {
    QModelIndex idx = _model->index(dir);
    if (idx.isValid()) {
        _treeView->setRootIndex(idx);
        _model->setRootPath(dir);

        // Update up button state
        _upAction->setEnabled(dir != QDir::rootPath() && dir != QDir::homePath());
    }
}

QString FileBrowserPanel::rootDirectory() const {
    return _model->rootPath();
}

void FileBrowserPanel::setCurrentProjectDirectory(const QString& dir) {
    _currentProjectDir = dir;
    if (!dir.isEmpty()) {
        setRootDirectory(dir);
    }
}

void FileBrowserPanel::refresh() {
    if (!_treeView->rootIndex().isValid()) return;

    QString currentPath = _model->filePath(_treeView->rootIndex());
    _model->setRootPath("");  // Force refresh
    setRootDirectory(currentPath.isEmpty() ? QDir::homePath() : currentPath);
}

void FileBrowserPanel::navigateUp() {
    QString currentPath = _model->filePath(_treeView->rootIndex());
    QDir dir(currentPath);
    if (dir.cdUp()) {
        setRootDirectory(dir.path());
    }
}

void FileBrowserPanel::navigateHome() {
    setRootDirectory(QDir::homePath());
}

void FileBrowserPanel::onDoubleClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);

    if (info.isFile()) {
        emit fileDoubleClicked(path);
    } else if (info.isDir()) {
        // Navigate into directory on double-click
        setRootDirectory(path);
    }
}

void FileBrowserPanel::onContextMenu(const QPoint& pos) {
    QModelIndex index = _treeView->indexAt(pos);
    if (!index.isValid()) {
        // Show context menu for empty area
        QMenu menu(this);
        menu.addAction(_refreshAction);
        menu.addAction(_homeAction);
        menu.exec(_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);

    QMenu menu(this);

    // Open action (for files)
    if (info.isFile()) {
        QAction* openAction = menu.addAction(tr("Open"));
        connect(openAction, &QAction::triggered, this, &FileBrowserPanel::openSelectedFile);
    }

    // Open in new window (for directories)
    if (info.isDir()) {
        QAction* openNewAction = menu.addAction(tr("Open in New Window"));
        connect(openNewAction, &QAction::triggered, [this, path]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        });
    }

    // Copy path action
    QAction* copyPathAction = menu.addAction(tr("Copy Path"));
    connect(copyPathAction, &QAction::triggered, this, &FileBrowserPanel::copyPathToClipboard);

    // Reveal in file manager
    QAction* revealAction = menu.addAction(tr("Reveal in File Manager"));
    connect(revealAction, &QAction::triggered, this, &FileBrowserPanel::revealInFileManager);

    // Add separator and navigation options for directories
    if (info.isDir()) {
        menu.addSeparator();
        QAction* openHereAction = menu.addAction(tr("Open Folder as Project"));
        connect(openHereAction, &QAction::triggered, [this, path]() {
            setCurrentProjectDirectory(path);
        });
    }

    menu.exec(_treeView->viewport()->mapToGlobal(pos));
}

void FileBrowserPanel::openSelectedFile() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);

    if (info.isFile()) {
        emit fileDoubleClicked(path);
    }
}

void FileBrowserPanel::copyPathToClipboard() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;

    QString path = _model->filePath(index);
    QApplication::clipboard()->setText(path);
}

void FileBrowserPanel::revealInFileManager() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);

    // If it's a file, use its directory; otherwise use the directory itself
    QString dirPath = info.isFile() ? info.absolutePath() : path;

    // Try to open the file manager at the path
    bool opened = false;

#ifdef Q_OS_WIN
    opened = QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
#else
    // On Unix-like systems, try different file managers
    QProcess process;
    process.startDetached("xdg-mime", {"query", "inode/directory.directory"});
    
    // Use xdg-open which should open the file manager
    opened = QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
#endif

    Q_UNUSED(opened);
}
