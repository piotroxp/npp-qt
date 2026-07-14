// ShortcutMapperDialog.cpp - Keyboard shortcut configuration dialog
// Copyright (C) 2026 Agent Army | GPL v3

#include "ShortcutMapperDialog.h"
#include "core/Application.h"
#include "core/EditorCommandManager.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QKeySequence>
#include <QDebug>

// ---------------------------------------------------------------------------
// Command data: name → {category, default shortcut}
// ---------------------------------------------------------------------------
struct CommandDef {
    const char* category;
    const char* defaultShortcut;
};

static const QMap<QString, CommandDef>& allCommandDefs() {
    static QMap<QString, CommandDef> map;
    if (map.isEmpty()) {
        // File
        map["New File"]           = {"File", "Ctrl+N"};
        map["Open File"]          = {"File", "Ctrl+O"};
        map["Save"]               = {"File", "Ctrl+S"};
        map["Save As"]            = {"File", "Ctrl+Shift+A"};
        map["Save All"]           = {"File", "Ctrl+Shift+S"};
        map["Close"]              = {"File", "Ctrl+W"};
        map["Close All"]          = {"File", "Ctrl+Shift+W"};
        map["Print"]              = {"File", "Ctrl+P"};
        map["Exit"]               = {"File", "Alt+F4"};
        // Edit
        map["Undo"]               = {"Edit", "Ctrl+Z"};
        map["Redo"]               = {"Edit", "Ctrl+Y"};
        map["Cut"]                = {"Edit", "Ctrl+X"};
        map["Copy"]               = {"Edit", "Ctrl+C"};
        map["Paste"]              = {"Edit", "Ctrl+V"};
        map["Select All"]         = {"Edit", "Ctrl+A"};
        map["Delete Line"]        = {"Edit", "Ctrl+Shift+L"};
        map["Find"]               = {"Edit", "Ctrl+F"};
        map["Replace"]            = {"Edit", "Ctrl+H"};
        map["Go to Line"]         = {"Edit", "Ctrl+G"};
        map["Column Editor"]      = {"Edit", "Alt+C"};
        // Search
        map["Find Next"]          = {"Search", "F3"};
        map["Find Previous"]      = {"Search", "Shift+F3"};
        map["Find in Files"]      = {"Search", "Ctrl+Shift+F"};
        map["Find Incremental"]    = {"Search", "Ctrl+Alt+I"};
        map["Mark"]                = {"Search", "Ctrl+M"};
        map["Mark All"]            = {"Search", "Ctrl+Shift+M"};
        map["Clear Marks"]        = {"Search", "Ctrl+Shift+N"};
        // View
        map["Zoom In"]            = {"View", "Ctrl++"};
        map["Zoom Out"]           = {"View", "Ctrl+-"};
        map["Zoom Restore"]       = {"View", "Ctrl+0"};
        map["Full Screen"]        = {"View", "F11"};
        map["Toggle Tab Bar"]     = {"View", "Ctrl+Alt+B"};
        map["Word Wrap"]          = {"View", "Ctrl+J"};
        map["Document List"]      = {"View", "Ctrl+Alt+N"};
        // Encoding
        map["Convert to ANSI"]    = {"Encoding", ""};
        map["Convert to UTF-8"]   = {"Encoding", ""};
        map["Convert to UTF-8 BOM"] = {"Encoding", ""};
        map["Convert to UTF-16 LE"] = {"Encoding", ""};
        map["Convert to UTF-16 BE"] = {"Encoding", ""};
        map["Encode as ANSI"]    = {"Encoding", ""};
        map["Encode as UTF-8"]   = {"Encoding", ""};
        // Settings
        map["Preferences"]        = {"Settings", "Ctrl+,"};
        map["Shortcut Mapper"]    = {"Settings", ""};
        map["Command Palette"]    = {"Settings", "Ctrl+Shift+P"};
        map["Toggle Toolbar"]     = {"Settings", ""};
        map["Toggle Status Bar"]  = {"Settings", ""};
        // Macro
        map["Start Recording"]    = {"Macro", "Ctrl+Shift+R"};
        map["Stop Recording"]    = {"Macro", "Ctrl+Shift+R"};
        map["Playback"]           = {"Macro", "Ctrl+Shift+P"};
        // Run
        map["Run in Browser"]     = {"Run", "Ctrl+Alt+Shift+B"};
        map["Open in Notepad++"]  = {"Run", ""};
        map["Launch"]             = {"Run", "F5"};
        map["Validate"]           = {"Run", "Ctrl+Alt+V"};
        // Help
        map["About"]              = {"Help", "F1"};
        map["Documentation"]      = {"Help", ""};
        map["Check for Updates"]  = {"Help", ""};
        // Language
        map["Fold Current"]       = {"Language", ""};
        map["Unfold Current"]     = {"Language", ""};
        map["Fold All"]           = {"Language", ""};
        map["Unfold All"]         = {"Language", ""};
    }
    return map;
}

static QStringList categories() {
    return {"File", "Edit", "Search", "View", "Encoding", "Settings", "Macro", "Run", "Help", "Language"};
}

