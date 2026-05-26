// MISC/Common/DirWatcher.h - Qt6 port of directory watching
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMap>
#include <QFileInfo>

class DirWatcher : public QObject
{
    Q_OBJECT
public:
    explicit DirWatcher(QObject* parent = nullptr);
    ~DirWatcher();
    
    // Watch management
    bool watchDirectory(const QString& dirPath);
    bool watchFile(const QString& filePath);
    void removeWatch(const QString& path);
    void clearAll();
    
    // Status
    bool isWatching() const { return !_watcher.files().isEmpty() || !_watcher.directories().isEmpty(); }
    QStringList watchedPaths() const;
    
    // Signals for file changes
signals:
    void fileChanged(const QString& path);
    void directoryChanged(const QString& path);
    void errorOccurred(const QString& error);
    
private slots:
    void onFileChanged(const QString& path);
    void onDirectoryChanged(const QString& path);
    void onTimeout();
    
private:
    QFileSystemWatcher _watcher;
    QTimer _debounceTimer;
    QStringList _pendingChanges;
    
    void processPendingChanges();
};

class DirWatcherManager
{
public:
    static DirWatcherManager& getInstance();
    
    DirWatcher* createWatcher(const QString& name);
    DirWatcher* getWatcher(const QString& name) const;
    void removeWatcher(const QString& name);
    QStringList getAllWatcherNames() const;
    
private:
    DirWatcherManager();
    ~DirWatcherManager();
    
    QMap<QString, DirWatcher*> _watchers;
};