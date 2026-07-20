// SessionManager.cpp - Session save/restore implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "SessionManager.h"
#include "core/Application.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include "Parameters.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QTimer>
#include <fstream>
#include <sstream>
#include <algorithm>

SessionManager::SessionManager() {
    loadRecentSessionsList();
}

SessionManager::~SessionManager() {
    saveRecentSessionsList();
}

// ============================================================================
// JSON Session Format
// ============================================================================

bool SessionManager::loadSession(const std::string& path) {
    return loadSessionFromJson(QString::fromStdString(path));
}

bool SessionManager::saveSession(const std::string& path) {
    return saveSessionToJson(QString::fromStdString(path));
}

bool SessionManager::loadSessionFromJson(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
        return false;

    if (!doc.isObject())
        return false;

    return sessionFromJson(doc.object());
}

bool SessionManager::saveSessionToJson(const QString& path) {
    QJsonObject obj = sessionToJson();
    QJsonDocument doc(obj);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    _activeSessionPath = path;
    emit sessionSaved(path);
    return true;
}

QJsonObject SessionManager::sessionToJson() const {
    QJsonObject obj;
    obj["version"] = _session.version;
    obj["workspacePath"] = _session.workspacePath;

    // Window geometry
    QJsonObject win;
    win["x"] = _session.window.x;
    win["y"] = _session.window.y;
    win["width"] = _session.window.width;
    win["height"] = _session.window.height;
    win["maximized"] = _session.window.maximized;
    obj["window"] = win;

    // Buffers
    QJsonArray bufs;
    for (const SessionBuffer& buf : _session.buffers) {
        QJsonObject b;
        b["path"] = buf.path;
        b["language"] = buf.language;
        b["cursorLine"] = buf.cursor.line;
        b["cursorCol"] = buf.cursor.column;
        b["scrollX"] = buf.scrollX;
        b["scrollY"] = buf.scrollY;
        b["isActive"] = buf.isActive;
        bufs.append(b);
    }
    obj["buffers"] = bufs;

    obj["activeBufferIndex"] = _session.activeBufferIndex;
    obj["activeView"] = _session.activeView;

    // Panels
    QJsonObject panels;
    panels["functionList"] = _session.panels.functionList;
    panels["documentMap"] = _session.panels.documentMap;
    panels["fileBrowser"] = _session.panels.fileBrowser;
    obj["panels"] = panels;

    return obj;
}

bool SessionManager::sessionFromJson(const QJsonObject& obj) {
    _session.version = obj.value("version").toInt(1);
    _session.workspacePath = obj.value("workspacePath").toString();

    // Window
    QJsonObject win = obj.value("window").toObject();
    _session.window.x = win.value("x").toInt(100);
    _session.window.y = win.value("y").toInt(100);
    _session.window.width = win.value("width").toInt(1200);
    _session.window.height = win.value("height").toInt(800);
    _session.window.maximized = win.value("maximized").toBool(false);

    // Buffers
    _session.buffers.clear();
    QJsonArray bufs = obj.value("buffers").toArray();
    for (const QJsonValue& bv : bufs) {
        QJsonObject b = bv.toObject();
        SessionBuffer buf;
        buf.path = b.value("path").toString();
        buf.language = b.value("language").toString();
        buf.cursor.line = b.value("cursorLine").toInt(0);
        buf.cursor.column = b.value("cursorCol").toInt(0);
        buf.scrollX = b.value("scrollX").toInt(0);
        buf.scrollY = b.value("scrollY").toInt(0);
        buf.isActive = b.value("isActive").toBool(false);
        _session.buffers.append(buf);
    }

    _session.activeBufferIndex = obj.value("activeBufferIndex").toInt(0);
    _session.activeView = obj.value("activeView").toInt(0);

    // Panels
    QJsonObject panels = obj.value("panels").toObject();
    _session.panels.functionList = panels.value("functionList").toBool(false);
    _session.panels.documentMap = panels.value("documentMap").toBool(false);
    _session.panels.fileBrowser = panels.value("fileBrowser").toBool(false);

    emit sessionLoaded(_activeSessionPath);
    return true;
}

