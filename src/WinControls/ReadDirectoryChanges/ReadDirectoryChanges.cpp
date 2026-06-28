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

// Qt6 port: directory change monitoring implementation
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ReadDirectoryChanges.cpp
//
// Windows build: uses CReadChangesServer with APC callbacks (ReadDirectoryChangesPrivate).
// Unix (Linux/macOS) build: uses QFileSystemWatcher + inotify/FSEvents.

#include "ReadDirectoryChanges.h"
#include "ReadDirectoryChangesPrivate.h"

#if defined(_WIN32)
#include <process.h>
#endif

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

using namespace ReadDirectoryChangesPrivate;

///////////////////////////////////////////////////////////////////////////
// CReadDirectoryChanges — cross-platform implementation

CReadDirectoryChanges::CReadDirectoryChanges(QObject* parent)
    : QObject(parent)
{
#if defined(_WIN32)
    m_pServer = new CReadChangesServer(this);
#else
    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, &QFileSystemWatcher::fileChanged,
            this, &CReadDirectoryChanges::handleFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &CReadDirectoryChanges::handleDirectoryChanged);
#endif
}

CReadDirectoryChanges::~CReadDirectoryChanges()
{
    terminate();
#if defined(_WIN32)
    delete m_pServer;
#endif
}

void CReadDirectoryChanges::init()
{
#if defined(_WIN32)
    // Kick off the worker thread managed by CReadChangesServer.
    m_workerThread = new QThread();
    // Move server to the thread
    m_pServer->moveToThread(m_workerThread);
    connect(m_workerThread, &QThread::started, m_pServer, [this]() {
        // Queue the initial start via APC equivalent
        // CReadChangesServer::run() runs its own loop
    });
    m_workerThread->start();
#else
    // QFileSystemWatcher lives on the main thread for signal delivery,
    // but we keep a worker thread for the event pump loop.
    m_workerThread = new QThread();
    connect(m_workerThread, &QThread::started, this, [this]() {
        // On Unix, no separate worker thread is needed for watching —
        // QFileSystemWatcher uses its own internal thread.
        // We just keep the thread alive as a placeholder.
        exec();
    });
    m_workerThread->start();
#endif
}

void CReadDirectoryChanges::addDirectory(const QString& dirPath, bool watchSubtree,
                                          DWORD notifyFilter, DWORD bufferSize)
{
    if (!m_workerThread)
        init();

#if defined(_WIN32)
    Q_UNUSED(notifyFilter);
    Q_UNUSED(bufferSize);
    if (!m_workerThread || !m_pServer)
        return;
    // Add the directory via the Windows server
    // Note: This is called from the main thread; the server runs in m_workerThread.
    // For thread-safety we signal the server via a queued connection.
    QMetaObject::invokeMethod(m_pServer, [this, dirPath, watchSubtree, notifyFilter, bufferSize]() {
        auto* pRequest = new CReadChangesRequest(
            m_pServer,
            reinterpret_cast<const wchar_t*>(dirPath.utf16()),
            watchSubtree,
            notifyFilter,
            bufferSize);
        m_pServer->addDirectory(pRequest);
    }, Qt::QueuedConnection);
#else
    // Linux / macOS: use QFileSystemWatcher
    QFileInfo info(dirPath);
    QString canonicalPath = info.canonicalFilePath();
    if (canonicalPath.isEmpty())
        return; // path does not exist

    m_watchSubtree[canonicalPath] = watchSubtree;

    if (info.isDir())
    {
        // Watch the directory itself.
        if (!m_watcher->addPath(canonicalPath))
            qWarning() << "ReadDirectoryChanges: failed to watch directory:" << canonicalPath;

        // If subtree watching is requested, also add all subdirectories.
        if (watchSubtree)
        {
            QDir dir(canonicalPath);
            dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            const auto subDirs = dir.entryList();
            for (const QString& sub : subDirs)
            {
                QString subPath = canonicalPath + QDir::separator() + sub;
                m_watchSubtree[subPath] = true;
                m_watcher->addPath(subPath);
            }
        }
    }
    else
    {
        // Watch the file.
        if (!m_watcher->addPath(canonicalPath))
            qWarning() << "ReadDirectoryChanges: failed to watch file:" << canonicalPath;
    }
#endif
}

bool CReadDirectoryChanges::pop(DWORD& action, QString& filePath)
{
    TDirectoryChangeNotification notification;
    if (!m_notifications.pop(notification))
        return false;

    action = notification.first;
    filePath = notification.second;
    return true;
}

