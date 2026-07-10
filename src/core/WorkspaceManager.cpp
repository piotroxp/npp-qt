// WorkspaceManager.cpp - Workspace/project management implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "WorkspaceManager.h"
#include "Parameters.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRegularExpression>
#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include <QJsonValue>

// ============================================================================
// Construction / Lifecycle
// ============================================================================

WorkspaceManager::WorkspaceManager(QObject* parent)
    : QObject(parent)
{
    loadRecentList();
}

WorkspaceManager::~WorkspaceManager() {
    saveRecentList();
}

// ============================================================================
// Open / Close
// ============================================================================

bool WorkspaceManager::openWorkspace(const QString& path) {
    closeWorkspace();

    if (!loadFromJson(path)) {
        qWarning() << "[WorkspaceManager] Failed to load workspace:" << path;
        return false;
    }

    _data.filePath = path;
    addRecentWorkspace(path);

    // Watch the workspace file for changes
    _watcher.addPath(path);
    connect(&_watcher, &QFileSystemWatcher::fileChanged,
            this, [this](const QString&) {
        emit workspaceFilesChanged();
    });

    emit workspaceOpened(path);
    return true;
}

void WorkspaceManager::closeWorkspace() {
    if (_data.isValid) {
        emit workspaceClosed();
    }
    _data = WorkspaceData{};
    _watcher.removePaths(_watcher.files());
    _watcher.removePaths(_watcher.directories());
}

bool WorkspaceManager::saveWorkspace(const QString& path) {
    QString savePath = path.isEmpty() ? _data.filePath : path;
    if (savePath.isEmpty())
        return false;

    QJsonObject obj = toJson();
    QJsonDocument doc(obj);

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    _data.filePath = savePath;
    return true;
}

// ============================================================================
// JSON Serialization
// ============================================================================

bool WorkspaceManager::loadFromJson(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "[WorkspaceManager] JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject())
        return false;

    return loadFromJsonObject(doc.object());
}

bool WorkspaceManager::loadFromJsonObject(const QJsonObject& obj) {
    _data.name = obj.value("name").toString();
    _data.root = obj.value("root").toString();
    _data.excludePatterns.clear();

    QJsonArray exclude = obj.value("exclude").toArray();
    for (const QJsonValue& v : exclude) {
        _data.excludePatterns.append(v.toString());
    }

    if (_data.name.isEmpty())
        _data.name = QFileInfo(_data.root).fileName();
    if (_data.root.isEmpty() || !QDir(_data.root).exists()) {
        qWarning() << "[WorkspaceManager] Invalid root directory:" << _data.root;
        return false;
    }

    _data.isValid = true;
    return true;
}

QJsonObject WorkspaceManager::toJson() const {
    QJsonObject obj;
    obj.insert("name", _data.name);
    obj.insert("root", _data.root);

    QJsonArray exclude;
    for (const QString& pattern : _data.excludePatterns)
        exclude.append(pattern);
    obj.insert("exclude", exclude);

    return obj;
}

// ============================================================================
// File Discovery
// ============================================================================

QStringList WorkspaceManager::getFiles() const {
    QStringList result;
    if (!_data.isValid)
        return result;

    QDir dir(_data.root);
    const QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& fi : entries) {
        QString path = fi.absoluteFilePath();
        if (fi.isDir()) {
            QStringList subFiles = getFilesRecursiveForDir(path);
            result.append(subFiles);
        } else {
            if (!isExcluded(path))
                result.append(path);
        }
    }
    return result;
}

QStringList WorkspaceManager::getFilesRecursive() const {
    if (!_data.isValid)
        return QStringList();
    return getFilesRecursiveForDir(_data.root);
}

QStringList WorkspaceManager::getFilesRecursiveForDir(const QString& dirPath) const {
    QStringList result;
    QDir dir(dirPath);
    const QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& fi : entries) {
        QString path = fi.absoluteFilePath();
        if (fi.isDir()) {
            if (!isExcluded(path))
                result.append(getFilesRecursiveForDir(path));
        } else {
            if (!isExcluded(path))
                result.append(path);
        }
    }
    return result;
}