// ---------------------------------------------------------------------------
// ShortcutMapperDialog
// ---------------------------------------------------------------------------
ShortcutMapperDialog::ShortcutMapperDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Shortcut Mapper");
    setMinimumSize(720, 480);
    QVBoxLayout* main = new QVBoxLayout(this);

    QHBoxLayout* content = new QHBoxLayout();
    main->addLayout(content);

    _categoryList = new QListWidget(this);
    _categoryList->setMaximumWidth(140);
    _categoryList->addItems(categories());
    content->addWidget(_categoryList);

    _shortcutTable = new QTableWidget(0, 3, this);
    _shortcutTable->setHorizontalHeaderLabels({"Command", "Current", "New"});
    _shortcutTable->horizontalHeader()->setStretchLastSection(true);
    _shortcutTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _shortcutTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _shortcutTable->setColumnWidth(0, 240);
    _shortcutTable->setColumnWidth(1, 160);
    _shortcutTable->verticalHeader()->setVisible(false);
    _shortcutTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _shortcutTable->setAlternatingRowColors(true);
    content->addWidget(_shortcutTable, 1);

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* clearBtn = new QPushButton("Clear", this);
    QPushButton* resetBtn = new QPushButton("Reset All", this);
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addStretch();
    btnRow->addWidget(_buttonBox);
    main->addLayout(btnRow);

    populateShortcuts("File");
    _categoryList->setCurrentRow(0);

    connect(_categoryList, &QListWidget::currentTextChanged,
            this, &ShortcutMapperDialog::onCategoryChanged);
    connect(_shortcutTable, &QTableWidget::cellDoubleClicked,
            this, &ShortcutMapperDialog::onItemDoubleClicked);
    connect(clearBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onClearShortcut);
    connect(resetBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onResetAll);
    connect(_buttonBox, &QDialogButtonBox::clicked, this,
            [this](QAbstractButton* btn) {
                if (btn == _buttonBox->button(QDialogButtonBox::Ok)) {
                    if (validateAndSave()) accept();
                } else if (btn == _buttonBox->button(QDialogButtonBox::Apply)) {
                    validateAndSave();
                } else if (btn == _buttonBox->button(QDialogButtonBox::Cancel)) {
                    reject();
                }
            });
}

ShortcutMapperDialog::~ShortcutMapperDialog() = default;

// ---------------------------------------------------------------------------
// populateShortcuts
// ---------------------------------------------------------------------------
void ShortcutMapperDialog::populateShortcuts(const QString& category) {
    _shortcutTable->setRowCount(0);
    _currentShortcuts.clear();
    _originalShortcuts.clear();

    const QMap<QString, CommandDef>& defs = allCommandDefs();
    const QStringList& cats = categories();
    const QString catName = category.isEmpty() ? cats.value(0) : category;

    // Collect commands belonging to this category
    QVector<QPair<QString, QString>> cmds; // name, default shortcut
    for (auto it = defs.constBegin(); it != defs.constEnd(); ++it) {
        if (it.value().category == catName) {
            // Try to get the actual registered shortcut from command manager
            QString registered = app().commandManager()->getShortcut(it.key());
            QString shortcut = registered.isEmpty() ? it.value().defaultShortcut : registered;
            cmds.append({it.key(), shortcut});
        }
    }

    // Sort alphabetically
    std::sort(cmds.begin(), cmds.end(),
              [](const QPair<QString,QString>& a, const QPair<QString,QString>& b) {
                  return a.first.localeAwareCompare(b.first) < 0;
              });

    _shortcutTable->setRowCount(cmds.size());
    for (int i = 0; i < cmds.size(); ++i) {
        const QString& name = cmds[i].first;
        const QString& shortcut = cmds[i].second;

        _shortcutTable->setItem(i, 0, new QTableWidgetItem(name));

        QTableWidgetItem* curItem = new QTableWidgetItem(shortcut);
        curItem->setFlags(curItem->flags() & ~Qt::ItemIsEditable);
        curItem->setForeground(QColor("#808080"));
        _shortcutTable->setItem(i, 1, curItem);

        QTableWidgetItem* newItem = new QTableWidgetItem("");
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        _shortcutTable->setItem(i, 2, newItem);

        _currentShortcuts[name] = shortcut;
        _originalShortcuts[name] = shortcut;
    }
}

// ---------------------------------------------------------------------------
// Interaction
// ---------------------------------------------------------------------------
void ShortcutMapperDialog::onCategoryChanged(const QString& cat) {
    populateShortcuts(cat);
}

void ShortcutMapperDialog::onItemDoubleClicked(int row, int col) {
    if (col != 2) return;

    QString combo = grabKeyCombo();
    if (!combo.isNull()) {
        _shortcutTable->item(row, 2)->setText(combo);
        QString name = _shortcutTable->item(row, 0)->text();
        _currentShortcuts[name] = combo;

        // Check for conflicts
        if (!combo.isEmpty()) {
            checkAndWarnConflict(name, combo);
        }
    }
}

