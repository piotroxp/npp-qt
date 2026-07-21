// ShortcutMapper.cpp - Qt6 port of ShortcutMapper dialog
#include "ShortcutMapper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>

// Qt key code mapping from Qt::Key_* to the keys used by Notepad++
static QString qtKeyToString(int key) {
    if (key >= Qt::Key_F1 && key <= Qt::Key_F12) {
        return QString("F%1").arg(key - Qt::Key_F1 + 1);
    }
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return QChar('A' + (key - Qt::Key_A));
    }
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return QChar('0' + (key - Qt::Key_0));
    }
    switch (key) {
        case Qt::Key_Space:    return "Space";
        case Qt::Key_Tab:      return "Tab";
        case Qt::Key_Return:   return "Enter";
        case Qt::Key_Escape:   return "Esc";
        case Qt::Key_Backspace: return "Backspace";
        case Qt::Key_Delete:   return "Delete";
        case Qt::Key_Up:       return "Up";
        case Qt::Key_Down:     return "Down";
        case Qt::Key_Left:     return "Left";
        case Qt::Key_Right:    return "Right";
        case Qt::Key_PageUp:   return "Page Up";
        case Qt::Key_PageDown: return "Page Down";
        case Qt::Key_Home:     return "Home";
        case Qt::Key_End:      return "End";
        case Qt::Key_Insert:   return "Insert";
        case Qt::Key_Plus:     return "Ctrl+Oem_Plus";
        case Qt::Key_Minus:    return "Ctrl+Oem_Minus";
        default: {
            if (key >= Qt::Key_BraceLeft && key <= Qt::Key_AsciiTilde) {
                return QChar(key);
            }
            return QString("0x%1").arg(key, 0, 16);
        }
    }
}

// ─────────────────────────────────────────────────────────────────
// ShortcutMapper
// ─────────────────────────────────────────────────────────────────
ShortcutMapper::ShortcutMapper(QWidget* parent)
    : QDialog(parent) {
    setupUi();
}

void ShortcutMapper::setupUi() {
    setWindowTitle(tr("Shortcut Mapper"));
    resize(700, 500);

    auto* mainLayout = new QVBoxLayout(this);

    // Tab bar
    _tabWidget = new QTabWidget(this);
    mainLayout->addWidget(_tabWidget);

    // BabyGrid area (placeholder — ShortcutMapper uses its own grid)
    _babyGrid = new BabyGridWrapper(this);
    mainLayout->addWidget(_babyGrid);

    // Info label + filter
    auto* filterLayout = new QHBoxLayout();
    _infoLabel = new QLabel(tr("Double-click to modify shortcut"), this);
    filterLayout->addWidget(_infoLabel);
    filterLayout->addStretch();

    _filterEdit = new QLineEdit(this);
    _filterEdit->setPlaceholderText(tr("Filter shortcuts..."));
    filterLayout->addWidget(_filterEdit);

    mainLayout->addLayout(filterLayout);

    // Right-click context menu
    _rightClickMenu = new QMenu(this);
    auto* modifyAction = _rightClickMenu->addAction(tr("Modify"));
    auto* clearAction = _rightClickMenu->addAction(tr("Clear"));
    connect(modifyAction, &QAction::triggered, this, [this]() {
        onCellDoubleClicked(_babyGrid->selectedRow(), ShortcutMapper::COL_SHORTCUT);
    });
    connect(clearAction, &QAction::triggered, this, [this]() {
        int row = _babyGrid->selectedRow();
        if (row >= 0) {
            _babyGrid->setCellText(row, ShortcutMapper::COL_SHORTCUT, QString());
        }
    });

    // Buttons
    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(btnBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(btnBox);

    // Connections
    connect(_tabWidget, &QTabWidget::currentChanged, this, &ShortcutMapper::onTabChanged);
    connect(_filterEdit, &QLineEdit::textChanged, this, &ShortcutMapper::onFilterChanged);

    // Setup BabyGrid columns
    _babyGrid->setGridDimensions(1, 4);
    _babyGrid->setHeaderHeight(24);
    _babyGrid->setColWidth(ShortcutMapper::COL_CATEGORY, 150);
    _babyGrid->setColWidth(ShortcutMapper::COL_NAME, 250);
    _babyGrid->setColWidth(ShortcutMapper::COL_SHORTCUT, 120);
    _babyGrid->setColWidth(ShortcutMapper::COL_ASSIGNED, 150);
    _babyGrid->setColAutoWidth(false);

    // Initial fill
    populateMainMenuTab();
}

void ShortcutMapper::init(QWidget* parent, GridState initState) {
    Q_UNUSED(parent);
    _currentState = initState;
}

void ShortcutMapper::doDialog(bool isRTL) {
    Q_UNUSED(isRTL);
    show();
    raise();
    activateWindow();
}

void ShortcutMapper::setupTabs() {
    _tabWidget->clear();
    _tabWidget->addTab(nullptr, tabName(0)); // Main menu
    _tabWidget->addTab(nullptr, tabName(1)); // Macros
    _tabWidget->addTab(nullptr, tabName(2)); // Run commands
    _tabWidget->addTab(nullptr, tabName(3)); // Plugins
    _tabWidget->addTab(nullptr, tabName(4)); // Scintilla
    _tabWidget->setCurrentIndex(static_cast<int>(_currentState));
}

void ShortcutMapper::onTabChanged(int index) {
    _currentState = static_cast<GridState>(index);
    _babyGrid->clearGrid();
    _babyGrid->setGridDimensions(1, 4);
    switch (_currentState) {
        case GridState::STATE_MENU:      populateMainMenuTab(); break;
        case GridState::STATE_MACRO:     populateMacrosTab();    break;
        case GridState::STATE_USER:      populateRunCommandsTab(); break;
        case GridState::STATE_PLUGIN:    populatePluginCommandsTab(); break;
        case GridState::STATE_SCINTILLA: populateScintillaTab();  break;
    }
}

void ShortcutMapper::onFilterChanged(const QString& text) {
    _filterText = text;
    // Re-populate the current tab with filter applied
    onTabChanged(_tabWidget->currentIndex());
}

void ShortcutMapper::onCellDoubleClicked(int row, int col) {
    Q_UNUSED(col);
    if (row < 0) return;

    // Ask user for the new key combination
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Enter Shortcut"));
    dialog.setLabelText(tr("Press a key combination (or Cancel to clear):"));
    dialog.setTextValue(_babyGrid->cellText(row, ShortcutMapper::COL_SHORTCUT));

    // For now, use a simple dialog
    bool ok;
    QString newShortcut = QInputDialog::getText(this, tr("Modify Shortcut"),
        tr("New shortcut:"), QLineEdit::Normal,
        _babyGrid->cellText(row, ShortcutMapper::COL_SHORTCUT), &ok);
    if (ok) {
        _babyGrid->setCellText(row, ShortcutMapper::COL_SHORTCUT, newShortcut);
    }
}

void ShortcutMapper::onClearFilter() {
    _filterEdit->clear();
}

void ShortcutMapper::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
}

