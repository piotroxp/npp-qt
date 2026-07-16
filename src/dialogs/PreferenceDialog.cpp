// PreferenceDialog.cpp - Preferences/Settings dialog implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "PreferenceDialog.h"
#include "ShortcutMapperDialog.h"
#include "core/Application.h"
#include "editor/ScintillaEditor.h"
#include "ui/MainWindow.h"
#include "ui/MenuBar.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QListWidget>
#include <QTableWidget>
#include <QFileDialog>
#include <QRadioButton>
#include <QDesktopServices>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

PreferenceDialog::PreferenceDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Preferences");
    setMinimumSize(700, 500);
    setupUi();
    loadSettings();
}

PreferenceDialog::~PreferenceDialog() = default;

// ============================================================================
// UI Setup
// ============================================================================
void PreferenceDialog::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Left side: category tree
    _categoryTree = new QTreeWidget(this);
    _categoryTree->setHeaderHidden(true);
    _categoryTree->setMaximumWidth(180);
    _categoryTree->setIndentation(15);

    // Create category items
    QTreeWidgetItem* general = new QTreeWidgetItem(_categoryTree);
    general->setText(0, "General");
    general->setData(0, Qt::UserRole, 0);

    QTreeWidgetItem* editor = new QTreeWidgetItem(_categoryTree);
    editor->setText(0, "Editor");
    editor->setData(0, Qt::UserRole, 1);

    QTreeWidgetItem* appearance = new QTreeWidgetItem(_categoryTree);
    appearance->setText(0, "Appearance");
    appearance->setData(0, Qt::UserRole, 2);

    QTreeWidgetItem* fileAssoc = new QTreeWidgetItem(_categoryTree);
    fileAssoc->setText(0, "File Associations");
    fileAssoc->setData(0, Qt::UserRole, 3);

    QTreeWidgetItem* shortcuts = new QTreeWidgetItem(_categoryTree);
    shortcuts->setText(0, "Shortcut Mapper");
    shortcuts->setData(0, Qt::UserRole, 4);

    QTreeWidgetItem* margins = new QTreeWidgetItem(_categoryTree);
    margins->setText(0, "Margins & Borders");
    margins->setData(0, Qt::UserRole, 5);

    QTreeWidgetItem* backup = new QTreeWidgetItem(_categoryTree);
    backup->setText(0, "Backup/Auto-Save");
    backup->setData(0, Qt::UserRole, 6);

    QTreeWidgetItem* language = new QTreeWidgetItem(_categoryTree);
    language->setText(0, "Language");
    language->setData(0, Qt::UserRole, 7);

    QTreeWidgetItem* search = new QTreeWidgetItem(_categoryTree);
    search->setText(0, "Search");
    search->setData(0, Qt::UserRole, 8);

    // Right side: stacked widget with pages
    _pageStack = new QStackedWidget(this);

    _pageStack->addWidget(createGeneralPage());
    _pageStack->addWidget(createEditorPage());
    _pageStack->addWidget(createAppearancePage());
    _pageStack->addWidget(createFileAssociationPage());
    _pageStack->addWidget(createShortcutPage());
    _pageStack->addWidget(createMarginsPage());
    _pageStack->addWidget(createBackupPage());
    _pageStack->addWidget(createLanguagePage());
    _pageStack->addWidget(createSearchPage());

    // Button box
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    _buttonBox->button(QDialogButtonBox::Apply)->setText("Apply");

    // Connect signals
    connect(_categoryTree, &QTreeWidget::itemClicked, this, &PreferenceDialog::onCategoryChanged);
    connect(_buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &PreferenceDialog::accept);
    connect(_buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &PreferenceDialog::reject);
    connect(_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferenceDialog::applySettings);

    // Layout
    mainLayout->addWidget(_categoryTree);
    mainLayout->addWidget(_pageStack, 1);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(_pageStack);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(_buttonBox);

    QWidget* rightWidget = new QWidget(this);
    rightWidget->setLayout(rightLayout);
    mainLayout->addWidget(rightWidget, 1);

    // Select first item by default
    _categoryTree->setCurrentItem(general);
}

// ============================================================================
// General Page
// ============================================================================
QWidget* PreferenceDialog::createGeneralPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Startup section
    QGroupBox* startupGroup = new QGroupBox("Startup", page);
    QVBoxLayout* startupLayout = new QVBoxLayout(startupGroup);

    _chkSingleInstance = new QCheckBox("Enable single instance mode", startupGroup);
    startupLayout->addWidget(_chkSingleInstance);

    _chkRememberSession = new QCheckBox("Remember current session on exit", startupGroup);
    startupLayout->addWidget(_chkRememberSession);

    layout->addWidget(startupGroup);

    // Recent Files section
    QGroupBox* recentGroup = new QGroupBox("Recent Files", page);
    QGridLayout* recentLayout = new QGridLayout(recentGroup);

    recentLayout->addWidget(new QLabel("Maximum recent files:", recentGroup), 0, 0);
    _spinMaxRecentFiles = new QSpinBox(recentGroup);
    _spinMaxRecentFiles->setRange(0, 100);
    _spinMaxRecentFiles->setSpecialValueText("No limit");
    recentLayout->addWidget(_spinMaxRecentFiles, 0, 1);

    recentLayout->setColumnStretch(2, 1);
    layout->addWidget(recentGroup);

    // Actions
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    _btnImportNpp = new QPushButton("Import from Notepad++...", page);
    _btnExportNpp = new QPushButton("Export to Notepad++...", page);
    _btnResetDefaults = new QPushButton("Reset to Defaults", page);
    btnRow->addWidget(_btnImportNpp);
    btnRow->addWidget(_btnExportNpp);
    btnRow->addWidget(_btnResetDefaults);
    layout->addLayout(btnRow);

    connect(_btnImportNpp, &QPushButton::clicked, this, &PreferenceDialog::onImportFromNpp);
    connect(_btnExportNpp, &QPushButton::clicked, this, &PreferenceDialog::onExportToNpp);
    connect(_btnResetDefaults, &QPushButton::clicked, this, &PreferenceDialog::resetToDefaults);

    layout->addStretch();

    return page;
}

