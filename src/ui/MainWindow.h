// MainWindow.h - Main application window
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QSignalMapper>
#include <QMap>
#include <QString>
#include "../common/Types.h"

class ScintillaEditor;
class QTabWidget;
class QMenu;
class QAction;
class MenuBar;
class ToolBar;
class StatusBar;
class TabBar;
class FindReplaceDialog;
class PreferenceDialog;
class AboutDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

    void addEditorTab(ScintillaEditor* editor, const QString& title);
    void removeEditorTab(int index);
    void setTabText(int index, const QString& title);
    void setTabModified(int index, bool modified);

    int currentTabIndex() const;
    ScintillaEditor* currentEditor() const;
    ScintillaEditor* editorAt(int index) const;
    int editorCount() const;

    void updateTabBar();
    void updateStatusBar();
    void updateTitleBar(const QString& fileName = "");

    MenuBar* menuBar() const { return _menuBar; }
    ToolBar* toolBar() const { return _toolBar; }
    StatusBar* statusBarWidget() const { return _statusBarWidget; }

public slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onCloseFile();
    void onExit();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    void onFind();
    void onReplace();
    void onGoto();
    void onFindNext();
    void onFindPrevious();
    void onPreferences();
    void onAbout();
    void onFullScreen();
    void onDistractionFree();
    void onShowTabBar(bool show);
    void onShowStatusBar(bool show);
    void onShowToolBar(bool show);
    void onEncodingChanged(const QString& encoding);

    // Command dispatchers
    void onMenuCommand(const QString& cmd);
    void onToolBarCommand(const QString& cmd);

    // Buffer notifications
    void onBufferOpened(BufferID buffer);
    void onBufferActivated(BufferID buffer);
    void onBufferClosed(BufferID buffer);
    void onBufferModified(BufferID buffer, bool modified);

    // Theme
    void onThemeChanged(const QString& theme);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onBufferChanged();
    void onRecentFileSelected(const QString& file);

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createPanels();
    void setupConnections();
    void dispatchCommand(const QString& cmd);
    void updateRecentFilesMenu();

    QTabWidget* _tabWidget = nullptr;
    TabBar* _tabBar = nullptr;
    MenuBar* _menuBar = nullptr;
    ToolBar* _toolBar = nullptr;
    StatusBar* _statusBarWidget = nullptr;

    QDockWidget* _docMapDock = nullptr;
    QDockWidget* _funcListDock = nullptr;
    QDockWidget* _fileBrowserDock = nullptr;

    QMap<QString, QAction*> _actions;
    QString _lastOpenedDirectory;
    QSignalMapper* _recentFileMapper = nullptr;
    
    // Buffer to tab mapping
    QMap<BufferID, int> _bufferToTab;
};
