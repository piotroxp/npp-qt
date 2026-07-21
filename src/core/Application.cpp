// Application.cpp - Main application controller implementation
#include "Application.h"

#include <cstdio>
#include <unistd.h>

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QPrinter>
#include <QProcess>
#include <QPrintDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStringConverter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWindow>
#include <algorithm>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <array>
#include <QProgressDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "Buffer.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include "common/Util.h"
#include "Constants.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/CommandPaletteDialog.h"
#include "dialogs/FindInFilesDialog.h"
#include "dialogs/FindInFilesWorker.h"
#include "dialogs/IncrementalSearchDialog.h"
#include "dialogs/GoToLineDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/ShortcutMapperDialog.h"
#include "EditorCommandManager.h"
#include "EncodingDetector.h"
#include "FileManager.h"
#include "FindReplaceDialog.h"
#include "LanguageManager.h"
#include "MacroManager.h"
#include "MISC/Common/shortcut.h"
#include "Parameters.h"
#include "RecentFilesManager.h"
#include "SessionManager.h"
#include "ShortcutManager.h"
#include "SyntaxHighlighter.h"
#include "ThemeManager.h"
#include "UdlManager.h"
#include <Qsci/qsciscintilla.h>
#include "editor/ScintillaEditor.h"
#include "panels/DocumentMapPanel.h"
#include "panels/FileBrowserPanel.h"
#include "panels/FunctionListPanel.h"
#include "ui/MainWindow.h"
#include "ui/MenuBar.h"
#include "ui/StatusBar.h"
#include "ui/TabBar.h"
#include "ui/ToolBar.h"

