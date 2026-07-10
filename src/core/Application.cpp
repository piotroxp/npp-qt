// Application.cpp - Main application controller implementation
#include "Application.h"
#include "FileManager.h"
#include "Buffer.h"
#include "EncodingDetector.h"
#include "LanguageManager.h"
#include "SessionManager.h"
#include "EditorCommandManager.h"
#include "MacroManager.h"
#include "RecentFilesManager.h"
#include "Parameters.h"
#include "UdlManager.h"
#include "editor/ScintillaEditor.h"
#include "editor/SyntaxHighlighter.h"
#include <Qsci/qsciscintilla.h>
#include "ui/MainWindow.h"
#include "ui/TabBar.h"
#include "ui/StatusBar.h"
#include "ui/ToolBar.h"
#include "ui/MenuBar.h"
#include "dialogs/FindReplaceDialog.h"
#include "dialogs/FindInFilesWorker.h"
#include "dialogs/GoToLineDialog.h"
#include "dialogs/ShortcutMapperDialog.h"
#include "ShortcutManager.h"
#include "dialogs/CommandPaletteDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/AboutDialog.h"
#include "panels/DocumentMapPanel.h"
#include "panels/FunctionListPanel.h"
#include "panels/FileBrowserPanel.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include "common/Util.h"
#include "Constants.h"
#include "Parameters.h"
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QUrl>
#include <QFileDialog>
#include <QInputDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QTextStream>
#include <QFile>
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
    // Delete owned objects BEFORE _mainWindow.
    // NOTE: panels and dialogs are parented to _mainWindow, so Qt will cascade-delete
    // them when _mainWindow is destroyed. We must NOT delete them again here.
    // Only delete objects that are NOT parented by Qt's ownership tree.
    if (_commandManager) delete _commandManager;
    if (_sessionManager) delete _sessionManager;
    if (_languageManager) delete _languageManager;
    if (_encodingDetector) delete _encodingDetector;
    if (_fileManager) delete _fileManager;

    // Delete view containers (ScintillaEditor widgets).  Must happen before
    // _mainWindow is destroyed so that the ScintillaEditor destructors run
    // while the Qt event loop is still alive (see ScintillaEditor::~ScintillaEditor).
    for (QWidget* container : _viewContainers) {
        delete container;
    }
    _viewContainers.clear();

    // Delete main window last — this triggers Qt's parent-child cascade delete
    // for all dock widgets, panels, and dialogs that are children of MainWindow.
    // After this, _findReplaceDialog, _docMapPanel, etc. are already deleted.
    delete _mainWindow;

    // These are now nullptr after _mainWindow deletion — do NOT delete again:
    // _findReplaceDialog, _gotoLineDialog, _preferenceDialog,
    // _aboutDialog, _docMapPanel, _funcListPanel, _fileBrowserPanel
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
        _macroManager = new MacroManager();
        _recentFilesManager = &RecentFilesManager::instance();
        _udlManager = new UdlManager(this);
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
    // Menu/ToolBar commands → command dispatch (menu already connected in setupMenuBar)
    connect(_toolBar, &ToolBar::toolBarCommand, this, &Application::onToolBarCommand);

    // Buffer lifecycle signals
    connect(this, &Application::bufferOpened, this, &Application::onBufferOpened);
    connect(this, &Application::bufferActivated, this, &Application::onBufferActivated);
    connect(this, &Application::bufferClosed, this, &Application::onBufferClosed);
    connect(this, &Application::bufferOpened, this, [this](BufferID) { updateUI(); });
    connect(this, &Application::bufferActivated, this, [this](BufferID) { updateUI(); });
    connect(this, &Application::fileSaved, this, &Application::onFileSaved);

    // File browser → open file
    if (_fileBrowserPanel) {
        connect(_fileBrowserPanel, &FileBrowserPanel::fileDoubleClicked,
                this, [this](const QString& path) {
            openFile(path.toStdString());
        });
    }

    // Find/Replace dialog
    if (_findReplaceDialog) {
        connect(_findReplaceDialog, &FindReplaceDialog::findNextRequested,
                this, [this](const QString&, FindOption) {
            onFindNext();
        });
        connect(_findReplaceDialog, &FindReplaceDialog::replaceRequested,
                this, [this](const QString&, const QString&, FindOption) {
            onFindNext();  // advance to next match after replace
        });
        connect(_findReplaceDialog, &FindReplaceDialog::countRequested,
                this, [this](const QString&, FindOption) { onCount(); });
        // Keep dialog's editor in sync with the active editor
        connect(this, &Application::activeEditorChanged, _findReplaceDialog,
                [this](ScintillaEditor* ed) {
                    if (_findReplaceDialog) _findReplaceDialog->setEditor(ed);
                });
    }

    // Theme changes
    connect(this, &Application::themeChanged, _mainWindow,
            [this](const std::string& name) {
                auto mw = static_cast<MainWindow*>(_mainWindow);
                mw->onThemeChanged(QString::fromStdString(name));
            });

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
    BufferID id = _fileManager->openFile(QString::fromStdString(path), readOnly);
    if (id && !path.empty())
        RecentFilesManager::instance().addFile(QString::fromStdString(path));
    return id;
}

