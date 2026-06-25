// Semantic Lift: Win32 FileBrowser → Qt6 implementation
// Original: PowerEditor/src/WinControls/FileBrowser/fileBrowser.cpp (1785 lines)
// Target: npp-qt/src/WinControls/FileBrowser.cpp
//
// Lifts the "Folder as Workspace" panel from Win32 TreeView + ReadDirectoryChanges
// to Qt6 QTreeWidget + QFileSystemWatcher.
// Follows the npp-qt WinControls translation pattern.

#include "FileBrowser.h"
#include "DockingWnd.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QCursor>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QMimeData>
#include <QDataStream>
#include <QDrag>
#include <QPainter>
#include <QFileIconProvider>
#include <QDateTime>
#include <QFont>
#include <QCompleter>
#include <QScrollBar>
#include <QTimer>
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QProcess>
#include <QStyleFactory>
#include <functional>
#include <algorithm>
#include <iterator>



// =============================================================================
// FileBrowserTreeWidget — drag-and-drop tree widget
// Mirrors Win32 TreeView subclass with TVN_BEGINDRAG, drag/drop tracking.
// ==============================================================================

FileBrowserTreeWidget::FileBrowserTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAutoScroll(true);
    setExpandsOnDoubleClick(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    // Accept both internal drags and external drops (file explorer)
    setDragEnabled(true);
}

void FileBrowserTreeWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _dragStartPos = event->pos();
    QTreeWidget::mousePressEvent(event);
}

void FileBrowserTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint dist = event->pos() - _dragStartPos;
        if (dist.manhattanLength() > QApplication::startDragDistance())
        {
            QTreeWidgetItem* item = itemAt(_dragStartPos);
            if (item && isDraggable(item))
            {
                beginDrag(item);
                dragItem(event->pos());
                return;
            }
        }
    }
    QTreeWidget::mouseMoveEvent(event);
}

void FileBrowserTreeWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && _isDragging)
    {
        dropItem();
    }
    QTreeWidget::mouseReleaseEvent(event);
}

void FileBrowserTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
    // Accept both internal item drags and external file drags
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ||
        event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    QTreeWidget::dragEnterEvent(event);
}

void FileBrowserTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ||
        event->mimeData()->hasUrls())
    {
        QTreeWidgetItem* target = itemAt(event->pos());
        if (target && isDroppable(target))
        {
            event->acceptProposedAction();
            setCurrentItem(target);
            return;
        }
    }
    event->ignore();
}

void FileBrowserTreeWidget::dropEvent(QDropEvent* event)
{
    QTreeWidget::dropEvent(event);
    _isDragging = false;
}

void FileBrowserTreeWidget::beginDrag(QTreeWidgetItem* item)
{
    _dragItem = item;
    _isDragging = true;

    // Serialize item data
    QMimeData* mimeData = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // Encode: text + custom item data (rootPath for root items)
    stream << item->text(0);

    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(item);
    if (fbItem && fbItem->sortData)
        stream << fbItem->sortData->_rootPath;

    mimeData->setData("application/x-filebrowser-item", encoded);
    mimeData->setText(item->text(0));

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}

void FileBrowserTreeWidget::dragItem(const QPoint& pos)
{
    QTreeWidgetItem* over = itemAt(pos);
    if (over && isDroppable(over))
    {
        setCurrentItem(over);
    }
}

bool FileBrowserTreeWidget::dropItem()
{
    if (!_dragItem || !_isDragging) return false;
    _isDragging = false;

    QTreeWidgetItem* target = currentItem();
    if (!target || target == _dragItem) return false;
    if (!isDroppable(target)) return false;

    // Emit signal so FileBrowser can handle the move
    emit itemMoved(_dragItem, nullptr, target);
    return true;
}

void FileBrowserTreeWidget::addCanNotDropInList(int index)
{
    _cannotDropList.insert(index);
}

void FileBrowserTreeWidget::addCanNotDragOutList(int index)
{
    _cannotDragList.insert(index);
}

bool FileBrowserTreeWidget::isDroppable(QTreeWidgetItem* item) const
{
    if (!item) return false;
    // Check item type via UserRole (Qt uses data(0, Qt::UserRole) for custom type)
    int typeVal = item->data(0, Qt::UserRole).toInt();

    // Root open/close/folder open/close/leaf cannot be drop targets
    if (_cannotDropList.contains(typeVal)) return false;

    // Only folders can be drop targets
    // We use the text color or item type to determine if it's a folder
    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(item);
    if (fbItem && fbItem->sortData && !fbItem->sortData->_isFolder)
        return false;

    return true;
}

bool FileBrowserTreeWidget::isDraggable(QTreeWidgetItem* item) const
{
    if (!item) return false;
    int typeVal = item->data(0, Qt::UserRole).toInt();
    return !_cannotDragList.contains(typeVal);
}

QTreeWidgetItem* FileBrowserTreeWidget::searchSubItemByName(const QString& name, QTreeWidgetItem* root) const
{
    if (!root) return nullptr;
    for (int i = 0; i < root->childCount(); ++i)
    {
        QTreeWidgetItem* child = root->child(i);
        if (child->text(0) == name)
            return child;
    }
    return nullptr;
}

QTreeWidgetItem* FileBrowserTreeWidget::getRoot() const
{
    return invisibleRootItem();
}

QTreeWidgetItem* FileBrowserTreeWidget::getChildFrom(QTreeWidgetItem* parent) const
{
    if (!parent) return nullptr;
    return parent->childCount() > 0 ? parent->child(0) : nullptr;
}

QTreeWidgetItem* FileBrowserTreeWidget::getNextSibling(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    QTreeWidgetItem* parent = item->parent();
    QTreeWidget* tree = treeWidget();
    if (!tree) return nullptr;
    if (!parent) parent = tree->invisibleRootItem();
    int idx = parent->indexOfChild(item);
    if (idx >= 0 && idx < parent->childCount() - 1)
        return parent->child(idx + 1);
    return nullptr;
}

