// Semantic Lift: Win32 FileBrowser → Qt6 Folder as Workspace Panel
// Original: PowerEditor/src/WinControls/FileBrowser/fileBrowser.h
// Target: npp-qt/src/WinControls/FileBrowser.h
//
// Translates Win32 TreeView + Shell API → Qt6 QTreeWidget + QFileSystemWatcher.
// Preserves original class names and API surface where possible.

#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileIconProvider>
#include <QString>
#include <QVector>
#include <QMap>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QApplication>
#include <QSet>
#include <QVariant>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QFileDialog>
#include "DockingWnd.h"

// =============================================================================
// Win32 → Qt6 translation constants
// =============================================================================

// Toolbar command IDs (mirrors Win32 FB_CMD_*)
const int FB_CMD_AIMFILE    = 1;
const int FB_CMD_FOLDALL    = 2;
const int FB_CMD_EXPANDALL  = 3;

// Context menu command IDs (mirrors Win32 IDM_FILEBROWSER_* from fileBrowser_rc.h)
const int IDM_FILEBROWSER_REMOVEROOTFOLDER = 1;
const int IDM_FILEBROWSER_REMOVEALLROOTS   = 2;
const int IDM_FILEBROWSER_ADDROOT          = 3;
const int IDM_FILEBROWSER_SHELLEXECUTE     = 4;
const int IDM_FILEBROWSER_OPENINNPP        = 5;
const int IDM_FILEBROWSER_COPYPATH         = 6;
const int IDM_FILEBROWSER_FINDINFILES      = 7;
const int IDM_FILEBROWSER_EXPLORERHERE     = 8;
const int IDM_FILEBROWSER_CMDHERE          = 9;
const int IDM_FILEBROWSER_COPYFILENAME     = 10;

// Custom registered messages (mirrors FB_ADDFILE, FB_RMFILE, FB_RNFILE)
const int FB_ADDFILE = QEvent::User + 1024;
const int FB_RMFILE  = QEvent::User + 1025;
const int FB_RNFILE  = QEvent::User + 1026;

// Item image indices (mirrors Win32 ItemIdx enum)
enum FBItemIdx {
    INDEX_ROOT_OPEN   = 0,
    INDEX_ROOT_CLOSE  = 1,
    INDEX_FOLDER_OPEN = 2,
    INDEX_FOLDER_CLOSE= 3,
    INDEX_LEAF        = 4
};

// Sort role for custom comparator (Qt has no built-in custom sort)
const int SortRole = Qt::UserRole;

// =============================================================================
// SortingData — lParam / custom data attached to each tree item
// Mirrors Win32 SortingData4lParam
// =============================================================================

struct SortingData {
    QString _rootPath;   // Only set for root nodes; empty for folder/file
    QString _label;       // Item display label
    bool    _isFolder = false;

    SortingData(const QString& rootPath, const QString& label, bool isFolder)
        : _rootPath(rootPath), _label(label), _isFolder(isFolder) {}
};

// =============================================================================
// FileBrowserTreeWidgetItem — QTreeWidgetItem with custom sort data
// Mirrors Win32 tree item with TVITEM.lParam
// =============================================================================

class FileBrowserTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit FileBrowserTreeWidgetItem(QTreeWidget* parent = nullptr)
        : QTreeWidgetItem(parent, Type) {}

    explicit FileBrowserTreeWidgetItem(QTreeWidgetItem* parent = nullptr)
        : QTreeWidgetItem(parent, Type) {}

    SortingData* sortData = nullptr;  // owned by FileBrowser::_sortingDataArray
};

// =============================================================================
// FileInfo / FolderInfo — in-memory model of a directory tree
// Mirrors Win32 FileInfo / FolderInfo
// =============================================================================

class FileInfo final
{
    friend class FileBrowser;
    friend class FolderInfo;

public:
    FileInfo() = delete;
    explicit FileInfo(const QString& name) noexcept : _name(name) {}
    const QString& getName() const { return _name; }
    void setName(const QString& name) { _name = name; }

private:
    QString _name;
};

class FolderInfo final
{
    friend class FileBrowser;
    friend class FolderUpdater;

public:
    FolderInfo() = delete;
    FolderInfo(const QString& name, FolderInfo* parent) : _name(name), _parent(parent) {}
    void setRootPath(const QString& rootPath) { _rootPath = rootPath; }
    const QString& getRootPath() const { return _rootPath; }
    void setName(const QString& name) { _name = name; }
    const QString& getName() const { return _name; }
    void addFile(const QString& fn) { _files.push_back(FileInfo(fn)); }
    void addSubFolder(FolderInfo subDirectoryStructure) { _subFolders.push_back(subDirectoryStructure); }

