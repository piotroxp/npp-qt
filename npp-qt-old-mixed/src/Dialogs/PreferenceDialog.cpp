// PreferenceDialog.cpp — npp-qt Qt6 Preferences dialog implementation

#include "PreferenceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

// =============================================================================
// Settings helper
// =============================================================================

static QSettings& nppSettings()
{
    static QSettings s(QStringLiteral("NotepadPlusPlus"), QStringLiteral("npp-qt"));
    return s;
}

// =============================================================================
// GeneralPage
// =============================================================================

GeneralPage::GeneralPage(QWidget* parent)
    : PreferencePage(parent)
{
    auto* layout = new QVBoxLayout(this);

    // Auto-completion group
    auto* acGroup = new QGroupBox(QStringLiteral("Auto-completion"), this);
    auto* acLayout = new QFormLayout(acGroup);

    _cbAutoCompWord = new QCheckBox(this);
    acLayout->addRow(QStringLiteral("Enable auto-completion:"), _cbAutoCompWord);

    _cbSmartComp = new QCheckBox(this);
    acLayout->addRow(QStringLiteral("Enable Smart completion:"), _cbSmartComp);

    _sbAutoCompParenth = new QSpinBox(this);
    _sbAutoCompParenth->setRange(0, 10);
    _sbAutoCompParenth->setSpecialValueText(QStringLiteral("None"));
    acLayout->addRow(QStringLiteral("Auto-insert ( ):"), _sbAutoCompParenth);

    _sbAutoCompBracket = new QSpinBox(this);
    _sbAutoCompBracket->setRange(0, 10);
    _sbAutoCompBracket->setSpecialValueText(QStringLiteral("None"));
    acLayout->addRow(QStringLiteral("Auto-insert []:"), _sbAutoCompBracket);

    _sbAutoCompBrace = new QSpinBox(this);
    _sbAutoCompBrace->setRange(0, 10);
    _sbAutoCompBrace->setSpecialValueText(QStringLiteral("None"));
    acLayout->addRow(QStringLiteral("Auto-insert {}:"), _sbAutoCompBrace);

    layout->addWidget(acGroup);

    // Session group
    auto* sessGroup = new QGroupBox(QStringLiteral("Session"), this);
    auto* sessLayout = new QFormLayout(sessGroup);

    _cbRememberLastSession = new QCheckBox(this);
    sessLayout->addRow(QStringLiteral("Remember last session"), _cbRememberLastSession);

    _cbRememberCurrentSession = new QCheckBox(this);
    sessLayout->addRow(QStringLiteral("Remember current session"), _cbRememberCurrentSession);

    layout->addWidget(sessGroup);

    // Multi-instance
    auto* instGroup = new QGroupBox(QStringLiteral("Instance"), this);
    auto* instLayout = new QFormLayout(instGroup);

    _cbMultiInstance = new QCheckBox(this);
    instLayout->addRow(QStringLiteral("Multi-instance mode"), _cbMultiInstance);

    layout->addWidget(instGroup);
    layout->addStretch();
}

void GeneralPage::load()
{
    QSettings& s = nppSettings();
    _cbAutoCompWord->setChecked(s.value(QStringLiteral("autoCompWord"), true).toBool());
    _cbSmartComp->setChecked(s.value(QStringLiteral("smartComp"), true).toBool());
    _sbAutoCompParenth->setValue(s.value(QStringLiteral("autoCompParenth"), 1).toInt());
    _sbAutoCompBracket->setValue(s.value(QStringLiteral("autoCompBracket"), 1).toInt());
    _sbAutoCompBrace->setValue(s.value(QStringLiteral("autoCompBrace"), 1).toInt());
    _cbRememberLastSession->setChecked(s.value(QStringLiteral("rememberLastSession"), false).toBool());
    _cbRememberCurrentSession->setChecked(s.value(QStringLiteral("rememberCurrentSession"), true).toBool());
    _cbMultiInstance->setChecked(s.value(QStringLiteral("multiInstance"), false).toBool());
}

void GeneralPage::save()
{
    QSettings& s = nppSettings();
    s.setValue(QStringLiteral("autoCompWord"), _cbAutoCompWord->isChecked());
    s.setValue(QStringLiteral("smartComp"), _cbSmartComp->isChecked());
    s.setValue(QStringLiteral("autoCompParenth"), _sbAutoCompParenth->value());
    s.setValue(QStringLiteral("autoCompBracket"), _sbAutoCompBracket->value());
    s.setValue(QStringLiteral("autoCompBrace"), _sbAutoCompBrace->value());
    s.setValue(QStringLiteral("rememberLastSession"), _cbRememberLastSession->isChecked());
    s.setValue(QStringLiteral("rememberCurrentSession"), _cbRememberCurrentSession->isChecked());
    s.setValue(QStringLiteral("multiInstance"), _cbMultiInstance->isChecked());
}

