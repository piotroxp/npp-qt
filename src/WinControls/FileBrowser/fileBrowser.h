// fileBrowser.h — Qt6 translation: folder tree view
#pragma once

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>
#include <QFileSystemWatcher>

class FileBrowser : public QDockWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget* parent = nullptr);
    ~FileBrowser() override;

    // Add root folder
    void addRootFolder(const QString& path);

    // Selection
    QString getSelectedItemPath() const;
    bool selectItemFromPath(const QString& path);

    // Refresh
    void deleteAllRoots();

    // File watching
    void startWatching(const QString& path);
    void stopWatching(const QString& path);

signals:
    void fileActivated(const QString& path);
    void folderContextMenuRequested(const QString& path, const QPoint& pos);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onActivated(const QModelIndex& index);
    void onCustomContextMenu(const QPoint& pos);
    void onDirectoryChanged(const QString& path);

private:
    QTreeView* _treeView = nullptr;
    QFileSystemModel* _model = nullptr;
    QFileSystemWatcher* _watcher = nullptr;
    QStringList _rootPaths;
};
