// clipboardHistoryPanel.cpp — Qt6 clipboard history panel implementation
// Copyright (C) 2026 Agent Army | GPL v3
//
// Win32 → Qt6 translation of clipboardHistoryPanel.cpp
// Original: Notepad++ PowerEditor/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp
//
// Key translations:
//   OpenClipboard/GetClipboardData    → QApplication::clipboard()->text()
//   SetClipboardViewer/Change…Chain  → QClipboard::changed signal (Qt handles chain)
//   SendDlgItemMessage(LB_*)         → QListWidget methods
//   WM_CHANGECBCHAIN / WM_DRAWCLIPBOARD → clipboard monitoring via QClipboard
//   lstrlenW / WideCharToMultiByte   → QString
//   DrawText on HDC                  → QListWidgetItem role or paintEvent override
//   GlobalLock / GlobalUnlock       → QByteArray (owning)
//   SetTimer / KillTimer            → QTimer

#include "clipboardHistoryPanel.h"
#include "clipboardHistoryPanel_rc.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QScrollBar>
#include <QHeaderView>

// ─── Max display length for list items ────────────────────────────────────────
static constexpr int MAX_DISPLAY_LENGTH = 64;

// ─── Persistence keys ─────────────────────────────────────────────────────────
static const char* SETTINGS_GROUP = "ClipboardHistory";
static const char* SETTINGS_MAX   = "maxItems";

// ─── ClipboardDataInfo helpers ─────────────────────────────────────────────────

ClipboardDataInfo ClipboardHistoryControl::getClipboardData()
{
    ClipboardDataInfo clipboardData;

    QClipboard* clipboard = QApplication::clipboard();
    // Support both clipboard and selection (middle-click on Linux).
    QString text = clipboard->text(QClipboard::Clipboard);
    if (text.isEmpty()) {
        text = clipboard->text(QClipboard::Selection);
    }
    if (text.isEmpty())
        return clipboardData;

    // Store as UTF-8 bytes; QString handles the encoding.
    clipboardData._data = text.toUtf8();
    clipboardData._isBinaryContained = false;
    return clipboardData;
}

// ─── ByteArray ─────────────────────────────────────────────────────────────────

ByteArray::ByteArray(const ClipboardDataInfo& cd)
{
    _length = static_cast<size_t>(cd._data.size());
    if (!_length) {
        _pBytes = nullptr;
        return;
    }
    _pBytes = new unsigned char[_length];
    std::memcpy(_pBytes, cd._data.constData(), _length);
}

StringArray::StringArray(const ClipboardDataInfo& cd, size_t maxLen)
{
    const size_t len = static_cast<size_t>(cd._data.size());
    if (!len) {
        _pBytes = nullptr;
        return;
    }

    bool isCompleted = (len <= maxLen);
    _length = isCompleted ? len : maxLen;

    _pBytes = new unsigned char[_length + (isCompleted ? 0 : 2)];
    size_t i = 0;
    for (; i < _length; ++i) {
        if (!isCompleted && (i == _length - 5 || i == _length - 3 || i == _length - 1))
            _pBytes[i] = 0;
        else if (!isCompleted && (i == _length - 6 || i == _length - 4 || i == _length - 2))
            _pBytes[i] = '.';
        else
            _pBytes[i] = static_cast<unsigned char>(cd._data[static_cast<int>(i)]);
    }

    if (!isCompleted) {
        _pBytes[i++] = 0;
        _pBytes[i]   = 0;
    }
}

// ─── Search in internal storage ─────────────────────────────────────────────────

int ClipboardHistoryControl::getClipboardDataIndex(const ClipboardDataInfo& cbd)
{
    for (int i = 0, len = m_items.size(); i < len; ++i) {
        if (cbd._data.size() == m_items[i]._data.size()) {
            bool found = true;
            for (int j = 0, jlen = cbd._data.size(); j < jlen; ++j) {
                if (static_cast<unsigned char>(cbd._data[j]) !=
                    static_cast<unsigned char>(m_items[i]._data[j])) {
                    found = false;
                    break;
                }
            }
            if (found)
                return i;
        }
    }
    return -1;
}

// ─── Add / insert entries ─────────────────────────────────────────────────────

void ClipboardHistoryControl::addToClipboardHistory(const ClipboardDataInfo& cbd)
{
    const int i = getClipboardDataIndex(cbd);
    if (i == 0)
        return; // Already at top.

    if (i != -1) {
        // Remove duplicate (will be re-inserted at top).
        m_items.remove(i);
        delete m_list->takeItem(i);
    }

    // Prepend — most recent first.
    m_items.prepend(cbd);
    insertEntry(cbd);
}

