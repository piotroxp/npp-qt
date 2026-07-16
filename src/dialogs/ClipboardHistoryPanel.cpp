// ClipboardHistoryPanel.cpp - Clipboard history dock panel
// Copyright (C) 2026 Agent Army
// GPL v3
//
// Enhanced clipboard history with:
// - Persistent storage via QSettings
// - Text and image support
// - Pin/unpin items
// - Search/filter
// - Multi-select and bulk operations
// - Timestamps and character counts
// - Drag to editor
// - Keyboard navigation (Ctrl+Shift+V shortcut, up/down, Enter)
// - Auto-cleanup of duplicates
// - "Paste and go" for commands
// - Dock panel behavior

#include "ClipboardHistoryPanel.h"
#include "../editor/ScintillaEditor.h"
#include "../core/ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QMimeData>
#include <QDrag>
#include <QKeySequence>
#include <QDebug>
#include <QDateTime>
#include <QUuid>

// Config key for persisting clipboard history
static const char* CLIPBOARD_HISTORY_KEY = "ClipboardHistory/items";
static const char* CLIPBOARD_HISTORY_GROUP = "Settings";

// ─── HistoryItem helpers ────────────────────────────────────────────────────────

QString ClipboardHistoryPanel::HistoryItem::displayText(int maxLen) const
{
    if (text.isEmpty()) return QString();

    // Show first line, truncated to maxLen.
    QString firstLine = text.split('\n').first();
    if (firstLine.length() > maxLen) {
        firstLine.truncate(maxLen);
        firstLine += "…";
    }
    return firstLine;
}

QString ClipboardHistoryPanel::HistoryItem::tooltipText() const
{
    if (isImage) {
        return QString("Image: %1×%2\nCaptured: %3\n%4 chars")
            .arg(image.width()).arg(image.height())
            .arg(timestamp.toString(Qt::ISODate))
            .arg(charCount);
    }
    return QString("Captured: %1\n%2 chars\n\n%3")
        .arg(timestamp.toString(Qt::ISODate))
        .arg(charCount)
        .arg(text);
}

// ─── Constructor / Destructor ──────────────────────────────────────────────────

ClipboardHistoryPanel::ClipboardHistoryPanel(QWidget* parent)
    : QDockWidget(tr("Clipboard History"), parent)
    , _listWidget(nullptr)
    , _searchBox(nullptr)
    , _statusLabel(nullptr)
    , _maxItems(50)
    , _skipNextClipboardEvent(false)
    , _autoCleanupDuplicates(true)
{
    setObjectName("ClipboardHistoryPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
                    Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    // Register Ctrl+Shift+V as a global shortcut to focus this panel.
    // Actual Ctrl+Shift+V shortcut is handled by the MainWindow that owns this panel.
    // Here we just set a accessible name and focus proxy.
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Clipboard History"));

    setupUi();
    setupContextMenu();

    // Connect to system clipboard changes.
    QClipboard* clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged,
            this, &ClipboardHistoryPanel::onClipboardChanged);

    // Listen for image changes too.
    connect(clipboard, &QClipboard::selectionChanged,
            this, &ClipboardHistoryPanel::onClipboardChanged);

    // Load persisted history.
    loadHistory();
}

ClipboardHistoryPanel::~ClipboardHistoryPanel() {
    saveHistory();
}

// ─── UI setup ─────────────────────────────────────────────────────────────────

