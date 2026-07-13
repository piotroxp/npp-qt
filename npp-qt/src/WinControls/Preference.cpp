// Semantic Lift: Win32 Preference → Qt6 implementation
// Original: PowerEditor/src/WinControls/Preference/preferenceDlg.cpp (7299 lines)
// Target:   npp-qt/src/WinControls/Preference.cpp
//
// Implements all PreferenceTab subclasses and PreferenceDlg.

#include "Preference.h"
#include "StaticDialog.h"
#include "ColourPicker.h"
#include "NppDarkMode.h"
#include <QApplication>
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
#include <QStyle>
#include <QMessageBox>
#include <QSettings>
#include <QDialogButtonBox>
#include <QInputDialog>

// =============================================================================
// NppSettings — singleton settings store (mirrors Win32 NppParameters)
// =============================================================================

// Static definitions
static const QString kSettingsOrg  = QStringLiteral("NotepadPlusPlus");
static const QString kSettingsApp  = QStringLiteral("npp-qt");

NppSettings& NppSettings::instance()
{
    static NppSettings inst;
    return inst;
}

QVariant NppSettings::get(const QString& key, const QVariant& defaultValue) const
{
    return _settings.value(key, defaultValue);
}

void NppSettings::set(const QString& key, const QVariant& value)
{
    _settings[key] = value;
}

void NppSettings::save()
{
    QSettings s(kSettingsOrg, kSettingsApp);
    for (auto it = _settings.begin(); it != _settings.end(); ++it) {
        s.setValue(it.key(), it.value());
    }
    s.sync();
}

void NppSettings::load()
{
    QSettings s(kSettingsOrg, kSettingsApp);
    for (const QString& key : s.allKeys()) {
        _settings[key] = s.value(key);
    }
}

// =============================================================================
// GeneralTab
// =============================================================================

GeneralTab::GeneralTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("General"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // --- Multi-instance group ---
    QGroupBox* instanceGrp = new QGroupBox(QStringLiteral("Instance Management"), this);
    QVBoxLayout* instLay = new QVBoxLayout(instanceGrp);
    _cbMultiInst = new QCheckBox(QStringLiteral("Open new instance"), this);
    _cbRememberSession = new QCheckBox(QStringLiteral("Remember current session"), this);
    _cbSaveOnBlur = new QCheckBox(QStringLiteral("Save on losing focus"), this);
    _cbMute = new QCheckBox(QStringLiteral("Mute new file notifications"), this);
    instLay->addWidget(_cbMultiInst);
    instLay->addWidget(_cbRememberSession);
    instLay->addWidget(_cbSaveOnBlur);
    instLay->addWidget(_cbMute);
    mainLay->addWidget(instanceGrp);

    // --- File handling group ---
    QGroupBox* fileGrp = new QGroupBox(QStringLiteral("File Handling"), this);
    QFormLayout* fileLay = new QFormLayout(fileGrp);
    _cbDragDrop = new QCheckBox(QStringLiteral("Enable drag & drop"), this);
    _cbAutoDetectEncoding = new QCheckBox(QStringLiteral("Auto-detect character encoding"), this);
    _cbWrapText = new QCheckBox(QStringLiteral("Default to word wrap"), this);
    fileLay->addRow(QString(), _cbDragDrop);
    fileLay->addRow(QString(), _cbAutoDetectEncoding);
    fileLay->addRow(QString(), _cbWrapText);
    mainLay->addWidget(fileGrp);

    // --- Tab settings group ---
    QGroupBox* tabGrp = new QGroupBox(QStringLiteral("Tab Settings"), this);
    QFormLayout* tabLay = new QFormLayout(tabGrp);
    _sbTabSize = new QSpinBox(this);
    _sbTabSize->setRange(1, 16);
    _sbIndentSize = new QSpinBox(this);
    _sbIndentSize->setRange(1, 16);
    _cbTabReassign = new QCheckBox(QStringLiteral("Replace by space"), this);
    tabLay->addRow(QStringLiteral("Tab size:"), _sbTabSize);
    tabLay->addRow(QStringLiteral("Indent size:"), _sbIndentSize);
    tabLay->addRow(QString(), _cbTabReassign);
    mainLay->addWidget(tabGrp);

    // --- Updates group ---
    QGroupBox* updGrp = new QGroupBox(QStringLiteral("Updates"), this);
    QVBoxLayout* updLay = new QVBoxLayout(updGrp);
    _cbCheck4Update = new QCheckBox(QStringLiteral("Check for updates at startup"), this);
    _cbBackups = new QCheckBox(QStringLiteral("Enable backup on save"), this);
    updLay->addWidget(_cbCheck4Update);
    updLay->addWidget(_cbBackups);
    mainLay->addWidget(updGrp);

    mainLay->addStretch();
}

void GeneralTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _cbMultiInst->setChecked(s.get(QStringLiteral("General.MultiInst"), false).toBool());
    _cbRememberSession->setChecked(s.get(QStringLiteral("General.RememberSession"), true).toBool());
    _cbSaveOnBlur->setChecked(s.get(QStringLiteral("General.SaveOnBlur"), false).toBool());
    _cbMute->setChecked(s.get(QStringLiteral("General.Mute"), false).toBool());
    _cbDragDrop->setChecked(s.get(QStringLiteral("General.DragDrop"), true).toBool());
    _cbAutoDetectEncoding->setChecked(s.get(QStringLiteral("General.AutoDetectEncoding"), true).toBool());
    _cbWrapText->setChecked(s.get(QStringLiteral("General.WrapText"), false).toBool());
    _sbTabSize->setValue(s.get(QStringLiteral("General.TabSize"), 4).toInt());
    _sbIndentSize->setValue(s.get(QStringLiteral("General.IndentSize"), 4).toInt());
    _cbTabReassign->setChecked(s.get(QStringLiteral("General.ReplaceTabBySpace"), true).toBool());
    _cbCheck4Update->setChecked(s.get(QStringLiteral("General.CheckUpdates"), true).toBool());
    _cbBackups->setChecked(s.get(QStringLiteral("General.EnableBackup"), false).toBool());
}

void GeneralTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("General.MultiInst"), _cbMultiInst->isChecked());
    s.set(QStringLiteral("General.RememberSession"), _cbRememberSession->isChecked());
    s.set(QStringLiteral("General.SaveOnBlur"), _cbSaveOnBlur->isChecked());
    s.set(QStringLiteral("General.Mute"), _cbMute->isChecked());
    s.set(QStringLiteral("General.DragDrop"), _cbDragDrop->isChecked());
    s.set(QStringLiteral("General.AutoDetectEncoding"), _cbAutoDetectEncoding->isChecked());
    s.set(QStringLiteral("General.WrapText"), _cbWrapText->isChecked());
    s.set(QStringLiteral("General.TabSize"), _sbTabSize->value());
    s.set(QStringLiteral("General.IndentSize"), _sbIndentSize->value());
    s.set(QStringLiteral("General.ReplaceTabBySpace"), _cbTabReassign->isChecked());
    s.set(QStringLiteral("General.CheckUpdates"), _cbCheck4Update->isChecked());
    s.set(QStringLiteral("General.EnableBackup"), _cbBackups->isChecked());
}

void GeneralTab::resetToDefaults()
{
    _cbMultiInst->setChecked(false);
    _cbRememberSession->setChecked(true);
    _cbSaveOnBlur->setChecked(false);
    _cbMute->setChecked(false);
    _cbDragDrop->setChecked(true);
    _cbAutoDetectEncoding->setChecked(true);
    _cbWrapText->setChecked(false);
    _sbTabSize->setValue(4);
    _sbIndentSize->setValue(4);
    _cbTabReassign->setChecked(true);
    _cbCheck4Update->setChecked(true);
    _cbBackups->setChecked(false);
}

// =============================================================================
// EditingTab
// =============================================================================

EditingTab::EditingTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Editing"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // --- Caret group ---
    QGroupBox* caretGrp = new QGroupBox(QStringLiteral("Caret"), this);
    QFormLayout* caretLay = new QFormLayout(caretGrp);
    _cbHighlightLine = new QCheckBox(QStringLiteral("Highlight current line"), this);
    _cbHighlightCurrentWord = new QCheckBox(QStringLiteral("Highlight matching words"), this);
    _cbCaretBlink = new QCheckBox(QStringLiteral("Caret blinking"), this);
    _sbCaretWidth = new QSpinBox(this);
    _sbCaretWidth->setRange(1, 3);
    _cbCaretStyle = new QComboBox(this);
    _cbCaretStyle->addItems({ QStringLiteral("Line"),
                               QStringLiteral("Block"),
                               QStringLiteral("Thin Block"),
                               QStringLiteral("Invisible") });
    caretLay->addRow(QString(), _cbHighlightLine);
    caretLay->addRow(QString(), _cbHighlightCurrentWord);
    caretLay->addRow(QStringLiteral("Blink rate:"), _cbCaretBlink);
    caretLay->addRow(QStringLiteral("Width (px):"), _sbCaretWidth);
    caretLay->addRow(QStringLiteral("Style:"), _cbCaretStyle);
    mainLay->addWidget(caretGrp);

    // --- Display group ---
    QGroupBox* dispGrp = new QGroupBox(QStringLiteral("Display"), this);
    QVBoxLayout* dispLay = new QVBoxLayout(dispGrp);
    _cbShowWhiteSpace = new QCheckBox(QStringLiteral("Show whitespace (tab & space)"), this);
    _cbShowEOL = new QCheckBox(QStringLiteral("Show end-of-line markers"), this);
    _cbShowLineNumbers = new QCheckBox(QStringLiteral("Show line numbers"), this);
    _sbLineNumberMargin = new QSpinBox(this);
    _sbLineNumberMargin->setRange(1, 20);
    dispLay->addWidget(_cbShowWhiteSpace);
    dispLay->addWidget(_cbShowEOL);
    dispLay->addWidget(_cbShowLineNumbers);
    QHBoxLayout* lnLay = new QHBoxLayout;
    lnLay->addWidget(new QLabel(QStringLiteral("Line number width (chars):"), this));
    lnLay->addWidget(_sbLineNumberMargin);
    dispLay->addLayout(lnLay);
    mainLay->addWidget(dispGrp);

    // --- Indentation group ---
    QGroupBox* indGrp = new QGroupBox(QStringLiteral("Indentation"), this);
    QVBoxLayout* indLay = new QVBoxLayout(indGrp);
    _cbAutoIndent = new QCheckBox(QStringLiteral("Auto-indent"), this);
    _cbSmartIndent = new QCheckBox(QStringLiteral("Smart indent (auto-close braces)"), this);
    _cbEnableScintillaHomoglyph = new QCheckBox(
        QStringLiteral("Enable homoglyph display for ambiguous characters"), this);
    _cbBookmark = new QCheckBox(QStringLiteral("Enable bookmarks"), this);
    indLay->addWidget(_cbAutoIndent);
    indLay->addWidget(_cbSmartIndent);
    indLay->addWidget(_cbEnableScintillaHomoglyph);
    indLay->addWidget(_cbBookmark);
    mainLay->addWidget(indGrp);

    mainLay->addStretch();
}

void EditingTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _cbHighlightLine->setChecked(s.get(QStringLiteral("Editing.HighlightLine"), true).toBool());
    _cbHighlightCurrentWord->setChecked(s.get(QStringLiteral("Editing.HighlightCurrentWord"), true).toBool());
    _cbCaretBlink->setChecked(s.get(QStringLiteral("Editing.CaretBlink"), true).toBool());
    _sbCaretWidth->setValue(s.get(QStringLiteral("Editing.CaretWidth"), 2).toInt());
    _cbCaretStyle->setCurrentIndex(s.get(QStringLiteral("Editing.CaretStyle"), 0).toInt());
    _cbShowWhiteSpace->setChecked(s.get(QStringLiteral("Editing.ShowWhiteSpace"), false).toBool());
    _cbShowEOL->setChecked(s.get(QStringLiteral("Editing.ShowEOL"), false).toBool());
    _cbShowLineNumbers->setChecked(s.get(QStringLiteral("Editing.ShowLineNumbers"), true).toBool());
    _sbLineNumberMargin->setValue(s.get(QStringLiteral("Editing.LineNumberMargin"), 4).toInt());
    _cbAutoIndent->setChecked(s.get(QStringLiteral("Editing.AutoIndent"), true).toBool());
    _cbSmartIndent->setChecked(s.get(QStringLiteral("Editing.SmartIndent"), true).toBool());
    _cbEnableScintillaHomoglyph->setChecked(s.get(QStringLiteral("Editing.Homoglyph"), false).toBool());
    _cbBookmark->setChecked(s.get(QStringLiteral("Editing.Bookmark"), true).toBool());
}

void EditingTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Editing.HighlightLine"), _cbHighlightLine->isChecked());
    s.set(QStringLiteral("Editing.HighlightCurrentWord"), _cbHighlightCurrentWord->isChecked());
    s.set(QStringLiteral("Editing.CaretBlink"), _cbCaretBlink->isChecked());
    s.set(QStringLiteral("Editing.CaretWidth"), _sbCaretWidth->value());
    s.set(QStringLiteral("Editing.CaretStyle"), _cbCaretStyle->currentIndex());
    s.set(QStringLiteral("Editing.ShowWhiteSpace"), _cbShowWhiteSpace->isChecked());
    s.set(QStringLiteral("Editing.ShowEOL"), _cbShowEOL->isChecked());
    s.set(QStringLiteral("Editing.ShowLineNumbers"), _cbShowLineNumbers->isChecked());
    s.set(QStringLiteral("Editing.LineNumberMargin"), _sbLineNumberMargin->value());
    s.set(QStringLiteral("Editing.AutoIndent"), _cbAutoIndent->isChecked());
    s.set(QStringLiteral("Editing.SmartIndent"), _cbSmartIndent->isChecked());
    s.set(QStringLiteral("Editing.Homoglyph"), _cbEnableScintillaHomoglyph->isChecked());
    s.set(QStringLiteral("Editing.Bookmark"), _cbBookmark->isChecked());
}

void EditingTab::resetToDefaults()
{
    _cbHighlightLine->setChecked(true);
    _cbHighlightCurrentWord->setChecked(true);
    _cbCaretBlink->setChecked(true);
    _sbCaretWidth->setValue(2);
    _cbCaretStyle->setCurrentIndex(0);
    _cbShowWhiteSpace->setChecked(false);
    _cbShowEOL->setChecked(false);
    _cbShowLineNumbers->setChecked(true);
    _sbLineNumberMargin->setValue(4);
    _cbAutoIndent->setChecked(true);
    _cbSmartIndent->setChecked(true);
    _cbEnableScintillaHomoglyph->setChecked(false);
    _cbBookmark->setChecked(true);
}

// =============================================================================
// LanguageTab
// =============================================================================

