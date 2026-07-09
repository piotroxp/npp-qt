// SessionManager.h - Session save/restore
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "../common/Types.h"
#include <string>
#include <vector>

class SessionManager : public NonCopyable {
public:
    SessionManager();
    ~SessionManager();

    bool loadSession(const std::string& path);
    bool saveSession(const std::string& path);

    void setActiveSession(const std::string& sessionPath);
    std::string getActiveSession() const;

    void addRecentFile(const std::string& path);
    void removeRecentFile(const std::string& path);
    std::vector<std::string> getRecentFiles() const;
    void clearRecentFiles();

    void setWorkingDirectory(const std::string& dir);
    std::string getWorkingDirectory() const;

    const Session& currentSession() const { return _current; }
    Session& currentSession() { return _current; }

private:
    Session _current;
    std::string _activeSessionPath;
};
