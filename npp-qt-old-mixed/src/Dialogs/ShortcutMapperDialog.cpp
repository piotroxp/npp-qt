// ShortcutMapperDialog.cpp — npp-qt Qt6 Shortcut Mapper dialog implementation

#include "ShortcutMapperDialog.h"
#include "menuCmdID.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QSortFilterProxyModel>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>

// =============================================================================
// Command name table — maps menu command IDs to display names
// =============================================================================

struct CommandDef {
    int id;
    const char* name;
    const char* category;
};

static const CommandDef kCommands[] = {
    {IDM_FILE_NEW, "New", "File"},
    {IDM_FILE_OPEN, "Open...", "File"},
    {IDM_FILE_CLOSE, "Close", "File"},
    {IDM_FILE_CLOSEALL, "Close All", "File"},
    {IDM_FILE_CLOSEALL_BUT_CURRENT, "Close All Except Current", "File"},
    {IDM_FILE_SAVE, "Save", "File"},
    {IDM_FILE_SAVEALL, "Save All", "File"},
    {IDM_FILE_SAVEAS, "Save As...", "File"},
    {IDM_FILE_SAVECOPYAS, "Save Copy As...", "File"},
    {IDM_FILE_DELETE, "Delete", "File"},
    {IDM_FILE_RENAME, "Rename", "File"},
    {IDM_FILE_RELOAD, "Reload", "File"},
    {IDM_FILE_PRINT, "Print...", "File"},
    {IDM_FILE_LOADSESSION, "Load Session...", "File"},
    {IDM_FILE_SAVESESSION, "Save Session...", "File"},
    {IDM_FILE_OPEN_FOLDER, "Open Containing Folder", "File"},
    {IDM_FILE_RESTORELASTCLOSEDFILE, "Restore Last Closed File", "File"},
    {IDM_FILE_EXIT, "Exit", "File"},
    {IDM_EDIT_CUT, "Cut", "Edit"},
    {IDM_EDIT_COPY, "Copy", "Edit"},
    {IDM_EDIT_UNDO, "Undo", "Edit"},
    {IDM_EDIT_REDO, "Redo", "Edit"},
    {IDM_EDIT_PASTE, "Paste", "Edit"},
    {IDM_EDIT_DELETE, "Delete", "Edit"},
    {IDM_EDIT_SELECTALL, "Select All", "Edit"},
    {IDM_EDIT_INS_TAB, "Insert Tab Character", "Edit"},
    {IDM_EDIT_RMV_TAB, "Remove Tab Characters", "Edit"},
    {IDM_EDIT_DUP_LINE, "Duplicate Current Line", "Edit"},
    {IDM_EDIT_TRANSPOSE_LINE, "Transpose Current Line", "Edit"},
    {IDM_EDIT_SPLIT_LINES, "Split Lines", "Edit"},
    {IDM_EDIT_JOIN_LINES, "Join Lines", "Edit"},
    {IDM_EDIT_LINE_UP, "Move Line Up", "Edit"},
    {IDM_EDIT_LINE_DOWN, "Move Line Down", "Edit"},
    {IDM_EDIT_UPPERCASE, "Convert to Uppercase", "Edit"},
    {IDM_EDIT_LOWERCASE, "Convert to Lowercase", "Edit"},
    {IDM_MACRO_STARTRECORDINGMACRO, "Start Recording Macro", "Macro"},
    {IDM_MACRO_STOPRECORDINGMACRO, "Stop Recording Macro", "Macro"},
    {IDM_MACRO_PLAYBACKRECORDEDMACRO, "Play Recording", "Macro"},
    {IDM_MACRO_SAVECURRENTMACRO, "Save Current Macro...", "Macro"},
    {IDM_MACRO_RUNMULTIMACRODLG, "Run Multiple Macros...", "Macro"},
    {IDM_EDIT_BLOCK_COMMENT, "Block Comment", "Edit"},
    {IDM_EDIT_STREAM_COMMENT, "Stream Comment", "Edit"},
    {IDM_EDIT_TRIMTRAILING, "Trim Trailing Space", "Edit"},
    {IDM_EDIT_RTL, "Set Direction RTL", "Edit"},
    {IDM_EDIT_LTR, "Set Direction LTR", "Edit"},
    {IDM_EDIT_TOGGLEREADONLY, "Toggle Read-Only", "Edit"},
    {IDM_EDIT_COLUMNMODE, "Column Mode", "Edit"},
    {IDM_EDIT_TRIMLINEHEAD, "Trim Leading Space", "Edit"},
    {IDM_EDIT_TRIM_BOTH, "Trim Both", "Edit"},
    {IDM_EDIT_EOL2WS, "Convert EOL to Spaces", "Edit"},
    {IDM_EDIT_TAB2SW, "Convert Tabs to Spaces", "Edit"},
    {IDM_EDIT_SORTLINES_LEXICOGRAPHIC_ASCENDING, "Sort Lines Lexicographically (Ascending)", "Edit"},
    {IDM_EDIT_SORTLINES_LEXICOGRAPHIC_DESCENDING, "Sort Lines Lexicographically (Descending)", "Edit"},
    {IDM_EDIT_REMOVEEMPTYLINES, "Remove Empty Lines", "Edit"},
    {IDM_EDIT_REMOVEEMPTYLINESWITHBLANK, "Remove Empty Lines (All)", "Edit"},
    {IDM_EDIT_BLANKLINEABOVECURRENT, "Blank Line Above", "Edit"},
    {IDM_EDIT_BLANKLINEBELOWCURRENT, "Blank Line Below", "Edit"},
    {IDM_SEARCH_FIND, "Find...", "Search"},
    {IDM_SEARCH_FINDNEXT, "Find Next", "Search"},
    {IDM_SEARCH_FINDPREV, "Find Previous", "Search"},
    {IDM_SEARCH_REPLACE, "Replace...", "Search"},
    {IDM_SEARCH_FINDINFILES, "Find in Files...", "Search"},
    {IDM_SEARCH_CLEAR_BOOKMARKS, "Clear All Bookmarks", "Search"},
    {IDM_SEARCH_GOTOLINE, "Go to Line...", "Search"},
    {IDM_SEARCH_GOTOMATCHINGBRACE, "Go to Matching Brace", "Search"},
    {IDM_SEARCH_MARK, "Mark All...", "Search"},
    {IDM_SEARCH_MARKALLEXT1, "Mark All (Style 1)", "Search"},
    {IDM_SEARCH_MARKALLEXT2, "Mark All (Style 2)", "Search"},
    {IDM_SEARCH_MARKALLEXT3, "Mark All (Style 3)", "Search"},
    {IDM_SEARCH_MARKALLEXT4, "Mark All (Style 4)", "Search"},
    {IDM_SEARCH_MARKALLEXT5, "Mark All (Style 5)", "Search"},
    {IDM_SEARCH_CLEARALLMARKS, "Clear All Marks", "Search"},
    {IDM_SEARCH_ALLSTYLESTOCLIP, "Copy All Marks to Clipboard", "Search"},
    {IDM_SEARCH_CUTMARKEDLINES, "Cut Marked Lines", "Search"},
    {IDM_SEARCH_COPYMARKEDLINES, "Copy Marked Lines", "Search"},
    {IDM_SEARCH_DELETEMARKEDLINES, "Delete Marked Lines", "Search"},
    {IDM_SEARCH_INVERSEMARKS, "Inverse Marks", "Search"},
    {IDM_SEARCH_FINDCHARINRANGE, "Find Char in Range...", "Search"},
    {IDM_SEARCH_MARKONEEXT1, "Mark One (Style 1)", "Search"},
    {IDM_VIEW_ALWAYSONTOP, "Always on Top", "View"},
    {IDM_VIEW_FULLSCREENTOGGLE, "Full Screen", "View"},
    {IDM_VIEW_POSTIT, "Post It", "View"},
    {IDM_VIEW_DISTRACTIONFREE, "Distraction Free", "View"},
    {IDM_VIEW_EOL, "Show EOL", "View"},
    {IDM_VIEW_NPC, "Show NPC", "View"},
    {IDM_VIEW_NPC_CCUNIEOL, "Show CC UNI EOL", "View"},
    {IDM_VIEW_INDENT_GUIDE, "Show Indent Guide", "View"},
    {IDM_VIEW_SYNSCROLLH, "Synchronous Horizontal Scroll", "View"},
    {IDM_VIEW_SYNSCROLLV, "Synchronous Vertical Scroll", "View"},
    {IDM_VIEW_FOLDALL, "Fold All", "View"},
    {IDM_VIEW_FOLD, "Toggle Current Fold", "View"},
    {IDM_VIEW_FOLD_CURRENT, "Fold Current Level", "View"},
    {IDM_VIEW_FOLD_1, "Fold Level 1", "View"},
    {IDM_VIEW_FOLD_2, "Fold Level 2", "View"},
    {IDM_VIEW_FOLD_3, "Fold Level 3", "View"},
    {IDM_VIEW_UNFOLD_1, "Unfold Level 1", "View"},
    {IDM_VIEW_UNFOLD_2, "Unfold Level 2", "View"},
    {IDM_VIEW_UNFOLD_3, "Unfold Level 3", "View"},
    {IDM_VIEW_GOTO_START, "Go to Start", "View"},
    {IDM_VIEW_GOTO_END, "Go to End", "View"},
    {IDM_VIEW_DOC_MAP, "Document Map", "View"},
    {IDM_VIEW_FUNC_LIST, "Function List", "View"},
    {IDM_VIEW_FILEBROWSER, "File Browser", "View"},
    {IDM_VIEW_PROJECT_PANEL_1, "Project Panel 1", "View"},
    {IDM_VIEW_PROJECT_PANEL_2, "Project Panel 2", "View"},
    {IDM_VIEW_PROJECT_PANEL_3, "Project Panel 3", "View"},
    {IDM_VIEW_DOCLIST, "Document List", "View"},
    {IDM_VIEW_SWITCHTO_OTHER_VIEW, "Switch to Other View", "View"},
    {IDM_VIEW_CLONE_TO_ANOTHER_VIEW, "Clone to Another View", "View"},
    {IDM_VIEW_GOTO_ANOTHER_VIEW, "Go to Another View", "View"},
    {IDM_VIEW_LOAD_IN_NEW_INSTANCE, "Open in New Instance", "View"},
    {IDM_VIEW_HIDELINES, "Hide Current Line", "View"},
    {IDM_VIEW_SUMMARY, "Document Summary", "View"},
    {IDM_VIEW_MONITORING, "File Monitoring", "View"},
    {IDM_VIEW_ALL_CHARACTERS, "Show All Characters", "View"},
    {IDM_FORMAT_ANSI, "Encode as ANSI", "Encoding"},
    {IDM_FORMAT_UTF_8, "Encode as UTF-8", "Encoding"},
    {IDM_FORMAT_UTF_16BE, "Encode as UTF-16 BE", "Encoding"},
    {IDM_FORMAT_UTF_16LE, "Encode as UTF-16 LE", "Encoding"},
    {IDM_FORMAT_CONV2_ANSI, "Convert to ANSI", "Encoding"},
    {IDM_FORMAT_CONV2_UTF_8, "Convert to UTF-8", "Encoding"},
    {IDM_FORMAT_CONV2_UTF_16BE, "Convert to UTF-16 BE", "Encoding"},
    {IDM_FORMAT_CONV2_UTF_16LE, "Convert to UTF-16 LE", "Encoding"},
    {IDM_EXECUTE, "Open Command Prompt...", "Run"},
    {IDM_SETTING_PREFERENCE, "Preferences...", "Settings"},
    {IDM_SETTING_SHORTCUT_MAPPER, "Shortcut Mapper...", "Settings"},
    {IDM_SETTING_IMPORTPLUGIN, "Import Plugin...", "Settings"},
    {IDM_SETTING_IMPORTSTYLETHEMES, "Import Style Themes...", "Settings"},
    {IDM_SETTING_EDITCONTEXTMENU, "Edit Context Menu...", "Settings"},
    {IDM_WINDOW_WINDOWS, "Window List", "Window"},
};

