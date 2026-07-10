// ShortcutMapperDialog.cpp - Keyboard shortcut configuration dialog
// Copyright (C) 2026 Agent Army | GPL v3

#include "ShortcutMapperDialog.h"
#include "../core/ShortcutManager.h"
#include "../core/EditorCommandManager.h"
#include "../core/Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QKeyEvent>
#include <QLabel>

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
    _categoryList->addItems({"File", "Edit", "Search", "Macro", "View", "Encoding",
        "Language", "Settings", "Run", "Help"});
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

    connect(_categoryList, &QListWidget::currentTextChanged, this, &ShortcutMapperDialog::onCategoryChanged);
    connect(_shortcutTable, &QTableWidget::cellDoubleClicked, this, &ShortcutMapperDialog::onItemDoubleClicked);
    connect(clearBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onClearShortcut);
    connect(resetBtn, &QPushButton::clicked, this, &ShortcutMapperDialog::onResetAll);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, [this]() { onApplyAndClose(); });
    connect(_buttonBox, &QDialogButtonBox::rejected, this, [this]() { reject(); });
}

ShortcutMapperDialog::~ShortcutMapperDialog() = default;

void ShortcutMapperDialog::populateShortcuts(const QString& category) {
    _shortcutTable->setRowCount(0);

    static const QMap<QString, std::pair<int, int>> ranges = {
        {"File",     {1001, 1099}},
        {"Edit",    {1101, 1199}},
        {"Search",   {1201, 1299}},
        {"Macro",   {1801, 1899}},
        {"View",    {1301, 1399}},
        {"Encoding", {1401, 1499}},
        {"Language",{1501, 1599}},
        {"Settings",{1601, 1699}},
        {"Run",     {1701, 1799}},
        {"Help",    {1701, 1799}},
    };

    auto range = ranges.value(category, {0, 0});
    EditorCommandManager& cmdMgr = EditorCommandManager::instance();
    QVector<KeyBinding> allBindings = cmdMgr.getAllBindings();
    QVector<EditorCommandManager::CommandEntry> allCmds = cmdMgr.getAllCommands();

    int row = 0;
    for (const EditorCommandManager::CommandEntry& entry : allCmds) {
        if (entry.id < range.first || entry.id > range.second)
            continue;

        QString name = QString::fromStdString(entry.name);
        if (name.isEmpty()) continue;

        QString currentShortcut;
        for (const KeyBinding& b : allBindings) {
            if (b.commandId == entry.id) {
                currentShortcut = b.shortcutText;
                break;
            }
        }

        _shortcutTable->insertRow(row);
        QTableWidgetItem* nameItem = new QTableWidgetItem(name);
        nameItem->setData(Qt::UserRole, entry.id);
        _shortcutTable->setItem(row, 0, nameItem);
        _shortcutTable->setItem(row, 1, new QTableWidgetItem(currentShortcut));
        QTableWidgetItem* newItem = new QTableWidgetItem();
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        _shortcutTable->setItem(row, 2, newItem);
        ++row;
    }
}

void ShortcutMapperDialog::onCategoryChanged(const QString& category) {
    if (!category.isEmpty())
        populateShortcuts(category);
}

void ShortcutMapperDialog::onItemDoubleClicked(int row, int col) {
    if (col != 2) return;

    QString combo = grabKeyComboSync(this);
    if (combo.isEmpty()) return;

    int existingCmd = ShortcutManager::instance().getBoundCommand(combo);
    int currentCmd = _shortcutTable->item(row, 0)->data(Qt::UserRole).toInt();

    if (existingCmd >= 0 && existingCmd != currentCmd) {
        QString existingName = QString::fromStdString(
            EditorCommandManager::instance().getCommandName(existingCmd));
        QMessageBox::warning(this, "Shortcut Conflict",
            QString("The shortcut \"%1\" is already assigned to \"%2\".\n"
                    "Choose a different shortcut or clear the conflicting one first.")
                .arg(combo).arg(existingName));
        return;
    }

    _shortcutTable->item(row, 2)->setText(combo);
}