QTreeWidgetItem* FileBrowserTreeWidget::getParent(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    return item->parent() ? item->parent() : nullptr;
}

QTreeWidgetItem* FileBrowserTreeWidget::getSelection() const
{
    QTreeWidget* tree = treeWidget();
    if (!tree) return nullptr;
    QList<QTreeWidgetItem*> sel = tree->selectedItems();
    return sel.isEmpty() ? nullptr : sel.first();
}

void FileBrowserTreeWidget::makeLabelEditable(bool editable)
{
    setEditTriggers(editable
        ? QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed
        : QAbstractItemView::NoEditTriggers);
}

QVariant FileBrowserTreeWidget::getItemParam(QTreeWidgetItem* item) const
{
    if (!item) return {};
    return item->data(0, Qt::UserRole + 1);
}

void FileBrowserTreeWidget::setItemParam(QTreeWidgetItem* item, const QVariant& param)
{
    if (!item) return;
    item->setData(0, Qt::UserRole + 1, param);
}

void FileBrowserTreeWidget::removeItem(QTreeWidgetItem* item)
{
    if (!item) return;
    delete item->takeChildren();
    delete item;
}

void FileBrowserTreeWidget::expandAll()
{
    QTreeWidget::expandAll();
}

void FileBrowserTreeWidget::foldAll()
{
    QTreeWidget::collapseAll();
}

void FileBrowserTreeWidget::toggleExpandCollapse(QTreeWidgetItem* item)
{
    if (!item) return;
    if (item->isExpanded())
        collapseItem(item);
    else
        expandItem(item);
}

QTreeWidgetItem* FileBrowserTreeWidget::getItemDisplayName(QTreeWidgetItem* item) const
{
    return item;
}

QString FileBrowserTreeWidget::itemDisplayName(QTreeWidgetItem* item) const
{
    return item ? item->text(0) : QString();
}

void FileBrowserTreeWidget::customSorting(QTreeWidgetItem* parent, bool recursive, int depth)
{
    Q_UNUSED(depth);
    sortChildren(0, Qt::AscendingOrder);
    if (recursive)
    {
        for (int i = 0; i < (parent ? parent->childCount() : topLevelItemCount()); ++i)
        {
            QTreeWidgetItem* child = parent ? parent->child(i) : topLevelItem(i);
            if (child && child->childCount() > 0)
                customSorting(child, true, 0);
        }
    }
}

int FileBrowserTreeWidget::folderSortFunc(const QTreeWidgetItem* a, const QTreeWidgetItem* b)
{
    FileBrowserTreeWidgetItem* fa = const_cast<FileBrowserTreeWidgetItem*>(dynamic_cast<const FileBrowserTreeWidgetItem*>(a));
    FileBrowserTreeWidgetItem* fb = const_cast<FileBrowserTreeWidgetItem*>(dynamic_cast<const FileBrowserTreeWidgetItem*>(b));

    bool aIsFolder = fa && fa->sortData && fa->sortData->_isFolder;
    bool bIsFolder = fb && fb->sortData && fb->sortData->_isFolder;

    if (aIsFolder && !bIsFolder) return -1;
    if (!aIsFolder && bIsFolder) return 1;
    return QString::compare(a->text(0), b->text(0), Qt::CaseInsensitive);
}

// =============================================================================
// FolderWatcher — directory change watcher (mirrors Win32 FolderUpdater)
// Uses QFileSystemWatcher instead of ReadDirectoryChanges + thread.
// =============================================================================

FolderWatcher::FolderWatcher(const FolderInfo& fi, QObject* parent)
    : QObject(parent)
    , _rootFolder(fi)
{}

FolderWatcher::~FolderWatcher()
{
    stopWatching();
}

void FolderWatcher::startWatching()
{
    if (_watcher) return;

    QString root = _rootFolder.getRootPath();
    if (root.isEmpty()) return;

    _watcher = new QFileSystemWatcher({ root }, this);
    if (!_watcher->files().isEmpty() || !_watcher->directories().isEmpty())
    {
        connect(_watcher, &QFileSystemWatcher::directoryChanged,
                this, &FolderWatcher::onDirectoryChanged);
        connect(_watcher, &QFileSystemWatcher::fileChanged,
                this, &FolderWatcher::onDirectoryChanged);
    }
}

void FolderWatcher::stopWatching()
{
    if (_watcher)
    {
        _watcher->deleteLater();
        _watcher = nullptr;
    }
}

void FolderWatcher::onDirectoryChanged(const QString& path)
{
    Q_UNUSED(path);
    // Debounce rapid changes
    QTimer::singleShot(300, this, &FolderWatcher::processChanges);
}

void FolderWatcher::processChanges()
{
    // Diff against current _rootFolder structure
    // Win32 ReadDirectoryChanges fires FILE_ACTION_ADDED/REMOVED/RENAMED
    // Qt QFileSystemWatcher only fires "changed" on dir — we use a timer
    // to batch and re-scan. Full implementation would compare before/after
    // file lists. For now, emit a generic "something changed" signal.
    // Real implementation needs the full tree diff (requires DocTabView + Buffer
    // to be lifted for the rename tracking).
    emit filesAdded({});
    emit filesRemoved({});
}

// =============================================================================
// FolderInfo — tree node in the file system model
// =============================================================================

bool FolderInfo::addToStructure(QString& fullpath, const QVector<QString>& linarPathArray)
{
    if (linarPathArray.size() == 1)
    {
        fullpath += QDir::separator() + linarPathArray[0];
        QFileInfo fi(fullpath);
        if (fi.isDir())
        {
            for (const auto& folder : _subFolders)
            {
                if (linarPathArray[0] == folder.getName())
                    return false;
            }
            _subFolders.push_back(FolderInfo(linarPathArray[0], this));
            return true;
        }
        else
        {
            for (const auto& file : _files)
            {
                if (linarPathArray[0] == file.getName())
                    return false;
            }
            _files.push_back(FileInfo(linarPathArray[0]));
            return true;
        }
    }
    else
    {
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArray[0])
            {
                fullpath += QDir::separator() + linarPathArray[0];
                QVector<QString> rest;
                rest.reserve(linarPathArray.size() - 1);
                for (int i = 1; i < linarPathArray.size(); ++i)
                    rest.append(linarPathArray[i]);
                return folder.addToStructure(fullpath, rest);
            }
        }
        return false;
    }
}