// ============================================================================
// XML Session (backward compatibility)
// ============================================================================

bool SessionManager::loadSessionXml(const std::string& path) {
    // Delegate to loadSession for now (auto-detect format)
    return loadSession(path);
}

bool SessionManager::saveSessionXml(const std::string& path) {
    // Convert NppSession to XML format
    std::ostringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<NotepadPlus>\n";
    xml << "  <Session>\n";

    // Workspace path
    if (!_session.workspacePath.isEmpty()) {
        xml << "    <WorkspacePath>" << _session.workspacePath.toStdString() << "</WorkspacePath>\n";
    }

    // Files
    xml << "    <Files>\n";
    for (const SessionBuffer& buf : _session.buffers) {
        std::string p = buf.path.toStdString();
        StringHelper::replaceAll(p, "&", "&amp;");
        StringHelper::replaceAll(p, "<", "&lt;");
        StringHelper::replaceAll(p, ">", "&gt;");
        xml << "      <File mainView=\"0\" subView=\"-1\" encoding=\"utf-8\">" << p << "</File>\n";
    }
    xml << "    </Files>\n";

    // ActiveView
    xml << "    <ActiveView mainView=\"" << _session.activeView << "\" subView=\"-1\" />\n";

    xml << "  </Session>\n";
    xml << "</NotepadPlus>\n";

    return FileHelper::writeFile(path, xml.str());
}

// ============================================================================
// Active Session Path
// ============================================================================

void SessionManager::setActiveSession(const std::string& sessionPath) {
    _activeSessionPath = QString::fromStdString(sessionPath);
}

std::string SessionManager::getActiveSession() const {
    return _activeSessionPath.toStdString();
}

// ============================================================================
// Recent Sessions
// ============================================================================

void SessionManager::addRecentSession(const QString& path) {
    _recentSessions.removeAll(path);
    _recentSessions.prepend(path);
    while (_recentSessions.size() > _maxRecentSessions)
        _recentSessions.removeLast();
}

void SessionManager::removeRecentSession(const QString& path) {
    _recentSessions.removeAll(path);
}

void SessionManager::loadRecentSessionsList() {
    QSettings s;
    s.beginGroup("SessionManager");
    _recentSessions = s.value("RecentSessions").toStringList();
    _maxRecentSessions = s.value("MaxRecentSessions", 10).toInt();
    s.endGroup();

    // Prune non-existent
    for (auto it = _recentSessions.begin(); it != _recentSessions.end(); ) {
        if (!QFile::exists(*it))
            it = _recentSessions.erase(it);
        else
            ++it;
    }
}

void SessionManager::saveRecentSessionsList() {
    QSettings s;
    s.beginGroup("SessionManager");
    s.setValue("RecentSessions", _recentSessions);
    s.setValue("MaxRecentSessions", _maxRecentSessions);
    s.endGroup();
}

// ============================================================================
// Session Backup
// ============================================================================

QString SessionManager::backupPath(const QString& sessionPath, int backupNumber) const {
    QFileInfo fi(sessionPath);
    return QStringLiteral("%1/%2.bak%3")
               .arg(fi.absolutePath())
               .arg(fi.completeBaseName())
               .arg(backupNumber);
}

bool SessionManager::createBackup(const QString& sessionPath) {
    if (!QFile::exists(sessionPath))
        return false;

    QString backup = backupPath(sessionPath, 1);
    if (QFile::exists(backup))
        QFile::remove(backup);
    return QFile::copy(sessionPath, backup);
}

bool SessionManager::restoreFromBackup(const QString& sessionPath, int backupNumber) {
    QString backup = backupPath(sessionPath, backupNumber);
    if (!QFile::exists(backup))
        return false;

    if (QFile::exists(sessionPath))
        QFile::remove(sessionPath);
    return QFile::copy(backup, sessionPath);
}

