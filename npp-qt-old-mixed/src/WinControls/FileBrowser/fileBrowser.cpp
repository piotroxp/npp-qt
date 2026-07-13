
// Semantic Lift: Win32 FileBrowser → Qt6 Folder as Workspace Panel
// Original: PowerEditor/src/WinControls/FileBrowser/fileBrowser.cpp
// Target: npp-qt/src/WinControls/FileBrowser/fileBrowser.cpp
//
// Translates Win32 TreeView + Shell API → Qt6 QTreeWidget + QFileSystemWatcher.
// Preserves original class names and API surface where possible.

#include "FileBrowser.h"

// Default constructor
FileBrowser::FileBrowser() : DockingDlgInterface(0) {}


#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QHeaderView>
#include <QFileSystemWatcher>
#include <QFileIconProvider>
#include <QClipboard>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QVariant>
#include <QFileDialog>
#include <QFontMetrics>
#include <QAbstractItemView>
#include <QStyledItemDelegate>

#include "NppDarkMode.h"
#include "Parameters.h"
#include "Common.h"
#include "menuCmdID.h"
#include "NppConstants.h"
#include "MISC/Common/localization.h"

// =============================================================================
// Win32 macro shims for Linux build
// =============================================================================
#ifndef LOWORD
#define LOWORD(x)  (int)((uintptr_t)(x) & 0xFFFF)
#endif
#ifndef HIWORD
#define HIWORD(x)  (int)(((uintptr_t)(x) >> 16) & 0xFFFF)
#endif

// =============================================================================
// Helper: split string by separator (mirrors Win32 split)
// =============================================================================
QVector<QString> FileBrowser::split(const QString& string2split, QChar sep) const
{
    QVector<QString> splittedStrings;
    const int len = string2split.length();
    int beginPos = 0;
    for (int i = 0; i < len + 1; ++i)
    {
        if (i == len || string2split[i] == sep)
        {
            splittedStrings.push_back(string2split.mid(beginPos, i - beginPos));
            beginPos = i + 1;
        }
    }
    return splittedStrings;
}

// =============================================================================
// Helper: check if subFolder is related to relatedRoot (mirrors Win32 isRelatedRootFolder)
// =============================================================================
bool FileBrowser::isRelatedRootFolder(const QString& relatedRoot, const QString& subFolder) const
{
    if (relatedRoot.isEmpty() || subFolder.isEmpty())
        return false;

    if (!subFolder.startsWith(relatedRoot))
        return false;

    QVector<QString> relatedRootArray = split(relatedRoot, QDir::separator());
    QVector<QString> subFolderArray   = split(subFolder,   QDir::separator());

    if (relatedRootArray.isEmpty() || subFolderArray.isEmpty())
        return false;

    size_t index2Compare = relatedRootArray.size() - 1;
    return index2Compare < subFolderArray.size()
        && relatedRootArray[index2Compare] == subFolderArray[index2Compare];
}

// =============================================================================
// Destructor
// =============================================================================
FileBrowser::~FileBrowser()
{
    for (const auto folder : _folderUpdaters)
    {
        folder->stopWatcher();
        delete folder;
    }

    for (const auto cd : _sortingDataArray)
        delete cd;

    destroyMenus();
}

// =============================================================================
// Init popup menus — mirrors Win32 initPopupMenus
// =============================================================================
void FileBrowser::initPopupMenus()
{
    NativeLangSpeaker* pNativeSpeaker = NppParameters::getInstance().getNativeLangSpeaker();

    auto lit = [](const char* s) { return QString::fromLatin1(s); };

    QString addRoot       = pNativeSpeaker->getAttrNameStr(lit("Add"),            "FolderAsWorkspace", "Add");
    QString removeAllRoot = pNativeSpeaker->getAttrNameStr(lit("Remove All"),    "FolderAsWorkspace", "RemoveAll");
    QString removeRoot    = pNativeSpeaker->getAttrNameStr(lit("Remove"),        "FolderAsWorkspace", "Remove");
    QString copyPath      = pNativeSpeaker->getAttrNameStr(lit("Copy path"),     "FolderAsWorkspace", "CopyPath");
    QString copyFileName  = pNativeSpeaker->getAttrNameStr(lit("Copy file name"),"FolderAsWorkspace", "CopyFileName");
    QString findInFiles   = pNativeSpeaker->getAttrNameStr(lit("Find in Files..."), "FolderAsWorkspace", "FindInFiles");
    QString explorerHere  = pNativeSpeaker->getAttrNameStr(lit("Explorer here"), "FolderAsWorkspace", "ExplorerHere");
    QString cmdHere       = pNativeSpeaker->getAttrNameStr(lit("CMD here"),      "FolderAsWorkspace", "CmdHere");
    QString openInNpp     = pNativeSpeaker->getAttrNameStr(lit("Open"),          "FolderAsWorkspace", "OpenInNpp");
    QString shellExecute  = pNativeSpeaker->getAttrNameStr(lit("Run by system"), "FolderAsWorkspace", "ShellExecute");

    // Fallbacks
    if (addRoot.isEmpty())       addRoot       = lit("Add");
    if (removeAllRoot.isEmpty()) removeAllRoot = lit("Remove All");
    if (removeRoot.isEmpty())    removeRoot    = lit("Remove");
    if (copyPath.isEmpty())      copyPath      = lit("Copy path");
    if (copyFileName.isEmpty())  copyFileName  = lit("Copy file name");
    if (findInFiles.isEmpty())   findInFiles   = lit("Find in Files...");
    if (explorerHere.isEmpty()) explorerHere  = lit("Explorer here");
    if (cmdHere.isEmpty())       cmdHere       = lit("CMD here");
    if (openInNpp.isEmpty())     openInNpp     = lit("Open");
    if (shellExecute.isEmpty()) shellExecute  = lit("Run by system");

    // Global menu
    _globalMenu = new QMenu(this);
    QAction* a1 = _globalMenu->addAction(addRoot);
    a1->setData(IDM_FILEBROWSER_ADDROOT);
    QAction* a2 = _globalMenu->addAction(removeAllRoot);
    a2->setData(IDM_FILEBROWSER_REMOVEALLROOTS);

    // Root folder menu
    _rootMenu = new QMenu(this);
    _rootMenu->addAction(removeRoot)->setData(IDM_FILEBROWSER_REMOVEROOTFOLDER);
    _rootMenu->addSeparator();
    _rootMenu->addAction(copyPath)->setData(IDM_FILEBROWSER_COPYPATH);
    _rootMenu->addAction(findInFiles)->setData(IDM_FILEBROWSER_FINDINFILES);
    _rootMenu->addSeparator();
    _rootMenu->addAction(explorerHere)->setData(IDM_FILEBROWSER_EXPLORERHERE);
    _rootMenu->addAction(cmdHere)->setData(IDM_FILEBROWSER_CMDHERE);

    // Folder menu
    _folderMenu = new QMenu(this);
    _folderMenu->addAction(copyPath)->setData(IDM_FILEBROWSER_COPYPATH);
    _folderMenu->addAction(findInFiles)->setData(IDM_FILEBROWSER_FINDINFILES);
    _folderMenu->addSeparator();
    _folderMenu->addAction(explorerHere)->setData(IDM_FILEBROWSER_EXPLORERHERE);
    _folderMenu->addAction(cmdHere)->setData(IDM_FILEBROWSER_CMDHERE);

    // File menu
    _fileMenu = new QMenu(this);
    _fileMenu->addAction(openInNpp)->setData(IDM_FILEBROWSER_OPENINNPP);
    _fileMenu->addSeparator();
    _fileMenu->addAction(copyPath)->setData(IDM_FILEBROWSER_COPYPATH);
    _fileMenu->addAction(copyFileName)->setData(IDM_FILEBROWSER_COPYFILENAME);
    _fileMenu->addAction(shellExecute)->setData(IDM_FILEBROWSER_SHELLEXECUTE);
    _fileMenu->addSeparator();
    _fileMenu->addAction(explorerHere)->setData(IDM_FILEBROWSER_EXPLORERHERE);
    _fileMenu->addAction(cmdHere)->setData(IDM_FILEBROWSER_CMDHERE);
}