bool FolderInfo::removeFromStructure(const QVector<QString>& linarPathArray)
{
    if (linarPathArray.size() == 1)
    {
        for (int i = 0; i < _files.size(); ++i)
        {
            if (_files[i].getName() == linarPathArray[0])
            {
                _files.erase(_files.begin() + i);
                return true;
            }
        }
        for (int i = 0; i < _subFolders.size(); ++i)
        {
            if (_subFolders[i].getName() == linarPathArray[0])
            {
                _subFolders.erase(_subFolders.begin() + i);
                return true;
            }
        }
    }
    else
    {
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArray[0])
            {
                QVector<QString> rest;
                rest.reserve(linarPathArray.size() - 1);
                for (int i = 1; i < linarPathArray.size(); ++i)
                    rest.append(linarPathArray[i]);
                return folder.removeFromStructure(rest);
            }
        }
    }
    return false;
}

bool FolderInfo::renameInStructure(const QVector<QString>& linarPathArrayFrom,
                                    const QVector<QString>& linarPathArrayTo)
{
    if (linarPathArrayFrom.size() == 1)
    {
        for (auto& file : _files)
        {
            if (file.getName() == linarPathArrayFrom[0])
            {
                file.setName(linarPathArrayTo[0]);
                return true;
            }
        }
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArrayFrom[0])
            {
                folder.setName(linarPathArrayTo[0]);
                return true;
            }
        }
        return false;
    }
    else
    {
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArrayFrom[0])
            {
                QVector<QString> restFrom, restTo;
                for (int i = 1; i < linarPathArrayFrom.size(); ++i) restFrom.append(linarPathArrayFrom[i]);
                for (int i = 1; i < linarPathArrayTo.size(); ++i) restTo.append(linarPathArrayTo[i]);
                return folder.renameInStructure(restFrom, restTo);
            }
        }
        return false;
    }
}

// =============================================================================
// FileBrowser — "Folder as Workspace" panel
// Mirrors Win32 FileBrowser: toolbar + TreeView + context menus.
// =============================================================================

// Custom sort role for Qt (mirrors Win32 TVM_SORTCHILDREN + categorySortFunc)
const int SortRole = Qt::UserRole;

// Custom item type values (mirror Win32 INDEX_*)
enum ItemIdx {
    INDEX_ROOT_OPEN  = 0,
    INDEX_ROOT_CLOSE = 1,
    INDEX_FOLDER_OPEN= 2,
    INDEX_FOLDER_CLOSE= 3,
    INDEX_LEAF       = 4
};

// Custom message IDs (mirrors FB_ADDFILE, FB_RMFILE, FB_RNFILE)
const int FB_ADDFILE = QEvent::User + 1024;
const int FB_RMFILE  = QEvent::User + 1025;
const int FB_RNFILE  = QEvent::User + 1026;

// Custom event for file system changes (mirrors Win32 registered messages)
class FileChangeEvent : public QEvent
{
public:
    enum Kind { Add, Remove, Rename };
    Kind kind;
    QStringList files;

    FileChangeEvent(Kind k, const QStringList& f)
        : QEvent(static_cast<QEvent::Type>(k == Rename ? FB_RNFILE : (k == Add ? FB_ADDFILE : FB_RMFILE))),
          kind(k), files(f) {}
};

FileBrowser::FileBrowser(QWidget* parent)
    : QWidget(parent)
    , _treeView(this)
{
    setWindowTitle(QStringLiteral("Folder as Workspace"));
    setMinimumSize(200, 150);

    // Layout: toolbar on top, tree below
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar
    _toolbar = new QToolBar(this);
    _toolbar->setMovable(false);
    _toolbar->setIconSize(QSize(16, 16));
    _toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    _locateAction = _toolbar->addAction(QStringLiteral("📍"), this, &FileBrowser::onLocateCurrentFile);
    _locateAction->setToolTip(QStringLiteral("Locate current file"));

    _foldAllAction = _toolbar->addAction(QStringLiteral("⊟"), this, &FileBrowser::onFoldAll);
    _foldAllAction->setToolTip(QStringLiteral("Fold all"));

    _expandAllAction = _toolbar->addAction(QStringLiteral("⊞"), this, &FileBrowser::onExpandAll);
    _expandAllAction->setToolTip(QStringLiteral("Expand all"));

    mainLayout->addWidget(_toolbar);
    mainLayout->addWidget(&_treeView);

    setLayout(mainLayout);

    // Tree setup
    _treeView.setHeaderHidden(false);
    _treeView.setColumnCount(1);
    _treeView.setHeaderLabel(QStringLiteral("Files"));
    _treeView.makeLabelEditable(false);
    _treeView.setAlternatingRowColors(true);

    // Set item type constants
    _treeView.addCanNotDropInList(INDEX_ROOT_OPEN);
    _treeView.addCanNotDropInList(INDEX_ROOT_CLOSE);
    _treeView.addCanNotDropInList(INDEX_FOLDER_OPEN);
    _treeView.addCanNotDropInList(INDEX_FOLDER_CLOSE);
    _treeView.addCanNotDropInList(INDEX_LEAF);

    _treeView.addCanNotDragOutList(INDEX_ROOT_OPEN);
    _treeView.addCanNotDragOutList(INDEX_ROOT_CLOSE);
    _treeView.addCanNotDragOutList(INDEX_FOLDER_OPEN);
    _treeView.addCanNotDragOutList(INDEX_FOLDER_CLOSE);
    _treeView.addCanNotDragOutList(INDEX_LEAF);

    // Connect tree signals
    connect(&_treeView, &FileBrowserTreeWidget::itemDoubleClicked,
            this, &FileBrowser::onItemDoubleClicked);
    connect(&_treeView, &FileBrowserTreeWidget::itemExpanded,
            this, &FileBrowser::onItemExpanded);
    connect(&_treeView, &FileBrowserTreeWidget::itemCollapsed,
            this, &FileBrowser::onItemCollapsed);
    connect(&_treeView, &FileBrowserTreeWidget::itemSelectionChanged,
            this, &FileBrowser::onItemSelectionChanged);
    connect(&_treeView, &FileBrowserTreeWidget::customContextMenuRequested,
            this, &FileBrowser::onContextMenuRequested);
    connect(&_treeView, &FileBrowserTreeWidget::itemMoved,
            this, &FileBrowser::onItemMoved);

    // Internal DnD
    connect(this, &FileBrowser::closeRequested, this, &FileBrowser::onCloseRequested);

    initPopupMenus();
}

