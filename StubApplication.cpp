// StubApplication.cpp — Application/DpiManager stub for test_core
#include "core/Application.h"
#include "core/ThemeManager.h"
#include "common/DpiManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// ─── DpiManager stub ────────────────────────────────────────────────────
DpiManager& DpiManager::instance() { static DpiManager inst; return inst; }
DpiManager::DpiManager(QObject* parent) : QObject(parent), _scaleFactor(1.0), _dpi(96), _logicalDpi(96) {}
DpiManager::~DpiManager() = default;
void DpiManager::recalculateDpi() { }
void DpiManager::updateFromScreen(QScreen*) { }
QString DpiManager::findScaledIcon(const QString&) const { return {}; }
QString DpiManager::scaleStyleSheet(const QString& ss) const { return ss; }
void DpiManager::applyScaledMinimumSize(QWidget*, int, int) { }
void DpiManager::retranslateUi(QWidget*) { }
void DpiManager::onScreenChanged() { }
void DpiManager::onScreensChanged() { }
int DpiManager::detectScreenDpi() const { return 96; }

// ─── ThemeManager stub ────────────────────────────────────────────────
ThemeManager& ThemeManager::instance() { static ThemeManager inst; return inst; }
ThemeManager::ThemeManager() = default;
ThemeManager::~ThemeManager() = default;
QStringList ThemeManager::availableThemes() const { return {"default"}; }
bool ThemeManager::loadTheme(const QString&) { return true; }
bool ThemeManager::loadThemeFromPath(const QString&) { return false; }
ThemeColors ThemeManager::getThemeColors(const QString&) const { return {}; }
QString ThemeManager::getThemeQss(const QString&, const QString&) const { return {}; }
QString ThemeManager::currentThemeQss(const QString&) const { return {}; }
QString ThemeManager::getScintillaStyle(const QString&, const QString&) const { return {}; }
QString ThemeManager::currentScintillaStyle(const QString&) const { return {}; }
void ThemeManager::resetToDefaults() { }
void ThemeManager::applyDefaultDark() { }
void ThemeManager::applyDefaultLight() { }

// ─── Application stub ─────────────────────────────────────────────────
Application& Application::instance() {
    static Application* inst = [] {
        if (QCoreApplication::instance() == nullptr) {
            static int ac = 0; static char* av[] = { nullptr };
            (void)new QCoreApplication(ac, av);
        }
        static Application app;
        return &app;
    }();
    return *inst;
}
Application::Application() : QObject(QCoreApplication::instance()), _state(AppState::Starting) {}
Application::~Application() { }

// ─── Public API stubs ────────────────────────────────────────────────
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
void Application::zoomRestore() { }
void Application::toggleWordWrap() { }
void Application::toggleEolVisibility() { }
void Application::toggleDocMap() { }
void Application::toggleFunctionList() { }
void Application::toggleFileBrowser() { }
void Application::toggleClipboardHistory() { }
void Application::deleteLine() { }
void Application::joinLines() { }
void Application::trimTrailing() { }
void Application::trimLeading() { }
void Application::sortAscending() { }
void Application::sortDescending() { }
void Application::sortIntAscending() { }
void Application::sortIntDescending() { }
void Application::sortReverse() { }
void Application::unmarkAll() { }
void Application::openContainingFolder(BufferID) { }
void Application::searchOnInternet() { }
BufferID Application::newFile(const std::string&) { return BUFFER_INVALID; }
BufferID Application::duplicateBuffer(BufferID) { return BUFFER_INVALID; }
std::optional<std::string> Application::getFileName(BufferID) const { return {}; }
BufferID Application::getActiveBuffer() const { return BUFFER_INVALID; }
BufferID Application::getBufferAt(int, int) const { return BUFFER_INVALID; }
BufferID Application::getBufferForPath(const std::string&) const { return BUFFER_INVALID; }
int Application::getBufferCount() const { return 0; }
void Application::setActiveBuffer(BufferID) { }
bool Application::isBufferModified(BufferID) const { return false; }
std::string Application::getBufferText(BufferID) const { return {}; }
bool Application::setBufferText(BufferID, const std::string&) { return false; }
EncodingType Application::getBufferEncoding(BufferID) const { return EncodingType::UTF_8; }
bool Application::setBufferEncoding(BufferID, EncodingType) { return false; }
EolType Application::getBufferEol(BufferID) const { return EolType::EOL_LF; }
bool Application::setBufferEol(BufferID, EolType) { return false; }
int Application::currentView() const { return 0; }
void Application::switchToView(int) { }
ScintillaEditor* Application::addView() { return nullptr; }
void Application::closeView(int) { }
int Application::activeViewId() const { return 0; }
bool Application::loadSession(const std::string&) { return false; }
bool Application::saveSession(const std::string&) { return false; }
void Application::addToRecentFiles(const std::string&) { }
std::vector<std::string> Application::getRecentFiles() const { return {}; }
void Application::clearRecentFiles() { }
void Application::showMainWindow() { }
void Application::hideMainWindow() { }
void Application::toggleFullScreen() { }
void Application::toggleDistractionFree() { }
void Application::closeAllBuffersExcept(BufferID) { }
void Application::setStatusBarText(const std::string&) { }
void Application::setStatusBarEncoding(const std::string&) { }
void Application::setStatusBarEol(const std::string&) { }
void Application::setStatusBarLanguage(const std::string&) { }
void Application::setStatusBarPosition(int, int) { }
void Application::setStatusBarSelection(int, int) { }
EncodingDetector* Application::encodingDetector() const { return nullptr; }
LanguageManager* Application::languageManager() const { return nullptr; }
FileManager* Application::fileManager() const { return nullptr; }
void Application::convertEncoding(EncodingType) { }
void Application::setLanguage(LangType) { }
std::string Application::encodingToString(EncodingType) const { return {}; }
void Application::loadTheme(const std::string&) { }
std::string Application::currentTheme() const { return {}; }
void Application::addToClipboardHistory(const std::string&) { }
std::vector<std::string> Application::getClipboardHistory() const { return {}; }
void Application::executeCommand(const std::string&) { }
void Application::executeCommand(int) { }
EditorCommandManager* Application::commandManager() const { return nullptr; }
MacroManager* Application::macroManager() const { return nullptr; }
RecentFilesManager* Application::recentFilesManager() const { return nullptr; }
void Application::updateWindowTitle() { }