void ClipboardHistoryControl::insertEntry(const ClipboardDataInfo& cbd)
{
    // Create item; display is driven by getDisplayString().
    QString display = getDisplayString(cbd, MAX_DISPLAY_LENGTH);
    QListWidgetItem* item = new QListWidgetItem(display, m_list);
    item->setData(Qt::UserRole + 1, cbd._data);
    item->setData(Qt::UserRole + 2, cbd._isBinaryContained);
    item->setToolTip(getDisplayString(cbd, 1024));
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    // Insert at row 0 (most recent).
    m_list->insertItem(0, item);
    m_list->setCurrentRow(0);
}

// ─── Display helper ──────────────────────────────────────────────────────────

QString ClipboardHistoryControl::getDisplayString(const ClipboardDataInfo& cbd, int maxLen) const
{
    QString full = QString::fromUtf8(cbd._data);
    if (cbd._isBinaryContained) {
        QByteArray preview = cbd._data.left(maxLen).toHex();
        return "[Binary] " + preview;
    }
    QString firstLine = full.split('\n').first();
    if (firstLine.length() > maxLen) {
        firstLine.truncate(maxLen);
        firstLine += "…";
    }
    return firstLine.isEmpty() ? tr("(empty)") : firstLine;
}

// ─── Constructor / destructor ─────────────────────────────────────────────────

ClipboardHistoryControl::ClipboardHistoryControl(QWidget* parent)
    : QWidget(parent)
{
    setupUi();

    // Monitor system clipboard — mirrors WM_DRAWCLIPBOARD.
    QClipboard* clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::changed,
            this, &ClipboardHistoryControl::onClipboardChanged,
            Qt::UniqueConnection);

    // Default colours.
    m_bgColor = QColor(0xFF, 0xFF, 0xFF);
    m_fgColor = QColor(0x00, 0x00, 0x00);

    loadHistory();
    updateStatusLabel();
}

ClipboardHistoryControl::~ClipboardHistoryControl()
{
    saveHistory();
}

// ─── UI setup ─────────────────────────────────────────────────────────────────

void ClipboardHistoryControl::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);

    // Search box
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(tr("Search…"));
    m_search->setObjectName("clipboardSearchBox");
    mainLayout->addWidget(m_search);
    connect(m_search, &QLineEdit::textChanged,
            this, &ClipboardHistoryControl::onSearchChanged);

    // History list
    m_list = new QListWidget(this);
    m_list->setObjectName("clipboardListWidget");
    m_list->setAlternatingRowColors(true);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(m_list);

    // Status label
    m_status = new QLabel(this);
    m_status->setObjectName("clipboardStatusLabel");
    mainLayout->addWidget(m_status);

    // Buttons
    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* copyBtn   = new QPushButton(tr("Copy"), this);
    QPushButton* delBtn    = new QPushButton(tr("Delete"), this);
    QPushButton* clearBtn  = new QPushButton(tr("Clear All"), this);
    btnRow->addWidget(copyBtn);
    btnRow->addWidget(delBtn);
    btnRow->addStretch();
    btnRow->addWidget(clearBtn);
    mainLayout->addLayout(btnRow);

    // Connections
    connect(m_list, &QListWidget::itemDoubleClicked,
            this, &ClipboardHistoryControl::onItemDoubleClicked);
    connect(m_list, &QListWidget::customContextMenuRequested,
            this, &ClipboardHistoryControl::onContextMenu);
    connect(copyBtn,  &QPushButton::clicked, this, &ClipboardHistoryControl::onCopyToClipboard);
    connect(delBtn,   &QPushButton::clicked, this, &ClipboardHistoryControl::onDeleteEntry);
    connect(clearBtn, &QPushButton::clicked, this, &ClipboardHistoryControl::clearHistory);
}

// ─── Clipboard monitoring ─────────────────────────────────────────────────────

void ClipboardHistoryControl::onClipboardChanged()
{
    if (m_skipNext) {
        m_skipNext = false;
        return;
    }
    if (!m_tracking)
        return;

    ClipboardDataInfo data = getClipboardData();
    if (!data._data.isEmpty())
        addToClipboardHistory(data);
}

// ─── List interaction ─────────────────────────────────────────────────────────

void ClipboardHistoryControl::onItemDoubleClicked(QListWidgetItem* item)
{
    if (!item)
        return;
    const int row = m_list->row(item);
    if (row < 0 || row >= m_items.size())
        return;
    doPaste(m_items[row]);
}

