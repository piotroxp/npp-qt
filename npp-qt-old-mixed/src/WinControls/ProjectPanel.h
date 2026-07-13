// Semantic Lift: Win32 ProjectPanel → Qt6 ProjectPanel
// Original: PowerEditor/src/WinControls/ProjectPanel/ProjectPanel.h
// Target: npp-qt/src/WinControls/ProjectPanel.h

#pragma once

#include "DockingWnd.h"
#include "StaticDialog.h"
#include "TreeView.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QVector>
#include <QString>
#include <vector>
#include <QToolBar>
#include <memory>
#include <string>

// Panel title constants (mirrors PM_* Win32 defines)
const QString PM_PROJECTPANELTITLE   = QStringLiteral("Project Panel");
const QString PM_WORKSPACEROOTNAME   = QStringLiteral("Workspace");
const QString PM_NEWFOLDERNAME       = QStringLiteral("New Folder");
const QString PM_NEWPROJECTNAME      = QStringLiteral("New Project");

// Node type enumeration (mirrors Win32 NodeType)
enum NodeType { nodeType_root = 0, nodeType_project = 1, nodeType_folder = 2, nodeType_file = 3 };

// Menu command IDs (mirrors menuCmdID.h IDM_PROJECT_*)
const int IDM_PROJECT_NEWWS          = 0;
const int IDM_PROJECT_OPENWS         = 1;
const int IDM_PROJECT_RELOADWS       = 2;
const int IDM_PROJECT_SAVEWS         = 3;
const int IDM_PROJECT_SAVEASWS       = 4;
const int IDM_PROJECT_SAVEACOPYASWS  = 5;
const int IDM_PROJECT_NEWPROJECT     = 6;
const int IDM_PROJECT_FINDINPROJECTSWS = 7;
const int IDM_PROJECT_MOVEUP         = 8;
const int IDM_PROJECT_MOVEDOWN       = 9;
const int IDM_PROJECT_RENAME         = 10;
const int IDM_PROJECT_NEWFOLDER     = 11;
const int IDM_PROJECT_ADDFILES      = 12;
const int IDM_PROJECT_ADDFILESRECUSIVELY = 13;
const int IDM_PROJECT_DELETEFOLDER   = 14;
const int IDM_PROJECT_DELETEFILE    = 15;
const int IDM_PROJECT_MODIFYFILEPATH = 16;

// =============================================================================
// ProjectTreeItem — tree item with type tag and metadata
// Mirrors Win32 HTREEITEM + TVITEM + custom lParam.
// =============================================================================

class ProjectTreeItem : public QTreeWidgetItem
{
public:
    explicit ProjectTreeItem(QTreeWidget* parent = nullptr);
    explicit ProjectTreeItem(QTreeWidgetItem* parent = nullptr);
    ~ProjectTreeItem() override = default;

    NodeType nodeType() const { return _nodeType; }
    void setNodeType(NodeType type) { _nodeType = type; }

    QString fullPath() const { return _fullPath; }
    void setFullPath(const QString& path) { _fullPath = path; }

    bool isDirty() const { return _isDirty; }
    void setDirty(bool dirty) { _isDirty = dirty; }

    // Load/save to XML (mirrors Win32 NppXml)
    void loadFromXml(QXmlStreamReader& xml);
    void saveToXml(QXmlStreamWriter& xml) const;

private:
    NodeType _nodeType = nodeType_root;
    QString _fullPath;
    bool _isDirty = false;
};

// =============================================================================
// FileRelocalizerDlg — dialog to modify file path when file moved
// Mirrors Win32 FileRelocalizerDlg (IDD_RELOCALIZE_FILEPATH).
// =============================================================================

class FileRelocalizerDlg : public QDialog
{
    Q_OBJECT
public:
    FileRelocalizerDlg();
    ~FileRelocalizerDlg() override = default;

    int doDialog(const QString& fn, bool isRTL = false);

    const QString& getFullFilePath() const { return _fullFilePath; }

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam);

private:
    QString _fullFilePath;
};

// =============================================================================
// ProjectPanel — multi-root workspace file browser
// Mirrors Win32 ProjectPanel (DockingDlgInterface + TreeView + context menus).
// =============================================================================

class ProjectPanel : public DockingDlgInterface
{
    Q_OBJECT

public:
    explicit ProjectPanel();
    ~ProjectPanel() override = default;

    void init(QApplication* app, QWidget* parent, int panelID);
    void setParentWindow(QWidget* parent) { _hParent = parent; }
    void enumWorkSpaceFiles(void* /*parent*/, std::vector<std::wstring>& /*files*/, std::vector<std::wstring>& /*dirs*/) const {}