LanguageTab::LanguageTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Language"), parent)
{
    QHBoxLayout* mainLay = new QHBoxLayout(this);

    // Left: language list
    _langList = new QListWidget(this);
    _langList->addItems({
        QStringLiteral("Normal"),
        QStringLiteral("C/C++"),
        QStringLiteral("Java"),
        QStringLiteral("Python"),
        QStringLiteral("HTML"),
        QStringLiteral("XML"),
        QStringLiteral("JavaScript"),
        QStringLiteral("CSS"),
        QStringLiteral("JSON"),
        QStringLiteral("SQL"),
        QStringLiteral("Bash/Shell"),
        QStringLiteral("PowerShell"),
        QStringLiteral("Ruby"),
        QStringLiteral("PHP"),
        QStringLiteral("Rust"),
        QStringLiteral("Go"),
        QStringLiteral("TypeScript"),
        QStringLiteral("YAML"),
        QStringLiteral("Markdown"),
        QStringLiteral("LaTeX"),
    });
    mainLay->addWidget(_langList, 1);

    // Right: detail panel
    _langDetail = new QWidget(this);
    QFormLayout* detLay = new QFormLayout(_langDetail);
    _cbDefaultLang = new QComboBox(this);
    {
        QStringList langNames;
        for (auto* item : _langList->findItems(QString(), Qt::MatchContains)) {
            langNames.append(item->text());
        }
        _cbDefaultLang->addItems(langNames);
    }
    _cbCaseSensitive = new QCheckBox(QStringLiteral("Case-sensitive syntax highlighting"), this);
    _cbFuzzyMatch = new QCheckBox(QStringLiteral("Fuzzy match on Search"), this);
    detLay->addRow(QStringLiteral("Default language:"), _cbDefaultLang);
    detLay->addRow(QString(), _cbCaseSensitive);
    detLay->addRow(QString(), _cbFuzzyMatch);
    mainLay->addWidget(_langDetail, 2);
}

void LanguageTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _cbDefaultLang->setCurrentText(s.get(QStringLiteral("Language.Default"), QStringLiteral("Normal")).toString());
    _cbCaseSensitive->setChecked(s.get(QStringLiteral("Language.CaseSensitive"), false).toBool());
    _cbFuzzyMatch->setChecked(s.get(QStringLiteral("Language.FuzzyMatch"), false).toBool());
}

void LanguageTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Language.Default"), _cbDefaultLang->currentText());
    s.set(QStringLiteral("Language.CaseSensitive"), _cbCaseSensitive->isChecked());
    s.set(QStringLiteral("Language.FuzzyMatch"), _cbFuzzyMatch->isChecked());
}

void LanguageTab::resetToDefaults()
{
    _cbDefaultLang->setCurrentText(QStringLiteral("Normal"));
    _cbCaseSensitive->setChecked(false);
    _cbFuzzyMatch->setChecked(false);
}

// =============================================================================
// BackupTab
// =============================================================================

BackupTab::BackupTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Backup"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    QGroupBox* autoSaveGrp = new QGroupBox(QStringLiteral("Auto-save"), this);
    QFormLayout* asLay = new QFormLayout(autoSaveGrp);
    _sbAutoSaveInterval = new QSpinBox(this);
    _sbAutoSaveInterval->setRange(0, 1440);
    _sbAutoSaveInterval->setSuffix(QStringLiteral(" min"));
    _cbRememberUnnamed = new QCheckBox(
        QStringLiteral("Remember unsaved files when closing"), this);
    asLay->addRow(QStringLiteral("Interval (0=off):"), _sbAutoSaveInterval);
    asLay->addRow(QString(), _cbRememberUnnamed);
    mainLay->addWidget(autoSaveGrp);

    QGroupBox* backupGrp = new QGroupBox(QStringLiteral("Backup on Save"), this);
    QFormLayout* bakLay = new QFormLayout(backupGrp);
    _cbBackupMode = new QComboBox(this);
    _cbBackupMode->addItems({
        QStringLiteral("None"),
        QStringLiteral("Simple backup (one file)"),
        QStringLiteral("Timestamped backup"),
        QStringLiteral("Backup to folder"),
    });
    _leBackupDir = new QLineEdit(this);
    _btnBrowseBackupDir = new QPushButton(QStringLiteral("Browse..."), this);
    _cbBakInSameDir = new QCheckBox(QStringLiteral("Save in same directory as original"), this);

    QHBoxLayout* dirLay = new QHBoxLayout;
    dirLay->addWidget(_leBackupDir);
    dirLay->addWidget(_btnBrowseBackupDir);

    bakLay->addRow(QStringLiteral("Mode:"), _cbBackupMode);
    bakLay->addRow(QStringLiteral("Backup folder:"), dirLay);
    bakLay->addRow(QString(), _cbBakInSameDir);

    connect(_btnBrowseBackupDir, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this,
            QStringLiteral("Select Backup Folder"),
            _leBackupDir->text());
        if (!dir.isEmpty())
            _leBackupDir->setText(dir);
    });

    mainLay->addWidget(backupGrp);
    mainLay->addStretch();
}

void BackupTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _sbAutoSaveInterval->setValue(s.get(QStringLiteral("Backup.AutoSaveInterval"), 0).toInt());
    _cbRememberUnnamed->setChecked(s.get(QStringLiteral("Backup.RememberUnnamed"), false).toBool());
    _cbBackupMode->setCurrentIndex(s.get(QStringLiteral("Backup.Mode"), 0).toInt());
    _leBackupDir->setText(s.get(QStringLiteral("Backup.Dir"), QDir::homePath()).toString());
    _cbBakInSameDir->setChecked(s.get(QStringLiteral("Backup.SameDir"), true).toBool());
}

void BackupTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Backup.AutoSaveInterval"), _sbAutoSaveInterval->value());
    s.set(QStringLiteral("Backup.RememberUnnamed"), _cbRememberUnnamed->isChecked());
    s.set(QStringLiteral("Backup.Mode"), _cbBackupMode->currentIndex());
    s.set(QStringLiteral("Backup.Dir"), _leBackupDir->text());
    s.set(QStringLiteral("Backup.SameDir"), _cbBakInSameDir->isChecked());
}

void BackupTab::resetToDefaults()
{
    _sbAutoSaveInterval->setValue(0);
    _cbRememberUnnamed->setChecked(false);
    _cbBackupMode->setCurrentIndex(0);
    _leBackupDir->clear();
    _cbBakInSameDir->setChecked(true);
}

// =============================================================================
// PrintingTab
// =============================================================================

PrintingTab::PrintingTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Printing"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    QGroupBox* headerGrp = new QGroupBox(QStringLiteral("Header & Footer"), this);
    QFormLayout* hfLay = new QFormLayout(headerGrp);
    _cbPrintHeader = new QComboBox(this);
    _cbPrintHeader->addItems({ QStringLiteral("None"),
                                QStringLiteral("Full path"),
                                QStringLiteral("Filename only"),
                                QStringLiteral("Custom...") });
    _cbPrintFooter = new QComboBox(this);
    _cbPrintFooter->addItems({ QStringLiteral("None"),
                                QStringLiteral("Page number"),
                                QStringLiteral("Date"),
                                QStringLiteral("Custom...") });
    hfLay->addRow(QStringLiteral("Header:"), _cbPrintHeader);
    hfLay->addRow(QStringLiteral("Footer:"), _cbPrintFooter);
    mainLay->addWidget(headerGrp);

    QGroupBox* colGrp = new QGroupBox(QStringLiteral("Colour Mode"), this);
    QFormLayout* colLay = new QFormLayout(colGrp);
    _cbPrintColorMode = new QComboBox(this);
    _cbPrintColorMode->addItems({
        QStringLiteral("Normal colours"),
        QStringLiteral("Inverted (dark on light)"),
        QStringLiteral("True black on white"),
        QStringLiteral("Force 1-colour (printer)"),
    });
    _cbPrintLineNumber = new QCheckBox(QStringLiteral("Print line numbers"), this);
    colLay->addRow(QStringLiteral("Mode:"), _cbPrintColorMode);
    colLay->addRow(QString(), _cbPrintLineNumber);
    mainLay->addWidget(colGrp);

    QGroupBox* marginGrp = new QGroupBox(QStringLiteral("Margins (mm)"), this);
    QFormLayout* mgLay = new QFormLayout(marginGrp);
    _sbPrintMarginLeft = new QSpinBox(this); _sbPrintMarginLeft->setRange(0, 100);
    _sbPrintMarginRight = new QSpinBox(this); _sbPrintMarginRight->setRange(0, 100);
    _sbPrintMarginTop = new QSpinBox(this); _sbPrintMarginTop->setRange(0, 100);
    _sbPrintMarginBottom = new QSpinBox(this); _sbPrintMarginBottom->setRange(0, 100);
    mgLay->addRow(QStringLiteral("Left:"), _sbPrintMarginLeft);
    mgLay->addRow(QStringLiteral("Right:"), _sbPrintMarginRight);
    mgLay->addRow(QStringLiteral("Top:"), _sbPrintMarginTop);
    mgLay->addRow(QStringLiteral("Bottom:"), _sbPrintMarginBottom);
    mainLay->addWidget(marginGrp);

    mainLay->addStretch();
}

void PrintingTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _cbPrintHeader->setCurrentIndex(s.get(QStringLiteral("Print.Header"), 0).toInt());
    _cbPrintFooter->setCurrentIndex(s.get(QStringLiteral("Print.Footer"), 0).toInt());
    _cbPrintColorMode->setCurrentIndex(s.get(QStringLiteral("Print.ColorMode"), 0).toInt());
    _cbPrintLineNumber->setChecked(s.get(QStringLiteral("Print.LineNumbers"), false).toBool());
    _sbPrintMarginLeft->setValue(s.get(QStringLiteral("Print.MarginLeft"), 15).toInt());
    _sbPrintMarginRight->setValue(s.get(QStringLiteral("Print.MarginRight"), 15).toInt());
    _sbPrintMarginTop->setValue(s.get(QStringLiteral("Print.MarginTop"), 15).toInt());
    _sbPrintMarginBottom->setValue(s.get(QStringLiteral("Print.MarginBottom"), 15).toInt());
}

void PrintingTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Print.Header"), _cbPrintHeader->currentIndex());
    s.set(QStringLiteral("Print.Footer"), _cbPrintFooter->currentIndex());
    s.set(QStringLiteral("Print.ColorMode"), _cbPrintColorMode->currentIndex());
    s.set(QStringLiteral("Print.LineNumbers"), _cbPrintLineNumber->isChecked());
    s.set(QStringLiteral("Print.MarginLeft"), _sbPrintMarginLeft->value());
    s.set(QStringLiteral("Print.MarginRight"), _sbPrintMarginRight->value());
    s.set(QStringLiteral("Print.MarginTop"), _sbPrintMarginTop->value());
    s.set(QStringLiteral("Print.MarginBottom"), _sbPrintMarginBottom->value());
}

void PrintingTab::resetToDefaults()
{
    _cbPrintHeader->setCurrentIndex(0);
    _cbPrintFooter->setCurrentIndex(0);
    _cbPrintColorMode->setCurrentIndex(0);
    _cbPrintLineNumber->setChecked(false);
    _sbPrintMarginLeft->setValue(15);
    _sbPrintMarginRight->setValue(15);
    _sbPrintMarginTop->setValue(15);
    _sbPrintMarginBottom->setValue(15);
}

// =============================================================================
// DarkModeTab
// =============================================================================

DarkModeTab::DarkModeTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Theme"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    QGroupBox* themeGrp = new QGroupBox(QStringLiteral("Theme"), this);
    QFormLayout* thLay = new QFormLayout(themeGrp);
    _cbTheme = new QComboBox(this);
    _cbTheme->addItems({ QStringLiteral("Light"),
                         QStringLiteral("Dark"),
                         QStringLiteral("OLED Black"),
                         QStringLiteral("Custom") });
    _cbDarkMode = new QCheckBox(QStringLiteral("Enable dark mode"), this);
    _cbFollowSystemTheme = new QCheckBox(QStringLiteral("Follow system theme"), this);
    thLay->addRow(QStringLiteral("Theme preset:"), _cbTheme);
    thLay->addRow(QString(), _cbDarkMode);
    thLay->addRow(QString(), _cbFollowSystemTheme);
    mainLay->addWidget(themeGrp);

    QGroupBox* colorGrp = new QGroupBox(QStringLiteral("Custom Colours"), this);
    QFormLayout* clLay = new QFormLayout(colorGrp);
    _btnFgColor = new QPushButton(QStringLiteral("Foreground..."), this);
    _btnBgColor = new QPushButton(QStringLiteral("Background..."), this);
    clLay->addRow(QStringLiteral("Foreground:"), _btnFgColor);
    clLay->addRow(QStringLiteral("Background:"), _btnBgColor);

    connect(_btnFgColor, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(_globalFg, this, QStringLiteral("Foreground Colour"));
        if (c.isValid())
            _globalFg = c;
    });
    connect(_btnBgColor, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(_globalBg, this, QStringLiteral("Background Colour"));
        if (c.isValid())
            _globalBg = c;
    });

    mainLay->addWidget(colorGrp);
    mainLay->addStretch();
}

void DarkModeTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _cbTheme->setCurrentIndex(s.get(QStringLiteral("Theme.Preset"), 1).toInt());
    _cbDarkMode->setChecked(s.get(QStringLiteral("Theme.DarkMode"), true).toBool());
    _cbFollowSystemTheme->setChecked(s.get(QStringLiteral("Theme.FollowSystem"), false).toBool());
    _globalFg = s.get(QStringLiteral("Theme.Fg"), QColor(Qt::white)).value<QColor>();
    _globalBg = s.get(QStringLiteral("Theme.Bg"), QColor(30, 30, 30)).value<QColor>();
}

void DarkModeTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Theme.Preset"), _cbTheme->currentIndex());
    s.set(QStringLiteral("Theme.DarkMode"), _cbDarkMode->isChecked());
    s.set(QStringLiteral("Theme.FollowSystem"), _cbFollowSystemTheme->isChecked());
    s.set(QStringLiteral("Theme.Fg"), _globalFg);
    s.set(QStringLiteral("Theme.Bg"), _globalBg);
}

void DarkModeTab::resetToDefaults()
{
    _cbTheme->setCurrentIndex(1);
    _cbDarkMode->setChecked(true);
    _cbFollowSystemTheme->setChecked(false);
    _globalFg = Qt::white;
    _globalBg = QColor(30, 30, 30);
}

// =============================================================================
// FileAssocTab
// =============================================================================

