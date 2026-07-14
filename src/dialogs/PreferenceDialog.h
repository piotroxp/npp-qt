// PreferenceDialog.h - Preferences/Settings dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QListWidget>
#include <QTableWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QFileDialog>

class PreferenceDialog : public QDialog {
    Q_OBJECT
public:
    explicit PreferenceDialog(QWidget* parent = nullptr);
    ~PreferenceDialog() override;

public slots:
    void accept() override;
    void reject() override;
    void applySettings();
    void resetToDefaults();

private slots:
    void onCategoryChanged(QTreeWidgetItem* item, int column);
    void onImportFromNpp();
    void onExportToNpp();
    void onOpenThemesFolder();
    void onReloadThemes();
    void onApply();

signals:
    void settingsApplied();

private:
    void setupUi();
    void loadSettings();
    void saveDialogGeometry();
    void restoreDialogGeometry();
    QWidget* createGeneralPage();
    QWidget* createEditorPage();
    QWidget* createAppearancePage();
    QWidget* createFileAssociationPage();
    QWidget* createShortcutPage();
    QWidget* createMarginsPage();
    QWidget* createBackupPage();
    QWidget* createLanguagePage();
    QWidget* createSearchPage();

    // General page widgets
    QCheckBox* _chkSingleInstance = nullptr;
    QCheckBox* _chkRememberSession = nullptr;
    QSpinBox* _spinMaxRecentFiles = nullptr;
    QPushButton* _btnImportNpp = nullptr;
    QPushButton* _btnExportNpp = nullptr;
    QPushButton* _btnResetDefaults = nullptr;

    // Editor page widgets
    QSpinBox* _spinTabWidth = nullptr;
    QSpinBox* _spinIndentWidth = nullptr;
    QCheckBox* _chkTabAsSpaces = nullptr;
    QCheckBox* _chkWordWrap = nullptr;
    QCheckBox* _chkVirtualSpace = nullptr;
    QCheckBox* _chkSmartHome = nullptr;
    QCheckBox* _chkBackspaceUnindent = nullptr;
    QComboBox* _cmbEolMode = nullptr;
    QComboBox* _cmbDefaultEncoding = nullptr;
    QCheckBox* _chkAutoIndent = nullptr;
    QCheckBox* _chkWrapWithQuotes = nullptr;
    QCheckBox* _chkShowEol = nullptr;
    QCheckBox* _chkShowSpaceTab = nullptr;

    // Appearance page widgets
    QComboBox* _cmbTheme = nullptr;
    QPushButton* _btnOpenThemesFolder = nullptr;
    QPushButton* _btnReloadThemes = nullptr;
    QCheckBox* _chkShowToolbar = nullptr;
    QCheckBox* _chkShowTabbar = nullptr;
    QCheckBox* _chkShowStatusbar = nullptr;
    QCheckBox* _chkShowMenubar = nullptr;
    QCheckBox* _chkCurrentLineHighlight = nullptr;
    QCheckBox* _chkShowIndentGuide = nullptr;
    QSpinBox* _spinCurrentLineColor = nullptr;

    // File Associations page widgets
    QListWidget* _extListWidget = nullptr;
    QLineEdit* _extLineEdit = nullptr;

    // Shortcut Mapper page widgets
    QCheckBox* _chkConflictWarning = nullptr;

    // Margins page widgets
    QCheckBox* _chkShowLineNumbers = nullptr;
    QSpinBox* _spinLineNumberWidth = nullptr;
    QCheckBox* _chkShowSymbols = nullptr;
    QCheckBox* _chkShowFolder = nullptr;
    QCheckBox* _chkHighlightCurrentLine = nullptr;
    QCheckBox* _chkShowEdgeLine = nullptr;
    QSpinBox* _spinSymbolMarginWidth = nullptr;
    QSpinBox* _spinEdgeColumn = nullptr;

    // Backup/Auto-Save page widgets
    QCheckBox* _chkAutoSave = nullptr;
    QSpinBox* _spinAutoSaveInterval = nullptr;
    QGroupBox* _grpAutoSaveOptions = nullptr;
    QCheckBox* _chkAutoSaveCurrentOnly = nullptr;
    QCheckBox* _chkAutoSaveInBackground = nullptr;
    QLineEdit* _backupDirEdit = nullptr;
    QComboBox* _backupStyleCombo = nullptr;
    QSpinBox* _spinMaxBackups = nullptr;

    // Language page widgets
    QComboBox* _cmbLanguage = nullptr;
    QCheckBox* _chkAutoDetectLanguage = nullptr;

    // Search page widgets
    QCheckBox* _chkSmartHighlighting = nullptr;
    QSpinBox* _spinMaxHighlightWords = nullptr;
    QCheckBox* _chkMatchCase = nullptr;
    QCheckBox* _chkMatchWholeWord = nullptr;
    QCheckBox* _chkWrapAround = nullptr;
    QSpinBox* _spinFindHistoryCount = nullptr;

    QTreeWidget* _categoryTree = nullptr;
    QStackedWidget* _pageStack = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;

    // Store original settings for cancel
    struct OriginalSettings {
        bool singleInstance;
        bool rememberSession;
        int maxRecentFiles;
        int tabWidth;
        int indentWidth;
        bool tabAsSpaces;
        bool wordWrap;
        bool virtualSpace;
        bool smartHome;
        bool backspaceUnindent;
        int eolMode;
        int defaultEncoding;
        bool autoIndent;
        bool wrapWithQuotes;
        bool showEol;
        bool showSpaceTab;
        QString theme;
        bool showToolbar;
        bool showTabbar;
        bool showStatusbar;
        bool showMenubar;
        bool currentLineHighlight;
        bool showIndentGuide;
        bool showLineNumbers;
        int lineNumberWidth;
        bool showSymbols;
        bool showFolder;
        int symbolMarginWidth;
        bool highlightCurrentLine;
        bool showEdgeLine;
        int edgeColumn;
        bool autoSave;
        int autoSaveInterval;
        bool autoSaveCurrentOnly;
        bool autoSaveInBackground;
        QString backupDir;
        int backupStyle;
        int maxBackups;
        bool smartHighlighting;
        int maxHighlightWords;
        bool matchCase;
        bool matchWholeWord;
        bool wrapAround;
        int findHistoryCount;
        QString language;
        bool autoDetectLanguage;
    } _originalSettings;

    // Track if settings were modified
    bool _settingsModified = false;
};