BufferID Application::openFiles(const std::vector<std::string>& paths) {
    BufferID last = nullptr;
    for (const auto& p : paths) {
        last = openFile(p);
    }
    return last;
}

bool Application::saveFile(BufferID buffer, const std::string& path) {
    return _fileManager->saveFile(buffer, path.empty() ? QString() : QString::fromStdString(path));
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

void Application::closeAllBuffersExcept(BufferID keepId) {
    if (!keepId || !_fileManager) return;
    size_t count = _fileManager->getBufferCount();
    std::vector<BufferID> toClose;
    for (size_t i = 0; i < count; ++i) {
        BufferID id = _fileManager->getBufferAt(static_cast<int>(i), 0);
        if (id && id != keepId) {
            toClose.push_back(id);
        }
    }
    for (BufferID id : toClose) {
        _fileManager->closeFile(id);
    }
}

BufferID Application::newFile(const std::string& encoding) {
    return _fileManager->createNewFile();
}

BufferID Application::duplicateBuffer(BufferID buffer) {
    return _fileManager->duplicateBuffer(buffer);
}

std::optional<std::string> Application::getFileName(BufferID buffer) const {
    QString name = _fileManager->getFileName(buffer);
    if (name.isEmpty()) return std::nullopt;
    return name.toStdString();
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
    return _fileManager->getBufferText(buffer).toStdString();
}

bool Application::setBufferText(BufferID buffer, const std::string& text) {
    return _fileManager->setBufferText(buffer, QString::fromStdString(text));
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
    return _activeEditor;
}

bool Application::syncEditorToBuffer(ScintillaEditor* editor, BufferID buffer) {
    if (!editor || !buffer) return false;
    QString text = editor->text();
    return _fileManager->setBufferText(buffer, text);
}

void Application::setActiveEditor(ScintillaEditor* editor) {
    _activeEditor = editor;
    if (editor) {
        emit activeEditorChanged(editor);
    }
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

// ============================================================================
// Buffer lifecycle slots — connected to Application's own signals
// ============================================================================
void Application::onBufferOpened(BufferID buffer) {
    qDebug() << "[App] Buffer opened:" << buffer;
    // MainWindow listens to bufferOpened and creates the tab
}

void Application::onBufferActivated(BufferID buffer) {
    qDebug() << "[App] Buffer activated:" << buffer;
    updateUI();
}

void Application::onBufferClosed(BufferID buffer) {
    qDebug() << "[App] Buffer closed:" << buffer;
    // Nothing to do here — MainWindow removes the tab
    Q_UNUSED(buffer);
}

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
    BufferID buf = newFile();
    if (buf) {
        emit bufferOpened(buf);
        emit bufferActivated(buf);
    }
}

void Application::onOpenFile() {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr, "Open File", QString(),
        "All Files (*);;Text Files (*.txt);;C/C++ (*.cpp *.h);;Python (*.py);;JavaScript (*.js)"
    );
    if (!filePath.isEmpty()) {
        BufferID buf = openFile(filePath.toStdString());
        if (buf) {
            emit bufferOpened(buf);
            emit bufferActivated(buf);
        }
    }
}