static const int kCommandCount = sizeof(kCommands) / sizeof(kCommands[0]);

// =============================================================================
// ShortcutMapperDialog
// =============================================================================

ShortcutMapperDialog::ShortcutMapperDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Shortcut Mapper"));
    setMinimumSize(800, 550);
    setupUi();
    populateShortcuts();
    loadShortcuts();
}

void ShortcutMapperDialog::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    // Filter row
    auto* filterRow = new QHBoxLayout;

    auto* catLabel = new QLabel(QStringLiteral("Category:"), this);
    _categoryFilter = new QComboBox(this);
    _categoryFilter->addItems({
        QStringLiteral("All"),
        QStringLiteral("File"),
        QStringLiteral("Edit"),
        QStringLiteral("Search"),
        QStringLiteral("View"),
        QStringLiteral("Encoding"),
        QStringLiteral("Language"),
        QStringLiteral("Macro"),
        QStringLiteral("Run"),
        QStringLiteral("Settings"),
    });
    connect(_categoryFilter, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &ShortcutMapperDialog::onCategoryFilterChanged);

    _searchEdit = new QLineEdit(this);
    _searchEdit->setPlaceholderText(QStringLiteral("Search commands..."));
    connect(_searchEdit, &QLineEdit::textChanged,
            this, &ShortcutMapperDialog::onSearchTextChanged);

    _btnClear = new QPushButton(QStringLiteral("Clear Selected"), this);
    connect(_btnClear, &QPushButton::clicked, this, &ShortcutMapperDialog::onClearShortcut);

    _btnResetAll = new QPushButton(QStringLiteral("Reset All"), this);
    connect(_btnResetAll, &QPushButton::clicked, this, &ShortcutMapperDialog::onResetAll);

    filterRow->addWidget(catLabel);
    filterRow->addWidget(_categoryFilter);
    filterRow->addWidget(_searchEdit, 1);
    filterRow->addWidget(_btnClear);
    filterRow->addWidget(_btnResetAll);

    mainLayout->addLayout(filterRow);

    // Shortcut tree
    _shortcutTree = new QTreeWidget(this);
    _shortcutTree->setHeaderLabels({
        QStringLiteral("Category"),
        QStringLiteral("Command"),
        QStringLiteral("Shortcut 1"),
        QStringLiteral("Shortcut 2"),
    });
    _shortcutTree->header()->setStretchLastSection(false);
    _shortcutTree->header()->setSectionResizeMode(0, QHeaderView::Fixed);
    _shortcutTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    _shortcutTree->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    _shortcutTree->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    _shortcutTree->setColumnWidth(0, 100);
    _shortcutTree->setAlternatingRowColors(true);
    connect(_shortcutTree, &QTreeWidget::itemDoubleClicked,
            this, &ShortcutMapperDialog::onItemDoubleClicked);

    mainLayout->addWidget(_shortcutTree, 1);

    // Buttons
    _buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &ShortcutMapperDialog::onOk);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &ShortcutMapperDialog::onCancel);
    connect(_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &ShortcutMapperDialog::onApply);

    mainLayout->addWidget(_buttonBox);
}

