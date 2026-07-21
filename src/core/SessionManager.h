// SessionManager.h - Session save/restore
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "../common/Types.h"
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <vector>

// ============================================================================
// Cursor position info for session restore
// ============================================================================
struct CursorPosition {
    int line = 0;
    int column = 0;
};

// ============================================================================
// Buffer state for session
// ============================================================================
struct SessionBuffer {
    QString path;
    QString language;
    CursorPosition cursor;
    int scrollX = 0;
    int scrollY = 0;
    bool isActive = false;
};

// ============================================================================
// Panel state for session
// ============================================================================
struct PanelState {
    bool functionList = false;
    bool documentMap = false;
    bool fileBrowser = false;
};

// ============================================================================
// Window geometry for session
// ============================================================================
struct WindowGeometry {
    int x = 100;
    int y = 100;
    int width = 1200;
    int height = 800;
    bool maximized = false;
};

// ============================================================================
// Full session data structure
// ============================================================================
struct NppSession {
    int version = 1;
    WindowGeometry window;
    QVector<SessionBuffer> buffers;
    int activeBufferIndex = 0;
    int activeView = 0;
    PanelState panels;
    QString workspacePath;  // Path to .npp-workspace.json
};

// ============================================================================
// SessionManager — Full session save/restore with JSON
// ============================================================================
class SessionManager : public QObject, public NonCopyable {
    Q_OBJECT

public:
    SessionManager();
    ~SessionManager();

    // === JSON Session Format (primary) ===

    bool loadSession(const std::string& path);
    bool saveSession(const std::string& path);

    // Load/save with NppSession object
    bool loadSessionFromJson(const QString& path);
    bool saveSessionToJson(const QString& path);

    // Get current session object
    NppSession currentSession() const { return _session; }
    void setCurrentSession(const NppSession& session) { _session = session; }

    // === XML Session Format (backward compatibility) ===

    bool loadSessionXml(const std::string& path);
    bool saveSessionXml(const std::string& path);

    // === Active Session Path ===

    void setActiveSession(const std::string& sessionPath);
    std::string getActiveSession() const;
    QString activeSessionPath() const { return _activeSessionPath; }

    // === Recent Sessions ===

    void addRecentSession(const QString& path);
    void removeRecentSession(const QString& path);
    QStringList recentSessions() const { return _recentSessions; }
    void setMaxRecentSessions(int max) { _maxRecentSessions = max; }
    int maxRecentSessions() const { return _maxRecentSessions; }

    // === Session Backup ===

    bool createBackup(const QString& sessionPath);
    bool restoreFromBackup(const QString& sessionPath, int backupNumber = 1);
    int maxBackups() const { return _maxBackups; }
    void setMaxBackups(int max) { _maxBackups = max; }

    // === Recent Files ===

    void addRecentFile(const std::string& path);
    void removeRecentFile(const std::string& path);
    std::vector<std::string> getRecentFiles() const;
    QStringList recentFiles() const { return _recentFiles; }
    void clearRecentFiles();

    // === Working Directory ===

    void setWorkingDirectory(const std::string& dir);
    std::string getWorkingDirectory() const;
    QString workingDirectory() const { return _workingDir; }

    // === Auto-save ===

    void setAutoSaveEnabled(bool enabled) { _autoSaveEnabled = enabled; }
    bool isAutoSaveEnabled() const { return _autoSaveEnabled; }
    void setAutoSaveInterval(int seconds) { _autoSaveInterval = seconds; }
    int autoSaveInterval() const { return _autoSaveInterval; }
    void triggerAutoSave();

    // === Build Session from Current State ===

    // Populate `out` with the current open buffer state (paths, cursors, scroll,
    // panel visibility).  Caller (typically Application) provides geometry and
    // active buffer tracking since that requires access to the main window.
    void captureCurrentSession(NppSession& out);

    // No-arg convenience: capture into the SessionManager's internal session
    // (used by tests and by callers that don't need a separate `out` variable).
    // Equivalent to `captureCurrentSession(_session)`.
    void captureCurrentSession();

    // Validate and normalize a session before restoration.  Returns false if
    // the session is unusable; logs warnings for missing files but continues.
    // Caller (Application) handles opening files and restoring editor state.
    bool applySession(const NppSession& session);

    // === Workspace Integration ===

    // Workspace path for session persistence
    QString workspacePath() const { return _session.workspacePath; }
    void setWorkspacePath(const QString& path) { _session.workspacePath = path; }

signals:
    void sessionLoaded(const QString& path);
    void sessionSaved(const QString& path);
    void recentSessionsChanged();
    void autoSaveTriggered();

private:
    // JSON helpers
    QJsonObject sessionToJson() const;
    bool sessionFromJson(const QJsonObject& obj);

    // Backup helpers
    QString backupPath(const QString& sessionPath, int backupNumber) const;
    bool rotateBackups(const QString& sessionPath);

    // Recent sessions helpers
    void loadRecentSessionsList();
    void saveRecentSessionsList();

    NppSession _session;
    QString _activeSessionPath;
    QString _workingDir;
    QStringList _recentFiles;
    QStringList _recentSessions;
    int _maxRecentSessions = 10;
    int _maxBackups = 5;
    bool _autoSaveEnabled = true;
    int _autoSaveInterval = 60;  // seconds
};