// ============================================================================
// Singleton
// ============================================================================
// DIAG (transient): one-shot re-entry counter for the __cxa_guard_acquire
// stall. Logs to fd 2 directly (NOT qDebug) so the instrumentation cannot
// itself recurse through Qt globals. Tag on the line makes it trivial to
// grep /tmp/npp-gdb.log for "APPINST".
static void diag_inst_enter(const char* site) {
    static int n = 0;
    ++n;
    char buf[160];
    int len = std::snprintf(buf, sizeof(buf),
        "APPINST site=%s n=%d\n", site, n);
    if (len > 0) ::write(2, buf, static_cast<size_t>(len));
}
Application& Application::instance() {
    diag_inst_enter("instance()");
    static Application inst;
    return inst;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
Application::Application() : QObject(qApp) {
    _mainWindow = new MainWindow(this);
    _centralWidget = new QWidget();
    _viewStack = new QStackedWidget();

    // FIX (recursion deadlock): MainWindow's ctor used to wire Application::bufferOpened
    // via the inline app() helper, which re-entered Application::instance() while the
    // static-init guard was still held. Wire it here, after MainWindow construction
    // returns and the singleton is fully constructed.
    connect(this, &Application::bufferOpened, _mainWindow, &MainWindow::onBufferOpened);

    // FIX (cherry-picked from 337e570 on semantic-lift/wip): also forward
    // bufferClosed the same way.  Application::closeFile() and
    // closeAllFiles() (re-)emit bufferClosed; MainWindow::onBufferClosed is
    // the handler that drops the tab from the QTabWidget.  Without this
    // connect, closing a file via File→Close or Close All leaves a stale
    // empty tab in the UI (X-button-on-tab worked only because that path
    // manually calls _tabWidget->removeTab()).
    connect(this, &Application::bufferClosed, _mainWindow, &MainWindow::onBufferClosed);
}

Application::~Application() {
    // Stop auto-save timer
    if (_autoSaveTimer) {
        _autoSaveTimer->stop();
        delete _autoSaveTimer;
        _autoSaveTimer = nullptr;
    }

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

    // Delete dialogs — owned by Application (nullptr parent), not children of
    // _mainWindow, so no double-delete from Qt's cascade.
    delete _findReplaceDialog;
    delete _findInFilesDialog;
    delete _gotoLineDialog;
    delete _preferenceDialog;
    delete _aboutDialog;

    // NOTE: Do NOT delete _mainWindow directly. It is a child of Application
    // (created with 'new MainWindow(this)'). Qt's parent-child ownership means
    // _mainWindow is destroyed when Application is destroyed. Deleting it explicitly
    // would cascade-delete _menuBar, _toolBar, and _statusBarWidget — all of which
    // are Qt children of _mainWindow — leading to heap-use-after-free crashes.
    // _mainWindow = nullptr;  // unnecessary; set automatically by Qt
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
        // Register the Scintilla editor as the live text provider for saveFile().
        // _bufferToEditor is populated by openFile() and bufferActivated().
        _fileManager->setBufferTextProvider([this](BufferID buffer, QString& outText) -> bool {
            ScintillaEditor* ed = nullptr;
            auto it = _bufferToEditor.find(buffer);
            if (it != _bufferToEditor.end()) {
                ed = it->second;
            } else {
                // Fallback: scan all editors if map isn't populated yet
                for (QWidget* container : _viewContainers) {
                    auto* e = container->findChild<ScintillaEditor*>();
                    if (e) { ed = e; break; }
                }
            }
            if (!ed) return false;
            outText = ed->text();
            return true;
        });
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

        // Load plugins from config directory
        if (!_skipPlugins) {
            QString pluginDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                                + "/notepad--qt/plugins";
            QDir().mkpath(pluginDir);
            PluginManager::instance().loadPlugins(pluginDir);
        }

        _state = AppState::Ready;
        logInfo("Initialization complete");

        // Auto-save timer
        if (_options.autoSave) {
            _autoSaveTimer = new QTimer(this);
            connect(_autoSaveTimer, &QTimer::timeout, this, [this]() {
                if (_options.autoSaveCurrentOnly) {
                    BufferID buf = getActiveBuffer();
                    if (buf && isBufferModified(buf)) saveFile(buf);
                } else {
                    saveAllFiles();
                }
            });
            _autoSaveTimer->start(_options.autoSaveInterval * 60 * 1000);
        }

        // Show main window
        showMainWindow();

        // Restore window geometry from settings
        {
            QSettings s;
            QByteArray geom = s.value("window/geometry").toByteArray();
            if (!geom.isEmpty() && _mainWindow) _mainWindow->restoreGeometry(geom);
            QByteArray state = s.value("window/state").toByteArray();
            if (!state.isEmpty() && _mainWindow) _mainWindow->restoreState(state);
        }

        // Incremental search dialog — owned by MainWindow, accessed via getter.
        // NOTE: signals are wired in MainWindow::MainWindow() to avoid double-fire.

        // Clipboard encoding auto-detection on paste
        connect(QApplication::clipboard(), &QClipboard::changed, this,
            [this](QClipboard::Mode mode) {
                if (mode != QClipboard::Clipboard) return;
                const QString& text = QApplication::clipboard()->text();
                if (text.isEmpty()) return;
                // Check for high Unicode or UTF-16 surrogate pairs
                bool hasHighUnicode = false;
                for (const QChar& c : text) {
                    if (c.unicode() > 0xFFFD) {
                        hasHighUnicode = true;
                        break;
                    }
                }
                if (hasHighUnicode) {
                    qDebug() << "[clipboard] High Unicode text detected — UTF-16 likely";
                }
            });

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
    _options.indentWidth       = ini.getInt("Editor", "IndentWidth", 4);
    _options.defaultTabAsSpaces = ini.getBool("Editor", "TabAsSpaces", false);
    _options.wordWrap          = ini.getBool("Editor", "WordWrap", false);
    _options.virtualSpace      = ini.getBool("Editor", "VirtualSpace", true);
    _options.smartHome         = ini.getBool("Editor", "SmartHome", true);
    _options.autoIndent        = ini.getBool("Editor", "AutoIndent", true);
    _options.defaultEolType    = EolType::EOL_LF;
    _options.defaultEncoding    = EncodingType::UTF_8_BOM;
    _options.defaultLang        = LangType::L_TEXT;
    _options.themeProfile       = ini.get("General", "Theme", "default");
    _currentTheme               = _options.themeProfile;

    // Appearance
    _options.showLineNumbers    = ini.getBool("Margins", "ShowLineNumbers", true);
    _options.lineNumberWidth    = ini.getInt("Margins", "LineNumberWidth", 4);
    _options.showSymbols        = ini.getBool("Margins", "ShowSymbols", false);
    _options.showFolderMargin   = ini.getBool("Margins", "ShowFolderMargin", true);
    _options.symbolMarginWidth  = ini.getInt("Margins", "SymbolMarginWidth", 1);
    _options.highlightCurrentLine = ini.getBool("Margins", "HighlightCurrentLine", true);
    _options.showEdgeLine       = ini.getBool("Margins", "ShowEdgeLine", false);
    _options.edgeColumn         = ini.getInt("Margins", "EdgeColumn", 80);

    // User-chosen highlight color (Preferences → Current Line group).  Stored
    // as a QColor::name() string under "Editor".  Missing key → the default
    // baked into AppOptions (cream) takes over.
    {
        std::string colorStr = ini.get("Editor", "CurrentLineHighlightColor", "");
        QColor saved(QString::fromStdString(colorStr));
        if (saved.isValid()) _options.currentLineHighlightColor = saved;
    }

    // File Associations
    _options.fileAssociations   = ini.getStringList("General", "FileAssociations", {});

    // Shortcut Mapper
    _options.warnOnShortcutConflict = ini.getBool("ShortcutMapper", "WarnOnConflict", true);

    // Backup / Auto-Save
    _options.autoSave           = ini.getBool("Backup", "AutoSave", false);
    _options.autoSaveInterval   = ini.getInt("Backup", "AutoSaveInterval", 5);
    _options.autoSaveCurrentOnly = ini.getBool("Backup", "AutoSaveCurrentOnly", false);
    _options.autoSaveInBackground = ini.getBool("Backup", "AutoSaveInBackground", true);
    _options.backupDir          = QString::fromUtf8(ini.get("Backup", "BackupDir", "").c_str());
    _options.backupStyle        = ini.getInt("Backup", "BackupStyle", 0);
    _options.maxBackups         = ini.getInt("Backup", "MaxBackups", 10);

    return true;
}

bool Application::saveConfig(const std::string& path) {
    (void)path;  // uses default config path
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
    ini.set("Editor",  "IndentWidth",       _options.indentWidth);
    ini.set("Editor",  "TabAsSpaces",       _options.defaultTabAsSpaces);
    ini.set("Editor",  "WordWrap",          _options.wordWrap);
    ini.set("Editor",  "VirtualSpace",       _options.virtualSpace);
    ini.set("Editor",  "SmartHome",         _options.smartHome);
    ini.set("Editor",  "AutoIndent",         _options.autoIndent);

    // Appearance
    ini.set("Margins", "ShowLineNumbers",     _options.showLineNumbers);
    ini.set("Margins", "LineNumberWidth",     _options.lineNumberWidth);
    ini.set("Margins", "ShowSymbols",         _options.showSymbols);
    ini.set("Margins", "ShowFolderMargin",    _options.showFolderMargin);
    ini.set("Margins", "SymbolMarginWidth",   _options.symbolMarginWidth);
    ini.set("Margins", "HighlightCurrentLine",_options.highlightCurrentLine);
    ini.set("Margins", "ShowEdgeLine",       _options.showEdgeLine);
    ini.set("Margins", "EdgeColumn",          _options.edgeColumn);

    // Persist user-chosen highlight color as #RRGGBB.  Reapply on next launch.
    ini.set("Editor", "CurrentLineHighlightColor",
            _options.currentLineHighlightColor.name());

    // File Associations
    ini.setStringList("General", "FileAssociations", _options.fileAssociations);

    // Shortcut Mapper
    ini.set("ShortcutMapper", "WarnOnConflict", _options.warnOnShortcutConflict);

    // Backup / Auto-Save
    ini.set("Backup", "AutoSave",              _options.autoSave);
    ini.set("Backup", "AutoSaveInterval",      _options.autoSaveInterval);
    ini.set("Backup", "AutoSaveCurrentOnly",   _options.autoSaveCurrentOnly);
    ini.set("Backup", "AutoSaveInBackground", _options.autoSaveInBackground);
    ini.set("Backup", "BackupDir",            _options.backupDir.toStdString());
    ini.set("Backup", "BackupStyle",          _options.backupStyle);
    ini.set("Backup", "MaxBackups",           _options.maxBackups);

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

    // Add initial view and set it as the active editor
    _activeEditor = addView();

    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockingPanels();

    // Create dialogs — parented to nullptr so Application exclusively owns them.
    // If they were children of _mainWindow, Qt's deleteChildren() cascade would
    // double-delete them (CRASH).  Application::initializeDialogs() below must be
    // called AFTER _mainWindow is created; destruction order is handled in
    // ~Application() where we delete dialogs before _mainWindow.
    _findReplaceDialog = new FindReplaceDialog(nullptr);
    _findInFilesDialog = new FindInFilesDialog(nullptr);
    _gotoLineDialog = new GoToLineDialog(nullptr);
    _preferenceDialog = new PreferenceDialog(nullptr);
    _aboutDialog = new AboutDialog(nullptr);
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
    _statusBarWidget = new StatusBar(_mainWindow);
    _mainWindow->setStatusBar(_statusBarWidget);
}

void Application::setupDockingPanels() {
    // FileBrowserPanel, FunctionListPanel, DocumentMapPanel are already created
    // in MainWindow::createPanels() and registered via setters below.
    // We only need to create and add ClipboardHistoryPanel here.
    if (!_clipboardHistoryPanel) {
        _clipboardHistoryPanel = new ClipboardHistoryPanel(_mainWindow);
        _mainWindow->addDockWidget(Qt::RightDockWidgetArea, _clipboardHistoryPanel);
        _clipboardHistoryPanel->hide();
    }
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
    connect(this, &Application::bufferModifiedChanged, this, &Application::updateWindowTitle);

    // File browser → open file.  Use _mainWindow->openFileInTab() so that all
    // ScintillaEditor construction goes through the same guarded code path:
    //   - _tabWidget null guard
    //   - _openingFileDepth re-entrancy guard
    //   - signal disconnection during construction
    // Do NOT use openFile() here — it bypasses MainWindow guards and can cause
    // re-entrant ScintillaEditor construction during rapid double-click.
    if (_fileBrowserPanel && _mainWindow) {
        connect(_fileBrowserPanel, &FileBrowserPanel::fileDoubleClicked,
                _mainWindow, [this](const QString& path) {
            _mainWindow->openFileInTab(path);
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

    // File change monitoring — FileManager's QFileSystemWatcher emits fileSystemChanged
    if (_fileManager) {
        connect(_fileManager, &FileManager::fileSystemChanged,
                this, [this](const QString& path) {
                    onExternalFileChanged(path);
                });
    }

    // StatusBar updates — encoding/EOL on buffer switch
    connect(this, &Application::bufferActivated, this, [this](BufferID buf) {
        if (buf && _statusBarWidget) {
            auto enc = getBufferEncoding(buf);
            auto encStr = encodingToString(enc);
            QString eolStr = "Unix (LF)";
            _statusBarWidget->setEncoding(QString::fromStdString(encStr));
            _statusBarWidget->setEol(eolStr);
        }
    });

    // Loading progress → status bar
    if (_fileManager) {
        connect(_fileManager, &FileManager::loadingProgress, this, [this](int pct) {
            if (_statusBarWidget) {
                _statusBarWidget->setMessage(QString("Loading... %1%").arg(pct));
            }
        });
        connect(_fileManager, &FileManager::fileLoaded, this, [this](bool, BufferID id, const QString&) {
            // Start file monitoring so external changes trigger the reload prompt.
            if (id) {
                auto* buf = static_cast<Buffer*>(id);
                buf->startMonitoring();
            }
        });
    }

    // Cursor position → StatusBar position
    connect(this, &Application::activeEditorChanged, this, [this](ScintillaEditor* ed) {
        if (ed && _statusBarWidget) {
            connect(ed, &ScintillaEditor::cursorPositionChanged, _statusBarWidget,
                [this](int line, int col) {
                    if (_statusBarWidget) _statusBarWidget->setPosition(line, col);
                });
        }
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

    // Unload plugins
    PluginManager::instance().unloadPlugins();

    _state = AppState::Starting;
}

// ============================================================================
// View Management
// ============================================================================
ScintillaEditor* Application::addView() {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    ScintillaEditor* editor = new ScintillaEditor(container);
    layout->addWidget(editor);

    [[maybe_unused]] int viewId = _viewStack->addWidget(container);
    _viewContainers.push_back(container);
    return editor;
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
bool Application::reloadFile(BufferID buffer) {
    if (!buffer) return false;
    auto fname = getFileName(buffer);
    if (!fname || fname->empty()) return false;
    // Read file from disk
    QFile file(QString::fromStdString(*fname));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QByteArray data = file.readAll();
    file.close();
    // Update editor with fresh content
    ScintillaEditor* ed = getActiveEditor();
    if (ed) {
        ed->setText(QString::fromUtf8(data));
        ed->setModified(false);
    }
    return true;
}


bool Application::saveAllFiles() {
    return _fileManager->saveAllFiles();
}

bool Application::closeFile(BufferID buffer, bool force) {
    (void)force;
    bool ok = _fileManager->closeFile(buffer);
    if (ok) {
        // Forward FileManager's bufferClosed to anyone listening on
        // Application::bufferClosed.  Before this re-emit, the buffer
        // lifecycle signals were wired (line ~503) but no close path
        // actually fired them, so MainWindow::onBufferClosed (which is
        // the tab-removal handler — see MainWindow.cpp:1426) was dead.
        // Buffer path File→Close → onCloseFile → closeFile now finally
        // drops the tab in the UI.
        emit bufferClosed(buffer);
    }
    return ok;
}

bool Application::closeAllFiles() {
    // Capture the buffer list BEFORE delegating: FileManager::closeAllFiles
    // emits bufferClosed per-file and destroys each buffer; the per-file
    // emit fires MainWindow::onBufferClosed → removeEditorTab, which is
    // idempotent so multiple tabs are removed sequentially.
    std::vector<BufferID> ids;
    if (_fileManager) {
        const size_t n = _fileManager->getBufferCount();
        ids.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            BufferID id = _fileManager->getBufferAt(static_cast<int>(i), 0);
            if (id) ids.push_back(id);
        }
    }
    bool ok = _fileManager->closeAllFiles();
    if (ok) {
        for (BufferID id : ids) emit bufferClosed(id);
    }
    return ok;
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
        // Route through Application::closeFile so the bufferClosed
        // re-emit fires MainWindow::onBufferClosed → tab removed.
        closeFile(id);
    }
}

void Application::closeAllFilesButCurrent() {
    BufferID active = getActiveBuffer();
    if (!active || !_fileManager) return;
    size_t count = _fileManager->getBufferCount();
    std::vector<BufferID> toClose;
    for (size_t i = 0; i < count; ++i) {
        BufferID id = _fileManager->getBufferAt(static_cast<int>(i), 0);
        if (id && id != active) {
            // Check if modified and prompt user
            QString path = _fileManager->getBufferPath(id);
            if (!path.isEmpty()) {
                QFileInfo fi(path);
                QString baseName = fi.fileName();
                QMessageBox::StandardButton reply = QMessageBox::question(
                    nullptr, "Notepad--",
                    QString("Save changes to \"%1\" before closing?").arg(baseName),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Save
                );
                if (reply == QMessageBox::Cancel) {
                    continue;  // skip this buffer
                }
                if (reply == QMessageBox::Save) {
                    saveFile(id);
                }
            }
            toClose.push_back(id);
        }
    }
    for (BufferID id : toClose) {
        // Route through Application::closeFile so the bufferClosed
        // re-emit fires MainWindow::onBufferClosed → tab removed.
        closeFile(id);
    }
}

BufferID Application::newFile(const std::string& encoding) {
    (void)encoding;  // encoding determined by file manager defaults
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


BufferID Application::getBufferForPath(const std::string& path) const {
    for (int i = 0; i < getBufferCount(); ++i) {
        BufferID buf = getBufferAt(i);
        auto fname = getFileName(buf);
        if (fname && *fname == path)
            return buf;
    }
    return BUFFER_INVALID;
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

EolType Application::getBufferEol(BufferID buffer) const {
    return _fileManager->getEolFormat(buffer);
}

bool Application::setBufferEol(BufferID buffer, EolType eol) {
    return _fileManager->setEolFormat(buffer, eol);
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

void Application::setStatusBarSelection(int chars, int lines) {
    StatusBar* sb = qobject_cast<StatusBar*>(_mainWindow->statusBar());
    if (sb) sb->setSelection(chars, lines);
}

// ============================================================================
// Session
// ============================================================================
bool Application::loadSession(const std::string& path) {
    // Restore window geometry from QSettings (set by previous saveSession)
    QSettings s;
    if (_mainWindow) {
        QByteArray geom = s.value("window/geometry").toByteArray();
        if (!geom.isEmpty()) _mainWindow->restoreGeometry(geom);
        QByteArray state = s.value("window/state").toByteArray();
        if (!state.isEmpty()) _mainWindow->restoreState(state);
    }

    // Load session data
    if (!_sessionManager->loadSession(path))
        return false;

    // Restore open files, cursor, and scroll from session data
    const NppSession& session = _sessionManager->currentSession();
    BufferID activeBuffer = nullptr;

    for (int i = 0; i < session.buffers.size(); ++i) {
        const SessionBuffer& buf = session.buffers.at(i);
        BufferID id = openFile(buf.path.toStdString());
        if (!id) continue;

        // Restore cursor position
        ScintillaEditor* ed = getEditor();
        if (ed) {
            ed->setCursorPosition(buf.cursor.line, buf.cursor.column);
            // Restore scroll (firstVisibleLine via SCI_LINESCROLL)
            ed->send(SCI_SETFIRSTVISIBLELINE, buf.scrollY);
        }

        if (buf.isActive)
            activeBuffer = id;
    }

    // Restore active buffer and view
    if (activeBuffer)
        setActiveBuffer(activeBuffer);
    if (session.activeView >= 0 && session.activeView < (int)_viewContainers.size())
        switchToView(session.activeView);

    // Restore panel states
    if (session.panels.functionList && _funcListPanel) _funcListPanel->show();
    if (session.panels.documentMap && _docMapPanel) _docMapPanel->show();
    if (session.panels.fileBrowser && _fileBrowserPanel) _fileBrowserPanel->show();

    return true;
}

bool Application::saveSession(const std::string& path) {
    auto sessionPath = path.empty() ? FileHelper::getSessionFilePath() : path;

    // Build NppSession from current Application state
    NppSession session;
    session.version = 1;

    // Window geometry
    if (_mainWindow) {
        QSettings s;
        s.setValue("window/geometry", _mainWindow->saveGeometry());
        s.setValue("window/state", _mainWindow->saveState());
        session.window.x = _mainWindow->x();
        session.window.y = _mainWindow->y();
        session.window.width = _mainWindow->width();
        session.window.height = _mainWindow->height();
        session.window.maximized = (_mainWindow->windowState() & Qt::WindowMaximized) != 0;
    }

    // Capture open buffers
    int bufCount = getBufferCount();
    BufferID activeBuf = getActiveBuffer();
    for (int i = 0; i < bufCount; ++i) {
        BufferID buf = getBufferAt(i);
        if (!buf) continue;

        SessionBuffer sb;
        sb.path = _fileManager->getBufferPath(buf);

        // Resolve language name (UDL name when user-defined, otherwise standard name)
        if (Buffer* b = _fileManager->getBufferByIndex(static_cast<size_t>(i))) {
            LangType lt = b->getLangType();
            if (lt == LangType::L_USER) {
                sb.language = b->getUserDefineLangName();
            } else if (_languageManager) {
                sb.language = QString::fromStdString(_languageManager->getLanguageName(lt));
            }
        }
        sb.isActive = (buf == activeBuf);

        // Look up the editor that holds this buffer (fall back to active editor)
        ScintillaEditor* ed = nullptr;
        auto it = _bufferToEditor.find(buf);
        if (it != _bufferToEditor.end()) {
            ed = it->second;
        } else {
            ed = getActiveEditor();
        }
        if (ed) {
            int line = 0, col = 0;
            ed->getCursorPosition(&line, &col);
            sb.cursor.line = line;
            sb.cursor.column = col;
            sb.scrollY = ed->send(SCI_GETFIRSTVISIBLELINE);
        }

        session.buffers.append(sb);
    }

    session.activeBufferIndex = 0;
    session.activeView = currentView();

    // Panel visibility
    session.panels.functionList = (_funcListPanel && _funcListPanel->isVisible());
    session.panels.documentMap = (_docMapPanel && _docMapPanel->isVisible());
    session.panels.fileBrowser = (_fileBrowserPanel && _fileBrowserPanel->isVisible());

    // Inject into SessionManager and save
    _sessionManager->setCurrentSession(session);
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
    // Keep incremental search dialog in sync with the active editor
    if (auto* is = _mainWindow ? _mainWindow->incrementalSearch() : nullptr) {
        is->setEditor(editor);
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

    // Reapply theme to all editors
    for (QWidget* container : _viewContainers) {
        QList<ScintillaEditor*> editors = container->findChildren<ScintillaEditor*>();
        for (ScintillaEditor* ed : editors) {
            ed->applyTheme(QString::fromStdString(themeName));
            ed->setLanguage(ed->language());  // re-apply lexer with new colors
        }
    }
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

void Application::onExternalFileChanged(const QString& path) {
    // Find buffer(s) for this path
    for (size_t i = 0; i < _fileManager->getBufferCount(); ++i) {
        BufferID id = _fileManager->getBufferAt(static_cast<int>(i));
        if (!id) continue;
        QString bufPath = _fileManager->getBufferPath(id);
        if (bufPath.isEmpty() || bufPath != path) continue;

        // Check if buffer is modified (dirty)
        bool isDirty = _fileManager->isBufferModified(id);

        if (!isDirty) {
            // Buffer clean → auto-reload
            QString content;
            EncodingType enc = _fileManager->getEncoding(id);
            if (_fileManager->loadFile(path, content, enc)) {
                auto it = _bufferToEditor.find(id);
                ScintillaEditor* ed = (it != _bufferToEditor.end()) ? it->second : nullptr;
                if (ed) ed->setPlainText(content);
                setStatusBarText(("Reloaded: " + path).toStdString());
            }
        } else {
            // Buffer dirty → prompt
            QMessageBox::StandardButton reply = QMessageBox::question(
                _mainWindow, "File Changed",
                QString("File '%1' was modified externally.\nReload (discard changes) or Keep Current?").arg(path),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QString content;
                EncodingType enc = _fileManager->getEncoding(id);
                if (_fileManager->loadFile(path, content, enc)) {
                    auto it = _bufferToEditor.find(id);
                    ScintillaEditor* ed = (it != _bufferToEditor.end()) ? it->second : nullptr;
                    if (ed) ed->setPlainText(content);
                    setStatusBarText(("Reloaded (with changes discarded): " + path).toStdString());
                }
            } else {
                setStatusBarText(("Keeping local changes for: " + path).toStdString());
            }
        }
        break;  // only handle first matching buffer
    }
}

void Application::updateUI() {
    updateWindowTitle();
}

void Application::updateWindowTitle() {
    if (!_mainWindow) return;
    QString title = "Notepad--Qt";
    BufferID buf = getActiveBuffer();
    if (buf) {
        auto path = getFileName(buf);
        if (path && !path->empty()) {
            QString fileName = QFileInfo(QString::fromStdString(*path)).fileName();
            title = fileName + " - Notepad--Qt";
        } else {
            title = "* Untitled - Notepad--Qt";
        }
        if (isBufferModified(buf)) {
            title = "* " + title;
        }
    }
    _mainWindow->setWindowTitle(title);
}

void Application::setConfigDirectory(const std::string& dir) { _options.configDir = dir; }
void Application::setPluginDirectory(const std::string& dir) { _options.pluginDir = dir; }
void Application::setThemeProfile(const std::string& profile) { _options.themeProfile = profile; }

// ============================================================================
// Buffer lifecycle slots — connected to Application's own signals
// ============================================================================
void Application::onBufferOpened(BufferID buffer) {
    if (!buffer) return;
    qDebug() << "[App] Buffer opened:" << buffer;
    // MainWindow listens to bufferOpened and creates the tab
    Q_UNUSED(buffer);
}

void Application::onBufferActivated(BufferID buffer) {
    if (!buffer) return;
    Buffer* bufProbe = buffer;
    qDebug() << "[App] Buffer activated:" << buffer;
    qDebug() << "[PROBE-SYNTAX] App::onBufferActivated ENTER buf=" << (void*)(uintptr_t)buffer
             << "_activeEditor=" << (void*)_activeEditor
             << "bufLang=int(" << (bufProbe ? static_cast<int>(bufProbe->getLangType()) : -1) << ")";

    ScintillaEditor* ed = getActiveEditor();
    if (!ed) {
        qDebug() << "[PROBE-SYNTAX] App::onBufferActivated EARLY_RETURN_NULL_EDITOR";
        return;
    }
    qDebug() << "[PROBE-SYNTAX] App::onBufferActivated got editor=" << (void*)ed;

    // Keep _bufferToEditor in sync so saveFile() can find the right editor
    _bufferToEditor[buffer] = ed;

    Buffer* buf = buffer;
    if (!buf) return;

    // Wave 6: if this buffer is associated with a User-Defined Language, register
    // its keyword sets with LanguageManager so AutoCompletion picks them up,
    // and mark it as active so the L_USER branch in getKeywords() can find them.
    const QString udlName = buf->getUserDefineLangName();
    if (!udlName.isEmpty() && _udlManager) {
        if (const UdlDefinition* def = _udlManager->getUdl(udlName)) {
            std::array<QString, 9> sets = {
                def->keywords.words0,
                def->keywords.words1,
                def->keywords.words2,
                def->keywords.words3,
                def->keywords.words4,
                def->keywords.words5,
                def->keywords.words6,
                def->keywords.words7,
                def->keywords.words8
            };
            _languageManager->registerUdlKeywords(udlName, sets);
            _languageManager->setActiveUdlName(udlName);
        }
    } else {
        // Clear active UDL so L_USER buffers without a known UDL get no keywords
        _languageManager->setActiveUdlName(QString());
    }

    ed->setLanguage(buf->getLangType());
    ed->setEncoding(buf->getEncoding());
    ed->setEolType(buf->getEolFormat());

    // Partial load indicator in status bar — guard against null _statusBarWidget
    if (_statusBarWidget) {
        if (buf->isPartialLoad()) {
            _statusBarWidget->setMessage(
                QString("Partial load: %1 / %2 MB")
                    .arg(static_cast<double>(buf->getDocumentLength()) / 1e6, 0, 'f', 1)
                    .arg(static_cast<double>(buf->totalFileSize()) / 1e6, 0, 'f', 1));
        } else {
            _statusBarWidget->clearMessage();
        }
    }

    updateUI();
}

void Application::onBufferClosed(BufferID buffer) {
    if (!buffer) return;
    qDebug() << "[App] Buffer closed:" << buffer;
    // Nothing to do here — MainWindow removes the tab
    Q_UNUSED(buffer);
}

void Application::onMenuCommand(const QString& cmd) {
    // File
    if (cmd == "file.new") { onNewFile(); }
    else if (cmd == "file.open") { onOpenFile(); }
    else if (cmd == "file.save") { onSaveFile(); }
    else if (cmd == "file.saveAs") { onSaveFileAs(); }
    else if (cmd == "file.saveAll") { onSaveAll(); }
    else if (cmd == "file.close") { onCloseFile(); }
    else if (cmd == "file.closeAll") { closeAllFiles(); }
    else if (cmd == "file.closeAll_BUT_THIS") { closeAllFilesButCurrent(); }
    else if (cmd == "file.saveAsCopy") { onSaveAsCopy(); }
    else if (cmd == "file.exit") { onExit(); }
    else if (cmd == "file.print") { onPrint(); }
    // Edit
    else if (cmd == "edit.undo") { onUndo(); }
    else if (cmd == "edit.redo") { onRedo(); }
    else if (cmd == "edit.cut") { onCut(); }
    else if (cmd == "edit.copy") { onCopy(); }
    else if (cmd == "edit.paste") { onPaste(); }
    else if (cmd == "edit.delete") { onDelete(); }
    else if (cmd == "edit.selectAll") { onSelectAll(); }
    else if (cmd == "edit.find") { onFind(); }
    else if (cmd == "edit.replace") { onReplace(); }
    else if (cmd == "edit.goto") { onGotoLine(); }
    else if (cmd == "edit.copyToClipboard") { onCopyToNamedClipboard(); }
    else if (cmd == "edit.moveToClipboard") { onMoveToNamedClipboard(); }
    else if (cmd == "edit.pasteFromClipboard") { onPasteFromNamedClipboard(); }
    // Search
    else if (cmd == "search.findNext") { onFindNext(); }
    else if (cmd == "search.findPrev") { onFindPrev(); }
    else if (cmd == "search.count") { onCount(); }
    else if (cmd == "search.markAll") { onMarkAll(); }
    else if (cmd == "search.findInFiles") { onFindInFiles(); }
    else if (cmd == "search.replaceInFiles") { onReplaceInFiles(); }
    else if (cmd == "search.findInProjects") { onFindInProjects(); }
    else if (cmd == "search.replaceInProjects") { onReplaceInProjects(); }
    else if (cmd == "search.incremental") {
        if (auto* is = _mainWindow ? _mainWindow->incrementalSearch() : nullptr) {
            is->setEditor(_activeEditor);
            is->showAtTop();
        }
    }
    // Bookmarks
    else if (cmd == "bookmark.toggle") {
        if (_activeEditor) _activeEditor->toggleBookmark(_activeEditor->currentLine());
    }
    else if (cmd == "bookmark.next") {
        if (_activeEditor) _activeEditor->nextBookmark();
    }
    else if (cmd == "bookmark.prev") {
        if (_activeEditor) _activeEditor->prevBookmark();
    }
    else if (cmd == "bookmark.clear") {
        if (_activeEditor) _activeEditor->clearBookmarks();
    }
    // View
    else if (cmd == "view.fullScreen") { onToggleFullScreen(); }
    else if (cmd == "view.distractionFree") { onToggleDistractionFree(); }
    else if (cmd == "view.zoomIn") { if (_activeEditor) _activeEditor->zoomIn(); }
    else if (cmd == "view.zoomOut") { if (_activeEditor) _activeEditor->zoomOut(); }
    else if (cmd == "view.zoomRestore") { if (_activeEditor) _activeEditor->zoomReset(); }
    else if (cmd == "file.reload") { onReloadFile(); }
    else if (cmd == "view.darkMode" || cmd == "view.toggleDarkMode") {
        QString nextTheme = _currentTheme == "dark" ? "light" : "dark";
        onThemeChanged(nextTheme.toStdString());
    }
    else if (cmd == "view.showTabBar") {
        _options.showTabBar = !_options.showTabBar;
        emit onToggleTabBar();
    }
    else if (cmd == "view.showStatusBar") {
        _options.showStatusBar = !_options.showStatusBar;
        emit onToggleStatusBar();
    }
    else if (cmd == "view.showToolBar") {
        _options.showToolBar = !_options.showToolBar;
        emit onToggleToolBar();
    }
    else if (cmd == "view.foldAll") { if (_activeEditor) _activeEditor->foldAll(); }
    else if (cmd == "view.unfoldAll") { if (_activeEditor) _activeEditor->unfoldAll(); }
    else if (cmd == "view.toggleFold") { if (_activeEditor) _activeEditor->toggleFold(_activeEditor->currentLine()); }
    else if (cmd == "view.toggleVirtualSpace") {
        if (_activeEditor) {
            // SC_VS_USERACCESSIBLE = 1, SC_VS_NONE = 0
            int current = _activeEditor->qsciEditor()->SendScintilla(QsciScintilla::SCI_GETVIRTUALSPACEOPTIONS);
            int next = (current == 1) ? 0 : 1;
            _activeEditor->qsciEditor()->SendScintilla(QsciScintilla::SCI_SETVIRTUALSPACEOPTIONS, next);
        }
    }
    // Rectangle selection
    else if (cmd == "edit.rectSelect") {
        if (_activeEditor) _activeEditor->setRectangularSelectionMode(true);
    }
    else if (cmd == "edit.normalSelect") {
        if (_activeEditor) _activeEditor->setRectangularSelectionMode(false);
    }
    // Encoding
    else if (cmd == "encoding.utf8") { onConvertEncoding(EncodingType::UTF_8); }
    else if (cmd == "encoding.utf8bom") { onConvertEncoding(EncodingType::UTF_8_BOM); }
    else if (cmd == "encoding.utf16le") { onConvertEncoding(EncodingType::UTF_16_LE); }
    else if (cmd == "encoding.utf16be") { onConvertEncoding(EncodingType::UTF_16_BE); }
    else if (cmd.startsWith("encoding.charset.")) {
        QString charset = cmd.mid(17);
        onConvertToCharset(charset);
    }
    // EOL conversion
    else if (cmd == "eol.CRLF") { onEolConversion("eol:CRLF"); }
    else if (cmd == "eol.LF") { onEolConversion("eol:LF"); }
    else if (cmd == "eol.CR") { onEolConversion("eol:CR"); }
    else if (cmd.startsWith("eol:")) { onEolConversion(cmd); }
    // Language
    else if (cmd.startsWith("language.")) {
        QString langName = cmd.mid(9);
        LangType lt = LanguageManager::mapStringToLang(langName);
        if (lt != LangType::L_TEXT || langName == "normal_text")
            onSetLanguage(lt);
    }
    // Settings
    else if (cmd == "settings.preferences") { onShowPreferences(); }
    else if (cmd == "settings.shortcutMapper") { onShowShortcutMapper(); }
    else if (cmd == "settings.commandPalette") { onShowCommandPalette(); }
    // Help
    else if (cmd == "help.about") { onShowAbout(); }
    // Macro
    else if (cmd == "macro.record") { onMacroStartRecording(); }
    else if (cmd == "macro.stop") { onMacroStopRecording(); }
    else if (cmd == "macro.playback") { onMacroPlaybackLast(); }
    else if (cmd == "macro.save") {
        QString path = QFileDialog::getSaveFileName(nullptr, "Save Macro", "", "Macro Files (*.json)");
        if (!path.isEmpty()) {
            if (!path.endsWith(".json")) path += ".json";
            MacroManager::instance().saveMacro("Macro", path);
        }
    }
    else if (cmd == "macro.load") {
        QString path = QFileDialog::getOpenFileName(nullptr, "Load Macro", "", "Macro Files (*.json)");
        if (!path.isEmpty()) {
            if (MacroManager::instance().loadMacro(path))
                qDebug() << "[macro] Loaded from" << path;
            else
                qDebug() << "[macro] Failed to load" << path;
        }
    }
    // Column selection
    else if (cmd == "edit.columnSelect") {
        if (auto* ed = getActiveEditor()) ed->setColumnSelectionMode(true);
    }
    else {
        qDebug() << "[onMenuCommand] Unknown command:" << cmd;
    }
    updateUI();
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

void Application::onSaveAsCopy() {
    qDebug() << "[App] Save as copy";
    BufferID buf = getActiveBuffer();
    if (!buf) return;
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    // Default directory = current file's directory
    QString currentPath = _fileManager->getBufferPath(buf);
    QString defaultDir;
    if (!currentPath.isEmpty()) {
        QFileInfo fi(currentPath);
        defaultDir = fi.absolutePath();
    }
    if (defaultDir.isEmpty())
        defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString filePath = QFileDialog::getSaveFileName(
        nullptr, "Save As Copy", defaultDir,
        "All Files (*);;Text Files (*.txt);;C/C++ (*.cpp *.h)"
    );
    if (filePath.isEmpty()) return;

    // Read current content from editor
    QString text = ed->text();

    // Write to new path with same encoding via FileManager
    EncodingType enc = getBufferEncoding(buf);
    EolType eol = getBufferEol(buf);
    if (_fileManager->saveFile(filePath, text, enc, eol)) {
        addToRecentFiles(filePath.toStdString());
        setStatusBarText(("Saved copy: " + filePath).toStdString().c_str());
    } else {
        QMessageBox::warning(nullptr, "Save As Copy",
            QString("Could not write to:\n%1").arg(filePath));
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

void Application::onReloadFile() {
    BufferID buf = getActiveBuffer();
    if (!buf) return;
    auto path = getFileName(buf);
    if (!path) return;
    if (isBufferModified(buf)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr, "Reload File",
            "This file has unsaved changes. Reload anyway?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) return;
    }
    QString filePath = QString::fromStdString(*path);
    closeFile(buf);
    ScintillaEditor* ed = getActiveEditor();
    if (ed) {
        BufferID newBuf = openFile(filePath.toStdString());
        if (newBuf) {
            setActiveBuffer(newBuf);
            ed->setPlainText(QString::fromStdString(getBufferText(newBuf)));
        }
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

void Application::onPrint() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, nullptr);
    if (dialog.exec() != QDialog::Accepted) return;
    ed->print(&printer);
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

void Application::onCopyToNamedClipboard() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;
    bool ok = false;
    QString name = QInputDialog::getText(nullptr, "Copy to Named Clipboard",
        "Clipboard name:", QLineEdit::Normal, QString(), &ok);
    if (!ok || name.isEmpty()) return;
    _namedClips[name] = ed->selectedText();
    setStatusBarText(("Copied to: " + name).toStdString().c_str());
}

void Application::onMoveToNamedClipboard() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;
    bool ok = false;
    QString name = QInputDialog::getText(nullptr, "Move to Named Clipboard",
        "Clipboard name:", QLineEdit::Normal, QString(), &ok);
    if (!ok || name.isEmpty()) return;
    _namedClips[name] = ed->selectedText();
    ed->send(SCI_CLEAR);
    setStatusBarText(("Moved to: " + name).toStdString().c_str());
}

void Application::onPasteFromNamedClipboard() {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;
    // Show list of available named clips
    QStringList keys = _namedClips.keys();
    if (keys.isEmpty()) {
        QMessageBox::information(nullptr, "Named Clipboard", "No named clips stored yet.");
        return;
    }
    bool ok = false;
    QString name = QInputDialog::getItem(nullptr, "Paste from Named Clipboard",
        "Select clipboard:", keys, 0, false, &ok);
    if (!ok || name.isEmpty()) return;
    const QString& text = _namedClips[name];
    const QByteArray textUtf8 = text.toUtf8();  // store — dangling pointer
    ed->send(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(textUtf8.constData()));
    setStatusBarText(("Pasted from: " + name).toStdString().c_str());
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
    _findReplaceDialog->show();
}

void Application::onReplace() {
    _findReplaceDialog->show();
}

void Application::onReplaceAll() {
    if (_findReplaceDialog) {
        _findReplaceDialog->replaceAll();
    }
}

void Application::onPurge() {
    if (_findReplaceDialog) {
        _findReplaceDialog->purgeMarkedLines();
    }
}

void Application::onBookmarkAll() {
    if (_findReplaceDialog) {
        _findReplaceDialog->bookmarkAllMatches();
    }
}

void Application::onGotoLine() {
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
    // Pre-fill search text from Find/Replace dialog if available
    if (_findReplaceDialog && !_findReplaceDialog->lastSearchText().isEmpty()) {
        _findInFilesDialog->setSearchText(_findReplaceDialog->lastSearchText());
    }

    // Default directory: current file's directory, or home
    BufferID buf = getActiveBuffer();
    if (buf) {
        QString bufPath = _fileManager->getBufferPath(buf);
        if (!bufPath.isEmpty()) {
            QFileInfo info(bufPath);
            _findInFilesDialog->setDirectory(info.dir().absolutePath());
        }
    }

    _findInFilesDialog->show();
    _findInFilesDialog->raise();
    _findInFilesDialog->activateWindow();
}

void Application::onReplaceInFiles() {
    QMessageBox::information(nullptr, "Project Search",
        "Replace in Files — working in progress");
}

void Application::onFindInProjects() {
    QMessageBox::information(nullptr, "Project Search",
        "Find in Projects — working in progress");
}

void Application::onReplaceInProjects() {
    QMessageBox::information(nullptr, "Project Search",
        "Replace in Projects — working in progress");
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

void Application::onEolConversion(const QString& eolCmd) {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;

    BufferID buffer = getActiveBuffer();
    if (!buffer) return;

    QString text = ed->text();
    if (text.isEmpty()) return;

    if (eolCmd == "eol:CRLF") {
        text.replace('\n', "\r\n");
        setBufferEol(buffer, EolType::EOL_CRLF);
        setStatusBarEol("Windows (CRLF)");
    } else if (eolCmd == "eol:LF") {
        text.replace("\r\n", "\n").replace('\r', '\n');
        setBufferEol(buffer, EolType::EOL_LF);
        setStatusBarEol("Unix (LF)");
    } else if (eolCmd == "eol:CR") {
        text.replace("\r\n", "\r").replace('\n', '\r');
        setBufferEol(buffer, EolType::EOL_CR);
        setStatusBarEol("Mac (CR)");
    }

    ed->setText(text);
    ed->setModified(true);
}

void Application::onConvertToCharset(const QString& charsetName) {
    ScintillaEditor* ed = getActiveEditor();
    if (!ed) return;
    QString text = ed->toPlainText();
    // Convert to UTF-8 bytes, treating charset as a rough encoding hint
    Q_UNUSED(charsetName);
    ed->setPlainText(text);
    setStatusBarEncoding(charsetName.toStdString());
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

void Application::onConvertEncoding(EncodingType enc) {
    ScintillaEditor* ed = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!ed) return;
    BufferID buf = getActiveBuffer();
    if (buf == BUFFER_INVALID) return;

    QString path = _fileManager->getBufferPath(buf);
    if (path.isEmpty()) {
        // Untitled buffer: just change the encoding and update status
        _fileManager->setEncoding(buf, enc);
        setStatusBarText(QString("Encoding set to %1").arg(encodingToString(enc).c_str()).toStdString());
        return;
    }

    // Save to a temp file with the current encoding, then re-save with the target encoding
    // We use the editor text as the source of truth
    QString text = ed->text();
    if (text.isEmpty()) return;

    // Save to a .tmp file with target encoding first, then rename
    QString tmpPath = path + ".conv.tmp";
    if (_fileManager->saveFile(tmpPath, text, enc, getBufferEol(buf))) {
        // Move temp over original
        QFile::remove(path);
        QFile::rename(tmpPath, path);
        // Reload
        BufferID newBuf = _fileManager->openFile(path);
        if (newBuf != BUFFER_INVALID) {
            setActiveBuffer(newBuf);
        }
        setStatusBarText(QString("Converted to %1").arg(encodingToString(enc).c_str()).toStdString());
    }
}

void Application::cloneToOtherView(BufferID buffer) {
    if (buffer == BUFFER_INVALID) return;
    if (!_mainWindow) return;
    // Open the same file in the other view
    QString path = _fileManager->getBufferPath(buffer);
    if (path.isEmpty()) return;
    // Switch to the other view
    _activeViewId = (_activeViewId == 0) ? 1 : 0;
    BufferID cloned = _fileManager->openFile(path);
    if (cloned != BUFFER_INVALID) {
        setActiveBuffer(cloned);
    }
}

void Application::openFolderAsWorkspace(const std::string& dir) {
    if (dir.empty()) return;
    // Set the folder as the workspace root for the file browser
    QString folder = QString::fromStdString(dir);
    if (_fileBrowserPanel) {
        _fileBrowserPanel->setRootDirectory(folder);
        _fileBrowserPanel->refresh();
    }
    setStatusBarText(("Opened workspace: " + dir).c_str());
}

void Application::onRun() {
    ScintillaEditor* ed = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!ed) return;
    // Determine the run command based on file type
    BufferID buf = getActiveBuffer();
    QString path = _fileManager->getBufferPath(buf);
    if (path.isEmpty()) return;

    // Get file extension
    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();
    QString cmd;
    bool needSave = false;

    // Map extension to run command
    if (ext == "py") {
        cmd = "python3 \"" + path + "\"";
    } else if (ext == "js") {
        cmd = "node \"" + path + "\"";
    } else if (ext == "sh" || ext == "bash") {
        cmd = "bash \"" + path + "\"";
    } else if (ext == "rb") {
        cmd = "ruby \"" + path + "\"";
    } else if (ext == "pl") {
        cmd = "perl \"" + path + "\"";
    } else if (ext == "php") {
        cmd = "php \"" + path + "\"";
    } else if (ext == "go") {
        cmd = "go run \"" + path + "\"";
    } else if (ext == "rs") {
        cmd = "cargo run --manifest-path \"" + fi.dir().filePath("Cargo.toml") + "\"";
    } else if (ext == "html" || ext == "htm") {
        cmd = "xdg-open \"" + path + "\"";
    } else if (ext == "c" || ext == "cpp" || ext == "cc") {
        // Try to compile and run
        QString base = fi.dir().filePath(fi.baseName());
        cmd = "(gcc \"" + path + "\" -o \"" + base + "\" && \"" + base + "\") 2>&1";
        needSave = true;
    } else if (ext == "java") {
        cmd = "cd \"" + fi.dir().path() + "\" && javac " + fi.fileName() + " && java " + fi.baseName();
        needSave = true;
    }

    if (cmd.isEmpty()) {
        QMessageBox::information(_mainWindow, "Run", "No run command configured for ." + ext + " files.");
        return;
    }

    // Save if modified
    if (needSave && isBufferModified(buf)) {
        saveFile(buf);
    }

    setStatusBarText(("Running: " + cmd).toStdString().c_str());
    QProcess* proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, proc](int exitCode, QProcess::ExitStatus) {
        QString output = QString::fromLocal8Bit(proc->readAllStandardOutput());
        QString err    = QString::fromLocal8Bit(proc->readAllStandardError());
        if (!output.isEmpty() || !err.isEmpty()) {
            qDebug() << "[Run]" << output;
            if (!err.isEmpty()) qWarning() << "[Run stderr]" << err;
        }
        setStatusBarText(("Exit code: " + QString::number(exitCode)).toStdString().c_str());
        proc->deleteLater();
    });
    proc->setWorkingDirectory(fi.dir().path());
    proc->start("/bin/sh", {"-c", cmd});
}

void Application::moveToSubView(BufferID buffer) {
    if (buffer == BUFFER_INVALID) return;
    if (!_mainWindow) return;
    QString path = _fileManager->getBufferPath(buffer);
    if (path.isEmpty()) return;
    closeFile(buffer, false);
    // Switch view
    _activeViewId = (_activeViewId == 0) ? 1 : 0;
    BufferID moved = openFile(path.toStdString());
    if (moved != BUFFER_INVALID) {
        setActiveBuffer(moved);
    }
}

void Application::onMacroRecord() {
    onMacroStartRecording();
}

void Application::onMacroStop() {
    onMacroStopRecording();
}

void Application::onMacroPlayback() {
    onMacroPlaybackLast();
}

void Application::onMacroSave() {
    if (!_macroManager) return;
    // Prompt for name
    bool ok;
    QString name = QInputDialog::getText(_mainWindow, "Save Macro", "Macro name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    // Save to standard location
    QString macroDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(macroDir + "/macros").exists()) QDir().mkpath(macroDir + "/macros");
    QString filePath = macroDir + "/macros/" + name + ".json";
    _macroManager->saveMacro(name, filePath);
    setStatusBarText(("Saved macro: " + name).toStdString().c_str());
}

void Application::onManageUserLanguages() {
    if (!_udlManager) {
        _udlManager = new UdlManager(this);
    }
    // Show UDL manager dialog
    auto* dlg = new QDialog(_mainWindow);
    dlg->setWindowTitle("User Defined Languages");
    dlg->resize(600, 400);
    QVBoxLayout* lay = new QVBoxLayout(dlg);
    QLabel* info = new QLabel("User defined languages are managed through the Preferences dialog.", dlg);
    lay->addWidget(info);
    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok, dlg);
    lay->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void Application::onToggleFileBrowser() {
    if (_fileBrowserPanel) {
        if (_fileBrowserPanel->isVisible()) {
            _fileBrowserPanel->hide();
        } else {
            _fileBrowserPanel->show();
        }
    } else {
        // Create on demand if never shown
        qDebug() << "[Application] File browser panel not set up";
    }
}

void Application::onSetEol(EolType format) {
    ScintillaEditor* ed = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!ed) return;
    BufferID buf = getActiveBuffer();
    if (buf == BUFFER_INVALID) return;
    // Convert existing line endings to the chosen format
    QString text = ed->text();
    QString converted;
    switch (format) {
        case EolType::EOL_CRLF:
            converted = text.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
            break;
        case EolType::EOL_LF:
            converted = text.replace("\r\n", "\n").replace("\r", "\n");
            break;
        case EolType::EOL_CR:
            converted = text.replace("\r\n", "\r").replace("\n", "\r");
            break;
        default:
            converted = text;
    }
    // Apply the converted text
    ed->setText(converted);
    setBufferEol(buf, format);
}

void Application::toggleDocMap() {
    if (_docMapPanel) {
        if (_docMapPanel->isVisible()) {
            _docMapPanel->hide();
        } else {
            _docMapPanel->show();
        }
    }
}

void Application::onToggleDocMap() {
    toggleDocMap();
}

void Application::onToggleFunctionList() {
    if (_funcListPanel) {
        if (_funcListPanel->isVisible()) {
            _funcListPanel->hide();
        } else {
            _funcListPanel->show();
            _funcListPanel->refresh();
        }
    }
}

// ============================================================================
// Encoding helpers
// ============================================================================
std::string Application::encodingToString(EncodingType enc) const {
    switch (enc) {
        case EncodingType::UTF_8:       return "UTF-8";
        case EncodingType::UTF_8_BOM:  return "UTF-8 BOM";
        case EncodingType::UTF_16_LE:  return "UTF-16 LE";
        case EncodingType::UTF_16_BE:  return "UTF-16 BE";
        case EncodingType::OEM:         return "OEM";
        case EncodingType::ANSI:
        default:                        return "ANSI";
    }
}

// ============================================================================
// Settings command slots
// ============================================================================
void Application::onShowPreferences() {
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

void Application::onMinimizeToTray() {
    if (_mainWindow) {
        _mainWindow->hide();
        qDebug() << "[App] Minimized to tray";
    }
}

void Application::onSwitchToOther() {
    if (!_mainWindow) return;
    // Switch focus between main editor and secondary view
    if (auto* ed = getActiveEditor()) {
        ed->setFocus();
    }
}

void Application::onToolbarCustomize() {
    qDebug() << "[App] Toolbar customize requested";
    if (_mainWindow) {
        QMessageBox::information(_mainWindow, "Toolbar Customize",
            "Toolbar customization is not yet implemented. "
            "You can edit shortcuts via Settings > Shortcut Mapper.");
    }
}

// ============================================================================
// Help command slots
// ============================================================================
void Application::onShowAbout() {
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
            (void)r;  // results consumed by FindInFilesPanel
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
                            ShortcutDef sc;
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
    for (const ShortcutDef& sc : NppParameters::getInstance().getShortcuts()) {
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
    for (const ShortcutDef& sc : NppParameters::getInstance().getShortcuts()) {
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

void Application::playbackMacroStep(int message, uintptr_t wParam, uintptr_t lParam,
                                    const char* sParam, int macroType) {
    ScintillaEditor* ed = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!ed) return;

    QsciScintilla* sci = ed->qsciEditor();
    if (!sci) return;

    // Replay the Scintilla message
    auto typeIndex = static_cast<RecordedMacroStep::MacroTypeIndex>(macroType);
    switch (typeIndex) {
        case RecordedMacroStep::mtUseSParameter: {
            QString s = QString::fromUtf8(sParam);
            const QByteArray sUtf8 = s.toUtf8();  // store — dangling pointer otherwise
            sci->SendScintilla(message, wParam,
                reinterpret_cast<sptr_t>(sUtf8.constData()));
            break;
        }
        case RecordedMacroStep::mtUseLParameter:
        case RecordedMacroStep::mtMenuCommand:
        case RecordedMacroStep::mtSavedSnR:
        default:
            sci->SendScintilla(message, wParam, lParam);
            break;
    }
}

// === Stubs ===
void Application::onMoveToSubView() {
    if (_mainWindow) {
        BufferID buf = getActiveBuffer();
        moveToSubView(buf);
    }
}

void Application::onActivatePane(int paneIndex) {
    if (!_mainWindow) return;
    switch (paneIndex) {
        case 0: {
            // Switch to main editor pane — focus the active editor
            if (auto* ed = getActiveEditor()) {
                ed->setFocus();
            }
            break;
        }
        default:
            qDebug() << "[App] onActivatePane: unknown pane" << paneIndex;
            break;
    }
}

void Application::onToggleWordWrap() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (editor) {
        editor->setWrapMode(!editor->wrapMode());
    }
}

void Application::onShowAllCharacters() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    editor->setWhitespaceVisibility(true);
}

void Application::toggleClipboardHistory() {
    if (_clipboardHistoryPanel) {
        if (_clipboardHistoryPanel->isVisible()) {
            _clipboardHistoryPanel->hide();
        } else {
            _clipboardHistoryPanel->show();
        }
    }
}

void Application::onToggleClipboardHistory() {
    toggleClipboardHistory();
}

void Application::onCloneToOtherView() {
    if (_mainWindow) {
        BufferID buf = getActiveBuffer();
        cloneToOtherView(buf);
    }
}

void Application::onZoomOut() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (editor) editor->zoomOut();
}

void Application::onToggleEolVisibility() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    editor->setEolVisibility(true);
}

void Application::onToggleShowSymbol() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    // Toggle indicator display: SCI_GETINDICATORCURRENT -> SCI_INDICATORFILLRANGE / CLEAR
    // Toggle all indicators via view menu proxy
    qDebug() << "[App] Toggle show symbol";
}

void Application::onZoomRestore() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (editor) editor->zoomReset();
}

void Application::onZoomIn() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (editor) editor->zoomIn();
}

void Application::onOpenContainingFolder() {
    if (!_mainWindow) return;
    BufferID buf = getActiveBuffer();
    if (buf == BUFFER_INVALID) return;
    QString path = _fileManager->getBufferPath(buf);
    if (path.isEmpty()) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
}

void Application::onJoinLines() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < lines.size(); ++i) lines[i] = lines[i].trimmed();
    QString joined = lines.join(" ");
    const QByteArray joinedUtf8 = joined.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(joinedUtf8.constData()));
}

void Application::onSortDescending() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    lines.sort(Qt::CaseInsensitive);
    std::reverse(lines.begin(), lines.end());
    QString result = lines.join("\n");
    const QByteArray resultUtf8 = result.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(resultUtf8.constData()));
}