void ShortcutMapperDialog::populateShortcuts()
{
    _entries.reserve(kCommandCount);

    for (int i = 0; i < kCommandCount; ++i) {
        const CommandDef& def = kCommands[i];
        ShortcutEntry entry;
        entry.commandId = def.id;
        entry.category = QString::fromLatin1(def.category);
        entry.commandName = QString::fromLatin1(def.name);
        entry.shortcut1.clear();
        entry.shortcut2.clear();
        entry.isModified = false;
        _entries.append(entry);
    }

    populateCommandTree();
}

void ShortcutMapperDialog::populateCommandTree()
{
    _shortcutTree->clear();

    for (const ShortcutEntry& entry : _entries) {
        auto* item = new QTreeWidgetItem(_shortcutTree);
        item->setText(0, entry.category);
        item->setText(1, entry.commandName);
        item->setText(2, entry.shortcut1.isEmpty() ? QStringLiteral("-") : entry.shortcut1);
        item->setText(3, entry.shortcut2.isEmpty() ? QStringLiteral("-") : entry.shortcut2);
        item->setData(0, Qt::UserRole, entry.commandId);
        _shortcutTree->addTopLevelItem(item);
    }
}

void ShortcutMapperDialog::loadShortcuts()
{
    QSettings s(QStringLiteral("NotepadPlusPlus"), QStringLiteral("npp-qt-shortcuts"));
    s.beginGroup(QStringLiteral("Shortcuts"));

    for (ShortcutEntry& entry : _entries) {
        QString key = QStringLiteral("cmd_%1/shortcut1").arg(entry.commandId);
        entry.shortcut1 = s.value(key, QString()).toString();

        QString key2 = QStringLiteral("cmd_%1/shortcut2").arg(entry.commandId);
        entry.shortcut2 = s.value(key2, QString()).toString();
    }

    s.endGroup();
    populateCommandTree();
}

