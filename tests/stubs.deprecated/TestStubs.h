// TestStubs.h — minimal declarations for test compilation
// Include this INSTEAD of the real headers (via -include in CMake)
// to avoid pulling in the full dependency graph.
// Real implementations are provided in TestStubs.cpp.
#pragma once
#include <QString>
#include <QVector>

// Forward declarations for types referenced by dialogs
class MenuBar;
class ScintillaEditor;

// Stub Application singleton + free app() accessor
class Application {
public:
    static Application& instance();
    Application();
    ~Application();

    struct AppOptions {
        bool showMenuBar = true;
        bool showTabBar = true;
        bool showToolBar = true;
        bool wrap = false;
    };
    AppOptions& options() { return _options; }
    const AppOptions& options() const { return _options; }
    class MainWindow* mainWindow() const { return nullptr; }
    class EditorCommandManager* commandManager() const { return nullptr; }
    ScintillaEditor* getActiveEditor() const { return nullptr; }
    void setActiveEditor(ScintillaEditor*) {}

private:
    AppOptions _options;
};
inline Application& app() { return Application::instance(); }

// Stub MainWindow
class MainWindow {
public:
    explicit MainWindow(Application*) {}
    ~MainWindow() = default;
    class MenuBar* menuBar() const { return nullptr; }
    template<typename T> QVector<T*> findChildren() const { return {}; }
};

// Stub FileManager
class FileManager {
public:
    FileManager();
    ~FileManager();
};

// Stub LanguageManager
class LanguageManager {
public:
    LanguageManager();
    ~LanguageManager();
};

// Stub SessionManager
class SessionManager {
public:
    SessionManager();
    ~SessionManager();
};

// Stub EditorCommandManager
class EditorCommandManager {
public:
    EditorCommandManager();
    ~EditorCommandManager();
    QString getShortcut(const QString&) const;
    void setShortcut(const QString&, const QString&);
};

// Stub MacroManager
class MacroManager {
public:
    MacroManager();
    ~MacroManager();
};

// Stub RecentFilesManager
class RecentFilesManager {
public:
    static RecentFilesManager& instance();
    RecentFilesManager();
    ~RecentFilesManager();
};

// Stub MenuBar
class MenuBar {
public:
    MenuBar() = default;
    ~MenuBar() = default;
    void setVisible(bool) {}
};

// Stub ScintillaEditor (partial — enough for PreferenceDialog stubs)
class ScintillaEditor {
public:
    ScintillaEditor() = default;
    ~ScintillaEditor() = default;
    using sptr_t = long;
    sptr_t SendScintilla(int, int = 0, int = 0) { return 0; }
    void setTabWidth(int) {}
    void setIndentWidth(int) {}
    void setUseTabs(bool) {}
    void setWrapMode(bool) {}
    void setMarginLineNumbers(int, bool) {}
    void setVirtualSpaceOptions(bool) {}
    void setHomeKeyNavigation(bool) {}
    void setAutoIndent(bool) {}
};