// =============================================================================
// Destroy menus
// =============================================================================
void FileBrowser::destroyMenus()
{
    delete _globalMenu;  _globalMenu  = nullptr;
    delete _rootMenu;    _rootMenu    = nullptr;
    delete _folderMenu;  _folderMenu = nullptr;
    delete _fileMenu;    _fileMenu   = nullptr;
}

// =============================================================================
// Main dialog procedure — mirrors Win32 run_dlgProc
// =============================================================================
intptr_t FileBrowser::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            // ---- Toolbar ----
            _toolbar = new QToolBar(this);
            _toolbar->setMovable(false);
            _toolbar->setFloatable(false);
            _toolbar->setIconSize(QSize(16, 16));
            _toolbar->setContentsMargins(2, 2, 2, 2);
            _toolbar->setStyleSheet(QStringLiteral(
                "QToolBar { border: none; background: transparent; spacing: 2px; padding: 2px; }"
                "QToolButton { background: transparent; border: none; }"
            ));

            _locateAction    = _toolbar->addAction(_locateCurrentFile);
            _foldAllAction   = _toolbar->addAction(_collapseAllFolders);
            _expandAllAction = _toolbar->addAction(_expandAllFolders);

            _locateAction->setObjectName(QString::number(FB_CMD_AIMFILE));
            _foldAllAction->setObjectName(QString::number(FB_CMD_FOLDALL));
            _expandAllAction->setObjectName(QString::number(FB_CMD_EXPANDALL));

            connect(_locateAction,    &QAction::triggered, this, [this]() { selectCurrentEditingFile(); });
            connect(_foldAllAction,   &QAction::triggered, this, [this]() { _treeView.foldAll(); });
            connect(_expandAllAction, &QAction::triggered, this, [this]() { _treeView.expandAll(); });

            connect(_toolbar, &QToolBar::actionTriggered, this, [this](QAction* action) {
                bool ok = false;
                int cmdId = action->objectName().toInt(&ok);
                if (ok)
                    popupMenuCmd(cmdId);
            });

            // ---- Tree view ----
            _treeView.setParent(this);
            _treeView.setHeaderHidden(true);
            _treeView.setAcceptDrops(true);
            _treeView.setDragEnabled(true);
            _treeView.setDropIndicatorShown(true);
            _treeView.setSelectionMode(QAbstractItemView::SingleSelection);
            _treeView.setEditTriggers(QAbstractItemView::NoEditTriggers);
            _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
            _treeView.setItemDelegate(new QStyledItemDelegate(this));

            if (NppDarkMode::isEnabled_Static())
                _treeView.setStyleSheet(QString());

            // Restrict drop targets
            _treeView.addCanNotDropInList(INDEX_ROOT_OPEN);
            _treeView.addCanNotDropInList(INDEX_ROOT_CLOSE);
            _treeView.addCanNotDropInList(INDEX_FOLDER_OPEN);
            _treeView.addCanNotDropInList(INDEX_FOLDER_CLOSE);
            _treeView.addCanNotDropInList(INDEX_LEAF);

            // Restrict drag-out
            _treeView.addCanNotDragOutList(INDEX_ROOT_OPEN);
            _treeView.addCanNotDragOutList(INDEX_ROOT_CLOSE);
            _treeView.addCanNotDragOutList(INDEX_FOLDER_OPEN);
            _treeView.addCanNotDragOutList(INDEX_FOLDER_CLOSE);
            _treeView.addCanNotDragOutList(INDEX_LEAF);

            // Double-click to open
            connect(&_treeView, &QTreeWidget::itemDoubleClicked,
                    this, &FileBrowser::openSelectFile);

            // Expand/collapse icons
            connect(&_treeView, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item) {
                BrowserNodeType nType = getNodeType(item);
                if (nType == browserNodeType_folder || nType == browserNodeType_root)
                    setFolderIcon(item, true);
            });

            connect(&_treeView, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem* item) {
                BrowserNodeType nType = getNodeType(item);
                if (nType == browserNodeType_folder || nType == browserNodeType_root)
                    setFolderIcon(item, false);
            });

            // Context menu
            connect(&_treeView, &QTreeWidget::customContextMenuRequested,
                    this, [this](const QPoint& pos) { showContextMenu(pos); });

            // Drag-and-drop signals
            connect(&_treeView, &FileBrowserTreeWidget::itemMoved,
                    this, [](QTreeWidgetItem*, QTreeWidgetItem*, QTreeWidgetItem*) {});

            connect(&_treeView, &FileBrowserTreeWidget::fileDropped,
                    this, [](const QString&) {});

            initPopupMenus();
            return TRUE;
        }

        case NPPM_INTERNAL_REFRESHDARKMODE:
        {
            _treeView.setStyleSheet(QString());
            return TRUE;
        }

        case WM_SIZE:
        {
            int width  = LOWORD(wParam);
            int height = HIWORD(wParam);
            const int tbHeight = 24;
            _toolbar->setGeometry(0, 0, width, tbHeight);
            _treeView.setGeometry(0, tbHeight + 4, width, height - tbHeight - 4);
            break;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case FB_CMD_AIMFILE:
                    selectCurrentEditingFile();
                    break;
                case FB_CMD_FOLDALL:
                    _treeView.foldAll();
                    break;
                case FB_CMD_EXPANDALL:
                    _treeView.expandAll();
                    break;
                default:
                    popupMenuCmd(LOWORD(wParam));
                    break;
            }
            break;
        }

        case WM_DESTROY:
        {
            destroyMenus();
            delete _toolbar; _toolbar = nullptr;
            break;
        }

        case FB_ADDFILE:
        {
            QVector<FilesToChange> groupedFiles = getFilesFromParam(static_cast<LPARAM>(lParam));
            for (auto& group : groupedFiles)
                addToTree(group, nullptr);
            break;
        }

        case FB_RMFILE:
        {
            QVector<FilesToChange> groupedFiles = getFilesFromParam(static_cast<LPARAM>(lParam));
            for (auto& group : groupedFiles)
                deleteFromTree(group);
            break;
        }

        case FB_RNFILE:
        {
            const auto& file2Change = *reinterpret_cast<QVector<QString>*>(static_cast<LPARAM>(lParam));
            if (file2Change.size() < 2)
                return FALSE;

            const QString sep = QStringLiteral("\\\\");
            int sepPos = file2Change[0].indexOf(sep);
            if (sepPos < 0)
                return FALSE;

            QString pathSuffix = file2Change[0].mid(sepPos + sep.length());
            QVector<QString> linarPathArray = split(pathSuffix, QDir::separator());
            const QString rootPath = file2Change[0].left(sepPos);

            int sepPos2 = file2Change[1].indexOf(sep);
            if (sepPos2 < 0)
                return FALSE;

            QString pathSuffix2 = file2Change[1].mid(sepPos2 + sep.length());
            QVector<QString> linarPathArray2 = split(pathSuffix2, QDir::separator());
            const QString renameTo = linarPathArray2.isEmpty() ? QString() : linarPathArray2.last();

            renameInTree(rootPath, nullptr, linarPathArray, renameTo);
            break;
        }

        default:
            return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
    }

    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

