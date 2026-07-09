// Application.cpp - Main application controller implementation
#include "Application.h"
#include "FileManager.h"
#include "EncodingDetector.h"
#include "LanguageManager.h"
#include "SessionManager.h"
#include "EditorCommandManager.h"
#include "editor/ScintillaEditor.h"
#include "editor/SyntaxHighlighter.h"
#include "ui/MainWindow.h"
#include "ui/TabBar.h"
#include "ui/StatusBar.h"
#include "ui/ToolBar.h"
#include "ui/MenuBar.h"
#include "dialogs/FindReplaceDialog.h"
#include "dialogs/GoToLineDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/AboutDialog.h"
#include "panels/DocumentMapPanel.h"
#include "panels/FunctionListPanel.h"
#include "panels/FileBrowserPanel.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include "common/Util.h"
#include "Constants.h"
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QInputDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>
#include <QWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>

// ============================================================================
// Singleton
// ============================================================================
Application& Application::instance() {
    static Application inst;
    return inst;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
Application::Application() : QObject(qApp) {
    _mainWindow = new QMainWindow();
    _centralWidget = new QWidget();
    _viewStack = new QStackedWidget();
}

Application::~Application() {
    if (_findReplaceDialog) delete _findReplaceDialog;
    if (_gotoLineDialog) delete _gotoLineDialog;
    if (_preferenceDialog) delete _preferenceDialog;
    if (_aboutDialog) delete _aboutDialog;
    if (_docMapPanel) delete _docMapPanel;
    if (_funcListPanel) delete _funcListPanel;
    if (_fileBrowserPanel) delete _fileBrowserPanel;
    if (_commandManager) delete _commandManager;
    if (_sessionManager) delete _sessionManager;
    if (_languageManager) delete _languageManager;
    if (_encodingDetector) delete _encodingDetector;
    if (_fileManager) delete _fileManager;
    delete _mainWindow;
}

// ============================================================================
// Initialization
// ============================================================================
bool Application::initialize() {
    std::lock_guard<std::mutex> lock(_mutex);

    try {
        logInfo("Initializing Notepad--Qt " + std::string(APP_VERSION));

        // Setup directories
        if (!setupDirectories()) {
            _lastError = "Failed to create application directories";
            _state = AppState::Error;
            return false;
        }

        // Load configuration
        if (!loadConfig()) {
            logWarning("No config file found, using defaults");
        }

        // Initialize core managers
        _fileManager = new FileManager();
        _encodingDetector = new EncodingDetector();
        _languageManager = new LanguageManager();
        _sessionManager = new SessionManager();
        _commandManager = new EditorCommandManager();
        _commandManager->registerAll(this);

        // Setup UI
        setupUI();
        setupConnections();

        _state = AppState::Ready;
        logInfo("Initialization complete");
        return true;
    } catch (const std::exception& e) {
        _lastError = e.what();
        _state = AppState::Error;
        logError(std::string("Initialization failed: ") + e.what());
        return false;
    }
}

bool Application::setupDirectories() {
    auto configDir = FileHelper::getConfigDirectory();
    if (!FileHelper::exists(configDir)) {
        if (!FileHelper::createDirectory(configDir)) return false;
    }
    if (!FileHelper::exists(configDir + "/" + THEMES_DIR)) {
        FileHelper::createDirectory(configDir + "/" + THEMES_DIR);
    }
    if (!FileHelper::exists(configDir + "/" + PLUGIN_DIR)) {
        FileHelper::createDirectory(configDir + "/" + PLUGIN_DIR);
    }
    if (!FileHelper::exists(configDir + "/" + USER_CONFIG_DIR)) {
        FileHelper::createDirectory(configDir + "/" + USER_CONFIG_DIR);
    }
    return true;
}

bool Application::loadConfig() {
    auto configPath = FileHelper::getConfigFilePath();
    IniParser ini;
    if (!ini.load(configPath)) return false;

    _options.singleInstance     = ini.getBool("General", "SingleInstance", true);
    _options.showTabBar         = ini.getBool("General", "ShowTabBar", true);
    _options.showStatusBar      = ini.getBool("General", "ShowStatusBar", true);
    _options.showToolBar        = ini.getBool("General", "ShowToolBar", true);
    _options.showMenuBar        = ini.getBool("General", "ShowMenuBar", true);
    _options.rememberSession    = ini.getBool("General", "RememberSession", true);
    _options.maxRecentFiles    = ini.getInt("General", "MaxRecentFiles", 50);
    _options.defaultTabWidth   = ini.getInt("Editor", "TabWidth", 4);
    _options.defaultTabAsSpaces = ini.getBool("Editor", "TabAsSpaces", false);
    _options.defaultEolType    = EolType::EOL_LF;
    _options.defaultEncoding    = EncodingType::UTF_8_BOM;
    _options.defaultLang        = LangType::L_TEXT;
    _options.themeProfile       = ini.get("General", "Theme", "default");
    _currentTheme               = _options.themeProfile;

    return true;
}

bool Application::saveConfig(const std::string& path) {
    IniParser ini;
    ini.set("General", "SingleInstance",    _options.singleInstance);
    ini.set("General", "ShowTabBar",         _options.showTabBar);
    ini.set("General", "ShowStatusBar",      _options.showStatusBar);
    ini.set("General", "ShowToolBar",       _options.showToolBar);
    ini.set("General", "ShowMenuBar",       _options.showMenuBar);
    ini.set("General", "RememberSession",   _options.rememberSession);
    ini.set("General", "MaxRecentFiles",    _options.maxRecentFiles);
    ini.set("General", "Theme",             _options.themeProfile);
    ini.set("Editor",  "TabWidth",          _options.defaultTabWidth);
    ini.set("Editor",  "TabAsSpaces",       _options.defaultTabAsSpaces);
    return ini.save(FileHelper::getConfigFilePath());
}

// ============================================================================
// UI Setup
// ============================================================================
void Application::setupUI() {
    _mainWindow->setWindowTitle(APP_Title);
    _mainWindow->setCentralWidget(_centralWidget);

    QHBoxLayout* centralLayout = new QHBoxLayout(_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(_viewStack);

    // Add initial view
    addView();

    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockingPanels();

    // Create dialogs
    _findReplaceDialog = new FindReplaceDialog(_mainWindow);
    _gotoLineDialog = new GoToLineDialog(_mainWindow);
    _preferenceDialog = new PreferenceDialog(_mainWindow);
    _aboutDialog = new AboutDialog(_mainWindow);
}

void Application::setupMenuBar() {
    _menuBar = new MenuBar(_mainWindow);
    _mainWindow->setMenuBar(_menuBar);
    connect(_menuBar, &MenuBar::menuCommand, this, &Application::onMenuCommand);
}

void Application::setupToolBar() {
    _toolBar = new ToolBar(_mainWindow);
    _mainWindow->addToolBar(_toolBar);
    connect(_toolBar, &ToolBar::toolBarCommand, this, &Application::onToolBarCommand);
}

void Application::setupStatusBar() {
    StatusBar* statusBar = new StatusBar(_mainWindow);
    _mainWindow->setStatusBar(statusBar);
}

void Application::setupDockingPanels() {
    _docMapPanel = new DocumentMapPanel(_mainWindow);
    _funcListPanel = new FunctionListPanel(_mainWindow);
    _fileBrowserPanel = new FileBrowserPanel(_mainWindow);

    _mainWindow->addDockWidget(Qt::RightDockWidgetArea, _docMapPanel);
    _mainWindow->addDockWidget(Qt::RightDockWidgetArea, _funcListPanel);
    _mainWindow->addDockWidget(Qt::LeftDockWidgetArea, _fileBrowserPanel);
}

void Application::setupConnections() {
    // Buffer signals
    connect(this, &Application::bufferOpened, this, [this](BufferID) { updateUI(); });
    connect(this, &Application::bufferActivated, this, [this](BufferID) { updateUI(); });
    connect(this, &Application::fileSaved, this, &Application::onFileSaved);

    // Command execution
    connect(this, &Application::commandExecuted, this, [this](int cmdId) {
        _commandManager->execute(cmdId);
    });
}

// ============================================================================
// Shutdown
// ============================================================================
void Application::shutdown() {
    std::lock_guard<std::mutex> lock(_mutex);
    _state = AppState::Closing;

    logInfo("Application shutting down");

    // Save session
    if (_options.rememberSession) {
        saveSession();
    }

    // Save config
    saveConfig();

    // Close all files
    closeAllFiles();

    _state = AppState::Starting;
}

// ============================================================================
// View Management
// ============================================================================
int Application::addView() {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    ScintillaEditor* editor = new ScintillaEditor();
    layout->addWidget(editor);

    int viewId = _viewStack->addWidget(container);
    _viewContainers.push_back(container);
    return viewId;
}

void Application::closeView(int viewId) {
    if (_viewContainers.size() <= 1) return;  // Keep at least one view
    if (viewId < 0 || viewId >= (int)_viewContainers.size()) return;

    QWidget* container = _viewContainers[viewId];
    _viewStack->removeWidget(container);
    _viewContainers.erase(_viewContainers.begin() + viewId);
    delete container;
}

void Application::switchToView(int viewId) {
    if (viewId < 0 || viewId >= (int)_viewContainers.size()) return;
    _activeViewId = viewId;
    _viewStack->setCurrentIndex(viewId);
    updateUI();
}

int Application::currentView() const {
    return _activeViewId;
}

// ============================================================================
// File Operations
// ============================================================================
BufferID Application::openFile(const std::string& path, bool readOnly) {
    return _fileManager->openFile(path, readOnly);
}

BufferID Application::openFiles(const std::vector<std::string>& paths) {
    BufferID last = nullptr;
    for (const auto& p : paths) {
        last = openFile(p);
    }
    return last;
}

bool Application::saveFile(BufferID buffer, const std::string& path) {
    return _fileManager->saveFile(buffer, path);
}

bool Application::saveAllFiles() {
    return _fileManager->saveAllFiles();
}

bool Application::closeFile(BufferID buffer, bool force) {
    (void)force;
    return _fileManager->closeFile(buffer);
}

bool Application::closeAllFiles() {
    return _fileManager->closeAllFiles();
}

BufferID Application::newFile(const std::string& encoding) {
    return _fileManager->createNewFile();
}

BufferID Application::duplicateBuffer(BufferID buffer) {
    return _fileManager->duplicateBuffer(buffer);
}

std::optional<std::string> Application::getFileName(BufferID buffer) const {
    return _fileManager->getFileName(buffer);
}

// ============================================================================
// Buffer Management
// ============================================================================
BufferID Application::getActiveBuffer() const {
    return _fileManager->getActiveBuffer();
}

BufferID Application::getBufferAt(int index, int view) const {
    (void)view;
    return _fileManager->getBufferAt(index);
}

int Application::getBufferCount() const {
    return static_cast<int>(_fileManager->getBufferCount());
}

void Application::setActiveBuffer(BufferID buffer) {
    _fileManager->setActiveBuffer(buffer);
    emit bufferActivated(buffer);
}

bool Application::isBufferModified(BufferID buffer) const {
    return _fileManager->isBufferModified(buffer);
}

std::string Application::getBufferText(BufferID buffer) const {
    return _fileManager->getBufferText(buffer);
}

bool Application::setBufferText(BufferID buffer, const std::string& text) {
    return _fileManager->setBufferText(buffer, text);
}

EncodingType Application::getBufferEncoding(BufferID buffer) const {
    return _fileManager->getEncoding(buffer);
}

bool Application::setBufferEncoding(BufferID buffer, EncodingType enc) {
    return _fileManager->setEncoding(buffer, enc);
}

// ============================================================================
// Main Window
// ============================================================================
void Application::showMainWindow() {
    _mainWindow->show();
}

void Application::hideMainWindow() {
    _mainWindow->hide();
}

void Application::toggleFullScreen() {
    if (_isFullScreen) {
        _mainWindow->showNormal();
        _isFullScreen = false;
    } else {
        _mainWindow->showFullScreen();
        _isFullScreen = true;
    }
}

void Application::toggleDistractionFree() {
    _isDistractionFree = !_isDistractionFree;
    if (_isDistractionFree) {
        _mainWindow->menuBar()->hide();
        _mainWindow->statusBar()->hide();
        // Hide all toolbars and panels
        for (auto* tb : _mainWindow->findChildren<QToolBar*>()) tb->hide();
        for (auto* dw : _mainWindow->findChildren<QDockWidget*>()) dw->hide();
    } else {
        _mainWindow->menuBar()->show();
        _mainWindow->statusBar()->show();
        for (auto* tb : _mainWindow->findChildren<QToolBar*>()) tb->show();
        for (auto* dw : _mainWindow->findChildren<QDockWidget*>()) dw->show();
    }
}

// ============================================================================
// Status Bar
// ============================================================================
void Application::setStatusBarText(const std::string& text) {
    _mainWindow->statusBar()->showMessage(QString::fromUtf8(text.c_str()));
}

void Application::setStatusBarEncoding(const std::string& enc) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setEncoding(QString::fromUtf8(enc.c_str()));
}

void Application::setStatusBarEol(const std::string& eol) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setEol(QString::fromUtf8(eol.c_str()));
}

