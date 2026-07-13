// Semantic lift: Main window (Notepad_plus_Window → QMainWindow)
// Original: PowerEditor/src/Notepad_plus_Window.h
// Target: npp-qt/src/MainWindow.h

#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTabWidget>
#include <QSyntaxHighlighter>
#include <QLabel>
#include <QSettings>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QApplication>
#include <QStyleFactory>

#include "WinControls/TabBar.h"
#include "FindReplaceDlg.h"
#include "NppCommands.h"
#include "NppDarkMode.h"     // QRgb, NppDarkMode::getDlgBackgroundColor()
#include "Parameters.h"      // CmdLineParams, NppParameters, NppGUI, LangType
#include "WinControls/RunMacroDlg.h"  // RunMacroDlg

// Forward declarations — Notepad_plus.h includes MainWindow.h (circular include)
// class Buffer;
// class ScintillaEdit;
// class RunMacroDlg;

// QuoteParams struct (needed by MainWindow member; defined here to avoid
// pulling in all of Notepad_plus.h just for this small struct)
struct QuoteParams
{
    enum Speed { slow = 0, rapid, speedOfLight };

    QuoteParams() {}
    QuoteParams(const wchar_t*, Speed sp, bool, int encoding, LangType lang, const wchar_t* quote) :
        _encoding(encoding), _lang(lang), _quote(quote) { _speed = sp; }

    void reset() {
        _quoter = nullptr;
        _speed = rapid;
        _shouldBeTrolling = false;
        _encoding = CP_UTF8;
        _lang = L_TEXT;
        _quote = nullptr;
    }

    const wchar_t* _quoter = nullptr;
    Speed _speed = rapid;
    bool _shouldBeTrolling = false;
    int _encoding = CP_UTF8;
    LangType _lang = L_TEXT;
    const wchar_t* _quote = nullptr;
};

// Forward declarations
class Buffer;
class ScintillaEdit;
class RunMacroDlg;

// Main window class - semantic lift from Notepad_plus_Window
// In Qt6: QMainWindow ← MainWindow ← Notepad_plus
// Notepad_plus_Window is an alias for MainWindow (see Notepad_plus_Window.h)
class MainWindow : public QMainWindow
{
    Q_OBJECT

    // Windows API compatibility — _hSelf holds the window handle (this on Qt)
    QWidget* _hSelf = nullptr;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // Static handle to the Notepad++ main window (Qt6 port of Notepad_plus_Window::gNppHWND)
    static QWidget* gNppHWND;

    // Show the Run Macro dialog (called from NppCommands::macroRunMultiple)
    void showRunMacroDialog() { if (_runMacroDlg) _runMacroDlg->doDialog(); }

    // Static class name (for compatibility; Qt uses objectName/objectName())
    static const char* getClassName() { return "Notepad_plus"; }

    // Win32 compatibility: HINSTANCE (module handle). Qt6 doesn't use HINSTANCE.
    void* getHinst() const { return nullptr; }

    // Load tray icon icon from Qt resources (mirrors N++ Notepad_plus_Window::loadTrayIcon)
    // On Win32: LoadIcon(hinst, MAKEINTRESOURCE(IDI_M30ICON))
    // On Qt6: load from application resource or use window icon
    static void loadTrayIcon(QApplication*, QIcon* icon) {
        if (icon) {
            // Try to load npp-qt icon from resources; fall back to application icon
            if (QIcon::hasThemeIcon("accessories-text-editor")) {
                *icon = QIcon::fromTheme("accessories-text-editor");
            } else {
                *icon = qApp->windowIcon();
            }
        }
    }

    // getHSelf() / getHinst() compatibility (Window.h interface for sub-components)
    QWidget* getHSelf() { return this; }
    const QWidget* getHSelf() const { return this; }
    QApplication* getHinstCompat() const { return qApp; }

