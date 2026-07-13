#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTreeWidget>

class ProjectPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectPanel(QWidget* parent = nullptr);
    ~ProjectPanel() override;

    void init(QWidget* parent);
    void destroy();

    // Project management
    void addProject(const QString& name);
    void removeProject(const QString& name);
    void addFolderToProject(const QString& projectName, const QString& folderPath);
    void addFileToFolder(const QString& folderPath, const QString& filePath);
    void removeItem(const QString& itemPath);

    // File operations
    void openFile(const QString& filePath);
    void createNewFile(const QString& folderPath);
    void createNewFolder(const QString& parentFolder);

    // Workspace
    void newWorkSpace();
    void openWorkSpace(const QString& wsFilePath);
    void saveWorkSpace();
    bool isModified() const;

    // Filters
    void setFileFilter(const QString& filter);
    QString getFileFilter() const;

    // Path helpers
    QString getRootProjectPath() const;
    QString getSelectedItemPath() const;
    bool isProjectNode(void* hItem) const;
    bool isFolderNode(void* hItem) const;
    bool isFileNode(void* hItem) const;

    // Context menu actions
    void setAsActiveProject(const QString& name);
    void closeWorkSpace();
    void deleteSelected();
    void renameSelected(const QString& newName);

signals:
    void fileOpened(const QString& filePath);
    void projectChanged(const QString& projectName);

private:
    QTreeWidget* _treeWidget;
    QString _currentWorkSpacePath;
    QString _currentProjectName;
    QString _fileFilter;
    bool _isModified = false;
    QString _rootPath;
};

