// FileBrowserPanel.cpp - File browser panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FileBrowserPanel.h"
#include "../core/Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QToolButton>
#include <QLabel>
#include <QScrollArea>

FileBrowserPanel::FileBrowserPanel(QWidget* parent)
    : QDockWidget("File Browser", parent)
    , _treeView(nullptr)
    , _model(nullptr)
    , _toolBar(nullptr)
    , _watcher(nullptr)
    , _completer(nullptr)
    , _completerModel(nullptr)
    , _filterEdit(nullptr)
    , _sortCombo(nullptr)
    , _breadcrumbBar(nullptr)
    , _showHidden(false)
    , _showSystem(false)
    , _sortMode(0)
{
    setObjectName("FileBrowserPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    setMinimumWidth(200);

    QWidget* content = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Setup toolbar
    setupToolbar();
    mainLayout->addWidget(_toolBar);

    // Breadcrumb bar
    setupBreadcrumbs();
    mainLayout->addWidget(_breadcrumbBar);

    // Filter + sort row
    QWidget* filterRow = new QWidget(content);
    QHBoxLayout* filterLayout = new QHBoxLayout(filterRow);
    filterLayout->setContentsMargins(4, 2, 4, 2);
    filterLayout->setSpacing(4);

    _filterEdit = new QLineEdit(filterRow);
    _filterEdit->setPlaceholderText(tr("Filter (*.ext)..."));
    _filterEdit->setClearButtonEnabled(true);
    filterLayout->addWidget(_filterEdit, 1);

    _sortCombo = new QComboBox(filterRow);
    _sortCombo->setFixedWidth(110);
    _sortCombo->addItem(tr("Name"), 0);
    _sortCombo->addItem(tr("Date Modified"), 1);
    _sortCombo->addItem(tr("Size"), 2);
    _sortCombo->addItem(tr("Type"), 3);
    _sortCombo->setCurrentIndex(0);
    filterLayout->addWidget(_sortCombo);

    mainLayout->addWidget(filterRow);

    // Create file system model
    _model = new QFileSystemModel(this);
    _model->setRootPath("");
    _model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

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
        << "CMakeLists.txt" << "Makefile*" << "*.pro" << "*.pri"
        << "*.qml" << "*.qrc";
    _model->setNameFilters(_nameFilters);
    _model->setNameFilterDisables(false);

    // Create tree view
    _treeView = new QTreeView(content);
    _treeView->setModel(_model);
    _treeView->setRootIndex(_model->index(QDir::homePath()));
    _treeView->setHeaderHidden(false);
    _treeView->setAlternatingRowColors(true);
    _treeView->setSortingEnabled(false);  // We handle sorting ourselves
    _treeView->setAnimated(true);
    _treeView->setIndentation(16);
    _treeView->setExpandsOnDoubleClick(true);
    _treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView->setDragDropMode(QAbstractItemView::DragDrop);
    _treeView->setAcceptDrops(true);
    _treeView->viewport()->setAcceptDrops(true);

    // Enable custom context menu
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Set column widths
    _treeView->header()->resizeSection(0, 220);
    _treeView->header()->resizeSection(1, 80);
    _treeView->header()->resizeSection(2, 100);
    _treeView->header()->resizeSection(3, 140);

    mainLayout->addWidget(_treeView);

    // File system watcher
    _watcher = new QFileSystemWatcher(this);
    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &FileBrowserPanel::onDirectoryChanged);
    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &FileBrowserPanel::onFileRenamed);

    // Setup completer
    setupCompleter();

    setWidget(content);

    // Connect signals
    connect(_treeView, &QTreeView::doubleClicked, this, &FileBrowserPanel::onDoubleClicked);
    connect(_treeView, &QTreeView::customContextMenuRequested, this, &FileBrowserPanel::onContextMenu);
    connect(_refreshAction, &QAction::triggered, this, &FileBrowserPanel::refresh);
    connect(_upAction, &QAction::triggered, this, &FileBrowserPanel::navigateUp);
    connect(_homeAction, &QAction::triggered, this, &FileBrowserPanel::navigateHome);
    connect(_filterEdit, &QLineEdit::textChanged, this, &FileBrowserPanel::onFilterChanged);
    connect(_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FileBrowserPanel::onSortModeChanged);
    connect(_hiddenFilesAction, &QAction::toggled, this, [this](bool on) {
        _showHidden = on;
        refresh();
    });
    connect(_systemFilesAction, &QAction::toggled, this, [this](bool on) {
        _showSystem = on;
        refresh();
    });
    connect(_collapseAllAction, &QAction::triggered, this, &FileBrowserPanel::collapseAll);
    connect(_expandAllAction, &QAction::triggered, this, &FileBrowserPanel::expandAll);
    connect(_followAction, &QAction::toggled, this, &FileBrowserPanel::followCurrentDocument);
    connect(_newFileAction, &QAction::triggered, this, &FileBrowserPanel::newFileHere);
    connect(_newFolderAction, &QAction::triggered, this, &FileBrowserPanel::newFolderHere);

    // Initial directory
    QString initialDir = QDir::homePath();
    if (qApp->applicationDirPath().contains("/workspace")) {
        initialDir = qApp->applicationDirPath();
        int wsIdx = initialDir.indexOf("/workspace");
        if (wsIdx > 0) {
            initialDir = initialDir.left(wsIdx + 10);
        }
    }
    setRootDirectory(initialDir);

    // Load recent dirs from settings
    QSettings s;
    _recentDirs = s.value("FileBrowser/recentDirs").toStringList();
}

