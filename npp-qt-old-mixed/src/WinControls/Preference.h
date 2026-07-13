// Semantic Lift: Win32 Preference → Qt6 implementation
// Original: PowerEditor/src/WinControls/Preference/preferenceDlg.h (452 lines)
// Target: npp-qt/src/WinControls/Preference.h
//
// Lifts the Preferences dialog from Win32 TabControl + many child dialogs
// to Qt6 QTabWidget with per-category QWidgets.
// Follows the npp-qt WinControls translation pattern.

#pragma once

#include "StaticDialog.h"
#include <QDialog>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFontComboBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QMap>
#include <QString>

// =============================================================================
// PreferenceTab — base class for each preferences page
// Mirrors Win32 preference dialog child windows.
// =============================================================================

class PreferenceTab : public QWidget
{
    Q_OBJECT
public:
    explicit PreferenceTab(const QString& title, QWidget* parent = nullptr)
        : QWidget(parent), _title(title) {}
    virtual ~PreferenceTab() = default;

    virtual void loadFromSettings() = 0;
    virtual void saveToSettings() = 0;
    virtual void resetToDefaults() = 0;

    QString title() const { return _title; }

protected:
    QString _title;
};

// =============================================================================
// GeneralTab — General settings (launch, drag-drop, file association)
// =============================================================================

// =============================================================================
// TabbarSubDlg — stub sub-dialog for Tab Bar preferences
// Needed by Notepad_plus.cpp call sites and PreferenceDlg members.
// =============================================================================
class TabbarSubDlg : public QWidget
{
    Q_OBJECT
public:
    explicit TabbarSubDlg(QWidget* parent = nullptr) : QWidget(parent) {}
    void setTabbarAlternateIcons(bool on) { _altIcons = on; }
    bool isTabbarAlternateIcons() const { return _altIcons; }
private:
    bool _altIcons = false;
};

// =============================================================================
// GeneralSubDlg — stub sub-dialog for General preferences
// Needed by Notepad_plus.cpp call sites and PreferenceDlg members.
// =============================================================================
class GeneralSubDlg : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralSubDlg(QWidget* parent = nullptr) : QWidget(parent) {}
    bool isCheckedOrNot(int /*id*/) const { return false; }
};


class GeneralTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit GeneralTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QCheckBox* _cbMultiInst = nullptr;
    QCheckBox* _cbRememberSession = nullptr;
    QCheckBox* _cbSaveOnBlur = nullptr;
    QCheckBox* _cbDragDrop = nullptr;
    QCheckBox* _cbMute = nullptr;
    QCheckBox* _cbCheck4Update = nullptr;
    QCheckBox* _cbAutoDetectEncoding = nullptr;
    QCheckBox* _cbWrapText = nullptr;
    QSpinBox* _sbTabSize = nullptr;
    QSpinBox* _sbIndentSize = nullptr;
    QCheckBox* _cbTabReassign = nullptr;
    QCheckBox* _cbBackups = nullptr;
    QComboBox* _cbBackupDir = nullptr;
};

// =============================================================================
// EditingTab — Editor behavior settings
// =============================================================================

class EditingTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit EditingTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QCheckBox* _cbHighlightLine = nullptr;
    QCheckBox* _cbHighlightCurrentWord = nullptr;
    QCheckBox* _cbShowWhiteSpace = nullptr;
    QCheckBox* _cbShowEOL = nullptr;
    QCheckBox* _cbAutoIndent = nullptr;
    QCheckBox* _cbSmartIndent = nullptr;
    QCheckBox* _cbCaretBlink = nullptr;
    QSpinBox* _sbCaretWidth = nullptr;
    QComboBox* _cbCaretStyle = nullptr;
    QCheckBox* _cbEnableScintillaHomoglyph = nullptr;
    QCheckBox* _cbShowLineNumbers = nullptr;
    QSpinBox* _sbLineNumberMargin = nullptr;
    QCheckBox* _cbBookmark = nullptr;
};

// =============================================================================
// LanguageTab — Language/Tab settings
// =============================================================================

class LanguageTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit LanguageTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QListWidget* _langList = nullptr;
    QWidget* _langDetail = nullptr;
    QComboBox* _cbDefaultLang = nullptr;
    QCheckBox* _cbCaseSensitive = nullptr;
    QCheckBox* _cbFuzzyMatch = nullptr;
};