QString ShortcutMapper::tabName(size_t i) const {
    switch (i) {
        case 1: return tr("Macros");
        case 2: return tr("Run Commands");
        case 3: return tr("Plugin Commands");
        case 4: return tr("Scintilla Commands");
        default: return tr("Main Menu");
    }
}

void ShortcutMapper::populateMainMenuTab() {
    // In a real implementation, populate from ShortcutManager
    // For now, show sample entries
    int row = 1;
    auto addRow = [&](const QString& cat, const QString& name, const QString& shortcut) {
        if (!_filterText.isEmpty() &&
            !name.contains(_filterText, Qt::CaseInsensitive) &&
            !cat.contains(_filterText, Qt::CaseInsensitive)) {
            return;
        }
        _babyGrid->setGridDimensions(row + 1, 4);
        _babyGrid->setCellText(row, COL_CATEGORY, cat);
        _babyGrid->setCellText(row, COL_NAME, name);
        _babyGrid->setCellText(row, COL_SHORTCUT, shortcut);
        _babyGrid->setCellText(row, COL_ASSIGNED, QString());
        ++row;
    };

    // Sample entries — replace with actual ShortcutManager data
    addRow(tr("File"), tr("New"), QString("Ctrl+N"));
    addRow(tr("File"), tr("Open"), QString("Ctrl+O"));
    addRow(tr("File"), tr("Save"), QString("Ctrl+S"));
    addRow(tr("File"), tr("Save As"), QString("Ctrl+Shift+S"));
    addRow(tr("Edit"), tr("Cut"), QString("Ctrl+X"));
    addRow(tr("Edit"), tr("Copy"), QString("Ctrl+C"));
    addRow(tr("Edit"), tr("Paste"), QString("Ctrl+V"));
    addRow(tr("Edit"), tr("Find"), QString("Ctrl+F"));
    addRow(tr("Edit"), tr("Replace"), QString("Ctrl+H"));
    addRow(tr("Search"), tr("Go to Line"), QString("Ctrl+G"));
    addRow(tr("View"), tr("Zoom In"), QString("Ctrl+Oem_Plus"));
    addRow(tr("View"), tr("Zoom Out"), QString("Ctrl+Oem_Minus"));
    addRow(tr("View"), tr("Full Screen"), QString("F11"));
    addRow(tr("Macro"), tr("Record"), QString("Alt+Shift+R"));
    addRow(tr("Run"), tr("Launch in Browser"), QString("Ctrl+Shift+B"));
}