void Application::setStatusBarLanguage(const std::string& lang) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setLanguage(QString::fromUtf8(lang.c_str()));
}

void Application::setStatusBarPosition(int line, int col) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setPosition(line, col);
}

void Application::setStatusBarSelection(int selStart, int selEnd) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setSelection(selStart, selEnd);
}

// ============================================================================
// Session
// ============================================================================
bool Application::loadSession(const std::string& path) {
    return _sessionManager->loadSession(path);
}

bool Application::saveSession(const std::string& path) {
    auto sessionPath = path.empty() ? FileHelper::getSessionFilePath() : path;
    return _sessionManager->saveSession(sessionPath);
}

void Application::addToRecentFiles(const std::string& path) {
    _sessionManager->addRecentFile(path);
}

std::vector<std::string> Application::getRecentFiles() const {
    return _sessionManager->getRecentFiles();
}

void Application::clearRecentFiles() {
    _sessionManager->clearRecentFiles();
}

// ============================================================================
// Editor
// ============================================================================
ScintillaEditor* Application::getEditor(int viewId) const {
    if (viewId < 0) viewId = _activeViewId;
    if (viewId < 0 || viewId >= (int)_viewContainers.size()) return nullptr;
    auto* editor = _viewContainers[viewId]->findChild<ScintillaEditor*>();
    return editor;
}

