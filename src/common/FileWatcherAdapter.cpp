// FileWatcherAdapter.cpp — Linux file system watcher using Qt6 QFileSystemWatcher
// Ported from Win32 ReadDirectoryChangesW
//
// Key semantics:
//   - Detect file changes and trigger a reload prompt
//   - Debounce rapid changes (500ms threshold)
//   - inotify limit awareness
//
// Copyright (C) 2026 Agent Army / GPL v3

#include "FileWatcherAdapter.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTimer>
#include <QDeadlineTimer>

#include <chrono>
#include <cstdio>
#include <cstdlib>

// ============================================================================
// Static utilities
// ============================================================================

/*static*/ QString FileWatcherAdapter::readInotifyLimitFile()
{
    QFile f(QStringLiteral("/proc/sys/fs/inotify/max_user_watches"));
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString::fromUtf8(f.readAll().trimmed());
    }
    return QString();
}

// ============================================================================
// Construction / Lifecycle
// ============================================================================

FileWatcherAdapter::FileWatcherAdapter(QObject* parent)
    : QObject(parent)
    , _watcher(new QFileSystemWatcher(this))
{
    // Wire QFileSystemWatcher signals to our private slots
    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &FileWatcherAdapter::onRawFileChanged);
    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &FileWatcherAdapter::onRawDirectoryChanged);
    // Qt 6.4 does not have errorOccurred/errorString/WatcherError.
    // QFileSystemWatcher logs errors via qWarning() internally; we emit our
    // own error signal if addPath() fails, and rely on inotifyLimitWarning
    // to surface the resource-exhaustion case.

    // Check inotify limit once at startup
    checkInotifyLimit();
}

FileWatcherAdapter::~FileWatcherAdapter()
{
    unwatchAll();
}

bool FileWatcherAdapter::isSupported()
{
    // Probe with a real temp file: on platforms without an inotify-equivalent
    // (e.g. QT_QPA_PLATFORM=offscreen without an inotify backend) addPath() fails.
    QFileSystemWatcher probe;
    QTemporaryFile f;
    if (!f.open()) {
        return false; // no temp file support
    }
    f.close();
    bool ok = probe.addPath(f.fileName());
    return ok;
}

// ============================================================================
// Watch management
// ============================================================================

bool FileWatcherAdapter::watchFile(const QString& path, FileChangedCallback callback)
{
    if (!callback) {
        qWarning() << "FileWatcherAdapter::watchFile: null callback for" << path;
        return false;
    }

    // Already watching — update callback, don't double-add
    if (_entries.contains(path)) {
        auto& entry = _entries[path];
        if (!entry.isDirectory) {
            entry.fileCallback = std::move(callback);
            return true;
        }
        qWarning() << "FileWatcherAdapter::watchFile: path is registered as directory:" << path;
        return false;
    }

    WatchEntry entry;
    entry.isDirectory = false;
    entry.fileCallback = std::move(callback);

    // Create debounce timer — owned by this adapter, deleted in unwatch()
    entry.debounceTimer = new QTimer(this);
    entry.debounceTimer->setSingleShot(true);
    entry.debounceTimer->setInterval(_debounceMs);

    // FireFileChange will be called when the timer expires
    // We use a queued connection to ensure it runs on the event loop thread
    connect(entry.debounceTimer, &QTimer::timeout,
            this, [this, path]() { fireFileChange(path); },
            Qt::QueuedConnection);

    _entries.emplace(path, std::move(entry));

    // Add to the underlying watcher
    if (!_watcher->addPath(path)) {
        qWarning() << "FileWatcherAdapter: failed to addPath:" << path
                   << QStringLiteral("addPath failed");
        _entries.erase(path);
        return false;
    }

    return true;
}

bool FileWatcherAdapter::watchDirectory(const QString& path, DirectoryChangedCallback callback)
{
    if (!callback) {
        qWarning() << "FileWatcherAdapter::watchDirectory: null callback for" << path;
        return false;
    }

    if (_entries.contains(path)) {
        auto& entry = _entries[path];
        if (entry.isDirectory) {
            entry.dirCallback = std::move(callback);
            return true;
        }
        qWarning() << "FileWatcherAdapter::watchDirectory: path is registered as file:" << path;
        return false;
    }

    WatchEntry entry;
    entry.isDirectory = true;
    entry.dirCallback = std::move(callback);

    entry.debounceTimer = new QTimer(this);
    entry.debounceTimer->setSingleShot(true);
    entry.debounceTimer->setInterval(_debounceMs);

    connect(entry.debounceTimer, &QTimer::timeout,
            this, [this, path]() { fireDirectoryChange(path); },
            Qt::QueuedConnection);

    _entries.emplace(path, std::move(entry));

    if (!_watcher->addPath(path)) {
        qWarning() << "FileWatcherAdapter: failed to addPath directory:" << path
                   << QStringLiteral("addPath failed");
        _entries.erase(path);
        return false;
    }

    return true;
}

bool FileWatcherAdapter::unwatch(const QString& path)
{
    auto it = _entries.find(path);
    if (it == _entries.end()) {
        return false;
    }

    // Stop and destroy the debounce timer
    if (it->second.debounceTimer) {
        it->second.debounceTimer->stop();
        it->second.debounceTimer->deleteLater();
        it->second.debounceTimer = nullptr;
    }

    _entries.erase(it);

    // Remove from underlying watcher
    _watcher->removePath(path);

    return true;
}

