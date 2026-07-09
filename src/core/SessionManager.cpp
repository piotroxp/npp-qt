// SessionManager.cpp - Session save/restore implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "SessionManager.h"
#include "core/Application.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include <fstream>
#include <sstream>
#include <algorithm>

SessionManager::SessionManager() = default;
SessionManager::~SessionManager() = default;

// ============================================================================
// XML Helpers
// ============================================================================
namespace {

// Find XML tag content (returns content between <tag> and </tag>)
std::string_view extractTagContent(std::string_view xml, std::string_view tagName) {
    std::string openTag = "<" + std::string(tagName) + ">";
    std::string closeTag = "</" + std::string(tagName) + ">";

    auto startPos = xml.find(openTag);
    if (startPos == std::string_view::npos) {
        openTag = "<" + std::string(tagName) + " ";
        startPos = xml.find(openTag);
        if (startPos == std::string_view::npos) return {};
        // Find the end of the opening tag
        auto tagEnd = xml.find('>', startPos);
        if (tagEnd == std::string_view::npos) return {};
        startPos = tagEnd + 1;
    } else {
        startPos += openTag.length();
    }

    auto endPos = xml.find(closeTag, startPos);
    if (endPos == std::string_view::npos) return {};

    return xml.substr(startPos, endPos - startPos);
}

// Extract attribute value from XML tag
std::string extractAttribute(std::string_view tag, const std::string& attrName) {
    std::string search = attrName + "=\"";
    auto pos = tag.find(search);
    if (pos == std::string_view::npos) return "";
    pos += search.length();
    auto end = tag.find('"', pos);
    if (end == std::string_view::npos) return "";
    return std::string(tag.substr(pos, end - pos));
}

// Parse a File element from session
bool parseFileElement(std::string_view fileXml, BufferViewInfo& info) {
    // Look for the file path (content of the File element)
    auto closePos = fileXml.find("</File>");
    if (closePos == std::string_view::npos) return false;

    // Extract path (trim whitespace and encoding markers)
    std::string_view path = fileXml.substr(0, closePos);
    while (!path.empty() && std::isspace(path.front())) path.remove_prefix(1);
    while (!path.empty() && std::isspace(path.back())) path.remove_suffix(1);

    // Get attributes
    std::string mainView = extractAttribute(fileXml, "mainView");
    std::string subView = extractAttribute(fileXml, "subView");

    info._iView = StringHelper::toInt(std::string(mainView), 0);
    // Note: The actual BufferID lookup happens in Application
    // For now we store the path in a way that can be matched later
    return !path.empty();
}

// Get the path from a File element
std::string getFilePathFromElement(std::string_view fileXml) {
    auto closePos = fileXml.find("</File>");
    if (closePos == std::string_view::npos) {
        closePos = fileXml.find("/>");
        if (closePos == std::string_view::npos) return "";
    }
    std::string_view content = fileXml.substr(0, closePos);
    // Remove any leading whitespace
    while (!content.empty() && std::isspace(content.front())) content.remove_prefix(1);
    return std::string(content);
}

} // anonymous namespace

