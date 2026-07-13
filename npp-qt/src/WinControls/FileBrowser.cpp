// Semantic lift: FileBrowser — Win32 TreeView file browser → Qt6 QTreeWidget
// Original: notepad-plus-plus-translation/PowerEditor/src/WinControls/FileBrowser/fileBrowser.cpp (1785 lines)
// Target: npp-qt/src/WinControls/FileBrowser.cpp
// Win32 → Qt6: HWND/TreeView_* → QTreeWidget, HANDLE/FindFirstChangeNotification → QFileSystemWatcher,
//              HMENU → QMenu, HTREEITEM → QTreeWidgetItem*, SHGetFileInfo → QFileIconProvider

#include "FileBrowser.h"
#include "WinControls/DockingWnd/DockingManager.h"
#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QFileIconProvider>
#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QDirIterator>
#include <QTimer>
#include <QPainter>

#include "Common.h"
#include "Parameters.h"
#include "NppDarkMode.h"
#include "localization.h"

// ── FileBrowser ──────────────────────────────────────────────────────────────

FileBrowser::~FileBrowser()
{
    // Stop and delete all folder watchers
    for (auto* watcher : qAsConst(_folderWatchers)) {
        watcher->blockSignals(true);
        watcher->deleteLater();
    }
    _folderWatchers.clear();

    // Delete sorting data
    qDeleteAll(_sortingDataArray);
    _sortingDataArray.clear();
}

void FileBrowser::init()
{
    // Initialize the tree widget
    _treeWidget.setHeaderHidden(false);
    _treeWidget.setAlternatingRowColors(false);
    _treeWidget.setSelectionMode(QAbstractItemView::SingleSelection);
    _treeWidget.setExpandsOnDoubleClick(true);
    _treeWidget.setContextMenuPolicy(Qt::CustomContextMenu);

    connect(&_treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &FileBrowser::showContextMenu);
    connect(&_treeWidget, &QTreeWidget::itemDoubleClicked,
            this, &FileBrowser::onItemDoubleClicked);
    connect(&_treeWidget, &QTreeWidget::itemExpanded,
            this, &FileBrowser::onItemExpanded);
    connect(&_treeWidget, &QTreeWidget::itemCollapsed,
            this, &FileBrowser::onItemCollapsed);
    connect(&_treeWidget, &QTreeWidget::itemClicked,
            this, &FileBrowser::onItemClicked);

    // Initialize the toolbar menu
    initToolbarMenu();

    // Apply dark mode
    NppDarkMode::instance().setTreeViewStyle(&_treeWidget, NppDarkMode::NppDarkModeTreeViewStyle::treeviewDark);
}

void FileBrowser::setParent(QWidget* parent)
{
    _parent = parent;
}

void FileBrowser::setBackgroundColor(QRgb bgColour)
{
    QPalette pal = _treeWidget.palette();
    pal.setColor(QPalette::Base, QColor::fromRgb(bgColour));
    _treeWidget.setPalette(pal);
}

void FileBrowser::setForegroundColor(QRgb fgColour)
{
    QPalette pal = _treeWidget.palette();
    pal.setColor(QPalette::Text, QColor::fromRgb(fgColour));
    _treeWidget.setPalette(pal);
}

QString FileBrowser::getNodePath(QTreeWidgetItem* node) const
{
    if (!node)
        return QString();

    QString path;
    QTreeWidgetItem* item = node;
    while (item) {
        QString name = item->text(0);
        path = name + QDir::separator() + path;
        item = item->parent();
    }

    // Get root path stored in item data
    if (node && node->data(0, Qt::UserRole).canConvert<QString>()) {
        QString rootPath = node->data(0, Qt::UserRole).toString();
        if (!rootPath.isEmpty() && rootPath != QStringLiteral("/")) {
            return rootPath + QDir::separator() + path;
        }
    }

    return path;
}

QString FileBrowser::getNodeName(QTreeWidgetItem* node) const
{
    return node ? node->text(0) : QString();
}

