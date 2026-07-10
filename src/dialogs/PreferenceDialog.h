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

private slots:
    void onCategoryChanged(QTreeWidgetItem* item, int column);

private:
    void setupUi();
    void loadSettings();
    QWidget* createGeneralPage();
    QWidget* createEditorPage();
    QWidget* createAppearancePage();
    QWidget* createFileAssociationPage();
    QWidget* createShortcutPage();
    QWidget* createMarginsPage();
    QWidget* createBackupPage();

    // General page widgets
    QCheckBox* _chkSingleInstance = nullptr;
    QCheckBox* _chkRememberSession = nullptr;
    QSpinBox* _spinMaxRecentFiles = nullptr;

    // Editor page widgets
    QSpinBox* _spinTabWidth = nullptr;
    QCheckBox* _chkTabAsSpaces = nullptr;
    QComboBox* _cmbEolMode = nullptr;
    QComboBox* _cmbDefaultEncoding = nullptr;
    QCheckBox* _chkAutoIndent = nullptr;
    QCheckBox* _chkWrapWithQuotes = nullptr;

    // Appearance page widgets
    QComboBox* _cmbTheme = nullptr;
    QCheckBox* _chkShowToolbar = nullptr;
    QCheckBox* _chkShowTabbar = nullptr;
    QCheckBox* _chkShowStatusbar = nullptr;
    QCheckBox* _chkShowMenubar = nullptr;

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

    QTreeWidget* _categoryTree = nullptr;
    QStackedWidget* _pageStack = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;

    // Store original settings for cancel
    struct OriginalSettings {
        bool singleInstance;
        bool rememberSession;
        int maxRecentFiles;
        int tabWidth;
        bool tabAsSpaces;
        int eolMode;
        int defaultEncoding;
        bool autoIndent;
        bool wrapWithQuotes;
        QString theme;
        bool showToolbar;
        bool showTabbar;
        bool showStatusbar;
        bool showMenubar;
    } _originalSettings;
};