// ============================================================================
// Load Session
// ============================================================================
bool SessionManager::loadSession(const std::string& path) {
    // Check if file exists
    if (!FileHelper::exists(path)) {
        return false;
    }

    // Read file content
    auto contentOpt = FileHelper::readFile(path);
    if (!contentOpt) {
        return false;
    }

    std::string_view xml(*contentOpt);

    // Check for NotepadPlus root element
    if (xml.find("<NotepadPlus>") == std::string_view::npos) {
        return false;
    }

    // Find Session element
    auto sessionStart = xml.find("<Session");
    if (sessionStart == std::string_view::npos) {
        // Try without attributes
        sessionStart = xml.find("<Session>");
        if (sessionStart == std::string_view::npos) {
            return false;
        }
    }

    // Find end of Session element
    auto sessionEnd = xml.find("</Session>");
    if (sessionEnd == std::string_view::npos) {
        return false;
    }
    std::string_view sessionXml = xml.substr(sessionStart, sessionEnd - sessionStart + strlen("</Session>"));

    // Parse Files section
    auto filesStart = sessionXml.find("<Files>");
    if (filesStart != std::string_view::npos) {
        auto filesEnd = sessionXml.find("</Files>");
        if (filesEnd != std::string_view::npos) {
            std::string_view filesXml = sessionXml.substr(filesStart + strlen("<Files>"), filesEnd - filesStart - strlen("<Files>"));

            // Parse each File element
            size_t filePos = 0;
            while ((filePos = filesXml.find("<File", filePos)) != std::string_view::npos) {
                auto fileEnd = filesXml.find("</File>", filePos);
                if (fileEnd == std::string_view::npos) break;

                std::string_view fileXml = filesXml.substr(filePos, fileEnd - filePos + strlen("</File>"));
                std::string filePath = getFilePathFromElement(fileXml);

                if (!filePath.empty()) {
                    BufferViewInfo info;
                    info._iView = StringHelper::toInt(extractAttribute(fileXml, "mainView"), 0);
                    // Store path as filePath in Buffer (we'll resolve to BufferID in Application)
                    // For session loading, we add the file to recent files for later opening
                    std::wstring wpath(filePath.begin(), filePath.end());
                    _current._recentFiles.push_back(wpath);
                }

                filePos = fileEnd + strlen("</File>");
            }
        }
    }

    // Parse ActiveView
    auto activeViewStart = sessionXml.find("<ActiveView");
    if (activeViewStart != std::string_view::npos) {
        std::string_view activeViewTag = sessionXml.substr(activeViewStart);
        auto tagEnd = activeViewTag.find('>');
        if (tagEnd != std::string_view::npos) {
            activeViewTag = activeViewTag.substr(0, tagEnd);
            std::string mainView = extractAttribute(activeViewTag, "mainView");
            _current._activeTab = StringHelper::toInt(mainView, 0);
        }
    }

    // Parse RecentFiles section
    auto recentStart = sessionXml.find("<RecentFiles>");
    if (recentStart != std::string_view::npos) {
        auto recentEnd = sessionXml.find("</RecentFiles>");
        if (recentEnd != std::string_view::npos) {
            std::string_view recentXml = sessionXml.substr(recentStart + strlen("<RecentFiles>"), recentEnd - recentStart - strlen("<RecentFiles>"));

            size_t filePos = 0;
            while ((filePos = recentXml.find("<File", filePos)) != std::string_view::npos) {
                auto fileEnd = recentXml.find("</File>", filePos);
                if (fileEnd == std::string_view::npos) break;

                std::string_view fileXml = recentXml.substr(filePos, fileEnd - filePos + strlen("</File>"));
                std::string filePath = getFilePathFromElement(fileXml);

                if (!filePath.empty()) {
                    std::wstring wpath(filePath.begin(), filePath.end());
                    // Check for duplicates
                    auto it = std::find(_current._recentFiles.begin(), _current._recentFiles.end(), wpath);
                    if (it == _current._recentFiles.end()) {
                        _current._recentFiles.push_back(wpath);
                    }
                }

                filePos = fileEnd + strlen("</File>");
            }
        }
    }

    // Parse WorkingDirectory
    auto workingDirContent = extractTagContent(sessionXml, "WorkingDir");
    if (!workingDirContent.empty()) {
        std::string dir(workingDirContent);
        StringHelper::trim(dir);
        if (!dir.empty()) {
            _current._workingDir = std::wstring(dir.begin(), dir.end());
        }
    }

    return true;
}