ScintillaEditor* Application::getActiveEditor() const {
    return getEditor(_activeViewId);
}

// ============================================================================
// Theme
// ============================================================================
void Application::loadTheme(const std::string& themeName) {
    _currentTheme = themeName;
    emit themeChanged(themeName);
}

void Application::onThemeChanged(const std::string& themeName) {
    _currentTheme = themeName;
    _themeObserver.notify({themeName});
}

// ============================================================================
// Clipboard
// ============================================================================
void Application::addToClipboardHistory(const std::string& text) {
    if (text.empty()) return;
    _clipboardHistory.erase(
        std::remove(_clipboardHistory.begin(), _clipboardHistory.end(), text),
        _clipboardHistory.end());
    _clipboardHistory.insert(_clipboardHistory.begin(), text);
    if (_clipboardHistory.size() > MAX_CLIPBOARD_HISTORY)
        _clipboardHistory.resize(MAX_CLIPBOARD_HISTORY);
}

std::vector<std::string> Application::getClipboardHistory() const {
    return _clipboardHistory;
}

// ============================================================================
// Command execution
// ============================================================================
void Application::executeCommand(const std::string& cmd) {
    _commandManager->execute(cmd);
}

void Application::executeCommand(int commandId) {
    _commandManager->execute(commandId);
}

