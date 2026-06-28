// Semantic Lift: Win32 ProjectPanel → Qt6 ProjectPanel
// Original: PowerEditor/src/WinControls/ProjectPanel/ProjectPanel.cpp
// Target: npp-qt/src/WinControls/ProjectPanel.cpp

#include "ProjectPanel.h"
#include "DockingWnd.h"
#include "NppConstants.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QHeaderView>
#include <QStyle>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>

// =============================================================================
// ProjectTreeItem — constructor
// =============================================================================

ProjectTreeItem::ProjectTreeItem(QTreeWidget* parent)
    : QTreeWidgetItem(parent, Type)
{}

ProjectTreeItem::ProjectTreeItem(QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent, Type)
{}

// =============================================================================
// ProjectTreeItem — XML serialization
// Mirrors Win32 buildProjectXml() / NppXml reader.
// =============================================================================

void ProjectTreeItem::loadFromXml(QXmlStreamReader& xml)
{
    if (xml.tokenType() != QXmlStreamReader::StartElement)
        return;

    const QStringView tagName = xml.name();
    if (tagName == QStringLiteral("Project"))
        setNodeType(nodeType_project);
    else if (tagName == QStringLiteral("Folder"))
        setNodeType(nodeType_folder);
    else if (tagName == QStringLiteral("File"))
        setNodeType(nodeType_file);

    // Read attributes
    QXmlStreamAttributes attrs = xml.attributes();
    if (attrs.hasAttribute(QStringLiteral("name")))
        setText(0, attrs.value(QStringLiteral("name")).toString());
    if (attrs.hasAttribute(QStringLiteral("path")))
        setFullPath(attrs.value(QStringLiteral("path")).toString());

    // Read children
    while (xml.readNextStartElement())
    {
        auto* child = new ProjectTreeItem(this);
        child->loadFromXml(xml);
    }

    if (xml.tokenType() == QXmlStreamReader::EndElement)
        xml.skipCurrentElement();
}

void ProjectTreeItem::saveToXml(QXmlStreamWriter& xml) const
{
    switch (nodeType())
    {
    case nodeType_project:
        xml.writeStartElement(QStringLiteral("Project"));
        break;
    case nodeType_folder:
        xml.writeStartElement(QStringLiteral("Folder"));
        break;
    case nodeType_file:
        xml.writeStartElement(QStringLiteral("File"));
        break;
    default:
        xml.writeStartElement(QStringLiteral("Project"));
        break;
    }

    xml.writeAttribute(QStringLiteral("name"), text(0));
    if (!_fullPath.isEmpty())
        xml.writeAttribute(QStringLiteral("path"), _fullPath);

    // Save children
    for (int i = 0; i < childCount(); ++i)
    {
        auto* childItem = dynamic_cast<ProjectTreeItem*>(child(i));
        if (childItem)
            childItem->saveToXml(xml);
    }

    xml.writeEndElement();
}

// =============================================================================
// FileRelocalizerDlg — dialog for modifying file path
// Mirrors Win32 FileRelocalizerDlg (IDD_RELOCALIZE_FILEPATH).
// =============================================================================