// =============================================================================
// EditingPage
// =============================================================================

EditingPage::EditingPage(QWidget* parent)
    : PreferencePage(parent)
{
    auto* layout = new QVBoxLayout(this);

    // Tab settings
    auto* tabGroup = new QGroupBox(QStringLiteral("Tab Settings"), this);
    auto* tabLayout = new QFormLayout(tabGroup);

    _sbTabSize = new QSpinBox(this);
    _sbTabSize->setRange(1, 8);
    _sbTabSize->setSuffix(QStringLiteral(" spaces"));
    tabLayout->addRow(QStringLiteral("Tab size:"), _sbTabSize);

    _cbReplaceBySpace = new QCheckBox(this);
    tabLayout->addRow(QStringLiteral("Replace by space"), _cbReplaceBySpace);

    _sbFolderDepth = new QSpinBox(this);
    _sbFolderDepth->setRange(0, 10);
    tabLayout->addRow(QStringLiteral("Folder auto-completion depth:"), _sbFolderDepth);

    layout->addWidget(tabGroup);

    // Indentation
    auto* indentGroup = new QGroupBox(QStringLiteral("Indentation"), this);
    auto* indentLayout = new QFormLayout(indentGroup);

    _cbAutoIndent = new QComboBox(this);
    _cbAutoIndent->addItems({QStringLiteral("None"), QStringLiteral("Basic"), QStringLiteral("Advanced")});
    indentLayout->addRow(QStringLiteral("Auto-indentation:"), _cbAutoIndent);

    layout->addWidget(indentGroup);

    // Display
    auto* displayGroup = new QGroupBox(QStringLiteral("Display"), this);
    auto* displayLayout = new QFormLayout(displayGroup);

    _cbEnableScintillaHomoglyph = new QCheckBox(this);
    displayLayout->addRow(QStringLiteral("Show homoglyphs (RTL/LTR):"), _cbEnableScintillaHomoglyph);

    _cbHighlightCurrentWord = new QCheckBox(this);
    displayLayout->addRow(QStringLiteral("Highlight current word:"), _cbHighlightCurrentWord);

    _cbHighlightLine = new QCheckBox(this);
    displayLayout->addRow(QStringLiteral("Highlight current line:"), _cbHighlightLine);

    layout->addWidget(displayGroup);
    layout->addStretch();
}

void EditingPage::load()
{
    QSettings& s = nppSettings();
    _sbTabSize->setValue(s.value(QStringLiteral("tabSize"), 4).toInt());
    _cbReplaceBySpace->setChecked(s.value(QStringLiteral("replaceBySpace"), false).toBool());
    _cbAutoIndent->setCurrentIndex(s.value(QStringLiteral("autoIndent"), 2).toInt());
    _sbFolderDepth->setValue(s.value(QStringLiteral("folderDepth"), 2).toInt());
    _cbEnableScintillaHomoglyph->setChecked(s.value(QStringLiteral("enableHomoglyph"), true).toBool());
    _cbHighlightCurrentWord->setChecked(s.value(QStringLiteral("highlightCurrentWord"), true).toBool());
    _cbHighlightLine->setChecked(s.value(QStringLiteral("highlightLine"), true).toBool());
}

void EditingPage::save()
{
    QSettings& s = nppSettings();
    s.setValue(QStringLiteral("tabSize"), _sbTabSize->value());
    s.setValue(QStringLiteral("replaceBySpace"), _cbReplaceBySpace->isChecked());
    s.setValue(QStringLiteral("autoIndent"), _cbAutoIndent->currentIndex());
    s.setValue(QStringLiteral("folderDepth"), _sbFolderDepth->value());
    s.setValue(QStringLiteral("enableHomoglyph"), _cbEnableScintillaHomoglyph->isChecked());
    s.setValue(QStringLiteral("highlightCurrentWord"), _cbHighlightCurrentWord->isChecked());
    s.setValue(QStringLiteral("highlightLine"), _cbHighlightLine->isChecked());
}

// =============================================================================
// LanguagePage
// =============================================================================