// ============================================================================
// Slots
// ============================================================================
void Application::onBufferChanged(BufferID buffer) {
    emit bufferActivated(buffer);
}

void Application::onFileSaved(const std::string& path) {
    emit fileSaved(path);
}

void Application::onFileModifiedExternally(const std::string& path) {
    // Prompt user to reload
    auto* mb = new QMessageBox(QMessageBox::Question,
        "File Modified",
        QString("The file has been modified externally:\n%1\n\nDo you want to reload it?").arg(QString::fromUtf8(path.c_str())),
        QMessageBox::Yes | QMessageBox::No);
    if (mb->exec() == QMessageBox::Yes) {
        openFile(path);
    }
    delete mb;
}

void Application::updateUI() {
    // Update window title with current file
    auto buffer = getActiveBuffer();
    if (buffer) {
        auto name = getFileName(buffer);
        if (name) {
            QString title = QString("%1 - %2").arg(QString::fromUtf8(name->c_str())).arg(APP_Title);
            if (isBufferModified(buffer)) title += " *";
            _mainWindow->setWindowTitle(title);
        }
    } else {
        _mainWindow->setWindowTitle(APP_Title);
    }
}

void Application::setConfigDirectory(const std::string& dir) { _options.configDir = dir; }
void Application::setPluginDirectory(const std::string& dir) { _options.pluginDir = dir; }
void Application::setThemeProfile(const std::string& profile) { _options.themeProfile = profile; }

