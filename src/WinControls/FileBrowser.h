// Semantic Lift: Win32 FileBrowser → Qt6 Folder as Workspace Panel
// Original: PowerEditor/src/WinControls/FileBrowser/
// Target: npp-qt/src/WinControls/FileBrowser.h + .cpp

#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QString>
#include <QVector>
#include <QMap>
#include <QFileSystemWatcher>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QApplication>
#include <QSet>
#include "DockingWnd.h"

// =============================================================================
// SortingData — lParam wrapper for tree items (mirrors Win32 SortingData4lParam)
// =============================================================================

struct SortingData {
    QString _rootPath; // Only for the root
    QString _label;
    bool _isFolder = false;

    SortingData(const QString& rootPath, const QString& label, bool isFolder)
        : _rootPath(rootPath), _label(label), _isFolder(isFolder) {}
};

// =============================================================================
// FileBrowserTreeWidgetItem — tree item with custom data storage
// =============================================================================

class FileBrowserTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit FileBrowserTreeWidgetItem(QTreeWidget* parent = nullptr)
        : QTreeWidgetItem(parent, Type) {}

    explicit FileBrowserTreeWidgetItem(QTreeWidgetItem* parent = nullptr)
        : QTreeWidgetItem(parent, Type) {}

    SortingData* sortData = nullptr;
};

// =============================================================================
// FileInfo / FolderInfo — data structures for file system representation
// =============================================================================

class FileInfo final {
public:
    explicit FileInfo(const QString& name) : _name(name) {}
    const QString& getName() const { return _name; }
    void setName(const QString& name) { _name = name; }
private:
    QString _name;
};

class FolderInfo final {
public:
    FolderInfo() = default;
    FolderInfo(const QString& name, FolderInfo* parent)
        : _name(name), _parent(parent) {}
    void setRootPath(const QString& rootPath) { _rootPath = rootPath; }
    const QString& getRootPath() const { return _rootPath; }
    void setName(const QString& name) { _name = name; }
    const QString& getName() const { return _name; }
    void addFile(const QString& fn) { _files.push_back(FileInfo(fn)); }
    void addSubFolder(FolderInfo subDirectoryStructure) { _subFolders.push_back(subDirectoryStructure); }
    bool addToStructure(QString& fullpath, const QVector<QString>& linarPathArray);
    bool removeFromStructure(const QVector<QString>& linarPathArray);
    bool renameInStructure(const QVector<QString>& linarPathArrayFrom, const QVector<QString>& linarPathArrayTo);

    // Public accessors for private members
    FolderInfo* parent() const { return _parent; }
    QVector<FolderInfo>& subFolders() { return _subFolders; }
    const QVector<FolderInfo>& subFolders() const { return _subFolders; }
    QVector<FileInfo>& files() { return _files; }
    const QVector<FileInfo>& files() const { return _files; }

private:
    QVector<FolderInfo> _subFolders;
    QVector<FileInfo> _files;
    QString _name;
    FolderInfo* _parent = nullptr;
    QString _rootPath; // only for root folder
};

// =============================================================================
// FolderWatcher — watches a directory for file system changes (lifted from FolderUpdater)
// =============================================================================

class FolderWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FolderWatcher(const FolderInfo& fi, QObject* parent = nullptr);
    ~FolderWatcher() override;

    void startWatching();
    void stopWatching();
    FolderInfo getRootFolder() const { return _rootFolder; }

public:
    // _rootFolder must be public — some callers access it directly for performance
    FolderInfo _rootFolder;

signals:
    void filesAdded(const QVector<QString>& files);
    void filesRemoved(const QVector<QString>& files);
    void filesRenamed(const QVector<QString>& from, const QVector<QString>& to);

private slots:
    void onDirectoryChanged(const QString& path);
    void processChanges();

private:
    QFileSystemWatcher* _watcher = nullptr;
    bool _stopped = false;

    QVector<QString> _pendingAdds;
    QVector<QString> _pendingRemoves;
    QMap<QString, QString> _pendingRenames;
};

// =============================================================================
// FileBrowserTreeWidget — tree view for file browser (lifted from TreeView)
// =============================================================================

enum class BrowserNodeType {
    root = 0,
    folder = 2,
    file = 3
};

class FileBrowserTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit FileBrowserTreeWidget(QWidget* parent = nullptr);
    ~FileBrowserTreeWidget() override = default;

    static constexpr int INDEX_ROOT_OPEN = 0;
    static constexpr int INDEX_ROOT_CLOSE = 1;
    static constexpr int INDEX_FOLDER_OPEN = 2;
    static constexpr int INDEX_FOLDER_CLOSE = 3;
    static constexpr int INDEX_LEAF = 4;

    // Drag and drop
    bool isDragging() const { return _isDragging; }
    void beginDrag(QTreeWidgetItem* item);
    void dragItem(const QPoint& pos);
    bool dropItem();
    void addCanNotDropInList(int index);
    void addCanNotDragOutList(int index);
    bool isDroppable(QTreeWidgetItem* item) const;
    bool isDraggable(QTreeWidgetItem* item) const;

    // Item management
    QTreeWidgetItem* searchSubItemByName(const QString& name, QTreeWidgetItem* root) const;
    void makeLabelEditable(bool editable);
    QVariant getItemParam(QTreeWidgetItem* item) const;
    void setItemParam(QTreeWidgetItem* item, const QVariant& param);
    void removeItem(QTreeWidgetItem* item);
    QTreeWidgetItem* getRoot() const;
    QTreeWidgetItem* getChildFrom(QTreeWidgetItem* parent) const;
    QTreeWidgetItem* getNextSibling(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getParent(QTreeWidgetItem* item) const;
    void expandAll();
    void foldAll();
    void toggleExpandCollapse(QTreeWidgetItem* item);
    QTreeWidgetItem* getItemDisplayName(QTreeWidgetItem* item) const;
    QString itemDisplayName(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getSelection() const;

    // Sorting
    void customSorting(QTreeWidgetItem* parent, bool recursive, int depth = 0);
    static int folderSortFunc(const QTreeWidgetItem* a, const QTreeWidgetItem* b);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void itemMoved(QTreeWidgetItem* item, QTreeWidgetItem* oldParent, QTreeWidgetItem* newParent);
    void fileDropped(const QString& path);

private:
    bool _isDragging = false;
    QTreeWidgetItem* _dragItem = nullptr;
    QPoint _dragStartPos;
    QSet<int> _cannotDropList;
    QSet<int> _cannotDragList;
};

// =============================================================================
// FileBrowser — main docking panel (lifted from FileBrowser)
// =============================================================================

class FileBrowser : public DockingDlgInterface
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget* parent = nullptr);
    ~FileBrowser() override;

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    void setParentWindow(QWidget* parent) { _hParent = parent; }
    void init(QApplication* app, QWidget* parent);

    void setBackgroundColor(const QColor& col);
    void setForegroundColor(const QColor& col);

    // Win32 compat: setText(bool) — show/hide panel
    void setText(bool show = true) { display(show); }
    void setText(const QString& /*title*/) { /* stub */ }

    // File system operations
    QString getNodePath(QTreeWidgetItem* node) const;
    QString getNodeName(QTreeWidgetItem* node) const;
    void addRootFolder(const QString& rootFolderPath);
    QTreeWidgetItem* getRootFromFullPath(const QString& rootPath) const;
    QTreeWidgetItem* findChildNodeFromName(QTreeWidgetItem* parent, const QString& label) const;
    QTreeWidgetItem* findInTree(const QString& rootPath, QTreeWidgetItem* node, const QVector<QString>& linarPathArray) const;
    bool renameInTree(const QString& rootPath, QTreeWidgetItem* node, const QVector<QString>& linarPathArrayFrom, const QString& renameTo);
    QVector<QString> getRoots() const;
    QString getSelectedItemPath() const;
    bool selectItemFromPath(const QString& itemPath) const;

public slots:
    void onLocateCurrentFile();
    void onFoldAll();
    void onExpandAll();
    void onAddRoot();
    void onRemoveAllRoots();
    void onRemoveRootFolder();
    void onCopyPath();
    void onCopyFileName();
    void onFindInFiles();
    void onExplorerHere();
    void onCmdHere();
    void onOpenInNpp();
    void onShellExecute();
    void onContextMenuRequested(const QPoint& pos);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onItemSelectionChanged();
    void onItemMoved(QTreeWidgetItem* item, QTreeWidgetItem* oldParent, QTreeWidgetItem* newParent);
    void onDirectoryChanged(const QString& path);
    void onCloseRequested();

signals:
    void closeRequested();
    void fileActivated(const QString& path);
    void rootFolderAdded(const QString& path);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    // Popup menus
    void initPopupMenus();
    void destroyMenus();
    void showContextMenu(QTreeWidgetItem* item, const QPoint& pos);
    void openSelectFile();

    // Directory operations
    void getDirectoryStructure(const QString& dir, const QVector<QString>& patterns,
                               FolderInfo& directoryStructure, bool isRecursive, bool isInHiddenDir);
    QTreeWidgetItem* createFolderItemsFromDirStruct(QTreeWidgetItem* hParentItem, const FolderInfo& directoryStructure);

    // Internal helpers
    BrowserNodeType getNodeType(QTreeWidgetItem* hItem);
    void deleteAllFromTree();
    QVector<QString> split(const QString& string2split, QChar sep) const;
    bool isRelatedRootFolder(const QString& relatedRoot, const QString& subFolder) const;
    void customTreeSort(QTreeWidgetItem* parent, int column, bool recursive);
    void setFolderIcon(QTreeWidgetItem* item, bool isOpen);
    void setFileIcon(QTreeWidgetItem* item, const QString& fileName);

    // Toolbar
    QToolBar* _toolbar = nullptr;
    QAction* _locateAction = nullptr;
    QAction* _foldAllAction = nullptr;
    QAction* _expandAllAction = nullptr;

    // Tree
    FileBrowserTreeWidget _treeView;

    // Menus
    QMenu* _globalMenu = nullptr;
    QMenu* _rootMenu = nullptr;
    QMenu* _folderMenu = nullptr;
    QMenu* _fileMenu = nullptr;

    // Folder watchers
    QVector<FolderWatcher*> _folderWatchers;

    // Parent window
    QWidget* _hParent = nullptr;

    // Localization
    QString _expandAllFolders = QStringLiteral("Unfold all");
    QString _collapseAllFolders = QStringLiteral("Fold all");
    QString _locateCurrentFile = QStringLiteral("Locate current file");

    // Drag tracking
    QString _selectedNodeFullPath;

    // Sorting data (lParam equivalents — must be deleted on destruction)
    QVector<SortingData*> _sortingDataArray;
};