FileBrowserPanel::~FileBrowserPanel() {
    QSettings s;
    s.setValue("FileBrowser/recentDirs", _recentDirs);
}

// ---------------------------------------------------------------------------
// Toolbar setup
// ---------------------------------------------------------------------------
void FileBrowserPanel::setupToolbar() {
    _toolBar = new QToolBar(this);
    _toolBar->setMovable(false);
    _toolBar->setFloatable(false);
    _toolBar->setIconSize(QSize(16, 16));
    _toolBar->setContentsMargins(2, 0, 2, 0);

    // Refresh
    _refreshAction = new QAction(QIcon::fromTheme("view-refresh", QIcon(":/icons/refresh")),
                                  tr("Refresh"), _toolBar);
    _refreshAction->setToolTip(tr("Refresh (F5)"));
    _toolBar->addAction(_refreshAction);

    // Up
    _upAction = new QAction(QIcon::fromTheme("go-up", QIcon(":/icons/up")),
                             tr("Up"), _toolBar);
    _upAction->setToolTip(tr("Go to parent directory"));
    _toolBar->addAction(_upAction);

    // Home
    _homeAction = new QAction(QIcon::fromTheme("go-home", QIcon(":/icons/home")),
                               tr("Home"), _toolBar);
    _homeAction->setToolTip(tr("Go to home directory"));
    _toolBar->addAction(_homeAction);

    _toolBar->addSeparator();

    // New File
    _newFileAction = new QAction(QIcon::fromTheme("document-new", QIcon(":/icons/new")),
                                  tr("New File"), _toolBar);
    _newFileAction->setToolTip(tr("New file"));
    _toolBar->addAction(_newFileAction);

    // New Folder
    _newFolderAction = new QAction(QIcon::fromTheme("folder-new", QIcon(":/icons/folder")),
                                     tr("New Folder"), _toolBar);
    _newFolderAction->setToolTip(tr("New folder"));
    _toolBar->addAction(_newFolderAction);

    _toolBar->addSeparator();

    // Expand/Collapse
    _expandAllAction = new QAction(QIcon::fromTheme("zoom-in", QIcon(":/icons/expand")),
                                    tr("Expand All"), _toolBar);
    _expandAllAction->setToolTip(tr("Expand all folders"));
    _toolBar->addAction(_expandAllAction);

    _collapseAllAction = new QAction(QIcon::fromTheme("zoom-out", QIcon(":/icons/collapse")),
                                       tr("Collapse All"), _toolBar);
    _collapseAllAction->setToolTip(tr("Collapse all folders"));
    _toolBar->addAction(_collapseAllAction);

    _toolBar->addSeparator();

    // Show hidden files
    _hiddenFilesAction = new QAction(QIcon::fromTheme("show-hidden", QIcon(":/icons/hidden")),
                                      tr("Hidden Files"), _toolBar);
    _hiddenFilesAction->setCheckable(true);
    _hiddenFilesAction->setChecked(_showHidden);
    _hiddenFilesAction->setToolTip(tr("Show hidden files"));
    _toolBar->addAction(_hiddenFilesAction);

    // Follow active document
    _followAction = new QAction(QIcon::fromTheme("go-jump", QIcon(":/icons/follow")),
                                 tr("Follow Active Doc"), _toolBar);
    _followAction->setCheckable(true);
    _followAction->setChecked(false);
    _followAction->setToolTip(tr("Follow the active document's directory"));
    _toolBar->addAction(_followAction);
}

