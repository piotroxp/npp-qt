// ToolbarCustomizeDialog.cpp — see ToolbarCustomizeDialog.h.

#include "ToolbarCustomizeDialog.h"
#include "../ui/ToolBar.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

ToolbarCustomizeDialog::ToolbarCustomizeDialog(ToolBar* toolbar, QWidget* parent)
    : QDialog(parent), m_toolbar(toolbar)
{
    setWindowTitle(tr("Customize Toolbar"));
    setMinimumSize(640, 480);

    // ── Capture default order for Reset ─────────────────────────────────────
    // ToolBar::setupActions adds them in this order — that's what Reset
    // restores. We snapshot the current toolbar order, since by the time
    // Reset is invoked the user has potentially changed it.
    if (m_toolbar) {
        for (QAction* a : m_toolbar->actions()) {
            m_defaultVisible.append(a->data().toString());
        }
    }

    // ── Two list widgets: available (left) and visible (right) ─────────────
    m_available = new QListWidget(this);
    m_visible = new QListWidget(this);
    m_available->setSelectionMode(QAbstractItemView::SingleSelection);
    m_visible->setSelectionMode(QAbstractItemView::SingleSelection);
    m_visible->setDragDropMode(QAbstractItemView::InternalMove);

    QLabel* availLabel = new QLabel(tr("Available commands:"), this);
    QLabel* visLabel   = new QLabel(tr("Visible on toolbar (drag to reorder):"), this);

    // ── Middle column: Add / Remove / Up / Down / Reset ─────────────────────
    m_addBtn    = makeButton(tr("Add >>"));
    m_removeBtn = makeButton(tr("<< Remove"));
    m_upBtn     = makeButton(tr("Move Up"));
    m_downBtn   = makeButton(tr("Move Down"));
    m_resetBtn  = makeButton(tr("Reset to Defaults"));

    auto* middleColumn = new QVBoxLayout;
    middleColumn->addStretch();
    middleColumn->addWidget(m_addBtn);
    middleColumn->addWidget(m_removeBtn);
    middleColumn->addSpacing(12);
    middleColumn->addWidget(m_upBtn);
    middleColumn->addWidget(m_downBtn);
    middleColumn->addSpacing(12);
    middleColumn->addWidget(m_resetBtn);
    middleColumn->addStretch();

    // ── Layout ──────────────────────────────────────────────────────────────
    auto* listsRow = new QHBoxLayout;
    auto* leftCol  = new QVBoxLayout;
    leftCol->addWidget(availLabel);
    leftCol->addWidget(m_available, 1);
    auto* rightCol = new QVBoxLayout;
    rightCol->addWidget(visLabel);
    rightCol->addWidget(m_visible, 1);
    listsRow->addLayout(leftCol, 1);
    listsRow->addLayout(middleColumn);
    listsRow->addLayout(rightCol, 1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* root = new QVBoxLayout(this);
    root->addLayout(listsRow, 1);
    root->addWidget(buttons);

    // ── Signal wiring ───────────────────────────────────────────────────────
    connect(m_addBtn,    &QPushButton::clicked, this, &ToolbarCustomizeDialog::onAdd);
    connect(m_removeBtn, &QPushButton::clicked, this, &ToolbarCustomizeDialog::onRemove);
    connect(m_upBtn,     &QPushButton::clicked, this, &ToolbarCustomizeDialog::onMoveUp);
    connect(m_downBtn,   &QPushButton::clicked, this, &ToolbarCustomizeDialog::onMoveDown);
    connect(m_resetBtn,  &QPushButton::clicked, this, &ToolbarCustomizeDialog::onReset);
    connect(m_visible,   &QListWidget::itemSelectionChanged,
            this, &ToolbarCustomizeDialog::onSelectionChanged);
    connect(m_available, &QListWidget::itemSelectionChanged,
            this, &ToolbarCustomizeDialog::onSelectionChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        applyToToolbar();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    rebuildAvailableList();
    rebuildVisibleList();
    onSelectionChanged();
}

ToolbarCustomizeDialog::~ToolbarCustomizeDialog() = default;

QPushButton* ToolbarCustomizeDialog::makeButton(const QString& label) {
    auto* b = new QPushButton(label, this);
    b->setMinimumWidth(140);
    return b;
}

QString ToolbarCustomizeDialog::actionDisplayName(const QString& command) const {
    if (!m_toolbar) return command;
    if (QAction* a = m_toolbar->actionForCommand(command)) {
        if (!a->text().isEmpty()) return a->text();
    }
    return command;
}

void ToolbarCustomizeDialog::rebuildAvailableList() {
    m_available->clear();
    if (!m_toolbar) return;

    // Collect the commands currently in m_visible so we don't show them
    // as "available" too.
    QStringList visibleNow;
    for (int i = 0; i < m_visible->count(); ++i) {
        visibleNow << m_visible->item(i)->data(Qt::UserRole).toString();
    }

    // Iterate m_actions in stable order (ToolBar::setupActions added them
    // in a logical sequence — File, Edit, Macro, View, Tools). QMap
    // already sorts by key, which gives an alphabetical-by-command view;
    // we instead iterate in the order they were added by walking the
    // toolbar's actions first (so we keep file.new before file.open),
    // then any remaining keys.
    QStringList seen;
    for (QAction* a : m_toolbar->actions()) {
        const QString cmd = a->data().toString();
        if (cmd.isEmpty() || visibleNow.contains(cmd)) continue;
        auto* item = new QListWidgetItem(actionDisplayName(cmd), m_available);
        item->setData(Qt::UserRole, cmd);
        seen << cmd;
    }
    // Pick up any actions not on the toolbar (e.g. commands registered
    // via setActionEnabled but never added).
    // ToolBar exposes the full pool via actionForCommand(); we already
    // covered those by walking actions(). If a future revision adds an
    // `allCommands()` accessor, this is the place to use it.
}

void ToolbarCustomizeDialog::rebuildVisibleList() {
    m_visible->clear();
    if (!m_toolbar) return;
    for (QAction* a : m_toolbar->actions()) {
        const QString cmd = a->data().toString();
        if (cmd.isEmpty()) continue;
        auto* item = new QListWidgetItem(actionDisplayName(cmd), m_visible);
        item->setData(Qt::UserRole, cmd);
    }
}

void ToolbarCustomizeDialog::onAdd() {
    QListWidgetItem* item = m_available->currentItem();
    if (!item) return;
    QString cmd = item->data(Qt::UserRole).toString();
    auto* newItem = new QListWidgetItem(actionDisplayName(cmd), m_visible);
    newItem->setData(Qt::UserRole, cmd);
    delete m_available->takeItem(m_available->row(item));
    m_visible->setCurrentItem(newItem);
    onSelectionChanged();
}

void ToolbarCustomizeDialog::onRemove() {
    QListWidgetItem* item = m_visible->currentItem();
    if (!item) return;
    QString cmd = item->data(Qt::UserRole).toString();
    auto* newItem = new QListWidgetItem(actionDisplayName(cmd), m_available);
    newItem->setData(Qt::UserRole, cmd);
    delete m_visible->takeItem(m_visible->row(item));
    m_available->setCurrentItem(newItem);
    onSelectionChanged();
}

void ToolbarCustomizeDialog::onMoveUp() {
    int row = m_visible->currentRow();
    if (row <= 0) return;
    QListWidgetItem* item = m_visible->takeItem(row);
    m_visible->insertItem(row - 1, item);
    m_visible->setCurrentRow(row - 1);
}

void ToolbarCustomizeDialog::onMoveDown() {
    int row = m_visible->currentRow();
    if (row < 0 || row >= m_visible->count() - 1) return;
    QListWidgetItem* item = m_visible->takeItem(row);
    m_visible->insertItem(row + 1, item);
    m_visible->setCurrentRow(row + 1);
}

void ToolbarCustomizeDialog::onReset() {
    m_visible->clear();
    for (const QString& cmd : m_defaultVisible) {
        auto* item = new QListWidgetItem(actionDisplayName(cmd), m_visible);
        item->setData(Qt::UserRole, cmd);
    }
    rebuildAvailableList();
    onSelectionChanged();
}

void ToolbarCustomizeDialog::onSelectionChanged() {
    bool hasAvailSel   = m_available->currentItem() != nullptr;
    bool hasVisibleSel = m_visible->currentItem() != nullptr;
    m_addBtn->setEnabled(hasAvailSel);
    m_removeBtn->setEnabled(hasVisibleSel);
    m_upBtn->setEnabled(hasVisibleSel && m_visible->currentRow() > 0);
    m_downBtn->setEnabled(hasVisibleSel
                          && m_visible->currentRow() < m_visible->count() - 1);
    m_resetBtn->setEnabled(!m_defaultVisible.isEmpty());
}

void ToolbarCustomizeDialog::applyToToolbar() {
    if (!m_toolbar) return;

    // Build the desired order from m_visible.
    QStringList desired;
    for (int i = 0; i < m_visible->count(); ++i) {
        desired << m_visible->item(i)->data(Qt::UserRole).toString();
    }

    // First remove all current actions (in reverse to avoid index churn).
    QList<QAction*> current = m_toolbar->actions();
    for (QAction* a : current) {
        m_toolbar->removeAction(a);
    }

    // Re-add in the desired order.
    for (const QString& cmd : desired) {
        if (QAction* a = m_toolbar->actionForCommand(cmd)) {
            m_toolbar->addAction(a);
        }
    }
}