void Application::onDeleteFile() {
    BufferID buf = getActiveBuffer();
    if (buf == BUFFER_INVALID) return;
    QString path = _fileManager->getBufferPath(buf);
    if (path.isEmpty()) return;
    int ret = QMessageBox::warning(_mainWindow, "Delete File",
        "Are you sure you want to delete the file?\n" + QFileInfo(path).fileName(),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;
    if (QFile::remove(path)) {
        closeFile(buf, true);
    }
}

void Application::onSortIntDescending() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    QList<int> nums;
    for (const QString& l : lines) {
        bool ok;
        nums.append(l.toInt(&ok));
        if (!ok) return; // not all integers
    }
    std::sort(nums.begin(), nums.end(), std::greater<int>());
    QStringList result;
    for (int n : nums) result.append(QString::number(n));
    QString out = result.join("\n");
    const QByteArray outUtf8 = out.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(outUtf8.constData()));
}

void Application::onUnmarkAll() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    editor->clearBookmarks();
}

void Application::onGoToNextMark() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    editor->nextBookmark();
}

void Application::onGoToPrevMark() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    editor->prevBookmark();
}

void Application::onTrimLeading() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    for (int i = 0; i < lines.size(); ++i) lines[i] = lines[i].trimmed();
    QString result = lines.join("\n");
    const QByteArray resultUtf8 = result.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(resultUtf8.constData()));
}