// ---------------------------------------------------------------------------
// Breadcrumb navigation
// ---------------------------------------------------------------------------
void FileBrowserPanel::setupBreadcrumbs() {
    _breadcrumbBar = new QWidget(this);
    QHBoxLayout* bl = new QHBoxLayout(_breadcrumbBar);
    bl->setContentsMargins(4, 2, 4, 2);
    bl->setSpacing(2);

    // Recent directories dropdown
    QToolButton* recentBtn = new QToolButton(_breadcrumbBar);
    recentBtn->setText(tr("Recent"));
    recentBtn->setPopupMode(QToolButton::InstantPopup);
    recentBtn->setMinimumWidth(60);
    QMenu* recentMenu = new QMenu(recentBtn);
    recentBtn->setMenu(recentMenu);
    connect(recentMenu, &QMenu::triggered, this, &FileBrowserPanel::onRecentDirTriggered);
    bl->addWidget(recentBtn);

    // Go-to-directory input
    _completerModel = new QStringListModel(this);
    _completer = new QCompleter(_completerModel, this);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    _completer->setFilterMode(Qt::MatchContains);

    QLineEdit* gotoEdit = new QLineEdit(_breadcrumbBar);
    gotoEdit->setCompleter(_completer);
    gotoEdit->setPlaceholderText(tr("Go to directory..."));
    gotoEdit->setMinimumWidth(120);
    gotoEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(gotoEdit, &QLineEdit::returnPressed, this, [this, gotoEdit]() {
        QString path = gotoEdit->text().trimmed();
        if (!path.isEmpty()) {
            QFileInfo info(path);
            if (info.isDir()) {
                setRootDirectory(info.absoluteFilePath());
            } else if (info.isFile()) {
                setRootDirectory(info.absolutePath());
            } else {
                // Try as relative path
                QDir dir(_model->rootPath());
                if (dir.cd(path)) {
                    setRootDirectory(dir.path());
                }
            }
            gotoEdit->clear();
        }
    });
    bl->addWidget(gotoEdit, 1);

    updateBreadcrumbs();
}