// ============================================================================
// Editor Page
// ============================================================================
QWidget* PreferenceDialog::createEditorPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Tab Settings
    QGroupBox* tabGroup = new QGroupBox("Tab Settings", page);
    QGridLayout* tabLayout = new QGridLayout(tabGroup);

    tabLayout->addWidget(new QLabel("Tab width:", tabGroup), 0, 0);
    _spinTabWidth = new QSpinBox(tabGroup);
    _spinTabWidth->setRange(1, 16);
    tabLayout->addWidget(_spinTabWidth, 0, 1);

    tabLayout->addWidget(new QLabel("Indent width:", tabGroup), 1, 0);
    _spinIndentWidth = new QSpinBox(tabGroup);
    _spinIndentWidth->setRange(1, 16);
    tabLayout->addWidget(_spinIndentWidth, 1, 1);

    _chkTabAsSpaces = new QCheckBox("Replace by spaces", tabGroup);
    tabLayout->addWidget(_chkTabAsSpaces, 2, 0, 1, 2);

    layout->addWidget(tabGroup);

    // Language Settings
    QGroupBox* langGroup = new QGroupBox("Language Settings", page);
    QGridLayout* langLayout = new QGridLayout(langGroup);

    langLayout->addWidget(new QLabel("Default EOL:", langGroup), 0, 0);
    _cmbEolMode = new QComboBox(langGroup);
    _cmbEolMode->addItem("Unix (LF)", QVariant(static_cast<int>(EolType::EOL_LF)));
    _cmbEolMode->addItem("Windows (CRLF)", QVariant(static_cast<int>(EolType::EOL_CRLF)));
    _cmbEolMode->addItem("Mac (CR)", QVariant(static_cast<int>(EolType::EOL_CR)));
    langLayout->addWidget(_cmbEolMode, 0, 1);

    langLayout->addWidget(new QLabel("Default encoding:", langGroup), 1, 0);
    _cmbDefaultEncoding = new QComboBox(langGroup);
    _cmbDefaultEncoding->addItem("UTF-8", QVariant(static_cast<int>(EncodingType::UTF_8)));
    _cmbDefaultEncoding->addItem("UTF-8 with BOM", QVariant(static_cast<int>(EncodingType::UTF_8_BOM)));
    _cmbDefaultEncoding->addItem("UTF-16 LE", QVariant(static_cast<int>(EncodingType::UTF_16_LE)));
    _cmbDefaultEncoding->addItem("UTF-16 BE", QVariant(static_cast<int>(EncodingType::UTF_16_BE)));
    _cmbDefaultEncoding->addItem("ANSI", QVariant(static_cast<int>(EncodingType::ANSI)));
    langLayout->addWidget(_cmbDefaultEncoding, 1, 1);

    layout->addWidget(langGroup);

    // Editing Behavior
    QGroupBox* editGroup = new QGroupBox("Editing Behavior", page);
    QVBoxLayout* editLayout = new QVBoxLayout(editGroup);

    _chkAutoIndent = new QCheckBox("Auto-indent", editGroup);
    editLayout->addWidget(_chkAutoIndent);

    _chkWordWrap = new QCheckBox("Word wrap", editGroup);
    editLayout->addWidget(_chkWordWrap);

    _chkVirtualSpace = new QCheckBox("Allow cursor past end of line (virtual space)", editGroup);
    editLayout->addWidget(_chkVirtualSpace);

    _chkSmartHome = new QCheckBox("Smart home (jump to indent, then BOL)", editGroup);
    editLayout->addWidget(_chkSmartHome);

    _chkWrapWithQuotes = new QCheckBox("Wrap with quotes (selection)", editGroup);
    editLayout->addWidget(_chkWrapWithQuotes);

    _chkBackspaceUnindent = new QCheckBox("Backspace unindents", editGroup);
    editLayout->addWidget(_chkBackspaceUnindent);

    _chkShowEol = new QCheckBox("Show end-of-line characters", editGroup);
    editLayout->addWidget(_chkShowEol);

    _chkShowSpaceTab = new QCheckBox("Show whitespace/tab characters", editGroup);
    editLayout->addWidget(_chkShowSpaceTab);

    layout->addWidget(editGroup);
    layout->addStretch();

    return page;
}

// ============================================================================
// Appearance Page
// ============================================================================
QWidget* PreferenceDialog::createAppearancePage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Theme
    QGroupBox* themeGroup = new QGroupBox("Theme", page);
    QGridLayout* themeLayout = new QGridLayout(themeGroup);

    themeLayout->addWidget(new QLabel("Theme:", themeGroup), 0, 0);
    _cmbTheme = new QComboBox(themeGroup);
    _cmbTheme->addItem("Default (Light)", "default");
    _cmbTheme->addItem("Dark", "dark");
    _cmbTheme->addItem("Light", "light");
    _cmbTheme->addItem("High Contrast", "high-contrast");
    themeLayout->addWidget(_cmbTheme, 0, 1);

    layout->addWidget(themeGroup);

    // UI Elements
    QGroupBox* uiGroup = new QGroupBox("User Interface", page);
    QVBoxLayout* uiLayout = new QVBoxLayout(uiGroup);

    _chkShowMenubar = new QCheckBox("Show menu bar", uiGroup);
    uiLayout->addWidget(_chkShowMenubar);

    _chkShowToolbar = new QCheckBox("Show toolbar", uiGroup);
    uiLayout->addWidget(_chkShowToolbar);

    _chkShowTabbar = new QCheckBox("Show tab bar", uiGroup);
    uiLayout->addWidget(_chkShowTabbar);

    _chkShowStatusbar = new QCheckBox("Show status bar", uiGroup);
    uiLayout->addWidget(_chkShowStatusbar);

    layout->addWidget(uiGroup);

    // Editor Appearance
    QGroupBox* editorApprGroup = new QGroupBox("Editor Appearance", page);
    QVBoxLayout* edApprLayout = new QVBoxLayout(editorApprGroup);

    _chkCurrentLineHighlight = new QCheckBox(
        "Highlight current line background", editorApprGroup);
    edApprLayout->addWidget(_chkCurrentLineHighlight);

    _chkShowIndentGuide = new QCheckBox(
        "Show indentation guide", editorApprGroup);
    edApprLayout->addWidget(_chkShowIndentGuide);

    layout->addWidget(editorApprGroup);

    // Theme Actions
    QHBoxLayout* themeBtnRow = new QHBoxLayout();
    themeBtnRow->addStretch();
    _btnOpenThemesFolder = new QPushButton("Open Themes Folder...", page);
    _btnReloadThemes = new QPushButton("Reload Themes", page);
    themeBtnRow->addWidget(_btnOpenThemesFolder);
    themeBtnRow->addWidget(_btnReloadThemes);
    layout->addLayout(themeBtnRow);

    connect(_btnOpenThemesFolder, &QPushButton::clicked, this, &PreferenceDialog::onOpenThemesFolder);
    connect(_btnReloadThemes, &QPushButton::clicked, this, &PreferenceDialog::onReloadThemes);

    layout->addStretch();
    return page;
}

// ============================================================================
// File Associations Page
// ============================================================================
// ============================================================================
// File Associations Page
// ============================================================================
QWidget* PreferenceDialog::createFileAssociationPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Info text
    layout->addWidget(new QLabel(
        "Associate file extensions with Notepad--Qt. "
        "Separate multiple extensions with commas (e.g. .txt, .md, .log).",
        page));

    QGroupBox* extGroup = new QGroupBox("Extension Mappings", page);
    QVBoxLayout* extLayout = new QVBoxLayout(extGroup);

    // Custom extensions list
    _extListWidget = new QListWidget(extGroup);
    _extListWidget->setAlternatingRowColors(true);
    _extListWidget->setToolTip("Double-click to edit, Delete key to remove");
    extLayout->addWidget(_extListWidget);

    // Add / Remove buttons
    QHBoxLayout* extBtnRow = new QHBoxLayout();
    QPushButton* addExtBtn = new QPushButton("Add...", extGroup);
    QPushButton* removeExtBtn = new QPushButton("Remove", extGroup);
    QPushButton* clearExtBtn = new QPushButton("Clear All", extGroup);
    extBtnRow->addWidget(addExtBtn);
    extBtnRow->addWidget(removeExtBtn);
    extBtnRow->addWidget(clearExtBtn);
    extBtnRow->addStretch();
    extLayout->addLayout(extBtnRow);
    layout->addWidget(extGroup);

    // Extension entry row
    QHBoxLayout* entryRow = new QHBoxLayout();
    entryRow->addWidget(new QLabel("Extension(s):", page));
    _extLineEdit = new QLineEdit(page);
    _extLineEdit->setPlaceholderText(".txt, .md, .log");
    entryRow->addWidget(_extLineEdit, 1);
    layout->addLayout(entryRow);

    // Buttons
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    QPushButton* importBtn = new QPushButton("Import from System", page);
    QPushButton* exportBtn = new QPushButton("Export", page);
    btnRow->addWidget(importBtn);
    btnRow->addWidget(exportBtn);
    layout->addLayout(btnRow);

    // Connections
    connect(addExtBtn, &QPushButton::clicked, this, [this, page]() {
        QString text = _extLineEdit->text().trimmed();
        if (text.isEmpty()) return;
        for (const QString& ext : text.split(',', Qt::SkipEmptyParts)) {
            QString clean = ext.trimmed();
            if (!clean.startsWith('.')) clean = "." + clean;
            QList<QListWidgetItem*> existing = _extListWidget->findItems(clean, Qt::MatchExactly);
            if (existing.isEmpty())
                _extListWidget->addItem(clean);
        }
        _extLineEdit->clear();
    });
    connect(removeExtBtn, &QPushButton::clicked, this, [this]() {
        qDeleteAll(_extListWidget->selectedItems());
    });
    connect(clearExtBtn, &QPushButton::clicked, this, [this]() {
        _extListWidget->clear();
    });

    layout->addStretch();
    return page;
}

