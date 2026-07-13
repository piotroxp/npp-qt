// Semantic Lift: ProjectPanel — Win32 TreeView + project files → Qt6 QTreeWidget + .proj
// Original: PowerEditor/src/WinControls/ProjectPanel/ProjectPanel.cpp (1432 lines)
// Win32 → Qt6: SHGetFileInfo + file ops → QTreeWidget + QFile + QDir

#include "ProjectPanel.h"
#include "NppDarkMode.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QUuid>
#include <QMenu>
#include <QContextMenuEvent>

ProjectPanel::ProjectPanel(QWidget* parent)
    : QWidget(parent)
    , _treeWidget(new QTreeWidget(this))
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(_treeWidget);

    _treeWidget->setHeaderLabel(QStringLiteral("Projects"));
    _treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    NppDarkMode::instance().setTreeViewStyle(_treeWidget);
}

ProjectPanel::~ProjectPanel()
{
}

void ProjectPanel::init(QWidget* parent)
{
    Q_UNUSED(parent);
}

void ProjectPanel::destroy()
{
    _treeWidget->clear();
    _currentWorkSpacePath.clear();
}

void ProjectPanel::addProject(const QString& name)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(_treeWidget);
    item->setText(0, name);
    item->setData(0, Qt::UserRole, QStringLiteral("project"));
    item->setIcon(0, folderIcon());
    item->setExpanded(false);
    _treeWidget->addTopLevelItem(item);
    _currentProjectName = name;
}

void ProjectPanel::removeProject(const QString& name)
{
    for (int i = 0; i < _treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _treeWidget->topLevelItem(i);
        if (item->text(0) == name) {
            delete item;
            break;
        }
    }
}

void ProjectPanel::addFolderToProject(const QString& projectName, const QString& folderPath)
{
    QTreeWidgetItem* projectItem = nullptr;
    for (int i = 0; i < _treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _treeWidget->topLevelItem(i);
        if (item->text(0) == projectName) {
            projectItem = item;
            break;
        }
    }
    if (!projectItem) return;

    QDir dir(folderPath);
    if (!dir.exists()) return;

    QTreeWidgetItem* folderItem = new QTreeWidgetItem();
    folderItem->setText(0, dir.dirName());
    folderItem->setData(0, Qt::UserRole, QStringLiteral("folder"));
    folderItem->setData(0, Qt::UserRole + 1, folderPath);
    folderItem->setIcon(0, folderIcon());
    folderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    projectItem->addChild(folderItem);

    // Add child files
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& fi : entries) {
        QTreeWidgetItem* childItem = new QTreeWidgetItem();
        childItem->setText(0, fi.fileName());
        childItem->setData(0, Qt::UserRole, QStringLiteral("file"));
        childItem->setData(0, Qt::UserRole + 1, fi.absoluteFilePath());
        childItem->setIcon(0, fileIcon(fi));
        folderItem->addChild(childItem);
    }

    projectItem->setExpanded(true);
    _isModified = true;
}

void ProjectPanel::addFileToFolder(const QString& folderPath, const QString& filePath)
{
    QTreeWidgetItem* folderItem = findFolderItem(_treeWidget->invisibleRootItem(), folderPath);
    if (!folderItem) return;

    QFileInfo fi(filePath);
    QTreeWidgetItem* fileItem = new QTreeWidgetItem();
    fileItem->setText(0, fi.fileName());
    fileItem->setData(0, Qt::UserRole, QStringLiteral("file"));
    fileItem->setData(0, Qt::UserRole + 1, filePath);
    fileItem->setIcon(0, fileIcon(fi));
    folderItem->addChild(fileItem);
    _isModified = true;
}

void ProjectPanel::removeItem(const QString& itemPath)
{
    QTreeWidgetItem* item = findItemByPath(_treeWidget->invisibleRootItem(), itemPath);
    if (item && item->parent()) {
        item->parent()->removeChild(item);
        delete item;
        _isModified = true;
    }
}

