// Parameters.h - Settings manager for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "common/Types.h"
#include <QObject>
#include <QSettings>
#include <QtGui/QFontDatabase>
#include <QString>
#include <QStringList>
#include <memory>
#include <vector>
#include <unordered_map>

// ============================================================================
// Forward declarations
// ============================================================================
class SessionManager;
class LanguageManager;

// ============================================================================
// Theme / Style definitions
// ============================================================================
struct NppStyle {
    QString name;
    int fgColor = 0;
    int bgColor = 0;
    int fontStyle = 0;        // bold=1, italic=2, underline=4
    int fontSize = 10;
    QString fontName;
    bool isEnabled = true;
};

struct NppTheme {
    QString name;
    QString path;
    QVector<NppStyle> styles;
};

// ============================================================================
// NppParameters - Central settings management (Singleton)
// ============================================================================
class NppParameters : public QObject, public NonCopyable {
    Q_OBJECT

public:
    static NppParameters& getInstance();

    // Initialization
    void load();
    void save();
    void resetToDefaults();

    // Accessors for key settings objects
    NppGUI& nppGUI() { return _nppGUI; }
    const NppGUI& nppGUI() const { return _nppGUI; }
    ScintillaViewParams& scintillaViewParams() { return _scintillaViewParams; }
    const ScintillaViewParams& scintillaViewParams() const { return _scintillaViewParams; }
    FindHistory& findHistory() { return _findHistory; }
    const FindHistory& findHistory() const { return _findHistory; }

    // Recent files
    QStringList getRecentFiles() const;
    void addRecentFile(const QString& filePath);
    void clearRecentFiles();
    void setMaxRecentFiles(int max);

    // Command line args
    QStringList getCLArguments() const { return _cmdLineArgs; }
    void setCLArguments(const QStringList& args) { _cmdLineArgs = args; }

    // Working directory
    QString getWorkingDirectory() const { return _workingDir; }
    void setWorkingDirectory(const QString& dir) { _workingDir = dir; }

    // File associated with this launch
    QString getAssociatedFile() const { return _associatedFile; }
    void setAssociatedFile(const QString& path) { _associatedFile = path; }

    // Language
    QString getLanguage() const { return _language; }
    void setLanguage(const QString& lang);
    void reloadLang();

    // Themes
    QString getThemeName() const { return _themeName; }
    void setTheme(const QString& themeName);
    void reloadStylers();
    bool writeStyles();
    const QVector<NppTheme>& getThemes() const { return _themes; }
    NppTheme* findTheme(const QString& name);

    // Shortcuts
    const std::vector<ShortcutDef>& getShortcuts() const { return _shortcuts; }
    void setShortcuts(const std::vector<ShortcutDef>& shortcuts);
    ShortcutDef* findShortcut(const QString& name);

    // User-defined languages path
    QString getUserDefineLangDir() const;
    void setUserDefineLangDir(const QString& dir);

    // Config paths
    QString getConfigDir() const;
    QString getStylersThemePath() const;
    QString getUserDefineLangPath() const;
    QString getShortcutsPath() const;

    // Search paths
    QStringList getSearchPaths() const;
    void addSearchPath(const QString& path);

    // N++ compatibility: app paths
    QString getNppHomePath() const { return _nppHomePath; }
    void setNppHomePath(const QString& path) { _nppHomePath = path; }

    // Encoding conversion (QTextCodec wrapper)
    QString convertEncoding(const QString& text, int fromSciCp, int toSciCp);

    // Font list
    QStringList getFontList() const;

signals:
    void settingsChanged();
    void languageChanged(const QString& lang);
    void themeChanged(const QString& theme);
    void recentFilesChanged();

private:
    explicit NppParameters(QObject* parent = nullptr);
    ~NppParameters() override;

    void loadSettings();
    void loadNppGUI();
    void loadScintillaViewParams();
    void loadFindHistory();
    void loadRecentFiles();
    void loadShortcuts();
    void loadThemes();
    void loadSearchPaths();

    void saveSettings();
    void saveNppGUI();
    void saveScintillaViewParams();
    void saveFindHistory();
    void saveRecentFiles();
    void saveShortcuts();

    void loadShortcutsFromXml(const QString& path);
    void loadThemesFromDir(const QString& dir);
    NppStyle parseStyleFromXml(const QString& styleXml);

    // QSettings
    std::unique_ptr<QSettings> _settings;

    // Settings objects
    NppGUI _nppGUI;
    ScintillaViewParams _scintillaViewParams;
    FindHistory _findHistory;
    QVector<NppTheme> _themes;

    // Runtime state
    QString _language;
    QString _themeName;
    QString _workingDir;
    QString _associatedFile;
    QString _nppHomePath;
    QStringList _cmdLineArgs;
    std::vector<ShortcutDef> _shortcuts;
    std::unordered_map<QString, int> _shortcutIndex;
    QString _userDefineLangDir;
    QStringList _searchPaths;
    int _maxRecentFiles = 10;
};