// ============================================================================
// Shortcut Mapper Page
// ============================================================================
QWidget* PreferenceDialog::createShortcutPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* info = new QLabel(
        "Click 'Open Shortcut Mapper' to view and customize keyboard shortcuts "
        "for all menu commands and Scintilla editor commands.", page);
    info->setWordWrap(true);
    layout->addWidget(info);

    QPushButton* openMapper = new QPushButton("Open Shortcut Mapper...", page);
    openMapper->setMinimumHeight(36);
    layout->addWidget(openMapper);

    // Conflict detection info
    QGroupBox* conflictBox = new QGroupBox("Conflict Detection", page);
    QVBoxLayout* conflictLayout = new QVBoxLayout(conflictBox);
    _chkConflictWarning = new QCheckBox(
        "Warn when a shortcut conflicts with an existing binding", conflictBox);
    _chkConflictWarning->setChecked(true);
    conflictLayout->addWidget(_chkConflictWarning);
    layout->addWidget(conflictBox);

    // Import / Export
    QGroupBox* impBox = new QGroupBox("Import / Export", page);
    QHBoxLayout* impRow = new QHBoxLayout(impBox);
    impRow->addWidget(new QPushButton("Import Shortcuts...", impBox));
    impRow->addWidget(new QPushButton("Export Shortcuts...", impBox));
    impRow->addStretch();
    layout->addWidget(impBox);

    layout->addStretch();

    connect(openMapper, &QPushButton::clicked, this, [this]() {
        // Hide pref dialog, open mapper, re-show when mapper closes
        this->hide();
        ShortcutMapperDialog mapper(this);
        mapper.exec();
        this->show();
    });

    return page;
}

// ============================================================================
// Margins & Borders Page
// ============================================================================
QWidget* PreferenceDialog::createMarginsPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Line Numbers
    QGroupBox* lnGroup = new QGroupBox("Line Number Margin", page);
    QGridLayout* lnGrid = new QGridLayout(lnGroup);
    _chkShowLineNumbers = new QCheckBox("Show line numbers", lnGroup);
    lnGrid->addWidget(_chkShowLineNumbers, 0, 0, 1, 2);

    lnGrid->addWidget(new QLabel("Width (pixels):", lnGroup), 1, 0);
    _spinLineNumberWidth = new QSpinBox(lnGroup);
    _spinLineNumberWidth->setRange(20, 200);
    _spinLineNumberWidth->setSuffix(" px");
    lnGrid->addWidget(_spinLineNumberWidth, 1, 1);
    layout->addWidget(lnGroup);

    // Symbol / Marker Margin
    QGroupBox* symGroup = new QGroupBox("Symbol Margin", page);
    QGridLayout* symGrid = new QGridLayout(symGroup);
    _chkShowSymbols = new QCheckBox("Show symbol margin", symGroup);
    _chkShowFolder = new QCheckBox("Show folder (expand/collapse) margin", symGroup);
    symGrid->addWidget(_chkShowSymbols, 0, 0);
    symGrid->addWidget(_chkShowFolder, 1, 0);
    symGrid->addWidget(new QLabel("Symbol margin width:", symGroup), 2, 0);
    _spinSymbolMarginWidth = new QSpinBox(symGroup);
    _spinSymbolMarginWidth->setRange(10, 80);
    _spinSymbolMarginWidth->setSuffix(" px");
    symGrid->addWidget(_spinSymbolMarginWidth, 2, 1);
    layout->addWidget(symGroup);

    // Current Line Highlight
    QGroupBox* curLineGroup = new QGroupBox("Current Line", page);
    QGridLayout* curLineGrid = new QGridLayout(curLineGroup);
    _chkHighlightCurrentLine = new QCheckBox(
        "Highlight current line background", curLineGroup);
    curLineGrid->addWidget(_chkHighlightCurrentLine, 0, 0);
    curLineGrid->addWidget(new QLabel("Highlight color:", curLineGroup), 1, 0);
    // Color indicator — a read-only line showing the color
    QLabel* colorPreview = new QLabel(curLineGroup);
    colorPreview->setFixedHeight(22);
    colorPreview->setStyleSheet("background: #FFFFD0; border: 1px solid gray; border-radius: 3px;");
    curLineGrid->addWidget(colorPreview, 1, 1);
    layout->addWidget(curLineGroup);

    // Border Edge
    QGroupBox* borderGroup = new QGroupBox("Edge / Right Margin", page);
    QGridLayout* borderGrid = new QGridLayout(borderGroup);
    _chkShowEdgeLine = new QCheckBox("Show vertical edge line", borderGroup);
    borderGrid->addWidget(_chkShowEdgeLine, 0, 0);
    borderGrid->addWidget(new QLabel("Edge column:", borderGroup), 1, 0);
    _spinEdgeColumn = new QSpinBox(borderGroup);
    _spinEdgeColumn->setRange(0, 500);
    borderGrid->addWidget(_spinEdgeColumn, 1, 1);
    borderGrid->addWidget(new QLabel("(0 = disabled)", borderGroup), 1, 2);
    layout->addWidget(borderGroup);

    layout->addStretch();
    return page;
}

