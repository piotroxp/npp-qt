// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QFileSystemWatcher>
#include <QString>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDir>

// CReadDirectoryChanges - Directory change monitoring
// Replaces Win32 ReadDirectoryChangesW with Qt QFileSystemWatcher
class CReadDirectoryChanges : public QObject
{
    Q_OBJECT

public:
    CReadDirectoryChanges(QObject* parent = nullptr);
    ~CReadDirectoryChanges();

    void Init();
    void Terminate();

    void AddDirectory(const QString& directory, bool watchSubtree, unsigned int notifyFilter, unsigned int bufferSize = 16384);

    bool Pop(unsigned int& dwAction, QString& wstrFilename);

signals:
    void directoryChanged(const QString& path, unsigned int action);

public slots:
    void onDirectoryChanged(const QString& path);

private:
    void processChanges(const QString& path);

    QFileSystemWatcher* _watcher = nullptr;
    QSet<QString> _watchedDirectories;
    bool _watchSubtree = false;
    unsigned int _notifyFilter = 0;
    
    // Notification queue
    QQueue<QPair<unsigned int, QString>> _notifications;
    QMutex _mutex;
};

// Action types for directory changes (Windows FILE_NOTIFY_INFORMATION values)
#define FILE_ACTION_ADDED           0x00000001
#define FILE_ACTION_REMOVED         0x00000002
#define FILE_ACTION_MODIFIED        0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME 0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME 0x00000005

// Common notify filter flags
#define FILE_NOTIFY_CHANGE_ATTRIBUTES    0x00000004
#define FILE_NOTIFY_CHANGE_DIRNAME        0x00000100
#define FILE_NOTIFY_CHANGE_FILENAME       0x00000001
#define FILE_NOTIFY_CHANGE_LAST_WRITE     0x00000010
#define FILE_NOTIFY_CHANGE_SIZE           0x00000008
#define FILE_NOTIFY_CHANGE_SECURITY       0x00000100