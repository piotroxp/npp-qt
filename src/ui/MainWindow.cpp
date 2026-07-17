// MainWindow.cpp - Main application window implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "MainWindow.h"
#include "TabBar.h"
#include "StatusBar.h"
#include "ToolBar.h"
#include "MenuBar.h"
#include "../editor/ScintillaEditor.h"
#include "../core/Application.h"
#include "../core/FileManager.h"
#include "../core/LanguageManager.h"
#include "../dialogs/FindReplaceDialog.h"
#include "../dialogs/IncrementalSearchDialog.h"
#include "../dialogs/GoToLineDialog.h"
#include "../dialogs/PreferenceDialog.h"
#include "../core/ShortcutManager.h"
#include "../core/EditorCommandManager.h"
#include "../core/MacroManager.h"
#include "../common/DpiManager.h"
#include <Qsci/qsciscintilla.h>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QCloseEvent>
#include <QFileInfo>
#include <QSettings>
#include <QShortcut>
#include <QAction>
#include <QKeySequence>
#include <QStyle>
#include <Qsci/qscilexercustom.h>
#include "../panels/DocumentMapPanel.h"
#include "../panels/FileBrowserPanel.h"
#include "../panels/FunctionListPanel.h"
#include "../core/Buffer.h"
#include "../dialogs/FileReloadDialog.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

MainWindow* MainWindow::_instance = nullptr;

MainWindow::MainWindow(Application* app) : QMainWindow(nullptr), _app(app)
{
    fprintf(stderr, "DEBUG: MainWindow ctor start\n"); fflush(stderr);
    _instance = this;
    setWindowTitle("Notepad--Qt");
    setAcceptDrops(true);
    resize(1200, 800);
    
    // MenuBar
    _menuBar = new MenuBar(this);
    setMenuBar(_menuBar);

    // Connect macro command signal from ShortcutManager
    connect(&ShortcutManager::instance(), &ShortcutManager::macroCommandRequested,
            this, &MainWindow::onMacroCommand);
    
    // Toolbar  
    _toolBar = new ToolBar(this);
    addToolBar(_toolBar);
    
    // Tab bar above tab widget
    _tabBar = new TabBar(this);
    
    // Central widget is the tab widget
    _tabWidget = new QTabWidget(this);
    _tabWidget->setTabsClosable(true);
    _tabWidget->setMovable(true);
    _tabWidget->setDocumentMode(true);
    
    // Stack tab bar and tab widget
    QWidget* centralContainer = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(_tabBar);
    centralLayout->addWidget(_tabWidget);
    
    setCentralWidget(centralContainer);
    
    // Status bar
    _statusBarWidget = new StatusBar(this);
    setStatusBar(_statusBarWidget);
    
    // Recent file mapper
    _recentFileMapper = new QSignalMapper(this);
    connect(_recentFileMapper, &QSignalMapper::mappedString, this, &MainWindow::onRecentFileSelected);
    
    // Create actions, menus, toolbar
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createPanels();
    setupConnections();
    
    // Connections
    connect(_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    // Tab context menu (right-click on tab)
    _tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_tabWidget, &QWidget::customContextMenuRequested, this, &MainWindow::onTabContextMenu);

    // Incremental Search dialog
    _incrementalSearch = new IncrementalSearchDialog(this);
    _incrementalSearch->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    // Wire Ctrl+I for incremental search
    connect(new QShortcut(QKeySequence("Ctrl+I"), this), &QShortcut::activated,
        this, [=]() {
            if (!_incrementalSearch) return;
            ScintillaEditor* ed = Application::instance().getActiveEditor();
            if (!ed) return;
            _incrementalSearch->showAtTop();
        });

    // Wire incremental search signals to the active editor
    connect(_incrementalSearch, &IncrementalSearchDialog::searchNext, this, [=](const QString& text) {
        if (auto* ed = Application::instance().getActiveEditor()) ed->findNext(text, {});
    });
    connect(_incrementalSearch, &IncrementalSearchDialog::searchPrev, this, [=](const QString& text) {
        if (auto* ed = Application::instance().getActiveEditor()) ed->findPrevious(text, {});
    });

    // Connect DPI changes to UI refresh
    connect(&DpiManager::instance(), &DpiManager::dpiChanged, this, &MainWindow::onDpiChanged);
}

MainWindow::~MainWindow() = default;