// =============================================================================
// BackupTab — Auto-save and backup settings
// =============================================================================

class BackupTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit BackupTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QComboBox* _cbBackupMode = nullptr;
    QLineEdit* _leBackupDir = nullptr;
    QPushButton* _btnBrowseBackupDir = nullptr;
    QCheckBox* _cbBakInSameDir = nullptr;
    QSpinBox* _sbAutoSaveInterval = nullptr;
    QCheckBox* _cbRememberUnnamed = nullptr;
};

// =============================================================================
// PrintingTab — Print settings
// =============================================================================

class PrintingTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit PrintingTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QComboBox* _cbPrintHeader = nullptr;
    QComboBox* _cbPrintFooter = nullptr;
    QComboBox* _cbPrintColorMode = nullptr;
    QCheckBox* _cbPrintLineNumber = nullptr;
    QSpinBox* _sbPrintMarginLeft = nullptr;
    QSpinBox* _sbPrintMarginRight = nullptr;
    QSpinBox* _sbPrintMarginTop = nullptr;
    QSpinBox* _sbPrintMarginBottom = nullptr;
};

// =============================================================================
// DarkModeTab — Theme settings
// =============================================================================

class DarkModeTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit DarkModeTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QComboBox* _cbTheme = nullptr;
    QCheckBox* _cbDarkMode = nullptr;
    QCheckBox* _cbFollowSystemTheme = nullptr;
    QColor _globalFg = Qt::white;
    QColor _globalBg = QColor(30, 30, 30);
    QPushButton* _btnFgColor = nullptr;
    QPushButton* _btnBgColor = nullptr;
};

// =============================================================================
// FileAssocTab — File association settings
// =============================================================================

class FileAssocTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit FileAssocTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QListWidget* _assocList = nullptr;
    QPushButton* _btnAddAssoc = nullptr;
    QPushButton* _btnRemoveAssoc = nullptr;
    QPushButton* _btnSetDefault = nullptr;
    QCheckBox* _cbUseWinShell = nullptr;
};

// =============================================================================
// PerformanceTab — Performance settings
// =============================================================================

class PerformanceTab : public PreferenceTab
{
    Q_OBJECT
public:
    explicit PerformanceTab(QWidget* parent = nullptr);

    void loadFromSettings() override;
    void saveToSettings() override;
    void resetToDefaults() override;

private:
    QSpinBox* _sbFileHistorySize = nullptr;
    QSpinBox* _sbMaxLineInMemory = nullptr;
    QCheckBox* _cbDoAero = nullptr;
    QCheckBox* _cbSmartHighlighting = nullptr;
    QSpinBox* _sbMaxHighlighting = nullptr;
};

// =============================================================================
// MISC — Settings persistence key/value store
// Mirrors Win32 NppParameters global.
// =============================================================================

class NppSettings
{
public:
    static NppSettings& instance();

    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void set(const QString& key, const QVariant& value);
    void save();
    void load();

private:
    NppSettings() = default;
    QMap<QString, QVariant> _settings;
};

// =============================================================================
// PreferenceDlg — main preferences dialog
// Mirrors Win32 PreferenceDlg with a QTabWidget.
// =============================================================================

class PreferenceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDlg();
    ~PreferenceDlg() override;

    void init(QWidget* parent);
    void doDialog(bool isRTL = false);
    void switchToTab(int index);

    // Notify sub-tabs of changes
    void notifyItemChanged(const QString& key, const QVariant& value);

    bool isCreated() const { return isVisible(); }

    // Sub-dialog stubs for dark mode / preferences access
    TabbarSubDlg _tabbarSubDlg;
    GeneralSubDlg _generalSubDlg;

    QWidget* getHSelf() { return this; }

private slots:
    void onOk();
    void onCancel();
    void onApply();
    void onTabChanged(int index);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam);

private:
    void createAllTabs();
    void loadAllTabs();
    void saveAllTabs();

    QTabWidget* _tabWidget = nullptr;
    QVector<PreferenceTab*> _tabs;

    QPushButton* _okBtn = nullptr;
    QPushButton* _cancelBtn = nullptr;
    QPushButton* _applyBtn = nullptr;

    int _initialTab = 0;
};