void ShortcutMapper::populateMacrosTab() {
    int row = 1;
    auto addRow = [&](const QString& name, const QString& shortcut) {
        if (!_filterText.isEmpty() &&
            !name.contains(_filterText, Qt::CaseInsensitive)) {
            return;
        }
        _babyGrid->setGridDimensions(row + 1, 4);
        _babyGrid->setCellText(row, COL_CATEGORY, tr("Macro"));
        _babyGrid->setCellText(row, COL_NAME, name);
        _babyGrid->setCellText(row, COL_SHORTCUT, shortcut);
        ++row;
    };
    // Populate from MacroManager — sample entries
    addRow(tr("Sample Macro 1"), tr("Ctrl+Shift+1"));
    addRow(tr("Sample Macro 2"), tr("Ctrl+Shift+2"));
}

void ShortcutMapper::populateRunCommandsTab() {
    int row = 1;
    auto addRow = [&](const QString& name, const QString& shortcut) {
        if (!_filterText.isEmpty() &&
            !name.contains(_filterText, Qt::CaseInsensitive)) {
            return;
        }
        _babyGrid->setGridDimensions(row + 1, 4);
        _babyGrid->setCellText(row, COL_CATEGORY, tr("Run Command"));
        _babyGrid->setCellText(row, COL_NAME, name);
        _babyGrid->setCellText(row, COL_SHORTCUT, shortcut);
        ++row;
    };
    addRow(tr("Launch in Chrome"), tr("Ctrl+Alt+Shift+B"));
    addRow(tr("Open in Notepad++"), tr(""));
}

void ShortcutMapper::populatePluginCommandsTab() {
    int row = 1;
    auto addRow = [&](const QString& plugin, const QString& name, const QString& shortcut) {
        if (!_filterText.isEmpty() &&
            !name.contains(_filterText, Qt::CaseInsensitive) &&
            !plugin.contains(_filterText, Qt::CaseInsensitive)) {
            return;
        }
        _babyGrid->setGridDimensions(row + 1, 4);
        _babyGrid->setCellText(row, COL_CATEGORY, plugin);
        _babyGrid->setCellText(row, COL_NAME, name);
        _babyGrid->setCellText(row, COL_SHORTCUT, shortcut);
        ++row;
    };
    addRow(tr("Plugins"), tr("No plugins loaded"), tr(""));
}

void ShortcutMapper::populateScintillaTab() {
    int row = 1;
    auto addRow = [&](const QString& name, const QString& shortcut) {
        if (!_filterText.isEmpty() &&
            !name.contains(_filterText, Qt::CaseInsensitive)) {
            return;
        }
        _babyGrid->setGridDimensions(row + 1, 4);
        _babyGrid->setCellText(row, COL_CATEGORY, tr("Scintilla"));
        _babyGrid->setCellText(row, COL_NAME, name);
        _babyGrid->setCellText(row, COL_SHORTCUT, shortcut);
        ++row;
    };
    addRow(tr("Word Part Left"), tr("Ctrl+Left"));
    addRow(tr("Word Part Right"), tr("Ctrl+Right"));
    addRow(tr("Line Cut"), tr("Ctrl+L"));
    addRow(tr("Line Duplicate"), tr("Ctrl+D"));
}

bool ShortcutMapper::isFilterValid(const ShortcutItem& sc) const {
    return _filterText.isEmpty() ||
        sc._name.contains(_filterText, Qt::CaseInsensitive);
}

bool ShortcutMapper::isFilterValid(const PluginCmdShortcutItem& sc) const {
    return _filterText.isEmpty() ||
        sc._name.contains(_filterText, Qt::CaseInsensitive) ||
        sc._pluginName.contains(_filterText, Qt::CaseInsensitive);
}

bool ShortcutMapper::isFilterValid(const ScintillaKeyMapItem& sc) const {
    return _filterText.isEmpty() ||
        sc._name.contains(_filterText, Qt::CaseInsensitive);
}

QString ShortcutMapper::shortcutToString(const KeyCombo& kc) const {
    QStringList parts;
    if (kc._isCtrl) parts << "Ctrl";
    if (kc._isAlt)  parts << "Alt";
    if (kc._isShift) parts << "Shift";
    if (kc._key != 0) parts << qtKeyToString(static_cast<int>(kc._key));
    return parts.join('+');
}