void Application::onSaveFile() {
    qDebug() << "[App] Save file";
    BufferID buf = getActiveBuffer();
    if (buf) {
        // Sync editor text to buffer before saving
        ScintillaEditor* ed = getActiveEditor();
        if (ed) {
            QString text = ed->text();
            _fileManager->setBufferText(buf, text);
        }
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
        if (buf) {
            // Sync editor text to buffer before saving
            ScintillaEditor* ed = getActiveEditor();
            if (ed) {
                QString text = ed->text();
                _fileManager->setBufferText(buf, text);
            }
            saveFile(buf, filePath.toStdString());
        }
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
    if (_sessionManager) {
        _sessionManager->clearRecentFiles();
    }
}

// ============================================================================
// Edit command slots
// ============================================================================
void Application::onUndo() {
    ScintillaEditor* ed = getActiveEditor();
    if (ed) ed->undo();
}

void Application::onRedo() {
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
    if (!_findReplaceDialog) {
        _findReplaceDialog = new FindReplaceDialog(_mainWindow);
    }
    _findReplaceDialog->show();
}

void Application::onReplace() {
    if (!_findReplaceDialog) {
        _findReplaceDialog = new FindReplaceDialog(_mainWindow);
    }
    _findReplaceDialog->show();
}

void Application::onGotoLine() {
    if (!_gotoLineDialog) {
        _gotoLineDialog = new GoToLineDialog(_mainWindow);
    }
    _gotoLineDialog->show();
}

// ============================================================================
// Search command slots
// ============================================================================
void Application::onFindNext() {
    if (_findReplaceDialog) {
        _findReplaceDialog->findNext();
    } else {
        onFind();
    }
}

void Application::onFindPrev() {
    if (_findReplaceDialog) {
        _findReplaceDialog->findPrevious();
    } else {
        onFind();
    }
}

void Application::onFindInFiles() {
    // Open directory picker first, then search
    QString dir = QFileDialog::getExistingDirectory(_mainWindow,
        "Select Directory to Search In", QDir::homePath());
    if (dir.isEmpty()) return;

    QString searchText = "";
    if (_findReplaceDialog) {
        searchText = _findReplaceDialog->lastSearchText();
    }
    if (searchText.isEmpty()) {
        searchText = QInputDialog::getText(_mainWindow, "Find in Files",
            "Search text:");
    }
    if (searchText.isEmpty()) return;

    // Search in background using QThread
    auto* thread = new QThread;
    auto* worker = new FindInFilesWorker(dir, searchText);
    worker->moveToThread(thread);

    QObject::connect(thread, &QThread::started, worker, &FindInFilesWorker::run);
    QObject::connect(worker, &FindInFilesWorker::resultsReady, this,
        [this](const QList<FindResult>& results) {
            showFindInFilesResults(results);
        }, Qt::QueuedConnection);
    QObject::connect(worker, &FindInFilesWorker::finished, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();

    setStatusBarText(("Searching in " + dir + "...").toStdString());
}

void Application::onCount() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    QString pattern;
    if (_findReplaceDialog) {
        pattern = _findReplaceDialog->lastSearchText();
    }

    if (pattern.isEmpty()) {
        setStatusBarText("Count: no search term");
        return;
    }

    FindOption opts = _findReplaceDialog ? _findReplaceDialog->lastSearchOptions() : FindOption::None;
    int count = ed->countMatches(pattern, opts);

    QString msg = QString("Found %1 occurrence%2 of \"%3\"")
                      .arg(count)
                      .arg(count == 1 ? "" : "s")
                      .arg(pattern);
    setStatusBarText(msg.toStdString());
    if (_findReplaceDialog) {
        _findReplaceDialog->setMatchCount(count);
    }
}

void Application::onMarkAll() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    QString pattern = ed->selectedText();
    if (pattern.isEmpty() && _findReplaceDialog) {
        pattern = _findReplaceDialog->lastSearchText();
    }

    if (pattern.isEmpty()) {
        setStatusBarText("Mark All: no text selected or search term entered");
        return;
    }

    FindOption opts = FindOption::None;
    if (_findReplaceDialog) {
        opts = _findReplaceDialog->lastSearchOptions();
    }

    ed->clearSelection();
    int count = ed->markAllMatches(pattern, opts);

    QString msg = QString("Marked %1 occurrence%2 of \"%3\"")
                      .arg(count)
                      .arg(count == 1 ? "" : "s")
                      .arg(pattern);
    setStatusBarText(msg.toStdString());
}

