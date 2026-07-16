// ClipboardHistoryPanel.h - Clipboard history dock panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QStringList>
#include <QListWidget>
#include <QImage>
#include <QDateTime>
#include <QPointer>
#include <QUuid>
#include <QLabel>

class ScintillaEditor;

class ClipboardHistoryPanel : public QDockWidget {
    Q_OBJECT

public:
    /// Entry stored in clipboard history.
    struct HistoryItem {
        QUuid id;                 // unique ID for this entry
        QString text;             // plain text content
        QImage image;             // image content (null if text-only)
        QString html;             // rich-text HTML (may be empty)
        QDateTime timestamp;      // when the item was captured
        int charCount = 0;       // character count (for text)
        bool isPinned = false;    // pinned items are not pushed out by new entries
        bool isImage = false;     // true = this entry is an image

        // For display: first line truncated to maxLen
        QString displayText(int maxLen = 120) const;
        QString tooltipText() const;
    };

    explicit ClipboardHistoryPanel(QWidget* parent = nullptr);
    ~ClipboardHistoryPanel() override;

    /// Set the target editor for paste operations.
    void setEditor(ScintillaEditor* editor) { _editor = editor; }

    /// Add a text entry to the clipboard history.
    void addItem(const QString& text);

    /// Add an image entry to the clipboard history.
    void addItem(const QImage& image);

    /// Clear all history items.
    void clearHistory();

    /// Return the current list of clipboard history items.
    QList<HistoryItem> historyItems() const { return _historyItems; }

    /// Load history from persisted storage.
    void loadHistory();

    /// Persist history to storage.
    void saveHistory() const;

    /// Set the maximum number of items to keep (default 50).
    void setMaxItems(int max) { _maxItems = qMax(1, max); }
    int maxItems() const { return _maxItems; }

    /// Set whether to auto-cleanup duplicates (default true).
    void setAutoCleanupDuplicates(bool on) { _autoCleanupDuplicates = on; }
    bool autoCleanupDuplicates() const { return _autoCleanupDuplicates; }

public slots:
    /// Called when the system clipboard changes.
    void onClipboardChanged();

    /// Pin/unpin the selected item.
    void togglePinSelected();

    /// Show the full text of the selected item in a dialog.
    void showFullText();

signals:
    /// Emitted when a history item is double-clicked and pasted.
    void itemPasted(const QString& text);
    /// Emitted when the history changes (add/delete/clear/pin).
    void historyChanged();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);
    void onDeleteClicked();
    void onClearAllClicked();
    void onSearchTextChanged(const QString& text);
    void onCopyClicked();
    void onPinToggled();
    void onExpandCollapse(bool expand);

private:
    void setupUi();
    void setupContextMenu();

    void addItemInternal(const HistoryItem& item);
    void deduplicateAndAdd(const QString& text);
    void refreshListWidget();
    int findItemIndex(const QString& text) const;
    void updateStatusLabel();

    ScintillaEditor* _editor = nullptr;
    QListWidget* _listWidget = nullptr;
    QLineEdit* _searchBox = nullptr;
    QLabel* _statusLabel = nullptr;
    QList<HistoryItem> _historyItems;
    int _maxItems = 50;

    // Track the last clipboard content to avoid duplicates from our own paste
    QString _lastClipboardText;
    QImage _lastClipboardImage;
    bool _skipNextClipboardEvent = false;

    // Auto-cleanup duplicates
    bool _autoCleanupDuplicates = true;

    // Selected items for multi-select operations
    QList<QListWidgetItem*> _selectedItems;
};