// ============================================================================
// Backup / Auto-Save Page
// ============================================================================
QWidget* PreferenceDialog::createBackupPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Auto-Save
    QGroupBox* autoSaveGroup = new QGroupBox("Auto-Save", page);
    QGridLayout* asGrid = new QGridLayout(autoSaveGroup);

    _chkAutoSave = new QCheckBox("Enable auto-save", autoSaveGroup);
    asGrid->addWidget(_chkAutoSave, 0, 0, 1, 3);

    asGrid->addWidget(new QLabel("Interval:", autoSaveGroup), 1, 0);
    _spinAutoSaveInterval = new QSpinBox(autoSaveGroup);
    _spinAutoSaveInterval->setRange(1, 1440);
    _spinAutoSaveInterval->setSuffix(" min");
    _spinAutoSaveInterval->setToolTip("How often to auto-save (in minutes)");
    asGrid->addWidget(_spinAutoSaveInterval, 1, 1);

    _grpAutoSaveOptions = new QGroupBox("Auto-save options", autoSaveGroup);
    QVBoxLayout* asOptLayout = new QVBoxLayout(_grpAutoSaveOptions);
    _chkAutoSaveCurrentOnly = new QCheckBox(
        "Auto-save current file only (not all open files)", _grpAutoSaveOptions);
    _chkAutoSaveInBackground = new QCheckBox(
        "Save silently without switching focus", _grpAutoSaveOptions);
    asOptLayout->addWidget(_chkAutoSaveCurrentOnly);
    asOptLayout->addWidget(_chkAutoSaveInBackground);
    asGrid->addWidget(_grpAutoSaveOptions, 2, 0, 1, 3);
    layout->addWidget(autoSaveGroup);

    // Backup
    QGroupBox* backupGroup = new QGroupBox("Backup", page);
    QGridLayout* bkGrid = new QGridLayout(backupGroup);

    bkGrid->addWidget(new QLabel("Backup directory:", backupGroup), 0, 0);
    _backupDirEdit = new QLineEdit(backupGroup);
    _backupDirEdit->setPlaceholderText("(same as original file)");
    bkGrid->addWidget(_backupDirEdit, 0, 1);
    QPushButton* browseBackupBtn = new QPushButton("Browse...", backupGroup);
    bkGrid->addWidget(browseBackupBtn, 0, 2);

    bkGrid->addWidget(new QLabel("Backup style:", backupGroup), 1, 0);
    _backupStyleCombo = new QComboBox(backupGroup);
    _backupStyleCombo->addItems({
        "Simple (single backup file)",
        "With date/time (one file per save)",
        "Numbered (backup_1, backup_2, ...)"
    });
    bkGrid->addWidget(_backupStyleCombo, 1, 1, 1, 2);

    bkGrid->addWidget(new QLabel("Keep up to:", backupGroup), 2, 0);
    _spinMaxBackups = new QSpinBox(backupGroup);
    _spinMaxBackups->setRange(1, 100);
    _spinMaxBackups->setSuffix(" backups");
    _spinMaxBackups->setToolTip("Maximum numbered backups to keep (numbered style only)");
    bkGrid->addWidget(_spinMaxBackups, 2, 1, 1, 2);
    layout->addWidget(backupGroup);

    // Connections
    connect(_chkAutoSave, &QCheckBox::toggled, this, [this](bool on) {
        _spinAutoSaveInterval->setEnabled(on);
        _grpAutoSaveOptions->setEnabled(on);
    });
    connect(browseBackupBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this,
            "Select Backup Directory", _backupDirEdit->text());
        if (!dir.isEmpty()) _backupDirEdit->setText(dir);
    });

    layout->addStretch();
    return page;
}

// ============================================================================
// Language Page
// ============================================================================
QWidget* PreferenceDialog::createLanguagePage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Default Language
    QGroupBox* langGroup = new QGroupBox("Default Language", page);
    QGridLayout* langGrid = new QGridLayout(langGroup);

    langGrid->addWidget(new QLabel("Language:", langGroup), 0, 0);
    _cmbLanguage = new QComboBox(langGroup);
    _cmbLanguage->addItems({
        "None / Normal Text",
        "C / C++",
        "C#",
        "D",
        "Go",
        "HTML",
        "Java",
        "JavaScript",
        "JSON",
        "Kotlin",
        "Lua",
        "Markdown",
        "Objective-C",
        "Pascal",
        "Perl",
        "PHP",
        "Python",
        "Ruby",
        "Rust",
        "SQL",
        "Swift",
        "Tcl",
        "Tex / LaTeX",
        "Vala",
        "XML",
        "YAML"
    });
    langGrid->addWidget(_cmbLanguage, 0, 1);
    langGrid->setColumnStretch(2, 1);
    layout->addWidget(langGroup);

    // Auto-Detection
    QGroupBox* detectGroup = new QGroupBox("Auto-Detection", page);
    QVBoxLayout* detectLayout = new QVBoxLayout(detectGroup);

    _chkAutoDetectLanguage = new QCheckBox(
        "Automatically detect language from file extension", detectGroup);
    detectLayout->addWidget(_chkAutoDetectLanguage);

    QLabel* info = new QLabel(
        "When enabled, the language is automatically selected based on "
        "the file extension when opening a file.", detectGroup);
    info->setWordWrap(true);
    info->setStyleSheet("color: gray; font-style: italic;");
    detectLayout->addWidget(info);

    layout->addWidget(detectGroup);
    layout->addStretch();

    return page;
}

// ============================================================================
// Search Page
// ============================================================================
QWidget* PreferenceDialog::createSearchPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Highlighting
    QGroupBox* hlGroup = new QGroupBox("Highlighting", page);
    QGridLayout* hlGrid = new QGridLayout(hlGroup);

    _chkSmartHighlighting = new QCheckBox(
        "Highlight all matches (smart highlighting)", hlGroup);
    hlGrid->addWidget(_chkSmartHighlighting, 0, 0, 1, 2);

    hlGrid->addWidget(new QLabel("Maximum words to highlight:", hlGroup), 1, 0);
    _spinMaxHighlightWords = new QSpinBox(hlGroup);
    _spinMaxHighlightWords->setRange(100, 10000);
    _spinMaxHighlightWords->setSuffix(" words");
    _spinMaxHighlightWords->setToolTip("Limit for performance. Too many highlights can slow the editor.");
    hlGrid->addWidget(_spinMaxHighlightWords, 1, 1);
    hlGrid->setColumnStretch(2, 1);
    layout->addWidget(hlGroup);

    // Search Behavior
    QGroupBox* searchGroup = new QGroupBox("Search Behavior", page);
    QVBoxLayout* searchLayout = new QVBoxLayout(searchGroup);

    _chkMatchCase = new QCheckBox("Match case by default", searchGroup);
    searchLayout->addWidget(_chkMatchCase);

    _chkMatchWholeWord = new QCheckBox("Match whole word by default", searchGroup);
    searchLayout->addWidget(_chkMatchWholeWord);

    _chkWrapAround = new QCheckBox("Wrap around when reaching end of file", searchGroup);
    searchLayout->addWidget(_chkWrapAround);

    layout->addWidget(searchGroup);

    // Find History
    QGroupBox* histGroup = new QGroupBox("Find History", page);
    QGridLayout* histGrid = new QGridLayout(histGroup);

    histGrid->addWidget(new QLabel("Number of entries to remember:", histGroup), 0, 0);
    _spinFindHistoryCount = new QSpinBox(histGroup);
    _spinFindHistoryCount->setRange(5, 100);
    histGrid->addWidget(_spinFindHistoryCount, 0, 1);
    histGrid->setColumnStretch(2, 1);
    layout->addWidget(histGroup);

    layout->addStretch();
    return page;
}