void MainWindow::createActions() {
    // File actions
    QAction* newAction = new QAction("&New", this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    _actions["file.new"] = newAction;
    
    QAction* openAction = new QAction("&Open...", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    _actions["file.open"] = openAction;
    
    QAction* saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    _actions["file.save"] = saveAction;
    
    QAction* saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    _actions["file.saveAs"] = saveAsAction;
    
    QAction* saveAllAction = new QAction("Save A&ll", this);
    saveAllAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    _actions["file.saveAll"] = saveAllAction;
    
    QAction* closeAction = new QAction("&Close", this);
    closeAction->setShortcut(QKeySequence("Ctrl+W"));
    _actions["file.close"] = closeAction;
    
    QAction* closeAllAction = new QAction("Clos&e All", this);
    _actions["file.closeAll"] = closeAllAction;
    
    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence("Alt+F4"));
    _actions["file.exit"] = exitAction;
    
    // Edit actions
    QAction* undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    _actions["edit.undo"] = undoAction;
    
    QAction* redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    _actions["edit.redo"] = redoAction;
    
    QAction* cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    _actions["edit.cut"] = cutAction;
    
    QAction* copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    _actions["edit.copy"] = copyAction;
    
    QAction* pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    _actions["edit.paste"] = pasteAction;
    
    QAction* deleteAction = new QAction("&Delete", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    _actions["edit.delete"] = deleteAction;
    
    QAction* selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    _actions["edit.selectAll"] = selectAllAction;
    
    QAction* findAction = new QAction("&Find...", this);
    findAction->setShortcut(QKeySequence::Find);
    findAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    _actions["edit.find"] = findAction;
    
    QAction* replaceAction = new QAction("&Replace...", this);
    replaceAction->setShortcut(QKeySequence("Ctrl+H"));
    _actions["edit.replace"] = replaceAction;
    
    QAction* gotoAction = new QAction("&Go to Line...", this);
    gotoAction->setShortcut(QKeySequence("Ctrl+G"));
    _actions["edit.goto"] = gotoAction;
    
    // Search actions
    QAction* findNextAction = new QAction("Find &Next", this);
    findNextAction->setShortcut(QKeySequence::FindNext);
    _actions["search.findNext"] = findNextAction;
    
    QAction* findPrevAction = new QAction("Find &Previous", this);
    findPrevAction->setShortcut(QKeySequence("Shift+F3"));
    _actions["search.findPrev"] = findPrevAction;
    
    QAction* findInFilesAction = new QAction("Find in Fi&les...", this);
    findInFilesAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    _actions["search.findInFiles"] = findInFilesAction;
    
    QAction* countAction = new QAction("&Count", this);
    _actions["search.count"] = countAction;
    
    QAction* markAllAction = new QAction("Mark &All", this);
    _actions["search.markAll"] = markAllAction;
    
    // View actions
    QAction* fullScreenAction = new QAction("Toggle F&ull Screen", this);
    fullScreenAction->setShortcut(QKeySequence("F11"));
    _actions["view.fullScreen"] = fullScreenAction;
    
    QAction* distractionFreeAction = new QAction("Toggle &Distraction-Free Mode", this);
    distractionFreeAction->setShortcut(QKeySequence("F12"));
    _actions["view.distractionFree"] = distractionFreeAction;
    
    QAction* showTabBarAction = new QAction("Show &Tab Bar", this);
    showTabBarAction->setCheckable(true);
    showTabBarAction->setChecked(true);
    _actions["view.showTabBar"] = showTabBarAction;
    
    QAction* showStatusBarAction = new QAction("Show &Status Bar", this);
    showStatusBarAction->setCheckable(true);
    showStatusBarAction->setChecked(true);
    _actions["view.showStatusBar"] = showStatusBarAction;
    
    QAction* showToolBarAction = new QAction("Show &Tool Bar", this);
    showToolBarAction->setCheckable(true);
    showToolBarAction->setChecked(true);
    _actions["view.showToolBar"] = showToolBarAction;
    
    QAction* fileBrowserAction = new QAction("&File Browser", this);
    fileBrowserAction->setCheckable(true);
    fileBrowserAction->setChecked(true);
    _actions["view.fileBrowser"] = fileBrowserAction;
    
    QAction* funcListAction = new QAction("F&unction List", this);
    funcListAction->setCheckable(true);
    funcListAction->setChecked(true);
    _actions["view.functionList"] = funcListAction;
    
    QAction* docMapAction = new QAction("&Document Map", this);
    docMapAction->setCheckable(true);
    docMapAction->setChecked(true);
    _actions["view.documentMap"] = docMapAction;
    
    // Encoding actions
    QAction* convUtf8Action = new QAction("Convert to &UTF-8", this);
    _actions["encoding.utf8"] = convUtf8Action;
    
    QAction* convUtf8BomAction = new QAction("Convert to UTF-8 &BOM", this);
    _actions["encoding.utf8bom"] = convUtf8BomAction;
    
    QAction* convUtf16LeAction = new QAction("Convert to UTF-16 LE", this);
    _actions["encoding.utf16le"] = convUtf16LeAction;
    
    QAction* convUtf16BeAction = new QAction("Convert to UTF-16 BE", this);
    _actions["encoding.utf16be"] = convUtf16BeAction;
    
    // Settings actions
    QAction* prefsAction = new QAction("&Preferences...", this);
    prefsAction->setShortcut(QKeySequence("Ctrl+Alt+P"));
    _actions["settings.preferences"] = prefsAction;
    
    QAction* shortcutMapperAction = new QAction("&Shortcut Mapper...", this);
    _actions["settings.shortcutMapper"] = shortcutMapperAction;
    
    QAction* commandPaletteAction = new QAction("&Command Palette...", this);
    commandPaletteAction->setShortcut(QKeySequence("Ctrl+Shift+P"));
    _actions["settings.commandPalette"] = commandPaletteAction;
    
    // Help actions
    QAction* aboutAction = new QAction("&About", this);
    _actions["help.about"] = aboutAction;
    
    // Connect all actions to dispatchCommand
    for (auto it = _actions.constBegin(); it != _actions.constEnd(); ++it) {
        connect(it.value(), &QAction::triggered, this, [this, key = it.key()]() {
            dispatchCommand(key);
        });
    }
}

void MainWindow::createMenus() {
    // File menu actions are already created
    if (MenuBar* mb = _menuBar) {
        QMenu* fileMenu = mb->fileMenu();
        // Ensure submenus exist by accessing them
        (void)mb->editMenu();
        (void)mb->searchMenu();
        (void)mb->viewMenu();
        (void)mb->encodingMenu();
        (void)mb->settingsMenu();
        (void)mb->helpMenu();
        
        // File menu items
        if (fileMenu) {
            // The menu bar already has its own File menu with actions
            // We could add additional file menu items here if needed
        }
        
        // Update recent files
        updateRecentFilesMenu();
    }
}

void MainWindow::createToolBar() {
    // Actions are already created, toolbar will use them via signal connections
}

void MainWindow::createStatusBar() {
    // Status bar is created in setupUi
    _statusBarWidget->setPosition(1, 1);
    _statusBarWidget->setEncoding("UTF-8");
    _statusBarWidget->setEol("Unix (LF)");
    _statusBarWidget->setLanguage("Normal Text");
}

void MainWindow::createPanels() {
    fprintf(stderr, "DEBUG: createPanels() start\n"); fflush(stderr);

    // File Browser panel (left dock) — created HERE so panels exist during construction
    _fileBrowserPanel = new FileBrowserPanel(this);
    _fileBrowserDock = new QDockWidget("File Browser", this);
    _fileBrowserDock->setWidget(_fileBrowserPanel);
    _fileBrowserDock->setObjectName("FileBrowserDock");
    addDockWidget(Qt::LeftDockWidgetArea, _fileBrowserDock);
    connect(_fileBrowserDock, &QDockWidget::visibilityChanged,
            this, [this](bool v){ if(v) _fileBrowserDock->setFocus(); });
    connect(_fileBrowserPanel, &FileBrowserPanel::fileDoubleClicked,
            this, [this](const QString& path){ openFileInTab(path); });

    // Function List panel (right dock)
    _funcListPanel = new FunctionListPanel(this);
    _funcListDock = new QDockWidget("Function List", this);
    _funcListDock->setWidget(_funcListPanel);
    _funcListDock->setObjectName("FunctionListDock");
    addDockWidget(Qt::RightDockWidgetArea, _funcListDock);

    // Document Map panel (right dock, tabified with Function List)
    _docMapPanel = new DocumentMapPanel(this);
    QDockWidget* docMapDock = new QDockWidget("Document Map", this);
    docMapDock->setWidget(_docMapPanel);
    addDockWidget(Qt::RightDockWidgetArea, docMapDock);
    tabifyDockWidget(_funcListDock, docMapDock);
    _funcListDock->raise();

    // Sync pointers to Application so both sides reference the same objects
    _app->setFileBrowserPanel(_fileBrowserPanel);
    _app->setFunctionListPanel(_funcListPanel);
    _app->setDocumentMapPanel(_docMapPanel);
    fprintf(stderr, "DEBUG: createPanels() done\n"); fflush(stderr);
}

void MainWindow::setupConnections() {
    // Connect menu bar commands
    connect(_menuBar, &MenuBar::menuCommand, this, &MainWindow::onMenuCommand);
    
    // Connect toolbar commands
    connect(_toolBar, &ToolBar::toolBarCommand, this, &MainWindow::onToolBarCommand);
}

void MainWindow::updateRecentFilesMenu() {
    if (!_menuBar) return;
    
    QMenu* fileMenu = _menuBar->fileMenu();
    if (!fileMenu) return;
    
    // Find and clear old Open Recent menu items
    QList<QAction*> fileActions = fileMenu->actions();
    for (int i = 0; i < fileActions.size(); ++i) {
        QAction* action = fileActions[i];
        if (action->text().contains("Open Recent")) {
            QMenu* submenu = action->menu();
            if (submenu) {
                // Keep the "Clear Recent Files" action, clear the rest
                QList<QAction*> subActions = submenu->actions();
                for (QAction* subAct : subActions) {
                    if (subAct->data().toString() != "file.clearRecent") {
                        submenu->removeAction(subAct);
                        delete subAct;
                    }
                }
                
                // Add recent files
                auto recentFiles = _app->getRecentFiles();
                for (const auto& file : recentFiles) {
                    QAction* recentAction = submenu->addAction(QString::fromStdString(file));
                    recentAction->setData(QString::fromStdString(file));
                    connect(recentAction, &QAction::triggered, _recentFileMapper, [this, file]() {
                        _recentFileMapper->setMapping(qobject_cast<QAction*>(sender()), QString::fromStdString(file));
                        _recentFileMapper->mappedString(QString::fromStdString(file));
                    });
                }
            }
            break;
        }
    }
}

void MainWindow::dispatchCommand(const QString& cmd) {
    // File commands
    if (cmd == "file.new") {
        onNewFile();
    } else if (cmd == "file.open") {
        onOpenFile();
    } else if (cmd == "file.save") {
        onSaveFile();
    } else if (cmd == "file.saveAs") {
        onSaveFileAs();
    } else if (cmd == "file.saveAll") {
        _app->saveAllFiles();
    } else if (cmd == "file.close") {
        onCloseFile();
    } else if (cmd == "file.closeAll") {
        _app->closeAllFiles();
    } else if (cmd == "file.exit") {
        onExit();
    } else if (cmd == "file.clearRecent") {
        _app->clearRecentFiles();
        updateRecentFilesMenu();
    }
    // Edit commands
    else if (cmd == "edit.undo") {
        onUndo();
    } else if (cmd == "edit.redo") {
        onRedo();
    } else if (cmd == "edit.cut") {
        onCut();
    } else if (cmd == "edit.copy") {
        onCopy();
    } else if (cmd == "edit.paste") {
        onPaste();
    } else if (cmd == "edit.delete") {
        onDelete();
    } else if (cmd == "edit.selectAll") {
        onSelectAll();
    } else if (cmd == "edit.find") {
        onFind();
    } else if (cmd == "edit.replace") {
        onReplace();
    } else if (cmd == "edit.goto") {
        onGoto();
    }
    // Search commands
    else if (cmd == "search.findNext") {
        onFindNext();
    } else if (cmd == "search.findPrev") {
        onFindPrevious();
    } else if (cmd == "search.incrementalSearch") {
        if (!_incrementalSearch) return;
        ScintillaEditor* ed = _app->getActiveEditor();
        if (!ed) return;
        _incrementalSearch->showAtTop();
    } else if (cmd == "search.findInFiles") {
        _app->onFindInFiles();
    } else if (cmd == "search.count") {
        if (auto* dlg = _app->findReplaceDialog()) {
            QString text = dlg->lastSearchText();
            if (!text.isEmpty()) {
                if (auto* editor = currentEditor()) {
                    int count = editor->countMatches(text, dlg->lastSearchOptions());
                    _statusBarWidget->setEncoding(QString("Found %1 matches").arg(count));
                }
            }
        }
    } else if (cmd == "search.markAll") {
        if (auto* dlg = _app->findReplaceDialog()) {
            QString text = dlg->lastSearchText();
            if (!text.isEmpty()) {
                if (auto* editor = currentEditor()) {
                    int count = editor->markAllMatches(text, dlg->lastSearchOptions());
                    _statusBarWidget->setEncoding(QString("Marked %1 matches").arg(count));
                }
            }
        }
    }
    // View commands
    else if (cmd == "view.fullScreen") {
        onFullScreen();
    } else if (cmd == "view.distractionFree") {
        onDistractionFree();
    } else if (cmd == "view.showTabBar") {
        onShowTabBar(_actions["view.showTabBar"]->isChecked());
    } else if (cmd == "view.showStatusBar") {
        onShowStatusBar(_actions["view.showStatusBar"]->isChecked());
    } else if (cmd == "view.showToolBar") {
        onShowToolBar(_actions["view.showToolBar"]->isChecked());
    } else if (cmd == "view.fileBrowser") {
        _fileBrowserDock->setVisible(_actions["view.fileBrowser"]->isChecked());
    } else if (cmd == "view.functionList") {
        _funcListDock->setVisible(_actions["view.functionList"]->isChecked());
    } else if (cmd == "view.documentMap") {
        app().documentMapPanel()->setVisible(_actions["view.documentMap"]->isChecked());
    } else if (cmd == "view.darkMode") {
        onThemeChanged("dark");
    } else if (cmd == "view.lightMode") {
        onThemeChanged("light");
    }
    // Encoding commands
    else if (cmd == "encoding.utf8") {
        onEncodingChanged("UTF-8");
    } else if (cmd == "encoding.utf8bom") {
        onEncodingChanged("UTF-8-BOM");
    } else if (cmd == "encoding.utf16le") {
        onEncodingChanged("UTF-16-LE");
    } else if (cmd == "encoding.utf16be") {
        onEncodingChanged("UTF-16-BE");
    } else if (cmd.startsWith("encoding.charset.")) {
        QString charset = cmd.mid(17);
        onEncodingChanged(charset);
    }
    // Language commands
    else if (cmd.startsWith("language.")) {
        QString langName = cmd.mid(9);
        langName = langName.replace('_', ' ');
        ScintillaEditor* ed = _app->getActiveEditor();
        LangType lang = LangType::L_TEXT;
        if (langName == "C++") lang = LangType::L_CPP;
        else if (langName == "C") lang = LangType::L_C;
        else if (langName == "Java") lang = LangType::L_JAVA;
        else if (langName == "C#") lang = LangType::L_CS;
        else if (langName == "HTML") lang = LangType::L_HTML;
        else if (langName == "XML") lang = LangType::L_XML;
        else if (langName == "PHP") lang = LangType::L_PHP;
        else if (langName == "Python") lang = LangType::L_PYTHON;
        else if (langName == "JavaScript") lang = LangType::L_JS;
        else if (langName == "JSON") lang = LangType::L_JSON;
        else if (langName == "CSS") lang = LangType::L_CSS;
        else if (langName == "YAML") lang = LangType::L_YAML;
        else if (langName == "Ruby") lang = LangType::L_RUBY;
        else if (langName == "Perl") lang = LangType::L_PERL;
        else if (langName == "Lua") lang = LangType::L_LUA;
        else if (langName == "Markdown") lang = LangType::L_MARKDOWN;
        else if (langName == "Makefile") lang = LangType::L_MAKEFILE;
        if (ed) {
            ed->setLanguage(lang);
            QsciLexer* lexer = LanguageManager::createLexer(lang);
            if (lexer) ed->qsciEditor()->setLexer(lexer);
        }
    }
    // Settings commands
    else if (cmd == "settings.preferences") {
        onPreferences();
    } else if (cmd == "settings.shortcutMapper") {
        onShortcutMapper();
    } else if (cmd == "settings.commandPalette") {
        onCommandPalette();
    }
    // Help commands
    else if (cmd == "help.about") {
        onAbout();
    }
}

void MainWindow::onMenuCommand(const QString& cmd) {
    dispatchCommand(cmd);
}

void MainWindow::onToolBarCommand(const QString& cmd) {
    dispatchCommand(cmd);
}

void MainWindow::onRecentFileSelected(const QString& file) {
    app().openFile(file.toStdString());
}

// Tab management
void MainWindow::addEditorTab(ScintillaEditor* editor, const QString& title) {
    int index = _tabWidget->addTab(editor, title);
    _tabWidget->setCurrentIndex(index);
    updateTabBar();
}

void MainWindow::removeEditorTab(int index, BufferID closingBuffer) {
    if (index < 0 || index >= _tabWidget->count()) return;
    if (closingBuffer != BUFFER_INVALID) {
        _bufferToEditor.remove(closingBuffer);
        _bufferToTab.remove(closingBuffer);
    }
    _tabToBuffer.remove(index);
    _tabWidget->removeTab(index);
    rebuildTabToBufferMap();
    updateTabBar();
}

void MainWindow::setTabText(int index, const QString& title) {
    _tabWidget->setTabText(index, title);
}

void MainWindow::setTabModified(int index, bool modified) {
    if (index < 0 || index >= _tabWidget->count()) return;
    QString title = _tabWidget->tabText(index);
    if (modified && !title.endsWith('*')) title += '*';
    else if (!modified && title.endsWith('*')) title.chop(1);
    _tabWidget->setTabText(index, title);
}

int MainWindow::currentTabIndex() const { 
    return _tabWidget->currentIndex(); 
}

ScintillaEditor* MainWindow::currentEditor() const {
    return qobject_cast<ScintillaEditor*>(_tabWidget->currentWidget());
}

ScintillaEditor* MainWindow::editorAt(int index) const {
    return qobject_cast<ScintillaEditor*>(_tabWidget->widget(index));
}

int MainWindow::editorCount() const {
    return _tabWidget->count();
}

BufferID MainWindow::bufferAtTabIndex(int tabIndex) const {
    if (tabIndex < 0 || tabIndex >= _tabWidget->count()) return BUFFER_INVALID;
    return _tabToBuffer.value(tabIndex, BUFFER_INVALID);
}

void MainWindow::updateTabBar() { 
    if (_tabBar) _tabBar->updateFrom(_tabWidget); 
}

void MainWindow::updateStatusBar() {
    if (!_statusBarWidget) return;
    
    auto* editor = currentEditor();
    if (editor) {
        _statusBarWidget->setPosition(editor->currentLine() + 1, editor->currentColumn() + 1);
        _statusBarWidget->setEncoding(QString::fromStdString(
            app().getBufferEncoding(app().getActiveBuffer()) == EncodingType::UTF_8 ? "UTF-8" : "Other"
        ));
        _statusBarWidget->setEol(editor->eolType() == EolType::EOL_CRLF ? "Windows (CRLF)" : "Unix (LF)");
    }
}

void MainWindow::updateTitleBar(const QString& fileName) {
    QString title = "Notepad--Qt";
    if (!fileName.isEmpty()) title = fileName + " - " + title;
    else if (currentEditor()) {
        // Show current file name
        BufferID buf = app().getActiveBuffer();
        if (buf) {
            auto name = app().getFileName(buf);
            if (name) {
                title = QString::fromStdString(*name) + " - " + title;
            }
        }
    }
    setWindowTitle(title);
}

// File operations
void MainWindow::onNewFile() {
    auto& appRef = Application::instance();
    BufferID buf = appRef.newFile();

    // Create editor
    ScintillaEditor* editor = new ScintillaEditor(_tabWidget);
    editor->applyTheme(ThemeManager::instance().currentTheme());  // apply current theme to status bar
    connect(editor, &ScintillaEditor::cursorPositionChanged,
             _statusBarWidget, [this](int line, int col) {
        _statusBarWidget->setPosition(line, col);
    });

    // Wire selection info to status bar
    connect(editor, qOverload<int, int, int>(&ScintillaEditor::selectionChanged),
            _statusBarWidget, [this](int start, int end, int lines) {
        _statusBarWidget->setSelection(end - start, lines);
    });

    // Connect text change to update buffer
    connect(editor, &ScintillaEditor::textChanged, this, [this, editor, buf]() {
        Application::instance().syncEditorToBuffer(editor, buf);
        if (_bufferToTab.contains(buf)) {
            int idx = _bufferToTab[buf];
            setTabModified(idx, editor->isModified());
        }
    });

    // Connect modificationChanged
    connect(editor, &ScintillaEditor::modificationChanged, this, [this, buf](bool modified) {
        onBufferModified(buf, modified);
        emit Application::instance().bufferModifiedChanged(buf);
    });

    int idx = _tabWidget->addTab(editor, "* Untitled");
    _tabWidget->setTabToolTip(idx, "Untitled");
    _tabWidget->setCurrentIndex(idx);

    // Register
    _tabToBuffer[idx] = buf;
    _bufferToEditor[buf] = editor;
    _bufferToTab[buf] = idx;

    updateTabBar();
    updateTitleBar();
    editor->setFocus();
}
void MainWindow::onOpenFile() {
    QString file = QFileDialog::getOpenFileName(this, "Open File", _lastOpenedDirectory);
    if (!file.isEmpty()) {
        _lastOpenedDirectory = QFileInfo(file).absolutePath();
        openFileInTab(file);
    }
}

void MainWindow::openFileInTab(const QString& path) {
    auto& appRef = Application::instance();
    BufferID buf = appRef.openFile(path.toStdString());
    if (!buf) {
        QMessageBox::warning(this, "Open Error", "Could not open: " + path);
        return;
    }

    // Check if already open — switch to existing tab
    if (_bufferToTab.contains(buf)) {
        _tabWidget->setCurrentIndex(_bufferToTab[buf]);
        return;
    }

    // Create editor
    ScintillaEditor* editor = new ScintillaEditor(_tabWidget);

    // Wire cursor position to status bar
    connect(editor, &ScintillaEditor::cursorPositionChanged,
             _statusBarWidget, [this](int line, int col) {
        _statusBarWidget->setPosition(line, col);
    });

    // Wire selection info to status bar
    connect(editor, qOverload<int, int, int>(&ScintillaEditor::selectionChanged),
            _statusBarWidget, [this](int start, int end, int lines) {
        _statusBarWidget->setSelection(end - start, lines);
    });

    // Load content
    QString text = QString::fromUtf8(appRef.getBufferText(buf).c_str());
    editor->setPlainText(text);

    // Encoding
    auto enc = appRef.getBufferEncoding(buf);
    editor->setEncoding(enc);

    // Tab title from filename
    QString fileName = QFileInfo(path).fileName();
    int idx = _tabWidget->addTab(editor, fileName);
    _tabWidget->setTabToolTip(idx, path);

    // Register in all three maps
    _tabToBuffer[idx] = buf;
    _bufferToEditor[buf] = editor;
    _bufferToTab[buf] = idx;

    // Connect text change to update buffer
    connect(editor, &ScintillaEditor::textChanged, this, [this, editor, buf]() {
        Application::instance().syncEditorToBuffer(editor, buf);
        if (_bufferToTab.contains(buf)) {
            int idx = _bufferToTab[buf];
            setTabModified(idx, editor->isModified());
        }
    });

    // Connect modificationChanged to update tab title and window title
    connect(editor, &ScintillaEditor::modificationChanged, this, [this, buf](bool modified) {
        onBufferModified(buf, modified);
        emit Application::instance().bufferModifiedChanged(buf);
    });

    _tabWidget->setCurrentIndex(idx);
    updateTabBar();
    updateTitleBar();

    // Connect Function List panel to the new editor
    if (app().functionListPanel()) {
        app().functionListPanel()->setEditor(editor);
    }
}

void MainWindow::updateTabTitle(BufferID buf, bool dirty) {
    if (!_bufferToTab.contains(buf)) return;
    int idx = _bufferToTab[buf];
    QString title = _tabWidget->tabText(idx);
    if (dirty && !title.startsWith("* ")) {
        _tabWidget->setTabText(idx, "* " + title);
    } else if (!dirty && title.startsWith("* ")) {
        _tabWidget->setTabText(idx, title.mid(2));
    }
}

void MainWindow::rebuildTabToBufferMap() {
    _tabToBuffer.clear();
    for (auto it = _bufferToTab.constBegin(); it != _bufferToTab.constEnd(); ++it) {
        _tabToBuffer[it.value()] = it.key();
    }
}

void MainWindow::onSaveFile() {
    BufferID buffer = app().getActiveBuffer();
    if (buffer) {
        auto name = app().getFileName(buffer);
        if (name && !name->empty()) {
            app().saveFile(buffer);
        } else {
            onSaveFileAs();
        }
    }
}

void MainWindow::onSaveFileAs() {
    BufferID buffer = app().getActiveBuffer();
    ScintillaEditor* ed = _app->getActiveEditor();
    if (!buffer) return;
    // Sync editor text into buffer before save-as
    if (ed) app().syncEditorToBuffer(ed, buffer);

    QString file = QFileDialog::getSaveFileName(this, "Save File As", _lastOpenedDirectory);
    if (!file.isEmpty()) {
        _lastOpenedDirectory = QFileInfo(file).absolutePath();
        if (app().saveFile(buffer, file.toStdString())) {
            if (ed) setTabModified(_tabWidget->currentIndex(), ed->isModified());
        }
        app().addToRecentFiles(file.toStdString());
        updateRecentFilesMenu();
        updateTitleBar();
    }
}

void MainWindow::onCloseFile() {
    BufferID buffer = app().getActiveBuffer();
    if (buffer) {
        app().closeFile(buffer);
    }
}

void MainWindow::onExit() { 
    close(); 
}

// Edit operations
void MainWindow::onUndo() { 
    if (auto* e = currentEditor()) e->undo(); 
}

void MainWindow::onRedo() { 
    if (auto* e = currentEditor()) e->redo(); 
}

void MainWindow::onCut() { 
    if (auto* e = currentEditor()) e->cut(); 
}

void MainWindow::onCopy() { 
    if (auto* e = currentEditor()) e->copy(); 
}

void MainWindow::onPaste() { 
    if (auto* e = currentEditor()) e->paste(); 
}

void MainWindow::onDelete() {
    if (auto* e = currentEditor()) e->deleteSelection();
}

void MainWindow::onSelectAll() { 
    if (auto* e = currentEditor()) e->selectAll(); 
}

void MainWindow::onFind() {
    // Open find dialog
    if (auto* dlg = _app->findReplaceDialog()) {
        dlg->show();
    }
}

void MainWindow::onReplace() {
    // Open replace dialog
    if (auto* dlg = _app->findReplaceDialog()) {
        dlg->show();
    }
}

void MainWindow::onGoto() {
    if (auto* dlg = app().gotoLineDialog()) {
        dlg->show();
    }
}

void MainWindow::onFindNext() {
    if (auto* dlg = _app->findReplaceDialog()) {
        QString text = dlg->lastSearchText();
        if (!text.isEmpty()) {
            if (auto* editor = currentEditor()) {
                editor->findNext(text, dlg->lastSearchOptions());
            }
        } else {
            // No last search — open the dialog
            onFind();
        }
    }
}

void MainWindow::onFindPrevious() {
    if (auto* dlg = _app->findReplaceDialog()) {
        QString text = dlg->lastSearchText();
        if (!text.isEmpty()) {
            if (auto* editor = currentEditor()) {
                editor->findPrevious(text, dlg->lastSearchOptions());
            }
        }
    }
}

// View operations
void MainWindow::onFullScreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::onDistractionFree() {
    app().toggleDistractionFree();
    if (app().isDistractionFree()) {
        // Hide all UI except editor
        _menuBar->hide();
        _toolBar->hide();
        _tabBar->hide();
        _statusBarWidget->hide();
    } else {
        _menuBar->show();
        _toolBar->show();
        _tabBar->show();
        _statusBarWidget->show();
    }
}

void MainWindow::onShowTabBar(bool show) {
    _tabBar->setVisible(show);
}

void MainWindow::onShowStatusBar(bool show) {
    _statusBarWidget->setVisible(show);
}

void MainWindow::onShowToolBar(bool show) {
    _toolBar->setVisible(show);
}

// Encoding
void MainWindow::onEncodingChanged(const QString& encoding) {
    BufferID buffer = app().getActiveBuffer();
    if (!buffer) return;
    
    EncodingType enc = EncodingType::ANSI;
    if (encoding == "UTF-8") enc = EncodingType::UTF_8;
    else if (encoding == "UTF-8-BOM") enc = EncodingType::UTF_8_BOM;
    else if (encoding == "UTF-16-LE") enc = EncodingType::UTF_16_LE;
    else if (encoding == "UTF-16-BE") enc = EncodingType::UTF_16_BE;
    
    app().setBufferEncoding(buffer, enc);
    if (auto* editor = currentEditor()) {
        editor->setEncoding(enc);
    }
    updateStatusBar();
}

// Settings
void MainWindow::onPreferences() {
    if (auto* dlg = app().preferenceDialog()) {
        dlg->show();
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About Notepad--Qt", 
        "<h3>Notepad--Qt</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A Qt-based text editor, inspired by Notepad++.</p>"
        "<p>GPL v3 License</p>"
        "<p>Copyright (C) 2026 Agent Army</p>");
}

void MainWindow::onShortcutMapper() {
    app().onShowShortcutMapper();
}

void MainWindow::onCommandPalette() {
    app().onShowCommandPalette();
}

void MainWindow::onMacroCommand(const QString& macroName) {
    app().macroManager()->playback(macroName);
}

// Buffer notifications
void MainWindow::onBufferOpened(BufferID buffer) {
    if (!buffer) return;

    // Check if already open — switch to existing tab
    if (_bufferToTab.contains(buffer)) {
        int idx = _bufferToTab[buffer];
        _tabWidget->setCurrentIndex(idx);
        return;
    }

    // Create editor for this buffer
    auto* editor = new ScintillaEditor(_tabWidget);

    // Load text
    std::string text = app().getBufferText(buffer);
    editor->setPlainText(QString::fromUtf8(text.c_str()));

    // Get file name for tab title
    QString title = "Untitled";
    auto name = app().getFileName(buffer);
    if (name && !name->empty()) {
        title = QString::fromStdString(*name);
    }

    // Add tab
    int idx = _tabWidget->addTab(editor, title);
    if (name && !name->empty()) {
        _tabWidget->setTabToolTip(idx, QString::fromStdString(*name));
    }
    _tabWidget->setCurrentIndex(idx);

    // Register in all three maps
    _tabToBuffer[idx] = buffer;
    _bufferToEditor[buffer] = editor;
    _bufferToTab[buffer] = idx;

    // Connect editor signals
    connect(editor, &ScintillaEditor::textChanged, this, &MainWindow::onBufferChanged);
    connect(editor, &ScintillaEditor::modificationChanged, this, [this, buffer](bool modified) {
        onBufferModified(buffer, modified);
        emit Application::instance().bufferModifiedChanged(buffer);
    });
    // Watch for external file changes
    connect(buffer, &Buffer::fileExternallyModified,
            this, [this](const QString& path) { onFileExternallyModified(path); });
    connect(editor, &ScintillaEditor::cursorPositionChanged, _statusBarWidget, [this](int line, int col) {
        _statusBarWidget->setPosition(line, col);
    });
    connect(editor, qOverload<int, int, int>(&ScintillaEditor::selectionChanged), _statusBarWidget, [this](int start, int end, int lines) {
        _statusBarWidget->setSelection(end - start, lines);
    });

    // Wire DocumentMap to the first editor (initial setup)
    if (app().documentMapPanel() && !_app->getActiveEditor()) {
        app().documentMapPanel()->setEditor(editor);
    }

    updateTabBar();
    updateTitleBar();
    updateStatusBar();

    // Register this editor as the active one
    app().setActiveEditor(editor);
}

void MainWindow::onFileExternallyModified(const QString& filePath) {
    FileReloadDialog::Action action = FileReloadDialog::prompt(filePath, this);
    
    if (action == FileReloadDialog::Action::Reload) {
        BufferID buf = app().getBufferForPath(filePath.toStdString());
        if (buf) {
            app().reloadFile(buf);
            ScintillaEditor* ed = _app->getActiveEditor();
            if (ed) {
                ed->setText(QString::fromStdString(app().getBufferText(buf)));
                ed->setModified(false);
            }
        }
    }
    // KeepDisk and DoNothing: leave editor as-is
}

void MainWindow::onBufferActivated(BufferID buffer) {
    if (!buffer) return;

    // Find tab for this buffer and switch to it
    if (_bufferToTab.contains(buffer)) {
        int index = _bufferToTab[buffer];
        if (index != _tabWidget->currentIndex()) {
            _tabWidget->setCurrentIndex(index);
        }
        // Also update active editor
        ScintillaEditor* ed = editorAt(index);
        if (ed) app().setActiveEditor(ed);
    }

    updateTitleBar();
    updateStatusBar();
}

void MainWindow::onBufferClosed(BufferID buffer) {
    if (_bufferToTab.contains(buffer)) {
        int index = _bufferToTab[buffer];
        removeEditorTab(index, buffer);  // removeEditorTab rebuilds the map
    }
}

void MainWindow::onBufferModified(BufferID buffer, bool modified) {
    if (_bufferToTab.contains(buffer)) {
        int index = _bufferToTab[buffer];
        setTabModified(index, modified);
    }
}

void MainWindow::onBufferChanged() {
    BufferID buffer = app().getActiveBuffer();
    ScintillaEditor* ed = _app->getActiveEditor();
    if (buffer && ed) {
        // Sync editor text into the buffer immediately so save is always fresh
        app().syncEditorToBuffer(ed, buffer);
        // Update tab modified indicator
        if (_bufferToTab.contains(buffer)) {
            int index = _bufferToTab[buffer];
            setTabModified(index, ed->isModified());
        }
    }
    updateStatusBar();
}

// Theme
void MainWindow::onThemeChanged(const QString& theme) {
    ThemeManager::instance().loadTheme(theme);
    // Apply dark/light QSS to all chrome elements
    setStyleSheet(ThemeManager::instance().getThemeQss(theme, "main"));
    if (_toolBar)
        _toolBar->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "toolbar"));
    if (_statusBarWidget)
        _statusBarWidget->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "statusbar"));
    if (_menuBar)
        _menuBar->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "menu"));
    _tabWidget->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "tabs"));
    // Reload current theme resource
    app().loadTheme(theme.toStdString());
}