// =============================================================================
// Event filter — keyboard handling (Return, Delete)
// =============================================================================
bool FileBrowser::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Return)
        {
            QTreeWidgetItem* item = _treeView.currentItem();
            if (item)
            {
                BrowserNodeType nType = getNodeType(item);
                if (nType == browserNodeType_file)
                    openSelectFile();
                else
                    _treeView.toggleExpandCollapse(item);
                return true;
            }
        }
        else if (ke->key() == Qt::Key_Delete)
        {
            QTreeWidgetItem* item = _treeView.currentItem();
            if (item && getNodeType(item) == browserNodeType_root)
            {
                popupMenuCmd(IDM_FILEBROWSER_REMOVEROOTFOLDER);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

// =============================================================================
// Resize event
// =============================================================================
void FileBrowser::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int width  = event->size().width();
    int height = event->size().height();
    const int tbHeight = (_toolbar ? _toolbar->height() : 24);
    if (_toolbar)
        _toolbar->setGeometry(0, 0, width, tbHeight);
    _treeView.setGeometry(0, tbHeight + 4, width, height - tbHeight - 4);
}

// =============================================================================
// Context menu — mirrors Win32 showContextMenu
// =============================================================================
void FileBrowser::showContextMenu(const QPoint& pos)
{
    if (_treeView.isDragging())
        return;

    QTreeWidgetItem* clickedItem = _treeView.itemAt(pos);
    QMenu* menuToShow = nullptr;

    if (!clickedItem)
        menuToShow = _globalMenu;
    else
    {
        _treeView.setCurrentItem(clickedItem);
        BrowserNodeType nType = getNodeType(clickedItem);
        if (nType == browserNodeType_root)
            menuToShow = _rootMenu;
        else if (nType == browserNodeType_folder)
            menuToShow = _folderMenu;
        else
            menuToShow = _fileMenu;
    }

    if (menuToShow)
    {
        const QPoint globalPos = _treeView.mapToGlobal(pos);
        QAction* triggered = menuToShow->exec(globalPos);
        if (triggered)
        {
            int cmdId = triggered->data().toInt();
            if (cmdId != 0)
                popupMenuCmd(cmdId);
        }
    }
}

// =============================================================================
// Menu command dispatcher — mirrors Win32 popupMenuCmd
// =============================================================================
void FileBrowser::popupMenuCmd(int cmdID)
{
    QTreeWidgetItem* selectedNode = _treeView.getSelection();

    switch (cmdID)
    {
        case IDM_FILEBROWSER_REMOVEROOTFOLDER:
        {
            if (!selectedNode)
                return;
            if (_treeView.getParent(selectedNode) != nullptr)
                return;

            FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(selectedNode);
            if (!fbItem || !fbItem->sortData)
                return;

            const QString rootPath = fbItem->sortData->_rootPath;
            for (int i = 0; i < _folderUpdaters.size(); ++i)
            {
                if (_folderUpdaters[i]->_rootFolder.getRootPath() == rootPath)
                {
                    _folderUpdaters[i]->stopWatcher();
                    delete _folderUpdaters[i];
                    _folderUpdaters.remove(i);
                    _treeView.removeItem(selectedNode);
                    break;
                }
            }
            break;
        }

        case IDM_FILEBROWSER_EXPLORERHERE:
        {
            if (!selectedNode)
                return;
            QString selPath = getNodePath(selectedNode);
            if (doesPathExist(selPath))
            {
                QFileInfo fi(selPath);
                QString pathToOpen = fi.isDir() ? selPath : fi.absolutePath();
                QDesktopServices::openUrl(QUrl::fromLocalFile(pathToOpen));
            }
            break;
        }

        case IDM_FILEBROWSER_CMDHERE:
        {
            if (!selectedNode)
                return;
            BrowserNodeType nType = getNodeType(selectedNode);
            if (nType == browserNodeType_file)
            {
                QTreeWidgetItem* parent = _treeView.getParent(selectedNode);
                if (parent)
                    selectedNode = parent;
            }
            QString path = getNodePath(selectedNode);
            if (doesPathExist(path))
            {
                QStringList args;
                args << QStringLiteral("/K") << QStringLiteral("cd /d \"") + path + QStringLiteral("\"");
                QProcess::startDetached(QStringLiteral("cmd.exe"), args);
            }
            break;
        }

        case IDM_FILEBROWSER_COPYPATH:
        {
            if (!selectedNode)
                return;
            str2Clipboard(getNodePath(selectedNode));
            break;
        }

        case IDM_FILEBROWSER_COPYFILENAME:
        {
            if (!selectedNode)
                return;
            str2Clipboard(getNodeName(selectedNode));
            break;
        }

        case IDM_FILEBROWSER_FINDINFILES:
        {
            if (!selectedNode)
                return;
            emit fileActivated(getNodePath(selectedNode));
            break;
        }

        case IDM_FILEBROWSER_OPENINNPP:
            openSelectFile();
            break;

        case IDM_FILEBROWSER_REMOVEALLROOTS:
        {
            for (int i = static_cast<int>(_folderUpdaters.size()) - 1; i >= 0; --i)
            {
                _folderUpdaters[i]->stopWatcher();
                QTreeWidgetItem* root = getRootFromFullPath(_folderUpdaters[i]->_rootFolder.getRootPath());
                if (root)
                    _treeView.removeItem(root);
                delete _folderUpdaters[i];
                _folderUpdaters.remove(i);
            }
            break;
        }

        case IDM_FILEBROWSER_ADDROOT:
        {
            QString folderPath = QFileDialog::getExistingDirectory(
                this,
                QStringLiteral("Select a folder to add in Folder as Workspace panel"),
                QString(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (!folderPath.isEmpty())
                addRootFolder(folderPath);
            break;
        }

        case IDM_FILEBROWSER_SHELLEXECUTE:
        {
            if (!selectedNode)
                return;
            QString path = getNodePath(selectedNode);
            if (doesPathExist(path))
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            break;
        }
    }
}

// =============================================================================
// Open the currently selected file
// =============================================================================
void FileBrowser::openSelectFile()
{
    QTreeWidgetItem* selectedNode = _treeView.getSelection();
    if (!selectedNode)
        return;

    _selectedNodeFullPath = getNodePath(selectedNode);
    if (!doesFileExist(_selectedNodeFullPath))
        return;

    emit fileActivated(_selectedNodeFullPath);
}

// =============================================================================
// Get full path from a tree item (mirrors Win32 getNodePath)
// =============================================================================
QString FileBrowser::getNodePath(QTreeWidgetItem* node) const
{
    if (!node)
        return QString();

    QStringList pathParts;
    QTreeWidgetItem* current = node;

    while (current != nullptr)
    {
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(current);
        if (fbItem && fbItem->sortData && !fbItem->sortData->_rootPath.isEmpty())
        {
            // Root node — its rootPath IS the full path
            pathParts.prepend(fbItem->sortData->_rootPath);
            break;
        }

        pathParts.prepend(current->text(0));
        current = _treeView.getParent(current);
    }

    return pathParts.join(QDir::separator());
}

// =============================================================================
// Get display name of a tree item (mirrors Win32 getNodeName)
// =============================================================================
QString FileBrowser::getNodeName(QTreeWidgetItem* node) const
{
    return node ? node->text(0) : QString();
}

// =============================================================================
// Determine node type (mirrors Win32 getNodeType)
// =============================================================================
BrowserNodeType FileBrowser::getNodeType(QTreeWidgetItem* hItem)
{
    if (!hItem)
        return browserNodeType_file;

    // File — leaf icon
    if (hItem->data(0, Qt::UserRole + 1).toInt() == INDEX_LEAF)
        return browserNodeType_file;

    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(hItem);
    if (fbItem && fbItem->sortData && !fbItem->sortData->_rootPath.isEmpty())
        return browserNodeType_root;

    return browserNodeType_folder;
}

// =============================================================================
// Select a file/folder in the tree by path (mirrors Win32 selectItemFromPath)
// =============================================================================
bool FileBrowser::selectItemFromPath(const QString& itemPath)
{
    if (itemPath.isEmpty())
        return false;

    for (const auto f : _folderUpdaters)
    {
        if (isRelatedRootFolder(f->_rootFolder.getRootPath(), itemPath))
        {
            const QString rootPath = f->_rootFolder.getRootPath();
            QVector<QString> linarPathArray;

            if (rootPath.length() < itemPath.length())
            {
                QString pathSuffix = itemPath.mid(rootPath.length() + 1);
                linarPathArray = split(pathSuffix, QDir::separator());
            }

            QTreeWidgetItem* foundItem = findInTree(rootPath, nullptr, linarPathArray);
            if (foundItem)
            {
                _treeView.setCurrentItem(foundItem);
                _treeView.scrollToItem(foundItem);
                _treeView.setFocus();
                return true;
            }
        }
    }
    return false;
}

// =============================================================================
// Locate the current editing file (mirrors Win32 selectCurrentEditingFile)
// =============================================================================
bool FileBrowser::selectCurrentEditingFile()
{
    QString currentDocPath;
    return selectItemFromPath(currentDocPath);
}

// =============================================================================
// Add a root folder to the panel (mirrors Win32 addRootFolder)
// =============================================================================
void FileBrowser::addRootFolder(const QString& rootFolderPath)
{
    if (!doesDirectoryExist(rootFolderPath))
        return;

    // Strip trailing separator
    QString cleanPath = rootFolderPath;
    if (cleanPath.endsWith(QDir::separator()) || cleanPath.endsWith('/'))
        cleanPath.chop(1);

    // Check for duplicates
    for (const auto f : _folderUpdaters)
    {
        if (f->_rootFolder.getRootPath() == cleanPath)
            return;

        if (isRelatedRootFolder(f->_rootFolder.getRootPath(), cleanPath))
        {
            const QString rootPath = f->_rootFolder.getRootPath();
            const QString pathSuffix = cleanPath.mid(rootPath.length() + 1);
            QVector<QString> linarPathArray = split(pathSuffix, QDir::separator());
            QTreeWidgetItem* foundItem = findInTree(rootPath, nullptr, linarPathArray);
            if (foundItem)
                _treeView.setCurrentItem(foundItem);
            return;
        }

        if (isRelatedRootFolder(cleanPath, f->_rootFolder.getRootPath()))
        {
            QMessageBox::warning(this,
                QStringLiteral("Folder as Workspace"),
                QStringLiteral("A sub-folder of the folder you want to add exists.\n"
                              "Please remove its root from the panel before you add folder:\n%1")
                    .arg(cleanPath));
            return;
        }
    }

    // Build directory structure
    QString folderLabel = cleanPath;
    int sepIdx = folderLabel.lastIndexOf(QDir::separator());
    if (sepIdx >= 0)
        folderLabel = folderLabel.mid(sepIdx + 1);

    FolderInfo directoryStructure(folderLabel, nullptr);
    QStringList patterns;
    patterns << QStringLiteral("*");
    getDirectoryStructure(cleanPath, patterns, directoryStructure, true, false);

    QTreeWidgetItem* hRootItem = createFolderItemsFromDirStruct(nullptr, directoryStructure);
    customTreeSort(hRootItem, false);

    if (hRootItem)
        hRootItem->setExpanded(true);

    FolderUpdater* updater = new FolderUpdater(directoryStructure, this);
    _folderUpdaters.append(updater);
    updater->startWatcher();

    emit rootFolderAdded(cleanPath);
}

// =============================================================================
// Recursively scan a directory into a FolderInfo tree
// (mirrors Win32 getDirectoryStructure)
// =============================================================================
void FileBrowser::getDirectoryStructure(const QString& dir,
                                         const QVector<QString>& patterns,
                                         FolderInfo& directoryStructure,
                                         bool isRecursive,
                                         bool isInHiddenDir)
{
    if (directoryStructure.parent() == nullptr)
        directoryStructure.setRootPath(dir);

    QDir dirHandle(dir);
    if (!dirHandle.exists())
        return;

    const QFileInfoList entries = dirHandle.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot);

    for (const QFileInfo& fi : entries)
    {
        const QString name = fi.fileName();
        bool isHidden = name.startsWith('.');
        if (!isInHiddenDir && isHidden)
            continue;

        if (fi.isDir())
        {
            if (isRecursive)
            {
                FolderInfo subDir(name, &directoryStructure);
                getDirectoryStructure(fi.absoluteFilePath(), patterns, subDir, isRecursive, isInHiddenDir);
                directoryStructure.addSubFolder(subDir);
            }
        }
        else
        {
            if (matchInList(name, patterns))
                directoryStructure.addFile(name);
        }
    }
}

// =============================================================================
// Create tree items from a FolderInfo structure
// (mirrors Win32 createFolderItemsFromDirStruct)
// =============================================================================
QTreeWidgetItem* FileBrowser::createFolderItemsFromDirStruct(QTreeWidgetItem* hParentItem,
                                                              const FolderInfo& directoryStructure)
{
    QTreeWidgetItem* hFolderItem = nullptr;

    if (directoryStructure.parent() == nullptr && hParentItem == nullptr)
    {
        // Root item
        QString rootPath = directoryStructure.getRootPath();
        if (rootPath.endsWith(QDir::separator()))
            rootPath.chop(1);

        SortingData* customData = new SortingData(rootPath, QString(), true);
        _sortingDataArray.push_back(customData);

        FileBrowserTreeWidgetItem* item = new FileBrowserTreeWidgetItem(static_cast<QTreeWidget*>(nullptr));
        item->setText(0, directoryStructure.getName());
        item->sortData = customData;
        item->setData(0, Qt::UserRole + 1, INDEX_ROOT_CLOSE);
        item->setData(0, Qt::UserRole + 2, INDEX_ROOT_OPEN);
        setFolderIcon(item, false);

        _treeView.addTopLevelItem(item);
        hFolderItem = item;
    }
    else
    {
        // Sub-folder item
        SortingData* customData = new SortingData(QString(), directoryStructure.getName(), true);
        _sortingDataArray.push_back(customData);

        FileBrowserTreeWidgetItem* item = new FileBrowserTreeWidgetItem(hParentItem);
        item->setText(0, directoryStructure.getName());
        item->sortData = customData;
        item->setData(0, Qt::UserRole + 1, INDEX_FOLDER_CLOSE);
        item->setData(0, Qt::UserRole + 2, INDEX_FOLDER_OPEN);
        setFolderIcon(item, false);

        hFolderItem = item;
    }

    // Recurse into sub-folders
    for (const auto& folder : directoryStructure.subFolders())
        createFolderItemsFromDirStruct(hFolderItem, folder);

    // Add file items
    for (const auto& file : directoryStructure.files())
    {
        SortingData* customData = new SortingData(QString(), file.getName(), false);
        _sortingDataArray.push_back(customData);

        FileBrowserTreeWidgetItem* item = new FileBrowserTreeWidgetItem(hFolderItem);
        item->setText(0, file.getName());
        item->sortData = customData;
        item->setData(0, Qt::UserRole + 1, INDEX_LEAF);
        setFileIcon(item, file.getName());
    }

    // Collapse this node
    _treeView.collapseItem(hParentItem ? hParentItem : hFolderItem);

    return hFolderItem;
}

// =============================================================================
// Find root node by root path (mirrors Win32 getRootFromFullPath)
// =============================================================================
QTreeWidgetItem* FileBrowser::getRootFromFullPath(const QString& rootPath) const
{
    QTreeWidgetItem* node = nullptr;
    for (QTreeWidgetItem* hItemNode = _treeView.getRoot();
         hItemNode != nullptr && node == nullptr;
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(hItemNode);
        if (fbItem && fbItem->sortData && fbItem->sortData->_rootPath == rootPath)
            node = hItemNode;
    }
    return node;
}

// =============================================================================
// Find child node by name (mirrors Win32 findChildNodeFromName)
// =============================================================================
QTreeWidgetItem* FileBrowser::findChildNodeFromName(QTreeWidgetItem* parent, const QString& label) const
{
    for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(parent);
         hItemNode != nullptr;
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        if (hItemNode->text(0) == label)
            return hItemNode;
    }
    return nullptr;
}

// =============================================================================
// Get all root paths (mirrors Win32 getRoots)
// =============================================================================
QVector<QString> FileBrowser::getRoots() const
{
    QVector<QString> roots;
    for (QTreeWidgetItem* hItemNode = _treeView.getRoot();
         hItemNode != nullptr;
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(hItemNode);
        if (fbItem && fbItem->sortData)
            roots.push_back(fbItem->sortData->_rootPath);
    }
    return roots;
}

// =============================================================================
// Get selected item path (mirrors Win32 getSelectedItemPath)
// =============================================================================
QString FileBrowser::getSelectedItemPath() const
{
    QTreeWidgetItem* hItemNode = _treeView.getSelection();
    if (hItemNode)
        return getNodePath(hItemNode);
    return QString();
}

// =============================================================================
// Parse batch file-change params into grouped file sets
// (mirrors Win32 getFilesFromParam)
// =============================================================================
QVector<FilesToChange> FileBrowser::getFilesFromParam(intptr_t lParam) const
{
    const auto& filesToChange = *reinterpret_cast<QVector<QString>*>(lParam);
    const QString sep = QStringLiteral("\\\\");
    const int sepLen = sep.length();

    QVector<FilesToChange> groupedFiles;

    for (const QString& filePath : filesToChange)
    {
        int sepPos = filePath.indexOf(sep);
        if (sepPos < 0)
            continue;

        QString pathSuffix = filePath.mid(sepPos + sepLen);
        QVector<QString> linarPathArray = split(pathSuffix, QDir::separator());

        if (linarPathArray.isEmpty())
            continue;

        QString lastElement = linarPathArray.last();
        linarPathArray.pop_back();

        const QString rootPath = filePath.left(sepPos);

        // Build common path
        QString commonPath = rootPath;
        for (const QString& elem : linarPathArray)
        {
            commonPath += QDir::separator();
            commonPath += elem;
        }
        commonPath += QDir::separator();

        // Find or create the group
        auto it = std::find_if(groupedFiles.begin(), groupedFiles.end(),
            [&commonPath](const FilesToChange& g) { return g._commonPath == commonPath; });

        if (it == groupedFiles.end())
        {
            FilesToChange group;
            group._commonPath = commonPath;
            group._rootPath = rootPath;
            group._linarWithoutLastPathElement = linarPathArray;
            group._files.push_back(lastElement);
            groupedFiles.push_back(group);
        }
        else
        {
            it->_files.push_back(lastElement);
        }
    }

    return groupedFiles;
}

// =============================================================================
// Add files/folders to tree (mirrors Win32 addToTree)
// =============================================================================

bool FileBrowser::addToTree(FilesToChange& group, QTreeWidgetItem* node)
{
    // Root search
    if (node == nullptr)
    {
        node = getRootFromFullPath(group._rootPath);
        if (!node)
            return false;
    }

    if (group._linarWithoutLastPathElement.isEmpty())
    {
        // Remove names that already exist in this node
        removeNamesAlreadyInNode(node, group._files);
        if (group._files.isEmpty())
            return false;

        // Filter to items that actually exist on disk
        group._files.erase(
            std::remove_if(group._files.begin(), group._files.end(),
                [&group](const QString& file) {
                    return !doesPathExist(group._commonPath + file);
                }),
            group._files.end());

        if (group._files.isEmpty())
            return false;

        // Add each file/folder as a new tree item
        for (const QString& file : group._files)
        {
            bool isDir = doesDirectoryExist(group._commonPath + file);
            SortingData* customData = new SortingData(QString(), file, isDir);
            _sortingDataArray.push_back(customData);

            FileBrowserTreeWidgetItem* item = new FileBrowserTreeWidgetItem(node);
            item->setText(0, file);
            item->sortData = customData;

            if (isDir)
            {
                item->setData(0, Qt::UserRole + 1, INDEX_FOLDER_CLOSE);
                item->setData(0, Qt::UserRole + 2, INDEX_FOLDER_OPEN);
                setFolderIcon(item, false);
            }
            else
            {
                item->setData(0, Qt::UserRole + 1, INDEX_LEAF);
                setFileIcon(item, file);
            }
        }

        customTreeSort(node, false);
        return true;
    }
    else
    {
        // Recurse into sub-folders
        for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(node);
             hItemNode != nullptr;
             hItemNode = _treeView.getNextSibling(hItemNode))
        {
            if (hItemNode->text(0) == group._linarWithoutLastPathElement[0])
            {
                // Recurse
                group._linarWithoutLastPathElement.erase(group._linarWithoutLastPathElement.begin());
                return addToTree(group, hItemNode);
            }
        }
        return false;
    }
}