void ShortcutMapperDialog::saveShortcuts()
{
    QSettings s(QStringLiteral("NotepadPlusPlus"), QStringLiteral("npp-qt-shortcuts"));
    s.beginGroup(QStringLiteral("Shortcuts"));

    for (const ShortcutEntry& entry : _entries) {
        QString key = QStringLiteral("cmd_%1/shortcut1").arg(entry.commandId);
        s.setValue(key, entry.shortcut1);

        QString key2 = QStringLiteral("cmd_%1/shortcut2").arg(entry.commandId);
        s.setValue(key2, entry.shortcut2);
    }

    s.endGroup();
    s.sync();
}

void ShortcutMapperDialog::onCategoryFilterChanged(int index)
{
    QString filterCat;
    if (index == 0) {
        filterCat.clear(); // All
    } else {
        filterCat = _categoryFilter->itemText(index);
    }

    for (int i = 0; i < _shortcutTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _shortcutTree->topLevelItem(i);
        if (filterCat.isEmpty() || item->text(0) == filterCat) {
            item->setHidden(false);
        } else {
            item->setHidden(true);
        }
    }
}

void ShortcutMapperDialog::onSearchTextChanged(const QString& text)
{
    QString search = text.toLower();
    for (int i = 0; i < _shortcutTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = _shortcutTree->topLevelItem(i);
        if (search.isEmpty() ||
            item->text(1).toLower().contains(search) ||
            item->text(0).toLower().contains(search)) {
            item->setHidden(false);
        } else {
            item->setHidden(true);
        }
    }
}