// DPI
void MainWindow::onDpiChanged(int dpi) {
    qDebug() << "DPI changed to" << dpi;

    // Reload the current theme's stylesheet with scaled sizes
    QString theme = QString::fromStdString(app().currentTheme());
    setStyleSheet(ThemeManager::instance().getThemeQss(theme, "main"));
    if (_toolBar)
        _toolBar->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "toolbar"));
    if (_statusBarWidget)
        _statusBarWidget->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "statusbar"));
    if (_menuBar)
        _menuBar->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "menu"));
    _tabWidget->setStyleSheet(ThemeManager::instance().getThemeQss(theme, "tabs"));

    // Update all open editors' fonts to the new DPI-scaled size
    for (int i = 0; i < _tabWidget->count(); ++i) {
        if (auto* editor = editorAt(i)) {
            editor->updateFontForDpi();
        }
    }

    // Rescale toolbar icons
    if (_toolBar) {
        _toolBar->updateForDpi();
    }

    // Rescale tab bar
    if (_tabBar) {
        _tabBar->updateForDpi();
    }

    // Rescale status bar
    if (_statusBarWidget) {
        _statusBarWidget->updateForDpi();
    }
}
// Tab events
void MainWindow::onTabChanged(int index) {
    if (index < 0) return;
    if (!_tabToBuffer.contains(index)) return;

    BufferID buf = _tabToBuffer[index];
    Application::instance().setActiveBuffer(buf);
    ScintillaEditor* ed = _bufferToEditor.value(buf);
    if (ed) Application::instance().setActiveEditor(ed);

    // Wire DocumentMap to the newly active editor
    if (Application::instance().documentMapPanel()) {
        Application::instance().documentMapPanel()->setEditor(ed);
        Application::instance().documentMapPanel()->onBufferChanged();
    }

    updateStatusBar();
    updateTitleBar();
}