void ClipboardHistoryPanel::setupUi()
{
    QWidget* content = new QWidget(this);
    content->setObjectName("ClipboardHistoryContent");
    QVBoxLayout* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // ── Search/filter box ──────────────────────────────────────────────────
    _searchBox = new QLineEdit(content);
    _searchBox->setObjectName("clipboardSearchBox");
    _searchBox->setPlaceholderText(tr("Search clipboard history…"));
    _searchBox->setClearButtonEnabled(true);
    _searchBox->setToolTip(tr("Filter clipboard history by text content"));
    QIcon searchIcon;
    _searchBox->addAction(QIcon::fromTheme("edit-find"), QLineEdit::LeadingPosition);
    mainLayout->addWidget(_searchBox);

    // ── List widget ──────────────────────────────────────────────────────
    _listWidget = new QListWidget(content);
    _listWidget->setObjectName("clipboardListWidget");
    _listWidget->setAlternatingRowColors(true);
    _listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _listWidget->setTextElideMode(Qt::ElideRight);
    _listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    _listWidget->setDragEnabled(true);
    _listWidget->setAcceptDrops(true);
    _listWidget->setDropIndicatorShown(true);
    _listWidget->setToolTip(tr("Double-click to paste into editor\n"
                                "Drag to editor\n"
                                "Ctrl+Shift+V to open"));
    mainLayout->addWidget(_listWidget);

    // ── Status label ───────────────────────────────────────────────────────
    _statusLabel = new QLabel(content);
    _statusLabel->setObjectName("clipboardStatusLabel");
    _statusLabel->setText(tr("0 items"));
    _statusLabel->setStyleSheet("QLabel { color: palette(mid); font-size: 11px; }");
    mainLayout->addWidget(_statusLabel);

    // ── Button row ────────────────────────────────────────────────────────
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(4);

    QPushButton* copyBtn = new QPushButton(tr("Copy"), content);
    copyBtn->setObjectName("clipboardCopyBtn");
    copyBtn->setToolTip(tr("Copy selected item back to clipboard"));
    btnLayout->addWidget(copyBtn);

    QPushButton* pinBtn = new QPushButton(tr("Pin"), content);
    pinBtn->setObjectName("clipboardPinBtn");
    pinBtn->setToolTip(tr("Pin/unpin selected item (pinned items stay at top)"));
    btnLayout->addWidget(pinBtn);

    QPushButton* expandBtn = new QPushButton(tr("Expand"), content);
    expandBtn->setObjectName("clipboardExpandBtn");
    expandBtn->setToolTip(tr("Show full text of selected item"));
    btnLayout->addWidget(expandBtn);

    QPushButton* deleteBtn = new QPushButton(tr("Delete"), content);
    deleteBtn->setObjectName("clipboardDeleteBtn");
    deleteBtn->setToolTip(tr("Delete selected item(s)"));
    btnLayout->addWidget(deleteBtn);

    QPushButton* clearBtn = new QPushButton(tr("Clear All"), content);
    clearBtn->setObjectName("clipboardClearBtn");
    clearBtn->setToolTip(tr("Clear all clipboard history"));
    btnLayout->addWidget(clearBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    setWidget(content);

    // ── Connections ───────────────────────────────────────────────────────
    connect(_listWidget, &QListWidget::itemDoubleClicked,
            this, &ClipboardHistoryPanel::onItemDoubleClicked);
    connect(_listWidget, &QListWidget::itemClicked,
            this, &ClipboardHistoryPanel::onItemClicked);
    connect(copyBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onCopyClicked);
    connect(pinBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onPinToggled);
    connect(expandBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::showFullText);
    connect(deleteBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onDeleteClicked);
    connect(clearBtn, &QPushButton::clicked,
            this, &ClipboardHistoryPanel::onClearAllClicked);
    connect(_searchBox, &QLineEdit::textChanged,
            this, &ClipboardHistoryPanel::onSearchTextChanged);
}

void ClipboardHistoryPanel::setupContextMenu()
{
    connect(_listWidget, &QListWidget::customContextMenuRequested,
            [this](const QPoint& pos) {
                Q_UNUSED(pos);
                QMenu menu(this);

                QAction* copyAct = menu.addAction(tr("Copy to Clipboard"));
                copyAct->setShortcut(QKeySequence::Copy);
                connect(copyAct, &QAction::triggered, this, &ClipboardHistoryPanel::onCopyClicked);

                QAction* pasteAct = menu.addAction(tr("Paste into Editor"));
                pasteAct->setShortcut(Qt::Key_Return);
                connect(pasteAct, &QAction::triggered, this, [this]() {
                    QListWidgetItem* item = _listWidget->currentItem();
                    onItemDoubleClicked(item);
                });

                menu.addSeparator();

                QAction* pinAct = menu.addAction(tr("Pin / Unpin"));
                connect(pinAct, &QAction::triggered, this, &ClipboardHistoryPanel::onPinToggled);

                QAction* expandAct = menu.addAction(tr("Show Full Text…"));
                connect(expandAct, &QAction::triggered, this, &ClipboardHistoryPanel::showFullText);

                menu.addSeparator();

                QAction* delAct = menu.addAction(tr("Delete"));
                delAct->setShortcut(QKeySequence::Delete);
                connect(delAct, &QAction::triggered, this, &ClipboardHistoryPanel::onDeleteClicked);

                QAction* clearAct = menu.addAction(tr("Clear All History"));
                clearAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Delete));
                connect(clearAct, &QAction::triggered, this, &ClipboardHistoryPanel::onClearAllClicked);

                menu.exec(QCursor::pos());
            });
}

// ─── Add items ────────────────────────────────────────────────────────────────

