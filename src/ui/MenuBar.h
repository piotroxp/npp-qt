// MenuBar.h - Application menu bar with full Notepad++ functionality
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QMenuBar>
#include <QString>
#include <QMap>
#include <QMenu>
#include <QAction>

class MenuBar : public QMenuBar {
    Q_OBJECT
public:
    explicit MenuBar(QWidget* parent = nullptr);
    ~MenuBar() override;

    // Menu accessors
    QMenu* fileMenu() const { return _fileMenu; }
    QMenu* editMenu() const { return _editMenu; }
    QMenu* searchMenu() const { return _searchMenu; }
    QMenu* viewMenu() const { return _viewMenu; }
    QMenu* encodingMenu() const { return _encodingMenu; }
    QMenu* languageMenu() const { return _languageMenu; }
    QMenu* settingsMenu() const { return _settingsMenu; }
    QMenu* toolsMenu() const { return _toolsMenu; }
    QMenu* windowMenu() const { return _windowMenu; }
    QMenu* helpMenu() const { return _helpMenu; }
    
    // Action management
    QAction* actionForCommand(const QString& command) const;
    void setActionEnabled(const QString& command, bool enabled);
    void setActionChecked(const QString& command, bool checked);
    
    // Menu updates
    void updateRecentFilesMenu();
    void updateLanguageMenu(const QStringList& languages);
    void updateWindowMenu(const QStringList& openFiles);
    
    // Plugin support
    void addPluginMenu(const QString& pluginName, QMenu* menu);
    void removePluginMenu(const QString& pluginName);
    
    // Recent files
    void setMaxRecentFiles(int max);
    int maxRecentFiles() const { return _maxRecentFiles; }

Q_SIGNALS:
    void menuCommand(const QString& cmd);
    void recentFileClicked(const QString& filePath);
    void windowSwitchRequested(int index);

public slots:
    void onOpenRecentClear();
    void onOpenRecentFile(const QString& filePath);

protected:
    void rebuildRecentFilesMenu();

private:
    void buildMenus();
    void buildFileMenu();
    void buildEditMenu();
    void buildSearchMenu();
    void buildViewMenu();
    void buildEncodingMenu();
    void buildLanguageMenu();
    void buildSettingsMenu();
    void buildToolsMenu();
    void buildWindowMenu();
    void buildHelpMenu();
    void connectMenuActions(QMenu* menu);
    void addActionWithShortcut(QMenu* menu, const QString& text, const QString& command, 
                               const QKeySequence& shortcut, QIcon icon = QIcon());

    // Main menus
    QMenu* _fileMenu = nullptr;
    QMenu* _editMenu = nullptr;
    QMenu* _searchMenu = nullptr;
    QMenu* _viewMenu = nullptr;
    QMenu* _encodingMenu = nullptr;
    QMenu* _languageMenu = nullptr;
    QMenu* _settingsMenu = nullptr;
    QMenu* _toolsMenu = nullptr;
    QMenu* _windowMenu = nullptr;
    QMenu* _helpMenu = nullptr;
    
    // Submenus
    QMenu* _recentFilesMenu = nullptr;
    QMenu* _printMenu = nullptr;
    QMenu* _macroMenu = nullptr;
    QMenu* _runMenu = nullptr;
    QMenu* _panelMenu = nullptr;
    QMenu* _tabMenu = nullptr;
    QMenu* _zoomMenu = nullptr;
    QMenu* _exportMenu = nullptr;
    
    // Action map
    QMap<QString, QAction*> _actions;
    
    // Plugin menus
    QMap<QString, QMenu*> _pluginMenus;
    
    // State
    int _maxRecentFiles = 10;
    QStringList _recentFiles;
};
