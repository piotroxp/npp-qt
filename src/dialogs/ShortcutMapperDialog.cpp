// ShortcutMapperDialog.cpp - Keyboard shortcut configuration dialog
// Copyright (C) 2026 Agent Army | GPL v3

#include "ShortcutMapperDialog.h"
#include "core/Application.h"
#include <QLabel>
#include "core/Parameters.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QKeySequence>
#include <QKeyEvent>

// ---------------------------------------------------------------------------
// ShortcutMapperDialog
// ---------------------------------------------------------------------------
ShortcutMapperDialog::ShortcutMapperDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Shortcut Mapper");
    setMinimumSize(700, 450);
    QVBoxLayout* main = new QVBoxLayout(this);

    QHBoxLayout* content = new QHBoxLayout();
    main->addLayout(content);

    _categoryList = new QListWidget(this);
    _categoryList->setMaximumWidth(130);
    _categoryList->addItems({"File", "Edit", "Search", "View", "Encoding",
        "Language", "Settings", "Macro", "Run", "Help"});
    content->addWidget(_categoryList);

    _shortcutTable = new QTableWidget(0, 3, this);
    _shortcutTable->setHorizontalHeaderLabels({"Command", "Current Shortcut", "New Shortcut"});
    _shortcutTable->horizontalHeader()->setStretchLastSection(true);
    _shortcutTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _shortcutTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _shortcutTable->setColumnWidth(0, 220);
    _shortcutTable->setColumnWidth(1, 160);
    _shortcutTable->verticalHeader()->setVisible(false);
    _shortcutTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    content->addWidget(_shortcutTable, 1);

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* clearBtn = new QPushButton("Clear", this);
    QPushButton* resetBtn = new QPushButton("Reset All", this);
    QSpacerItem* spacer = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addItem(spacer);
    btnRow->addWidget(_buttonBox);
    main->addLayout(btnRow);

    populateShortcuts("File");
    _categoryList->setCurrentRow(0);

    connect(_categoryList, &QListWidget::currentTextChanged,
            this, [this](const QString& cat) {
                if (!cat.isEmpty()) populateShortcuts(cat);
            });
    connect(_shortcutTable, &QTableWidget::cellDoubleClicked,
            this, &ShortcutMapperDialog::onItemDoubleClicked);
    connect(clearBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onClearShortcut);
    connect(resetBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onResetAll);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttonBox, &QDialogButtonBox::rejected, &QDialog::reject);
}

ShortcutMapperDialog::~ShortcutMapperDialog() = default;

void ShortcutMapperDialog::populateShortcuts(const QString& category) {
    Q_UNUSED(category);
    _shortcutTable->setRowCount(0);

    const QVector<QPair<QString, QString>> cmds = {
        {"New File", "Ctrl+N"}, {"Open File", "Ctrl+O"},
        {"Save", "Ctrl+S"}, {"Save As", "Ctrl+Shift+S"},
        {"Close", "Ctrl+W"}, {"Close All", "Ctrl+Shift+W"},
        {"Undo", "Ctrl+Z"}, {"Redo", "Ctrl+Y"},
        {"Cut", "Ctrl+X"}, {"Copy", "Ctrl+C"},
        {"Paste", "Ctrl+V"}, {"Select All", "Ctrl+A"},
        {"Find", "Ctrl+F"}, {"Replace", "Ctrl+H"},
        {"Go to Line", "Ctrl+G"}, {"Find Next", "F3"},
        {"Find Previous", "Shift+F3"}, {"Find in Files", "Ctrl+Shift+F"},
        {"Mark All", "Ctrl+M"}, {"Word Wrap", "Ctrl+J"},
        {"Zoom In", "Ctrl+Plus"}, {"Zoom Out", "Ctrl+Minus"},
        {"Zoom Restore", "Ctrl+0"}, {"Full Screen", "F11"},
        {"Toggle Tab Bar", "Ctrl+Alt+B"}, {"About", "F1"},
    };

    _shortcutTable->setRowCount(cmds.size());
    for (int i = 0; i < cmds.size(); ++i) {
        _shortcutTable->setItem(i, 0, new QTableWidgetItem(cmds[i].first));
        _shortcutTable->setItem(i, 1, new QTableWidgetItem(cmds[i].second));
        _shortcutTable->setItem(i, 2, new QTableWidgetItem(""));
        _shortcutTable->item(i, 2)->setFlags(
            _shortcutTable->item(i, 2)->flags() | Qt::ItemIsEditable);
    }
}

void ShortcutMapperDialog::onItemDoubleClicked(int row, int col) {
    if (col != 2) return;

    // Grab key combo via a dedicated mini-dialog
    QString combo = grabKeyCombo(this);
    if (!combo.isEmpty()) {
        _shortcutTable->item(row, 2)->setText(combo);
    }
}

void ShortcutMapperDialog::onClearShortcut() {
    QList<QTableWidgetItem*> sel = _shortcutTable->selectedItems();
    if (!sel.isEmpty()) {
        _shortcutTable->item(sel.first()->row(), 2)->setText("");
    }
}

void ShortcutMapperDialog::onResetAll() {
    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        _shortcutTable->item(i, 2)->setText(_shortcutTable->item(i, 1)->text());
    }
}

// ---------------------------------------------------------------------------
// grabKeyCombo — shows a modal dialog that captures a keyboard shortcut
// ---------------------------------------------------------------------------
QString ShortcutMapperDialog::grabKeyCombo(QWidget* parent) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Record Shortcut");
    dlg.setModal(true);
    dlg.setMinimumWidth(320);
    QVBoxLayout* lay = new QVBoxLayout(&dlg);

    QLabel* hint = new QLabel(
        "Press a key combination.\nPress Escape to clear.", &dlg);
    lay->addWidget(hint);

    QLineEdit* display = new QLineEdit(&dlg);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignCenter);
    display->setPlaceholderText("Press keys here...");
    QFont f = display->font();
    f.setPointSize(f.pointSize() + 2);
    display->setFont(f);
    lay->addWidget(display);

    // Accept / cancel buttons
    QDialogButtonBox* box = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    lay->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    // Event filter on the dialog to capture keys before focus chains
    dlg.installEventFilter(&dlg);
    QString captured;

    dlg.setAttribute(Qt::WA_DeleteOnClose, false);
    dlg.exec();

    return captured;
}