    // ── Notepad_plus_Window::init() translation ──────────────────────────────
    // Signature mirrors original: HINSTANCE/HWND dropped (Qt6 has no such types)
    void init(HINSTANCE /*hInst*/, HWND /*parent*/, const wchar_t* cmdLine, CmdLineParams* cmdLineParams);
    void setStartupBgColor(QRgb bgColor);
    bool isDlgsMsg(QEvent* msg) const;

    void init(); // lightweight init (called without params)

    void enableDarkMode(bool enable = true);
    bool isDarkModeEnabled() const { return _isDarkMode; }

signals:
    void documentChanged(int index);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool event(QEvent* event) override;

    // Windows message / Qt event handler dispatch (NppBigSwitch.cpp)
    qintptr Notepad_plus_Proc(QWidget* hwnd, unsigned int message, quintptr wParam, qintptr lParam);
    qintptr runProc(QWidget* hwnd, unsigned int message, quintptr wParam, qintptr lParam);

    // Buffer access — used by Notepad_plus to iterate/close buffers
    Buffer* getBufferFromIndex(int index) const;
    void closeFile(int index);

    // File operations — used by Notepad_plus implementations
    void newFile();
    void openFile();
    void openFile(const QString& path);
    void saveFile();
    void saveFileAs();
    void closeFile();  // close current tab

private slots:
    // File operations (new/open/save/close) are declared in protected section
    void exit();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void find();
    void replace();

    void toggleFullScreen();
    void toggleWordWrap();
    void zoomIn();
    void zoomOut();
    void resetZoom();

    void onTabChanged(int index);
    void onTabCloseRequested(int index);

    // Scintilla notification signal handlers — bridge SCN_* to Qt6 slots
    void onScintillaModified(ScintillaComponent* editor, int type,
                              size_t position, size_t length,
                              const QString& text, size_t line);
    void onScintillaSavePointChanged(ScintillaComponent* editor, bool dirty);
    void onScintillaUpdateUI(ScintillaComponent* editor, int updated);
    void onScintillaCharAdded(ScintillaComponent* editor, int ch);
    void onScintillaMarginClicked(ScintillaComponent* editor,
                                   size_t position, int modifiers, int margin);
    void onScintillaDoubleClicked(ScintillaComponent* editor,
                                   size_t position, size_t line);
    void onScintillaZoomChanged(ScintillaComponent* editor, int zoom);
    void onScintillaPainted(ScintillaComponent* editor);
    void onScintillaModifyAttemptRO(ScintillaComponent* editor);

    void updateStatusBar();
    void updateTabBar();

    // NppCommands command dispatcher — allows NppCommands to route IDM_* via MainWindow
    void command(int cmdId);
    // Active view accessor — used by NppCommands to get the current editor
    ScintillaEditView* getActiveView();

public slots:
    // Language menu action — sets the lexer for the current editor tab
    void setLanguage(QAction* action);
    void setLanguage(LangType lang);

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createTabBar();
    void createDocumentPane();
    void createPanels();

    void loadSettings();
    void saveSettings();
    void loadSession();
    void saveSession();

    Buffer* createNewBuffer();
    ScintillaComponent* currentScintilla() const;

    void applyDarkModeStyle();
    void applyLightModeStyle();

    bool _isDarkMode = false;
    bool _isFullScreen = false;
    int _currentZoomFactor = 100;

    RunMacroDlg* _runMacroDlg = nullptr;

    TabBarPlus* _pTabBar = nullptr;
    QTabWidget* _pDocTabBar = nullptr;

    QToolBar* _pFileToolBar = nullptr;
    QToolBar* _pEditToolBar = nullptr;
    QToolBar* _pViewToolBar = nullptr;
    QToolBar* _pSearchToolBar = nullptr;

    // NppCommands — bridges all menu actions to command handlers
protected:
    NppCommands* _pNppCommands = nullptr;

    // Toolbar actions (owned by this class, created in createActions())
    QAction* _pActNew = nullptr;
    QAction* _pActOpen = nullptr;
    QAction* _pActSave = nullptr;
    QAction* _pActUndo = nullptr;
    QAction* _pActRedo = nullptr;
    QAction* _pActCut = nullptr;
    QAction* _pActCopy = nullptr;
    QAction* _pActPaste = nullptr;
    QAction* _pActFind = nullptr;
    QAction* _pActReplace = nullptr;