void FileBrowserPanel::setupCompleter() {
    // Build list of directories for completion
    QString rootPath = _model->rootPath();
    QDirIterator it(rootPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList dirs;
    int count = 0;
    while (it.hasNext() && count < 500) {
        dirs << QFileInfo(it.next()).absoluteFilePath();
        ++count;
    }
    if (_completerModel) {
        _completerModel->setStringList(dirs);
    }
}

void FileBrowserPanel::updateBreadcrumbs() {
    if (!_breadcrumbBar) return;
    QString currentPath = _model->filePath(_treeView->rootIndex());

    // Clear old actions
    for (QAction* a : _breadcrumbActions) {
        _breadcrumbBar->layout()->removeWidget(qobject_cast<QWidget*>(a->parent()));
        delete a;
    }
    _breadcrumbActions.clear();

    QHBoxLayout* bl = qobject_cast<QHBoxLayout*>(_breadcrumbBar->layout());
    if (!bl) return;

    // Remove all widgets except recent button and goto edit
    while (bl->count() > 2) {
        QLayoutItem* item = bl->takeAt(1);
        if (item->widget()) {
            if (qobject_cast<QLabel*>(item->widget()) ||
                qobject_cast<QToolButton*>(item->widget())) {
                delete item->widget();
            }
        }
        delete item;
    }

    // Build breadcrumb path
    QStringList parts;
    QString path = currentPath;
    while (!path.isEmpty() && path != "/") {
        parts.prepend(path);
        QDir d(path);
        if (!d.cdUp()) break;
        path = d.path();
    }

    // Add separator at beginning
    bl->insertWidget(1, new QLabel("/"), 0);

    for (int i = 0; i < parts.size(); ++i) {
        const QString& part = parts[i];
        QToolButton* btn = new QToolButton(_breadcrumbBar);
        QString label = QFileInfo(part).fileName();
        if (label.isEmpty()) label = part;  // root
        btn->setText(label);
        btn->setPopupMode(QToolButton::InstantPopup);
        QMenu* menu = new QMenu(btn);
        QAction* a = menu->addAction(tr("Open Folder as Project"));
        a->setData(part);
        connect(a, &QAction::triggered, this, [this, part]() {
            setCurrentProjectDirectory(part);
        });
        a = menu->addAction(tr("Copy Path"));
        a->setData(part);
        connect(a, &QAction::triggered, this, [this, part]() {
            QApplication::clipboard()->setText(part);
        });
        a = menu->addAction(tr("Open in Terminal"));
        a->setData(part);
        connect(a, &QAction::triggered, this, [this, part]() {
            openInFileManagerAt(part, true);
        });
        btn->setMenu(menu);
        bl->insertWidget(i + 2, btn, 0);
        if (i < parts.size() - 1) {
            bl->insertWidget(i + 3, new QLabel("/"), 0);
        }
    }
}

void FileBrowserPanel::addRecentDirectory(const QString& dir) {
    if (dir.isEmpty()) return;
    _recentDirs.removeAll(dir);
    _recentDirs.prepend(dir);
    while (_recentDirs.size() > MaxRecentDirs) {
        _recentDirs.removeLast();
    }

    // Rebuild recent menu
    QToolButton* recentBtn = _breadcrumbBar->findChild<QToolButton*>();
    if (recentBtn && recentBtn->menu()) {
        QMenu* m = recentBtn->menu();
        m->clear();
        for (const QString& d : _recentDirs) {
            QAction* a = m->addAction(QFileInfo(d).fileName() + "  [" + d + "]");
            a->setData(d);
        }
    }
}

void FileBrowserPanel::onRecentDirTriggered(QAction* action) {
    QString path = action->data().toString();
    if (!path.isEmpty()) {
        QFileInfo info(path);
        if (info.isDir()) {
            setRootDirectory(path);
        }
    }
}

// ---------------------------------------------------------------------------
// Root / Navigation
// ---------------------------------------------------------------------------
void FileBrowserPanel::setRootDirectory(const QString& dir) {
    QModelIndex idx = _model->index(dir);
    if (!idx.isValid()) return;

    _treeView->setRootIndex(idx);
    _model->setRootPath(dir);

    // Update file watcher
    QStringList watched = _watcher->directories();
    for (const QString& w : watched) {
        _watcher->removePath(w);
    }
    _watcher->addPath(dir);

    // Update up button state
    _upAction->setEnabled(dir != QDir::rootPath() && dir != QDir::homePath());

    // Update breadcrumbs
    updateBreadcrumbs();

    // Add to recent
    addRecentDirectory(dir);

    // Update completer
    setupCompleter();

    // Sync follow state
    syncFollowState();

    emit directoryChanged(dir);
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
    if (currentPath.isEmpty()) currentPath = QDir::homePath();

    // Rebuild model with updated filters
    QDir::Filters f = QDir::AllEntries | QDir::NoDotAndDotDot;
    if (_showHidden) f |= QDir::Hidden;
    if (_showSystem) f |= QDir::System;
    else f |= QDir::Drives;
    _model->setFilter(f);
    _model->setRootPath("");
    setRootDirectory(currentPath);

    // Refresh completer
    setupCompleter();
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

void FileBrowserPanel::followActiveDocument(const QString& filePath) {
    if (filePath.isEmpty()) return;
    QFileInfo info(filePath);
    QString dirPath = info.absolutePath();
    if (dirPath != _model->rootPath()) {
        setRootDirectory(dirPath);
        _activeFilePath = filePath;
        // Expand tree to show the file
        QModelIndex fileIdx = _model->index(filePath);
        if (fileIdx.isValid()) {
            // Expand all parent directories
            QModelIndex parent = fileIdx.parent();
            while (parent.isValid() && parent != _treeView->rootIndex()) {
                _treeView->expand(parent);
                parent = parent.parent();
            }
            _treeView->scrollTo(fileIdx);
            _treeView->setCurrentIndex(fileIdx);
        }
    }
    _lastFollowedPath = filePath;
}

void FileBrowserPanel::syncFollowState() {
    if (_followAction && _followAction->isChecked() && !_lastFollowedPath.isEmpty()) {
        followActiveDocument(_lastFollowedPath);
    }
}

void FileBrowserPanel::followCurrentDocument() {
    if (_followAction && _followAction->isChecked() && !_activeFilePath.isEmpty()) {
        followActiveDocument(_activeFilePath);
    }
}

// ---------------------------------------------------------------------------
// Sorting / Filtering
// ---------------------------------------------------------------------------
void FileBrowserPanel::onSortModeChanged(int index) {
    _sortMode = index;
    applySortMode();
}

void FileBrowserPanel::applySortMode() {
    if (!_treeView) return;
    Qt::SortOrder order = Qt::AscendingOrder;
    int column = 0;
    switch (_sortMode) {
        case 0: column = 0; order = Qt::AscendingOrder; break;   // Name A-Z
        case 1: column = 3; order = Qt::DescendingOrder; break;   // Date newest
        case 2: column = 1; order = Qt::DescendingOrder; break;   // Size largest
        case 3: column = 2; order = Qt::AscendingOrder; break;    // Type
    }
    _treeView->sortByColumn(column, order);
}

void FileBrowserPanel::onFilterChanged(const QString& text) {
    if (text.isEmpty()) {
        _model->setNameFilters(_nameFilters);
    } else {
        // Parse filter: "*.ext1;*.ext2" or just "*.ext"
        QStringList filters;
        QStringList parts = text.split(';', Qt::SkipEmptyParts);
        for (const QString& part : parts) {
            QString f = part.trimmed();
            if (!f.startsWith("*.")) {
                if (f.contains('.')) {
                    f = "*" + f;
                } else {
                    f = "*." + f;
                }
            }
            filters << f;
        }
        if (!filters.isEmpty()) {
            _model->setNameFilters(filters);
        }
    }
}

void FileBrowserPanel::expandAll() {
    _treeView->expandAll();
}

void FileBrowserPanel::collapseAll() {
    _treeView->collapseAll();
}

// ---------------------------------------------------------------------------
// Double-click
// ---------------------------------------------------------------------------
void FileBrowserPanel::onDoubleClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);

    if (info.isFile()) {
        _activeFilePath = path;
        emit fileDoubleClicked(path);
    } else if (info.isDir()) {
        setRootDirectory(path);
    }
}