// ============================================================================
// View command slots
// ============================================================================
void Application::onToggleFullScreen() {
    QWidget* w = _mainWindow;
    if (w->windowState() & Qt::WindowFullScreen) {
        w->setWindowState(w->windowState() & ~Qt::WindowFullScreen);
    } else {
        w->setWindowState(w->windowState() | Qt::WindowFullScreen);
    }
}

void Application::onToggleDistractionFree() {
    qDebug() << "[App] Toggle distraction-free";
    _isDistractionFree = !_isDistractionFree;

    if (_isDistractionFree) {
        // Hide all UI except the editor
        if (_menuBar) _menuBar->setVisible(false);
        if (_toolBar) _toolBar->setVisible(false);

        QList<QTabBar*> bars = _mainWindow->findChildren<QTabBar*>();
        for (QTabBar* bar : bars) {
            bar->setVisible(false);
        }

        QStatusBar* sb = _mainWindow->statusBar();
        if (sb) sb->setVisible(false);

        // Hide docking panels
        if (_docMapPanel) _docMapPanel->hide();
        if (_funcListPanel) _funcListPanel->hide();
        if (_fileBrowserPanel) _fileBrowserPanel->hide();

        // Hide find/replace dialog if visible
        if (_findReplaceDialog) _findReplaceDialog->hide();

        // Enter fullscreen
        _mainWindow->showFullScreen();
    } else {
        // Restore all UI
        if (_menuBar) _menuBar->setVisible(true);
        if (_toolBar) _toolBar->setVisible(true);

        QList<QTabBar*> bars = _mainWindow->findChildren<QTabBar*>();
        for (QTabBar* bar : bars) {
            bar->setVisible(true);
        }

        QStatusBar* sb = _mainWindow->statusBar();
        if (sb) sb->setVisible(true);

        // Show panels based on their previous state (default to visible)
        if (_docMapPanel) _docMapPanel->show();
        if (_funcListPanel) _funcListPanel->show();
        if (_fileBrowserPanel) _fileBrowserPanel->show();

        // Exit fullscreen
        _mainWindow->showNormal();
    }
}

