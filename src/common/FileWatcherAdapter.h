// FileWatcherAdapter.h — Linux file system watcher using Qt6 QFileSystemWatcher
// Ported from Win32 ReadDirectoryChangesW (notepad-plus-plus-translation/PowerEditor/src/WinControls/ReadDirectoryChanges/)
//
// Key semantics preserved:
//   - Detect file changes and trigger a reload prompt
//   - Debounce rapid changes (500ms threshold)
//   - inotify limit awareness (/proc/sys/fs/inotify/max_user_watches)
//
// Copyright (C) 2026 Agent Army / GPL v3
#pragma once

#include <QObject>
#include <QFileSystemWatcher>
#include <QString>
#include <QStringView>
#include <QTimer>
#include <functional>
#include <unordered_map>
#include <chrono>

// ============================================================================
// FileWatcherAdapter
//
// Thin wrapper around QFileSystemWatcher that adds:
//   1. Per-path debouncing (500ms) — suppresses duplicate events from
//      save-heavy editors (vim, git) that touch the mtime multiple times.
//   2. inotify resource-limit check — warns if the system is near the
//      /proc/sys/fs/inotify/max_user_watches ceiling (8192 by default).
//   3. Signal-style callbacks (std::function) for fileChanged / directoryChanged.
//
// Usage:
//     FileWatcherAdapter watcher;
//     watcher.watchFile("/path/to/file.txt", [](const QString& p) {
//         qDebug() << "changed:" << p;
//     });
//     watcher.unwatch("/path/to/file.txt");
//
// Thread-safety note: QFileSystemWatcher must live on the same thread as
// the Qt event loop that dispatches its signals. All callbacks run in
// that thread. QTimer (used for debounce) must also live there.
// ============================================================================

class FileWatcherAdapter : public QObject
{
    Q_OBJECT

public:
    // ------------------------------------------------------------------------
    // Callback types — mirrors Win32 FILE_NOTIFY_INFORMATION action codes
    // ------------------------------------------------------------------------

    /// Called when a watched file's content or timestamp changes.
    using FileChangedCallback = std::function<void(const QString& path)>;

    /// Called when a watched directory's contents change.
    using DirectoryChangedCallback = std::function<void(const QString& path)>;

    // ------------------------------------------------------------------------
    // Lifecycle
    // ------------------------------------------------------------------------

    /// Default constructor.  Installs the inotify-limit check.
    explicit FileWatcherAdapter(QObject* parent = nullptr);

    /// Returns true when QFileSystemWatcher::addPath() is functional on this
    /// platform (i.e. an inotify fd is available).  Used by tests to skip
    /// assertions that require a real kernel file-descriptor.
    static bool isSupported();

    /// Stops all watchers and cancels all pending debounce timers.
    ~FileWatcherAdapter() override;

    // Non-copyable, non-movable (holds QTimer* and QFileSystemWatcher*)
    FileWatcherAdapter(const FileWatcherAdapter&) = delete;
    FileWatcherAdapter& operator=(const FileWatcherAdapter&) = delete;
    FileWatcherAdapter(FileWatcherAdapter&&) = delete;
    FileWatcherAdapter& operator=(FileWatcherAdapter&&) = delete;

    // ------------------------------------------------------------------------
    // Watch management
    // ------------------------------------------------------------------------

    /// Begin watching a file.  Calls `callback` (debounced) whenever the file
    /// is modified, moved over, or deleted.
    ///
    /// If the file does not exist it is still watched — the watcher fires on
    /// creation.
    ///
    /// @param path     Absolute path to the file.
    /// @param callback Called on the Qt event loop thread.
    /// @return true if the path was added; false if already watched or on error.
    bool watchFile(const QString& path, FileChangedCallback callback);

    /// Begin watching a directory.  Calls `callback` when any file inside it
    /// is created, modified, renamed, or deleted.
    ///
    /// @param path     Absolute path to the directory.
    /// @param callback Called on the Qt event loop thread.
    /// @return true if the path was added; false if already watched or on error.
    bool watchDirectory(const QString& path, DirectoryChangedCallback callback);

    /// Stop watching a previously registered file or directory.
    ///
    /// Cancels any pending debounce timer for the path.
    ///
    /// @param path Previously registered path.
    /// @return true if the path was removed; false if it was not being watched.
    bool unwatch(const QString& path);

    /// Stop watching all registered paths and clear all callbacks.
    void unwatchAll();

    // ------------------------------------------------------------------------
    // Query
    // ------------------------------------------------------------------------

    /// Returns the set of currently watched paths.
    QStringList watchedPaths() const;

    /// Returns true if `path` is currently being watched.
    bool isWatching(const QString& path) const;

    /// Number of active watch entries.
    int watchCount() const;

    // ------------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------------

    /// Sets the debounce interval.  Default is 500 ms.
    /// A value of 0 disables debouncing.
    void setDebounceMs(int ms);

    /// Current debounce interval in milliseconds.
    int debounceMs() const { return _debounceMs; }

signals:
    /// Emitted when a file change has passed the debounce window and is
    /// confirmed as a real change.  Equivalent to Win32
    /// FILE_ACTION_MODIFIED / FILE_ACTION_RENAMED.
    void fileChanged(const QString& path);

    /// Emitted when a directory change has passed the debounce window.
    /// Equivalent to Win32 FILE_NOTIFY_CHANGE_DIR_NAME events.
    void directoryChanged(const QString& path);

    /// Emitted once during construction if the system inotify limit is
    /// at or below the warning threshold.  Editors with many open files
    /// may exhaust watches — users should raise the limit:
    ///   echo 524288 | sudo tee /proc/sys/fs/inotify/max_user_watches
    void inotifyLimitWarning(int currentLimit, int recommendedMinimum);

    /// Emitted when the underlying QFileSystemWatcher reports an error.
    void error(const QString& message);

private:
    // Internal per-path debounce entry
    struct WatchEntry {
        bool isDirectory = false;
        FileChangedCallback     fileCallback;
        DirectoryChangedCallback dirCallback;
        QTimer* debounceTimer = nullptr;   // owned by this object
        QString pendingPath;                // path stored before debounce fires
        std::chrono::steady_clock::time_point firstEventTime;
    };

    void onRawFileChanged(const QString& path);
    void onRawDirectoryChanged(const QString& path);
    void fireFileChange(const QString& path);
    void fireDirectoryChange(const QString& path);
    void checkInotifyLimit();

    static QString readInotifyLimitFile();

    QFileSystemWatcher* _watcher = nullptr;

    // path → entry
    std::unordered_map<QString, WatchEntry> _entries;

    int _debounceMs = 500;   // milliseconds

    // inotify warning threshold — set to 80 % of the system limit
    int _inotifyWarningThreshold = 0;
};