    // File menu named actions
    QAction* _pActSaveAs = nullptr;
    QAction* _pActReload = nullptr;
    QAction* _pActClose = nullptr;
    QAction* _pActCloseAll = nullptr;
    QAction* _pActCloseAllButCurrent = nullptr;
    QAction* _pActPrint = nullptr;
    QAction* _pActExit = nullptr;

    // Edit menu named actions
    QAction* _pActSelectAll = nullptr;
    QAction* _pActDelete = nullptr;
    QAction* _pActUndoCut = nullptr;

    // Search menu named actions
    QAction* _pActFindNext = nullptr;
    QAction* _pActFindPrev = nullptr;
    QAction* _pActGoToLine = nullptr;

    // View menu named actions
    QAction* _pActFullScreen = nullptr;
    QAction* _pActWordWrap = nullptr;
    QAction* _pActZoomIn = nullptr;
    QAction* _pActZoomOut = nullptr;
    QAction* _pActZoomRestore = nullptr;
    QAction* _pActShowEol = nullptr;
    QAction* _pActShowIndentGuide = nullptr;
    QAction* _pActShowAllChars = nullptr;

    // Encoding menu named actions
    QAction* _pActEncAnsi = nullptr;
    QAction* _pActEncUTF8 = nullptr;
    QAction* _pActEncUTF8BOM = nullptr;
    QAction* _pActEncUTF16LE = nullptr;
    QAction* _pActEncUTF16BE = nullptr;
    QAction* _pActEncConvAnsi = nullptr;
    QAction* _pActEncConvUTF8 = nullptr;
    QAction* _pActEncConvUTF8BOM = nullptr;
    QAction* _pActEncConvUTF16LE = nullptr;
    QAction* _pActEncConvUTF16BE = nullptr;

    // Settings menu named actions
    QAction* _pActPreferences = nullptr;
    QAction* _pActStyleConfig = nullptr;
    QAction* _pActShortcutMapper = nullptr;

    // Macro menu named actions
    QAction* _pActMacroStartRecord = nullptr;
    QAction* _pActMacroStopRecord = nullptr;
    QAction* _pActMacroPlay = nullptr;

    // Run menu named actions
    QAction* _pActRunExec = nullptr;

    QMenuBar* _pMenuBar = nullptr;
    QMenu* _langMenu = nullptr;
    QStatusBar* _pStatusBar = nullptr;

    QDockWidget* _pFileBrowserDock = nullptr;
    QDockWidget* _pFunctionListDock = nullptr;
    QDockWidget* _pDocMapDock = nullptr;
    QDockWidget* _pProjectPanelDock = nullptr;
    QDockWidget* _pPluginsAdminDock = nullptr;

    FindReplaceDlg* _pFindReplaceDlg = nullptr;

    QLabel* _pPositionLabel = nullptr;
    QLabel* _pLengthLabel = nullptr;
    QLabel* _pEncodingLabel = nullptr;
    QLabel* _pEolLabel = nullptr;
    QLabel* _pLanguageLabel = nullptr;

    Buffer* _pCurrentBuffer = nullptr;
    QVector<Buffer*> _buffers;
    // Scintilla editor widgets, one per document tab
    QVector<ScintillaComponent*> _scintillaEditors;

    QSettings _settings;

    // ── Members from original Notepad_plus_Window ───────────────────────────
    // Static class name (matches original: L"Notepad++")
    static constexpr wchar_t _className[32] = L"Notepad++";
    bool _isPrelaunch = false;
    // Tray icon handle (Qt6: unused — we use QSystemTrayIcon in Notepad_plus)
    void* _hIconAbsent = nullptr;
    // Quote parameters for easter egg / ghost typing feature
    QuoteParams _quoteParams;
    std::wstring _userQuote;

    friend class NppParameters;
};