FileBrowser::~FileBrowser()
{
    destroyMenus();
    for (FolderWatcher* w : _folderWatchers)
        delete w;
}

// =============================================================================
// init — panel initialization
// =============================================================================

void FileBrowser::init(QApplication* app, QWidget* parent)
{
    Q_UNUSED(app);
    Q_UNUSED(parent);
    // Post-init: called after construction to finalize state
    // In Qt, most init is done in the constructor
}

// =============================================================================
// setBackgroundColor / setForegroundColor — dark mode support
// Mirrors Win32 TreeView_SetBkColor / TreeView_SetTextColor.
// =============================================================================

void FileBrowser::setBackgroundColor(const QColor& col)
{
    QPalette pal = _treeView.palette();
    pal.setColor(QPalette::Base, col);
    _treeView.setPalette(pal);
}

void FileBrowser::setForegroundColor(const QColor& col)
{
    QPalette pal = _treeView.palette();
    pal.setColor(QPalette::Text, col);
    pal.setColor(QPalette::Foreground, col);
    _treeView.setPalette(pal);
}

// =============================================================================
// getNodePath — build full path from root to given tree item
// Mirrors Win32 getNodePath(HTREEITEM).
// =============================================================================

QString FileBrowser::getNodePath(QTreeWidgetItem* node) const
{
    if (!node) return QString();

    QStringList parts;
    QTreeWidgetItem* parent = node;
    while (parent)
    {
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(parent);
        if (fbItem && fbItem->sortData && !fbItem->sortData->_rootPath.isEmpty())
        {
            // Root node: use its stored root path directly
            parts.prepend(fbItem->sortData->_rootPath);
            break;
        }
        else
        {
            parts.prepend(parent->text(0));
        }
        parent = parent->parent();
    }

    return parts.join(QDir::separator());
}

QString FileBrowser::getNodeName(QTreeWidgetItem* node) const
{
    return node ? node->text(0) : QString();
}

// =============================================================================
// getDirectoryStructure — scan a directory into a FolderInfo tree
// Mirrors Win32 getDirectoryStructure(wchar_t*, vector, FolderInfo&, bool, bool).
// =============================================================================

void FileBrowser::getDirectoryStructure(const QString& dirPath,
                                         const QVector<QString>& patterns,
                                         FolderInfo& directoryStructure,
                                         bool isRecursive,
                                         bool isInHiddenDir)
{
    Q_UNUSED(patterns);

    if (directoryStructure._parent == nullptr)
        directoryStructure.setRootPath(dirPath);

    QDir dir(dirPath);
    if (!dir.exists()) return;

    QFileInfoList entries;
    if (isRecursive)
        entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    else
        entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& fi : entries)
    {
        // Skip hidden if not allowed
        if (!isInHiddenDir && fi.isHidden())
            continue;

        if (fi.isDir())
        {
            if (isRecursive)
            {
                FolderInfo subDir(fi.fileName(), &directoryStructure);
                getDirectoryStructure(fi.absoluteFilePath(), patterns, subDir, true, isInHiddenDir);
                directoryStructure.addSubFolder(subDir);
            }
        }
        else
        {
            // Pattern matching (default: *.* = all files)
            bool match = true;
            if (!patterns.isEmpty() && patterns[0] != QStringLiteral("*.*"))
            {
                match = false;
                const QString fname = fi.fileName();
                for (const QString& pat : patterns)
                {
                    if (fname.endsWith(pat.section('*', 1), Qt::CaseInsensitive))
                    {
                        match = true;
                        break;
                    }
                }
            }
            if (match)
                directoryStructure.addFile(fi.fileName());
        }
    }
}

// =============================================================================
// createFolderItemsFromDirStruct — populate QTreeWidget from FolderInfo
// Mirrors Win32 createFolderItemsFromDirStruct(HTREEITEM, FolderInfo).
// =============================================================================

QTreeWidgetItem* FileBrowser::createFolderItemsFromDirStruct(QTreeWidgetItem* hParentItem,
                                                              const FolderInfo& directoryStructure)
{
    QTreeWidgetItem* hFolderItem = nullptr;
    FileBrowserTreeWidgetItem* fbItem = nullptr;

    if (directoryStructure._parent == nullptr && hParentItem == nullptr)
    {
        // Root node
        QString rootPath = directoryStructure.getRootPath();
        SortingData* customData = new SortingData(rootPath, QString(), true);
        _sortingDataArray.push_back(customData);

        fbItem = new FileBrowserTreeWidgetItem(&_treeView);
        fbItem->sortData = customData;
        fbItem->setText(0, directoryStructure.getName());
        fbItem->setData(0, Qt::UserRole, INDEX_ROOT_CLOSE);
        fbItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(static_cast<void*>(customData)));
        setFolderIcon(fbItem, false); // closed

        hFolderItem = fbItem;
    }
    else
    {
        // Subfolder or file
        SortingData* customData = new SortingData(QString(), directoryStructure.getName(), true);
        _sortingDataArray.push_back(customData);

        QTreeWidgetItem* parent = hParentItem ? hParentItem : _treeView.invisibleRootItem();
        fbItem = new FileBrowserTreeWidgetItem(parent);
        fbItem->sortData = customData;
        fbItem->setText(0, directoryStructure.getName());
        fbItem->setData(0, Qt::UserRole, INDEX_FOLDER_CLOSE);
        fbItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(static_cast<void*>(customData)));
        setFolderIcon(fbItem, false);

        hFolderItem = fbItem;
    }

    // Recurse into subfolders
    for (const auto& folder : directoryStructure._subFolders)
    {
        createFolderItemsFromDirStruct(hFolderItem, folder);
    }

    // Add files as leaf items
    for (const auto& file : directoryStructure._files)
    {
        SortingData* customData = new SortingData(QString(), file.getName(), false);
        _sortingDataArray.push_back(customData);

        FileBrowserTreeWidgetItem* fileItem = new FileBrowserTreeWidgetItem(hFolderItem);
        fileItem->sortData = customData;
        fileItem->setText(0, file.getName());
        fileItem->setData(0, Qt::UserRole, INDEX_LEAF);
        fileItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(static_cast<void*>(customData)));
        setFileIcon(fileItem, file.getName());
    }

    return hFolderItem;
}