void Application::onRenameFile() {
    BufferID buf = getActiveBuffer();
    if (buf == BUFFER_INVALID) return;
    QString oldPath = _fileManager->getBufferPath(buf);
    if (oldPath.isEmpty()) return;
    QFileInfo fi(oldPath);
    bool ok;
    QString newName = QInputDialog::getText(_mainWindow, "Rename File", "New name:",
        QLineEdit::Normal, fi.fileName(), &ok);
    if (!ok || newName.isEmpty() || newName == fi.fileName()) return;
    QString newPath = fi.absolutePath() + "/" + newName;
    if (QFile::rename(oldPath, newPath)) {
        _fileManager->setBufferPath(buf, newPath);
    }
}

void Application::onSortReverse() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    std::reverse(lines.begin(), lines.end());
    QString result = lines.join("\n");
    const QByteArray resultUtf8 = result.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(resultUtf8.constData()));
}

void Application::onDeleteLine() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    int curLine = editor->currentLine();
    int startPos = editor->send(SCI_POSITIONFROMLINE, curLine);
    int endPos = editor->send(SCI_GETLINEENDPOSITION, curLine);
    // Include the line ending if not the last line
    if (curLine + 1 < editor->lineCount()) {
        endPos = editor->send(SCI_GETLINEENDPOSITION, curLine + 1);
    }
    editor->send(SCI_SETSEL, startPos, endPos);
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(""));
}