// ============================================================================
// Load Settings
// ============================================================================
void PreferenceDialog::loadSettings() {
    const AppOptions& opts = app().options();

    // General
    _chkSingleInstance->setChecked(opts.singleInstance);
    _chkRememberSession->setChecked(opts.rememberSession);
    _spinMaxRecentFiles->setValue(opts.maxRecentFiles);

    // Editor
    _spinTabWidth->setValue(opts.defaultTabWidth);
    _spinIndentWidth->setValue(opts.defaultTabWidth);  // same as tab width by default
    _chkTabAsSpaces->setChecked(opts.defaultTabAsSpaces);
    _chkWordWrap->setChecked(opts.wordWrap);
    _chkVirtualSpace->setChecked(opts.virtualSpace);
    _chkSmartHome->setChecked(opts.smartHome);

    int eolIdx = _cmbEolMode->findData(QVariant(static_cast<int>(opts.defaultEolType)));
    if (eolIdx >= 0) _cmbEolMode->setCurrentIndex(eolIdx);

    int encIdx = _cmbDefaultEncoding->findData(QVariant(static_cast<int>(opts.defaultEncoding)));
    if (encIdx >= 0) _cmbDefaultEncoding->setCurrentIndex(encIdx);

    _chkAutoIndent->setChecked(opts.autoIndent);
    _chkWrapWithQuotes->setChecked(opts.wrapWithQuotes);
    _chkBackspaceUnindent->setChecked(opts.backspaceUnindent);
    _chkShowEol->setChecked(opts.showEol);
    _chkShowSpaceTab->setChecked(opts.showSpaceTab);

    // Appearance
    int themeIdx = _cmbTheme->findData(QString::fromUtf8(opts.themeProfile.c_str()));
    if (themeIdx >= 0) _cmbTheme->setCurrentIndex(themeIdx);

    _chkShowToolbar->setChecked(opts.showToolBar);
    _chkShowTabbar->setChecked(opts.showTabBar);
    _chkShowStatusbar->setChecked(opts.showStatusBar);
    _chkShowMenubar->setChecked(opts.showMenuBar);
    _chkCurrentLineHighlight->setChecked(opts.currentLineHighlight);
    _chkShowIndentGuide->setChecked(opts.showIndentGuide);

    // File Associations
    _extListWidget->clear();
    for (const QString& ext : opts.fileAssociations)
        _extListWidget->addItem(ext);

    // Shortcut Mapper
    _chkConflictWarning->setChecked(opts.warnOnShortcutConflict);

    // Margins
    _chkShowLineNumbers->setChecked(opts.showLineNumbers);
    _spinLineNumberWidth->setValue(opts.lineNumberWidth);
    _chkShowSymbols->setChecked(opts.showSymbols);
    _chkShowFolder->setChecked(opts.showFolderMargin);
    _spinSymbolMarginWidth->setValue(opts.symbolMarginWidth);
    _chkHighlightCurrentLine->setChecked(opts.highlightCurrentLine);
    _chkShowEdgeLine->setChecked(opts.showEdgeLine);
    _spinEdgeColumn->setValue(opts.edgeColumn);

    // Backup / Auto-Save
    _chkAutoSave->setChecked(opts.autoSave);
    _spinAutoSaveInterval->setValue(opts.autoSaveInterval > 0 ? opts.autoSaveInterval : 5);
    _chkAutoSaveCurrentOnly->setChecked(opts.autoSaveCurrentOnly);
    _chkAutoSaveInBackground->setChecked(opts.autoSaveInBackground);
    _backupDirEdit->setText(opts.backupDir);
    _backupStyleCombo->setCurrentIndex(opts.backupStyle);
    _spinMaxBackups->setValue(opts.maxBackups);
    _grpAutoSaveOptions->setEnabled(opts.autoSave);
    _spinAutoSaveInterval->setEnabled(opts.autoSave);

    // Language
    if (_cmbLanguage) {
        // Map LangType to combo index (simplified — full mapping would need LangType enum)
        _cmbLanguage->setCurrentIndex(0);  // "None / Normal Text" as default
    }
    if (_chkAutoDetectLanguage)
        _chkAutoDetectLanguage->setChecked(opts.autoDetectLanguage);

    // Search
    if (_chkSmartHighlighting)
        _chkSmartHighlighting->setChecked(opts.smartHighlighting);
    if (_spinMaxHighlightWords)
        _spinMaxHighlightWords->setValue(opts.maxHighlightWords);
    if (_chkMatchCase)
        _chkMatchCase->setChecked(opts.matchCaseDefault);
    if (_chkMatchWholeWord)
        _chkMatchWholeWord->setChecked(opts.matchWholeWordDefault);
    if (_chkWrapAround)
        _chkWrapAround->setChecked(opts.wrapAroundDefault);
    if (_spinFindHistoryCount)
        _spinFindHistoryCount->setValue(opts.findHistoryCount);

    // Store original values
    _originalSettings.singleInstance = opts.singleInstance;
    _originalSettings.rememberSession = opts.rememberSession;
    _originalSettings.maxRecentFiles = opts.maxRecentFiles;
    _originalSettings.tabWidth = opts.defaultTabWidth;
    _originalSettings.indentWidth = opts.defaultTabWidth;
    _originalSettings.tabAsSpaces = opts.defaultTabAsSpaces;
    _originalSettings.wordWrap = opts.wordWrap;
    _originalSettings.virtualSpace = opts.virtualSpace;
    _originalSettings.smartHome = opts.smartHome;
    _originalSettings.eolMode = _cmbEolMode->currentData().toInt();
    _originalSettings.defaultEncoding = _cmbDefaultEncoding->currentData().toInt();
    _originalSettings.autoIndent = opts.autoIndent;
    _originalSettings.wrapWithQuotes = opts.wrapWithQuotes;
    _originalSettings.backspaceUnindent = opts.backspaceUnindent;
    _originalSettings.theme = _cmbTheme->currentData().toString();
    _originalSettings.showToolbar = opts.showToolBar;
    _originalSettings.currentLineHighlight = opts.currentLineHighlight;
    _originalSettings.showIndentGuide = opts.showIndentGuide;
    _originalSettings.showTabbar = opts.showTabBar;
    _originalSettings.showStatusbar = opts.showStatusBar;
    _originalSettings.showMenubar = opts.showMenuBar;
}

