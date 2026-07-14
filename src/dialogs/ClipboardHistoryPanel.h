// ClipboardHistoryPanel.h - Clipboard history dock panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QStringList>
#include <QListWidget>

class ScintillaEditor;

class ClipboardHistoryPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ClipboardHistoryPanel(QWidget* parent = nullptr);
    ~ClipboardHistoryPanel() override;

    /// Set the target editor for paste operations.
    void setEditor(ScintillaEditor* editor) { _editor = editor; }

    /// Add a text entry to the clipboard history.
    /// If the entry already exists, it is moved to the top.
    void addItem(const QString& text);

    /// Clear all history items.
    void clearHistory();

    /// Return the current list of clipboard history items.
    QStringList historyItems() const { return _historyItems; }

    /// Load history from persisted storage.
    void loadHistory();

    /// Persist history to storage.
    void saveHistory() const;

    /// Set the maximum number of items to keep (default 50).
    void setMaxItems(int max) { _maxItems = max; }

public slots:
    /// Called when the system clipboard changes.
    void onClipboardChanged();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onDeleteClicked();
    void onClearAllClicked();
    void onSearchTextChanged(const QString& text);

private:
    void setupUi();
    void deduplicateAndAdd(const QString& text);

    ScintillaEditor* _editor = nullptr;
    QListWidget* _listWidget = nullptr;
    QLineEdit* _searchBox = nullptr;
    QStringList _historyItems;
    int _maxItems = 50;

    // Track the last clipboard text to avoid duplicates from our own paste
    QString _lastClipboardText;
    bool _skipNextClipboardEvent = false;
};
