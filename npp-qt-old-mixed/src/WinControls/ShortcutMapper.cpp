// Semantic Lift: Win32 ShortcutMapper → Qt6 implementation
// Original: PowerEditor/src/WinControls/shortcut/shortcut.cpp (1485 lines)
// Target: npp-qt/src/WinControls/ShortcutMapper.cpp

#include "ShortcutMapper.h"
#include "StaticDialog.h"
#include "DockingWnd.h"
#include <QKeySequence>
#include <QApplication>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QScrollArea>
#include <QMessageBox>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionButton>
#include <functional>

// KeySequenceEdit implementation (class declared in ShortcutMapper.h)
KeySequenceEdit::KeySequenceEdit(QWidget* parent)
    : QKeySequenceEdit(parent) {}

ShortcutKeyCombo KeySequenceEdit::keyCombo() const {
    ShortcutKeyCombo kc;
    QKeySequence seq = keySequence();
    int mods = seq.isEmpty() ? 0 : seq[0];
    kc._isCtrl = (mods & Qt::ControlModifier);
    kc._isAlt  = (mods & Qt::AltModifier);
    kc._isShift = (mods & Qt::ShiftModifier);
    int key = seq.isEmpty() ? 0 : (mods & ~Qt::KeyboardModifierMask);
    if (key == Qt::Key_Up) kc._key = '↑';
    else if (key == Qt::Key_Down) kc._key = '↓';
    else if (key == Qt::Key_Left) kc._key = '←';
    else if (key == Qt::Key_Right) kc._key = '→';
    else kc._key = static_cast<unsigned char>(key);
    return kc;
}

void KeySequenceEdit::setKeyCombo(const ShortcutKeyCombo& kc) {
    if (!kc.isEnabled()) { clear(); return; }
    int mods = 0;
    if (kc._isCtrl) mods |= Qt::ControlModifier;
    if (kc._isAlt)  mods |= Qt::AltModifier;
    if (kc._isShift) mods |= Qt::ShiftModifier;
    mods |= kc._key;
    setKeySequence(QKeySequence(mods));
}

QWidget* ShortcutTableDelegate::createEditor(QWidget* parent,
                                              const QStyleOptionViewItem&,
                                              const QModelIndex& index) const
{
    if (index.column() != 1) return nullptr; // Only "Shortcut" column editable
    auto* editor = new KeySequenceEdit(parent);
    return editor;
}

void ShortcutTableDelegate::setEditorData(QWidget* editor,
                                            const QModelIndex& index) const
{
    auto* edit = qobject_cast<KeySequenceEdit*>(editor);
    if (!edit) return;
    ShortcutItem item = index.data(Qt::UserRole).value<ShortcutItem>();
    edit->setKeyCombo(item._keyCombo);
}

void ShortcutTableDelegate::setModelData(QWidget* editor,
                                          QAbstractItemModel* model,
                                          const QModelIndex& index) const
{
    auto* edit = qobject_cast<KeySequenceEdit*>(editor);
    if (!edit) return;
    ShortcutItem item = index.data(Qt::UserRole).value<ShortcutItem>();
    item._keyCombo = edit->keyCombo();
    model->setData(index, QVariant::fromValue(item), Qt::UserRole);
}

void ShortcutTableDelegate::updateEditorGeometry(QWidget* editor,
                                                  const QStyleOptionViewItem& option,
                                                  const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}

// =============================================================================
// ShortcutMapper — constructor
// =============================================================================