void Application::onToggleTabBar() {
    if (_mainWindow) {
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
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    BufferID buffer = getActiveBuffer();
    if (!buffer) return;

    QString text = QString::fromStdString(getBufferText(buffer));
    if (text.isEmpty()) return;

    // Map EncodingType → Scintilla codepage → QTextCodec MIB
    auto encToMib = [](EncodingType e) -> int {
        switch (e) {
            case EncodingType::ANSI:       return 4;     // Latin-1
            case EncodingType::UTF_8:
            case EncodingType::UTF_8_BOM:  return 106;    // UTF-8
            case EncodingType::UTF_16_LE: return 1000;   // UTF-16LE
            case EncodingType::UTF_16_BE: return 1001;   // UTF-16BE
            case EncodingType::OEM:        return 4;      // OEM (Latin-1 as fallback)
            default:                       return 4;
        }
    };

    EncodingType currentEnc = getBufferEncoding(buffer);
    int fromMib = encToMib(currentEnc);
    int toMib   = encToMib(enc);

    if (fromMib != toMib) {
        text = NppParameters::getInstance().convertEncoding(text, fromMib, toMib);
    }

    // Apply the new encoding to buffer and editor
    setBufferEncoding(buffer, enc);
    ed->setEncoding(enc);
    ed->setText(text);
    ed->setModified(true);

    // Update status bar with the new encoding name
    const char* encName = "ANSI";
    switch (enc) {
        case EncodingType::UTF_8:      encName = "UTF-8"; break;
        case EncodingType::UTF_8_BOM:  encName = "UTF-8-BOM"; break;
        case EncodingType::UTF_16_LE: encName = "UTF-16 LE"; break;
        case EncodingType::UTF_16_BE: encName = "UTF-16 BE"; break;
        case EncodingType::OEM:       encName = "OEM"; break;
        default:                      encName = "ANSI"; break;
    }
    setStatusBarEncoding(encName);
}

void Application::onSetLanguage(LangType lang) {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    // Use language manager to get the appropriate lexer
    if (_languageManager) {
        ed->setLanguage(lang);
    }
}

// ============================================================================
// Encoding conversion helper (called from menu commands)
// ============================================================================
void Application::convertEncoding(EncodingType enc) {
    onConvertEncoding(enc);
}

// ============================================================================
// Language change helper (called from menu commands)
// ============================================================================
void Application::setLanguage(LangType lang) {
    onSetLanguage(lang);
}

// ============================================================================
// Settings command slots
// ============================================================================
void Application::onShowPreferences() {
    if (!_preferenceDialog) {
        _preferenceDialog = new PreferenceDialog(_mainWindow);
    }
    _preferenceDialog->show();
    _preferenceDialog->raise();
    _preferenceDialog->activateWindow();
}

void Application::onShowShortcutMapper() {
    ShortcutMapperDialog dlg(_mainWindow);
    dlg.exec();
}

void Application::onShowCommandPalette() {
    static CommandPaletteDialog* dlg = nullptr;
    if (!dlg) dlg = new CommandPaletteDialog(_mainWindow);
    // Center on main window
    if (_mainWindow) {
        QPoint center = _mainWindow->mapToGlobal(_mainWindow->rect().center());
        dlg->move(center.x() - dlg->width() / 2, center.y() - 100);
    }
    dlg->show();
    dlg->activateWindow();
}

// ============================================================================
// Help command slots
// ============================================================================
void Application::onShowAbout() {
    if (!_aboutDialog) {
        _aboutDialog = new AboutDialog(_mainWindow);
    }
    _aboutDialog->show();
}

// Find in Files — uses FindInFilesWorker from dialogs/FindInFilesWorker.h
// ============================================================================

void Application::showFindInFilesResults(const QList<FindResult>& results) {
    QString msg;
    if (results.isEmpty()) {
        msg = "No matches found.";
    } else {
        msg = QString("Found %1 match%2:").arg(results.size()).arg(results.size() == 1 ? "" : "es");
        for (const FindResult& r : results) {
            // Build a results panel or show in find dialog
            // For now, show first 20 in status
            static int shown = 0;
            shown = (shown + 1) % 1000;  // cycle to avoid spam
        }
    }
    setStatusBarText(msg.toStdString());

    // Show results in a dialog
    if (results.isEmpty()) {
        QMessageBox::information(_mainWindow, "Find in Files", msg);
        return;
    }

    // Build simple results dialog
    auto* dlg = new QDialog(_mainWindow);
    dlg->setWindowTitle(QString("Find in Files — %1 results").arg(results.size()));
    dlg->resize(700, 400);
    QVBoxLayout* lay = new QVBoxLayout(dlg);
    QTableWidget* table = new QTableWidget(dlg);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"File", "Line", "Content"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setRowCount(qMin(results.size(), 500));
    for (int i = 0; i < table->rowCount(); ++i) {
        const FindResult& r = results[i];
        table->setItem(i, 0, new QTableWidgetItem(r.filePath));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(r.lineNumber)));
        table->setItem(i, 2, new QTableWidgetItem(r.lineContent));
    }
    table->resizeColumnsToContents();
    lay->addWidget(table);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Close, dlg);
    lay->addWidget(box);
    connect(box, static_cast<void(QDialogButtonBox::*)(QAbstractButton*)>(&QDialogButtonBox::clicked), dlg, [dlg](QAbstractButton*) { dlg->close(); });

    // Double-click to open file at line
    connect(table, &QTableWidget::cellDoubleClicked, this,
        [this, table, results](int row, int) {
            if (row >= results.size()) return;
            const FindResult& r = results[row];
            openFile(r.filePath.toStdString());
            ScintillaEditor* ed = getActiveEditor();
            if (ed) ed->gotoLine(r.lineNumber - 1, r.column);
        });

    dlg->show();
}

// ============================================================================
// Notepad++ Compatibility
// ============================================================================

#include "UdlManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QProgressDialog>
#include <QFileDialog>

ApplicationImportResult Application::importFromNpp() {
    QStringList paths = detectNppPaths();
    if (paths.isEmpty())
        return ImportResult_NppNotFound;
    return importFromNpp(paths.first());
}