void CReadDirectoryChanges::terminate()
{
#if defined(_WIN32)
    if (m_workerThread)
    {
        // Signal server termination
        if (m_pServer)
            m_pServer->requestTermination();
        m_workerThread->quit();
        if (!m_workerThread->wait(10000))
            qWarning() << "ReadDirectoryChanges: worker thread did not stop cleanly";
        delete m_workerThread;
        m_workerThread = nullptr;
    }
#else
    if (m_workerThread)
    {
        m_workerThread->quit();
        if (!m_workerThread->wait(5000))
            qWarning() << "ReadDirectoryChanges: worker thread did not stop cleanly";
        delete m_workerThread;
        m_workerThread = nullptr;
    }
    if (m_watcher)
    {
        m_watcher->removePaths(m_watcher->files());
        m_watcher->removePaths(m_watcher->directories());
    }
#endif
}

void CReadDirectoryChanges::push(DWORD action, const QString& filePath)
{
    TDirectoryChangeNotification notification(action, filePath);
    m_notifications.push(notification);
}

#if !defined(_WIN32)

void CReadDirectoryChanges::handleFileChanged(const QString& path)
{
    // QFileSystemWatcher emits fileChanged when a watched file is modified, renamed, or deleted.
    // Distinguish by checking if the file still exists.
    QFileInfo fi(path);
    DWORD action;

    if (!fi.exists())
    {
        action = FILE_ACTION_REMOVED;
    }
    else
    {
        // Check if modification time changed from last recorded time.
        QDateTime now = fi.lastModified();
        QDateTime prev = m_fileModTime.value(path);
        if (prev.isValid() && prev != now)
        {
            action = FILE_ACTION_MODIFIED;
        }
        else if (!prev.isValid())
        {
            // First time seeing this file (e.g. was just created and we got a change notice).
            action = FILE_ACTION_ADDED;
        }
        else
        {
            // No meaningful change detected — possibly a benign inotify event.
            // Still report as modified to be safe.
            action = FILE_ACTION_MODIFIED;
        }
        m_fileModTime[path] = now;
    }

    TDirectoryChangeNotification notification(action, path);
    m_notifications.push(notification);
}

void CReadDirectoryChanges::handleDirectoryChanged(const QString& dirPath)
{
    bool watchSubtree = m_watchSubtree.value(dirPath, false);

    // Re-add the path if it was removed (watcher removes broken paths automatically).
    if (!m_watcher->directories().contains(dirPath))
        m_watcher->addPath(dirPath);

    // Scan the directory to detect added/removed files.
    QDir dir(dirPath);
    if (!dir.exists())
        return;

    // Get current entries.
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    const auto entries = dir.entryInfoList();

    // Track existing files we know about.
    // Entries not in this set are new; entries in the set but missing are removed.
    // We use the fileModTime map as a registry of known files.
    QSet<QString> currentPaths;
    for (const QFileInfo& fi : entries)
    {
        currentPaths.insert(fi.absoluteFilePath());
    }

    // Detect removed files.
    for (auto it = m_fileModTime.begin(); it != m_fileModTime.end(); )
    {
        const QString& knownPath = it.key();
        if (!currentPaths.contains(knownPath) && knownPath.startsWith(dirPath))
        {
            // File was removed.
            TDirectoryChangeNotification notification(FILE_ACTION_REMOVED, knownPath);
            m_notifications.push(notification);
            it = m_fileModTime.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Detect added or modified files.
    for (const QFileInfo& fi : entries)
    {
        const QString& absPath = fi.absoluteFilePath();
        QDateTime now = fi.lastModified();
        QDateTime prev = m_fileModTime.value(absPath);

        if (!prev.isValid())
        {
            // New file.
            TDirectoryChangeNotification notification(FILE_ACTION_ADDED, absPath);
            m_notifications.push(notification);
        }
        else if (prev != now)
        {
            // Modified.
            TDirectoryChangeNotification notification(FILE_ACTION_MODIFIED, absPath);
            m_notifications.push(notification);
        }

        m_fileModTime[absPath] = now;

        // Recurse into subdirectories if subtree watching is enabled.
        if (watchSubtree && fi.isDir())
        {
            m_watchSubtree[absPath] = true;
            m_watcher->addPath(absPath);
        }
    }
}

#endif // !_WIN32

///////////////////////////////////////////////////////////////////////////
// WatcherThread — thin QThread wrapper for the Windows server path

void WatcherThread::run()
{
#if defined(_WIN32)
    // On Windows, CReadChangesServer manages its own thread internally via _beginthreadex.
    // This Qt thread just exists as a placeholder.
    exec();
#else
    // On Unix, the worker thread is not strictly needed (QFileSystemWatcher is self-contained),
    // but we keep it for consistency with the Win32 architecture.
    exec();
#endif
}