void ShortcutMapperDialog::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (!item || (column != 2 && column != 3))
        return;

    int row = _shortcutTree->indexOfTopLevelItem(item);
    if (row < 0 || row >= _entries.size())
        return;

    ShortcutEntry& entry = _entries[row];

    // Show key sequence input dialog
    QInputDialog dlg(this);
    dlg.setWindowTitle(column == 2 ? QStringLiteral("Enter Shortcut 1") : QStringLiteral("Enter Shortcut 2"));
    dlg.setLabelText(QStringLiteral("Press a key combination (e.g. Ctrl+Shift+P):"));
    dlg.setInputMode(QInputDialog::TextInput);

    if (dlg.exec() != QDialog::Accepted)
        return;

    QString newSeq = dlg.textValue().trimmed();
    applyShortcut(row, newSeq);
}

void ShortcutMapperDialog::applyShortcut(int row, const QString& keySeq)
{
    if (row < 0 || row >= _entries.size())
        return;

    ShortcutEntry& entry = _entries[row];
    QTreeWidgetItem* item = _shortcutTree->topLevelItem(row);

    // Check for conflicts
    for (int i = 0; i < _entries.size(); ++i) {
        if (i == row)
            continue;
        if (_entries[i].shortcut1 == keySeq || _entries[i].shortcut2 == keySeq) {
            QMessageBox::warning(this, QStringLiteral("Conflict"),
                QStringLiteral("The shortcut '%1' is already assigned to '%2'.\n\nProceed anyway?").arg(keySeq, _entries[i].commandName),
                QMessageBox::Yes | QMessageBox::No);
            break;
        }
    }

    item->setText(2, keySeq.isEmpty() ? QStringLiteral("-") : keySeq);
    entry.shortcut1 = keySeq;
    entry.isModified = true;
}

void ShortcutMapperDialog::onClearShortcut()
{
    QTreeWidgetItem* item = _shortcutTree->currentItem();
    if (!item)
        return;

    int row = _shortcutTree->indexOfTopLevelItem(item);
    if (row < 0 || row >= _entries.size())
        return;

    ShortcutEntry& entry = _entries[row];
    entry.shortcut1.clear();
    entry.shortcut2.clear();
    entry.isModified = true;
    item->setText(2, QStringLiteral("-"));
    item->setText(3, QStringLiteral("-"));
}

void ShortcutMapperDialog::onResetAll()
{
    auto reply = QMessageBox::question(this, QStringLiteral("Reset All Shortcuts"),
        QStringLiteral("Are you sure you want to reset all shortcuts to defaults?"),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    for (ShortcutEntry& entry : _entries) {
        entry.shortcut1.clear();
        entry.shortcut2.clear();
        entry.isModified = false;
    }
    populateCommandTree();
}

void ShortcutMapperDialog::onApply()
{
    saveShortcuts();
}

void ShortcutMapperDialog::onOk()
{
    saveShortcuts();
    accept();
}

void ShortcutMapperDialog::onCancel()
{
    reject();
}
