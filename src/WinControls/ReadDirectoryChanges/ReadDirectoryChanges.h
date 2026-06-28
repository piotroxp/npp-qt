// The MIT License
//
// Copyright (c) 2010 James E Beveridge
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// Qt6 port: directory change monitoring
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ReadDirectoryChanges.h
//
// On Windows: uses ReadDirectoryChangesW with APC callbacks (ReadDirectoryChangesPrivate).
// On Linux:   uses QFileSystemWatcher (inotify) via the worker thread.
// On macOS:   uses QFileSystemWatcher (FSEvents).

#pragma once

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QPair>
#include <QFileInfo>
#include <QHash>
#include <QSet>

#include "ThreadSafeQueue.h"
#include "targetver.h"

#if defined(_WIN32)
#include <windows.h>
#endif

// File action constants (mirrors FILE_NOTIFY_INFORMATION values)
enum {
    FILE_ACTION_ADDED           = 0x00000001,
    FILE_ACTION_REMOVED         = 0x00000002,
    FILE_ACTION_MODIFIED       = 0x00000003,
    FILE_ACTION_RENAMED_OLD_NAME = 0x00000004,
    FILE_ACTION_RENAMED_NEW_NAME = 0x00000005,
};

// Notification pair: action code + absolute file path
using TDirectoryChangeNotification = QPair<DWORD, QString>;

namespace ReadDirectoryChangesPrivate
{
    class CReadChangesServer;
}

///
/// Monitors file system directory changes and delivers notifications
/// via a thread-safe queue.
///
/// Usage (cross-platform):
/// \code
///     CReadDirectoryChanges watcher;
///     watcher.addDirectory("/path/to/watch", true, FILE_NOTIFY_CHANGE_FILE_NAME);
///     DWORD action;
///     QString path;
///     while (watcher.pop(action, path)) {
///         // handle change
///     }
/// \endcode
///
class CReadDirectoryChanges : public QObject
{
    Q_OBJECT

public:
    explicit CReadDirectoryChanges(QObject* parent = nullptr);
    ~CReadDirectoryChanges();

    /// Add a directory to monitor.
    /// \param dirPath  Absolute directory path.
    /// \param watchSubtree  True to monitor subdirectories.
    /// \param notifyFilter  Combination of FILE_NOTIFY_CHANGE_* flags (Win32 only;
    ///                      on Unix this parameter is ignored as all change types
    ///                      reported by QFileSystemWatcher are forwarded).
    /// \param bufferSize    Notification buffer size (Win32 only; default 16384).
    void addDirectory(const QString& dirPath, bool watchSubtree,
                      DWORD notifyFilter = 0, DWORD bufferSize = 16384);

    /// Pop a pending notification from the queue.
    /// Returns true if a notification was retrieved.
    bool pop(DWORD& action, QString& filePath);

    /// Stop monitoring and join the worker thread.
    void terminate();

    /// Returns the worker thread for external wait loops.
    QThread* workerThread() const { return m_workerThread; }

signals:
    /// Emitted on the worker thread when a directory change is detected.
    /// Parameters: action code, absolute file path.
    void directoryChanged(DWORD action, const QString& filePath);

protected:
    void init();

    // Called from CReadChangesRequest to enqueue a notification.
    void push(DWORD action, const QString& filePath);

private:
    ReadDirectoryChangesPrivate::CReadChangesServer* m_pServer = nullptr;
    QThread* m_workerThread = nullptr;

    // Queue of pending notifications consumed by the main thread.
    CThreadSafeQueue<TDirectoryChangeNotification> m_notifications;

#if !defined(_WIN32)
    // Qt6 cross-platform watcher for non-Windows builds.
    QFileSystemWatcher* m_watcher = nullptr;

    void handleFileChanged(const QString& path);
    void handleDirectoryChanged(const QString& path);

    // Map watched dirs to their original watch-subtree flag.
    QHash<QString, bool> m_watchSubtree;

    // Track previous file states for modification detection.
    QHash<QString, QDateTime> m_fileModTime;
#endif

    friend class WatcherThread;
    friend class ReadDirectoryChangesPrivate::CReadChangesRequest;
};


// Worker thread that drives either:
//   - Win32 APC-based ReadDirectoryChangesW (Windows)
//   - QFileSystemWatcher signals (Linux/macOS)
class WatcherThread : public QThread
{
    Q_OBJECT

public:
    WatcherThread(CReadDirectoryChanges* parent)
        : QThread(parent), m_changes(parent) {}

    void run() override;

    CReadDirectoryChanges* const m_changes;

signals:
    void fileChanged(DWORD action, const QString& filePath);
};