// =============================================================================
// Delete files/folders from tree (mirrors Win32 deleteFromTree)
// =============================================================================
bool FileBrowser::deleteFromTree(FilesToChange& group)
{
    QVector<QTreeWidgetItem*> foundItems = findInTree(group, nullptr);

    if (foundItems.isEmpty())
        return false;

    for (QTreeWidgetItem* item : foundItems)
        _treeView.removeItem(item);

    return true;
}

// =============================================================================
// Find items in tree matching a FilesToChange group (mirrors Win32 findInTree)
// =============================================================================
QVector<QTreeWidgetItem*> FileBrowser::findInTree(FilesToChange& group, QTreeWidgetItem* node) const
{
    // Root search
    if (node == nullptr)
    {
        node = const_cast<FileBrowser*>(this)->getRootFromFullPath(group._rootPath);
        if (!node)
            return {};
    }

    if (group._linarWithoutLastPathElement.isEmpty())
    {
        return findChildNodesFromNames(node, group._files);
    }
    else
    {
        for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(node);
             hItemNode != nullptr;
             hItemNode = _treeView.getNextSibling(hItemNode))
        {
            if (hItemNode->text(0) == group._linarWithoutLastPathElement[0])
            {
                group._linarWithoutLastPathElement.erase(group._linarWithoutLastPathElement.begin());
                return findInTree(group, hItemNode);
            }
        }
        return {};
    }
}

