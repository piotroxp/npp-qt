// PreferenceDialog.cpp - Preferences/Settings dialog implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "PreferenceDialog.h"
#include "core/Application.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>

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

    // Right side: stacked widget with pages
    _pageStack = new QStackedWidget(this);

    _pageStack->addWidget(createGeneralPage());
    _pageStack->addWidget(createEditorPage());
    _pageStack->addWidget(createAppearancePage());
    _pageStack->addWidget(createFileAssociationPage());
    _pageStack->addWidget(createShortcutPage());
    _pageStack->addWidget(createMarginsPage());
    _pageStack->addWidget(createBackupPage());

    // Button box
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    _buttonBox->button(QDialogButtonBox::Apply)->setText("Apply");

    // Connect signals
    connect(_categoryTree, &QTreeWidget::itemClicked, this, &PreferenceDialog::onCategoryChanged);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &PreferenceDialog::accept);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &PreferenceDialog::reject);
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

    // Add stretch to push everything to top
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

    _chkTabAsSpaces = new QCheckBox("Replace by spaces", tabGroup);
    tabLayout->addWidget(_chkTabAsSpaces, 1, 0, 1, 2);

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

    _chkWrapWithQuotes = new QCheckBox("Wrap with quotes (selection)", editGroup);
    editLayout->addWidget(_chkWrapWithQuotes);

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
    layout->addStretch();

    return page;
}

// ============================================================================
// File Associations Page
// ============================================================================
QWidget* PreferenceDialog::createFileAssociationPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* placeholder = new QLabel(
        "File associations allow you to open specific file types "
        "with Notepad-- by default.\n\n"
        "This feature is not yet implemented.\n"
        "You can configure file associations through your operating system instead.",
        page);
    placeholder->setWordWrap(true);
    placeholder->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    layout->addWidget(placeholder);
    layout->addStretch();

    return page;
}

// ============================================================================
// Shortcut Mapper Page
// ============================================================================
QWidget* PreferenceDialog::createShortcutPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* placeholder = new QLabel(
        "Shortcut Mapper allows you to view and customize keyboard shortcuts "
        "for menu commands and Scintilla commands.\n\n"
        "This feature is not yet implemented.",
        page);
    placeholder->setWordWrap(true);

    QPushButton* openMapper = new QPushButton("Open Shortcut Mapper", page);
    connect(openMapper, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "Shortcut Mapper",
            "Shortcut Mapper will be implemented in a future version.");
    });

    layout->addWidget(placeholder);
    layout->addWidget(openMapper);
    layout->addStretch();

    return page;
}

// ============================================================================
// Margins & Borders Page
// ============================================================================
QWidget* PreferenceDialog::createMarginsPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* placeholder = new QLabel(
        "Margins and Borders settings allow you to customize:\n\n"
        "• Line number margin width\n"
        "• Symbol margin visibility\n"
        "• Fold margin settings\n"
        "• Current line highlighting\n\n"
        "This feature is not yet implemented.",
        page);
    placeholder->setWordWrap(true);

    layout->addWidget(placeholder);
    layout->addStretch();

    return page;
}

