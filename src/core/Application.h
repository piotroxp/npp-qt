// Application.h - Main application controller for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QObject>
#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QString>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "common/Types.h"
#include "common/Util.h"
#include "ThemeManager.h"

// Forward declarations
class ScintillaEditor;
class Buffer;
class FileManager;
class EncodingDetector;
class LanguageManager;
class SessionManager;
class MacroManager;
class RecentFilesManager;
class EditorCommandManager;
class MainWindow;
class SyntaxHighlighter;
class FindReplaceDialog;
class GoToLineDialog;
class PreferenceDialog;
class AboutDialog;
class DocumentMapPanel;
class FunctionListPanel;
class FileBrowserPanel;
class MenuBar;
class ToolBar;

// ============================================================================
// Command line options
// ============================================================================
struct AppOptions {
    std::string  configDir;
    std::string  pluginDir;
    std::string  themeProfile = "default";
    bool         singleInstance = true;
    bool         showTabBar = true;
    bool         showStatusBar = true;
    bool         showToolBar = true;
    bool         showMenuBar = true;
    bool         rememberSession = true;
    int          maxRecentFiles = 50;
    int          defaultTabWidth = 4;
    bool         defaultTabAsSpaces = false;
    EolType      defaultEolType = EolType::EOL_LF;
    EncodingType defaultEncoding = EncodingType::UTF_8_BOM;
    LangType     defaultLang = LangType::L_TEXT;
    int          autoDetectionTimeout = 500;  // ms
    bool         smartHighlighting = true;
    int          maxHighlightingWords = 1000;
    bool         wrapWithQuotes = false;

    // File Associations
    QStringList   fileAssociations;

    // Backup / Auto-Save
    bool         autoSave = false;
    int          autoSaveInterval = 5;
    bool         autoSaveCurrentOnly = false;
    bool         autoSaveInBackground = false;
    std::string  backupDir;
    int          backupStyle = 0;        // 0=simple, 1=datetime, 2=numbered
    int          maxBackups = 5;

    // Margins / Editor Display
    bool         showLineNumbers = true;
    int          lineNumberWidth = 40;
    bool         showSymbols = true;
    bool         showFolderMargin = true;
    int          symbolMarginWidth = 14;
    bool         highlightCurrentLine = true;
    bool         showEdgeLine = false;
    int          edgeColumn = 80;

    // Shortcut Mapper
    bool         warnOnShortcutConflict = true;
};

// ============================================================================
// Application state
// ============================================================================
enum class AppState : uint8_t {
    Starting = 0,
    Ready = 1,
    Closing = 2,
    Error = 3
};

// ============================================================================
// Notification types
// ============================================================================
struct BufferNotification {
    BufferID buffer;
    int flags;
};
struct FileNotification {
    std::string path;
    int event;  // watch event type
};
struct ThemeNotification {
    std::string themeName;
};

// ============================================================================
// Application singleton
// ============================================================================
class Application : public QObject, public NonCopyable {
    Q_OBJECT

public:
    static Application& instance();

    // Initialization / Shutdown
    bool initialize();
    void shutdown();
    bool isInitialized() const { return _state == AppState::Ready; }
    AppState state() const { return _state; }
    QString lastError() const { return _lastError; }

    // Configuration
    void setConfigDirectory(const std::string& dir);
    void setPluginDirectory(const std::string& dir);
    void setThemeProfile(const std::string& profile);
    bool saveConfig(const std::string& path = "");
    const AppOptions& options() const { return _options; }
    AppOptions& options() { return _options; }

    // File operations
    BufferID openFile(const std::string& path, bool readOnly = false);
    BufferID openFiles(const std::vector<std::string>& paths);
    bool saveFile(BufferID buffer, const std::string& path = "");
    bool saveAllFiles();
    bool closeFile(BufferID buffer, bool force = false);
    bool closeAllFiles();
    BufferID newFile(const std::string& encoding = "");
    BufferID duplicateBuffer(BufferID buffer);
    std::optional<std::string> getFileName(BufferID buffer) const;

