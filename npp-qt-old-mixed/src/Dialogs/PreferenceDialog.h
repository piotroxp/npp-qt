// PreferenceDialog.h — npp-qt Qt6 Preferences dialog
// Replaces stub in NppCommands::settingsPreference()

#pragma once

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QSettings>
#include <QString>

// =============================================================================
// PreferencePage — base for each preferences category
// =============================================================================

class PreferencePage : public QWidget
{
    Q_OBJECT
public:
    explicit PreferencePage(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual void load() = 0;
    virtual void save() = 0;
};

// =============================================================================
// GeneralPage — General settings
// =============================================================================

class GeneralPage : public PreferencePage
{
    Q_OBJECT
public:
    explicit GeneralPage(QWidget* parent = nullptr);

    void load() override;
    void save() override;

private:
    QCheckBox* _cbAutoCompWord = nullptr;
    QCheckBox* _cbSmartComp = nullptr;
    QSpinBox* _sbAutoCompParenth = nullptr;
    QSpinBox* _sbAutoCompBracket = nullptr;
    QSpinBox* _sbAutoCompBrace = nullptr;
    QCheckBox* _cbRememberLastSession = nullptr;
    QCheckBox* _cbRememberCurrentSession = nullptr;
    QCheckBox* _cbMultiInstance = nullptr;
};

// =============================================================================
// EditingPage — Editor behavior settings
// =============================================================================

class EditingPage : public PreferencePage
{
    Q_OBJECT
public:
    explicit EditingPage(QWidget* parent = nullptr);

    void load() override;
    void save() override;

private:
    QSpinBox* _sbTabSize = nullptr;
    QCheckBox* _cbReplaceBySpace = nullptr;
    QComboBox* _cbAutoIndent = nullptr;
    QSpinBox* _sbFolderDepth = nullptr;
    QCheckBox* _cbEnableScintillaHomoglyph = nullptr;
    QCheckBox* _cbHighlightCurrentWord = nullptr;
    QCheckBox* _cbHighlightLine = nullptr;
};

// =============================================================================
// LanguagePage — Language / encoding settings
// =============================================================================

class LanguagePage : public PreferencePage
{
    Q_OBJECT
public:
    explicit LanguagePage(QWidget* parent = nullptr);

    void load() override;
    void save() override;

private:
    void populateLanguageCombo();

    QComboBox* _cbDefaultLanguage = nullptr;
    QCheckBox* _cbDetectEncoding = nullptr;
    QCheckBox* _cbApplyToOpened = nullptr;
};

// =============================================================================
// BackupPage — Auto-save and backup
// =============================================================================

class BackupPage : public PreferencePage
{
    Q_OBJECT
public:
    explicit BackupPage(QWidget* parent = nullptr);

    void load() override;
    void save() override;

private slots:
    void browseBackupPath();

private:
    QCheckBox* _cbAutoSave = nullptr;
    QSpinBox* _sbAutoSaveInterval = nullptr;
    QCheckBox* _cbSessionSnapshot = nullptr;
    QLineEdit* _leBackupPath = nullptr;
    QPushButton* _btnBrowse = nullptr;
};

// =============================================================================
// PrintPage — Print settings
// =============================================================================

class PrintPage : public PreferencePage
{
    Q_OBJECT
public:
    explicit PrintPage(QWidget* parent = nullptr);

    void load() override;
    void save() override;

private:
    QCheckBox* _cbPrintHeader = nullptr;
    QCheckBox* _cbPrintLineNumbers = nullptr;
    QComboBox* _cbColorTheme = nullptr;
    QComboBox* _cbPrintColorMode = nullptr;
    QSpinBox* _sbMarginLeft = nullptr;
    QSpinBox* _sbMarginRight = nullptr;
    QSpinBox* _sbMarginTop = nullptr;
    QSpinBox* _sbMarginBottom = nullptr;
};

// =============================================================================
// PreferenceDialog — main preferences container
// =============================================================================

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget* parent = nullptr);
    ~PreferenceDialog() override = default;

private slots:
    void onCategoryChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void onOk();
    void onCancel();
    void onApply();

private:
    void setupUi();
    void createAllPages();
    void loadAllPages();
    void saveAllPages();

    QListWidget* _categoryList = nullptr;
    QStackedWidget* _stack = nullptr;
    QVector<PreferencePage*> _pages;

    // Pages
    GeneralPage* _generalPage = nullptr;
    EditingPage* _editingPage = nullptr;
    LanguagePage* _languagePage = nullptr;
    BackupPage* _backupPage = nullptr;
    PrintPage* _printPage = nullptr;

    QDialogButtonBox* _buttonBox = nullptr;
};