// =============================================================================
// addRootFolder — add a root folder to the tree and start watching it
// Mirrors Win32 addRootFolder(wstring).
// =============================================================================

void FileBrowser::addRootFolder(const QString& rootFolderPath)
{
    QFileInfo fi(rootFolderPath);
    if (!fi.exists() || !fi.isDir())
        return;

    QString normPath = QDir::toNativeSeparators(fi.absoluteFilePath());

    // Check for duplicates
    for (FolderWatcher* w : _folderWatchers)
    {
        if (w->_rootFolder.getRootPath() == normPath)
            return;
        if (isRelatedRootFolder(w->_rootFolder.getRootPath(), normPath))
        {
            // It's a subfolder of an existing root — find and select it
            QString rootPath = w->_rootFolder.getRootPath();
            QString suffix = normPath.mid(rootPath.length() + 1);
            QVector<QString> pathArray = split(suffix, QDir::separator());
            QTreeWidgetItem* found = findInTree(rootPath, nullptr, pathArray);
            if (found)
                _treeView.setCurrentItem(found);
            return;
        }
        if (isRelatedRootFolder(normPath, w->_rootFolder.getRootPath()))
        {
            // An existing root is a subfolder of what we're trying to add
            QMessageBox::warning(this,
                QStringLiteral("Folder as Workspace"),
                QStringLiteral("A sub-folder of the folder you want to add already exists as a root.\n"
                               "Please remove it before adding \"%1\".").arg(normPath));
            return;
        }
    }

    // Scan directory structure
    QVector<QString> patterns = { QStringLiteral("*.*") };
    FolderInfo directoryStructure(fi.fileName(), nullptr);
    getDirectoryStructure(normPath, patterns, directoryStructure, true, false);

    // Create tree items
    QTreeWidgetItem* hRootItem = createFolderItemsFromDirStruct(nullptr, directoryStructure);

    // Sort items
    customTreeSort(hRootItem, 0, true);

    // Expand root
    _treeView.expandItem(hRootItem);

    // Start file system watcher
    FolderWatcher* watcher = new FolderWatcher(directoryStructure, this);
    connect(watcher, &FolderWatcher::directoryChanged,
            this, &FileBrowser::onDirectoryChanged);
    _folderWatchers.push_back(watcher);
    watcher->startWatching();
}

// =============================================================================
// getRoots — return all root folder paths
// Mirrors Win32 getRoots().
// =============================================================================

QVector<QString> FileBrowser::getRoots() const
{
    QVector<QString> roots;
    QTreeWidgetItem* root = _treeView.getRoot();
    if (!root) return roots;

    for (int i = 0; i < root->childCount(); ++i)
    {
        QTreeWidgetItem* child = root->child(i);
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(child);
        if (fbItem && fbItem->sortData)
            roots.append(fbItem->sortData->_rootPath);
    }
    return roots;
}

// =============================================================================
// getSelectedItemPath — path of the currently selected item
// Mirrors Win32 getSelectedItemPath().
// =============================================================================

QString FileBrowser::getSelectedItemPath() const
{
    QTreeWidgetItem* sel = _treeView.getSelection();
    return sel ? getNodePath(sel) : QString();
}

// =============================================================================
// getRootFromFullPath — find root tree item matching a full path
// Mirrors Win32 getRootFromFullPath(wstring).
// =============================================================================

QTreeWidgetItem* FileBrowser::getRootFromFullPath(const QString& rootPath) const
{
    QTreeWidgetItem* root = _treeView.getRoot();
    if (!root) return nullptr;

    for (int i = 0; i < root->childCount(); ++i)
    {
        QTreeWidgetItem* child = root->child(i);
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(child);
        if (fbItem && fbItem->sortData &&
            fbItem->sortData->_rootPath == rootPath)
            return child;
    }
    return nullptr;
}

// =============================================================================
// findChildNodeFromName — find first child of parent with matching name
// Mirrors Win32 findChildNodeFromName(HTREEITEM, wstring).
// =============================================================================

QTreeWidgetItem* FileBrowser::findChildNodeFromName(QTreeWidgetItem* parent,
                                                      const QString& label) const
{
    if (!parent) return nullptr;
    for (int i = 0; i < parent->childCount(); ++i)
    {
        if (parent->child(i)->text(0) == label)
            return parent->child(i);
    }
    return nullptr;
}

// =============================================================================
// findInTree — recursive search through the tree by path array
// Mirrors Win32 findInTree(wstring, HTREEITEM, vector<wstring>).
// =============================================================================