// ─── Slots ────────────────────────────────────────────────────────────
void Application::onMenuCommand(const QString&) { }
void Application::onToolBarCommand(const QString&) { }
void Application::playbackMacroStep(int, uintptr_t, uintptr_t, const char*, int) { }
void Application::onBufferOpened(BufferID) { }
void Application::onBufferActivated(BufferID) { }
void Application::onBufferClosed(BufferID) { }
void Application::onBufferChanged(BufferID) { }
void Application::onFileSaved(const std::string&) { }
void Application::onFileModifiedExternally(const std::string&) { }
void Application::onExternalFileChanged(const QString&) { }
void Application::onThemeChanged(const std::string&) { }
void Application::onNewFile() { }
void Application::onOpenFile() { }
void Application::onSaveFile() { }
void Application::onSaveFileAs() { }
void Application::onSaveAll() { }
void Application::onCloseFile() { }
void Application::onCloseAll() { }
void Application::onCloseAllButCurrent() { }
void Application::onExit() { }
void Application::onClearRecentFiles() { }
void Application::onDeleteFile() { }
void Application::onRenameFile() { }
void Application::onSaveAsCopy() { }
void Application::onCopyToNamedClipboard() { }
void Application::onMoveToNamedClipboard() { }
void Application::onPasteFromNamedClipboard() { }
void Application::onFindInProjects() { }
void Application::onReplaceInProjects() { }
void Application::onReplaceInFiles() { }
void Application::onUndo() { }
void Application::onRedo() { }
void Application::onCut() { }
void Application::onCopy() { }
void Application::onPaste() { }
void Application::onDelete() { }
void Application::onSelectAll() { }
void Application::onDeleteLine() { }
void Application::onJoinLines() { }
void Application::onTrimTrailing() { }
void Application::onTrimLeading() { }
void Application::onSortAscending() { }
void Application::onSortDescending() { }
void Application::onSortIntAscending() { }
void Application::onSortIntDescending() { }
void Application::onSortReverse() { }
void Application::onOpenContainingFolder() { }
void Application::onSearchOnInternet() { }
void Application::onFind() { }
void Application::onReplace() { }
void Application::onGotoLine() { }
void Application::onFindNext() { }
void Application::onFindPrev() { }
void Application::onFindInFiles() { }
void Application::onCount() { }
void Application::onMarkAll() { }
void Application::onUnmarkAll() { }
void Application::showFindInFilesResults(const QList<FindResult>&) { }
void Application::onToggleFullScreen() { }
void Application::onToggleDistractionFree() { }
void Application::onToggleTabBar() { }
void Application::onToggleStatusBar() { }
void Application::onToggleToolBar() { }
void Application::onZoomIn() { }
void Application::onZoomOut() { }
void Application::onZoomRestore() { }
void Application::onToggleWordWrap() { }
void Application::onToggleEolVisibility() { }
void Application::onShowAllCharacters() { }
void Application::onToggleDocMap() { }
void Application::onToggleFunctionList() { }
void Application::onToggleFileBrowser() { }
void Application::onToggleClipboardHistory() { }
void Application::onCloneToOtherView() { }
void Application::onMoveToSubView() { }
void Application::onConvertEncoding(EncodingType) { }
void Application::onSetEol(EolType) { }
void Application::onMacroStartRecording() { }
void Application::onMacroStopRecording() { }
void Application::onMacroPlaybackLast() { }
void Application::onMacroRecord() { }
void Application::onMacroStop() { }
void Application::onMacroPlayback() { }
void Application::onMacroSave() { }
void Application::onPrint() { }
ApplicationImportResult Application::importFromNpp() { return ApplicationImportResult::ImportResult_Success; }
ApplicationImportResult Application::importFromNpp(const QString&) { return ApplicationImportResult::ImportResult_Success; }
QStringList Application::detectNppPaths() const { return {}; }
bool Application::exportSettingsToJson(const QString&) { return false; }
bool Application::exportSettingsToNpp(const QString&) { return false; }
bool Application::loadUdlsFromNpp(const QString&) { return false; }
bool Application::loadUdlsFromNpp() { return false; }
void Application::onConvertToCharset(const QString&) { }
void Application::onEolConversion(const QString&) { }
void Application::onSetLanguage(LangType) { }
void Application::onManageUserLanguages() { }
void Application::onShowPreferences() { }
void Application::onShowShortcutMapper() { }
void Application::onReloadFile() { }
void Application::onShowCommandPalette() { }
void Application::openFolderAsWorkspace(const std::string&) { }
void Application::onRun() { }
void Application::onShowAbout() { }