// ---------------------------------------------------------------------------
// Context Menu
// ---------------------------------------------------------------------------
void FileBrowserPanel::onContextMenu(const QPoint& pos) {
    QModelIndex index = _treeView->indexAt(pos);

    QMenu menu(this);

    if (!index.isValid()) {
        // Empty area: navigation + creation
        QAction* a = menu.addAction(tr("Refresh"));
        connect(a, &QAction::triggered, this, &FileBrowserPanel::refresh);
        a = menu.addAction(tr("New File Here"));
        connect(a, &QAction::triggered, this, &FileBrowserPanel::newFileHere);
        a = menu.addAction(tr("New Folder Here"));
        connect(a, &QAction::triggered, this, &FileBrowserPanel::newFolderHere);
        menu.addSeparator();
        a = menu.addAction(tr("Go to Home Directory"));
        connect(a, &QAction::triggered, this, &FileBrowserPanel::navigateHome);
        menu.exec(_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);
    bool isFile = info.isFile();
    bool isDir = info.isDir();

    // Open
    QAction* openAction = menu.addAction(tr("Open"));
    connect(openAction, &QAction::triggered, this, &FileBrowserPanel::openSelectedFile);

    if (isFile) {
        // Open as Read-Only
        QAction* roAction = menu.addAction(tr("Open as Read-Only"));
        connect(roAction, &QAction::triggered, this, &FileBrowserPanel::openReadOnly);

        // Open in New Instance
        QAction* niAction = menu.addAction(tr("Open in New Instance"));
        connect(niAction, &QAction::triggered, this, &FileBrowserPanel::openInNewInstance);
    }

    menu.addSeparator();

    // Add to Workspace
    if (isDir) {
        QAction* wsAction = menu.addAction(tr("Add to Workspace"));
        connect(wsAction, &QAction::triggered, this, &FileBrowserPanel::addToWorkspace);
    }

    menu.addSeparator();

    // Copy Path
    QAction* copyPathAction = menu.addAction(tr("Copy Path"));
    connect(copyPathAction, &QAction::triggered, this, &FileBrowserPanel::copyPathToClipboard);

    // Copy Filename
    QAction* copyNameAction = menu.addAction(tr("Copy Filename"));
    connect(copyNameAction, &QAction::triggered, [this, path]() {
        QApplication::clipboard()->setText(QFileInfo(path).fileName());
    });

    menu.addSeparator();

    // Reveal in File Manager
    QAction* revealAction = menu.addAction(tr("Reveal in File Manager"));
    connect(revealAction, &QAction::triggered, this, &FileBrowserPanel::revealInFileManager);

    // Open in Terminal
    QAction* termAction = menu.addAction(tr("Open in Terminal"));
    connect(termAction, &QAction::triggered, this, &FileBrowserPanel::openInTerminal);

    menu.addSeparator();

    // Rename
    QAction* renameAction = menu.addAction(tr("Rename"));
    connect(renameAction, &QAction::triggered, this, &FileBrowserPanel::renameSelected);

    // Delete
    QAction* deleteAction = menu.addAction(tr("Delete"));
    connect(deleteAction, &QAction::triggered, this, &FileBrowserPanel::deleteSelected);

    if (isDir) {
        menu.addSeparator();
        QAction* openHereAction = menu.addAction(tr("Open Folder as Project"));
        connect(openHereAction, &QAction::triggered, [this, path]() {
            setCurrentProjectDirectory(path);
        });
    }

    menu.exec(_treeView->viewport()->mapToGlobal(pos));
}

// ---------------------------------------------------------------------------
// File operations
// ---------------------------------------------------------------------------
void FileBrowserPanel::openSelectedFile() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    if (_model->fileInfo(index).isFile()) {
        _activeFilePath = path;
        emit fileDoubleClicked(path);
    }
}