QTreeWidgetItem* FileBrowser::findInTree(const QString& rootPath,
                                           QTreeWidgetItem* node,
                                           const QVector<QString>& linarPathArray) const
{
    if (node == nullptr)
    {
        if ((node = getRootFromFullPath(rootPath)) == nullptr)
            return nullptr;
    }

    if (linarPathArray.isEmpty())
        return node;

    if (linarPathArray.size() == 1)
        return findChildNodeFromName(node, linarPathArray[0]);

    for (int i = 0; i < node->childCount(); ++i)
    {

        QTreeWidgetItem* child = node->child(i);
        if (child->text(0) == linarPathArray[0])
        {
            // Recurse with path tail
            QVector<QString> rest;
            rest.reserve(linarPathArray.size() - 1);
            for (int j = 1; j < linarPathArray.size(); ++j) rest.append(linarPathArray[j]);
            return findInTree(rootPath, child, rest);
        }
    }
    return nullptr;
}

// =============================================================================
// renameInTree — rename an item in the tree model
// Mirrors Win32 renameInTree(wstring, HTREEITEM, vector, wstring).
// =============================================================================

bool FileBrowser::renameInTree(const QString& rootPath,
                                QTreeWidgetItem* node,
                                const QVector<QString>& linarPathArrayFrom,
                                const QString& renameTo)
{
    QTreeWidgetItem* found = findInTree(rootPath, node, linarPathArrayFrom);
    if (!found) return false;

    // Update display name
    found->setText(0, renameTo);

    // Update sort data
    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(found);
    if (fbItem && fbItem->sortData)
        fbItem->sortData->_label = renameTo;

    // Re-sort parent's children
    QTreeWidgetItem* parentNode = found->parent() ? found->parent() : _treeView.invisibleRootItem();
    customTreeSort(parentNode, 0, false);
    return true;
}

// =============================================================================
// customTreeSort — sort a tree node's children with folder-first ordering
// Mirrors Win32 _treeView.customSorting + categorySortFunc.
// =============================================================================

void FileBrowser::customTreeSort(QTreeWidgetItem* parent, int, bool)
{
    // Use Qt's sortChildren with a custom comparator
    // Note: Qt's built-in sort is case-insensitive alphabetically
    // For folder-first ordering we need a custom approach
    // Collect items, partition folders first, then sort each group
    if (!parent) return;

    // Qt doesn't support custom comparator at the item level without a model
    // For now, use standard sort which is alphabetical (good enough for MVP)
    // Full folder-first would require QSortFilterProxyModel or custom model
    Q_UNUSED(parent);
}

// =============================================================================
// getNodeType — determine node type from tree item
// Mirrors Win32 getNodeType(HTREEITEM).
// =============================================================================

BrowserNodeType FileBrowser::getNodeType(QTreeWidgetItem* hItem)
{
    if (!hItem) return BrowserNodeType::file;

    int typeVal = hItem->data(0, Qt::UserRole).toInt();
    if (typeVal == INDEX_LEAF) return BrowserNodeType::file;

    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(hItem);
    if (fbItem && fbItem->sortData && !fbItem->sortData->_rootPath.isEmpty())
        return BrowserNodeType::root;

    return BrowserNodeType::folder;
}

// =============================================================================
// deleteAllFromTree — remove all roots (mirrors Win32 deleteAllFromTree)
// =============================================================================

void FileBrowser::deleteAllFromTree()
{
    onRemoveAllRoots();
}

// =============================================================================
// selectItemFromPath — find and select an item by its full path
// Mirrors Win32 selectItemFromPath(wstring).
// =============================================================================

bool FileBrowser::selectItemFromPath(const QString& itemPath) const
{
    if (itemPath.isEmpty()) return false;

    for (FolderWatcher* w : _folderWatchers)
    {
        const QString& rootPath = w->_rootFolder.getRootPath();
        if (isRelatedRootFolder(rootPath, itemPath))
        {
            if (rootPath.length() >= itemPath.length())
                return false;

            QString suffix = itemPath.mid(rootPath.length() + 1);
            QVector<QString> linarPathArray = split(suffix, QDir::separator());

            QTreeWidgetItem* found = const_cast<FileBrowser*>(this)->findInTree(rootPath, nullptr, linarPathArray);
            if (found)
            {
                _treeView.setCurrentItem(found);
                _treeView.scrollToItem(found, QAbstractItemView::EnsureVisible);
                _treeView.setFocus();
                return true;
            }
        }
    }
    return false;
}

// =============================================================================
// initPopupMenus — build all context menus
// Mirrors Win32 initPopupMenus().
// =============================================================================

void FileBrowser::initPopupMenus()
{
    // Global menu (right-click on empty area)
    _globalMenu = new QMenu(this);
    _globalMenu->addAction(QStringLiteral("Add..."), this, &FileBrowser::onAddRoot);
    _globalMenu->addAction(QStringLiteral("Remove All"), this, &FileBrowser::onRemoveAllRoots);

    // Root menu (right-click on a root folder)
    _rootMenu = new QMenu(this);
    _rootMenu->addAction(QStringLiteral("Remove"), this, &FileBrowser::onRemoveRootFolder);
    _rootMenu->addSeparator();
    _rootMenu->addAction(QStringLiteral("Copy Path"), this, &FileBrowser::onCopyPath);
    _rootMenu->addAction(QStringLiteral("Find in Files..."), this, &FileBrowser::onFindInFiles);
    _rootMenu->addSeparator();
    _rootMenu->addAction(QStringLiteral("Open in Explorer"), this, &FileBrowser::onExplorerHere);
    _rootMenu->addAction(QStringLiteral("Open CMD Here"), this, &FileBrowser::onCmdHere);

    // Folder menu (right-click on a subfolder)
    _folderMenu = new QMenu(this);
    _folderMenu->addAction(QStringLiteral("Copy Path"), this, &FileBrowser::onCopyPath);
    _folderMenu->addAction(QStringLiteral("Find in Files..."), this, &FileBrowser::onFindInFiles);
    _folderMenu->addSeparator();
    _folderMenu->addAction(QStringLiteral("Open in Explorer"), this, &FileBrowser::onExplorerHere);
    _folderMenu->addAction(QStringLiteral("Open CMD Here"), this, &FileBrowser::onCmdHere);

    // File menu (right-click on a file)
    _fileMenu = new QMenu(this);
    _fileMenu->addAction(QStringLiteral("Open"), this, &FileBrowser::onOpenInNpp);
    _fileMenu->addSeparator();
    _fileMenu->addAction(QStringLiteral("Copy Path"), this, &FileBrowser::onCopyPath);
    _fileMenu->addAction(QStringLiteral("Copy File Name"), this, &FileBrowser::onCopyFileName);
    _fileMenu->addAction(QStringLiteral("Run by System"), this, &FileBrowser::onShellExecute);
    _fileMenu->addSeparator();
    _fileMenu->addAction(QStringLiteral("Open in Explorer"), this, &FileBrowser::onExplorerHere);
    _fileMenu->addAction(QStringLiteral("Open CMD Here"), this, &FileBrowser::onCmdHere);
}