    // Buffer management
    BufferID getActiveBuffer() const;
    BufferID getBufferAt(int index, int view = 0) const;
    int getBufferCount() const;
    void setActiveBuffer(BufferID buffer);
    bool isBufferModified(BufferID buffer) const;
    std::string getBufferText(BufferID buffer) const;
    bool setBufferText(BufferID buffer, const std::string& text);
    EncodingType getBufferEncoding(BufferID buffer) const;
    bool setBufferEncoding(BufferID buffer, EncodingType enc);

    // View management
    int currentView() const;
    void switchToView(int viewId);
    int addView();
    void closeView(int viewId);
    int activeViewId() const { return _activeViewId; }

    // Session
    bool loadSession(const std::string& path);
    bool saveSession(const std::string& path = "");
    void addToRecentFiles(const std::string& path);
    std::vector<std::string> getRecentFiles() const;
    void clearRecentFiles();

    // Main window
    void showMainWindow();
    void hideMainWindow();
    void toggleFullScreen();
    void toggleDistractionFree();
    bool isFullScreen() const { return _isFullScreen; }
    bool isDistractionFree() const { return _isDistractionFree; }
    QMainWindow* mainWindow() const { return _mainWindow; }

    // Dialogs
    FindReplaceDialog* findReplaceDialog() const { return _findReplaceDialog; }
    GoToLineDialog* gotoLineDialog() const { return _gotoLineDialog; }
    PreferenceDialog* preferenceDialog() const { return _preferenceDialog; }
    AboutDialog* aboutDialog() const { return _aboutDialog; }

    // Panels
    DocumentMapPanel* documentMapPanel() const { return _docMapPanel; }
    FunctionListPanel* functionListPanel() const { return _funcListPanel; }
    FileBrowserPanel* fileBrowserPanel() const { return _fileBrowserPanel; }

    // Editors
    ScintillaEditor* getEditor(int viewId = -1) const;
    ScintillaEditor* getActiveEditor() const;
    bool syncEditorToBuffer(ScintillaEditor* editor, BufferID buffer);
    void setActiveEditor(ScintillaEditor* editor);

    // Command system
    void executeCommand(const std::string& cmd);
    void executeCommand(int commandId);
    EditorCommandManager* commandManager() const { return _commandManager; }
    MacroManager* macroManager() const { return _macroManager; }
    RecentFilesManager* recentFilesManager() const { return _recentFilesManager; }

    // Notifications / observers
    Observer<BufferNotification>& bufferObserver() { return _bufferObserver; }
    Observer<FileNotification>& fileWatcherObserver() { return _fileWatcherObserver; }
    Observer<ThemeNotification>& themeObserver() { return _themeObserver; }

    // Status bar
    void setStatusBarText(const std::string& text);
    void setStatusBarEncoding(const std::string& enc);
    void setStatusBarEol(const std::string& eol);
    void setStatusBarLanguage(const std::string& lang);
    void setStatusBarPosition(int line, int col);
    void setStatusBarSelection(int selStart, int selEnd);

    // Encoding detection
    EncodingDetector* encodingDetector() const { return _encodingDetector; }

    // Language management
    LanguageManager* languageManager() const { return _languageManager; }

    // File management
    FileManager* fileManager() const { return _fileManager; }
    void closeAllBuffersExcept(BufferID keepId);

    // Encoding conversion
    void convertEncoding(EncodingType enc);

    // Language
    void setLanguage(LangType lang);

    // Theme
    void loadTheme(const std::string& themeName);
    std::string currentTheme() const { return _currentTheme; }
    ThemeManager* themeManager() { return _themeManager.get(); }