bool WorkspaceManager::isExcluded(const QString& path) const {
    for (const QString& pattern : _data.excludePatterns) {
        QRegularExpression re(
            QRegularExpression::wildcardToRegularExpression(pattern),
            QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(path);
        if (match.hasMatch())
            return true;
    }
    return false;
}

// ============================================================================
// Search
// ============================================================================

bool WorkspaceManager::findInWorkspace(const QString& query,
                                        QStringList& results,
                                        bool matchCase,
                                        bool wholeWord,
                                        const QStringList& extensions) const {
    std::vector<WorkspaceSearchResult> searchResults =
        searchFiles(query, matchCase, wholeWord);

    for (const WorkspaceSearchResult& r : searchResults) {
        QString ext = QFileInfo(r.filePath).suffix();
        if (!extensions.isEmpty() && !extensions.contains(ext, Qt::CaseInsensitive))
            continue;

        results.append(QStringLiteral("%1:%2: %3")
                           .arg(r.filePath)
                           .arg(r.lineNumber + 1)
                           .arg(r.matchingLine.trimmed()));
    }

    return !results.isEmpty();
}

std::vector<WorkspaceSearchResult> WorkspaceManager::searchFiles(
        const QString& query,
        bool matchCase,
        bool wholeWord) const {

    std::vector<WorkspaceSearchResult> results;
    QStringList files = getFilesRecursive();

    for (const QString& filePath : files) {
        searchInFile(filePath, query, matchCase, wholeWord, results);
        if (results.size() > 10000)  // Limit results
            break;
    }

    const_cast<WorkspaceManager*>(this)->searchCompleted(static_cast<int>(results.size()));
    return results;
}

bool WorkspaceManager::searchInFile(const QString& filePath,
                                     const QString& query,
                                     bool matchCase,
                                     bool wholeWord,
                                     std::vector<WorkspaceSearchResult>& results) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // Skip binary files
    QByteArray sample = file.read(256);
    if (sample.contains('\0'))
        return false;
    file.seek(0);

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    int lineNum = 0;

    QString patternStr = wholeWord
        ? QStringLiteral("\\b%1\\b").arg(QRegularExpression::escape(query))
        : QRegularExpression::escape(query);

    QRegularExpression re(patternStr);
    re.setPatternOptions(matchCase ? QRegularExpression::NoPatternOption
                                   : QRegularExpression::CaseInsensitiveOption);

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            WorkspaceSearchResult r;
            r.filePath = filePath;
            r.lineNumber = lineNum;
            r.matchingLine = line;
            r.matchColumn = match.capturedStart();
            results.push_back(r);
        }
        ++lineNum;
    }

    return true;
}

// ============================================================================
// Recent Workspaces
// ============================================================================

void WorkspaceManager::addRecentWorkspace(const QString& path) {
    _recentWorkspaces.removeAll(path);
    _recentWorkspaces.prepend(path);
    while (_recentWorkspaces.size() > _maxRecentWorkspaces)
        _recentWorkspaces.removeLast();
    saveRecentList();
    emit recentWorkspacesChanged();
}

void WorkspaceManager::clearRecentWorkspaces() {
    _recentWorkspaces.clear();
    saveRecentList();
    emit recentWorkspacesChanged();
}

void WorkspaceManager::loadRecentList() {
    QSettings s;
    s.beginGroup("WorkspaceManager");
    _recentWorkspaces = s.value("RecentWorkspaces").toStringList();
    _maxRecentWorkspaces = s.value("MaxRecentWorkspaces", 10).toInt();
    s.endGroup();

    // Prune non-existent workspaces
    for (auto it = _recentWorkspaces.begin(); it != _recentWorkspaces.end(); ) {
        if (!QFile::exists(*it))
            it = _recentWorkspaces.erase(it);
        else
            ++it;
    }
}

void WorkspaceManager::saveRecentList() {
    QSettings s;
    s.beginGroup("WorkspaceManager");
    s.setValue("RecentWorkspaces", _recentWorkspaces);
    s.setValue("MaxRecentWorkspaces", _maxRecentWorkspaces);
    s.endGroup();
}

// ============================================================================
// Session Integration
// ============================================================================

bool WorkspaceManager::loadWorkspaceFromSession(const QString& path) {
    if (path.isEmpty())
        return false;
    if (!QFile::exists(path))
        return false;
    return openWorkspace(path);
}