void Application::onMenuCommand(const QString& cmd) {
    executeCommand(cmd.toStdString());
}

void Application::onToolBarCommand(const QString& cmd) {
    executeCommand(cmd.toStdString());
}

// ============================================================================
// File command slots
// ============================================================================
void Application::onNewFile() {
    qDebug() << "[App] New file";
    // Emit bufferOpened with 0 as placeholder; MainWindow handles buffer creation
    emit bufferOpened(nullptr);
}

void Application::onOpenFile() {
    qDebug() << "[App] Open file";
    QString filePath = QFileDialog::getOpenFileName(
        nullptr, "Open File", QString(),
        "All Files (*);;Text Files (*.txt);;C/C++ (*.cpp *.h);;Python (*.py);;JavaScript (*.js)"
    );
    if (!filePath.isEmpty()) {
        openFile(filePath.toStdString());
    }
}

void Application::onSaveFile() {
    qDebug() << "[App] Save file";
    BufferID buf = getActiveBuffer();
    if (buf != EDITOR_INVALID) {
        saveFile(buf);
    }
}

void Application::onSaveFileAs() {
    qDebug() << "[App] Save file as";
    QString filePath = QFileDialog::getSaveFileName(
        nullptr, "Save File As", QString(),
        "All Files (*);;Text Files (*.txt);;C/C++ (*.cpp *.h)"
    );
    if (!filePath.isEmpty()) {
        BufferID buf = getActiveBuffer();
        if (buf) saveFile(buf, filePath.toStdString());
    }
}

void Application::onSaveAll() {
    qDebug() << "[App] Save all";
    int count = getBufferCount();
    for (int i = 0; i < count; ++i) {
        BufferID id = getBufferAt(i);
        if (id) saveFile(id);
    }
}

void Application::onCloseFile() {
    qDebug() << "[App] Close file";
    BufferID buf = getActiveBuffer();
    if (buf != EDITOR_INVALID) {
        closeFile(buf);
    }
}

void Application::onCloseAll() {
    qDebug() << "[App] Close all files";
    int count = getBufferCount();
    for (int i = count - 1; i >= 0; --i) {
        BufferID id = getBufferAt(i);
        if (id) closeFile(id);
    }
}

void Application::onExit() {
    qDebug() << "[App] Exit";
    shutdown();
    QCoreApplication::quit();
}

void Application::onClearRecentFiles() {
    qDebug() << "[App] Clear recent files";
    if (_sessionManager) {
        _sessionManager->clearRecentFiles();
    }
}

// ============================================================================
// Edit command slots
// ============================================================================
void Application::onUndo() {
    qDebug() << "[App] Undo";
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->undo();
}

void Application::onRedo() {
    qDebug() << "[App] Redo";
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->redo();
}

void Application::onCut() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->cut();
}

void Application::onCopy() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->copy();
}

void Application::onPaste() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->paste();
}

void Application::onDelete() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->deleteSelection();
}

void Application::onSelectAll() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->selectAll();
}