    // Clipboard history
    void addToClipboardHistory(const std::string& text);
    std::vector<std::string> getClipboardHistory() const;

public slots:
    void onBufferOpened(BufferID buffer);
    void onBufferActivated(BufferID buffer);
    void onBufferClosed(BufferID buffer);
    void onBufferChanged(BufferID buffer);
    void onMenuCommand(const QString& cmd);
    void onToolBarCommand(const QString& cmd);
    void onFileSaved(const std::string& path);
    void onFileModifiedExternally(const std::string& path);
    void onThemeChanged(const std::string& themeName);

    // File commands
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onSaveAll();
    void onCloseFile();
    void onCloseAll();
    void onExit();
    void onClearRecentFiles();

    // Edit commands
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    void onFind();
    void onReplace();
    void onGotoLine();

    // Search commands
    void onFindNext();
    void onFindPrev();
    void onFindInFiles();
    void onCount();
    void onMarkAll();

    // View commands
    void onToggleFullScreen();
    void onToggleDistractionFree();
    void onToggleTabBar();
    void onToggleStatusBar();
    void onToggleToolBar();

    // Encoding commands
    void onConvertEncoding(EncodingType enc);

    // Language commands
    void onSetLanguage(LangType lang);

    // Settings commands
    void onShowPreferences();
    void onShowShortcutMapper();
    void onShowCommandPalette();

    // Help commands
    void onShowAbout();

signals:
    void bufferOpened(BufferID buffer);
    void bufferClosed(BufferID buffer);
    void bufferActivated(BufferID buffer);
    void fileSaved(const std::string& path);
    void themeChanged(const std::string& themeName);
    void commandExecuted(int commandId);
    void activeEditorChanged(ScintillaEditor* editor);

protected:
    Application();
    ~Application() override;

private:
    bool loadConfig();
    bool setupDirectories();
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockingPanels();
    void setupConnections();
    void updateUI();

    AppState        _state = AppState::Starting;
    QString         _lastError;

    AppOptions      _options;
    std::string     _currentTheme = "default";
    bool            _isFullScreen = false;
    bool            _isDistractionFree = false;

    QMainWindow*    _mainWindow = nullptr;
    QWidget*        _centralWidget = nullptr;
    QStackedWidget* _viewStack = nullptr;

    // Core managers
    FileManager*            _fileManager = nullptr;
    MacroManager*           _macroManager = nullptr;
    RecentFilesManager*      _recentFilesManager = nullptr;
    EncodingDetector*       _encodingDetector = nullptr;
    LanguageManager*        _languageManager = nullptr;
    SessionManager*         _sessionManager = nullptr;
    EditorCommandManager*   _commandManager = nullptr;
    std::unique_ptr<ThemeManager> _themeManager;

    // Dialogs
    FindReplaceDialog*  _findReplaceDialog = nullptr;
    GoToLineDialog*     _gotoLineDialog = nullptr;
    PreferenceDialog*   _preferenceDialog = nullptr;
    AboutDialog*        _aboutDialog = nullptr;

    // Panels
    DocumentMapPanel*   _docMapPanel = nullptr;
    FunctionListPanel*  _funcListPanel = nullptr;
    FileBrowserPanel*   _fileBrowserPanel = nullptr;

    // UI components (typed for signal access)
    MenuBar*    _menuBar = nullptr;
    ToolBar*    _toolBar = nullptr;

    // Views
    std::vector<QWidget*>    _viewContainers;
    int                       _activeViewId = 0;
    ScintillaEditor*          _activeEditor = nullptr;

    // Clipboard
    std::vector<std::string> _clipboardHistory;
    static constexpr size_t MAX_CLIPBOARD_HISTORY = 50;

    // Observers
    Observer<BufferNotification> _bufferObserver;
    Observer<FileNotification>  _fileWatcherObserver;
    Observer<ThemeNotification>  _themeObserver;

    std::mutex _mutex;
};

// ============================================================================
// Inline singleton accessor
// ============================================================================
inline Application& app() { return Application::instance(); }