void FileBrowserPanel::openReadOnly() {
    // Open as read-only (emit signal with read-only flag)
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    // For now, emit same signal — the Application can check readonly flag
    emit fileDoubleClicked(path + "?readonly=1");
}

void FileBrowserPanel::openInNewInstance() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    // Launch a new instance with this file
    QString appPath = QApplication::applicationFilePath();
    QProcess::startDetached(appPath, {path});
}

void FileBrowserPanel::copyPathToClipboard() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QApplication::clipboard()->setText(_model->filePath(index));
}

QStringList FileBrowserPanel::getSelectedPaths() const {
    QStringList paths;
    QModelIndexList selected = _treeView->selectionModel()->selectedIndexes();
    for (const QModelIndex& idx : selected) {
        if (idx.column() == 0) {
            paths << _model->filePath(idx);
        }
    }
    return paths;
}

void FileBrowserPanel::revealInFileManager() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);
    QString dirPath = info.isFile() ? info.absolutePath() : path;
    openInFileManagerAt(dirPath, false);
}

void FileBrowserPanel::openInTerminal() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    QFileInfo info = _model->fileInfo(index);
    QString dirPath = info.isFile() ? info.absolutePath() : path;
    openInFileManagerAt(dirPath, true);
}

void FileBrowserPanel::openInFileManagerAt(const QString& path, bool terminal) {
#ifdef Q_OS_WIN
    if (terminal) {
        QProcess::startDetached("cmd.exe", {"/c", "start", "cmd"}, path);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
#elif defined(Q_OS_MAC)
    if (terminal) {
        QProcess::startDetached("open", {"-a", "Terminal", path});
    } else {
        QProcess::startDetached("open", {"-R", path});
    }
#else
    if (terminal) {
        // Try common terminal emulators
        QString term;
        QStringList terms = {"konsole", "gnome-terminal", "xfce4-terminal", "xterm", "urxvt", "alacritty"};
        for (const QString& t : terms) {
            if (QProcess::startDetached(t, {"--workdir", path})) {
                term = t;
                break;
            }
        }
        if (term.isEmpty()) {
            QProcess::startDetached("xdg-terminal", {path});
        }
    } else {
        QProcess proc;
        proc.start("xdg-mime", {"query", "inode/directory"});
        proc.waitForFinished();
        QString mime = proc.readAllStandardOutput().trimmed();
        if (mime.contains("nautilus")) {
            QProcess::startDetached("nautilus", {path});
        } else if (mime.contains("dolphin")) {
            QProcess::startDetached("dolphin", {path});
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }
#endif
}

void FileBrowserPanel::renameSelected() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    QFileInfo info(_model->fileInfo(index));
    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename"),
                                            tr("New name:"),
                                            QLineEdit::Normal,
                                            info.fileName(),
                                            &ok);
    if (ok && !newName.isEmpty() && newName != info.fileName()) {
        QString newPath = info.absolutePath() + "/" + newName;
        QFile file(path);
        if (file.rename(newPath)) {
            refresh();
        } else {
            QMessageBox::warning(this, tr("Rename Failed"),
                                 tr("Could not rename %1").arg(path));
        }
    }
}