    // Workspace management
    void newWorkSpace();
    bool saveWorkspaceRequest();
    bool openWorkSpace(const QString& projectFileName, bool force = false);
    bool saveWorkSpace();
    bool saveWorkSpaceAs(bool saveCopyAs = false);
    bool isClosed() const { return false; }  // stub: panel never fully closed
    QString getWorkSpaceFilePath() const;  // stub
    void enumWorkSpaceFiles(std::vector<QString>& paths, int type) const;  // stub
    void setWorkSpaceFilePath(const QString& path) { _workSpaceFilePath = path; }
    QString workSpaceFilePath() const { return _workSpaceFilePath; }
    bool isDirty() const { return _isDirty; }
    bool checkIfNeedSave();

    // Win32 compat: setText(bool) — show/hide panel
    void setText(bool show = true) { display(show); }
    void setText(const QString& /*title*/) { /* stub */ }

    // Colors
    void setBackgroundColor(const QColor& col) override;
    void setForegroundColor(const QColor& col) override;

    // Panel title (mirrors Win32 DockingDlgInterface method)
    QString getPanelTitle() const { return _panelTitle; }
    void setPanelTitle(const QString& title) { _panelTitle = title; }
    void setPanelTitle(const std::wstring& title);

    // File enumeration
    QStringList enumWorkSpaceFiles(const QStringList& patterns) const;
    void collectFilesRecursive(QTreeWidgetItem* item, std::vector<QString>& paths) const;

signals:
    void fileActivated(const QString& path);
    void workSpaceSaved(const QString& path);
    void dirtyChanged(bool isDirty);
    void findInProjectsRequested(const QStringList& files);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenu(const QPoint& pos);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);

    // Context menu commands
    void onNewWorkspace();
    void onOpenWorkspace();
    void onSaveWorkspace();
    void onSaveAsWorkspace();
    void onSaveACopyAsWorkspace();
    void onReloadWorkspace();
    void onAddFolder();
    void onAddFiles();
    void onAddFilesRecursive();
    void onRemoveItem();
    void onRenameItem();
    void onModifyFilePath();
    void onFindInProjects();
    void onMoveUp();
    void onMoveDown();

private:
    void initMenus();
    void destroyMenus();
    void addFiles(QTreeWidgetItem* hTreeItem);
    void addFilesFromDirectory(QTreeWidgetItem* hTreeItem);
    void recursiveAddFilesFrom(const QString& folderPath, QTreeWidgetItem* hTreeItem);
    ProjectTreeItem* addFolder(QTreeWidgetItem* hTreeItem, const QString& folderName);
    ProjectTreeItem* addFile(QTreeWidgetItem* hTreeItem, const QString& filePath);

    bool writeWorkSpace(const QString& projectFileName = QString(), bool doUpdateGUI = true);
    bool readWorkSpace(const QString& projectFileName);
    void buildProjectXml(QXmlStreamWriter& xml, QTreeWidgetItem* hItem) const;
    NodeType getNodeType(QTreeWidgetItem* hItem) const;
    void setWorkSpaceDirty(bool isDirty);
    void popupMenuCmd(int cmdID);
    QString getAbsoluteFilePath(const QString& relativePath) const;
    void openSelectFile();
    bool buildTreeFrom(QXmlStreamReader& xml, ProjectTreeItem* parent);
    QString getRelativePath(const QString& filePath, const QString& workSpaceFileName) const;

    // Toolbar
    QToolBar* _toolbar = nullptr;
    QAction* _newWsAction = nullptr;
    QAction* _openWsAction = nullptr;
    QAction* _saveWsAction = nullptr;
    QAction* _foldAllAction = nullptr;
    QAction* _expandAllAction = nullptr;
    QAction* _findInProjectsAction = nullptr;

    // Tree widget
    QTreeWidget* _treeWidget = nullptr;

    // Context menus
    QMenu* _workspaceMenu = nullptr;
    QMenu* _projectMenu = nullptr;
    QMenu* _folderMenu = nullptr;
    QMenu* _fileMenu = nullptr;

    // State
    QString _workSpaceFilePath;
    QString _panelTitle = PM_PROJECTPANELTITLE;
    bool _isDirty = false;
    int _panelID = 0;
    QWidget* _hParent = nullptr;

    // Cached full paths for relative resolution (mirrors _fullPathStrs)
    QVector<QString> _fullPathCache;
};