// =============================================================================
// Recursive find by path array (mirrors Win32 findInTree single-item version)
// =============================================================================
QTreeWidgetItem* FileBrowser::findInTree(const QString& rootPath,
                                         QTreeWidgetItem* node,
                                         const QVector<QString>& linarPathArray) const
{
    // Root search
    if (node == nullptr)
    {
        node = const_cast<FileBrowser*>(this)->getRootFromFullPath(rootPath);
        if (!node)
            return nullptr;
    }

    if (linarPathArray.isEmpty())
        return node;

    if (linarPathArray.size() == 1)
        return findChildNodeFromName(node, linarPathArray[0]);

    for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(node);
         hItemNode != nullptr;
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        if (hItemNode->text(0) == linarPathArray[0])
        {
            QVector<QString> remaining = linarPathArray;
            remaining.erase(remaining.begin());
            return findInTree(rootPath, hItemNode, remaining);
        }
    }
    return nullptr;
}

// =============================================================================
// Find child nodes by name list (mirrors Win32 findChildNodesFromNames)
// =============================================================================
QVector<QTreeWidgetItem*> FileBrowser::findChildNodesFromNames(QTreeWidgetItem* parent,
                                                                QVector<QString>& labels) const
{
    QVector<QTreeWidgetItem*> itemNodes;

    for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(parent);
         hItemNode != nullptr && !labels.isEmpty();
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        auto it = std::find(labels.begin(), labels.end(), hItemNode->text(0));
        if (it != labels.end())
        {
            labels.erase(it);
            itemNodes.push_back(hItemNode);
        }
    }
    return itemNodes;
}

