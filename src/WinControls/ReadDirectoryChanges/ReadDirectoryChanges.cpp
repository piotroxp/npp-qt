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

#include "ReadDirectoryChanges.h"

#include <QFileInfo>
#include <QDateTime>

CReadDirectoryChanges::CReadDirectoryChanges(QObject* parent)
    : QObject(parent)
    , _watcher(nullptr)
    , _notifyFilter(0)
    , _watchSubtree(false)
{
}

CReadDirectoryChanges::~CReadDirectoryChanges()
{
    Terminate();
}

void CReadDirectoryChanges::Init()
{
    if (!_watcher) {
        _watcher = new QFileSystemWatcher(this);
        connect(_watcher, &QFileSystemWatcher::directoryChanged,
                this, &CReadDirectoryChanges::onDirectoryChanged);
    }
}

void CReadDirectoryChanges::Terminate()
{
    if (_watcher) {
        _watcher->deleteLater();
        _watcher = nullptr;
    }
    _watchedDirectories.clear();
    _notifications.clear();
}

void CReadDirectoryChanges::AddDirectory(const QString& directory, bool watchSubtree, unsigned int notifyFilter, unsigned int bufferSize)
{
    Q_UNUSED(bufferSize);
    
    if (!_watcher) {
        Init();
    }
    
    _watchedDirectories.insert(directory);
    _watchSubtree = watchSubtree;
    _notifyFilter = notifyFilter;
    
    _watcher->addPath(directory);
}

bool CReadDirectoryChanges::Pop(unsigned int& dwAction, QString& wstrFilename)
{
    QMutexLocker locker(&_mutex);
    
    if (_notifications.isEmpty()) {
        return false;
    }
    
    auto item = _notifications.dequeue();
    dwAction = item.first;
    wstrFilename = item.second;
    
    return true;
}

void CReadDirectoryChanges::onDirectoryChanged(const QString& path)
{
    processChanges(path);
}

void CReadDirectoryChanges::processChanges(const QString& path)
{
    QDir dir(path);
    
    // Check what changed based on notify filter
    if (_notifyFilter & FILE_NOTIFY_CHANGE_ATTRIBUTES ||
        _notifyFilter & FILE_NOTIFY_CHANGE_SIZE ||
        _notifyFilter & FILE_NOTIFY_CHANGE_LAST_WRITE) {
        
        // Scan directory for changes
        QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QFileInfo& info : entries) {
            QString fullPath = info.absoluteFilePath();
            unsigned int action = FILE_ACTION_MODIFIED;
            
            if (!info.exists()) {
                action = FILE_ACTION_REMOVED;
            } else {
                // Check if it's a new file/directory
                if (!_watchedDirectories.contains(fullPath)) {
                    action = FILE_ACTION_ADDED;
                }
            }
            
            {
                QMutexLocker locker(&_mutex);
                _notifications.enqueue(qMakePair(action, info.fileName()));
            }
            
            emit directoryChanged(fullPath, action);
        }
    }
    
    // Handle new files specifically
    if (_notifyFilter & FILE_NOTIFY_CHANGE_FILENAME) {
        QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QFileInfo& info : entries) {
            {
                QMutexLocker locker(&_mutex);
                _notifications.enqueue(qMakePair(FILE_ACTION_ADDED, info.fileName()));
            }
            emit directoryChanged(info.absoluteFilePath(), FILE_ACTION_ADDED);
        }
    }
}