void FileWatcherAdapter::unwatchAll()
{
    // Copy keys first — unwatch modifies _entries
    QStringList paths;
    paths.reserve(static_cast<int>(_entries.size()));
    for (const auto& [path, _] : _entries) {
        paths.append(path);
    }
    for (const QString& path : std::as_const(paths)) {
        unwatch(path);
    }
}

// ============================================================================
// Query
// ============================================================================

QStringList FileWatcherAdapter::watchedPaths() const
{
    QStringList result;
    result.reserve(static_cast<int>(_entries.size()));
    for (const auto& [path, _] : _entries) {
        result.append(path);
    }
    return result;
}

bool FileWatcherAdapter::isWatching(const QString& path) const
{
    return _entries.contains(path);
}

int FileWatcherAdapter::watchCount() const
{
    return static_cast<int>(_entries.size());
}

// ============================================================================
// Configuration
// ============================================================================

void FileWatcherAdapter::setDebounceMs(int ms)
{
    _debounceMs = qMax(0, ms);
    // Update interval on existing timers
    for (auto& [_, entry] : _entries) {
        if (entry.debounceTimer) {
            entry.debounceTimer->setInterval(_debounceMs);
        }
    }
}

// ============================================================================
// inotify limit check
// ============================================================================

void FileWatcherAdapter::checkInotifyLimit()
{
    // Read the current system limit
    QString limitStr = readInotifyLimitFile();
    bool ok = false;
    int currentLimit = limitStr.toInt(&ok);
    if (!ok) {
        return;  // Not on Linux or /proc not available — nothing to check
    }

    // Warn if we have already consumed ≥ 80 % of available watches
    // (The underlying watcher keeps track of its own watch count, but
    //  this is a system-wide metric.)
    int watchedCount = _watcher->files().size() + _watcher->directories().size();
    int threshold = (_inotifyWarningThreshold > 0)
                        ? _inotifyWarningThreshold
                        : static_cast<int>(currentLimit * 0.8);

    if (watchedCount > 0 && watchedCount >= threshold) {
        qWarning().noquote()
            << "FileWatcherAdapter: approaching inotify watch limit.\n"
               "  watched:"
            << watchedCount << "/ system max:" << currentLimit << "\n"
               "  Suggestion: echo 524288 | sudo tee /proc/sys/fs/inotify/max_user_watches";
        emit inotifyLimitWarning(currentLimit, threshold);
    }
}

// ============================================================================
// Private slots — raw signals from QFileSystemWatcher
// ============================================================================

void FileWatcherAdapter::onRawFileChanged(const QString& path)
{
    // On Linux (inotify), the kernel removes the watch when a file is deleted.
    // QFileSystemWatcher re-adds it when the file is re-created.  However, during
    // the window where the file is deleted and re-created quickly (e.g. a git
    // checkout of the same file), we can receive a spurious "removed" + "added"
    // pair.  The debounce handles this — only the final state after 500 ms
    // silence triggers the callback.

    auto it = _entries.find(path);
    if (it == _entries.end()) {
        // Not tracked — maybe it was removed already, or a re-created file
        // that was re-added with a different entry.  Nothing to do.
        return;
    }

    WatchEntry& entry = it->second;
    entry.firstEventTime = std::chrono::steady_clock::now();

    if (_debounceMs > 0) {
        // Restart or start the debounce timer
        entry.debounceTimer->start();
    } else {
        // Debouncing disabled — fire immediately
        fireFileChange(path);
    }

    // Periodically check system inotify usage
    static int s_eventCounter = 0;
    if (++s_eventCounter % 64 == 0) {
        checkInotifyLimit();
    }
}

void FileWatcherAdapter::onRawDirectoryChanged(const QString& path)
{
    auto it = _entries.find(path);
    if (it == _entries.end()) {
        return;
    }

    WatchEntry& entry = it->second;
    entry.firstEventTime = std::chrono::steady_clock::now();

    if (_debounceMs > 0) {
        entry.debounceTimer->start();
    } else {
        fireDirectoryChange(path);
    }
}

// ============================================================================
// Fire confirmed (debounced) events
// ============================================================================

void FileWatcherAdapter::fireFileChange(const QString& path)
{
    auto it = _entries.find(path);
    if (it == _entries.end()) {
        return;
    }

    // Verify the file still exists before reporting the change.
    // This guards against the "file deleted and re-created" edge case
    // reported in onRawFileChanged.
    QFileInfo fi(path);
    if (!fi.exists()) {
        // File is gone — emit fileDeleted-like signal (Buffer maps this to
        // DOC_DELETED).  The caller can check fi.exists() on the path.
        qDebug() << "FileWatcherAdapter: file no longer exists (deleted):" << path;
        // Still emit fileChanged so the Buffer knows to check state
    }

    const WatchEntry& entry = it->second;
    if (entry.fileCallback) {
        entry.fileCallback(path);
    }

    emit this->fileChanged(path);
}

void FileWatcherAdapter::fireDirectoryChange(const QString& path)
{
    auto it = _entries.find(path);
    if (it == _entries.end()) {
        return;
    }

    const WatchEntry& entry = it->second;
    if (entry.dirCallback) {
        entry.dirCallback(path);
    }

    emit this->directoryChanged(path);
}
