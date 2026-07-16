// ClipboardHistoryPanel.h - Clipboard history with pinning and search
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QListWidget>
#include <QWidget>
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QList>
#include <QVector>
#include <QMenu>

// ============================================================================
// EntryType — types of clipboard entries
// ============================================================================
enum class EntryType { Text, Image, File, Html };

// ============================================================================
// ClipboardEntry — single clipboard history entry
// ============================================================================
struct ClipboardEntry {
    QString text;              // Text content (for Text/Html types)
    QByteArray imageData;     // Image data as Base64 (for Image type)
    QString filePath;         // File path (for File type)
    EntryType type = EntryType::Text;
    QDateTime timestamp;
    bool pinned = false;
    int refCount = 0;
};

// ============================================================================
// ClipboardHistoryPanel — main clipboard history widget
// ============================================================================
class ClipboardHistoryPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ClipboardHistoryPanel(QWidget* parent = nullptr);
    ~ClipboardHistoryPanel() override;

    // Configuration
    void setMaxEntries(int max);
    int maxEntries() const { return _maxEntries; }

    void setEnablePinning(bool enable);
    bool isPinningEnabled() const { return pinningEnabled; }

    void setEnableSearch(bool enable);
    bool isSearchEnabled() const { return searchEnabled; }

    // Entry management
    void addEntry(const ClipboardEntry& entry);
    void pinEntry(int index);
    void unpinEntry(int index);
    void copyEntry(int index);
    void deleteEntry(int index);
    void clearAll();

    // Filtering
    void filter(const QString& text);
    QString currentFilter() const { return _currentFilter; }

    // Persistence
    void saveHistory();
    void loadHistory();

    // Import clipboard content
    void captureClipboard();

signals:
    void entrySelected(const ClipboardEntry& entry);
    void entryDeleted(int index);
    void entryPinned(int index, bool pinned);
    void clearAllRequested();

public slots:
    void onClearAll();

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);
    void onItemChanged(QListWidgetItem* item);
    void onSearchTextChanged(const QString& text);
    void onPinClicked();
    void onDeleteClicked();
    void onContextMenu(const QPoint& pos);
    void onCopySelected();
    void onPinSelected();
    void onDeleteSelected();
    void onSelectAll();
    void onRefreshDisplay();
    void onCopyToClipboard(const ClipboardEntry& entry);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    void setupToolbar();
    void setupSearchBar();
    void setupListWidget();
    void rebuildList();
    void sortEntries();
    void trimToMaxEntries();
    QString getEntryPreview(const ClipboardEntry& entry) const;
    QString getRelativeTime(const QDateTime& dt) const;
    QIcon getTypeIcon(EntryType type) const;
    QIcon getPinIcon(bool pinned) const;
    int entryIndexForItem(QListWidgetItem* item) const;
    void showContextMenuForItem(QListWidgetItem* item, const QPoint& pos);
    QString getConfigPath() const;

    // UI components
    QWidget* _content = nullptr;
    QListWidget* _listWidget = nullptr;
    QWidget* _searchBar = nullptr;
    QWidget* _toolbar = nullptr;

    // Toolbar actions
    QAction* _clearAllAction = nullptr;
    QAction* _refreshAction = nullptr;

    // Internal state
    QList<ClipboardEntry> entries;
    QString _currentFilter;
    int _maxEntries = 100;
    bool pinningEnabled = true;
    bool searchEnabled = true;
    bool _multiSelect = false;

    // Icons (lazy loaded)
    QIcon _iconText;
    QIcon _iconImage;
    QIcon _iconFile;
    QIcon _iconHtml;
    QIcon _iconPin;
    QIcon _iconUnpin;

    // Config file
    static constexpr const char* ConfigFileName = "clipboard_history.json";
};

// ============================================================================
// ClipboardItemDelegate — custom delegate for clipboard list items
// ============================================================================
class ClipboardItemDelegate : public QAbstractItemDelegate {
    Q_OBJECT
public:
    explicit ClipboardItemDelegate(QObject* parent = nullptr);
    ~ClipboardItemDelegate() override;

    void setPinningEnabled(bool enabled) { _pinningEnabled = enabled; }

    // QAbstractItemDelegate interface
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

signals:
    void pinClicked(const QModelIndex& index);
    void deleteClicked(const QModelIndex& index);

protected:
    void drawTextPreview(QPainter* painter, const QRect& rect,
                        const QString& text, const QStyleOptionViewItem& option) const;
    void drawImageThumbnail(QPainter* painter, const QRect& rect,
                           const QByteArray& imageData,
                           const QStyleOptionViewItem& option) const;
    void drawFileEntry(QPainter* painter, const QRect& rect,
                      const QString& filePath,
                      const QStyleOptionViewItem& option) const;

private:
    bool _pinningEnabled = true;
};