// =============================================================================
// Remove names that already exist under a node (mirrors Win32 removeNamesAlreadyInNode)
// =============================================================================
void FileBrowser::removeNamesAlreadyInNode(QTreeWidgetItem* parent, QVector<QString>& labels) const
{
    for (QTreeWidgetItem* hItemNode = _treeView.getChildFrom(parent);
         hItemNode != nullptr && !labels.isEmpty();
         hItemNode = _treeView.getNextSibling(hItemNode))
    {
        auto it = std::find(labels.begin(), labels.end(), hItemNode->text(0));
        if (it != labels.end())
            labels.erase(it);
    }
}

// =============================================================================
// Rename item in tree (mirrors Win32 renameInTree)
// =============================================================================
bool FileBrowser::renameInTree(const QString& rootPath,
                                 QTreeWidgetItem* node,
                                 const QVector<QString>& linarPathArrayFrom,
                                 const QString& renameTo)
{
    QTreeWidgetItem* foundItem = findInTree(rootPath, node, linarPathArrayFrom);
    if (!foundItem)
        return false;

    // Apply rename
    foundItem->setText(0, renameTo);

    // Update sort data
    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(foundItem);
    if (fbItem && fbItem->sortData)
        fbItem->sortData->_label = renameTo;

    // Re-sort parent
    QTreeWidgetItem* parent = _treeView.getParent(foundItem);
    customTreeSort(parent, false);

    return true;
}

// =============================================================================
// FolderInfo: add path to structure (mirrors Win32 addToStructure)
// =============================================================================
bool FolderInfo::addToStructure(QString& fullpath, const QVector<QString>& linarPathArray)
{
    if (linarPathArray.size() == 1)
    {
        fullpath += QDir::separator();
        fullpath += linarPathArray[0];

        if (doesDirectoryExist(fullpath))
        {
            // Search in sub-folders
            for (const auto& folder : _subFolders)
            {
                if (linarPathArray[0] == folder.getName())
                    return false; // Already exists
            }
            _subFolders.push_back(FolderInfo(linarPathArray[0], this));
            return true;
        }
        else
        {
            // Search in files
            for (const auto& file : _files)
            {
                if (linarPathArray[0] == file.getName())
                    return false; // Already exists
            }
            _files.push_back(FileInfo(linarPathArray[0]));
            return true;
        }
    }
    else
    {
        // Navigate into sub-folders
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArray[0])
            {
                fullpath += QDir::separator();
                fullpath += linarPathArray[0];
                QVector<QString> remaining = linarPathArray;
                remaining.erase(remaining.begin());
                return folder.addToStructure(fullpath, remaining);
            }
        }
        return false;
    }
}

// =============================================================================
// FolderInfo: remove path from structure (mirrors Win32 removeFromStructure)
// =============================================================================
bool FolderInfo::removeFromStructure(const QVector<QString>& linarPathArray)
{
    if (linarPathArray.size() == 1)
    {
        // Remove from files
        for (int i = 0; i < _files.size(); ++i)
        {
            if (_files[i].getName() == linarPathArray[0])
            {
                _files.erase(_files.begin() + i);
                return true;
            }
        }

        // Remove from sub-folders
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
        for (int i = 0; i < _subFolders.size(); ++i)
        {
            if (_subFolders[i].getName() == linarPathArray[0])
            {
                QVector<QString> remaining = linarPathArray;
                remaining.erase(remaining.begin());
                return _subFolders[i].removeFromStructure(remaining);
            }
        }
    }
    return false;
}

// =============================================================================
// FolderInfo: rename path in structure (mirrors Win32 renameInStructure)
// =============================================================================
bool FolderInfo::renameInStructure(const QVector<QString>& linarPathArrayFrom,
                                    const QVector<QString>& linarPathArrayTo)
{
    if (linarPathArrayFrom.size() == 1)
    {
        // Rename file
        for (auto& file : _files)
        {
            if (file.getName() == linarPathArrayFrom[0])
            {
                file.setName(linarPathArrayTo.isEmpty() ? QString() : linarPathArrayTo[0]);
                return true;
            }
        }

        // Rename folder
        for (auto& folder : _subFolders)
        {
            if (folder.getName() == linarPathArrayFrom[0])
            {
                folder.setName(linarPathArrayTo.isEmpty() ? QString() : linarPathArrayTo[0]);
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
                QVector<QString> fromRemaining = linarPathArrayFrom;
                fromRemaining.erase(fromRemaining.begin());
                QVector<QString> toRemaining = linarPathArrayTo;
                if (!toRemaining.isEmpty())
                    toRemaining.erase(toRemaining.begin());
                return folder.renameInStructure(fromRemaining, toRemaining);
            }
        }
        return false;
    }
}

// =============================================================================
// FolderUpdater: start file system watcher
// =============================================================================
FolderUpdater::FolderUpdater(const FolderInfo& fi, FileBrowser* pFileBrowser)
    : _rootFolder(fi), _pFileBrowser(pFileBrowser)
{
}

FolderUpdater::~FolderUpdater()
{
    stopWatcher();
}