LanguagePage::LanguagePage(QWidget* parent)
    : PreferencePage(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* langGroup = new QGroupBox(QStringLiteral("Language Settings"), this);
    auto* langLayout = new QFormLayout(langGroup);

    _cbDefaultLanguage = new QComboBox(this);
    populateLanguageCombo();
    langLayout->addRow(QStringLiteral("Default language:"), _cbDefaultLanguage);

    layout->addWidget(langGroup);

    // Encoding group
    auto* encGroup = new QGroupBox(QStringLiteral("Encoding"), this);
    auto* encLayout = new QFormLayout(encGroup);

    _cbDetectEncoding = new QCheckBox(this);
    encLayout->addRow(QStringLiteral("Detect encoding from file:"), _cbDetectEncoding);

    _cbApplyToOpened = new QCheckBox(this);
    encLayout->addRow(QStringLiteral("Apply to opened files:"), _cbApplyToOpened);

    layout->addWidget(encGroup);
    layout->addStretch();
}

void LanguagePage::populateLanguageCombo()
{
    // Popular languages matching LangType enum in ScintillaComponent
    _cbDefaultLanguage->addItems({
        QStringLiteral("Normal Text"),
        QStringLiteral("C/C++"),
        QStringLiteral("C"),
        QStringLiteral("Java"),
        QStringLiteral("C#"),
        QStringLiteral("HTML"),
        QStringLiteral("XML"),
        QStringLiteral("JSON"),
        QStringLiteral("JavaScript"),
        QStringLiteral("Python"),
        QStringLiteral("PHP"),
        QStringLiteral("SQL"),
        QStringLiteral("Bash"),
        QStringLiteral("Perl"),
        QStringLiteral("Ruby"),
        QStringLiteral("Go"),
        QStringLiteral("Rust"),
        QStringLiteral("Markdown"),
        QStringLiteral("YAML"),
        QStringLiteral("Makefile"),
        QStringLiteral("Batch"),
        QStringLiteral("PowerShell"),
        QStringLiteral("CSS"),
        QStringLiteral("Pascal"),
        QStringLiteral("Fortran"),
        QStringLiteral("Fortran 77"),
        QStringLiteral("Lisp"),
        QStringLiteral("Scheme"),
        QStringLiteral("Smalltalk"),
        QStringLiteral("Prolog"),
        QStringLiteral("TCL"),
        QStringLiteral("Lua"),
        QStringLiteral("Verilog"),
        QStringLiteral("VHDL"),
    });
}

void LanguagePage::load()
{
    QSettings& s = nppSettings();
    int langIdx = s.value(QStringLiteral("defaultLanguage"), 0).toInt();
    _cbDefaultLanguage->setCurrentIndex(langIdx);
    _cbDetectEncoding->setChecked(s.value(QStringLiteral("detectEncoding"), true).toBool());
    _cbApplyToOpened->setChecked(s.value(QStringLiteral("applyEncodingToOpened"), false).toBool());
}

void LanguagePage::save()
{
    QSettings& s = nppSettings();
    s.setValue(QStringLiteral("defaultLanguage"), _cbDefaultLanguage->currentIndex());
    s.setValue(QStringLiteral("detectEncoding"), _cbDetectEncoding->isChecked());
    s.setValue(QStringLiteral("applyEncodingToOpened"), _cbApplyToOpened->isChecked());
}

// =============================================================================
// BackupPage
// =============================================================================

BackupPage::BackupPage(QWidget* parent)
    : PreferencePage(parent)
{
    auto* layout = new QVBoxLayout(this);

    // Auto-save
    auto* asGroup = new QGroupBox(QStringLiteral("Auto-save"), this);
    auto* asLayout = new QFormLayout(asGroup);

    _cbAutoSave = new QCheckBox(this);
    asLayout->addRow(QStringLiteral("Enable auto-save:"), _cbAutoSave);

    _sbAutoSaveInterval = new QSpinBox(this);
    _sbAutoSaveInterval->setRange(1, 60);
    _sbAutoSaveInterval->setSuffix(QStringLiteral(" min"));
    asLayout->addRow(QStringLiteral("Interval:"), _sbAutoSaveInterval);

    layout->addWidget(asGroup);

    // Session snapshot
    auto* snapGroup = new QGroupBox(QStringLiteral("Session Snapshot"), this);
    auto* snapLayout = new QFormLayout(snapGroup);

    _cbSessionSnapshot = new QCheckBox(this);
    snapLayout->addRow(QStringLiteral("Enable session snapshot:"), _cbSessionSnapshot);

    layout->addWidget(snapGroup);

    // Backup path
    auto* pathGroup = new QGroupBox(QStringLiteral("Backup Path"), this);
    auto* pathLayout = new QFormLayout(pathGroup);

    _leBackupPath = new QLineEdit(this);
    _leBackupPath->setPlaceholderText(QStringLiteral("Default: ~/.config/Notepad++/backup"));
    _btnBrowse = new QPushButton(QStringLiteral("Browse..."), this);
    connect(_btnBrowse, &QPushButton::clicked, this, &BackupPage::browseBackupPath);

    auto* pathRow = new QHBoxLayout;
    pathRow->addWidget(_leBackupPath);
    pathRow->addWidget(_btnBrowse);
    pathLayout->addRow(QStringLiteral("Path:"), pathRow);

    layout->addWidget(pathGroup);
    layout->addStretch();
}