void MainWindow::onTabCloseRequested(int index) {
    if (index < 0 || index >= _tabWidget->count()) return;

    BufferID buf = _tabToBuffer.value(index);
    ScintillaEditor* editor = _bufferToEditor.value(buf);

    // Save prompt if modified
    if (editor && editor->isModified()) {
        QString title = _tabWidget->tabText(index);
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Save Changes",
            QString("Do you want to save changes to %1?").arg(title),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Save) {
            Application::instance().setActiveEditor(editor);
            Application::instance().setActiveBuffer(buf);
            if (buf != BUFFER_INVALID) Application::instance().syncEditorToBuffer(editor, buf);
            QString path = Application::instance().fileManager()->getBufferPath(buf);
            if (path.isEmpty()) {
                // New file — use Save As
                onSaveFileAs();
            } else {
                Application::instance().saveFile(buf, path.toStdString());
            }
        } else if (btn == QMessageBox::Cancel) {
            return;
        }
    }

    // Remove from registry maps
    if (buf != BUFFER_INVALID) {
        _bufferToEditor.remove(buf);
        _bufferToTab.remove(buf);
    }
    _tabToBuffer.remove(index);

    // Close buffer
    if (buf != BUFFER_INVALID) Application::instance().closeFile(buf);

    // Remove tab
    _tabWidget->removeTab(index);

    // Rebuild tab→buffer map since indices shift
    rebuildTabToBufferMap();

    // Activate adjacent tab
    if (_tabWidget->count() > 0) {
        onTabChanged(_tabWidget->currentIndex());
    }
}

