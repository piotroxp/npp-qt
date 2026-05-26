// fileBrowser.cpp — Qt6 translation
#include "fileBrowser.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QFileDialog>

FileBrowser::FileBrowser(QWidget* parent)
    : QDockWidget("Folder as Workspace", parent)
{
    setObjectName("FileBrowser");

    QWidget* content = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create toolbar
    QToolBar* toolbar = new QToolBar(content);
    toolbar->addAction("Add Folder", this, &FileBrowser::addRootFolder);
    toolbar->addAction("Expand All", this, [this] {
        if (_treeView)
            _treeView->expandAll();
    });
    toolbar->addAction("Collapse All", this, [this] {
        if (_treeView)
            _treeView->collapseAll();
    });
    layout->addWidget(toolbar);

    // Create tree view
    _model = new QFileSystemModel(this);
    _model->setRootPath(QString()); // Start from root

    _treeView = new QTreeView(content);
    _treeView->setModel(_model);
    _treeView->setHeaderHidden(true);
    _treeView->setAnimated(true);
    _treeView->setIndentation(20);
    _treeView->setSortingEnabled(true);
    _treeView->sortByColumn(0, Qt::AscendingOrder);
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_treeView, &QTreeView::activated, this, &FileBrowser::onActivated);
    connect(_treeView, &QTreeView::customContextMenuRequested, this, &FileBrowser::onCustomContextMenu);

    layout->addWidget(_treeView);
    setWidget(content);

    // File watcher
    _watcher = new QFileSystemWatcher(this);
    connect(_watcher, &QFileSystemWatcher::directoryChanged, this, &FileBrowser::onDirectoryChanged);
}

FileBrowser::~FileBrowser() = default;

void FileBrowser::addRootFolder(const QString& path)
{
    if (path.isEmpty())
        return;

    _rootPaths.append(path);
    QModelIndex index = _model->setRootPath(path);
    _treeView->setRootIndex(index);
    _watcher->addPath(path);
}

QString FileBrowser::getSelectedItemPath() const
{
    QModelIndex idx = _treeView->currentIndex();
    if (idx.isValid())
        return _model->filePath(idx);
    return QString();
}

bool FileBrowser::selectItemFromPath(const QString& path)
{
    QModelIndex index = _model->index(path);
    if (index.isValid())
    {
        _treeView->setCurrentIndex(index);
        return true;
    }
    return false;
}

void FileBrowser::deleteAllRoots()
{
    _rootPaths.clear();
    _model->setRootPath(QString());
    _treeView->reset();
}

void FileBrowser::startWatching(const QString& path)
{
    if (!_watcher->files().contains(path))
        _watcher->addPath(path);
}

void FileBrowser::stopWatching(const QString& path)
{
    if (_watcher->files().contains(path))
        _watcher->removePath(path);
}

void FileBrowser::onActivated(const QModelIndex& index)
{
    if (_model->isDir(index))
    {
        // Expand/collapse directory
        if (_treeView->isExpanded(index))
            _treeView->collapse(index);
        else
            _treeView->expand(index);
    }
    else
    {
        emit fileActivated(_model->filePath(index));
    }
}

void FileBrowser::onCustomContextMenu(const QPoint& pos)
{
    QModelIndex index = _treeView->indexAt(pos);
    QString path = index.isValid() ? _model->filePath(index) : QString();
    emit folderContextMenuRequested(path, QCursor::pos());
}

void FileBrowser::onDirectoryChanged(const QString& path)
{
    Q_UNUSED(path);
    // Refresh tree
    _treeView->update();
}