// ============================================================================
// Apply Settings
// ============================================================================
void PreferenceDialog::applySettings() {
    AppOptions& opts = app().options();

    // General
    opts.singleInstance = _chkSingleInstance->isChecked();
    opts.rememberSession = _chkRememberSession->isChecked();
    opts.maxRecentFiles = _spinMaxRecentFiles->value();

    // Editor
    opts.defaultTabWidth = _spinTabWidth->value();
    opts.indentWidth = _spinIndentWidth->value();
    opts.defaultTabAsSpaces = _chkTabAsSpaces->isChecked();
    opts.wordWrap = _chkWordWrap->isChecked();
    opts.virtualSpace = _chkVirtualSpace->isChecked();
    opts.smartHome = _chkSmartHome->isChecked();
    opts.autoIndent = _chkAutoIndent->isChecked();

    EolType eolType = static_cast<EolType>(_cmbEolMode->currentData().toInt());
    opts.defaultEolType = eolType;

    EncodingType encType = static_cast<EncodingType>(_cmbDefaultEncoding->currentData().toInt());
    opts.defaultEncoding = encType;

    opts.wrapWithQuotes = _chkWrapWithQuotes->isChecked();
    opts.backspaceUnindent = _chkBackspaceUnindent->isChecked();
    opts.showEol = _chkShowEol->isChecked();
    opts.showSpaceTab = _chkShowSpaceTab->isChecked();

    // Appearance
    opts.themeProfile = _cmbTheme->currentData().toString().toUtf8().constData();
    opts.showToolBar = _chkShowToolbar->isChecked();
    opts.showTabBar = _chkShowTabbar->isChecked();
    opts.showStatusBar = _chkShowStatusbar->isChecked();
    opts.showMenuBar = _chkShowMenubar->isChecked();
    opts.currentLineHighlight = _chkCurrentLineHighlight->isChecked();
    opts.showIndentGuide = _chkShowIndentGuide->isChecked();

    // File Associations — collect from list widget
    opts.fileAssociations.clear();
    for (int i = 0; i < _extListWidget->count(); ++i)
        opts.fileAssociations.append(_extListWidget->item(i)->text());

    // Shortcut Mapper
    opts.warnOnShortcutConflict = _chkConflictWarning->isChecked();

    // Margins
    opts.showLineNumbers = _chkShowLineNumbers->isChecked();
    opts.lineNumberWidth = _spinLineNumberWidth->value();
    opts.showSymbols = _chkShowSymbols->isChecked();
    opts.showFolderMargin = _chkShowFolder->isChecked();
    opts.symbolMarginWidth = _spinSymbolMarginWidth->value();
    opts.highlightCurrentLine = _chkHighlightCurrentLine->isChecked();
    opts.showEdgeLine = _chkShowEdgeLine->isChecked();
    opts.edgeColumn = _spinEdgeColumn->value();

    // Backup / Auto-Save
    opts.autoSave = _chkAutoSave->isChecked();
    opts.autoSaveInterval = _spinAutoSaveInterval->value();
    opts.autoSaveCurrentOnly = _chkAutoSaveCurrentOnly->isChecked();
    opts.autoSaveInBackground = _chkAutoSaveInBackground->isChecked();
    opts.backupDir = _backupDirEdit->text();
    opts.backupStyle = _backupStyleCombo->currentIndex();
    opts.maxBackups = _spinMaxBackups->value();

    // Language
    opts.autoDetectLanguage = _chkAutoDetectLanguage->isChecked();

    // Search
    opts.smartHighlighting = _chkSmartHighlighting->isChecked();
    opts.maxHighlightWords = _spinMaxHighlightWords->value();
    opts.matchCaseDefault = _chkMatchCase->isChecked();
    opts.matchWholeWordDefault = _chkMatchWholeWord->isChecked();
    opts.wrapAroundDefault = _chkWrapAround->isChecked();
    opts.findHistoryCount = _spinFindHistoryCount->value();

    // Apply settings to the active editor (live preview)
    if (ScintillaEditor* ed = Application::instance().getActiveEditor()) {
        ed->setTabWidth(opts.defaultTabWidth);
        ed->setIndentWidth(opts.indentWidth);
        ed->setUseTabs(!opts.defaultTabAsSpaces);
        ed->setWrapMode(opts.wordWrap);
        ed->setMarginLineNumbers(0, opts.showLineNumbers);
        ed->setVirtualSpaceOptions(opts.virtualSpace);
        ed->setHomeKeyNavigation(opts.smartHome);
        ed->setAutoIndent(opts.autoIndent);
        ed->setBackspaceUnindents(opts.backspaceUnindent);
        ed->setEolVisibility(opts.showEol);
        ed->setWhitespaceVisibility(opts.showSpaceTab);
        ed->setCaretLineVisibility(opts.highlightCurrentLine);
        // Indent guide via Scintilla direct function (IDC_SETINDENTATIONGUIDES)
        if (opts.showIndentGuide) {
            ed->send(3135 /* SCI_SETINDENTATIONGUIDES*/, 1);
        } else {
            ed->send(3135 /* SCI_SETINDENTATIONGUIDES*/, 0);
        }
    }

    // Apply UI changes
    if (app().mainWindow()) {
        app().mainWindow()->menuBar()->setVisible(opts.showMenuBar);

        // Show/hide toolbar
        auto toolbars = app().mainWindow()->findChildren<QToolBar*>();
        for (auto* tb : toolbars) {
            tb->setVisible(opts.showToolBar);
        }

        // Show/hide status bar
        app().mainWindow()->statusBar()->setVisible(opts.showStatusBar);
    }

    // Load theme if changed
    QString newTheme = _cmbTheme->currentData().toString();
    if (newTheme != _originalSettings.theme) {
        app().loadTheme(opts.themeProfile);
    }

    // Save to config
    app().saveConfig();

    // Update original values for next cancel
    _originalSettings.singleInstance = opts.singleInstance;
    _originalSettings.rememberSession = opts.rememberSession;
    _originalSettings.maxRecentFiles = opts.maxRecentFiles;
    _originalSettings.tabWidth = opts.defaultTabWidth;
    _originalSettings.indentWidth = opts.indentWidth;
    _originalSettings.tabAsSpaces = opts.defaultTabAsSpaces;
    _originalSettings.wordWrap = opts.wordWrap;
    _originalSettings.virtualSpace = opts.virtualSpace;
    _originalSettings.smartHome = opts.smartHome;
    _originalSettings.eolMode = _cmbEolMode->currentData().toInt();
    _originalSettings.defaultEncoding = _cmbDefaultEncoding->currentData().toInt();
    _originalSettings.autoIndent = opts.autoIndent;
    _originalSettings.wrapWithQuotes = opts.wrapWithQuotes;
    _originalSettings.backspaceUnindent = opts.backspaceUnindent;
    _originalSettings.theme = _cmbTheme->currentData().toString();
    _originalSettings.showToolbar = opts.showToolBar;
    _originalSettings.currentLineHighlight = opts.currentLineHighlight;
    _originalSettings.showIndentGuide = opts.showIndentGuide;
    _originalSettings.showTabbar = opts.showTabBar;
    _originalSettings.showStatusbar = opts.showStatusBar;
    _originalSettings.showMenubar = opts.showMenuBar;
}

// ============================================================================
// Category Changed
// ============================================================================
void PreferenceDialog::onCategoryChanged(QTreeWidgetItem* item, int column) {
    (void)column;
    if (!item) return;

    bool ok = false;
    int index = item->data(0, Qt::UserRole).toInt(&ok);
    if (ok) {
        _pageStack->setCurrentIndex(index);
        _categoryTree->setCurrentItem(item);
    }
}

// ============================================================================
// Accept / Reject
// ============================================================================
void PreferenceDialog::accept() {
    // Apply settings before closing
    applySettings();
    QDialog::accept();
}

void PreferenceDialog::reject() {
    // Revert to original values — no need to write back to config since
    // settings were never persisted (only stored in memory during the dialog).
    // The active editor and UI will revert on next buffer activation.
    QDialog::reject();
}

// ============================================================================
// Theme Management
// ============================================================================

void PreferenceDialog::onReloadThemes() {
    // Reload the theme list from standard locations and refresh the theme combo
    QStringList themeFiles;

    // Scan user themes directory: ~/.local/share/notepad--/themes/
    QString userThemesDir = QDir::homePath() + "/.local/share/notepad--/themes";
    QDir userDir(userThemesDir);
    if (userDir.exists()) {
        for (const QString& file : userDir.entryList(QStringList() << "*.xml", QDir::Files)) {
            themeFiles.append(userThemesDir + "/" + file);
        }
    }

    // Scan system themes directory: /usr/share/notepad--/themes/
    QString systemThemesDir = "/usr/share/notepad--/themes";
    QDir systemDir(systemThemesDir);
    if (systemDir.exists()) {
        for (const QString& file : systemDir.entryList(QStringList() << "*.xml", QDir::Files)) {
            QString fullPath = systemThemesDir + "/" + file;
            // Avoid duplicates
            if (!themeFiles.contains(fullPath))
                themeFiles.append(fullPath);
        }
    }

    // Also scan config dir themes if it exists
    QString configDir = QDir::homePath() + "/.config/notepad--/themes";
    QDir configThemeDir(configDir);
    if (configThemeDir.exists()) {
        for (const QString& file : configThemeDir.entryList(QStringList() << "*.xml", QDir::Files)) {
            QString fullPath = configDir + "/" + file;
            if (!themeFiles.contains(fullPath))
                themeFiles.append(fullPath);
        }
    }

    // Store current selection before reload
    QString currentTheme = _cmbTheme->currentData().toString();

    // Clear and repopulate the theme combo
    _cmbTheme->clear();

    // Add built-in themes first
    _cmbTheme->addItem("Default (Light)", "default");
    _cmbTheme->addItem("Dark", "dark");
    _cmbTheme->addItem("Light", "light");
    _cmbTheme->addItem("High Contrast", "high-contrast");

    // Add discovered XML themes
    for (const QString& path : themeFiles) {
        QFileInfo fi(path);
        QString baseName = fi.baseName();
        // Capitalize for display
        QString displayName = baseName;
        displayName[0] = displayName[0].toUpper();
        _cmbTheme->addItem(displayName, baseName);
    }

    // Try to restore previous selection
    int restoredIndex = _cmbTheme->findData(currentTheme);
    if (restoredIndex >= 0) {
        _cmbTheme->setCurrentIndex(restoredIndex);
    }

    qDebug() << "[PreferenceDialog] Reloaded" << themeFiles.size()
             << "theme files. Current selection:" << currentTheme;
}

