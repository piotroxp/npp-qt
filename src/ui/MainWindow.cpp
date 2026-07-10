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
#include "../dialogs/GoToLineDialog.h"
#include "../dialogs/PreferenceDialog.h"
#include <Qsci/qsciscintilla.h>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QCloseEvent>
#include <QFileInfo>
#include <QAction>
#include <QKeySequence>
#include <QStyle>
#include <Qsci/qscilexercustom.h>
#include "../panels/DocumentMapPanel.h"
#include <QApplication>

MainWindow::MainWindow()
    : QMainWindow(nullptr)
{
    setWindowTitle("Notepad--Qt");
    setAcceptDrops(true);
    resize(1200, 800);
    
    // MenuBar
    _menuBar = new MenuBar(this);
    setMenuBar(_menuBar);
    
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
        QMenu* editMenu = mb->editMenu();
        QMenu* searchMenu = mb->searchMenu();
        QMenu* viewMenu = mb->viewMenu();
        QMenu* encodingMenu = mb->encodingMenu();
        QMenu* settingsMenu = mb->settingsMenu();
        QMenu* helpMenu = mb->helpMenu();
        
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
    // Panels are disabled for now - can be enabled later
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
                auto recentFiles = app().getRecentFiles();
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
        app().saveAllFiles();
    } else if (cmd == "file.close") {
        onCloseFile();
    } else if (cmd == "file.closeAll") {
        app().closeAllFiles();
    } else if (cmd == "file.exit") {
        onExit();
    } else if (cmd == "file.clearRecent") {
        app().clearRecentFiles();
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
    } else if (cmd == "search.findInFiles") {
        // Find in files dialog
        onFind();
    } else if (cmd == "search.count") {
        if (auto* dlg = app().findReplaceDialog()) {
            QString text = dlg->lastSearchText();
            if (!text.isEmpty()) {
                if (auto* editor = currentEditor()) {
                    int count = editor->countMatches(text, dlg->lastSearchOptions());
                    _statusBarWidget->setEncoding(QString("Found %1 matches").arg(count));
                }
            }
        }
    } else if (cmd == "search.markAll") {
        if (auto* dlg = app().findReplaceDialog()) {
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
        BufferID buf = app().getActiveBuffer();
        ScintillaEditor* ed = app().getActiveEditor();
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
    if (closingBuffer != BUFFER_INVALID) _bufferToTab.remove(closingBuffer);
    _tabWidget->removeTab(index);
    // Rebuild map: tab indices shift down after removal
    QMap<BufferID, int> newMap;
    for (int i = 0; i < _tabWidget->count(); ++i) {
        if (auto* editor = editorAt(i)) {
            if (BufferID buf = bufferAtTabIndex(i); buf != BUFFER_INVALID) {
                newMap[buf] = i;
            }
        }
    }
    _bufferToTab = newMap;
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
    // Find the buffer whose tab index matches
    for (auto it = _bufferToTab.constBegin(); it != _bufferToTab.constEnd(); ++it) {
        if (it.value() == tabIndex) return it.key();
    }
    return BUFFER_INVALID;
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
    else if (auto* editor = currentEditor()) {
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
    BufferID buffer = app().newFile();
    emit app().bufferOpened(buffer);
}

void MainWindow::onOpenFile() {
    qDebug() << "[onOpenFile] START";
    QString file = QFileDialog::getOpenFileName(this, "Open File", _lastOpenedDirectory);
    qDebug() << "[onOpenFile] file selected:" << file;
    if (!file.isEmpty()) {
        _lastOpenedDirectory = QFileInfo(file).absolutePath();
        qDebug() << "[onOpenFile] calling openFile";
        BufferID buffer = app().openFile(file.toStdString());
        qDebug() << "[onOpenFile] openFile returned:" << buffer;
        if (buffer) {
            qDebug() << "[onOpenFile] emitting bufferOpened";
            emit app().bufferOpened(buffer);
            qDebug() << "[onOpenFile] adding to recent";
            app().addToRecentFiles(file.toStdString());
            qDebug() << "[onOpenFile] updating menu";
            updateRecentFilesMenu();
            qDebug() << "[onOpenFile] DONE";
        }
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
    ScintillaEditor* ed = app().getActiveEditor();
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
    if (auto* dlg = app().findReplaceDialog()) {
        dlg->show();
    }
}

void MainWindow::onReplace() {
    // Open replace dialog
    if (auto* dlg = app().findReplaceDialog()) {
        dlg->show();
    }
}

void MainWindow::onGoto() {
    if (auto* dlg = app().gotoLineDialog()) {
        dlg->show();
    }
}

void MainWindow::onFindNext() {
    if (auto* dlg = app().findReplaceDialog()) {
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
    if (auto* dlg = app().findReplaceDialog()) {
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

// Buffer notifications
void MainWindow::onBufferOpened(BufferID buffer) {
    if (!buffer) return;
    qDebug() << "[onBufferOpened] buffer:" << buffer;

    // Create editor for this buffer
    auto* editor = new ScintillaEditor(_tabWidget);
    qDebug() << "[onBufferOpened] editor created:" << editor;

    // Load text
    std::string text = app().getBufferText(buffer);
    qDebug() << "[onBufferOpened] text loaded, length:" << text.size();
    editor->setPlainText(QString::fromUtf8(text.c_str()));
    qDebug() << "[onBufferOpened] text set in editor";
    
    // Get file name
    QString title = "Untitled";
    auto name = app().getFileName(buffer);
    if (name && !name->empty()) {
        title = QString::fromStdString(*name);
    }
    
    // Add tab
    int index = _tabWidget->addTab(editor, title);
    _tabWidget->setCurrentIndex(index);
    
    // Map buffer to tab
    _bufferToTab[buffer] = index;
    
    // Connect editor signals
    connect(editor, &ScintillaEditor::textChanged, this, &MainWindow::onBufferChanged);
    connect(editor, &ScintillaEditor::modificationChanged, this, [this, buffer](bool modified) {
        onBufferModified(buffer, modified);
    });
    
    // Wire DocumentMap to the first editor (initial setup)
    if (app().documentMapPanel() && !app().getActiveEditor()) {
        app().documentMapPanel()->setEditor(editor);
    }

    updateTabBar();
    updateTitleBar();
    updateStatusBar();

    // Register this editor as the active one
    app().setActiveEditor(editor);
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
    ScintillaEditor* ed = app().getActiveEditor();
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
    // Load theme colors from theme file and apply to toolbar and status bar
    ThemeManager* tm = app().themeManager();
    ThemeColors colors = tm ? tm->getThemeColors(theme) : ThemeColors();
    Q_UNUSED(colors);
    // Apply to toolbar
    if (_toolBar) {
        QString qss = tm ? tm->getThemeQss(theme, "toolbar") : QString();
        if (!qss.isEmpty()) _toolBar->setStyleSheet(qss);
    }
    // Apply to status bar
    if (_statusBar) {
        QString qss = tm ? tm->getThemeQss(theme, "statusbar") : QString();
        if (!qss.isEmpty()) _statusBar->setStyleSheet(qss);
    }
    // Reload current theme resource
    app().loadTheme(theme.toStdString());
}
// Tab events
void MainWindow::onTabChanged(int index) {
    if (index >= 0) {
        BufferID buffer = bufferAtTabIndex(index);
        if (buffer) app().setActiveBuffer(buffer);
        ScintillaEditor* ed = editorAt(index);
        if (ed) app().setActiveEditor(ed);

        // Wire DocumentMap to the newly active editor
        if (app().documentMapPanel()) {
            app().documentMapPanel()->setEditor(ed);
            app().documentMapPanel()->onBufferChanged();
        }
    }
    updateStatusBar();
    updateTitleBar();
}

void MainWindow::onTabCloseRequested(int index) {
    if (index < 0 || index >= _tabWidget->count()) return;

    BufferID closingBuffer = bufferAtTabIndex(index);
    ScintillaEditor* editor = editorAt(index);

    // Save prompt if modified
    if (editor && editor->isModified()) {
        QString title = _tabWidget->tabText(index);
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Save Changes",
            QString("Do you want to save changes to %1?").arg(title),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Save) {
            // Sync and save: switch to this tab, then save
            app().setActiveEditor(editor);
            app().setActiveBuffer(closingBuffer);
            if (closingBuffer != BUFFER_INVALID) app().syncEditorToBuffer(editor, closingBuffer);
            QString path = app().fileManager()->getBufferPath(closingBuffer);
            if (!path.isEmpty()) app().saveFile(closingBuffer, path.toStdString());
            // If no path (new file), fall through to save-as
            if (path.isEmpty() || app().saveFile(closingBuffer, path.toStdString())) {
                // saved
            }
        } else if (btn == QMessageBox::Cancel) {
            return;
        }
    }

    // Close buffer and remove tab (removeEditorTab handles the map)
    if (closingBuffer != BUFFER_INVALID) app().closeFile(closingBuffer);
    removeEditorTab(index, closingBuffer);
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