void ClipboardHistoryControl::onCopyToClipboard()
{
    const int row = m_list->currentRow();
    if (row < 0 || row >= m_items.size())
        return;
    const ClipboardDataInfo& cbd = m_items[row];
    m_skipNext = true;
    QClipboard* clipboard = QApplication::clipboard();
    QString text = QString::fromUtf8(cbd._data);
    clipboard->setText(text, QClipboard::Clipboard);
}

void ClipboardHistoryControl::onDeleteEntry()
{
    const int row = m_list->currentRow();
    if (row < 0)
        return;
    if (row < m_items.size())
        m_items.remove(row);
    delete m_list->takeItem(row);
    emit historyChanged();
    updateStatusLabel();
    saveHistory();
}

void ClipboardHistoryControl::clearHistory()
{
    m_items.clear();
    m_list->clear();
    emit historyChanged();
    updateStatusLabel();
    saveHistory();
}

void ClipboardHistoryControl::onSearchChanged(const QString& text)
{
    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem* item = m_list->item(i);
        if (text.isEmpty()) {
            item->setHidden(false);
            continue;
        }
        bool match = false;
        if (i < m_items.size()) {
            QString entryText = QString::fromUtf8(m_items[i]._data).toLower();
            match = entryText.contains(text.toLower());
        }
        item->setHidden(!match);
    }
}

void ClipboardHistoryControl::onContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = m_list->itemAt(pos);
    QMenu menu(this);

    QAction* copyAct = menu.addAction(tr("Copy to Clipboard"));
    connect(copyAct, &QAction::triggered, this, &ClipboardHistoryControl::onCopyToClipboard);

    QAction* pasteAct = menu.addAction(tr("Paste into Editor"));
    connect(pasteAct, &QAction::triggered, this, [this, item]() {
        onItemDoubleClicked(item);
    });

    menu.addSeparator();

    QAction* delAct = menu.addAction(tr("Delete"));
    connect(delAct, &QAction::triggered, this, &ClipboardHistoryControl::onDeleteEntry);

    menu.addSeparator();

    QAction* clearAct = menu.addAction(tr("Clear All"));
    connect(clearAct, &QAction::triggered, this, &ClipboardHistoryControl::clearHistory);

    menu.exec(m_list->mapToGlobal(pos));
}

// ─── Refresh / status ───────────────────────────────────────────────────────

void ClipboardHistoryControl::refreshList()
{
    m_list->clear();
    for (const ClipboardDataInfo& cbd : std::as_const(m_items)) {
        insertEntry(cbd);
    }
    updateStatusLabel();
}

void ClipboardHistoryControl::updateStatusLabel()
{
    m_status->setText(tr("%1 item(s)").arg(m_items.size()));
}

// ─── Paste into editor ────────────────────────────────────────────────────────

void ClipboardHistoryControl::doPaste(const ClipboardDataInfo& cbd)
{
    // Copy text to clipboard and signal the parent (e.g. MainWindow) to handle
    // paste into the active Scintilla view.  This mirrors the original
    // Win32 approach of sending SCI_PASTE through the Scintilla HWND.
    m_skipNext = true;
    QClipboard* clipboard = QApplication::clipboard();
    QString text = QString::fromUtf8(cbd._data);
    clipboard->setText(text, QClipboard::Clipboard);

    emit pasteRequested(text);
}

// ─── Colour API ───────────────────────────────────────────────────────────────

void ClipboardHistoryControl::paintEvent(QPaintEvent* event)
{
    // Paint the list area with the themed background.
    if (m_bgColor.isValid()) {
        QPainter p(this);
        p.fillRect(event->rect(), m_bgColor);
    }
    QWidget::paintEvent(event);
}

void ClipboardHistoryControl::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

// ─── Persistence ─────────────────────────────────────────────────────────────

void ClipboardHistoryControl::loadHistory()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    // TODO: deserialise ClipboardDataInfo entries from JSON.
    settings.endGroup();
}

void ClipboardHistoryControl::saveHistory() const
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(SETTINGS_MAX, 20);
    // TODO: serialise ClipboardDataInfo entries to JSON.
    settings.endGroup();
}

// ─── Public API ───────────────────────────────────────────────────────────────

void ClipboardHistoryControl::setEditor(ScintillaEditView* editor)
{
    m_editor = editor;
}

void ClipboardHistoryControl::setParentWindow(QWidget* parent)
{
    m_parent = parent;
}

bool ClipboardHistoryControl::trackClipboardOps(bool track)
{
    bool previous = m_tracking;
    m_tracking = track;
    return previous;
}