void FolderUpdater::startWatcher()
{
    _watcher = new QFileSystemWatcher(this);
    _watcher->addPath(_rootFolder.getRootPath());

    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &FolderUpdater::onDirectoryChanged, Qt::QueuedConnection);

    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &FolderUpdater::onDirectoryChanged, Qt::QueuedConnection);
}

void FolderUpdater::stopWatcher()
{
    if (_watcher)
    {
        _watcher->deleteLater();
        _watcher = nullptr;
    }
}

// =============================================================================
// FolderUpdater: directory change notification
// =============================================================================
void FolderUpdater::onDirectoryChanged(const QString& path)
{
    Q_UNUSED(path);
    QTimer::singleShot(100, this, &FolderUpdater::processChanges);
}

// =============================================================================
// FolderUpdater: debounced process of pending changes
// =============================================================================
void FolderUpdater::processChanges()
{
    if (_stopped)
        return;

    doAdd();
    doRemove();
    doRename();
}

// =============================================================================
// FolderUpdater: process file additions
// =============================================================================
void FolderUpdater::doAdd()
{
    if (_pendingAdds.isEmpty())
        return;

    if (_pFileBrowser)
        QApplication::postEvent(_pFileBrowser,
            new QFileBrowserEvent(FB_ADDFILE, QVariant::fromValue(_pendingAdds)));
    _pendingAdds.clear();
}

// =============================================================================
// FolderUpdater: process file removals
// =============================================================================
void FolderUpdater::doRemove()
{
    if (_pendingRemoves.isEmpty())
        return;

    if (_pFileBrowser)
        QApplication::postEvent(_pFileBrowser,
            new QFileBrowserEvent(FB_RMFILE, QVariant::fromValue(_pendingRemoves)));
    _pendingRemoves.clear();
}

// =============================================================================
// FolderUpdater: process renames
// =============================================================================
void FolderUpdater::doRename()
{
    if (_pendingRenames.isEmpty())
        return;

    QVector<QString> renameList;
    for (auto it = _pendingRenames.begin(); it != _pendingRenames.end(); ++it)
    {
        renameList.push_back(it.key());
        renameList.push_back(it.value());
    }

    if (_pFileBrowser)
        QApplication::postEvent(_pFileBrowser,
            new QFileBrowserEvent(FB_RNFILE, QVariant::fromValue(renameList)));
    _pendingRenames.clear();
}

// =============================================================================
// QFileBrowserEvent — custom event for async file changes
// =============================================================================
QEvent::Type QFileBrowserEvent::fbEventType(int message)
{
    static QEvent::Type addType    = QEvent::Type(QEvent::User + 1024);
    static QEvent::Type rmType     = QEvent::Type(QEvent::User + 1025);
    static QEvent::Type renameType = QEvent::Type(QEvent::User + 1026);
    switch (message)
    {
        case FB_ADDFILE: return addType;
        case FB_RMFILE:  return rmType;
        case FB_RNFILE:  return renameType;
        default:         return QEvent::None;
    }
}

QFileBrowserEvent::QFileBrowserEvent(int message, const QVariant& data)
    : QEvent(fbEventType(message)), _message(message), _data(data)
{
}

// =============================================================================
// FileBrowserTreeWidget: constructor
// =============================================================================
FileBrowserTreeWidget::FileBrowserTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHeaderHidden(true);
}

// =============================================================================
// Begin drag — mirrors Win32 TVN_BEGINDRAG
// =============================================================================
void FileBrowserTreeWidget::beginDrag(QTreeWidgetItem* item)
{
    if (!item)
        return;

    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(item);
    if (!fbItem)
        return;

    // Check if this item can be dragged out
    if (!isDraggable(item))
        return;

    _isDragging = true;
    _dragItem = item;
    _dragStartPos = QCursor::pos();

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(item->text(0));
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}

// =============================================================================
// Drag item with mouse move
// =============================================================================
void FileBrowserTreeWidget::dragItem(const QPoint& pos)
{
    Q_UNUSED(pos);
}

// =============================================================================
// Drop item — check if target accepts drop
// =============================================================================
bool FileBrowserTreeWidget::dropItem()
{
    _isDragging = false;
    return false;
}

// =============================================================================
// Add item to "cannot drop in" list (by image index)
// =============================================================================
void FileBrowserTreeWidget::addCanNotDropInList(int index)
{
    _cannotDropList.insert(index);
}

// =============================================================================
// Add item to "cannot drag out" list (by image index)
// =============================================================================
void FileBrowserTreeWidget::addCanNotDragOutList(int index)
{
    _cannotDragList.insert(index);
}

// =============================================================================
// Can item be dropped on target?
// =============================================================================
bool FileBrowserTreeWidget::isDroppable(QTreeWidgetItem* item) const
{
    if (!item)
        return false;

    int imageIdx = item->data(0, Qt::UserRole + 1).toInt();
    return !_cannotDropList.contains(imageIdx);
}

// =============================================================================
// Can item be dragged?
// =============================================================================
bool FileBrowserTreeWidget::isDraggable(QTreeWidgetItem* item) const
{
    if (!item)
        return false;

    int imageIdx = item->data(0, Qt::UserRole + 1).toInt();
    return !_cannotDragList.contains(imageIdx);
}

// =============================================================================
// Get item lParam (custom data)
// =============================================================================
void* FileBrowserTreeWidget::getItemParam(QTreeWidgetItem* item) const
{
    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(item);
    return fbItem ? static_cast<void*>(fbItem->sortData) : nullptr;
}

// =============================================================================
// Set item lParam
// =============================================================================
void FileBrowserTreeWidget::setItemParam(QTreeWidgetItem* item, void* param)
{
    FileBrowserTreeWidgetItem* fbItem = dynamic_cast<FileBrowserTreeWidgetItem*>(item);
    if (fbItem)
        fbItem->sortData = static_cast<SortingData*>(param);
}

// =============================================================================
// Remove item from tree
// =============================================================================
void FileBrowserTreeWidget::removeItem(QTreeWidgetItem* item)
{
    if (item)
        delete item;
}

// =============================================================================
// Get root item
// =============================================================================
QTreeWidgetItem* FileBrowserTreeWidget::getRoot() const
{
    if (topLevelItemCount() > 0)
        return topLevelItem(0);
    return nullptr;
}

// =============================================================================
// Get first child of parent
// =============================================================================
QTreeWidgetItem* FileBrowserTreeWidget::getChildFrom(QTreeWidgetItem* parent) const
{
    if (!parent)
        return nullptr;
    return parent->child(0);
}

// =============================================================================
// Get next sibling
// =============================================================================
QTreeWidgetItem* FileBrowserTreeWidget::getNextSibling(QTreeWidgetItem* item) const
{
    if (!item)
        return nullptr;
    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return nullptr;
    int idx = parent->indexOfChild(item);
    if (idx < 0 || idx >= parent->childCount() - 1)
        return nullptr;
    return parent->child(idx + 1);
}

// =============================================================================
// Get parent of item
// =============================================================================
QTreeWidgetItem* FileBrowserTreeWidget::getParent(QTreeWidgetItem* item) const
{
    return item ? item->parent() : nullptr;
}

// =============================================================================
// Get selected item
// =============================================================================
QTreeWidgetItem* FileBrowserTreeWidget::getSelection() const
{
    QList<QTreeWidgetItem*> sel = selectedItems();
    return sel.isEmpty() ? nullptr : sel.first();
}