FileAssocTab::FileAssocTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("File Associations"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    _assocList = new QListWidget(this);
    mainLay->addWidget(_assocList);

    QHBoxLayout* btnLay = new QHBoxLayout;
    _btnAddAssoc = new QPushButton(QStringLiteral("Add..."), this);
    _btnRemoveAssoc = new QPushButton(QStringLiteral("Remove"), this);
    _btnSetDefault = new QPushButton(QStringLiteral("Set as Default"), this);
    _cbUseWinShell = new QCheckBox(
        QStringLiteral("Use Windows shell integration"), this);
    btnLay->addWidget(_btnAddAssoc);
    btnLay->addWidget(_btnRemoveAssoc);
    btnLay->addWidget(_btnSetDefault);
    btnLay->addStretch();

    mainLay->addLayout(btnLay);
    mainLay->addWidget(_cbUseWinShell);

    connect(_btnAddAssoc, &QPushButton::clicked, this, [this]() {
        QString ext = QInputDialog::getText(this, QStringLiteral("Add Extension"),
                                            QStringLiteral("File extension (e.g. .txt):"));
        if (!ext.isEmpty() && !ext.startsWith('.'))
            ext = QStringLiteral(".") + ext;
        if (!ext.isEmpty())
            _assocList->addItem(ext);
    });

    connect(_btnRemoveAssoc, &QPushButton::clicked, this, [this]() {
        delete _assocList->currentItem();
    });
}

void FileAssocTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    QStringList assocs = s.get(QStringLiteral("Assoc.Extensions"), QStringList()).toStringList();
    _assocList->clear();
    _assocList->addItems(assocs);
    _cbUseWinShell->setChecked(s.get(QStringLiteral("Assoc.UseShell"), false).toBool());
}

void FileAssocTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    QStringList assocs;
    for (int i = 0; i < _assocList->count(); ++i)
        assocs << _assocList->item(i)->text();
    s.set(QStringLiteral("Assoc.Extensions"), assocs);
    s.set(QStringLiteral("Assoc.UseShell"), _cbUseWinShell->isChecked());
}

void FileAssocTab::resetToDefaults()
{
    _assocList->clear();
    _cbUseWinShell->setChecked(false);
}

// =============================================================================
// PerformanceTab
// =============================================================================

PerformanceTab::PerformanceTab(QWidget* parent)
    : PreferenceTab(QStringLiteral("Performance"), parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    QGroupBox* memoryGrp = new QGroupBox(QStringLiteral("Memory"), this);
    QFormLayout* memLay = new QFormLayout(memoryGrp);
    _sbFileHistorySize = new QSpinBox(this);
    _sbFileHistorySize->setRange(10, 200);
    _sbMaxLineInMemory = new QSpinBox(this);
    _sbMaxLineInMemory->setRange(1000, 10000000);
    memLay->addRow(QStringLiteral("Recent files list size:"), _sbFileHistorySize);
    memLay->addRow(QStringLiteral("Max lines in memory:"), _sbMaxLineInMemory);
    mainLay->addWidget(memoryGrp);

    QGroupBox* highlightGrp = new QGroupBox(QStringLiteral("Highlighting"), this);
    QFormLayout* hlLay = new QFormLayout(highlightGrp);
    _cbSmartHighlighting = new QCheckBox(QStringLiteral("Enable smart highlighting"), this);
    _sbMaxHighlighting = new QSpinBox(this);
    _sbMaxHighlighting->setRange(100, 100000);
    hlLay->addRow(QString(), _cbSmartHighlighting);
    hlLay->addRow(QStringLiteral("Max highlighted matches:"), _sbMaxHighlighting);
    mainLay->addWidget(highlightGrp);

    QGroupBox* compatGrp = new QGroupBox(QStringLiteral("Compatibility"), this);
    QVBoxLayout* compLay = new QVBoxLayout(compatGrp);
    _cbDoAero = new QCheckBox(
        QStringLiteral("Enable Aero Snap / DWM shadow (Windows)"), this);
    compLay->addWidget(_cbDoAero);
    mainLay->addWidget(compatGrp);

    mainLay->addStretch();
}

void PerformanceTab::loadFromSettings()
{
    NppSettings& s = NppSettings::instance();
    _sbFileHistorySize->setValue(s.get(QStringLiteral("Perf.FileHistorySize"), 20).toInt());
    _sbMaxLineInMemory->setValue(s.get(QStringLiteral("Perf.MaxLineInMemory"), 2000000).toInt());
    _cbSmartHighlighting->setChecked(s.get(QStringLiteral("Perf.SmartHighlighting"), true).toBool());
    _sbMaxHighlighting->setValue(s.get(QStringLiteral("Perf.MaxHighlighting"), 10000).toInt());
    _cbDoAero->setChecked(s.get(QStringLiteral("Perf.AeroSnap"), true).toBool());
}

void PerformanceTab::saveToSettings()
{
    NppSettings& s = NppSettings::instance();
    s.set(QStringLiteral("Perf.FileHistorySize"), _sbFileHistorySize->value());
    s.set(QStringLiteral("Perf.MaxLineInMemory"), _sbMaxLineInMemory->value());
    s.set(QStringLiteral("Perf.SmartHighlighting"), _cbSmartHighlighting->isChecked());
    s.set(QStringLiteral("Perf.MaxHighlighting"), _sbMaxHighlighting->value());
    // AeroSnap is a Windows-specific feature — on Linux/X11, window snapping
    // is handled natively by the window manager (KWin, Mutter, etc.)
    s.set(QStringLiteral("Perf.AeroSnap"), _cbDoAero->isChecked());
}

