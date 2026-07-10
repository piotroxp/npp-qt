// WorkspaceManager.h - Workspace/project management for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3
//
// Workspace format (.npp-workspace.json):
// {
//     "name": "My Project",
//     "root": "/path/to/project",
//     "exclude": ["**/build/**", "**/.git/**", "**/node_modules/**"]
// }

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileSystemWatcher>
#include <vector>

// ============================================================================
// Workspace Data
// ============================================================================
struct WorkspaceData {
    QString name;
    QString root;
    QStringList excludePatterns;
    QString filePath;  // Path to the .npp-workspace.json file
    bool isValid = false;
};

// ============================================================================
// Search Result
// ============================================================================
struct WorkspaceSearchResult {
    QString filePath;
    int lineNumber = 0;
    QString matchingLine;
    int matchColumn = 0;
};

// ============================================================================
// WorkspaceManager — Open/close .npp-workspace.json workspaces
// ============================================================================
class WorkspaceManager : public QObject {
    Q_OBJECT

public:
    explicit WorkspaceManager(QObject* parent = nullptr);
    ~WorkspaceManager();

    // === Open / Close ===

    bool openWorkspace(const QString& path);
    void closeWorkspace();
    bool saveWorkspace(const QString& path = QString());

    // === Workspace Info ===

    bool isValid() const { return _data.isValid; }
    QString workspaceName() const { return _data.name; }
    QString workspaceRoot() const { return _data.root; }
    QStringList excludePatterns() const { return _data.excludePatterns; }
    QString currentWorkspacePath() const { return _data.filePath; }

    // === File Discovery ===

    QStringList getFiles() const;
    QStringList getFilesRecursive() const;

private:
    // Private helper for recursive file listing
    QStringList getFilesRecursiveForDir(const QString& dirPath) const;

    // Search within a single file

    bool findInWorkspace(const QString& query,
                         QStringList& results,
                         bool matchCase = false,
                         bool wholeWord = false,
                         const QStringList& extensions = QStringList()) const;

    std::vector<WorkspaceSearchResult> searchFiles(const QString& query,
                                                    bool matchCase = false,
                                                    bool wholeWord = false) const;

    // === Recent Workspaces ===

    QStringList recentWorkspaces() const { return _recentWorkspaces; }
    void addRecentWorkspace(const QString& path);
    void clearRecentWorkspaces();
    void setMaxRecentWorkspaces(int max) { _maxRecentWorkspaces = max; }
    int maxRecentWorkspaces() const { return _maxRecentWorkspaces; }

    // === Session Integration ===

    bool loadWorkspaceFromSession(const QString& path);
    QString workspacePathForSession() const { return _data.filePath; }

signals:
    void workspaceOpened(const QString& path);
    void workspaceClosed();
    void workspaceFilesChanged();
    void searchCompleted(int resultCount);
    void recentWorkspacesChanged();

private:
    // Parse .npp-workspace.json
    bool loadFromJson(const QString& path);
    bool loadFromJsonObject(const QJsonObject& obj);

    // Serialize to .npp-workspace.json
    QJsonObject toJson() const;

    // Load recent workspaces list from disk
    void loadRecentList();
    void saveRecentList();

    // Check if a path matches any exclude pattern
    bool isExcluded(const QString& path) const;

    // Search within a single file
    bool searchInFile(const QString& filePath,
                       const QString& query,
                       bool matchCase,
                       bool wholeWord,
                       std::vector<WorkspaceSearchResult>& results) const;

    WorkspaceData _data;
    QStringList _recentWorkspaces;
    int _maxRecentWorkspaces = 10;
    mutable QFileSystemWatcher _watcher;
};