// ============================================================================
// Save Session
// ============================================================================
bool SessionManager::saveSession(const std::string& path) {
    std::ostringstream xml;

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<NotepadPlus>\n";
    xml << "  <Session>\n";

    // Save Files section
    xml << "    <Files>\n";

    // Get all open buffers from Application
    int bufferCount = app().getBufferCount();
    int activeView = app().activeViewId();

    for (int i = 0; i < bufferCount; ++i) {
        BufferID buf = app().getBufferAt(i);
        if (buf) {
            auto nameOpt = app().getFileName(buf);
            if (nameOpt && !nameOpt->empty()) {
                // Escape XML special characters in path
                std::string escapedPath = *nameOpt;
                StringHelper::replaceAll(escapedPath, "&", "&amp;");
                StringHelper::replaceAll(escapedPath, "<", "&lt;");
                StringHelper::replaceAll(escapedPath, ">", "&gt;");
                StringHelper::replaceAll(escapedPath, "\"", "&quot;");

                xml << "      <File mainView=\"" << i << "\" subView=\"-1\""
                    << " encoding=\"utf-8\">" << escapedPath << "</File>\n";
            }
        }
    }

    xml << "    </Files>\n";

    // Save ActiveView
    xml << "    <ActiveView mainView=\"" << activeView << "\" subView=\"-1\" />\n";

    // Save RecentFiles
    xml << "    <RecentFiles>\n";
    for (const auto& wpath : _current._recentFiles) {
        std::string path(wpath.begin(), wpath.end());
        // Escape XML special characters
        std::string escapedPath = path;
        StringHelper::replaceAll(escapedPath, "&", "&amp;");
        StringHelper::replaceAll(escapedPath, "<", "&lt;");
        StringHelper::replaceAll(escapedPath, ">", "&gt;");
        StringHelper::replaceAll(escapedPath, "\"", "&quot;");

        xml << "      <File encoding=\"utf-8\">" << escapedPath << "</File>\n";
    }
    xml << "    </RecentFiles>\n";

    // Save WorkingDirectory
    if (!_current._workingDir.empty()) {
        std::string workDir(_current._workingDir.begin(), _current._workingDir.end());
        xml << "    <WorkingDir>" << workDir << "</WorkingDir>\n";
    }

    xml << "  </Session>\n";
    xml << "</NotepadPlus>\n";

    // Write to file
    return FileHelper::writeFile(path, xml.str());
}

// ============================================================================
// Active Session Path
// ============================================================================
void SessionManager::setActiveSession(const std::string& sessionPath) {
    _activeSessionPath = sessionPath;
}

std::string SessionManager::getActiveSession() const {
    return _activeSessionPath;
}

// ============================================================================
// Recent Files
// ============================================================================
void SessionManager::addRecentFile(const std::string& path) {
    std::wstring wpath(path.begin(), path.end());

    // Remove if already exists (to move to front)
    auto it = std::remove(_current._recentFiles.begin(), _current._recentFiles.end(), wpath);
    _current._recentFiles.erase(it, _current._recentFiles.end());

    // Add to front
    _current._recentFiles.insert(_current._recentFiles.begin(), wpath);

    // Limit to max recent files
    int maxRecent = app().options().maxRecentFiles;
    if ((int)_current._recentFiles.size() > maxRecent) {
        _current._recentFiles.resize(maxRecent);
    }
}

void SessionManager::removeRecentFile(const std::string& path) {
    std::wstring wpath(path.begin(), path.end());
    auto it = std::remove(_current._recentFiles.begin(), _current._recentFiles.end(), wpath);
    _current._recentFiles.erase(it, _current._recentFiles.end());
}

std::vector<std::string> SessionManager::getRecentFiles() const {
    std::vector<std::string> result;
    result.reserve(_current._recentFiles.size());
    for (const auto& wpath : _current._recentFiles) {
        result.emplace_back(wpath.begin(), wpath.end());
    }
    return result;
}

void SessionManager::clearRecentFiles() {
    _current._recentFiles.clear();
}

// ============================================================================
// Working Directory
// ============================================================================
void SessionManager::setWorkingDirectory(const std::string& dir) {
    _current._workingDir = std::wstring(dir.begin(), dir.end());
}

std::string SessionManager::getWorkingDirectory() const {
    if (_current._workingDir.empty()) {
        return "";
    }
    return std::string(_current._workingDir.begin(), _current._workingDir.end());
}