void FileBrowser::addRootFolder(const QString& rootFolderPath)
{
    QDir dir(rootFolderPath);
    if (!dir.exists())
        return;

    // Check if root already exists
    for (int i = 0; i < _treeWidget.topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _treeWidget.topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == rootFolderPath)
            return; // Already added
    }

    // Create root item
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, dir.dirName());
    rootItem->setData(0, Qt::UserRole, rootFolderPath);
    rootItem->setData(0, Qt::UserRole + 1, QStringLiteral("root"));
    rootItem->setIcon(0, folderIcon());
    rootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

    _treeWidget.addTopLevelItem(rootItem);
    rootItem->setExpanded(false);

    // Start file system watcher for this root
    startWatcher(rootFolderPath, rootItem);

    // Load directory structure
    loadDir(rootFolderPath, rootItem, rootFolderPath);
}

QTreeWidgetItem* FileBrowser::getRootFromFullPath(const QString& rootPath) const
{
    for (int i = 0; i < _treeWidget.topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _treeWidget.topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == rootPath)
            return item;
    }
    return nullptr;
}

void FileBrowser::loadDir(const QString& dirPath, QTreeWidgetItem* parentItem, const QString& rootPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
        return;

    // Get list of folders and files
    QFileInfoList entries = dir.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
        QDir::Name | QDir::DirsFirst
    );

    for (const QFileInfo& info : entries) {
        if (info.isDir()) {
            QTreeWidgetItem* folderItem = new QTreeWidgetItem();
            folderItem->setText(0, info.fileName());
            folderItem->setData(0, Qt::UserRole, rootPath);
            folderItem->setData(0, Qt::UserRole + 1, QStringLiteral("folder"));
            folderItem->setIcon(0, folderIcon());
            folderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            parentItem->addChild(folderItem);
        } else {
            QTreeWidgetItem* fileItem = new QTreeWidgetItem();
            fileItem->setText(0, info.fileName());
            fileItem->setData(0, Qt::UserRole, rootPath);
            fileItem->setData(0, Qt::UserRole + 1, QStringLiteral("file"));
            fileItem->setIcon(0, fileIcon(info));
            parentItem->addChild(fileItem);
        }
    }
}

void FileBrowser::onItemExpanded(QTreeWidgetItem* item)
{
    if (!item)
        return;

    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    if (nodeType != QStringLiteral("folder") && nodeType != QStringLiteral("root"))
        return;

    // Lazy load: if no children loaded yet (only the expand indicator), load now
    if (item->childCount() == 0) {
        QString fullPath = getNodePath(item);
        QString rootPath = item->data(0, Qt::UserRole).toString();
        loadDir(fullPath, item, rootPath);
    }
}

void FileBrowser::onItemCollapsed(QTreeWidgetItem*)
{
    // Nothing special needed on collapse
}

void FileBrowser::onItemClicked(QTreeWidgetItem* item, int)
{
    if (!item)
        return;

    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    if (nodeType == QStringLiteral("file")) {
        // Open the file
        QString fullPath = getNodePath(item);
        openFile(fullPath);
    }
}

void FileBrowser::onItemDoubleClicked(QTreeWidgetItem* item, int)
{
    if (!item)
        return;

    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    if (nodeType == QStringLiteral("file")) {
        QString fullPath = getNodePath(item);
        openFile(fullPath);
    }
}

void FileBrowser::openFile(const QString& filePath)
{
    // Emit signal to open in Notepad++
    // This connects to the main window's file open handler
    emit fileOpened(filePath);
}

