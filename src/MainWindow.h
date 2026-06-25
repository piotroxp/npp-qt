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

// Forward declarations
class Buffer;
class ScintillaEdit;

// Main window class - semantic lift from Notepad_plus_Window
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // Static handle to the Notepad++ main window (Qt6 port of Notepad_plus_Window::gNppHWND)
    static QWidget* gNppHWND;

    // Static class name (for compatibility; Qt uses objectName/objectName())
    static const char* getClassName() { return "Notepad_plus"; }

    // Win32 compatibility: HINSTANCE (module handle). Qt6 doesn't use HINSTANCE.
    void* getHinst() const { return nullptr; }

    // Load tray icon (stub - QSystemTrayIcon in Qt)
    static void loadTrayIcon(QApplication*, QIcon*) { /* TODO: QSystemTrayIcon */ }

    // getHSelf() / getHinst() compatibility (Window.h interface for sub-components)
    const QWidget* getHSelf() const { return this; }
    QApplication* getHinstCompat() const { return qApp; }

    void init();

    void enableDarkMode(bool enable = true);
    bool isDarkModeEnabled() const { return _isDarkMode; }

signals:
    void documentChanged(int index);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void newFile();
    void openFile();
    void openFile(const QString& path);
    void saveFile();
    void saveFileAs();
    void closeFile();
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

    void updateStatusBar();
    void updateTabBar();

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
    Buffer* getBufferFromIndex(int index) const;

    void applyDarkModeStyle();
    void applyLightModeStyle();

    bool _isDarkMode = false;
    bool _isFullScreen = false;
    int _currentZoomFactor = 100;

    TabBarPlus* _pTabBar = nullptr;
    QTabWidget* _pDocTabBar = nullptr;

    QToolBar* _pFileToolBar = nullptr;
    QToolBar* _pEditToolBar = nullptr;
    QToolBar* _pViewToolBar = nullptr;
    QToolBar* _pSearchToolBar = nullptr;

    QMenuBar* _pMenuBar = nullptr;
    QStatusBar* _pStatusBar = nullptr;

    QDockWidget* _pFileBrowserDock = nullptr;
    QDockWidget* _pFunctionListDock = nullptr;
    QDockWidget* _pDocMapDock = nullptr;
    QDockWidget* _pProjectPanelDock = nullptr;
    QDockWidget* _pPluginsAdminDock = nullptr;

    QLabel* _pPositionLabel = nullptr;
    QLabel* _pLengthLabel = nullptr;
    QLabel* _pEncodingLabel = nullptr;
    QLabel* _pEolLabel = nullptr;
    QLabel* _pLanguageLabel = nullptr;

    Buffer* _pCurrentBuffer = nullptr;
    QVector<Buffer*> _buffers;

    QSettings _settings;

    friend class NppParameters;
};
