// MainWindow.h - Main application window
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QMainWindow>

class Application;  // forward declaration
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QPointer>
#include <QSignalMapper>
#include <QMap>
#include <QString>
#include <QKeyEvent>
#include "../common/Types.h"
#include "TabBar.h"
#include "../core/Application.h"
#include "../panels/FileBrowserPanel.h"
#include "../dialogs/ClipboardHistoryPanel.h"
#include "../panels/FunctionListPanel.h"
#include "../panels/DocumentMapPanel.h"

class ScintillaEditor;
class QMenu;
class QTabWidget;
class QMenu;
class QAction;
class MenuBar;
class ToolBar;
class StatusBar;
class TabBar;
class FindReplaceDialog;
class IncrementalSearchDialog;
class PreferenceDialog;
class AboutDialog;
class ShortcutAdapter;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    static MainWindow* instance() { return _instance; }

    explicit MainWindow(Application* app);
    ~MainWindow() override;

    void addEditorTab(ScintillaEditor* editor, const QString& title);
    void removeEditorTab(int index, BufferID closingBuffer = BUFFER_INVALID);
    void setTabText(int index, const QString& title);
    void setTabModified(int index, bool modified);
    void switchToTab(int index);  // 0-based; Ctrl+1..Ctrl+8 shortcuts

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
    IncrementalSearchDialog* incrementalSearch() const { return _incrementalSearch; }

public slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onCloseFile();
    void onExit();
    void openFileInTab(const QString& path);
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
    void onFileExternallyModified(const QString& filePath);

    // Theme
    void onThemeChanged(const QString& theme);

    // DPI
    void onDpiChanged(int dpi);
    void onShowAllCharacters();
    void onRun();
    void doPrint();

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onBufferChanged();
    void onRecentFileSelected(const QString& file);
    void onTabContextMenu(const QPoint& pos);
    void onTabMoved(int from, int to);
    void onTabBarClicked(int index);
    void onMacroCommand(const QString& macroName);

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
    void onShortcutMapper();
    void onCommandPalette();

    friend class ShortcutManager;

    QTabWidget* _tabWidget = nullptr;
    int _openingFileDepth = 0;  // call-depth counter: 0 = idle, 1 = first call (allowed), 2+ = re-entrant (blocked)
    QMetaObject::Connection _fileBrowserConnection;  // tracks fileDoubleClicked signal for disconnect/reconnect
    BufferID bufferAtTabIndex(int tabIndex) const;
    QPointer<TabBar> _tabBar = nullptr;
    MenuBar* _menuBar = nullptr;
    ToolBar* _toolBar = nullptr;
    StatusBar* _statusBarWidget = nullptr;

    QDockWidget* _docMapDock = nullptr;
    QDockWidget* _funcListDock = nullptr;
    QDockWidget* _fileBrowserDock = nullptr;
    FileBrowserPanel* _fileBrowserPanel = nullptr;
    ClipboardHistoryPanel* _clipboardHistoryPanel = nullptr;
    FunctionListPanel* _funcListPanel = nullptr;
    DocumentMapPanel* _docMapPanel = nullptr;
    IncrementalSearchDialog* _incrementalSearch = nullptr;

    // Keyboard shortcut adapter (Win32 RegisterHotKey → Qt6 QShortcut)
    ShortcutAdapter* _shortcutAdapter = nullptr;

    QMap<QString, QAction*> _actions;
    QString _lastOpenedDirectory;
    QSignalMapper* _recentFileMapper = nullptr;
    Application* _app = nullptr;

    // Editor registry — maps between tabs, buffers, and editors
    QMap<int, BufferID> _tabToBuffer;          // tab index → buffer
    static MainWindow* _instance;
    QMap<BufferID, ScintillaEditor*> _bufferToEditor; // buffer → editor
    QMap<BufferID, int> _bufferToTab;          // buffer → tab index

    // Update tab title to reflect modified state
    void updateTabTitle(BufferID buf, bool dirty);

    // Rebuild _tabToBuffer from _bufferToTab (call after tab reordering)
    void rebuildTabToBufferMap();
};