void ShortcutMapperDialog::onClearShortcut() {
    QList<QTableWidgetItem*> sel = _shortcutTable->selectedItems();
    if (!sel.isEmpty())
        _shortcutTable->item(sel.first()->row(), 2)->setText("");
}

void ShortcutMapperDialog::onResetAll() {
    for (int i = 0; i < _shortcutTable->rowCount(); ++i)
        _shortcutTable->item(i, 2)->setText(_shortcutTable->item(i, 1)->text());
}

void ShortcutMapperDialog::onApplyAndClose() {
    ShortcutManager& shortcuts = ShortcutManager::instance();

    for (int i = 0; i < _shortcutTable->rowCount(); ++i) {
        QString newShortcut = _shortcutTable->item(i, 2)->text().trimmed();
        if (newShortcut.isEmpty()) continue;
        int cmdId = _shortcutTable->item(i, 0)->data(Qt::UserRole).toInt();
        shortcuts.bindKey(cmdId, newShortcut);
    }

    shortcuts.saveToJson(shortcuts.defaultConfigPath());
    EditorCommandManager::instance().allShortcutsChanged();
    accept();
}

// ---------------------------------------------------------------------------
// grabKeyComboSync - capture a keyboard shortcut
// ---------------------------------------------------------------------------
QString ShortcutMapperDialog::grabKeyComboSync(QWidget* parent) {
    // Create a helper object to collect the result
    class GrabDialog : public QDialog {
    public:
        QString captured;
        QLineEdit* display;
        GrabDialog(QWidget* p) : QDialog(p) {
            setWindowTitle("Record Shortcut");
            setModal(true);
            setMinimumWidth(320);
            QVBoxLayout* lay = new QVBoxLayout(this);
            (void)lay;

            QLabel* hint = new QLabel("Press a key combination.\nPress Escape to cancel.", this);
            layout()->addWidget(hint);

            display = new QLineEdit(this);
            display->setReadOnly(true);
            display->setAlignment(Qt::AlignCenter);
            display->setPlaceholderText("Press keys here...");
            layout()->addWidget(display);

            QDialogButtonBox* box = new QDialogButtonBox(
                QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
            layout()->addWidget(box);

            connect(box, &QDialogButtonBox::accepted, this, [this]() { accept(); });
            connect(box, &QDialogButtonBox::rejected, this, [this]() { reject(); });
        }

        QString formatKey(int key, Qt::KeyboardModifiers mods) {
            QStringList parts;
            if (mods & Qt::ControlModifier) parts.append("Ctrl");
            if (mods & Qt::AltModifier) parts.append("Alt");
            if (mods & Qt::ShiftModifier) parts.append("Shift");
            if (mods & Qt::MetaModifier) parts.append("Meta");

            QString keyPart;
            if (key >= Qt::Key_Space && key <= Qt::Key_AsciiTilde)
                keyPart = QString(QChar(key));
            else if (key >= Qt::Key_F1 && key <= Qt::Key_F12)
                keyPart = QString("F%1").arg(key - Qt::Key_F1 + 1);
            else
                keyPart = QKeySequence(key).toString();

            if (!keyPart.isEmpty()) parts.append(keyPart);
            return parts.join("+");
        }

        bool eventFilter(QObject* obj, QEvent* ev) override {
            if (ev->type() == QEvent::KeyPress) {
                auto* kev = static_cast<QKeyEvent*>(ev);
                int k = kev->key();
                if (k == Qt::Key_Escape) {
                    captured.clear();
                    reject();
                    return true;
                }
                captured = formatKey(k, kev->modifiers());
                display->setText(captured);
                return true;
            }
            return QDialog::eventFilter(obj, ev);
        }
    };

    GrabDialog dlg(parent);
    dlg.installEventFilter(&dlg);
    dlg.exec();
    return dlg.captured;
}

bool ShortcutMapperDialog::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);
    Q_UNUSED(event);
    return false;
}