void ClipboardHistoryPanel::addItem(const QString& text)
{
    if (text.isEmpty()) return;
    HistoryItem item;
    item.id = QUuid::createUuid();
    item.text = text;
    item.timestamp = QDateTime::currentDateTime();
    item.charCount = text.length();
    item.isPinned = false;
    item.isImage = false;
    addItemInternal(item);
}

void ClipboardHistoryPanel::addItem(const QImage& image)
{
    if (image.isNull()) return;
    HistoryItem item;
    item.id = QUuid::createUuid();
    item.image = image;
    item.timestamp = QDateTime::currentDateTime();
    item.charCount = image.width() * image.height(); // rough metric
    item.isPinned = false;
    item.isImage = true;
    addItemInternal(item);
}

void ClipboardHistoryPanel::addItemInternal(const HistoryItem& item)
{
    // Auto-cleanup duplicates for text items.
    if (!item.isImage && _autoCleanupDuplicates) {
        int existingIdx = findItemIndex(item.text);
        if (existingIdx >= 0) {
            // Move existing item to top instead of adding duplicate.
            _historyItems.move(existingIdx, 0);
            _historyItems[0].timestamp = QDateTime::currentDateTime();
            refreshListWidget();
            saveHistory();
            return;
        }
    }

    // Insert at front (most recent).
    _historyItems.prepend(item);

    // Trim to max size (but never remove pinned items from the list).
    int removedCount = 0;
    while (_historyItems.size() > _maxItems) {
        // Find the last non-pinned item and remove it.
        bool found = false;
        for (int i = _historyItems.size() - 1; i >= 0; --i) {
            if (!_historyItems.at(i).isPinned) {
                _historyItems.removeAt(i);
                found = true;
                ++removedCount;
                break;
            }
        }
        if (!found) break; // All items are pinned
    }

    refreshListWidget();
    saveHistory();
    emit historyChanged();
}

void ClipboardHistoryPanel::deduplicateAndAdd(const QString& text)
{
    addItem(text);
}

// ─── List refresh ───────────────────────────────────────────────────────────────

void ClipboardHistoryPanel::refreshListWidget()
{
    QString searchText = _searchBox->text().toLower();
    _listWidget->clear();

    // Sort: pinned items first, then by timestamp (most recent first).
    QList<HistoryItem> sorted = _historyItems;
    std::sort(sorted.begin(), sorted.end(),
              [](const HistoryItem& a, const HistoryItem& b) {
                  if (a.isPinned != b.isPinned) return a.isPinned;
                  return a.timestamp > b.timestamp;
              });

    for (const HistoryItem& item : sorted) {
        // Apply search filter.
        if (!searchText.isEmpty() && !item.isImage) {
            if (!item.text.toLower().contains(searchText)) continue;
        }

        QListWidgetItem* listItem = new QListWidgetItem(_listWidget);
        QString display = item.displayText();
        if (item.isPinned)
            display.prepend("📌 ");
        listItem->setText(display);
        listItem->setToolTip(item.tooltipText());
        listItem->setData(Qt::UserRole, QVariant::fromValue(item));
        listItem->setData(Qt::UserRole + 1, item.id.toString());

        // Set icon for pinned items.
        if (item.isPinned) {
            listItem->setBackground(QColor(0x3A, 0x5F, 0x8B).lighter(130));
        }

        // Show image thumbnail for image entries.
        if (item.isImage && !item.image.isNull()) {
            QIcon imgIcon(QPixmap::fromImage(item.image.scaled(64, 64, Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation)));
            listItem->setIcon(imgIcon);
        }
    }

    updateStatusLabel();
}

int ClipboardHistoryPanel::findItemIndex(const QString& text) const
{
    for (int i = 0; i < _historyItems.size(); ++i) {
        if (_historyItems.at(i).text == text) return i;
    }
    return -1;
}

void ClipboardHistoryPanel::updateStatusLabel()
{
    int total = _historyItems.size();
    int pinned = 0;
    for (const auto& item : _historyItems) {
        if (item.isPinned) ++pinned;
    }
    if (pinned > 0) {
        _statusLabel->setText(tr("%1 items (%2 pinned)").arg(total).arg(pinned));
    } else {
        _statusLabel->setText(tr("%1 items").arg(total));
    }
}

// ─── Clipboard change handling ─────────────────────────────────────────────────