    bool addToStructure(QString& fullpath, const QVector<QString>& linarPathArray);
    bool removeFromStructure(const QVector<QString>& linarPathArray);
    bool renameInStructure(const QVector<QString>& linarPathArrayFrom,
                           const QVector<QString>& linarPathArrayTo);

    // Accessors
    FolderInfo* parent() const { return _parent; }
    QVector<FolderInfo>& subFolders()       { return _subFolders; }
    const QVector<FolderInfo>& subFolders() const { return _subFolders; }
    QVector<FileInfo>& files()               { return _files; }
    const QVector<FileInfo>& files() const  { return _files; }

private:
    QVector<FolderInfo> _subFolders;
    QVector<FileInfo>   _files;
    QString _name;
    FolderInfo* _parent = nullptr;
    QString _rootPath;  // set only for root; empty for normal folders
};

// =============================================================================
// FolderUpdater — watches a directory for file system changes
// Mirrors Win32 FolderUpdater (uses QFileSystemWatcher instead of
// ReadDirectoryChanges + thread).
// =============================================================================

class FolderUpdater : public QObject
{
    Q_OBJECT

public:
    FolderUpdater(const FolderInfo& fi, class FileBrowser* pFileBrowser);
    virtual ~FolderUpdater();

    void startWatcher();
    void stopWatcher();

    FolderInfo _rootFolder;
    class FileBrowser* _pFileBrowser = nullptr;

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
// BrowserNodeType — tree node type
// Mirrors Win32 BrowserNodeType enum
// =============================================================================

enum BrowserNodeType {
    browserNodeType_root   = 0,
    browserNodeType_folder = 2,
    browserNodeType_file  = 3
};

// =============================================================================
// QFileBrowserEvent — custom QEvent for async file system notifications
// Bridges FolderUpdater → FileBrowser (mirrors Win32 FB_ADDFILE/FB_RMFILE/FB_RNFILE)
// =============================================================================

class QFileBrowserEvent : public QEvent
{
public:
    explicit QFileBrowserEvent(int message, const QVariant& data = QVariant());

    int message() const { return _message; }
    QVariant data() const { return _data; }

private:
    static QEvent::Type fbEventType(int message);

    int _message = 0;
    QVariant _data;
};

// =============================================================================
// FileBrowserTreeWidget — QTreeWidget with Win32 TreeView semantics
// =============================================================================

class FileBrowserTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit FileBrowserTreeWidget(QWidget* parent = nullptr);
    ~FileBrowserTreeWidget() override = default;

    // Drag & drop tracking (mirrors Win32 _treeView drag state)
    bool isDragging() const { return _isDragging; }
    void beginDrag(QTreeWidgetItem* item);
    void dragItem(const QPoint& pos);
    bool dropItem();

    // Drop restrictions
    void addCanNotDropInList(int index);
    void addCanNotDragOutList(int index);
    bool isDroppable(QTreeWidgetItem* item) const;
    bool isDraggable(QTreeWidgetItem* item) const;

    // Item data
    void* getItemParam(QTreeWidgetItem* item) const;
    void setItemParam(QTreeWidgetItem* item, void* param);

    // Item removal
    void removeItem(QTreeWidgetItem* item);

