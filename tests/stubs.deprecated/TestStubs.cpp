// TestStubs.cpp — minimal implementations for test compilation
// Provides implementations for TestStubs.h declarations.

#include "TestStubs.h"
#include <QString>

// ─── Application ─────────────────────────────────────────────────────────────
Application& Application::instance() {
    static Application inst;
    return inst;
}
Application::Application() = default;
Application::~Application() = default;

// ─── Sub-managers ────────────────────────────────────────────────────────────
FileManager::FileManager() = default;
FileManager::~FileManager() = default;

LanguageManager::LanguageManager() = default;
LanguageManager::~LanguageManager() = default;

SessionManager::SessionManager() = default;
SessionManager::~SessionManager() = default;

EditorCommandManager::EditorCommandManager() = default;
EditorCommandManager::~EditorCommandManager() = default;
QString EditorCommandManager::getShortcut(const QString&) const { return {}; }
void EditorCommandManager::setShortcut(const QString&, const QString&) {}

MacroManager::MacroManager() = default;
MacroManager::~MacroManager() = default;

// ─── RecentFilesManager ──────────────────────────────────────────────────────
RecentFilesManager& RecentFilesManager::instance() {
    static RecentFilesManager inst;
    return inst;
}
RecentFilesManager::RecentFilesManager() = default;
RecentFilesManager::~RecentFilesManager() = default;

// === Wave 1: Additional Application stubs ===
void Application::onGoToNextMark() {}
void Application::onGoToPrevMark() {}
void Application::onToggleShowSymbol() {}
void Application::onSwitchToOther() {}
void Application::onToolbarCustomize() {}
void Application::onMinimizeToTray() {}
void Application::onActivatePane(int) {}
void Application::onReplaceAll() {}
void Application::onPurge() {}
void Application::onBookmarkAll() {}