void MainWindow::onTabContextMenu(const QPoint& pos) {
    int tabIdx = _tabWidget->tabBar()->tabAt(pos);
    if (tabIdx < 0) return;

    QMenu menu(this);
    QAction* closeAct = menu.addAction("Close");
    QAction* closeOthersAct = menu.addAction("Close Other Tabs");
    QAction* closeAllAct = menu.addAction("Close All Tabs");
    menu.addSeparator();
    QAction* openFolderAct = menu.addAction("Open Containing Folder");
    QAction* copyPathAct = menu.addAction("Copy Full Path");

    QAction* chosen = menu.exec(_tabWidget->tabBar()->mapToGlobal(pos));
    if (!chosen) return;

    if (chosen == closeAct) {
        onTabCloseRequested(tabIdx);
    } else if (chosen == closeOthersAct) {
        for (int i = _tabWidget->count() - 1; i >= 0; --i) {
            if (i != tabIdx) onTabCloseRequested(i);
        }
    } else if (chosen == closeAllAct) {
        for (int i = _tabWidget->count() - 1; i >= 0; --i) {
            onTabCloseRequested(i);
        }
    } else if (chosen == openFolderAct) {
        BufferID buf = _tabToBuffer.value(tabIdx);
        if (buf) {
            QString path = QString::fromStdString(
                Application::instance().getFileName(buf).value_or(""));
            if (!path.isEmpty()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
            }
        }
    } else if (chosen == copyPathAct) {
        BufferID buf = _tabToBuffer.value(tabIdx);
        if (buf) {
            QString path = QString::fromStdString(
                Application::instance().getFileName(buf).value_or(""));
            if (!path.isEmpty()) {
                QApplication::clipboard()->setText(path);
            }
        }
    }
}