void ClipboardHistoryPanel::onClipboardChanged()
{
    // Skip if this change was triggered by us (paste action).
    if (_skipNextClipboardEvent) {
        _skipNextClipboardEvent = false;
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();

    // Check for image first.
    if (!clipboard->image().isNull()) {
        QImage image = clipboard->image();
        if (!image.isNull() && image != _lastClipboardImage) {
            _lastClipboardImage = image;
            addItem(image);
            saveHistory();
            return;
        }
    }

    // Check for text.
    QString text = clipboard->text(QClipboard::Clipboard);
    if (!text.isEmpty() && text != _lastClipboardText) {
        _lastClipboardText = text;
        addItem(text);
        saveHistory();
    }
}

// ─── Interaction ───────────────────────────────────────────────────────────────

void ClipboardHistoryPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;

    HistoryItem histItem = item->data(Qt::UserRole).value<HistoryItem>();
    if (histItem.isImage) {
        // For images: copy to clipboard.
        QImage img = histItem.image;
        if (!img.isNull()) {
            _skipNextClipboardEvent = true;
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setImage(img, QClipboard::Clipboard);
            _lastClipboardImage = img;
        }
        return;
    }

    const QString& fullText = histItem.text;
    if (fullText.isEmpty()) return;

    // Copy to clipboard first.
    _skipNextClipboardEvent = true;
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(fullText, QClipboard::Clipboard);
    _lastClipboardText = fullText;

    emit itemPasted(fullText);

    // If editor is available, paste into it.
    if (_editor) {
        _editor->setSelection(-1, -1, -1, -1);
        _editor->paste();
        _editor->setFocus();
    }
}

void ClipboardHistoryPanel::onItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    // Single click selects; double click pastes.
}

void ClipboardHistoryPanel::onCopyClicked()
{
    QList<QListWidgetItem*> selected = _listWidget->selectedItems();
    if (selected.isEmpty()) return;

    // Copy the most recently selected item.
    QListWidgetItem* item = selected.last();
    HistoryItem histItem = item->data(Qt::UserRole).value<HistoryItem>();

    if (histItem.isImage) {
        if (!histItem.image.isNull()) {
            _skipNextClipboardEvent = true;
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setImage(histItem.image, QClipboard::Clipboard);
            _lastClipboardImage = histItem.image;
        }
    } else {
        _skipNextClipboardEvent = true;
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(histItem.text, QClipboard::Clipboard);
        _lastClipboardText = histItem.text;
    }
}

void ClipboardHistoryPanel::onDeleteClicked()
{
    QList<QListWidgetItem*> selected = _listWidget->selectedItems();
    if (selected.isEmpty()) return;

    // Remove items from history (in reverse order to preserve indices).
    QList<QUuid> idsToDelete;
    for (QListWidgetItem* item : selected) {
        idsToDelete.append(QUuid(item->data(Qt::UserRole + 1).toString()));
    }

    for (const QUuid& id : idsToDelete) {
        for (int i = 0; i < _historyItems.size(); ++i) {
            if (_historyItems.at(i).id == id) {
                _historyItems.removeAt(i);
                break;
            }
        }
    }

    refreshListWidget();
    saveHistory();
    emit historyChanged();
}

void ClipboardHistoryPanel::onClearAllClicked()
{
    // Clear all non-pinned items.
    for (int i = _historyItems.size() - 1; i >= 0; --i) {
        if (!_historyItems.at(i).isPinned) {
            _historyItems.removeAt(i);
        }
    }
    refreshListWidget();
    saveHistory();
    emit historyChanged();
}

void ClipboardHistoryPanel::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    refreshListWidget();
}

void ClipboardHistoryPanel::togglePinSelected()
{
    QListWidgetItem* current = _listWidget->currentItem();
    if (!current) return;

    HistoryItem item = current->data(Qt::UserRole).value<HistoryItem>();
    QUuid id = QUuid(current->data(Qt::UserRole + 1).toString());

    for (int i = 0; i < _historyItems.size(); ++i) {
        if (_historyItems.at(i).id == id) {
            _historyItems[i].isPinned = !_historyItems[i].isPinned;
            break;
        }
    }

    refreshListWidget();
    saveHistory();
    emit historyChanged();
}

void ClipboardHistoryPanel::onPinToggled()
{
    togglePinSelected();
}

void ClipboardHistoryPanel::showFullText()
{
    QListWidgetItem* current = _listWidget->currentItem();
    if (!current) return;

    HistoryItem item = current->data(Qt::UserRole).value<HistoryItem>();
    if (item.isImage) return;

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Clipboard Entry"));
    dialog.setMinimumSize(500, 300);
    dialog.setModal(false);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* meta = new QLabel(tr("Captured: %1 | %2 characters")
        .arg(item.timestamp.toString(Qt::ISODate))
        .arg(item.charCount));
    meta->setStyleSheet("QLabel { color: palette(mid); }");
    layout->addWidget(meta);

    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(item.text);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    layout->addWidget(textEdit);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    dialog.exec();
}

