// Semantic Lift: Win32 ReadDirectoryChanges → Qt6 DirectoryWatcher
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ReadDirectoryChanges.cpp
//   + ReadDirectoryChanges.h / ReadDirectoryChangesPrivate.h
// Target: npp-qt/src/WinControls/ReadDirectoryChanges.h + .cpp

#pragma once

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QFileInfo>
#include <QDir>
#include <QString>

// Forward-declare the private server class.
// On Windows it is defined in ReadDirectoryChangesPrivate.h;
// on Linux it is unused (the Qt6 DirectoryWatcher handles everything).
class CReadChangesServer;

// -----------------------------------------------------------------------
// Change action flags — mirror the Win32 FILE_NOTIFY_INFORMATION values
// -----------------------------------------------------------------------
constexpr quint32 FILE_ACTION_ADDED            = 0x00000001;
constexpr quint32 FILE_ACTION_REMOVED          = 0x00000002;
constexpr quint32 FILE_ACTION_MODIFIED         = 0x00000003;
constexpr quint32 FILE_ACTION_RENAMED_OLD_NAME = 0x00000004;
constexpr quint32 FILE_ACTION_RENAMED_NEW_NAME = 0x00000005;

struct DirectoryChangeNotification {
    quint32 action = 0;
    QString fileName;
};

// -----------------------------------------------------------------------
// DirectoryWatcher — Qt6 replacement for CReadDirectoryChanges
//
// Monitors one or more directory trees for changes using QFileSystemWatcher.
// Changes are queued and retrievable via pop() on the main thread.
// -----------------------------------------------------------------------
class DirectoryWatcher : public QObject
{
    Q_OBJECT

public:
    // Notify filter flags — mirrors Win32 FILE_NOTIFY_INFORMATION filter.
    enum NotifyFilter {
        FileName     = 0x00000001,
        DirName      = 0x00000002,
        Attributes   = 0x00000004,
        Size         = 0x00000008,
        LastWrite    = 0x00000010,
        FileSecurity = 0x00000100
    };
    Q_DECLARE_FLAGS(NotifyFilters, NotifyFilter)

    explicit DirectoryWatcher(QObject* parent = nullptr);
    ~DirectoryWatcher() override;

    // Start watching a directory. Must be called before using the watcher.
    // Returns true on success.
    bool init();

    // Stop the watcher and clean up.
    void terminate();

    // Add a directory to watch.
    // bWatchSubtree: if true, watches all subdirectories recursively.
    // filter: what kinds of changes to report.
    // bufferSize: ignored on Qt (event-driven via inotify/FSEvents).
    void addDirectory(const QString& directoryPath,
                      bool watchSubtree = false,
                      NotifyFilters filter = NotifyFilters(FileName),
                      int bufferSize = 4096);

    // Push a notification (called internally by the watcher).
    void push(quint32 action, const QString& fileName);

    // Pop the next notification. Returns true if a notification was retrieved.
    // Blocks if the queue is empty and waitMs > 0 (uses QWaitCondition).
    bool pop(quint32& outAction, QString& outFileName, int waitMs = 0);

    // Returns the number of pending notifications.
    int pendingCount() const;

    // Returns true if the watcher thread is running.
    bool isRunning() const { return _isRunning; }

signals:
    // Emitted for each change detected.
    void directoryChanged(quint32 action, const QString& fileName);

    // Emitted when a file is added to a watched directory.
    void fileAdded(const QString& fileName);

    // Emitted when an error occurs.
    void error(const QString& errorMsg);

    // Emitted when a file is removed from a watched directory.
    void fileRemoved(const QString& fileName);

    // Emitted when a file is modified in a watched directory.
    void fileModified(const QString& fileName);

    // Emitted when an error occurs (e.g., directory removed).
    void watcherError(const QString& errorString);

public:
    // Win32 compatibility stub: returns nullptr in Qt6.
    // Win32 version returned a HANDLE for WaitForMultipleObjects.
    void* GetWaitHandle() const { return nullptr; }

public slots:
    void onDirectoryChanged(const QString& path);
    void onFileChanged(const QString& path);
    void onWatcherError(const QString& error);

private:
    void processPath(const QString& path, quint32 action);

    QFileSystemWatcher* _watcher = nullptr;
    QTimer* _pollTimer = nullptr;

    // Notification queue (protected by _mutex).
    QQueue<DirectoryChangeNotification> _notifications;
    mutable QMutex _mutex;
    QWaitCondition _notEmpty;

    bool _isRunning = false;

    // Directories being watched.
    QStringList _watchedPaths;

    // Track files that existed before (for ADD vs MODIFY differentiation).
    QMap<QString, QDateTime> _knownFiles;

    friend class DirectoryWatcherThread;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DirectoryWatcher::NotifyFilters)