void PerformanceTab::resetToDefaults()
{
    _sbFileHistorySize->setValue(20);
    _sbMaxLineInMemory->setValue(2000000);
    _cbSmartHighlighting->setChecked(true);
    _sbMaxHighlighting->setValue(10000);
    _cbDoAero->setChecked(true);
}

// =============================================================================
// PreferenceDlg — main preferences dialog
// =============================================================================

PreferenceDlg::PreferenceDlg()
    : QDialog()
{
    setWindowTitle(QStringLiteral("Preferences"));
    setModal(true);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Tab widget holds all preference pages
    _tabWidget = new QTabWidget(this);
    mainLay->addWidget(_tabWidget);

    // Create all tabs
    createAllTabs();

    // Button row
    QHBoxLayout* btnLay = new QHBoxLayout;
    btnLay->addStretch();

    _okBtn = new QPushButton(QStringLiteral("OK"), this);
    _cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
    _applyBtn = new QPushButton(QStringLiteral("Apply"), this);

    connect(_okBtn, &QPushButton::clicked, this, &PreferenceDlg::onOk);
    connect(_cancelBtn, &QPushButton::clicked, this, &PreferenceDlg::onCancel);
    connect(_applyBtn, &QPushButton::clicked, this, &PreferenceDlg::onApply);

    btnLay->addWidget(_okBtn);
    btnLay->addWidget(_cancelBtn);
    btnLay->addWidget(_applyBtn);

    mainLay->addLayout(btnLay);

    // Load settings into tabs
    loadAllTabs();
}

PreferenceDlg::~PreferenceDlg() = default;

void PreferenceDlg::createAllTabs()
{
    // Each tab is a new instance of a PreferenceTab subclass
    GeneralTab* generalTab = new GeneralTab(this);
    EditingTab* editingTab = new EditingTab(this);
    LanguageTab* languageTab = new LanguageTab(this);
    BackupTab* backupTab = new BackupTab(this);
    PrintingTab* printingTab = new PrintingTab(this);
    DarkModeTab* darkModeTab = new DarkModeTab(this);
    FileAssocTab* fileAssocTab = new FileAssocTab(this);
    PerformanceTab* performanceTab = new PerformanceTab(this);

    _tabs.append(generalTab);
    _tabs.append(editingTab);
    _tabs.append(languageTab);
    _tabs.append(backupTab);
    _tabs.append(printingTab);
    _tabs.append(darkModeTab);
    _tabs.append(fileAssocTab);
    _tabs.append(performanceTab);

    _tabWidget->addTab(generalTab,     generalTab->title());
    _tabWidget->addTab(editingTab,     editingTab->title());
    _tabWidget->addTab(languageTab,    languageTab->title());
    _tabWidget->addTab(backupTab,      backupTab->title());
    _tabWidget->addTab(printingTab,    printingTab->title());
    _tabWidget->addTab(darkModeTab,    darkModeTab->title());
    _tabWidget->addTab(fileAssocTab,   fileAssocTab->title());
    _tabWidget->addTab(performanceTab, performanceTab->title());
}

void PreferenceDlg::loadAllTabs()
{
    for (PreferenceTab* tab : _tabs)
        tab->loadFromSettings();
}

void PreferenceDlg::saveAllTabs()
{
    for (PreferenceTab* tab : _tabs)
        tab->saveToSettings();
    NppSettings::instance().save();
}

void PreferenceDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    // Load from persisted settings
    NppSettings::instance().load();
    loadAllTabs();
    // Switch to saved or default tab
    _tabWidget->setCurrentIndex(_initialTab);

    // Connect tab change after loading
    connect(_tabWidget, &QTabWidget::currentChanged,
            this, &PreferenceDlg::onTabChanged);

    // Show the dialog
    show();
    raise();
    activateWindow();
}

void PreferenceDlg::switchToTab(int index)
{
    if (index >= 0 && index < _tabWidget->count())
        _tabWidget->setCurrentIndex(index);
}

void PreferenceDlg::onTabChanged(int index)
{
    _initialTab = index;
}

void PreferenceDlg::onOk()
{
    saveAllTabs();
    accept(); // close the dialog
}

void PreferenceDlg::onCancel()
{
    // Reload settings to discard any changes
    NppSettings::instance().load();
    reject(); // close the dialog
}

void PreferenceDlg::onApply()
{
    saveAllTabs();
    // Apply dark mode if it changed (mirrors original NPPM_INTERNAL_REFRESHDARKMODE)
    // setEnabled() triggers darkModeChanged → NPPN_DARKMODECHANGED
    // refreshDarkMode() updates all widgets with new palette
    NppSettings& s = NppSettings::instance();
    bool darkEnabled = s.get(QStringLiteral("Theme.DarkMode"), false).toBool();
    NppDarkMode::instance().setEnabled(darkEnabled);
    // Note: darkModeChanged signal will trigger NPPN_DARKMODECHANGED via Notepad_plus connection
}

void PreferenceDlg::notifyItemChanged(const QString& key, const QVariant& value)
{
    NppSettings::instance().set(key, value);
}

intptr_t PreferenceDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(message);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return 0;
}