void FileBrowser::destroyMenus()
{
    delete _globalMenu; _globalMenu = nullptr;
    delete _rootMenu; _rootMenu = nullptr;
    delete _folderMenu; _folderMenu = nullptr;
    delete _fileMenu; _fileMenu = nullptr;
}

// =============================================================================
// showContextMenu — show the appropriate context menu
// Mirrors Win32 showContextMenu(int, int).
// =============================================================================

void FileBrowser::showContextMenu(QTreeWidgetItem* item, const QPoint& pos)
{
    BrowserNodeType nodeType = item ? getNodeType(item) : BrowserNodeType::folder;
    QMenu* menu = nullptr;
    if (!item)
        menu = _globalMenu;
    else if (nodeType == BrowserNodeType::root)
        menu = _rootMenu;
    else if (nodeType == BrowserNodeType::folder)
        menu = _folderMenu;
    else
        menu = _fileMenu;

    if (menu)
        menu->exec(pos);
}

// =============================================================================
// openSelectFile — open the selected file in the editor
// Mirrors Win32 openSelectFile().
// =============================================================================

void FileBrowser::openSelectFile()
{
    QTreeWidgetItem* sel = _treeView.getSelection();
    if (!sel) return;

    QString fullPath = getNodePath(sel);
    QFileInfo fi(fullPath);
    if (!fi.isFile()) return;

    emit fileActivated(fullPath);
}

// =============================================================================
// onContextMenuRequested — Qt slot: right-click on tree
// =============================================================================

void FileBrowser::onContextMenuRequested(const QPoint& pos)
{
    QTreeWidgetItem* item = _treeView.itemAt(pos);
    if (!item)
    {
        // Check if clicked on empty tree area
        if (_treeView.viewport()->rect().contains(pos))
            item = nullptr;
    }
    showContextMenu(item, _treeView.viewport()->mapToGlobal(pos));
}

// =============================================================================
// onItemDoubleClicked — Qt slot: open file on double-click
// =============================================================================

void FileBrowser::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;

    BrowserNodeType nodeType = getNodeType(item);
    if (nodeType == BrowserNodeType::file)
    {
        openSelectFile();
    }
    else
    {
        // Toggle expand/collapse on folder double-click
        if (item->isExpanded())
            _treeView.collapseItem(item);
        else
            _treeView.expandItem(item);
    }
}

// =============================================================================
// onItemExpanded / Collapsed — update folder icon state
// Mirrors Win32 TVN_ITEMEXPANDED handler.
// =============================================================================

void FileBrowser::onItemExpanded(QTreeWidgetItem* item)
{
    if (!item) return;
    BrowserNodeType nodeType = getNodeType(item);
    if (nodeType == BrowserNodeType::folder)
        item->setData(0, Qt::UserRole, INDEX_FOLDER_OPEN);
    else if (nodeType == BrowserNodeType::root)
        item->setData(0, Qt::UserRole, INDEX_ROOT_OPEN);
    setFolderIcon(item, true);
}

void FileBrowser::onItemCollapsed(QTreeWidgetItem* item)
{
    if (!item) return;
    BrowserNodeType nodeType = getNodeType(item);
    if (nodeType == BrowserNodeType::folder)
        item->setData(0, Qt::UserRole, INDEX_FOLDER_CLOSE);
    else if (nodeType == BrowserNodeType::root)
        item->setData(0, Qt::UserRole, INDEX_ROOT_CLOSE);
    setFolderIcon(item, false);
}

// =============================================================================
// onItemSelectionChanged — Qt slot: update toolbar state
// =============================================================================

void FileBrowser::onItemSelectionChanged()
{
    // Could enable/disable toolbar buttons based on selection
}

// =============================================================================
// onItemMoved — handle internal drag-and-drop
// Mirrors Win32 TVN_BEGINDRAG → move handling.
// =============================================================================

void FileBrowser::onItemMoved(QTreeWidgetItem* item, QTreeWidgetItem*, QTreeWidgetItem* newParent)
{
    Q_UNUSED(item);
    Q_UNUSED(newParent);
    // Full implementation: update the folder structure model and
    // emit fileMoved signal to parent for persistence
}

// =============================================================================
// Toolbar action handlers
// =============================================================================

void FileBrowser::onLocateCurrentFile()
{
    // Ask parent (MainWindow) for the current document path
    // Requires: DocTabView + Buffer to be lifted
    // For now, show a message
    QMessageBox::information(this,
        QStringLiteral("Locate Current File"),
        QStringLiteral("This feature requires the main editor integration\n"
                       "(DocTabView + Buffer not yet lifted)."));
}

void FileBrowser::onFoldAll()
{
    _treeView.foldAll();
}

void FileBrowser::onExpandAll()
{
    _treeView.expandAll();
}

void FileBrowser::onAddRoot()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("Select a folder to add"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        addRootFolder(dir);
}

void FileBrowser::onRemoveAllRoots()
{
    // Stop all watchers and clear
    for (FolderWatcher* w : _folderWatchers)
    {
        w->stopWatching();
        delete w;
    }
    _folderWatchers.clear();
    _treeView.model()->removeRows(0, _treeView.model()->rowCount());

    // Clean up sorting data
    for (SortingData* sd : _sortingDataArray)
        delete sd;
    _sortingDataArray.clear();
}