// ─── Keyboard navigation ───────────────────────────────────────────────────────

void ClipboardHistoryPanel::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (_listWidget->currentItem()) {
            onItemDoubleClicked(_listWidget->currentItem());
            event->accept();
            return;
        }
        break;
    case Qt::Key_Delete:
        onDeleteClicked();
        event->accept();
        return;
    case Qt::Key_Escape:
        // Close the panel.
        close();
        event->accept();
        return;
    default:
        break;
    }

    QDockWidget::keyPressEvent(event);
}

// ─── Persistence ──────────────────────────────────────────────────────────────

void ClipboardHistoryPanel::loadHistory()
{
    QSettings settings;
    settings.beginGroup(CLIPBOARD_HISTORY_GROUP);

    int size = settings.beginReadArray(CLIPBOARD_HISTORY_KEY);
    _historyItems.clear();
    _historyItems.reserve(size);

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        HistoryItem item;
        item.id = QUuid(settings.value("id").toString());
        item.text = settings.value("text").toString();
        item.charCount = settings.value("charCount").toInt();
        item.isPinned = settings.value("pinned").toBool();
        item.isImage = false; // Images are not persisted (too large)
        QString tsStr = settings.value("timestamp").toString();
        if (!tsStr.isEmpty())
            item.timestamp = QDateTime::fromString(tsStr, Qt::ISODate);
        else
            item.timestamp = QDateTime::currentDateTime();

        // Skip entries with empty text.
        if (!item.text.isEmpty())
            _historyItems.append(item);
    }

    settings.endArray();
    settings.endGroup();

    // Load settings.
    settings.beginGroup(CLIPBOARD_HISTORY_GROUP);
    _maxItems = settings.value("maxItems", 50).toInt();
    _autoCleanupDuplicates = settings.value("autoCleanupDuplicates", true).toBool();
    settings.endGroup();

    refreshListWidget();
}

void ClipboardHistoryPanel::saveHistory() const
{
    QSettings settings;
    settings.beginGroup(CLIPBOARD_HISTORY_GROUP);

    settings.beginWriteArray(CLIPBOARD_HISTORY_KEY, _historyItems.size());
    for (int i = 0; i < _historyItems.size(); ++i) {
        const HistoryItem& item = _historyItems.at(i);
        settings.setArrayIndex(i);
        settings.setValue("id", item.id.toString());
        settings.setValue("text", item.text);
        settings.setValue("charCount", item.charCount);
        settings.setValue("pinned", item.isPinned);
        settings.setValue("timestamp", item.timestamp.toString(Qt::ISODate));
        // Note: images are not persisted.
    }
    settings.endArray();

    settings.setValue("maxItems", _maxItems);
    settings.setValue("autoCleanupDuplicates", _autoCleanupDuplicates);
    settings.endGroup();
}

// ─── Event filter ──────────────────────────────────────────────────────────────

bool ClipboardHistoryPanel::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        // Pass key events to the panel.
        keyPressEvent(keyEvent);
        if (keyEvent->isAccepted()) return true;
    }
    return QDockWidget::eventFilter(watched, event);
}

// ─── Drag to editor ───────────────────────────────────────────────────────────

// Note: QListWidget::startDrag() is used for drag-and-drop.
// The editor accepts drops via its own event handlers.

// ─── Expand / Collapse ────────────────────────────────────────────────────────

void ClipboardHistoryPanel::onExpandCollapse(bool expand)
{
    if (_expanded == expand) return; // Already in desired state
    _expanded = expand;

    QWidget* content = widget();
    if (!content) return;

    if (expand) {
        // Expanded: show search box and status label, restore list widget height
        _searchBox->show();
        _statusLabel->show();

        // Restore list widget to stretch with available space
        _listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _listWidget->setFixedHeight(QWIDGETSIZE_MAX);

        // Trigger layout update
        content->layout()->activate();
    } else {
        // Collapsed: hide search box and status label, shrink list widget
        _searchBox->hide();
        _statusLabel->hide();

        // Show only ~3 items by fixing list widget height
        int itemHeight = _listWidget->sizeHintForRow(0);
        if (itemHeight <= 0) itemHeight = 24; // fallback
        _listWidget->setFixedHeight(itemHeight * 3 + 2 * _listWidget->frameWidth());
        _listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
}