void Application::onCloseAllButCurrent() {
    closeAllBuffersExcept(getActiveBuffer());
}

void Application::onSortAscending() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    lines.sort(Qt::CaseInsensitive);
    QString result = lines.join("\n");
    const QByteArray resultUtf8 = result.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(resultUtf8.constData()));
}

void Application::onSortIntAscending() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    QList<int> nums;
    for (const QString& l : lines) {
        bool ok;
        nums.append(l.toInt(&ok));
        if (!ok) return;
    }
    std::sort(nums.begin(), nums.end());
    QStringList result;
    for (int n : nums) result.append(QString::number(n));
    QString out = result.join("\n");
    const QByteArray outUtf8 = out.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(outUtf8.constData()));
}

void Application::onTrimTrailing() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QStringList lines = text.split('\n');
    for (int i = 0; i < lines.size(); ++i) lines[i] = lines[i].trimmed();
    QString result = lines.join("\n");
    const QByteArray resultUtf8 = result.toUtf8();  // store — dangling pointer otherwise
    editor->qsciEditor()->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
        reinterpret_cast<sptr_t>(resultUtf8.constData()));
}

void Application::onSearchOnInternet() {
    ScintillaEditor* editor = _mainWindow ? _mainWindow->currentEditor() : nullptr;
    if (!editor) return;
    QString text = editor->selectedText();
    if (text.isEmpty()) return;
    QString query = QUrl::toPercentEncoding(text);
    QDesktopServices::openUrl(QUrl("https://www.google.com/search?q=" + query));
}

// Forwarding stubs for methods called by NppBigSwitch but implemented as on*()
// in Application.cpp. These bridge the naming convention.
void Application::zoomIn()    { onZoomIn(); }
void Application::zoomOut()   { onZoomOut(); }
void Application::zoomRestore() { onZoomRestore(); }
void Application::toggleWordWrap() { onToggleWordWrap(); }
void Application::toggleEolVisibility() { onToggleEolVisibility(); }
void Application::toggleFunctionList() { onToggleFunctionList(); }
void Application::toggleFileBrowser() { onToggleFileBrowser(); }
