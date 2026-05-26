// MISC/Common/DirWatcher.cpp - Qt6 port of directory watching
#include "DirWatcher.h"

DirWatcher::DirWatcher(QObject* parent)
    : QObject(parent)
{
    // Connect filesystem watcher signals
    connect(&_watcher, &QFileSystemWatcher::fileChanged, this, &DirWatcher::onFileChanged);
    connect(&_watcher, &QFileSystemWatcher::directoryChanged, this, &DirWatcher::onDirectoryChanged);
    
    // Setup debounce timer to prevent rapid-fire events
    _debounceTimer.setSingleShot(true);
    _debounceTimer.setInterval(100);
    connect(&_debounceTimer, &QTimer::timeout, this, &DirWatcher::processPendingChanges);
}

DirWatcher::~DirWatcher()
{
    clearAll();
}

bool DirWatcher::watchDirectory(const QString& dirPath)
{
    if (_watcher.addPath(dirPath)) {
        return true;
    }
    emit errorOccurred(QString("Failed to watch directory: %1").arg(dirPath));
    return false;
}

bool DirWatcher::watchFile(const QString& filePath)
{
    if (_watcher.addPath(filePath)) {
        return true;
    }
    emit errorOccurred(QString("Failed to watch file: %1").arg(filePath));
    return false;
}

void DirWatcher::removeWatch(const QString& path)
{
    _watcher.removePath(path);
    _pendingChanges.removeAll(path);
}

void DirWatcher::clearAll()
{
    _watcher.removePaths(_watcher.files());
    _watcher.removePaths(_watcher.directories());
    _pendingChanges.clear();
}

QStringList DirWatcher::watchedPaths() const
{
    QStringList paths = _watcher.files();
    paths.append(_watcher.directories());
    return paths;
}

void DirWatcher::onFileChanged(const QString& path)
{
    if (!_pendingChanges.contains(path)) {
        _pendingChanges.append(path);
    }
    _debounceTimer.start();
}

void DirWatcher::onDirectoryChanged(const QString& path)
{
    if (!_pendingChanges.contains(path)) {
        _pendingChanges.append(path);
    }
    _debounceTimer.start();
}

void DirWatcher::processPendingChanges()
{
    for (const QString& path : _pendingChanges) {
        // Check if it's a file or directory
        QFileInfo fi(path);
        if (fi.isDir()) {
            emit directoryChanged(path);
        } else {
            emit fileChanged(path);
        }
    }
    _pendingChanges.clear();
}

// DirWatcherManager implementation
DirWatcherManager::DirWatcherManager()
{
}

DirWatcherManager::~DirWatcherManager()
{
    qDeleteAll(_watchers);
}

DirWatcherManager& DirWatcherManager::getInstance()
{
    static DirWatcherManager instance;
    return instance;
}

DirWatcher* DirWatcherManager::createWatcher(const QString& name)
{
    if (_watchers.contains(name)) {
        return _watchers[name];
    }
    
    DirWatcher* watcher = new DirWatcher();
    _watchers[name] = watcher;
    return watcher;
}

DirWatcher* DirWatcherManager::getWatcher(const QString& name) const
{
    return _watchers.value(name, nullptr);
}

void DirWatcherManager::removeWatcher(const QString& name)
{
    if (_watchers.contains(name)) {
        delete _watchers[name];
        _watchers.remove(name);
    }
}

QStringList DirWatcherManager::getAllWatcherNames() const
{
    return _watchers.keys();
}