// Drag and drop
void MainWindow::closeEvent(QCloseEvent* event) {
    // Check for unsaved changes
    bool hasUnsaved = false;
    for (int i = 0; i < _tabWidget->count(); ++i) {
        if (auto* editor = editorAt(i)) {
            if (editor->isModified()) {
                hasUnsaved = true;
                break;
            }
        }
    }
    
    if (hasUnsaved) {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Unsaved Changes",
            "Some files have unsaved changes. Exit anyway?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (btn == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    
    // Save window geometry and state
    QSettings s;
    s.setValue("window/geometry", saveGeometry());
    s.setValue("window/state", saveState());
    
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile()) {
            QString file = url.toLocalFile();
            BufferID buffer = app().openFile(file.toStdString());
            if (buffer) {
                emit app().bufferOpened(buffer);
                app().addToRecentFiles(file.toStdString());
            }
        }
    }
    event->acceptProposedAction();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    // Resolve the key combination through ShortcutManager
    int key = event->key();
    int mods = event->modifiers();

    // Skip modifier-only events and unknown keys
    if (key == Qt::Key_unknown || key == Qt::Key_Shift
        || key == Qt::Key_Control || key == Qt::Key_Alt || key == Qt::Key_Meta)
    {
        QMainWindow::keyPressEvent(event);
        return;
    }

    // Build shortcut string
    QStringList parts;
    if (mods & Qt::ControlModifier) parts.append("Ctrl");
    if (mods & Qt::AltModifier)     parts.append("Alt");
    if (mods & Qt::ShiftModifier)   parts.append("Shift");
    if (mods & Qt::MetaModifier)   parts.append("Meta");

    // Convert key code to string
    int keyChar = 0;
    if (key >= Qt::Key_Space && key <= Qt::Key_AsciiTilde)
        keyChar = key;
    else if (key >= Qt::Key_A && key <= Qt::Key_Z)
        keyChar = key;
    else if (key >= Qt::Key_0 && key <= Qt::Key_9)
        keyChar = key;

    if (keyChar)
        parts.append(QString(QChar(keyChar)));
    else
        parts.append(QKeySequence(key).toString());

    QString shortcutStr = parts.join("+");

    // Resolve through ShortcutManager using the public API
    bool triggered = false;
    int cmdId = ShortcutManager::instance().resolveBinding(key, mods, nullptr);

    if (cmdId >= 0) {
        // Check if this command is a macro shortcut first
        QString shortcutStr = ShortcutManager::instance().makeShortcutText(key, mods);
        int macroIndex = ShortcutManager::instance().getMacroForShortcut(shortcutStr);
        if (macroIndex >= 0) {
            MacroManager::instance().playback(macroIndex);
            event->accept();
            return;
        }
        QString cmd = QString::fromStdString(EditorCommandManager::instance().getCommandName(cmdId));
        if (!cmd.isEmpty()) {
            dispatchCommand(cmd);
            triggered = true;
        }
    }

    // If no command was triggered, check if the resolved command maps to a macro
    if (!triggered) {
        QString cmd = (cmdId >= 0)
            ? QString::fromStdString(EditorCommandManager::instance().getCommandName(cmdId))
            : QString();
        QString macroName;
        if (!cmd.isEmpty() && ShortcutManager::instance().resolveMacroBinding(cmd, macroName)) {
            app().macroManager()->playback(macroName);
            triggered = true;
        }
    }

    if (triggered) {
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}
