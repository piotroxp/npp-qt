// StubApplication.cpp — Minimal stub for Application/DpiManager/ThemeManager (MOC-processed)
// Compiled as SHARED library so AUTOMOC runs and links moc symbols correctly.

#include "TestStubs.h"
#include <QCoreApplication>
#include <QDir>

// ─── DpiManager stub ──────────────────────────────────────────────────────────
DpiManager& DpiManager::instance() {
    static DpiManager inst;
    return inst;
}
DpiManager::DpiManager(QObject* parent) : QObject(parent), _scaleFactor(1.0), _dpi(96), _logicalDpi(96) {}
DpiManager::~DpiManager() = default;
void DpiManager::onScreenChanged() {}
void DpiManager::onScreensChanged() {}

// ─── ThemeManager stub ─────────────────────────────────────────────────────────
ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}
ThemeManager::ThemeManager() = default;
ThemeManager::~ThemeManager() = default;
QStringList ThemeManager::availableThemes() const { return {"default"}; }

// ─── Application singleton + constructor ────────────────────────────────────────
//
// Mirror the real Application::instance() idiom (Construct-On-First-Use
// + atomic re-entry guard). See src/core/Application.cpp for the full
// rationale. The stub deliberately reproduces the multi-thread safety
// property so that test_application_singleton can verify it under
// contention without dragging in the full Application initialisation
// path (which needs MainWindow, QApplication, Qt platform plugin, etc.).

#include <atomic>
#include <thread>

Application& Application::instance() {
    static Application* inst = nullptr;
    if (inst) return *inst;

    static std::atomic<bool> in_progress{false};
    bool expected = false;
    while (!in_progress.compare_exchange_strong(expected, true)) {
        std::this_thread::yield();
        expected = false;
    }

    if (inst) {
        in_progress.store(false);
        return *inst;
    }

    inst = new Application();
    in_progress.store(false);
    return *inst;
}
Application::Application() : QObject(QCoreApplication::instance()), _state(AppState::Starting) {}
Application::~Application() { }

bool Application::initialize() { _state = AppState::Ready; return true; }
void Application::shutdown() { }
void Application::setConfigDirectory(const std::string&) { }
void Application::setPluginDirectory(const std::string&) { }
void Application::setThemeProfile(const std::string&) { }
bool Application::saveConfig(const std::string&) { return true; }
BufferID Application::openFile(const std::string&, bool) { return BUFFER_INVALID; }
BufferID Application::openFiles(const std::vector<std::string>&) { return BUFFER_INVALID; }
bool Application::saveFile(BufferID, const std::string&) { return false; }
bool Application::saveAllFiles() { return false; }
bool Application::reloadFile(BufferID) { return false; }
bool Application::closeFile(BufferID, bool) { return false; }
bool Application::closeAllFiles() { return false; }
void Application::closeAllButCurrent() { }
bool Application::deleteFile(BufferID) { return false; }
bool Application::renameFile(BufferID, const QString&) { return false; }
void Application::cloneToOtherView(BufferID) { }
void Application::moveToSubView(BufferID) { }
void Application::zoomIn() { }
void Application::zoomOut() { }

// ─── Application slot stubs ──────────────────────────────────────────────────────────
void Application::onBufferOpened(BufferID) {}
void Application::onBufferActivated(BufferID) {}
void Application::onBufferClosed(BufferID) {}
void Application::onBufferChanged(BufferID) {}
void Application::onMenuCommand(const QString&) {}
void Application::onToolBarCommand(const QString&) {}
void Application::onFileSaved(const std::string&) {}
void Application::onFileModifiedExternally(const std::string&) {}
void Application::onExternalFileChanged(const QString&) {}
void Application::onThemeChanged(const std::string&) {}
void Application::onNewFile() {}
void Application::onOpenFile() {}
void Application::onSaveFile() {}
void Application::onSaveFileAs() {}
void Application::onSaveAll() {}
void Application::onCloseFile() {}
void Application::onCloseAll() {}
void Application::onCloseAllButCurrent() {}
void Application::onExit() {}
void Application::onClearRecentFiles() {}
void Application::onDeleteFile() {}
void Application::onRenameFile() {}
void Application::onSaveAsCopy() {}
void Application::closeAllFilesButCurrent() {}
void Application::onCopyToNamedClipboard() {}
void Application::onMoveToNamedClipboard() {}
void Application::onPasteFromNamedClipboard() {}
void Application::onFindInProjects() {}
void Application::onReplaceInProjects() {}
void Application::onReplaceInFiles() {}
void Application::onUndo() {}
void Application::onRedo() {}
void Application::onCut() {}
void Application::onCopy() {}
void Application::onPaste() {}
void Application::onDelete() {}
void Application::onSelectAll() {}
void Application::onDeleteLine() {}
void Application::onJoinLines() {}
void Application::onTrimTrailing() {}
void Application::onTrimLeading() {}
void Application::onSortAscending() {}
void Application::onSortDescending() {}
void Application::onSortIntAscending() {}
void Application::onSortIntDescending() {}
void Application::onSortReverse() {}
void Application::onOpenContainingFolder() {}
void Application::onSearchOnInternet() {}
void Application::onFind() {}
void Application::onReplace() {}
void Application::onGotoLine() {}
void Application::onFindNext() {}
void Application::onFindPrev() {}
void Application::onFindInFiles() {}
void Application::onCount() {}
void Application::onMarkAll() {}
void Application::onUnmarkAll() {}
void Application::showFindInFilesResults(const QList<FindResult>&) {}
void Application::onToggleFullScreen() {}
void Application::onToggleDistractionFree() {}
void Application::onToggleTabBar() {}
void Application::onToggleStatusBar() {}
void Application::onToggleToolBar() {}
void Application::onZoomIn() {}
void Application::onZoomOut() {}
void Application::onZoomRestore() {}
void Application::onToggleWordWrap() {}
void Application::onToggleEolVisibility() {}
void Application::onShowAllCharacters() {}
void Application::onToggleDocMap() {}
void Application::onToggleFunctionList() {}
void Application::onToggleFileBrowser() {}
void Application::onToggleClipboardHistory() {}
void Application::onCloneToOtherView() {}
void Application::onMoveToSubView() {}
void Application::onConvertEncoding(EncodingType) {}
void Application::onSetEol(EolType) {}
void Application::onMacroStartRecording() {}
void Application::onMacroStopRecording() {}
void Application::onMacroPlaybackLast() {}
void Application::onMacroRecord() {}
void Application::onMacroStop() {}
void Application::onMacroPlayback() {}
void Application::onMacroSave() {}
void Application::playbackMacroStep(int, uintptr_t, uintptr_t, const char*, int) {}
void Application::onPrint() {}
ApplicationImportResult Application::importFromNpp() { return {}; }
ApplicationImportResult Application::importFromNpp(const QString&) { return {}; }
bool Application::exportSettingsToJson(const QString&) { return false; }
bool Application::exportSettingsToNpp(const QString&) { return false; }
bool Application::loadUdlsFromNpp(const QString&) { return false; }
bool Application::loadUdlsFromNpp() { return false; }
QStringList Application::detectNppPaths() const { return {}; }
void Application::onConvertToCharset(const QString&) {}
void Application::onEolConversion(const QString&) {}
void Application::onSetLanguage(LangType) {}
void Application::onManageUserLanguages() {}
void Application::onShowPreferences() {}
void Application::onShowShortcutMapper() {}
void Application::onReloadFile() {}
void Application::onShowCommandPalette() {}
void Application::openFolderAsWorkspace(const std::string&) {}
void Application::onRun() {}
void Application::onShowAbout() {}
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