// ============================================================================
// Apply Settings
// ============================================================================

void PreferenceDialog::onApply() {
    // Delegate to the existing applySettings() which handles:
    // - Writing settings to config file via app().saveConfig()
    // - Updating lexer colors and editor state
    // - Reloading theme if changed
    applySettings();

    // Emit signal for listeners to react
    emit settingsApplied();

    qDebug() << "[PreferenceDialog] Settings applied and config saved";
}

// ============================================================================
// Reset to Defaults
// ============================================================================

void PreferenceDialog::resetToDefaults() {
    // Ask user to confirm before resetting
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Reset to Defaults"),
        tr("Are you sure you want to reset all settings to their default values?\n\n"
           "This will clear your configuration file and cannot be undone."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    // Reset all widgets to their default values
    // General
    _chkSingleInstance->setChecked(true);
    _chkRememberSession->setChecked(true);
    _spinMaxRecentFiles->setValue(50);

    // Editor
    _spinTabWidth->setValue(4);
    _spinIndentWidth->setValue(4);
    _chkTabAsSpaces->setChecked(false);
    _chkWordWrap->setChecked(false);
    _chkVirtualSpace->setChecked(true);
    _chkSmartHome->setChecked(true);
    _chkBackspaceUnindent->setChecked(false);
    _cmbEolMode->setCurrentIndex(0);  // Unix (LF)
    _cmbDefaultEncoding->setCurrentIndex(0);  // UTF-8
    _chkAutoIndent->setChecked(true);
    _chkWrapWithQuotes->setChecked(false);
    _chkShowEol->setChecked(false);
    _chkShowSpaceTab->setChecked(false);

    // Appearance
    _cmbTheme->setCurrentIndex(0);  // Default (Light)
    _chkShowMenubar->setChecked(true);
    _chkShowToolbar->setChecked(true);
    _chkShowTabbar->setChecked(true);
    _chkShowStatusbar->setChecked(true);
    _chkCurrentLineHighlight->setChecked(true);
    _chkShowIndentGuide->setChecked(false);

    // File Associations — clear custom
    _extListWidget->clear();

    // Shortcut Mapper
    _chkConflictWarning->setChecked(true);

    // Margins
    _chkShowLineNumbers->setChecked(true);
    _spinLineNumberWidth->setValue(4);
    _chkShowSymbols->setChecked(false);
    _chkShowFolder->setChecked(true);
    _spinSymbolMarginWidth->setValue(1);
    _chkHighlightCurrentLine->setChecked(true);
    _chkShowEdgeLine->setChecked(false);
    _spinEdgeColumn->setValue(80);

    // Backup / Auto-Save
    _chkAutoSave->setChecked(false);
    _spinAutoSaveInterval->setValue(5);
    _chkAutoSaveCurrentOnly->setChecked(false);
    _chkAutoSaveInBackground->setChecked(true);
    _backupDirEdit->clear();
    _backupStyleCombo->setCurrentIndex(0);
    _spinMaxBackups->setValue(10);

    // Language
    _cmbLanguage->setCurrentIndex(0);
    _chkAutoDetectLanguage->setChecked(true);

    // Search
    _chkSmartHighlighting->setChecked(true);
    _spinMaxHighlightWords->setValue(1000);
    _chkMatchCase->setChecked(false);
    _chkMatchWholeWord->setChecked(false);
    _chkWrapAround->setChecked(true);
    _spinFindHistoryCount->setValue(10);

    // Delete the config file to clear any persisted settings
    QString configDir = QDir::homePath() + "/.config/notepad--";
    QString configFile = configDir + "/config.xml";
    if (QFile::exists(configFile)) {
        QFile::remove(configFile);
        qDebug() << "[PreferenceDialog] Removed config file:" << configFile;
    }

    // Also try app-level config path
    app().saveConfig();

    qDebug() << "[PreferenceDialog] Settings reset to defaults";
}

// ============================================================================
// Import from Notepad++
// ============================================================================

void PreferenceDialog::onImportFromNpp() {
    // Open file dialog to select a Notepad++ config file
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import Notepad++ Config"),
        QString(),
        tr("Notepad++ Files (*.xml *.json);;All Files (*)"));

    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Import Error"),
            tr("Could not open file:\n%1").arg(filePath));
        return;
    }

    QXmlStreamReader reader(&file);
    bool importedAny = false;

    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString name = reader.name().toString();

            // Parse Notepad++ config.xml settings
            if (name == "Global") {
                while (!(reader.tokenType() == QXmlStreamReader::EndElement &&
                         reader.name().toString() == QStringLiteral("Global")) && !reader.atEnd()) {
                    reader.readNext();
                    if (reader.tokenType() == QXmlStreamReader::StartElement) {
                        QString setting = reader.name().toString();
                        QString text = reader.readElementText();

                        // Map N++ settings to npp-qt equivalents
                        if (setting == "TabSize")
                            _spinTabWidth->setValue(text.toInt());
                        else if (setting == "IndentSize")
                            _spinIndentWidth->setValue(text.toInt());
                        else if (setting == "TabSize" && !_chkTabAsSpaces->isChecked())
                            _chkTabAsSpaces->setChecked(true);
                        else if (setting == "WordWrap")
                            _chkWordWrap->setChecked(text == "yes");
                        else if (setting == "ScintillaViewsSettings") {
                            // Scintilla view settings
                        }
                    }
                }
                importedAny = true;
            }

            // Parse stylers.xml theme colors
            else if (name == "LexerType") {
                QString lexName = reader.attributes().value("name").toString();
                // Theme colors would be applied via ThemeManager
                Q_UNUSED(lexName);
            }

            // Parse shortcuts.xml
            else if (name == "Shortcut") {
                QString cmdName = reader.attributes().value("name").toString();
                QString ctrl = reader.attributes().value("ctrl").toString();
                QString alt = reader.attributes().value("alt").toString();
                QString shift = reader.attributes().value("shift").toString();
                QString key = reader.attributes().value("key").toString();

                // Shortcuts would be imported via ShortcutManager
                Q_UNUSED(cmdName);
                Q_UNUSED(ctrl);
                Q_UNUSED(alt);
                Q_UNUSED(shift);
                Q_UNUSED(key);
            }
        }
    }

    file.close();

    if (reader.hasError()) {
        QMessageBox::warning(this, tr("Parse Error"),
            tr("Error parsing file:\n%1").arg(reader.errorString()));
        return;
    }

    QString msg = importedAny
        ? tr("Settings imported successfully from:\n%1").arg(filePath)
        : tr("No recognized settings found in:\n%1").arg(filePath);
    QMessageBox::information(this, tr("Import Complete"), msg);

    qDebug() << "[PreferenceDialog] Imported from Notepad++ config:" << filePath;
}