FileRelocalizerDlg::FileRelocalizerDlg()
    : QDialog()
{
    setWindowTitle(QStringLiteral("Modify File Path"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    QFormLayout* form = new QFormLayout();

    QLineEdit* pathEdit = new QLineEdit(this);
    pathEdit->setObjectName("pathEdit");
    form->addRow(QStringLiteral("New path:"), pathEdit);

    mainLayout->addLayout(form);
    mainLayout->addSpacing(8);

    // QDialogButtonBox provides standard Win32 IDOK/IDCANCEL behavior
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this, pathEdit]() {
        _fullFilePath = pathEdit->text();
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

int FileRelocalizerDlg::doDialog(const QString& fn, bool isRTL)
{
    Q_UNUSED(isRTL);
    QLineEdit* pathEdit = findChild<QLineEdit*>(QStringLiteral("pathEdit"));
    if (pathEdit)
        pathEdit->setText(fn);
    _fullFilePath.clear();

    if (exec() == QDialog::Accepted)
        return IDOK;
    return IDCANCEL;
}

// =============================================================================
// ProjectPanel — constructor
// =============================================================================

ProjectPanel::ProjectPanel()
    : DockingDlgInterface(0)
{
    setWindowTitle(PM_PROJECTPANELTITLE);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar (mirrors Win32 TB with IDB_PROJECT_BTN + IDB_EDIT_BTN)
    _toolbar = new QToolBar(this);
    _toolbar->setMovable(false);
    _toolbar->setIconSize(QSize(16, 16));

    _newWsAction   = _toolbar->addAction(QStringLiteral("New"), this, &ProjectPanel::onNewWorkspace);
    _openWsAction  = _toolbar->addAction(QStringLiteral("Open"), this, &ProjectPanel::onOpenWorkspace);
    _saveWsAction  = _toolbar->addAction(QStringLiteral("Save"), this, &ProjectPanel::onSaveWorkspace);
    _toolbar->addSeparator();
    _foldAllAction = _toolbar->addAction(QStringLiteral("Fold All"), this, [this]() { _treeWidget->collapseAll(); });
    _expandAllAction = _toolbar->addAction(QStringLiteral("Expand All"), this, [this]() { _treeWidget->expandAll(); });
    _toolbar->addSeparator();
    _findInProjectsAction = _toolbar->addAction(QStringLiteral("Find in Projects"), this, &ProjectPanel::onFindInProjects);

    mainLayout->addWidget(_toolbar);

    // Tree widget
    _treeWidget = new QTreeWidget(this);
    _treeWidget->setHeaderLabel(PM_WORKSPACEROOTNAME);
    _treeWidget->setAlternatingRowColors(true);
    _treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    _treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    _treeWidget->setDragEnabled(true);
    _treeWidget->setAcceptDrops(true);
    _treeWidget->setDropIndicatorShown(true);

    connect(_treeWidget, &QTreeWidget::itemDoubleClicked,
            this, &ProjectPanel::onItemDoubleClicked);
    connect(_treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &ProjectPanel::onContextMenu);
    connect(_treeWidget, &QTreeWidget::itemExpanded,
            this, &ProjectPanel::onItemExpanded);
    connect(_treeWidget, &QTreeWidget::itemCollapsed,
            this, &ProjectPanel::onItemCollapsed);

    mainLayout->addWidget(_treeWidget);
    setLayout(mainLayout);

    initMenus();
}

// =============================================================================
// init — panel initialization
// Mirrors Win32 WM_INITDIALOG.
// =============================================================================

void ProjectPanel::init(QApplication*, QWidget*, int panelID)
{
    _panelID = panelID;
    newWorkSpace();
}

// =============================================================================
// initMenus — build context menus for each node type
// Mirrors Win32 ProjectPanel::initMenus().
// =============================================================================

void ProjectPanel::initMenus()
{
    // Workspace root menu (mirrors _hWorkSpaceMenu)
    _workspaceMenu = new QMenu(this);
    _workspaceMenu->addAction(QStringLiteral("New Workspace"), this, &ProjectPanel::onNewWorkspace);
    _workspaceMenu->addAction(QStringLiteral("Open Workspace..."), this, &ProjectPanel::onOpenWorkspace);
    _workspaceMenu->addAction(QStringLiteral("Reload Workspace"), this, &ProjectPanel::onReloadWorkspace);
    _workspaceMenu->addAction(QStringLiteral("Save"), this, &ProjectPanel::onSaveWorkspace);
    _workspaceMenu->addAction(QStringLiteral("Save As..."), this, &ProjectPanel::onSaveAsWorkspace);
    _workspaceMenu->addAction(QStringLiteral("Save a Copy As..."), this, &ProjectPanel::onSaveACopyAsWorkspace);
    _workspaceMenu->addSeparator();
    _workspaceMenu->addAction(QStringLiteral("New Project"), this, &ProjectPanel::onAddFolder);
    _workspaceMenu->addSeparator();
    _workspaceMenu->addAction(QStringLiteral("Find in Projects..."), this, &ProjectPanel::onFindInProjects);

    // Project/Folder menu (mirrors _hProjectMenu)
    _projectMenu = new QMenu(this);
    _projectMenu->addAction(QStringLiteral("Move Up"), this, &ProjectPanel::onMoveUp);
    _projectMenu->addAction(QStringLiteral("Move Down"), this, &ProjectPanel::onMoveDown);
    _projectMenu->addSeparator();
    _projectMenu->addAction(QStringLiteral("Rename"), this, &ProjectPanel::onRenameItem);
    _projectMenu->addAction(QStringLiteral("Add Folder"), this, &ProjectPanel::onAddFolder);
    _projectMenu->addAction(QStringLiteral("Add Files..."), this, &ProjectPanel::onAddFiles);
    _projectMenu->addAction(QStringLiteral("Add Files Recursively..."), this, &ProjectPanel::onAddFilesRecursive);
    _projectMenu->addSeparator();
    _projectMenu->addAction(QStringLiteral("Remove"), this, &ProjectPanel::onRemoveItem);

    // Folder menu (mirrors _hFolderMenu)
    _folderMenu = new QMenu(this);
    _folderMenu->addAction(QStringLiteral("Move Up"), this, &ProjectPanel::onMoveUp);
    _folderMenu->addAction(QStringLiteral("Move Down"), this, &ProjectPanel::onMoveDown);
    _folderMenu->addSeparator();
    _folderMenu->addAction(QStringLiteral("Rename"), this, &ProjectPanel::onRenameItem);
    _folderMenu->addAction(QStringLiteral("Add Folder"), this, &ProjectPanel::onAddFolder);
    _folderMenu->addAction(QStringLiteral("Add Files..."), this, &ProjectPanel::onAddFiles);
    _folderMenu->addAction(QStringLiteral("Add Files Recursively..."), this, &ProjectPanel::onAddFilesRecursive);
    _folderMenu->addSeparator();
    _folderMenu->addAction(QStringLiteral("Remove"), this, &ProjectPanel::onRemoveItem);

    // File menu (mirrors _hFileMenu)
    _fileMenu = new QMenu(this);
    _fileMenu->addAction(QStringLiteral("Move Up"), this, &ProjectPanel::onMoveUp);
    _fileMenu->addAction(QStringLiteral("Move Down"), this, &ProjectPanel::onMoveDown);
    _fileMenu->addSeparator();
    _fileMenu->addAction(QStringLiteral("Rename"), this, &ProjectPanel::onRenameItem);
    _fileMenu->addAction(QStringLiteral("Modify File Path..."), this, &ProjectPanel::onModifyFilePath);
    _fileMenu->addSeparator();
    _fileMenu->addAction(QStringLiteral("Remove"), this, &ProjectPanel::onRemoveItem);
}

void ProjectPanel::destroyMenus()
{
    delete _workspaceMenu; _workspaceMenu = nullptr;
    delete _projectMenu; _projectMenu = nullptr;
    delete _folderMenu; _folderMenu = nullptr;
    delete _fileMenu; _fileMenu = nullptr;
}

// =============================================================================
// Workspace management
// Mirrors Win32 ProjectPanel workspace functions.
// =============================================================================

void ProjectPanel::newWorkSpace()
{
    _treeWidget->clear();
    _workSpaceFilePath.clear();
    _fullPathCache.clear();
    setWorkSpaceDirty(false);

    // Add root "Workspace" node
    auto* root = new ProjectTreeItem(_treeWidget);
    root->setNodeType(nodeType_root);
    root->setText(0, PM_WORKSPACEROOTNAME);
    root->setExpanded(true);
    _treeWidget->addTopLevelItem(root);
}

bool ProjectPanel::openWorkSpace(const QString& projectFileName, bool force)
{
    if (projectFileName.isEmpty())
        return false;

    if (!force && !_workSpaceFilePath.isEmpty())
    {
        if (projectFileName == _workSpaceFilePath)
            return true;
        if (!saveWorkspaceRequest())
            return true;
    }

    QFileInfo fi(projectFileName);
    if (!fi.exists())
        return false;

    return readWorkSpace(projectFileName);
}

bool ProjectPanel::readWorkSpace(const QString& projectFileName)
{
    QFile file(projectFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    _treeWidget->clear();
    _fullPathCache.clear();
    QXmlStreamReader xml(&file);

    // Expect <NotepadPlus>
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("NotepadPlus"))
        {
            // Expect <Project> children
            while (xml.readNextStartElement())
            {
                if (xml.name() == QStringLiteral("Project"))
                {
                    ProjectTreeItem* root = dynamic_cast<ProjectTreeItem*>(_treeWidget->topLevelItem(0));
                    if (!root)
                    {
                        root = new ProjectTreeItem(_treeWidget);
                        root->setNodeType(nodeType_root);
                        root->setText(0, PM_WORKSPACEROOTNAME);
                        root->setExpanded(true);
                        _treeWidget->addTopLevelItem(root);
                    }

                    auto* projectItem = new ProjectTreeItem(root);
                    projectItem->loadFromXml(xml);
                    root->addChild(projectItem);
                    root->setExpanded(true);
                }
                else
                {
                    xml.skipCurrentElement();
                }
            }
        }
        else
        {
            xml.skipCurrentElement();
        }
    }

    file.close();
    _workSpaceFilePath = projectFileName;
    setWorkSpaceDirty(false);
    return true;
}

bool ProjectPanel::saveWorkSpace()
{
    if (_workSpaceFilePath.isEmpty())
        return saveWorkSpaceAs(false);
    return writeWorkSpace(_workSpaceFilePath, true);
}

bool ProjectPanel::saveWorkSpaceAs(bool saveCopyAs)
{
    QString path = QFileDialog::getSaveFileName(this,
        QStringLiteral("Save Workspace"),
        QDir::homePath(),
        QStringLiteral("Project Workspace (*.workspace.xml)"));
    if (path.isEmpty())
        return false;
    if (saveCopyAs)
    {
        bool ok = writeWorkSpace(path, false);
        if (ok)
            emit workSpaceSaved(path);
        return ok;
    }
    return writeWorkSpace(path, true);
}

bool ProjectPanel::writeWorkSpace(const QString& projectFileName, bool doUpdateGUI)
{
    QFile file(projectFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("NotepadPlus"));

    QTreeWidgetItem* root = _treeWidget->topLevelItem(0);
    if (root)
    {
        // Write workspace name as attribute on NotepadPlus
        QFileInfo fi(projectFileName);
        xml.writeAttribute(QStringLiteral("name"), fi.baseName());

        for (int i = 0; i < root->childCount(); ++i)
        {
            auto* item = dynamic_cast<ProjectTreeItem*>(root->child(i));
            if (item)
                item->saveToXml(xml);
        }
    }

    xml.writeEndElement(); // NotepadPlus
    xml.writeEndDocument();
    file.close();

    if (doUpdateGUI)
    {
        _workSpaceFilePath = projectFileName;
        QFileInfo fi(projectFileName);
        if (root)
            root->setText(0, fi.baseName());
    }

    setWorkSpaceDirty(false);
    emit workSpaceSaved(projectFileName);
    return true;
}

bool ProjectPanel::checkIfNeedSave()
{
    if (isDirty())
    {
        QString title = !_workSpaceFilePath.isEmpty()
            ? QFileInfo(_workSpaceFilePath).fileName()
            : _panelTitle;

        QMessageBox::StandardButton reply = QMessageBox::question(this, title,
            QStringLiteral("The workspace was modified. Do you want to save it?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes)
        {
            if (!saveWorkSpace())
                return false;
        }
        else if (reply == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

// =============================================================================
// Node management
// =============================================================================

ProjectTreeItem* ProjectPanel::addFolder(QTreeWidgetItem* hTreeItem, const QString& folderName)
{
    auto* parent = hTreeItem
        ? dynamic_cast<ProjectTreeItem*>(hTreeItem)
        : dynamic_cast<ProjectTreeItem*>(_treeWidget->topLevelItem(0));
    if (!parent)
        return nullptr;

    auto* item = new ProjectTreeItem(parent);
    item->setNodeType(nodeType_folder);
    item->setText(0, folderName.isEmpty() ? PM_NEWFOLDERNAME : folderName);
    item->setExpanded(true);
    parent->addChild(item);
    parent->setExpanded(true);

    setWorkSpaceDirty(true);
    return item;
}

ProjectTreeItem* ProjectPanel::addFile(QTreeWidgetItem* hTreeItem, const QString& filePath)
{
    auto* parent = hTreeItem
        ? dynamic_cast<ProjectTreeItem*>(hTreeItem)
        : dynamic_cast<ProjectTreeItem*>(_treeWidget->topLevelItem(0));
    if (!parent)
        return nullptr;

    QFileInfo fi(filePath);
    auto* item = new ProjectTreeItem(parent);
    item->setNodeType(nodeType_file);
    item->setText(0, fi.fileName());
    item->setFullPath(filePath);
    parent->addChild(item);
    _fullPathCache.append(filePath);

    setWorkSpaceDirty(true);
    return item;
}

void ProjectPanel::addFiles(QTreeWidgetItem* hTreeItem)
{
    QStringList files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("Add Files"),
        QDir::homePath(),
        QStringLiteral("All Files (*.*)"));
    for (const QString& f : files)
        addFile(hTreeItem, f);
}

void ProjectPanel::addFilesFromDirectory(QTreeWidgetItem* hTreeItem)
{
    QString dir = QFileDialog::getExistingDirectory(this,
        QStringLiteral("Select Directory"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        recursiveAddFilesFrom(dir, hTreeItem);
}

void ProjectPanel::recursiveAddFilesFrom(const QString& folderPath, QTreeWidgetItem* hTreeItem)
{
    QDir dir(folderPath);
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    auto* folder = addFolder(hTreeItem, dir.dirName());
    if (!folder)
        return;

    for (const QFileInfo& fi : entries)
    {
        if (fi.isDir())
            recursiveAddFilesFrom(fi.absoluteFilePath(), folder);
        else if (fi.isFile())
            addFile(folder, fi.absoluteFilePath());
    }
}

// =============================================================================
// Context menu handling
// Mirrors Win32 WM_CONTEXTMENU + showContextMenu().
// =============================================================================

void ProjectPanel::onContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = _treeWidget->itemAt(pos);
    QMenu* menu = nullptr;

    if (!item)
    {
        menu = _workspaceMenu;
    }
    else
    {
        auto* projItem = dynamic_cast<ProjectTreeItem*>(item);
        if (projItem)
        {
            switch (projItem->nodeType())
            {
            case nodeType_root:
            case nodeType_project:
                menu = _workspaceMenu;
                break;
            case nodeType_folder:
                menu = _folderMenu;
                break;
            case nodeType_file:
                menu = _fileMenu;
                break;
            }
        }
    }

    if (menu)
        menu->exec(_treeWidget->mapToGlobal(pos));
}

void ProjectPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    auto* projItem = dynamic_cast<ProjectTreeItem*>(item);
    if (!projItem)
        return;

    if (projItem->nodeType() == nodeType_file)
    {
        openSelectFile();
    }
    else
    {
        if (item->isExpanded())
            _treeWidget->collapseItem(item);
        else
            _treeWidget->expandItem(item);
    }
}

void ProjectPanel::onItemExpanded(QTreeWidgetItem*) {}
void ProjectPanel::onItemCollapsed(QTreeWidgetItem*) {}

void ProjectPanel::onNewWorkspace()
{
    if (!checkIfNeedSave())
        return;
    newWorkSpace();
}

void ProjectPanel::onOpenWorkspace()
{
    if (!checkIfNeedSave())
        return;

    QString path = QFileDialog::getOpenFileName(this,
        QStringLiteral("Open Workspace"),
        QDir::homePath(),
        QStringLiteral("Project Workspace (*.workspace.xml)"));
    if (!path.isEmpty())
        openWorkSpace(path, false);
}

void ProjectPanel::onSaveWorkspace()
{
    saveWorkSpace();
}

void ProjectPanel::onSaveAsWorkspace()
{
    saveWorkSpaceAs(false);
}

void ProjectPanel::onSaveACopyAsWorkspace()
{
    saveWorkSpaceAs(true);
}

void ProjectPanel::onReloadWorkspace()
{
    if (!_workSpaceFilePath.isEmpty())
        readWorkSpace(_workSpaceFilePath);
}

void ProjectPanel::onAddFolder()
{
    QTreeWidgetItem* sel = _treeWidget->currentItem();
    QString name = QInputDialog::getText(this,
        QStringLiteral("New Folder"),
        QStringLiteral("Folder name:"),
        QLineEdit::Normal, PM_NEWFOLDERNAME);
    if (!name.isEmpty())
    {
        addFolder(sel, name);
        setWorkSpaceDirty(true);
    }
}

void ProjectPanel::onAddFiles()
{
    addFiles(_treeWidget->currentItem());
}

void ProjectPanel::onAddFilesRecursive()
{
    addFilesFromDirectory(_treeWidget->currentItem());
}

void ProjectPanel::onRemoveItem()
{
    QTreeWidgetItem* sel = _treeWidget->currentItem();
    if (!sel)
        return;
    delete sel;
    setWorkSpaceDirty(true);
}

void ProjectPanel::onRenameItem()
{
    _treeWidget->editItem(_treeWidget->currentItem(), 0);
    setWorkSpaceDirty(true);
}

void ProjectPanel::onModifyFilePath()
{
    auto* item = dynamic_cast<ProjectTreeItem*>(_treeWidget->currentItem());
    if (!item || item->nodeType() != nodeType_file)
        return;

    QString oldPath = item->fullPath();
    if (oldPath.isEmpty())
        return;

    FileRelocalizerDlg dlg;
    int result = dlg.doDialog(oldPath, false);
    if (result == IDOK)
    {
        QString newPath = dlg.getFullFilePath();
        if (!newPath.isEmpty() && newPath != oldPath)
        {
            // Update the fullPathCache
            int idx = _fullPathCache.indexOf(oldPath);
            if (idx >= 0)
                _fullPathCache[idx] = newPath;
            item->setFullPath(newPath);
            setWorkSpaceDirty(true);
        }
    }
}

void ProjectPanel::onFindInProjects()
{
    QStringList files = enumWorkSpaceFiles({});
    emit findInProjectsRequested(files);
}

void ProjectPanel::onMoveUp()
{
    QTreeWidgetItem* sel = _treeWidget->currentItem();
    if (!sel)
        return;

    QTreeWidgetItem* parent = sel->parent();
    if (!parent)
        return;

    int idx = parent->indexOfChild(sel);
    if (idx > 0)
    {
        parent->removeChild(sel);
        parent->insertChild(idx - 1, sel);
        _treeWidget->setCurrentItem(sel);
        setWorkSpaceDirty(true);
    }
}

void ProjectPanel::onMoveDown()
{
    QTreeWidgetItem* sel = _treeWidget->currentItem();
    if (!sel)
        return;

    QTreeWidgetItem* parent = sel->parent();
    if (!parent)
        return;

    int idx = parent->indexOfChild(sel);
    if (idx < parent->childCount() - 1)
    {
        parent->removeChild(sel);
        parent->insertChild(idx + 1, sel);
        _treeWidget->setCurrentItem(sel);
        setWorkSpaceDirty(true);
    }
}

// =============================================================================
// Helpers
// =============================================================================

void ProjectPanel::openSelectFile()
{
    auto* item = dynamic_cast<ProjectTreeItem*>(_treeWidget->currentItem());
    if (!item || item->nodeType() != nodeType_file)
        return;
    emit fileActivated(item->fullPath());
}

NodeType ProjectPanel::getNodeType(QTreeWidgetItem* hItem) const
{
    auto* item = dynamic_cast<ProjectTreeItem*>(hItem);
    return item ? item->nodeType() : nodeType_file;
}

void ProjectPanel::setWorkSpaceDirty(bool isDirty)
{
    _isDirty = isDirty;
    emit dirtyChanged(isDirty);
}

QString ProjectPanel::getAbsoluteFilePath(const QString& relativePath) const
{
    QFileInfo fi(relativePath);
    if (fi.isAbsolute())
        return relativePath;

    QFileInfo wsFi(_workSpaceFilePath);
    return wsFi.absoluteDir().absoluteFilePath(relativePath);
}

QString ProjectPanel::getRelativePath(const QString& filePath, const QString& workSpaceFileName) const
{
    QFileInfo wsFi(workSpaceFileName);
    QString wsDir = wsFi.absolutePath();

    if (!filePath.startsWith(wsDir))
        return filePath;

    QString rel = filePath.mid(wsDir.length());
    if (rel.startsWith(QDir::separator()) || rel.startsWith('/'))
        rel = rel.mid(1);
    return rel;
}

QStringList ProjectPanel::enumWorkSpaceFiles(const QStringList& patterns) const
{
    QStringList result;
    Q_UNUSED(patterns);

    std::function<void(QTreeWidgetItem*)> collect;
    collect = [&](QTreeWidgetItem* item) {
        auto* projItem = dynamic_cast<ProjectTreeItem*>(item);
        if (projItem && projItem->nodeType() == nodeType_file)
        {
            QString path = projItem->fullPath();
            if (!path.isEmpty())
                result.append(path);
        }
        for (int i = 0; i < item->childCount(); ++i)
            collect(item->child(i));
    };

    for (int i = 0; i < _treeWidget->topLevelItemCount(); ++i)
        collect(_treeWidget->topLevelItem(i));

    return result;
}

void ProjectPanel::setBackgroundColor(const QColor& col)
{
    QPalette pal = _treeWidget->palette();
    pal.setColor(QPalette::Base, col);
    pal.setColor(QPalette::AlternateBase, col);
    _treeWidget->setPalette(pal);
}

void ProjectPanel::setForegroundColor(const QColor& col)
{
    QPalette pal = _treeWidget->palette();
    pal.setColor(QPalette::Text, col);
    _treeWidget->setPalette(pal);
}

// =============================================================================
// run_dlgProc — message dispatcher
// Mirrors Win32 ProjectPanel::run_dlgProc.
// =============================================================================

intptr_t ProjectPanel::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        initMenus();
        if (!_workSpaceFilePath.isEmpty())
            readWorkSpace(_workSpaceFilePath);
        else
            newWorkSpace();
        return TRUE;

    case WM_SIZE:
    {
        if (lParam)
        {
            int width = wParam & 0xFFFF;
            int height = (wParam >> 16) & 0xFFFF;
            Q_UNUSED(width);
            Q_UNUSED(height);
            // Toolbar + tree resize handled by layout
        }
        return TRUE;
    }

    case WM_DESTROY:
        destroyMenus();
        return TRUE;

    default:
        break;
    }
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}


void ProjectPanel::setPanelTitle(const std::wstring& title) {
    _panelTitle = QString::fromStdWString(title);
}

QString ProjectPanel::getWorkSpaceFilePath() const
{
    return QString();
}

void ProjectPanel::enumWorkSpaceFiles(std::vector<QString>& paths, int type) const
{
    Q_UNUSED(paths);
    Q_UNUSED(type);
}
