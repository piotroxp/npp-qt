// ClipboardHistoryPanel.cpp - Clipboard history dock panel
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ClipboardHistoryPanel.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QSettings>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>

// Config key for persisting clipboard history
static const char* CLIPBOARD_HISTORY_KEY = "ClipboardHistory/items";
static const char* CLIPBOARD_HISTORY_GROUP = "Settings";

ClipboardHistoryPanel::ClipboardHistoryPanel(QWidget* parent)
    : QDockWidget(tr("Clipboard History"), parent)
    , _listWidget(nullptr)
    , _searchBox(nullptr)
    , _maxItems(50)
    , _skipNextClipboardEvent(false)
{
    setObjectName("ClipboardHistoryPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
                    Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    setupUi();

    // Connect to system clipboard changes
    QClipboard* clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ClipboardHistoryPanel::onClipboardChanged);

    // Load persisted history
    loadHistory();
}

ClipboardHistoryPanel::~ClipboardHistoryPanel() {
    saveHistory();
}

void ClipboardHistoryPanel::setupUi() {
    QWidget* content = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Search box
    _searchBox = new QLineEdit(content);
    _searchBox->setPlaceholderText(tr("Search clipboard..."));
    _searchBox->setClearButtonEnabled(true);
    mainLayout->addWidget(_searchBox);

    // List widget
    _listWidget = new QListWidget(content);
    _listWidget->setAlternatingRowColors(true);
    _listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _listWidget->setTextElideMode(Qt::ElideRight);
    _listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(_listWidget);

    // Button row
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(4);

    QPushButton* deleteBtn = new QPushButton(tr("Delete"), content);
    deleteBtn->setToolTip(tr("Delete selected item"));
    btnLayout->addWidget(deleteBtn);

    QPushButton* clearBtn = new QPushButton(tr("Clear All"), content);
    clearBtn->setToolTip(tr("Clear clipboard history"));
    btnLayout->addWidget(clearBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    setWidget(content);

    // Connections
    connect(_listWidget, &QListWidget::itemDoubleClicked,
            this, &ClipboardHistoryPanel::onItemDoubleClicked);
    connect(deleteBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onDeleteClicked);
    connect(clearBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onClearAllClicked);
    connect(_searchBox, &QLineEdit::textChanged,
            this, &ClipboardHistoryPanel::onSearchTextChanged);
}

void ClipboardHistoryPanel::addItem(const QString& text) {
    if (text.isEmpty()) return;

    deduplicateAndAdd(text);

    // Update list widget to reflect changes
    refreshListWidget();
}

void ClipboardHistoryPanel::deduplicateAndAdd(const QString& text) {
    // Remove existing occurrence of the same text
    _historyItems.removeAll(text);

    // Insert at the front (most recent first)
    _historyItems.prepend(text);

    // Trim to max size
    while (_historyItems.size() > _maxItems) {
        _historyItems.removeLast();
    }
}

void ClipboardHistoryPanel::refreshListWidget() {
    QString searchText = _searchBox->text().toLower();
    _listWidget->clear();

    for (const QString& item : std::as_const(_historyItems)) {
        // Apply search filter
        if (!searchText.isEmpty() && !item.toLower().contains(searchText)) {
            continue;
        }

        QListWidgetItem* listItem = new QListWidgetItem(_listWidget);
        // Show first line, truncated
        QString displayText = item.split('\n').first();
        if (displayText.length() > 100) {
            displayText.truncate(100);
            displayText += "…";
        }
        listItem->setText(displayText);
        listItem->setToolTip(item);  // Full text in tooltip
        listItem->setData(Qt::UserRole, item);  // Store full text
    }
}

void ClipboardHistoryPanel::onClipboardChanged() {
    // Skip if this change was triggered by us (paste action)
    if (_skipNextClipboardEvent) {
        _skipNextClipboardEvent = false;
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    QString text = clipboard->text(QClipboard::Clipboard);

    if (!text.isEmpty() && text != _lastClipboardText) {
        _lastClipboardText = text;
        addItem(text);
        saveHistory();
    }
}

void ClipboardHistoryPanel::onItemDoubleClicked(QListWidgetItem* item) {
    if (!item) return;

    QString fullText = item->data(Qt::UserRole).toString();
    if (fullText.isEmpty()) return;

    // Copy to clipboard
    _skipNextClipboardEvent = true;
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(fullText, QClipboard::Clipboard);
    _lastClipboardText = fullText;

    // If editor is available, paste into it
    if (_editor) {
        _editor->setSelection(-1, -1, -1, -1);  // Clear any selection
        _editor->paste();
        _editor->setFocus();
    }
}

void ClipboardHistoryPanel::onDeleteClicked() {
    QListWidgetItem* current = _listWidget->currentItem();
    if (!current) return;

    QString fullText = current->data(Qt::UserRole).toString();
    _historyItems.removeAll(fullText);
    delete current;
    saveHistory();
}

void ClipboardHistoryPanel::onClearAllClicked() {
    _historyItems.clear();
    _listWidget->clear();
    saveHistory();
}

void ClipboardHistoryPanel::onSearchTextChanged(const QString& text) {
    Q_UNUSED(text);
    refreshListWidget();
}

void ClipboardHistoryPanel::loadHistory() {
    // Load from QSettings using the same config dir as NppParameters
    QSettings settings;
    settings.beginGroup(CLIPBOARD_HISTORY_GROUP);
    QStringList loaded = settings.value(CLIPBOARD_HISTORY_KEY).toStringList();
    settings.endGroup();

    _historyItems = loaded;
    refreshListWidget();
}

void ClipboardHistoryPanel::saveHistory() const {
    // Save to QSettings
    QSettings settings;
    settings.beginGroup(CLIPBOARD_HISTORY_GROUP);
    settings.setValue(CLIPBOARD_HISTORY_KEY, _historyItems);
    settings.endGroup();
}

void ClipboardHistoryPanel::clearHistory() {
    _historyItems.clear();
    _listWidget->clear();
    saveHistory();
}