void Application::onFind() {
    qDebug() << "[App] Find";
    if (!_findReplaceDialog) {
        _findReplaceDialog = new FindReplaceDialog(_mainWindow);
    }
    _findReplaceDialog->show();
}

void Application::onReplace() {
    qDebug() << "[App] Replace";
    if (!_findReplaceDialog) {
        _findReplaceDialog = new FindReplaceDialog(_mainWindow);
    }
    _findReplaceDialog->show();
}

void Application::onGotoLine() {
    qDebug() << "[App] Go to line";
    if (!_gotoLineDialog) {
        _gotoLineDialog = new GoToLineDialog(_mainWindow);
    }
    _gotoLineDialog->show();
}

// ============================================================================
// Search command slots
// ============================================================================
void Application::onFindNext() {
    qDebug() << "[App] Find next";
    if (_findReplaceDialog) {
        _findReplaceDialog->findNext();
    }
}

void Application::onFindPrev() {
    qDebug() << "[App] Find previous";
    if (_findReplaceDialog) {
        _findReplaceDialog->findPrevious();
    }
}

void Application::onFindInFiles() {
    qDebug() << "[App] Find in files";
    // TODO: implement find-in-files dialog
}

void Application::onCount() {
    qDebug() << "[App] Count occurrences";
    // TODO: implement count
}

void Application::onMarkAll() {
    qDebug() << "[App] Mark all";
    // TODO: implement mark
}

// ============================================================================
// View command slots
// ============================================================================
void Application::onToggleFullScreen() {
    qDebug() << "[App] Toggle full screen";
    QWidget* w = _mainWindow;
    if (w->windowState() & Qt::WindowFullScreen) {
        w->setWindowState(w->windowState() & ~Qt::WindowFullScreen);
    } else {
        w->setWindowState(w->windowState() | Qt::WindowFullScreen);
    }
}

void Application::onToggleDistractionFree() {
    qDebug() << "[App] Toggle distraction-free";
    // TODO: implement distraction-free mode (hide all UI except editor)
}

void Application::onToggleTabBar() {
    qDebug() << "[App] Toggle tab bar";
    if (_mainWindow) {
        // Find and toggle the tab bar visibility
        QList<QTabBar*> bars = _mainWindow->findChildren<QTabBar*>();
        for (QTabBar* bar : bars) {
            bar->setVisible(!bar->isVisible());
        }
    }
}

void Application::onToggleStatusBar() {
    if (_mainWindow) {
        QStatusBar* sb = _mainWindow->statusBar();
        if (sb) sb->setVisible(!sb->isVisible());
    }
}

void Application::onToggleToolBar() {
    if (_mainWindow) {
        QList<QToolBar*> bars = _mainWindow->findChildren<QToolBar*>();
        for (QToolBar* bar : bars) {
            bar->setVisible(!bar->isVisible());
        }
    }
}

// ============================================================================
// Encoding command slots
// ============================================================================
void Application::onConvertEncoding(EncodingType enc) {
    qDebug() << "[App] Convert encoding to" << static_cast<int>(enc);
    // TODO: implement encoding conversion
    Q_UNUSED(enc);
}

void Application::onSetLanguage(LangType lang) {
    qDebug() << "[App] Set language" << static_cast<int>(lang);
    // TODO: implement language change
    Q_UNUSED(lang);
}

// ============================================================================
// Settings command slots
// ============================================================================
void Application::onShowPreferences() {
    qDebug() << "[App] Show preferences";
    if (!_preferenceDialog) {
        _preferenceDialog = new PreferenceDialog(_mainWindow);
    }
    _preferenceDialog->show();
}

void Application::onShowShortcutMapper() {
    qDebug() << "[App] Show shortcut mapper";
    // TODO: implement shortcut mapper
}

void Application::onShowCommandPalette() {
    qDebug() << "[App] Show command palette";
    // TODO: implement command palette
}

// ============================================================================
// Help command slots
// ============================================================================
void Application::onShowAbout() {
    qDebug() << "[App] Show about";
    if (!_aboutDialog) {
        _aboutDialog = new AboutDialog(_mainWindow);
    }
    _aboutDialog->show();
}