void BackupPage::browseBackupPath()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        QStringLiteral("Select Backup Directory"),
        _leBackupPath->text(),
        QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        _leBackupPath->setText(dir);
}

void BackupPage::load()
{
    QSettings& s = nppSettings();
    _cbAutoSave->setChecked(s.value(QStringLiteral("autoSave"), false).toBool());
    _sbAutoSaveInterval->setValue(s.value(QStringLiteral("autoSaveInterval"), 5).toInt());
    _cbSessionSnapshot->setChecked(s.value(QStringLiteral("sessionSnapshot"), false).toBool());
    _leBackupPath->setText(s.value(QStringLiteral("backupPath"), QString()).toString());
}

void BackupPage::save()
{
    QSettings& s = nppSettings();
    s.setValue(QStringLiteral("autoSave"), _cbAutoSave->isChecked());
    s.setValue(QStringLiteral("autoSaveInterval"), _sbAutoSaveInterval->value());
    s.setValue(QStringLiteral("sessionSnapshot"), _cbSessionSnapshot->isChecked());
    s.setValue(QStringLiteral("backupPath"), _leBackupPath->text());
}

// =============================================================================
// PrintPage
// =============================================================================

PrintPage::PrintPage(QWidget* parent)
    : PreferencePage(parent)
{
    auto* layout = new QVBoxLayout(this);

    // Header/Footer
    auto* hfGroup = new QGroupBox(QStringLiteral("Header & Footer"), this);
    auto* hfLayout = new QFormLayout(hfGroup);

    _cbPrintHeader = new QCheckBox(this);
    hfLayout->addRow(QStringLiteral("Print header:"), _cbPrintHeader);

    layout->addWidget(hfGroup);

    // Options
    auto* optGroup = new QGroupBox(QStringLiteral("Options"), this);
    auto* optLayout = new QFormLayout(optGroup);

    _cbPrintLineNumbers = new QCheckBox(this);
    optLayout->addRow(QStringLiteral("Print line numbers:"), _cbPrintLineNumbers);

    _cbColorTheme = new QComboBox(this);
    _cbColorTheme->addItems({QStringLiteral("Light"), QStringLiteral("Dark")});
    optLayout->addRow(QStringLiteral("Color theme:"), _cbColorTheme);

    _cbPrintColorMode = new QComboBox(this);
    _cbPrintColorMode->addItems({
        QStringLiteral("Print Normal"),
        QStringLiteral("Print Inverted"),
        QStringLiteral("Print B&W"),
        QStringLiteral("Print Highlighted"),
    });
    optLayout->addRow(QStringLiteral("Color mode:"), _cbPrintColorMode);

    layout->addWidget(optGroup);

    // Margins
    auto* marginGroup = new QGroupBox(QStringLiteral("Margins (mm)"), this);
    auto* marginLayout = new QFormLayout(marginGroup);

    _sbMarginLeft = new QSpinBox(this);
    _sbMarginLeft->setRange(0, 50);
    marginLayout->addRow(QStringLiteral("Left:"), _sbMarginLeft);

    _sbMarginRight = new QSpinBox(this);
    _sbMarginRight->setRange(0, 50);
    marginLayout->addRow(QStringLiteral("Right:"), _sbMarginRight);

    _sbMarginTop = new QSpinBox(this);
    _sbMarginTop->setRange(0, 50);
    marginLayout->addRow(QStringLiteral("Top:"), _sbMarginTop);

    _sbMarginBottom = new QSpinBox(this);
    _sbMarginBottom->setRange(0, 50);
    marginLayout->addRow(QStringLiteral("Bottom:"), _sbMarginBottom);

    layout->addWidget(marginGroup);
    layout->addStretch();
}

