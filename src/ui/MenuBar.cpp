// MenuBar.cpp - Application menu bar implementation
#include "../core/Application.h"
// Copyright (C) 2026 Agent Army
// GPL v3

#include "MenuBar.h"
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QStyle>
#include "../core/RecentFilesManager.h"
#include <QApplication>
#include <QShortcut>

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent) {
    buildMenus();
}

MenuBar::~MenuBar() = default;

void MenuBar::buildMenus() {
    // File menu
    _fileMenu = addMenu("&File");
    
    QAction* newAction = new QAction("&New", this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setData("file.new");
    newAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    _fileMenu->addAction(newAction);
    
    QAction* openAction = new QAction("&Open...", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setData("file.open");
    openAction->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    _fileMenu->addAction(openAction);
    
    _fileMenu->addSeparator();

    // Recent Files submenu (populated from RecentFilesManager)
    _recentFilesMenu = _fileMenu->addMenu("Recent Files");
    RecentFilesManager::instance().populateMenu(_recentFilesMenu);
    // Rebuild menu when files change
    QObject::connect(&RecentFilesManager::instance(), &RecentFilesManager::menuNeedsRebuild,
                     _recentFilesMenu, [this]() {
        RecentFilesManager::instance().populateMenu(_recentFilesMenu);
    });
    // Open file when recent action triggered
    QObject::connect(_recentFilesMenu, &QMenu::triggered,
                     this, [](QAction* action) {
        QString path = action->data().toString();
        if (!path.isEmpty())
            Application::instance().openFile(path.toStdString());
    });

    _fileMenu->addSeparator();
    
    QAction* saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setData("file.save");
    saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    _fileMenu->addAction(saveAction);
    
    QAction* saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setData("file.saveAs");
    _fileMenu->addAction(saveAsAction);
    
    QAction* saveAllAction = new QAction("Save A&ll", this);
    saveAllAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    saveAllAction->setData("file.saveAll");
    _fileMenu->addAction(saveAllAction);
    
    _fileMenu->addSeparator();
    
    QAction* closeAction = new QAction("&Close", this);
    closeAction->setShortcut(QKeySequence("Ctrl+W"));
    closeAction->setData("file.close");
    _fileMenu->addAction(closeAction);
    
    QAction* closeAllAction = new QAction("Clos&e All", this);
    closeAllAction->setData("file.closeAll");
    _fileMenu->addAction(closeAllAction);
    
    _fileMenu->addSeparator();
    
    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence("Alt+F4"));
    exitAction->setData("file.exit");
    _fileMenu->addAction(exitAction);
    
    // Edit menu
    _editMenu = addMenu("&Edit");
    
    QAction* undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setData("edit.undo");
    undoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    _editMenu->addAction(undoAction);
    
    QAction* redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setData("edit.redo");
    redoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    _editMenu->addAction(redoAction);
    
    _editMenu->addSeparator();
    
    QAction* cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setData("edit.cut");
    cutAction->setIcon(QIcon::fromTheme("edit-cut", style()->standardIcon(QStyle::SP_TitleBarMinButton)));
    _editMenu->addAction(cutAction);
    
    QAction* copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setData("edit.copy");
    copyAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    _editMenu->addAction(copyAction);
    
    QAction* pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setData("edit.paste");
    pasteAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    _editMenu->addAction(pasteAction);
    
    QAction* deleteAction = new QAction("&Delete", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setData("edit.delete");
    _editMenu->addAction(deleteAction);
    
    _editMenu->addSeparator();
    
    QAction* selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setData("edit.selectAll");
    _editMenu->addAction(selectAllAction);
    
    _editMenu->addSeparator();
    
    QAction* findAction = new QAction("&Find...", this);
    findAction->setShortcut(QKeySequence::Find);
    findAction->setData("edit.find");
    findAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    _editMenu->addAction(findAction);
    
    QAction* replaceAction = new QAction("&Replace...", this);
    replaceAction->setShortcut(QKeySequence("Ctrl+H"));
    replaceAction->setData("edit.replace");
    _editMenu->addAction(replaceAction);
    
    _editMenu->addSeparator();
    
    QAction* gotoAction = new QAction("&Go to Line...", this);
    gotoAction->setShortcut(QKeySequence("Ctrl+G"));
    gotoAction->setData("edit.goto");
    _editMenu->addAction(gotoAction);
    
    // Search menu
    _searchMenu = addMenu("&Search");
    
    QAction* findNextAction = new QAction("Find &Next", this);
    findNextAction->setShortcut(QKeySequence::FindNext);
    findNextAction->setData("search.findNext");
    _searchMenu->addAction(findNextAction);
    
    QAction* findPrevAction = new QAction("Find &Previous", this);
    findPrevAction->setShortcut(QKeySequence("Shift+F3"));
    findPrevAction->setData("search.findPrev");
    _searchMenu->addAction(findPrevAction);
    
    _searchMenu->addSeparator();
    
    QAction* findInFilesAction = new QAction("Find in Fi&les...", this);
    findInFilesAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    findInFilesAction->setData("search.findInFiles");
    _searchMenu->addAction(findInFilesAction);
    
    QAction* incSearchAction = new QAction("&Incremental Search", this);
    incSearchAction->setShortcut(QKeySequence("Ctrl+I"));
    incSearchAction->setData("search.incrementalSearch");
    _searchMenu->addAction(incSearchAction);
    
    _searchMenu->addSeparator();
    
    QAction* countAction = new QAction("&Count", this);
    countAction->setData("search.count");
    _searchMenu->addAction(countAction);
    
    QAction* markAllAction = new QAction("Mark &All", this);
    markAllAction->setData("search.markAll");
    _searchMenu->addAction(markAllAction);
    
    // View menu
    _viewMenu = addMenu("&View");
    
    QAction* darkModeAction = new QAction("&Dark Mode", this);
    darkModeAction->setCheckable(true);
    darkModeAction->setChecked(true);  // default dark
    darkModeAction->setData("view.darkMode");
    _viewMenu->addAction(darkModeAction);
    
    QAction* lightModeAction = new QAction("&Light Mode", this);
    lightModeAction->setCheckable(true);
    lightModeAction->setChecked(false);
    lightModeAction->setData("view.lightMode");
    _viewMenu->addAction(lightModeAction);
    
    // Make dark/light mutually exclusive
    QObject::connect(darkModeAction, &QAction::toggled,
                     lightModeAction, &QAction::setChecked, Qt::UniqueConnection);
    QObject::connect(lightModeAction, &QAction::toggled,
                     darkModeAction, &QAction::setChecked, Qt::UniqueConnection);
    
    _viewMenu->addSeparator();
    
    QAction* fullScreenAction = new QAction("Toggle F&ull Screen", this);
    fullScreenAction->setShortcut(QKeySequence("F11"));
    fullScreenAction->setData("view.fullScreen");
    _viewMenu->addAction(fullScreenAction);
    
    QAction* distractionFreeAction = new QAction("Toggle &Distraction-Free Mode", this);
    distractionFreeAction->setShortcut(QKeySequence("F12"));
    distractionFreeAction->setData("view.distractionFree");
    _viewMenu->addAction(distractionFreeAction);
    
    _viewMenu->addSeparator();
    
    QAction* showTabBarAction = new QAction("Show &Tab Bar", this);
    showTabBarAction->setCheckable(true);
    showTabBarAction->setChecked(true);
    showTabBarAction->setData("view.showTabBar");
    _viewMenu->addAction(showTabBarAction);
    
    QAction* showStatusBarAction = new QAction("Show &Status Bar", this);
    showStatusBarAction->setCheckable(true);
    showStatusBarAction->setChecked(true);
    showStatusBarAction->setData("view.showStatusBar");
    _viewMenu->addAction(showStatusBarAction);
    
    QAction* showToolBarAction = new QAction("Show &Tool Bar", this);
    showToolBarAction->setCheckable(true);
    showToolBarAction->setChecked(true);
    showToolBarAction->setData("view.showToolBar");
    _viewMenu->addAction(showToolBarAction);
    
    _viewMenu->addSeparator();
    
    QAction* fileBrowserAction = new QAction("&File Browser", this);
    fileBrowserAction->setCheckable(true);
    fileBrowserAction->setChecked(true);
    fileBrowserAction->setData("view.fileBrowser");
    _viewMenu->addAction(fileBrowserAction);
    
    QAction* funcListAction = new QAction("F&unction List", this);
    funcListAction->setCheckable(true);
    funcListAction->setChecked(true);
    funcListAction->setData("view.functionList");
    _viewMenu->addAction(funcListAction);
    
    QAction* docMapAction = new QAction("&Document Map", this);
    docMapAction->setCheckable(true);
    docMapAction->setChecked(true);
    docMapAction->setData("view.documentMap");
    _viewMenu->addAction(docMapAction);
    
    // Encoding menu
    _encodingMenu = addMenu("&Encoding");
    
    QAction* convUtf8Action = new QAction("Convert to &UTF-8", this);
    convUtf8Action->setData("encoding.utf8");
    _encodingMenu->addAction(convUtf8Action);
    
    QAction* convUtf8BomAction = new QAction("Convert to UTF-8 &BOM", this);
    convUtf8BomAction->setData("encoding.utf8bom");
    _encodingMenu->addAction(convUtf8BomAction);
    
    QAction* convUtf16LeAction = new QAction("Convert to UTF-16 LE", this);
    convUtf16LeAction->setData("encoding.utf16le");
    _encodingMenu->addAction(convUtf16LeAction);
    
    QAction* convUtf16BeAction = new QAction("Convert to UTF-16 BE", this);
    convUtf16BeAction->setData("encoding.utf16be");
    _encodingMenu->addAction(convUtf16BeAction);
    
    _encodingMenu->addSeparator();
    
    // Character Sets submenu
    QMenu* charSetsMenu = _encodingMenu->addMenu("Character Sets");
    QStringList encodings = {
        "Western European (ISO-8859-1)",
        "Central European (ISO-8859-2)",
        "Cyrillic (ISO-8859-5)",
        "Greek (ISO-8859-7)",
        "Turkish (ISO-8859-9)",
        "Hebrew (ISO-8859-8)",
        "Arabic (ISO-8859-6)",
        "Baltic (ISO-8859-13)",
        "Greek (Windows-1253)",
        "Cyrillic (Windows-1251)",
        "Western European (Windows-1252)",
        "Turkish (Windows-1254)"
    };
    for (const QString& enc : encodings) {
        QAction* encAction = charSetsMenu->addAction(enc);
        encAction->setData("encoding.charset." + enc);
    }
    
    // Language menu
    _languageMenu = addMenu("&Language");
    
    // Populate language menu with common languages
    QStringList languages = {
        "Normal Text",
        "C", "C++", "C#", "Java", "JavaScript",
        "HTML", "XML", "XAML",
        "PHP", "Python", "Ruby", "Perl", "Lua",
        "CSS", "JSON", "YAML",
        "Makefile", "Batch", "INI", "Markdown"
    };
    
    QMenu* progLangMenu = _languageMenu->addMenu("Programming Languages");
    for (const QString& lang : languages) {
        if (lang == "Normal Text" || lang == "HTML") {
            QAction* langAction = _languageMenu->addAction(lang);
            langAction->setData("language." + lang.toLower().replace(' ', '_'));
        } else {
            QAction* langAction = progLangMenu->addAction(lang);
            langAction->setData("language." + lang.toLower().replace(' ', '_'));
        }
    }
    
    // Settings menu
    _settingsMenu = addMenu("Se&ttings");
    
    QAction* prefsAction = new QAction("&Preferences...", this);
    prefsAction->setShortcut(QKeySequence("Ctrl+Alt+P"));
    prefsAction->setData("settings.preferences");
    _settingsMenu->addAction(prefsAction);
    
    QAction* shortcutMapperAction = new QAction("&Shortcut Mapper...", this);
    shortcutMapperAction->setData("settings.shortcutMapper");
    _settingsMenu->addAction(shortcutMapperAction);
    
    _settingsMenu->addSeparator();
    
    QAction* commandPaletteAction = new QAction("&Command Palette...", this);
    commandPaletteAction->setShortcut(QKeySequence("Ctrl+Shift+P"));
    commandPaletteAction->setData("settings.commandPalette");
    _settingsMenu->addAction(commandPaletteAction);
    
    // Help menu
    _helpMenu = addMenu("&Help");
    
    QAction* aboutAction = new QAction("&About", this);
    aboutAction->setData("help.about");
    _helpMenu->addAction(aboutAction);
    
    // Connect all actions to emit menuCommand signal
    for (QMenu* menu : {_fileMenu, _editMenu, _searchMenu, _viewMenu, _encodingMenu, _languageMenu, _settingsMenu, _helpMenu}) {
        for (QAction* action : menu->actions()) {
            if (!action->data().isNull()) {
                connect(action, &QAction::triggered, this, [this, action]() {
                    QString cmd = action->data().toString();
                    if (!cmd.isEmpty()) {
                        emit menuCommand(cmd);
                    }
                });
            }
        }
    }
    
    // Handle Open Recent submenu actions
    for (QAction* action : _recentFilesMenu->actions()) {
        if (!action->data().isNull()) {
            connect(action, &QAction::triggered, this, [this, action]() {
                QString cmd = action->data().toString();
                if (!cmd.isEmpty()) {
                    emit menuCommand(cmd);
                }
            });
        }
    }
    
    // Handle Character Sets submenu actions
    for (QAction* action : charSetsMenu->actions()) {
        if (!action->data().isNull()) {
            connect(action, &QAction::triggered, this, [this, action]() {
                QString cmd = action->data().toString();
                if (!cmd.isEmpty()) {
                    emit menuCommand(cmd);
                }
            });
        }
    }
    
    // Handle Language submenu actions
    for (QAction* action : progLangMenu->actions()) {
        if (!action->data().isNull()) {
            connect(action, &QAction::triggered, this, [this, action]() {
                QString cmd = action->data().toString();
                if (!cmd.isEmpty()) {
                    emit menuCommand(cmd);
                }
            });
        }
    }
}