// =============================================================================
// Get display name of item
// =============================================================================
QString FileBrowserTreeWidget::itemDisplayName(QTreeWidgetItem* item) const
{
    return item ? item->text(0) : QString();
}

// =============================================================================
// Expand all items
// =============================================================================
void FileBrowserTreeWidget::expandAll()
{
    QTreeWidget::expandAll();
}

// =============================================================================
// Collapse all items
// =============================================================================
void FileBrowserTreeWidget::foldAll()
{
    QTreeWidget::collapseAll();
}

// =============================================================================
// Toggle expand/collapse
// =============================================================================
void FileBrowserTreeWidget::toggleExpandCollapse(QTreeWidgetItem* item)
{
    if (!item)
        return;
    if (item->isExpanded())
        collapseItem(item);
    else
        expandItem(item);
}

// =============================================================================
// Make labels editable
// =============================================================================
void FileBrowserTreeWidget::makeLabelEditable(bool editable)
{
    if (editable)
        setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    else
        setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// =============================================================================
// Custom sorting (folders before files, case-insensitive)
// =============================================================================
void FileBrowserTreeWidget::customSorting(QTreeWidgetItem* parent, bool recursive)
{
    // Use the tree's sort function — will call folderSortFunc
    if (parent)
    {
        parent->sortChildren(0, Qt::AscendingOrder);
        if (recursive)
        {
            for (int i = 0; i < parent->childCount(); ++i)
                customSorting(parent->child(i), true);
        }
    }
    else
    {
        sortItems(0, Qt::AscendingOrder);
    }
}

// =============================================================================
// Folder sort function (static) — folders before files, then case-insensitive
// =============================================================================
int FileBrowserTreeWidget::folderSortFunc(const QTreeWidgetItem* a, const QTreeWidgetItem* b)
{
    FileBrowserTreeWidgetItem* fa = const_cast<FileBrowserTreeWidgetItem*>(
        dynamic_cast<const FileBrowserTreeWidgetItem*>(a));
    FileBrowserTreeWidgetItem* fbItem = const_cast<FileBrowserTreeWidgetItem*>(
        dynamic_cast<const FileBrowserTreeWidgetItem*>(b));

    SortingData* sa = fa ? fa->sortData : nullptr;
    SortingData* sb = fbItem ? fbItem->sortData : nullptr;

    bool aIsFolder = (sa ? sa->_isFolder : true);
    bool bIsFolder = (sb ? sb->_isFolder : true);

    if (aIsFolder && !bIsFolder)
        return -1;
    if (!aIsFolder && bIsFolder)
        return 1;

    return a->text(0).toLower().localeAwareCompare(b->text(0).toLower());
}

// =============================================================================
// Set item icon by image index
// =============================================================================
void FileBrowserTreeWidget::setItemIcon(QTreeWidgetItem* item, int imageIndex)
{
    Q_UNUSED(item);
    Q_UNUSED(imageIndex);
    // Icons are set via setFolderIcon / setFileIcon in FileBrowser
}

// =============================================================================
// Mouse press — start drag if dragging from an allowed icon
// =============================================================================
void FileBrowserTreeWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QTreeWidgetItem* item = itemAt(event->pos());
        if (item && isDraggable(item))
        {
            _dragStartPos = event->pos();
        }
    }
    QTreeWidget::mousePressEvent(event);
}

// =============================================================================
// Mouse move — initiate drag
// =============================================================================
void FileBrowserTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QTreeWidgetItem* item = itemAt(event->pos());
        if (item && isDraggable(item))
        {
            QPoint diff = event->pos() - _dragStartPos;
            if (diff.manhattanLength() > QApplication::startDragDistance())
            {
                beginDrag(item);
            }
        }
    }
    QTreeWidget::mouseMoveEvent(event);
}

// =============================================================================
// Mouse release — end drag
// =============================================================================
void FileBrowserTreeWidget::mouseReleaseEvent(QMouseEvent* event)
{
    _isDragging = false;
    QTreeWidget::mouseReleaseEvent(event);
}

// =============================================================================
// Drag enter — accept if dropping on a valid target
// =============================================================================
void FileBrowserTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTreeWidget::dragEnterEvent(event);
    }
}

// =============================================================================
// Drag move — update drop indicator
// =============================================================================
void FileBrowserTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    QTreeWidgetItem* targetItem = itemAt(event->pos());
    if (targetItem && isDroppable(targetItem))
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

// =============================================================================
// Drop event — handle file drops from OS
// =============================================================================
void FileBrowserTreeWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        const QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QString path = urls.first().toLocalFile();
            if (!path.isEmpty())
                emit fileDropped(path);
        }
    }
    event->acceptProposedAction();
}

// =============================================================================
// FileBrowser: set folder icon (open or closed)
// =============================================================================
void FileBrowser::setFolderIcon(QTreeWidgetItem* item, bool isOpen)
{
    if (!item)
        return;

    // Use Qt's file icon provider for folder icons
    static QIcon folderOpenIcon   = QFileIconProvider().icon(QFileIconProvider::Folder);
    static QIcon folderClosedIcon = QFileIconProvider().icon(QFileIconProvider::Folder);

    if (isOpen)
        item->setIcon(0, folderOpenIcon);
    else
        item->setIcon(0, folderClosedIcon);
}

// =============================================================================
// FileBrowser: set file icon based on extension
// =============================================================================
void FileBrowser::setFileIcon(QTreeWidgetItem* item, const QString& fileName)
{
    if (!item)
        return;

    QFileInfo fi(fileName);
    QIcon icon = QFileIconProvider().icon(fi);
    item->setIcon(0, icon);
}

// =============================================================================
// FileBrowser: custom tree sort comparator (folders before files)
// =============================================================================
int FileBrowser::categorySortFunc(const SortingData* a, const SortingData* b)
{
    if (!a || !b)
        return 0;

    if (a->_isFolder && !b->_isFolder)
        return -1;
    if (!a->_isFolder && b->_isFolder)
        return 1;

    return a->_label.toLower().localeAwareCompare(b->_label.toLower());
}

// =============================================================================
// FileBrowser: apply custom sort to tree (recursive)
// =============================================================================
void FileBrowser::customTreeSort(QTreeWidgetItem* parent, bool recursive)
{
    if (!parent)
        return;

    // Sort this parent's children
    parent->sortChildren(0, Qt::AscendingOrder);

    if (recursive)
    {
        for (int i = 0; i < parent->childCount(); ++i)
            customTreeSort(parent->child(i), true);
    }
}

// =============================================================================
// FileBrowser: set background color (mirrors Win32 setBackgroundColor)
// =============================================================================
void FileBrowser::setBackgroundColor(const QColor& col)
{
    QPalette pal = _treeView.palette();
    pal.setColor(QPalette::Base, col);
    _treeView.setPalette(pal);
}

// =============================================================================
// FileBrowser: set foreground color (mirrors Win32 setForegroundColor)
// =============================================================================
void FileBrowser::setForegroundColor(const QColor& col)
{
    QPalette pal = _treeView.palette();
    pal.setColor(QPalette::Text, col);
    _treeView.setPalette(pal);
}

// =============================================================================
// FileBrowser: delete all roots from tree
// =============================================================================
void FileBrowser::deleteAllFromTree()
{
    popupMenuCmd(IDM_FILEBROWSER_REMOVEALLROOTS);
}