void FileBrowser::showContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = _treeWidget.itemAt(pos);
    if (!item)
        return;

    QMenu menu;

    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    QString fullPath = getNodePath(item);
    QString rootPath = item->data(0, Qt::UserRole).toString();

    if (nodeType == QStringLiteral("root") || nodeType == QStringLiteral("folder")) {
        // Folder context menu
        QAction* openInExplorer = menu.addAction(tr("Open in Explorer"));
        QAction* cmdHere = menu.addAction(tr("CMD here"));
        QAction* findInFiles = menu.addAction(tr("Find in Files..."));
        menu.addSeparator();
        QAction* copyPath = menu.addAction(tr("Copy path"));
        QAction* copyName = menu.addAction(tr("Copy file name"));
        menu.addSeparator();
        QAction* remove = menu.addAction(tr("Remove"));
        if (nodeType == QStringLiteral("root"))
            remove->setText(tr("Remove root"));

        connect(openInExplorer, &QAction::triggered, this, [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
        });
        connect(cmdHere, &QAction::triggered, this, [=]() {
            QProcess::startDetached(QStringLiteral("xterm"), {QStringLiteral("-e"), QStringLiteral("cd"), fullPath});
        });
        connect(findInFiles, &QAction::triggered, this, [=]() {
            emit findInFolder(fullPath);
        });
        connect(copyPath, &QAction::triggered, this, [=]() {
            QApplication::clipboard()->setText(fullPath);
        });
        connect(copyName, &QAction::triggered, this, [=]() {
            QApplication::clipboard()->setText(item->text(0));
        });
        connect(remove, &QAction::triggered, this, [=]() {
            deleteItem(item);
        });
    } else {
        // File context menu
        QAction* open = menu.addAction(tr("Open"));
        QAction* openInExplorer = menu.addAction(tr("Open in Explorer"));
        QAction* findInFiles = menu.addAction(tr("Find in Files..."));
        menu.addSeparator();
        QAction* copyPath = menu.addAction(tr("Copy path"));
        QAction* copyName = menu.addAction(tr("Copy file name"));
        menu.addSeparator();
        QAction* rename = menu.addAction(tr("Rename"));
        QAction* deleteFile = menu.addAction(tr("Delete"));

        connect(open, &QAction::triggered, this, [=]() {
            openFile(fullPath);
        });
        connect(openInExplorer, &QAction::triggered, this, [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(fullPath).absolutePath()));
        });
        connect(findInFiles, &QAction::triggered, this, [=]() {
            emit findInFolder(QFileInfo(fullPath).absolutePath());
        });
        connect(copyPath, &QAction::triggered, this, [=]() {
            QApplication::clipboard()->setText(fullPath);
        });
        connect(copyName, &QAction::triggered, this, [=]() {
            QApplication::clipboard()->setText(item->text(0));
        });
        connect(rename, &QAction::triggered, this, [=]() {
            bool ok;
            QString newName = QInputDialog::getText(this, tr("Rename"),
                                                    tr("New name:"), QLineEdit::Normal,
                                                    item->text(0), &ok);
            if (ok && !newName.isEmpty()) {
                renameItem(item, newName);
            }
        });
        connect(deleteFile, &QAction::triggered, this, [=]() {
            if (QMessageBox::question(this, tr("Delete"),
                                     tr("Delete '%1'?").arg(item->text(0))) == QMessageBox::Yes) {
                deleteItem(item);
            }
        });
    }

    menu.exec(_treeWidget.mapToGlobal(pos));
}

void FileBrowser::initToolbarMenu()
{
    // Toolbar menu items are handled via the toolbar button signals
}

void FileBrowser::deleteItem(QTreeWidgetItem* item)
{
    if (!item)
        return;

    QString nodeType = item->data(0, Qt::UserRole + 1).toString();
    QString fullPath = getNodePath(item);

    if (nodeType == QStringLiteral("root")) {
        // Remove root folder watcher
        QString rootPath = item->data(0, Qt::UserRole).toString();
        if (_folderWatchers.contains(rootPath)) {
            auto* watcher = _folderWatchers.take(rootPath);
            watcher->deleteLater();
        }

        // Remove from tree
        int idx = _treeWidget.indexOfTopLevelItem(item);
        if (idx >= 0)
            _treeWidget.takeTopLevelItem(idx);
    } else {
        // Remove child item
        if (item->parent())
            item->parent()->removeChild(item);
        else
            delete item;
    }

    // Optionally delete file/folder from disk
    // QFile::remove(fullPath); // Uncomment to delete from disk
}