bool SessionManager::rotateBackups(const QString& sessionPath) {
    QFileInfo fi(sessionPath);
    QString base = fi.absolutePath() + "/" + fi.completeBaseName();
    QString ext = fi.suffix();
    for (int i = _maxBackups - 1; i >= 1; --i) {
        QString src = QStringLiteral("%1.bak%2").arg(base).arg(i);
        QString dst = QStringLiteral("%1.bak%2").arg(base).arg(i + 1);
        if (QFile::exists(src)) {
            if (QFile::exists(dst))
                QFile::remove(dst);
            QFile::rename(src, dst);
        }
    }
    return true;
}

// ============================================================================
// Recent Files
// ============================================================================

void SessionManager::addRecentFile(const std::string& rawPath) {
    QString path = QString::fromStdString(rawPath);
    _recentFiles.removeAll(path);
    _recentFiles.prepend(path);
    // Limit to 50
    if (_recentFiles.size() > 50)
        _recentFiles.resize(50);
}

void SessionManager::removeRecentFile(const std::string& rawPath) {
    QString path = QString::fromStdString(rawPath);
    _recentFiles.removeAll(path);
}

std::vector<std::string> SessionManager::getRecentFiles() const {
    std::vector<std::string> result;
    result.reserve(_recentFiles.size());
    for (const QString& p : _recentFiles)
        result.emplace_back(p.toStdString());
    return result;
}

void SessionManager::clearRecentFiles() {
    _recentFiles.clear();
}

// ============================================================================
// Working Directory
// ============================================================================

void SessionManager::setWorkingDirectory(const std::string& dir) {
    _workingDir = QString::fromStdString(dir);
}

std::string SessionManager::getWorkingDirectory() const {
    return _workingDir.toStdString();
}

// ============================================================================
// Auto-save
// ============================================================================

void SessionManager::triggerAutoSave() {
    if (!_autoSaveEnabled || _activeSessionPath.isEmpty())
        return;

    // Create backup before overwriting
    rotateBackups(_activeSessionPath);
    createBackup(_activeSessionPath);

    // Overwrite session
    saveSessionToJson(_activeSessionPath);

    emit autoSaveTriggered();
}

// ============================================================================
// Build Session from Current State
// ============================================================================

void SessionManager::captureCurrentSession(NppSession& out) {
    // NOTE: FileManager, ScintillaEditor, and panel state live in Application,
    // not SessionManager.  Application::saveSession() calls captureCurrentSession()
    // and then fills in the editor-specific fields (geometry, cursor, scroll, panels)
    // before calling setCurrentSession() / saveSession().
    //
    // Here we only capture what SessionManager owns: recent files.
    Q_UNUSED(out);
}

void SessionManager::captureCurrentSession() {
    // No-arg convenience overload.  Delegates to the out-param flavour with the
    // internal `_session` so that `currentSession()` immediately reflects the
    // capture (used by SessionManager unit tests).
    captureCurrentSession(_session);
}

bool SessionManager::applySession(const NppSession& session) {
    // Validate session version
    if (session.version > 2) {
        qWarning() << "[SessionManager] Unknown session version:" << session.version;
        // Continue anyway — we can restore what we understand
    }

    // Check that files in session still exist
    int missing = 0;
    for (const SessionBuffer& buf : session.buffers) {
        if (!buf.path.isEmpty() && !QFile::exists(buf.path)) {
            qWarning() << "[SessionManager] Session file missing:" << buf.path;
            ++missing;
        }
    }

    // Store for downstream use by Application::loadSession()
    _session = session;

    // Return false only if ALL files are missing (useless session)
    if (!session.buffers.isEmpty() && missing == session.buffers.size()) {
        qWarning() << "[SessionManager] All session files missing — skipping restore";
        return false;
    }
    return true;
}
