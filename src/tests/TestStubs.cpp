// TestStubs.cpp - Minimal stubs for test_core
// Provides real implementations of Application, ThemeManager, DpiManager
// that ScintillaEditor depends on, without pulling in the full heavy
// Application hierarchy.
//
// The real DpiManager.cpp and ThemeManager.cpp provide constructors only.
// These stubs implement the actual methods needed by consumers.

#include "core/Application.h"
#include "core/ThemeManager.h"
#include "common/DpiManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// ─── DpiManager stub ──────────────────────────────────────────────────────────
DpiManager& DpiManager::instance() {
    static DpiManager inst;
    return inst;
}

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

// ─── ThemeManager stub ────────────────────────────────────────────────────────
ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

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

// ─── Application stub ─────────────────────────────────────────────────────────
Application& Application::instance() {
    static Application* inst = [] {
        if (QCoreApplication::instance() == nullptr) {
            static int argc = 0;
            static char* argv = nullptr;
            (void)new QCoreApplication(argc, &argv);
        }
        static Application app;
        return &app;
    }();
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

BufferID Application::openFile(const std::string&, bool) { return 0; }
BufferID Application::openFiles(const std::vector<std::string>&) { return 0; }
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
void Application::showAllCharacters() { }
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