void FileBrowser::renameItem(QTreeWidgetItem* item, const QString& newName)
{
    if (!item)
        return;

    QString oldPath = getNodePath(item);
    QString newPath = QFileInfo(oldPath).absolutePath() + QDir::separator() + newName;

    QFile file(oldPath);
    if (file.rename(newPath)) {
        item->setText(0, newName);
    }
}

QIcon FileBrowser::folderIcon()
{
    static QIcon icon(QPixmap::fromImage(QImage(
        ":/icons/folder.svg", "SVG").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    return icon;
}

QIcon FileBrowser::fileIcon(const QFileInfo& info)
{
    static QFileIconProvider provider;
    return provider.icon(info);
}

void FileBrowser::startWatcher(const QString& dirPath, QTreeWidgetItem* rootItem)
{
    auto* watcher = new QFileSystemWatcher(this);
    watcher->addPath(dirPath);

    _folderWatchers.insert(dirPath, watcher);

    connect(watcher, &QFileSystemWatcher::directoryChanged,
            this, [this, dirPath, rootItem]() {
        refreshFolder(dirPath, rootItem);
    });
}

void FileBrowser::refreshFolder(const QString& dirPath, QTreeWidgetItem* item)
{
    if (!item)
        return;

    // Save expanded state
    QSet<QString> expandedPaths;
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        if (child->isExpanded()) {
            expandedPaths.insert(getNodePath(child));
        }
    }

    // Remove all children
    while (item->childCount() > 0) {
        item->removeChild(item->child(0));
    }

    // Reload
    QString rootPath = item->data(0, Qt::UserRole).toString();
    loadDir(dirPath, item, rootPath);

    // Restore expanded state
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        if (expandedPaths.contains(getNodePath(child))) {
            child->setExpanded(true);
        }
    }
}

QStringList FileBrowser::getRoots() const
{
    QStringList roots;
    for (int i = 0; i < _treeWidget.topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _treeWidget.topLevelItem(i);
        if (item)
            roots.append(item->data(0, Qt::UserRole).toString());
    }
    return roots;
}

QString FileBrowser::getSelectedItemPath() const
{
    QTreeWidgetItem* item = _treeWidget.currentItem();
    if (!item)
        return QString();
    return getNodePath(item);
}

bool FileBrowser::selectItemFromPath(const QString& itemPath)
{
    // Find the item matching itemPath
    QTreeWidgetItemIterator it(&_treeWidget);
    while (*it) {
        QTreeWidgetItem* item = *it;
        if (getNodePath(item) == itemPath) {
            _treeWidget.setCurrentItem(item);
            return true;
        }
        ++it;
    }
    return false;
}

bool FileBrowser::selectCurrentEditingFile()
{
    // Get current file from NppCore/Parameters and select it
    QString currentFile = Parameters::getInstance().getCurrentFilePath();
    return selectItemFromPath(currentFile);
}

void FileBrowser::deleteAllRoots()
{
    // Stop all watchers
    for (auto* watcher : qAsConst(_folderWatchers)) {
        watcher->blockSignals(true);
        watcher->deleteLater();
    }
    _folderWatchers.clear();

    // Remove all items
    _treeWidget.clear();
}

void FileBrowser::foldAllFolders()
{
    for (int i = 0; i < _treeWidget.topLevelItemCount(); ++i) {
        collapseItemRecursively(_treeWidget.topLevelItem(i));
    }
}

void FileBrowser::unfoldAllFolders()
{
    for (int i = 0; i < _treeWidget.topLevelItemCount(); ++i) {
        expandItemRecursively(_treeWidget.topLevelItem(i));
    }
}

void FileBrowser::collapseItemRecursively(QTreeWidgetItem* item)
{
    if (!item)
        return;
    item->setExpanded(false);
    for (int i = 0; i < item->childCount(); ++i) {
        collapseItemRecursively(item->child(i));
    }
}

void FileBrowser::expandItemRecursively(QTreeWidgetItem* item)
{
    if (!item)
        return;
    item->setExpanded(true);
    for (int i = 0; i < item->childCount(); ++i) {
        expandItemRecursively(item->child(i));
    }
}