    // Navigation helpers (mirrors Win32 _treeView methods)
    QTreeWidgetItem* getRoot() const;
    QTreeWidgetItem* getChildFrom(QTreeWidgetItem* parent) const;
    QTreeWidgetItem* getNextSibling(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getParent(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getSelection() const;
    QString itemDisplayName(QTreeWidgetItem* item) const;

    // Expand/collapse (mirrors TreeView_* macros)
    void expandAll();
    void foldAll();
    void toggleExpandCollapse(QTreeWidgetItem* item);

    // Editable labels
    void makeLabelEditable(bool editable);

    // Sorting
    void customSorting(QTreeWidgetItem* parent, bool recursive);

    // Icon
    void setItemIcon(QTreeWidgetItem* item, int imageIndex);

signals:
    void itemMoved(QTreeWidgetItem* item, QTreeWidgetItem* oldParent, QTreeWidgetItem* newParent);
    void fileDropped(const QString& path);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    bool _isDragging = false;
    QTreeWidgetItem* _dragItem = nullptr;
    QPoint _dragStartPos;
    QSet<int> _cannotDropList;
    QSet<int> _cannotDragList;

    static int folderSortFunc(const QTreeWidgetItem* a, const QTreeWidgetItem* b);
};

// =============================================================================
// FilesToChange — batch file change info (Win32 FB_ADDFILE/FB_RMFILE/FB_RNFILE)
// =============================================================================

struct FilesToChange {
    QString _commonPath;
    QString _rootPath;
    QVector<QString> _linarWithoutLastPathElement;
    QVector<QString> _files;
};

// =============================================================================
// FileBrowser — "Folder as Workspace" docking panel
// Mirrors Win32 FileBrowser: toolbar + TreeView + context menus + file watching
// =============================================================================

class FileBrowser : public DockingDlgInterface
{
    Q_OBJECT

public:
    FileBrowser();
    ~FileBrowser() override;

    // DockingDlgInterface message handler (mirrors Win32 CALLBACK run_dlgProc)
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    // Compatibility shims
    void setBackgroundColor(const QColor& col);
    void setForegroundColor(const QColor& col);

    // Node queries
    QString getNodePath(QTreeWidgetItem* node) const;
    QString getNodeName(QTreeWidgetItem* node) const;
    BrowserNodeType getNodeType(QTreeWidgetItem* hItem);
    QString getSelectedItemPath() const;
    QVector<QString> getRoots() const;

    // Root management
    void addRootFolder(const QString& rootFolderPath);
    void deleteAllFromTree();

    // Tree navigation
    QTreeWidgetItem* getRootFromFullPath(const QString& rootPath) const;
    QTreeWidgetItem* findChildNodeFromName(QTreeWidgetItem* parent, const QString& label) const;
    QTreeWidgetItem* findInTree(const QString& rootPath, QTreeWidgetItem* node,
                                const QVector<QString>& linarPathArray) const;

    // Item mutation
    bool renameInTree(const QString& rootPath, QTreeWidgetItem* node,
                      const QVector<QString>& linarPathArrayFrom,
                      const QString& renameTo);

    // Path selection
    bool selectItemFromPath(const QString& itemPath) const;
    bool selectCurrentEditingFile() const;

    // Custom events (Win32 registered messages)
    bool handleAddFile(LPARAM lParam);
    bool handleRmFile(LPARAM lParam);
    bool handleRenameFile(LPARAM lParam);

    // Accessors
    FileBrowserTreeWidget& treeView() { return _treeView; }

signals:
    void closeRequested();
    void fileActivated(const QString& path);
    void rootFolderAdded(const QString& path);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    // Menus
    void initPopupMenus();
    void destroyMenus();
    void showContextMenu(const QPoint& pos);
    void popupMenuCmd(int cmdID);

    // Tree operations
    void getDirectoryStructure(const QString& dir,
                               const QVector<QString>& patterns,
                               FolderInfo& directoryStructure,
                               bool isRecursive,
                               bool isInHiddenDir);
    QTreeWidgetItem* createFolderItemsFromDirStruct(QTreeWidgetItem* hParentItem,
                                                     const FolderInfo& directoryStructure);

    // File batch helpers (Win32 getFilesFromParam)
    QVector<FilesToChange> getFilesFromParam(LPARAM lParam) const;

    // addToTree / deleteFromTree (Win32 methods)
    bool addToTree(FilesToChange& group, QTreeWidgetItem* node);
    bool deleteFromTree(FilesToChange& group);
    QVector<QTreeWidgetItem*> findInTree(FilesToChange& group, QTreeWidgetItem* node) const;
    QVector<QTreeWidgetItem*> findChildNodesFromNames(QTreeWidgetItem* parent,
                                                       QVector<QString>& labels) const;
    void removeNamesAlreadyInNode(QTreeWidgetItem* parent, QVector<QString>& labels) const;

    // Sorting
    static int categorySortFunc(const SortingData* a, const SortingData* b);
    void customTreeSort(QTreeWidgetItem* parent, bool recursive);

    // Icon helpers
    void setFolderIcon(QTreeWidgetItem* item, bool isOpen);
    void setFileIcon(QTreeWidgetItem* item, const QString& fileName);

    // Open file in Notepad++
    void openSelectFile();

    // String helpers
    QVector<QString> split(const QString& string2split, QChar sep) const;
    bool isRelatedRootFolder(const QString& relatedRoot, const QString& subFolder) const;

    // Toolbar
    QToolBar* _toolbar = nullptr;
    QAction* _locateAction = nullptr;
    QAction* _foldAllAction = nullptr;
    QAction* _expandAllAction = nullptr;
    QVector<QIcon> _iconListNormal;
    QVector<QIcon> _iconListDark;

    // Tree
    FileBrowserTreeWidget _treeView;

    // Menus
    QMenu* _globalMenu = nullptr;
    QMenu* _rootMenu = nullptr;
    QMenu* _folderMenu = nullptr;
    QMenu* _fileMenu = nullptr;

    // File watchers
    QVector<FolderUpdater*> _folderUpdaters;

    // Localization strings
    QString _expandAllFolders = QStringLiteral("Unfold all");
    QString _collapseAllFolders = QStringLiteral("Fold all");
    QString _locateCurrentFile = QStringLiteral("Locate current file");

    // Selected node path (used for async open)
    QString _selectedNodeFullPath;

    // Sorting data — owned here, must be deleted in destructor
    QVector<SortingData*> _sortingDataArray;
};