ApplicationImportResult Application::importFromNpp(const QString& nppPath) {
    QDir nppDir(nppPath);
    if (!nppDir.exists()) {
        qWarning() << "[Application] N++ path does not exist:" << nppPath;
        return ImportResult_NppNotFound;
    }

    int imported = 0;
    int failed = 0;

    // Progress dialog
    QProgressDialog progress(tr("Importing from Notepad++..."), tr("Cancel"), 0, 4, _mainWindow);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue(0);

    // 1. Import recent files from config/recentFileList.xml
    QString recentPath = nppPath + "/config/recentFileList.xml";
    if (QFile::exists(recentPath)) {
        QFile file(recentPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader reader(&file);
            while (!reader.atEnd()) {
                if (reader.readNext() == QXmlStreamReader::StartElement) {
                    if (reader.name().toString() == QStringLiteral("File")) {
                        QString path = reader.attributes().value("path").toString();
                        if (!path.isEmpty()) {
                            _recentFilesManager->addFile(path);
                            ++imported;
                        }
                    }
                }
            }
            file.close();
        }
        progress.setValue(1);
    }

    // 2. Import shortcuts from config/shortcuts.xml
    QString shortcutsPath = nppPath + "/config/shortcuts.xml";
    if (QFile::exists(shortcutsPath)) {
        QFile file(shortcutsPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader reader(&file);
            while (!reader.atEnd()) {
                if (reader.readNext() == QXmlStreamReader::StartElement) {
                    if (reader.name().toString() == QStringLiteral("Shortcut")) {
                        QString name = reader.attributes().value("name").toString();
                        QString ctrl = reader.attributes().value("ctrl").toString();
                        QString alt = reader.attributes().value("alt").toString();
                        QString shift = reader.attributes().value("shift").toString();
                        QString key = reader.attributes().value("key").toString();
                        if (!name.isEmpty()) {
                            Shortcut sc;
                            sc.name = name.toStdString();
                            sc.ctrl = ctrl.toInt();
                            sc.alt = alt.toInt();
                            sc.shift = shift.toInt();
                            sc.key = key.toInt();
                            NppParameters::getInstance().setShortcuts(
                                NppParameters::getInstance().getShortcuts());
                            ++imported;
                        }
                    }
                }
            }
            file.close();
        }
        progress.setValue(2);
    }

    // 3. Import UDL files
    QString udlDir = nppPath + "/notepad-plus-plus-translation/PowerEditor/bin/userDefineLangs";
    if (QDir(udlDir).exists() && _udlManager) {
        int udlCount = _udlManager->loadAllUdls(udlDir);
        imported += udlCount;
        progress.setValue(3);
    }

    // 4. Import theme from stylers.xml
    QString stylersPath = nppPath + "/config/stylers.xml";
    if (QFile::exists(stylersPath)) {
        // Simplified: just copy the theme file
        QString destDir = NppParameters::getInstance().getConfigDir();
        QString destPath = destDir + "/themes/stylers.xml";
        if (QDir(destDir + "/themes").exists() || QDir().mkpath(destDir + "/themes")) {
            if (QFile::copy(stylersPath, destPath)) {
                NppParameters::getInstance().setTheme("styler");
                ++imported;
            }
        }
        progress.setValue(4);
    }

    progress.close();

    if (imported == 0 && failed == 0)
        return ImportResult_Success;  // Nothing to import but no errors

    return (failed > 0) ? ImportResult_PartialSuccess : ImportResult_Success;
}

QStringList Application::detectNppPaths() const {
    QStringList candidates;

    // Linux Wine
    candidates << QDir::homePath() + "/.wine/drive_c/Program Files/Notepad++";
    candidates << QDir::homePath() + "/.wine/drive_c/Program Files (x86)/Notepad++";

    // Linux flatpak
    candidates << QDir::homePath() + "/.var/app/org.notepad_plus_plus.NotepadPlusPlus/data/notepad-plus-plus";

    // Linux native
    candidates << "/usr/share/notepad-plus-plus";
    candidates << "/opt/notepad-plus-plus";

    // macOS
    candidates << "/Applications/Notepad++";

    // Windows
    candidates << "C:/Program Files/Notepad++";
    candidates << "C:/Program Files (x86)/Notepad++";

    QStringList result;
    for (const QString& path : candidates) {
        if (QDir(path).exists())
            result.append(path);
    }
    return result;
}