ShortcutMapper::ShortcutMapper()
    : StaticDialog()
{
    setWindowTitle(QStringLiteral("Shortcut Mapper"));
    setMinimumSize(700, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // Tab widget
    _tabWidget = new QTabWidget(this);
    _tabWidget->addTab(new QWidget(), QStringLiteral("Commands"));
    _tabWidget->addTab(new QWidget(), QStringLiteral("Scintilla"));
    _tabWidget->addTab(new QWidget(), QStringLiteral("Macros"));

    mainLayout->addWidget(_tabWidget, 1);

    // Button row
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton* clearBtn = new QPushButton(QStringLiteral("Clear"), this);
    connect(clearBtn, &QPushButton::clicked, this, [this]() {
        int row = _cmdTable->currentRow();
        if (row >= 0) clearShortcut(row);
    });

    QPushButton* resetBtn = new QPushButton(QStringLiteral("Reset All"), this);
    connect(resetBtn, &QPushButton::clicked, this, &ShortcutMapper::resetAllShortcuts);

    QPushButton* saveBtn = new QPushButton(QStringLiteral("Save"), this);
    connect(saveBtn, &QPushButton::clicked, this, &ShortcutMapper::saveShortcuts);

    QPushButton* closeBtn = new QPushButton(QStringLiteral("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, [this]() {
        hide();
    });

    btnRow->addWidget(clearBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(resetBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(saveBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(closeBtn);

    mainLayout->addLayout(btnRow);
    setLayout(mainLayout);

    _delegate = new ShortcutTableDelegate(this);
    initTables();
}

// =============================================================================
// initTables — create and configure all three tables
// Mirrors Win32 ShortcutMapper::initTabs().
// =============================================================================

void ShortcutMapper::initTables()
{
    // === Command Table ===
    _cmdTable = new QTableWidget(_tabWidget->widget(0));
    _cmdTable->setColumnCount(2);
    _cmdTable->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Shortcut")});
    _cmdTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _cmdTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    _cmdTable->setColumnWidth(1, 180);
    _cmdTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _cmdTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _cmdTable->setAlternatingRowColors(true);
    _cmdTable->setItemDelegate(_delegate);
    _cmdTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    _cmdTable->verticalHeader()->setVisible(false);
    QVBoxLayout* cmdLayout = new QVBoxLayout(_tabWidget->widget(0));
    cmdLayout->setContentsMargins(0, 0, 0, 0);
    cmdLayout->addWidget(_cmdTable);

    // === Scintilla Table ===
    _sciTable = new QTableWidget(_tabWidget->widget(1));
    _sciTable->setColumnCount(3);
    _sciTable->setHorizontalHeaderLabels({QStringLiteral("Action"), QStringLiteral("Category"), QStringLiteral("Shortcut")});
    _sciTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _sciTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    _sciTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    _sciTable->setColumnWidth(1, 150);
    _sciTable->setColumnWidth(2, 180);
    _sciTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _sciTable->setAlternatingRowColors(true);
    _sciTable->setItemDelegate(_delegate);
    QVBoxLayout* sciLayout = new QVBoxLayout(_tabWidget->widget(1));
    sciLayout->setContentsMargins(0, 0, 0, 0);
    sciLayout->addWidget(_sciTable);

    // === Macro Table ===
    _macroTable = new QTableWidget(_tabWidget->widget(2));
    _macroTable->setColumnCount(2);
    _macroTable->setHorizontalHeaderLabels({QStringLiteral("Macro Name"), QStringLiteral("Shortcut")});
    _macroTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _macroTable->setColumnWidth(1, 180);
    _macroTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _macroTable->setAlternatingRowColors(true);
    _macroTable->setItemDelegate(_delegate);
    QVBoxLayout* macroLayout = new QVBoxLayout(_tabWidget->widget(2));
    macroLayout->setContentsMargins(0, 0, 0, 0);
    macroLayout->addWidget(_macroTable);

    populateCommandTable();
    populateScintillaTable();
    populateMacroTable();
}

// =============================================================================
// populateCommandTable — load command shortcuts into the table
// Mirrors Win32 ShortcutMapper::initCommand().
// =============================================================================

void ShortcutMapper::populateCommandTable()
{
    _cmdTable->setRowCount(0);
    _shortcuts.clear();

    // Sample command entries — full implementation requires NppCommands lift
    static const struct { const char* name; int id; } commands[] = {
        {"New", 40001}, {"Open", 40002}, {"Save", 40003},
        {"Save As...", 40004}, {"Close", 40005},
        {"Cut", 40010}, {"Copy", 40011}, {"Paste", 40012},
        {"Find", 40020}, {"Replace", 40021}, {"Find Next", 40022},
        {"Go To...", 40030}, {"Select All", 40040},
    };

    _cmdTable->setRowCount(sizeof(commands) / sizeof(commands[0]));
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i)
    {
        ShortcutItem item;
        item._name = QString::fromUtf8(commands[i].name);
        item._id = commands[i].id;
        item._type = 0;
        item._keyCombo._key = 0; // Unbound by default

        QTableWidgetItem* nameItem = new QTableWidgetItem(item._name);
        nameItem->setData(Qt::UserRole, QVariant::fromValue(item));
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        _cmdTable->setItem(static_cast<int>(i), 0, nameItem);

        QTableWidgetItem* shortcutItem = new QTableWidgetItem(item.keyString());
        shortcutItem->setData(Qt::UserRole, QVariant::fromValue(item));
        _cmdTable->setItem(static_cast<int>(i), 1, shortcutItem);
    }
    _shortcuts.resize(sizeof(commands) / sizeof(commands[0]));
}

void ShortcutMapper::populateScintillaTable()
{
    _sciTable->setRowCount(0);
    // Scintilla actions — populated from Scintilla key map
    // Requires ScintillaComponent lift
}

void ShortcutMapper::populateMacroTable()
{
    _macroTable->setRowCount(0);
    // Macro shortcuts — populated from saved macros
    // Requires MacroManager lift
}

// =============================================================================
// modifyShortcut — edit a shortcut (dialog-based, mirrors Win32 Modify)
// =============================================================================

void ShortcutMapper::modifyShortcut(int row)
{
    QTableWidget* table = currentTable();
    if (!table || row < 0 || row >= table->rowCount()) return;
    table->editItem(table->item(row, 1));
}

void ShortcutMapper::clearShortcut(int row)
{
    QTableWidget* table = currentTable();
    if (!table || row < 0 || row >= table->rowCount()) return;

    ShortcutItem item = table->item(row, 1)->data(Qt::UserRole).value<ShortcutItem>();
    item._keyCombo._key = 0;
    item._keyCombo._isCtrl = false;
    item._keyCombo._isAlt = false;
    item._keyCombo._isShift = false;

    table->item(row, 1)->setText(item.keyString());
    table->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(item));
}

// =============================================================================
// saveShortcuts — persist shortcut changes
// Mirrors Win32 ShortcutMapper::saveShortcuts().
// =============================================================================

void ShortcutMapper::saveShortcuts()
{
    // Collect all shortcut items and emit save signal
    std::vector<ShortcutItem> allItems;
    for (int r = 0; r < _cmdTable->rowCount(); ++r)
    {
        QTableWidgetItem* it = _cmdTable->item(r, 1);
        if (it)
        {
            ShortcutItem item = it->data(Qt::UserRole).value<ShortcutItem>();
            allItems.push_back(item);
        }
    }
    Q_UNUSED(allItems);
    QMessageBox::information(this, QStringLiteral("Shortcut Mapper"),
        QStringLiteral("Shortcuts saved. (Persistence requires Parameters lift.)"));
}

void ShortcutMapper::resetAllShortcuts()
{
    auto reply = QMessageBox::question(this, QStringLiteral("Reset All Shortcuts"),
        QStringLiteral("Reset all shortcuts to defaults? This cannot be undone."),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        populateCommandTable();
        populateScintillaTable();
        populateMacroTable();
    }
}

// =============================================================================
// switchMapperTab — switch to tab (mirrors Win32 ShortcutMapper::switchMapperTab)
// =============================================================================

void ShortcutMapper::switchMapperTab(int categoryIndex)
{
    _currentCategory = categoryIndex;
    _tabWidget->setCurrentIndex(categoryIndex);
}

void ShortcutMapper::goToSchwerCategory(int categoryIndex)
{
    switchMapperTab(categoryIndex);
}

// =============================================================================
// updateShortcut — update a single shortcut (mirrors Win32 ShortcutMapper::update)
// =============================================================================

void ShortcutMapper::updateShortcut(const ShortcutItem& item)
{
    for (int r = 0; r < _cmdTable->rowCount(); ++r)
    {
        QTableWidgetItem* it = _cmdTable->item(r, 1);
        if (it)
        {
            ShortcutItem cur = it->data(Qt::UserRole).value<ShortcutItem>();
            if (cur._id == item._id)
            {
                it->setText(item.keyString());
                it->setData(Qt::UserRole, QVariant::fromValue(item));
                return;
            }
        }
    }
}

// =============================================================================
// run_dlgProc — message dispatcher
// Mirrors Win32 ShortcutMapper::run_dlgProc.
// =============================================================================

intptr_t ShortcutMapper::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        switchMapperTab(0);
        return TRUE;

    case WM_DESTROY:
        return TRUE;

    default:
        break;
    }
    return 0;
}

QTableWidget* ShortcutMapper::currentTable() const
{
    switch (_tabWidget->currentIndex())
    {
    case 0: return _cmdTable;
    case 1: return _sciTable;
    case 2: return _macroTable;
    default: return _cmdTable;
    }
}

