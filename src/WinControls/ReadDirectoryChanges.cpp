// Semantic Lift: Win32 ReadDirectoryChanges → Qt6 DirectoryWatcher
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ReadDirectoryChanges.cpp
// Target: npp-qt/src/WinControls/ReadDirectoryChanges.cpp

#include "ReadDirectoryChanges.h"
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <QFileDevice>
#include <QSet>
#include <QDateTime>

DirectoryWatcher::DirectoryWatcher(QObject* parent)
    : QObject(parent)
{
}

DirectoryWatcher::~DirectoryWatcher()
{
    terminate();
}

bool DirectoryWatcher::init()
{
    if (_isRunning) {
        return false;
    }

    _watcher = new QFileSystemWatcher(this);
    _pollTimer = new QTimer(this);
    _pollTimer->setInterval(2000); // 2-second polling as a fallback

    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &DirectoryWatcher::onDirectoryChanged);
    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &DirectoryWatcher::onFileChanged);
    connect(_watcher, &QFileSystemWatcher::errorOccurred,
            this, &DirectoryWatcher::onWatcherError);
    connect(_pollTimer, &QTimer::timeout,
            this, &DirectoryWatcher::onDirectoryChanged);

    // Seed the known-files map for ADD vs MODIFY differentiation.
    for (const QString& path : _watchedPaths) {
        QDir dir(path);
        if (dir.exists()) {
            QDirIterator it(path, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
            while (it.hasNext()) {
                it.next();
                _knownFiles.insert(it.filePath(), it.fileTime(QFileDevice::FileModificationTime));
            }
        }
    }

    _isRunning = true;
    _pollTimer->start();
    return true;
}

void DirectoryWatcher::terminate()
{
    if (!_isRunning) {
        return;
    }

    _isRunning = false;

    if (_pollTimer) {
        _pollTimer->stop();
        _pollTimer->deleteLater();
        _pollTimer = nullptr;
    }

    if (_watcher) {
        _watcher->deleteLater();
        _watcher = nullptr;
    }

    QMutexLocker locker(&_mutex);
    _notifications.clear();
}

void DirectoryWatcher::addDirectory(const QString& directoryPath,
                                     bool watchSubtree,
                                     NotifyFilters /*filter*/,
                                     int /*bufferSize*/)
{
    if (!_watcher) {
        return;
    }

    QFileInfo info(directoryPath);
    if (!info.exists() || !info.isDir()) {
        return;
    }

    _watchedPaths.append(directoryPath);

    // QFileSystemWatcher handles recursive watching automatically when
    // the directory path is added (Linux inotify watches the dir itself;
    // recursive watching is supported via the directory path on Linux).
    // Note: On Linux, subdirectory watching requires adding each subdirectory
    // individually if the watcher doesn't handle it automatically.
    if (!_watcher->addPath(directoryPath)) {
        return;
    }

    // On Linux, to watch subdirectories we need to add them explicitly.
    // Scan and add immediate children if requested.
    if (watchSubtree) {
        QDir dir(directoryPath);
        const QFileInfoList entries = dir.entryInfoList(
            QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);
        for (const QFileInfo& fi : entries) {
            QString childPath = fi.absoluteFilePath();
            _watchedPaths.append(childPath);
            _watcher->addPath(childPath);

            // Seed known files for subdirectory too.
            QDirIterator subit(childPath, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
            while (subit.hasNext()) {
                subit.next();
                _knownFiles.insert(subit.filePath(),
                    subit.fileTime(QFileDevice::FileModificationTime));
            }
        }
    }

    // Seed known files for the root directory.
    QDirIterator it(directoryPath, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    while (it.hasNext()) {
        it.next();
        _knownFiles.insert(it.filePath(),
            it.fileTime(QFileDevice::FileModificationTime));
    }
}

void DirectoryWatcher::processPath(const QString& path, quint32 action)
{
    QMutexLocker locker(&_mutex);
    _notifications.append({action, path});
    _notEmpty.wakeOne();
}

void DirectoryWatcher::onDirectoryChanged(const QString& path)
{
    QFileInfo info(path);
    if (!info.exists()) {
        // Directory was deleted or renamed.
        processPath(path, FILE_ACTION_REMOVED);
        emit watcherError(QStringLiteral("Watched directory removed: %1").arg(path));
        return;
    }

    // Scan the directory to detect ADDED and REMOVED files.
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    QSet<QString> currentFiles;

    QDirIterator it(path, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    while (it.hasNext()) {
        it.next();
        currentFiles.insert(it.filePath());
    }

    // Compare against known files.
    {
        QMutexLocker locker(&_mutex);
        for (const QString& file : currentFiles) {
            if (!_knownFiles.contains(file)) {
                // New file
                _knownFiles.insert(file, QDateTime::currentDateTime());
                processPath(file, FILE_ACTION_ADDED);
                emit fileAdded(file);
            }
        }

        QStringList toRemove;
        for (auto it2 = _knownFiles.constKeyValueBegin();
             it2 != _knownFiles.constKeyValueEnd(); ++it2) {
            if ((*it2).first.startsWith(path) && !currentFiles.contains((*it2).first)) {
                processPath((*it2).first, FILE_ACTION_REMOVED);
                emit fileRemoved((*it2).first);
                toRemove.append((*it2).first);
            }
        }
        for (const QString& r : toRemove) {
            _knownFiles.remove(r);
        }
    }

    // Also handle direct subdirectory changes for recursive mode.
    const QFileInfoList dirs = dir.entryInfoList(
        QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);
    for (const QFileInfo& fi : dirs) {
        QString childPath = fi.absoluteFilePath();
        if (!_watchedPaths.contains(childPath)) {
            // New subdirectory discovered — add it to the watcher.
            _watchedPaths.append(childPath);
            if (_watcher) {
                _watcher->addPath(childPath);
            }
            processPath(childPath, FILE_ACTION_ADDED);
        }
    }

    emit directoryChanged(FILE_ACTION_MODIFIED, path);
}

void DirectoryWatcher::onFileChanged(const QString& path)
{
    QFileInfo info(path);
    if (info.exists()) {
        processPath(path, FILE_ACTION_MODIFIED);
        emit fileModified(path);
    } else {
        {
            QMutexLocker locker(&_mutex);
            _knownFiles.remove(path);
        }
        processPath(path, FILE_ACTION_REMOVED);
        emit fileRemoved(path);
    }
}

void DirectoryWatcher::onWatcherError(const QString& error)
{
    emit this->error(error);
}

void DirectoryWatcher::push(quint32 action, const QString& fileName)
{
    QMutexLocker locker(&_mutex);
    _notifications.append({action, fileName});
    _notEmpty.wakeOne();
}

bool DirectoryWatcher::pop(quint32& outAction, QString& outFileName, int waitMs)
{
    QMutexLocker locker(&_mutex);

    if (_notifications.isEmpty()) {
        if (waitMs > 0) {
            _notEmpty.wait(&_mutex, static_cast<unsigned long>(waitMs));
        } else if (waitMs < 0) {
            // Negative = infinite wait (but check periodically via processEvents)
            while (_notifications.isEmpty() && _isRunning) {
                _notEmpty.wait(&_mutex, 100);
            }
        }
        if (_notifications.isEmpty()) {
            return false;
        }
    }

    DirectoryChangeNotification n = _notifications.dequeue();
    outAction = n.action;
    outFileName = n.fileName;
    return true;
}

int DirectoryWatcher::pendingCount() const
{
    QMutexLocker locker(&_mutex);
    return _notifications.count();
}