bool Application::exportSettingsToJson(const QString& path) {
    QJsonObject root;

    // Export NppGUI
    const NppGUI& gui = NppParameters::getInstance().nppGUI();
    QJsonObject nppGui;
    nppGui["backupMode"] = gui.backupMode;
    nppGui["maxBackups"] = gui.maxBackups;
    nppGui["customBackupDir"] = gui.customBackupDir;
    nppGui["autoSaveInterval"] = gui.autoSaveInterval;
    nppGui["tabSize"] = gui.tabSize;
    nppGui["useSpaces"] = gui.useSpaces;
    nppGui["autoIndent"] = gui.autoIndent;
    nppGui["showEol"] = gui.showEol;
    nppGui["wrapMode"] = gui.wrapMode;
    root["NppGUI"] = nppGui;

    // Export recent files
    QJsonArray recentFiles;
    for (const QString& f : _recentFilesManager->files())
        recentFiles.append(f);
    root["recentFiles"] = recentFiles;

    // Export shortcuts
    QJsonArray shortcuts;
    for (const Shortcut& sc : NppParameters::getInstance().getShortcuts()) {
        QJsonObject obj;
        obj["name"] = QString::fromStdString(sc.name);
        obj["ctrl"] = sc.ctrl;
        obj["alt"] = sc.alt;
        obj["shift"] = sc.shift;
        obj["key"] = sc.key;
        shortcuts.append(obj);
    }
    root["shortcuts"] = shortcuts;

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool Application::exportSettingsToNpp(const QString& path) {
    // Export to N++ config format (XML)
    // This is the reverse of importFromNpp()
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("NotepadPlus");
    writer.writeStartElement("Config");

    // Write recent files
    writer.writeStartElement("RecentFiles");
    for (const QString& f : _recentFilesManager->files()) {
        writer.writeEmptyElement("File");
        writer.writeAttribute("path", f);
    }
    writer.writeEndElement();  // RecentFiles

    // Write shortcuts
    writer.writeStartElement("Shortcuts");
    for (const Shortcut& sc : NppParameters::getInstance().getShortcuts()) {
        writer.writeEmptyElement("Shortcut");
        writer.writeAttribute("name", QString::fromStdString(sc.name));
        writer.writeAttribute("ctrl", QString::number(sc.ctrl));
        writer.writeAttribute("alt", QString::number(sc.alt));
        writer.writeAttribute("shift", QString::number(sc.shift));
        writer.writeAttribute("key", QString::number(sc.key));
    }
    writer.writeEndElement();  // Shortcuts

    writer.writeEndElement();  // Config
    writer.writeEndElement();  // NotepadPlus
    writer.writeEndDocument();
    file.close();
    return true;
}

bool Application::loadUdlsFromNpp(const QString& nppPath) {
    if (!_udlManager) {
        _udlManager = new UdlManager(this);
    }
    QString udlDir = nppPath + "/notepad-plus-plus-translation/PowerEditor/bin/userDefineLangs";
    int count = _udlManager->loadAllUdls(udlDir);
    return count > 0;
}

bool Application::loadUdlsFromNpp() {
    QStringList paths = detectNppPaths();
    for (const QString& p : paths) {
        if (loadUdlsFromNpp(p))
            return true;
    }
    return false;
}

#include "Application.moc"

void Application::onMacroStartRecording() {
    if (_macroManager) {
        _macroManager->startRecording();
        setStatusBarText("Macro recording started...");
    }
}

void Application::onMacroStopRecording() {
    if (_macroManager) {
        _macroManager->stopRecording();
        setStatusBarText("Macro recording stopped. Enter a name to save.");
    }
}

void Application::onMacroPlaybackLast() {
    if (_macroManager) {
        int count = _macroManager->macroCount();
        if (count > 0) {
            _macroManager->playback(count - 1);
            setStatusBarText("Macro playback complete.");
        } else {
            setStatusBarText("No macros saved yet.");
        }
    }
}