void FileBrowserPanel::deleteSelected() {
    QStringList paths = getSelectedPaths();
    if (paths.isEmpty()) return;

    QString msg = paths.size() == 1
        ? tr("Delete '%1'?\nThis cannot be undone.").arg(QFileInfo(paths[0]).fileName())
        : tr("Delete %1 items?\nThis cannot be undone.").arg(paths.size());

    QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Confirm Delete"), msg);
    if (btn != QMessageBox::Yes) return;

    QStringList failed;
    for (const QString& path : paths) {
        QFileInfo info(path);
        bool success = false;
        if (info.isDir()) {
            success = QDir(info.absolutePath()).rmdir(info.fileName());
            if (!success) {
                // Try recursive delete for non-empty dirs
                success = QDir(path).removeRecursively();
            }
        } else {
            success = QFile::remove(path);
        }
        if (!success) failed << path;
    }

    if (!failed.isEmpty()) {
        QMessageBox::warning(this, tr("Delete Failed"),
                             tr("Could not delete:\n") + failed.join("\n"));
    }

    refresh();
}

void FileBrowserPanel::newFileHere() {
    QString dirPath = _model->rootPath();
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New File"),
                                          tr("File name:"),
                                          QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QString filePath = dirPath + "/" + name;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            refresh();
            // Navigate to and select the new file
            QModelIndex idx = _model->index(filePath);
            if (idx.isValid()) {
                _treeView->setCurrentIndex(idx);
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not create file."));
        }
    }
}

void FileBrowserPanel::newFolderHere() {
    QString dirPath = _model->rootPath();
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New Folder"),
                                          tr("Folder name:"),
                                          QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QString folderPath = dirPath + "/" + name;
        if (QDir().mkdir(folderPath)) {
            refresh();
            QModelIndex idx = _model->index(folderPath);
            if (idx.isValid()) {
                _treeView->setCurrentIndex(idx);
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not create folder."));
        }
    }
}

void FileBrowserPanel::addToWorkspace() {
    QModelIndex index = _treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = _model->filePath(index);
    // Emit signal that Application can use to add folder to workspace
    emit directoryChanged(path);
}

// ---------------------------------------------------------------------------
// File system watcher
// ---------------------------------------------------------------------------
void FileBrowserPanel::onDirectoryChanged(const QString& path) {
    Q_UNUSED(path);
    // Debounce: schedule refresh
    QTimer::singleShot(100, this, [this]() {
        refresh();
    });
}

void FileBrowserPanel::onFileRenamed(const QString& /*path*/) {
    refresh();
}