// ============================================================================
// Backup/Auto-Save Page
// ============================================================================
QWidget* PreferenceDialog::createBackupPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* placeholder = new QLabel(
        "Backup and Auto-Save settings allow you to:\n\n"
        "• Enable periodic auto-save\n"
        "• Configure backup directory\n"
        "• Set backup file naming scheme\n"
        "• Control snapshot retention\n\n"
        "This feature is not yet implemented.",
        page);
    placeholder->setWordWrap(true);

    layout->addWidget(placeholder);
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
    _chkTabAsSpaces->setChecked(opts.defaultTabAsSpaces);

    int eolIdx = _cmbEolMode->findData(QVariant(static_cast<int>(opts.defaultEolType)));
    if (eolIdx >= 0) _cmbEolMode->setCurrentIndex(eolIdx);

    int encIdx = _cmbDefaultEncoding->findData(QVariant(static_cast<int>(opts.defaultEncoding)));
    if (encIdx >= 0) _cmbDefaultEncoding->setCurrentIndex(encIdx);

    _chkAutoIndent->setChecked(true);  // Default to enabled
    _chkWrapWithQuotes->setChecked(opts.wrapWithQuotes);

    // Appearance
    int themeIdx = _cmbTheme->findData(QString::fromUtf8(opts.themeProfile.c_str()));
    if (themeIdx >= 0) _cmbTheme->setCurrentIndex(themeIdx);

    _chkShowToolbar->setChecked(opts.showToolBar);
    _chkShowTabbar->setChecked(opts.showTabBar);
    _chkShowStatusbar->setChecked(opts.showStatusBar);
    _chkShowMenubar->setChecked(opts.showMenuBar);

    // Store original values
    _originalSettings.singleInstance = opts.singleInstance;
    _originalSettings.rememberSession = opts.rememberSession;
    _originalSettings.maxRecentFiles = opts.maxRecentFiles;
    _originalSettings.tabWidth = opts.defaultTabWidth;
    _originalSettings.tabAsSpaces = opts.defaultTabAsSpaces;
    _originalSettings.eolMode = _cmbEolMode->currentData().toInt();
    _originalSettings.defaultEncoding = _cmbDefaultEncoding->currentData().toInt();
    _originalSettings.autoIndent = true;
    _originalSettings.wrapWithQuotes = opts.wrapWithQuotes;
    _originalSettings.theme = _cmbTheme->currentData().toString();
    _originalSettings.showToolbar = opts.showToolBar;
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
    opts.defaultTabAsSpaces = _chkTabAsSpaces->isChecked();

    EolType eolType = static_cast<EolType>(_cmbEolMode->currentData().toInt());
    opts.defaultEolType = eolType;

    EncodingType encType = static_cast<EncodingType>(_cmbDefaultEncoding->currentData().toInt());
    opts.defaultEncoding = encType;

    opts.wrapWithQuotes = _chkWrapWithQuotes->isChecked();

    // Appearance
    opts.themeProfile = _cmbTheme->currentData().toString().toUtf8().constData();
    opts.showToolBar = _chkShowToolbar->isChecked();
    opts.showTabBar = _chkShowTabbar->isChecked();
    opts.showStatusBar = _chkShowStatusbar->isChecked();
    opts.showMenuBar = _chkShowMenubar->isChecked();

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
    if (opts.themeProfile != _originalSettings.theme.toUtf8().constData()) {
        app().loadTheme(opts.themeProfile);
    }

    // Save to config
    app().saveConfig();

    // Update original values for next cancel
    _originalSettings.singleInstance = opts.singleInstance;
    _originalSettings.rememberSession = opts.rememberSession;
    _originalSettings.maxRecentFiles = opts.maxRecentFiles;
    _originalSettings.tabWidth = opts.defaultTabWidth;
    _originalSettings.tabAsSpaces = opts.defaultTabAsSpaces;
    _originalSettings.eolMode = _cmbEolMode->currentData().toInt();
    _originalSettings.defaultEncoding = _cmbDefaultEncoding->currentData().toInt();
    _originalSettings.wrapWithQuotes = opts.wrapWithQuotes;
    _originalSettings.theme = _cmbTheme->currentData().toString();
    _originalSettings.showToolbar = opts.showToolBar;
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
    // Revert to original values
    AppOptions& opts = app().options();

    opts.singleInstance = _originalSettings.singleInstance;
    opts.rememberSession = _originalSettings.rememberSession;
    opts.maxRecentFiles = _originalSettings.maxRecentFiles;
    opts.defaultTabWidth = _originalSettings.tabWidth;
    opts.defaultTabAsSpaces = _originalSettings.tabAsSpaces;
    opts.defaultEolType = static_cast<EolType>(_originalSettings.eolMode);
    opts.defaultEncoding = static_cast<EncodingType>(_originalSettings.defaultEncoding);
    opts.wrapWithQuotes = _originalSettings.wrapWithQuotes;
    opts.themeProfile = _originalSettings.theme.toUtf8().constData();
    opts.showToolBar = _originalSettings.showToolbar;
    opts.showTabBar = _originalSettings.showTabbar;
    opts.showStatusBar = _originalSettings.showStatusbar;
    opts.showMenuBar = _originalSettings.showMenubar;

    QDialog::reject();
}