void ShortcutMapperDialog::onClearShortcut() {
    QList<QTableWidgetItem*> sel = _shortcutTable->selectedItems();
    if (!sel.isEmpty()) {
        int row = sel.first()->row();
        _shortcutTable->item(row, 2)->setText("");
        QString name = _shortcutTable->item(row, 0)->text();
        _currentShortcuts[name] = "";
    }
}

void ShortcutMapperDialog::onResetAll() {
    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        QString name = _shortcutTable->item(i, 0)->text();
        QString orig = _originalShortcuts.value(name, "");
        _shortcutTable->item(i, 2)->setText(orig);
        _currentShortcuts[name] = orig;
    }
}

void ShortcutMapperDialog::onApply() {
    validateAndSave();
}

// ---------------------------------------------------------------------------
// Conflict detection
// ---------------------------------------------------------------------------
void ShortcutMapperDialog::checkAndWarnConflict(const QString& changedCmd, const QString& newShortcut) {
    if (newShortcut.isEmpty()) return;

    // Check against other commands in the same category
    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        QString name = _shortcutTable->item(i, 0)->text();
        if (name == changedCmd) continue;

        // Check both "current" and "new" columns for conflicts
        QString curShortcut = _shortcutTable->item(i, 1)->text();
        QString newColShortcut = _shortcutTable->item(i, 2)->text().trimmed();
        QString conflict = newColShortcut.isEmpty() ? curShortcut : newColShortcut;

        if (conflict.toLower() == newShortcut.toLower()) {
            QMessageBox::warning(this, "Shortcut Conflict",
                QString("The shortcut \"%1\" is already assigned to \"%2\".\n"
                        "Assigning it to \"%3\" will remove it from \"%2\".")
                    .arg(newShortcut, name, changedCmd));
            // Clear the conflicting entry's new-shortcut field
            if (!newColShortcut.isEmpty()) {
                _shortcutTable->item(i, 2)->setText("");
                _currentShortcuts[name] = curShortcut;
            }
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Validate & Save
// ---------------------------------------------------------------------------
bool ShortcutMapperDialog::validateAndSave() {
    // Build a map of shortcut → command name for conflict checking
    QMap<QString, QString> shortcutToCmd;
    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        QString name = _shortcutTable->item(i, 0)->text();
        QString newShortcut = _shortcutTable->item(i, 2)->text().trimmed();
        if (!newShortcut.isEmpty()) {
            QString lower = newShortcut.toLower();
            if (shortcutToCmd.contains(lower) &&
                shortcutToCmd[lower] != name) {
                QMessageBox::warning(this, "Shortcut Conflict",
                    QString("The shortcut \"%1\" is assigned to both \"%2\" and \"%3\". "
                            "Please resolve the conflict before saving.")
                        .arg(newShortcut, shortcutToCmd[lower], name));
                return false;
            }
            shortcutToCmd[lower] = name;
        }
    }

    saveShortcuts();
    return true;
}

// ---------------------------------------------------------------------------
// Key grabber
// ---------------------------------------------------------------------------
QString ShortcutMapperDialog::grabKeyCombo() {
    QDialog dlg(this);
    dlg.setWindowTitle("Record Shortcut");
    dlg.setModal(true);
    dlg.setMinimumWidth(320);

    QVBoxLayout* lay = new QVBoxLayout(&dlg);
    QLabel* hint = new QLabel("Press a key combination.\nPress Escape to cancel.", &dlg);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("font-size: 13pt; color: #d4d4d4;");
    lay->addWidget(hint);

    QLineEdit* display = new QLineEdit(&dlg);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignCenter);
    display->setPlaceholderText("Press keys here...");
    QFont f = display->font();
    f.setPointSize(f.pointSize() + 2);
    display->setFont(f);
    display->setStyleSheet(R"(
        background: #1e1e1e;
        color: #d4d4d4;
        border: 1px solid #3c3c3c;
        border-radius: 3px;
        padding: 6px;
    )");
    lay->addWidget(display);

    QLabel* note = new QLabel("Modifiers are accepted; a main key is required.", &dlg);
    note->setAlignment(Qt::AlignCenter);
    note->setStyleSheet("color: #808080; font-size: 10pt;");
    lay->addWidget(note);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    lay->addWidget(box);

    // Grabber captures keystrokes and updates display
    KeyGrabber grabber(display);
    connect(&grabber, &KeyGrabber::captured, this, [](const QString&) {});

    // Only call exec() once — inside grabber.run()
    QString result = grabber.run(&dlg);
    return result;
}

// ---------------------------------------------------------------------------
// Apply shortcuts
// ---------------------------------------------------------------------------
void ShortcutMapperDialog::saveShortcuts() {
    int applied = 0;
    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        QString name = _shortcutTable->item(i, 0)->text();
        QString newShortcut = _shortcutTable->item(i, 2)->text().trimmed();
        if (!newShortcut.isEmpty()) {
            app().commandManager()->setShortcut(name, newShortcut);
            ++applied;
        }
    }
    if (applied > 0) {
        QMessageBox::information(this, "Shortcut Mapper",
            QString("Applied %1 shortcut%2.")
                .arg(applied).arg(applied == 1 ? "" : "s"));
    }
}