// ============================================================================
// Export to Notepad++
// ============================================================================

void PreferenceDialog::onExportToNpp() {
    // Open file dialog to choose export destination
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Settings to Notepad++ Format"),
        QDir::homePath() + "/npp-qt-export.xml",
        tr("Notepad++ Files (*.xml);;All Files (*)"));

    if (filePath.isEmpty())
        return;

    // Ensure .xml extension
    if (!filePath.endsWith(".xml", Qt::CaseInsensitive))
        filePath += ".xml";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Error"),
            tr("Could not create file:\n%1").arg(filePath));
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("NotepadPlus");

    // Export General settings
    writer.writeStartElement("Global");
    writer.writeTextElement("TabSize", QString::number(_spinTabWidth->value()));
    writer.writeTextElement("IndentSize", QString::number(_spinIndentWidth->value()));
    writer.writeTextElement("TabSizeUsed", _chkTabAsSpaces->isChecked() ? "yes" : "no");
    writer.writeTextElement("WordWrap", _chkWordWrap->isChecked() ? "yes" : "no");
    writer.writeTextElement("SingleInstance",
        _chkSingleInstance->isChecked() ? "yes" : "no");
    writer.writeTextElement("RememberSession",
        _chkRememberSession->isChecked() ? "yes" : "no");
    writer.writeTextElement("MaxRecentFiles",
        QString::number(_spinMaxRecentFiles->value()));
    writer.writeEndElement();  // Global

    // Export EOL mode
    writer.writeStartElement("EOL");
    int eolIdx = _cmbEolMode->currentIndex();
    QString eolStr = (eolIdx == 0) ? "LF" : (eolIdx == 1) ? "CRLF" : "CR";
    writer.writeAttribute("mode", eolStr);
    writer.writeEndElement();

    // Export encoding
    writer.writeStartElement("Encoding");
    int encIdx = _cmbDefaultEncoding->currentIndex();
    QString encStr = (encIdx == 0) ? "UTF-8" : (encIdx == 1) ? "UTF-8-BOM"
                    : (encIdx == 2) ? "UTF-16-LE" : (encIdx == 3) ? "UTF-16-BE" : "ANSI";
    writer.writeCharacters(encStr);
    writer.writeEndElement();

    // Export UI visibility settings
    writer.writeStartElement("UI");
    writer.writeTextElement("ShowMenuBar", _chkShowMenubar->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowToolBar", _chkShowToolbar->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowTabBar", _chkShowTabbar->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowStatusBar", _chkShowStatusbar->isChecked() ? "yes" : "no");
    writer.writeEndElement();  // UI

    // Export Editor settings
    writer.writeStartElement("Editor");
    writer.writeTextElement("AutoIndent", _chkAutoIndent->isChecked() ? "yes" : "no");
    writer.writeTextElement("VirtualSpace", _chkVirtualSpace->isChecked() ? "yes" : "no");
    writer.writeTextElement("SmartHome", _chkSmartHome->isChecked() ? "yes" : "no");
    writer.writeTextElement("WrapWithQuotes", _chkWrapWithQuotes->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowEOL", _chkShowEol->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowSpaces", _chkShowSpaceTab->isChecked() ? "yes" : "no");
    writer.writeEndElement();  // Editor

    // Export theme
    writer.writeStartElement("Theme");
    writer.writeAttribute("name", _cmbTheme->currentData().toString());
    writer.writeEndElement();

    // Export margin settings
    writer.writeStartElement("Margins");
    writer.writeTextElement("ShowLineNumbers", _chkShowLineNumbers->isChecked() ? "yes" : "no");
    writer.writeTextElement("LineNumberWidth", QString::number(_spinLineNumberWidth->value()));
    writer.writeTextElement("ShowSymbolMargin", _chkShowSymbols->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowFolderMargin", _chkShowFolder->isChecked() ? "yes" : "no");
    writer.writeTextElement("HighlightCurrentLine",
        _chkHighlightCurrentLine->isChecked() ? "yes" : "no");
    writer.writeTextElement("ShowEdgeLine", _chkShowEdgeLine->isChecked() ? "yes" : "no");
    writer.writeTextElement("EdgeColumn", QString::number(_spinEdgeColumn->value()));
    writer.writeEndElement();  // Margins

    // Export Backup/Auto-Save settings
    writer.writeStartElement("Backup");
    writer.writeTextElement("AutoSave", _chkAutoSave->isChecked() ? "yes" : "no");
    writer.writeTextElement("AutoSaveInterval",
        QString::number(_spinAutoSaveInterval->value()));
    writer.writeTextElement("AutoSaveCurrentOnly",
        _chkAutoSaveCurrentOnly->isChecked() ? "yes" : "no");
    writer.writeTextElement("AutoSaveInBackground",
        _chkAutoSaveInBackground->isChecked() ? "yes" : "no");
    writer.writeTextElement("BackupDir", _backupDirEdit->text());
    writer.writeTextElement("BackupStyle",
        QString::number(_backupStyleCombo->currentIndex()));
    writer.writeTextElement("MaxBackups", QString::number(_spinMaxBackups->value()));
    writer.writeEndElement();  // Backup

    // Export File Associations
    writer.writeStartElement("FileAssociations");
    for (int i = 0; i < _extListWidget->count(); ++i) {
        writer.writeEmptyElement("Extension");
        writer.writeAttribute("name", _extListWidget->item(i)->text());
    }
    writer.writeEndElement();  // FileAssociations

    writer.writeEndElement();  // NotepadPlus
    writer.writeEndDocument();
    file.close();

    QMessageBox::information(this, tr("Export Complete"),
        tr("Settings exported successfully to:\n%1").arg(filePath));

    qDebug() << "[PreferenceDialog] Exported to Notepad++ format:" << filePath;
}

// ============================================================================
// Open Themes Folder
// ============================================================================

void PreferenceDialog::onOpenThemesFolder() {
    // Determine the themes directory — prefer user-local over system
    QStringList searchPaths;
    searchPaths << QDir::homePath() + "/.config/notepad--/themes";
    searchPaths << QDir::homePath() + "/.local/share/notepad--/themes";
    searchPaths << "/usr/share/notepad--/themes";

    QString themesDir;
    for (const QString& path : searchPaths) {
        if (QDir(path).exists()) {
            themesDir = path;
            break;
        }
    }

    // If none exist, use the user config dir (it will be created on first theme save)
    if (themesDir.isEmpty()) {
        themesDir = QDir::homePath() + "/.config/notepad--/themes";
        // Ensure directory exists
        if (!QDir().mkpath(themesDir)) {
            QMessageBox::warning(this, tr("Error"),
                tr("Could not create themes directory:\n%1").arg(themesDir));
            return;
        }
    }

    // Open the folder in the system file manager
    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(themesDir));
    if (!opened) {
        QMessageBox::warning(this, tr("Error"),
            tr("Could not open folder:\n%1").arg(themesDir));
    }

    qDebug() << "[PreferenceDialog] Opened themes folder:" << themesDir;
}