void PrintPage::load()
{
    QSettings& s = nppSettings();
    _cbPrintHeader->setChecked(s.value(QStringLiteral("printHeader"), true).toBool());
    _cbPrintLineNumbers->setChecked(s.value(QStringLiteral("printLineNumbers"), false).toBool());
    _cbColorTheme->setCurrentIndex(s.value(QStringLiteral("printColorTheme"), 0).toInt());
    _cbPrintColorMode->setCurrentIndex(s.value(QStringLiteral("printColorMode"), 0).toInt());
    _sbMarginLeft->setValue(s.value(QStringLiteral("marginLeft"), 15).toInt());
    _sbMarginRight->setValue(s.value(QStringLiteral("marginRight"), 15).toInt());
    _sbMarginTop->setValue(s.value(QStringLiteral("marginTop"), 15).toInt());
    _sbMarginBottom->setValue(s.value(QStringLiteral("marginBottom"), 15).toInt());
}

void PrintPage::save()
{
    QSettings& s = nppSettings();
    s.setValue(QStringLiteral("printHeader"), _cbPrintHeader->isChecked());
    s.setValue(QStringLiteral("printLineNumbers"), _cbPrintLineNumbers->isChecked());
    s.setValue(QStringLiteral("printColorTheme"), _cbColorTheme->currentIndex());
    s.setValue(QStringLiteral("printColorMode"), _cbPrintColorMode->currentIndex());
    s.setValue(QStringLiteral("marginLeft"), _sbMarginLeft->value());
    s.setValue(QStringLiteral("marginRight"), _sbMarginRight->value());
    s.setValue(QStringLiteral("marginTop"), _sbMarginTop->value());
    s.setValue(QStringLiteral("marginBottom"), _sbMarginBottom->value());
}

// =============================================================================
// PreferenceDialog
// =============================================================================

PreferenceDialog::PreferenceDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Preferences"));
    setMinimumSize(700, 500);
    setupUi();
    createAllPages();
    loadAllPages();
}

void PreferenceDialog::setupUi()
{
    auto* mainLayout = new QHBoxLayout(this);

    // Left category list
    _categoryList = new QListWidget(this);
    _categoryList->setMaximumWidth(160);
    _categoryList->setSpacing(4);

    _categoryList->addItem(QStringLiteral("General"));
    _categoryList->addItem(QStringLiteral("Editing"));
    _categoryList->addItem(QStringLiteral("Language"));
    _categoryList->addItem(QStringLiteral("Backup"));
    _categoryList->addItem(QStringLiteral("Print"));
    _categoryList->setCurrentRow(0);

    connect(_categoryList, &QListWidget::currentItemChanged,
            this, &PreferenceDialog::onCategoryChanged);

    // Right stacked pages
    _stack = new QStackedWidget(this);

    // Buttons
    _buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &PreferenceDialog::onOk);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &PreferenceDialog::onCancel);
    connect(_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &PreferenceDialog::onApply);

    mainLayout->addWidget(_categoryList);
    mainLayout->addWidget(_stack, 1);

    auto* bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(_buttonBox);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addLayout(mainLayout, 1);
    outerLayout->addLayout(bottomLayout);
}

void PreferenceDialog::createAllPages()
{
    _generalPage = new GeneralPage(this);
    _editingPage = new EditingPage(this);
    _languagePage = new LanguagePage(this);
    _backupPage = new BackupPage(this);
    _printPage = new PrintPage(this);

    _stack->addWidget(_generalPage);
    _stack->addWidget(_editingPage);
    _stack->addWidget(_languagePage);
    _stack->addWidget(_backupPage);
    _stack->addWidget(_printPage);

    _pages = {_generalPage, _editingPage, _languagePage, _backupPage, _printPage};
}

void PreferenceDialog::loadAllPages()
{
    for (PreferencePage* page : _pages)
        page->load();
}

void PreferenceDialog::saveAllPages()
{
    for (PreferencePage* page : _pages)
        page->save();
    nppSettings().sync();
}

void PreferenceDialog::onCategoryChanged(QListWidgetItem* current, QListWidgetItem*)
{
    if (!current) return;
    _stack->setCurrentIndex(_categoryList->row(current));
    Q_UNUSED(current);
}

void PreferenceDialog::onOk()
{
    saveAllPages();
    accept();
}

void PreferenceDialog::onCancel()
{
    reject();
}

void PreferenceDialog::onApply()
{
    saveAllPages();
}