void ProjectPanel::openFile(const QString& filePath)
{
    emit fileOpened(filePath);
}

void ProjectPanel::createNewFile(const QString& folderPath)
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("New File"),
                                             tr("File name:"), QLineEdit::Normal,
                                             QString(), &ok);
    if (ok && !fileName.isEmpty()) {
        QString fullPath = folderPath + QDir::separator() + fileName;
        QFile file(fullPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            addFileToFolder(folderPath, fullPath);
        }
    }
}

void ProjectPanel::createNewFolder(const QString& parentFolder)
{
    bool ok;
    QString folderName = QInputDialog::getText(this, tr("New Folder"),
                                               tr("Folder name:"), QLineEdit::Normal,
                                               QString(), &ok);
    if (ok && !folderName.isEmpty()) {
        QString fullPath = parentFolder + QDir::separator() + folderName;
        QDir dir;
        if (dir.mkpath(fullPath)) {
            addFolderToProject(_currentProjectName, fullPath);
        }
    }
}

void ProjectPanel::newWorkSpace()
{
    bool ok;
    QString wsName = QInputDialog::getText(this, tr("New Workspace"),
                                            tr("Workspace name:"), QLineEdit::Normal,
                                            QString(), &ok);
    if (ok && !wsName.isEmpty()) {
        _currentWorkSpacePath.clear();
        _treeWidget->clear();
        addProject(wsName);
        _isModified = true;
    }
}

void ProjectPanel::openWorkSpace(const QString& wsFilePath)
{
    QFile file(wsFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Parse .workspace file (simple XML format)
    // For now, treat wsFilePath's directory as root
    QFileInfo fi(wsFilePath);
    _currentWorkSpacePath = wsFilePath;
    _rootPath = fi.absolutePath();
    _treeWidget->clear();

    // Load projects from workspace file
    // (simplified — real implementation would parse XML)
    QString wsDir = fi.absolutePath();
    QDir dir(wsDir);
    QStringList projectDirs = dir.entryList(QStringList() << QStringLiteral("*.proj"), QDir::Dirs);
    for (const QString& projDir : projectDirs) {
        addProject(projDir);
        addFolderToProject(projDir, wsDir + QDir::separator() + projDir);
    }

    _isModified = false;
}

void ProjectPanel::saveWorkSpace()
{
    if (_currentWorkSpacePath.isEmpty()) {
        QString path = QFileDialog::getSaveFileName(this, tr("Save Workspace"),
            QString(), QStringLiteral("Workspace (*.workspace)"));
        if (path.isEmpty()) return;
        _currentWorkSpacePath = path;
    }

    QFile file(_currentWorkSpacePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<NotepadPlus>\n";
    out << "  <WorkSpace name=\"" << _currentProjectName << "\">\n";

    // Save project structure
    for (int i = 0; i < _treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* proj = _treeWidget->topLevelItem(i);
        out << "    <Project name=\"" << proj->text(0) << "\">\n";
        saveProjectItems(out, proj, 4);
        out << "    </Project>\n";
    }

    out << "  </WorkSpace>\n";
    out << "</NotepadPlus>\n";
    file.close();
    _isModified = false;
}

bool ProjectPanel::isModified() const
{
    return _isModified;
}

void ProjectPanel::setFileFilter(const QString& filter)
{
    _fileFilter = filter;
}

QString ProjectPanel::getFileFilter() const
{
    return _fileFilter;
}

QIcon ProjectPanel::folderIcon()
{
    static QIcon icon;
    if (icon.isNull()) {
        // Use a standard folder icon
        icon = QIcon::fromTheme(QStringLiteral("folder"));
    }
    return icon;
}

QIcon ProjectPanel::fileIcon(const QFileInfo& fi)
{
    static QFileIconProvider provider;
    return provider.icon(fi);
}
