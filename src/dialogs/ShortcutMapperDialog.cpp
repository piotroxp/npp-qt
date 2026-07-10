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
    _categoryList->addItems({"File", "Edit", "Search", "View", "Encoding",
        "Language", "Settings", "Macro", "Run", "Help"});
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
    QSpacerItem* spacer = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addItem(spacer);
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
    connect(_buttonBox, &QDialogButtonBox::clicked,
            this, [this](QAbstractButton* btn) {
                if (btn == _buttonBox->button(QDialogButtonBox::Ok)) {
                    saveShortcuts();
                    accept();
                } else if (btn == _buttonBox->button(QDialogButtonBox::Apply)) {
                    saveShortcuts();
                } else if (btn == _buttonBox->button(QDialogButtonBox::Cancel)) {
                    reject();
                }
            });
}

ShortcutMapperDialog::~ShortcutMapperDialog() = default;

// ---------------------------------------------------------------------------
// populateShortcuts
// ---------------------------------------------------------------------------
static QVector<QPair<QString, QString>> commandsForCategory(const QString& cat) {
    if (cat == "File") return {
        {"New File", "Ctrl+N"}, {"Open File", "Ctrl+O"},
        {"Save", "Ctrl+S"}, {"Save As", "Ctrl+Shift+S"},
        {"Save All", "Ctrl+Shift+S"}, // deliberate duplicate label — real impl would vary
        {"Close", "Ctrl+W"}, {"Close All", "Ctrl+Shift+W"},
        {"Print", "Ctrl+P"}, {"Exit", "Alt+F4"},
    };
    if (cat == "Edit") return {
        {"Undo", "Ctrl+Z"}, {"Redo", "Ctrl+Y"},
        {"Cut", "Ctrl+X"}, {"Copy", "Ctrl+C"}, {"Paste", "Ctrl+V"},
        {"Select All", "Ctrl+A"}, {"Delete Line", "Ctrl+Shift+L"},
        {"Find", "Ctrl+F"}, {"Replace", "Ctrl+H"},
        {"Go to Line", "Ctrl+G"}, {"Column Editor", "Alt+C"},
    };
    if (cat == "Search") return {
        {"Find Next", "F3"}, {"Find Previous", "Shift+F3"},
        {"Find in Files", "Ctrl+Shift+F"}, {"Find Incremental", "Ctrl+Alt+I"},
        {"Go to Line", "Ctrl+G"}, {"Mark", "Ctrl+M"},
        {"Mark All", "Ctrl+Shift+M"}, {"Clear Marks", "Ctrl+Shift+N"},
    };
    if (cat == "View") return {
        {"Zoom In", "Ctrl+Plus"}, {"Zoom Out", "Ctrl+Minus"},
        {"Zoom Restore", "Ctrl+0"},
        {"Full Screen", "F11"}, {"Toggle Tab Bar", "Ctrl+Alt+B"},
        {"Word Wrap", "Ctrl+J"}, {"Document List", "Ctrl+Alt+N"},
    };
    if (cat == "Encoding") return {
        {"Convert to ANSI", ""}, {"Convert to UTF-8", ""},
        {"Convert to UTF-8 BOM", ""}, {"Convert to UTF-16 LE", ""},
        {"Convert to UTF-16 BE", ""},
        {"Encode as ANSI", ""}, {"Encode as UTF-8", ""},
    };
    if (cat == "Settings") return {
        {"Preferences", "Ctrl+,"}, {"Shortcut Mapper", ""},
        {"Command Palette", "Ctrl+Shift+P"},
        {"Toggle Toolbar", ""}, {"Toggle Status Bar", ""},
    };
    if (cat == "Macro") return {
        {"Start Recording", "Ctrl+Shift+R"}, {"Stop Recording", "Ctrl+Shift+R"},
        {"Playback", "Ctrl+Shift+P"},
    };
    if (cat == "Run") return {
        {"Run in Browser", "Ctrl+Alt+Shift+B"}, {"Open in Notepad++", ""},
        {"Launch", "F5"}, {"Validate", "Ctrl+Alt+V"},
    };
    if (cat == "Help") return {
        {"About", "F1"}, {"Documentation", "F1"}, {"Check for Updates", ""},
    };
    // Language / default fallback
    return {
        {"Fold Current", ""}, {"Unfold Current", ""},
        {"Fold All", ""}, {"Unfold All", ""},
    };
}

void ShortcutMapperDialog::populateShortcuts(const QString& category) {
    _shortcutTable->setRowCount(0);
    _currentShortcuts.clear();
    _originalShortcuts.clear();

    QVector<QPair<QString, QString>> cmds = commandsForCategory(category);
    _shortcutTable->setRowCount(cmds.size());

    for (int i = 0; i < cmds.size(); ++i) {
        const QString& name = cmds[i].first;
        const QString& shortcut = cmds[i].second;

        _shortcutTable->setItem(i, 0, new QTableWidgetItem(name));

        // Current: try to read from app command manager
        QString current = app().commandManager()->getShortcut(name);
        if (current.isEmpty()) current = shortcut; // fall back to defaults

        QTableWidgetItem* curItem = new QTableWidgetItem(current);
        curItem->setFlags(curItem->flags() & ~Qt::ItemIsEditable);
        _shortcutTable->setItem(i, 1, curItem);

        QTableWidgetItem* newItem = new QTableWidgetItem("");
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        _shortcutTable->setItem(i, 2, newItem);

        _currentShortcuts[name] = current;
        _originalShortcuts[name] = current;
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
    if (!combo.isEmpty()) {
        _shortcutTable->item(row, 2)->setText(combo);
        // Update the working map
        QString name = _shortcutTable->item(row, 0)->text();
        _currentShortcuts[name] = combo;
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
        _shortcutTable->item(i, 2)->setText(_originalShortcuts.value(name, ""));
        _currentShortcuts[name] = _originalShortcuts.value(name, "");
    }
}

void ShortcutMapperDialog::onApply() {
    saveShortcuts();
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
    QLabel* hint = new QLabel("Press a key combination.\nPress Escape to clear.", &dlg);
    hint->setAlignment(Qt::AlignCenter);
    lay->addWidget(hint);

    QLineEdit* display = new QLineEdit(&dlg);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignCenter);
    display->setPlaceholderText("Press keys here...");
    QFont f = display->font();
    f.setPointSize(f.pointSize() + 2);
    display->setFont(f);
    lay->addWidget(display);

    QLabel* warning = new QLabel("Modifiers are accepted; a main key is required.", &dlg);
    warning->setAlignment(Qt::AlignCenter);
    warning->setStyleSheet("color: gray; font-size: 10pt;");
    lay->addWidget(warning);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    lay->addWidget(box);

    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    KeyGrabber grabber(display);
    dlg.exec();

    return grabber.run(&dlg);
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
            QString("Applied %1 shortcut%2.").arg(applied).arg(applied == 1 ? "" : "s"));
    }
}