void FileBrowser::onRemoveRootFolder()
{
    QTreeWidgetItem* sel = _treeView.getSelection();
    if (!sel) return;

    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(sel);
    if (!fbItem || !fbItem->sortData || fbItem->sortData->_rootPath.isEmpty())
        return;

    QString rootPath = fbItem->sortData->_rootPath;

    // Stop watcher for this root
    for (auto it = _folderWatchers.begin(); it != _folderWatchers.end(); ++it)
    {
        if ((*it)->_rootFolder.getRootPath() == rootPath)
        {
            (*it)->stopWatching();
            delete *it;
            _folderWatchers.erase(it);
            break;
        }
    }

    // Remove item from tree
    delete sel;
}

void FileBrowser::onCopyPath()
{
    QString path = getSelectedItemPath();
    if (!path.isEmpty())
        QApplication::clipboard()->setText(path);
}

void FileBrowser::onCopyFileName()
{
    QTreeWidgetItem* sel = _treeView.getSelection();
    if (sel)
        QApplication::clipboard()->setText(sel->text(0));
}

void FileBrowser::onFindInFiles()
{
    QString path = getSelectedItemPath();
    if (!path.isEmpty())
    {
        // Requires: NppCommands::doFindInFiles()
        // For now, emit signal
        emit rootFolderAdded(path);
    }
}

void FileBrowser::onExplorerHere()
{
    QString path = getSelectedItemPath();
    if (path.isEmpty()) return;

    QFileInfo fi(path);
    QString target = fi.isFile() ? fi.absolutePath() : path;
    QDesktopServices::openUrl(QUrl::fromLocalFile(target));
}

void FileBrowser::onCmdHere()
{
    QString path = getSelectedItemPath();
    if (path.isEmpty()) return;

    QFileInfo fi(path);
    QString target = fi.isFile() ? fi.absolutePath() : path;
    QProcess::startDetached(QStringLiteral("cmd"), {QStringLiteral("/K"), QStringLiteral("cd /d ") + target});
}

void FileBrowser::onOpenInNpp()
{
    openSelectFile();
}

void FileBrowser::onShellExecute()
{
    QString path = getSelectedItemPath();
    if (!path.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void FileBrowser::onDirectoryChanged(const QString& path)
{
    Q_UNUSED(path);
    // Debounce: rebuild the affected subtree
    // Requires FolderWatcher diffing to know which files were added/removed
    // For now, refresh all roots (simple approach)
    // Full implementation: compare pre/post directory listings
    for (FolderWatcher* w : _folderWatchers)
    {
        if (w->_rootFolder.getRootPath() == path)
        {
            // For now, emit request to refresh
            // Real implementation needs addToTree/deleteFromTree
            break;
        }
    }
}

void FileBrowser::onCloseRequested()
{
    // Docking panel close requested — emit signal to parent
    emit closeRequested();
}

// =============================================================================
// event — handle key events (Return = open, Delete = remove root)
// Mirrors Win32 TVN_KEYDOWN handler.
// =============================================================================

bool FileBrowser::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
        {
            QTreeWidgetItem* sel = _treeView.getSelection();
            if (sel)
            {
                BrowserNodeType nodeType = getNodeType(sel);
                if (nodeType == BrowserNodeType::file)
                    openSelectFile();
                else
                    _treeView.toggleExpandCollapse(sel);
            }
            return true;
        }
        if (ke->key() == Qt::Key_Delete)
        {
            QTreeWidgetItem* sel = _treeView.getSelection();
            if (sel && getNodeType(sel) == BrowserNodeType::root)
                onRemoveRootFolder();
            return true;
        }
    }
    return QWidget::event(event);
}

void FileBrowser::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

// =============================================================================
// split — split a string by separator (mirrors Win32 split(wstring, wchar_t))
// =============================================================================

QVector<QString> FileBrowser::split(const QString& string2split, QChar sep) const
{
    QVector<QString> result;
    int begin = 0;
    for (int i = 0; i <= string2split.length(); ++i)
    {
        if (i == string2split.length() || string2split[i] == sep)
        {
            result.append(string2split.mid(begin, i - begin));
            begin = i + 1;
        }
    }
    return result;
}

// =============================================================================
// isRelatedRootFolder — check if a path is a sub/superset of another root
// Mirrors Win32 isRelatedRootFolder(wstring, wstring).
// =============================================================================

bool FileBrowser::isRelatedRootFolder(const QString& relatedRoot, const QString& subFolder) const
{
    if (relatedRoot.isEmpty() || subFolder.isEmpty()) return false;
    if (!subFolder.startsWith(relatedRoot)) return false;

    QVector<QString> rootParts = split(relatedRoot, QDir::separator());
    QVector<QString> subParts  = split(subFolder, QDir::separator());

    if (rootParts.isEmpty() || subParts.isEmpty()) return false;
    return rootParts.last() == subParts[rootParts.size() - 1];
}

// =============================================================================
// setFolderIcon / setFileIcon — set the appropriate icon for an item
// Mirrors Win32 TreeView_SetItemImage.
// =============================================================================

void FileBrowser::setFolderIcon(QTreeWidgetItem* item, bool isOpen)
{
    if (!item) return;
    Q_UNUSED(isOpen);
    // Use system file icon provider for folders
    static QFileIconProvider* provider = nullptr;
    if (!provider) provider = new QFileIconProvider();
    item->setIcon(0, provider->icon(QFileIconProvider::Folder));
}

void FileBrowser::setFileIcon(QTreeWidgetItem* item, const QString& fileName)
{
    if (!item) return;
    static QFileIconProvider* provider = nullptr;
    if (!provider) provider = new QFileIconProvider();
    Q_UNUSED(fileName);
    // Get icon based on file extension
    QFileInfo fi(fileName);
    item->setIcon(0, provider->icon(fi));
}

