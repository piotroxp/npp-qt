// clipboardHistoryPanel.h — Qt6 clipboard history panel
// Copyright (C) 2026 Agent Army | GPL v3
//
// Win32 → Qt6 translation of clipboardHistoryPanel.h
// Original: Notepad++ PowerEditor/WinControls/ClipboardHistory/clipboardHistoryPanel.h

#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QColor>
#include <QPaintEvent>
#include <QPointer>

// Forward declaration (Scintilla editor interface)
class ScintillaEditView;

#define CH_PROJECTPANELTITLE  QT_TR_NOOP("Clipboard History")

// ─── Data structures (translated from Win32 byte-array model) ─────────────────

/// Raw clipboard entry holding raw bytes (mirrors original HGLOBAL storage)
struct ClipboardDataInfo {
    QByteArray _data;
    bool _isBinaryContained = false;
};

/// Byte array wrapper — owns a heap-allocated buffer
class ByteArray {
public:
    ByteArray() = default;
    explicit ByteArray(const ClipboardDataInfo& cd);

    ~ByteArray() { delete[] _pBytes; }

    const unsigned char* getPointer() const { return _pBytes; }
    size_t getLength() const { return _length; }

protected:
    unsigned char* _pBytes = nullptr;
    size_t _length = 0;
};

/// String array — truncated byte buffer for display preview
class StringArray : public ByteArray {
public:
    StringArray(const ClipboardDataInfo& cd, size_t maxLen);
};

// ─── ClipboardHistoryPanel ────────────────────────────────────────────────────

/// Clipboard history panel — Win32 DockingDlgInterface → QWidget + QDockWidget
class ClipboardHistoryControl : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardHistoryControl(QWidget* parent = nullptr);
    ~ClipboardHistoryControl() override;

    /// Initialise with the editor widget pointer (for paste operations).
    void setEditor(ScintillaEditView* editor);

    /// Set the docking parent window.
    void setParentWindow(QWidget* parent);

    /// Called when the NPP dark-mode state changes.
    void setBackgroundColor(const QColor& bg) { m_bgColor = bg; }
    void setForegroundColor(const QColor& fg) { m_fgColor = fg; }

    /// Enable/disable clipboard tracking (mirrors trackClipboardOps).
    bool trackClipboardOps(bool track);

    // ── Clipboard data access (Qt6 translation of Win32 APIs) ──────────────

    /// Read current clipboard data (replaces OpenClipboard / GetClipboardData).
    ClipboardDataInfo getClipboardData();

    /// Add a captured entry to history.
    void addToClipboardHistory(const ClipboardDataInfo& cbd);

    /// Find index of existing entry in internal storage.
    int getClipboardDataIndex(const ClipboardDataInfo& cbd);

signals:
    /// Emitted when the user double-clicks an entry to paste it.
    void pasteRequested(const QString& text);

    /// Emitted when the history list changes.
    void historyChanged();

public slots:
    /// Handle clipboard change notification (connected to QClipboard::changed).
    void onClipboardChanged();

    /// Clear all history entries.
    void clearHistory();

protected:
    /// Custom painting for list items (mirrors WM_DRAWITEM).
    void paintEvent(QPaintEvent* event) override;

    /// Resize list when panel resizes.
    void resizeEvent(QResizeEvent* event) override;

private slots:
    /// Handle double-click on history item.
    void onItemDoubleClicked(QListWidgetItem* item);

    /// Handle context menu request.
    void onContextMenu(const QPoint& pos);

    /// Copy selected entry back to clipboard.
    void onCopyToClipboard();

    /// Delete selected entry.
    void onDeleteEntry();

    /// Search/filter the list.
    void onSearchChanged(const QString& text);

private:
    /// Build the UI layout.
    void setupUi();

    /// Rebuild the list widget from internal storage.
    void refreshList();
    void loadHistory();
    void saveHistory() const;
    void updateStatusLabel();

    /// Get display string for an entry (truncated preview).
    QString getDisplayString(const ClipboardDataInfo& cbd, int maxLen = 64) const;

    /// Insert entry at the top of the list.
    void insertEntry(const ClipboardDataInfo& cbd);

    /// Perform paste into the editor.
    void doPaste(const ClipboardDataInfo& cbd);

    // ── Internal state ─────────────────────────────────────────────────────
    ScintillaEditView* m_editor = nullptr;
    QPointer<QWidget> m_parent;

    QListWidget* m_list = nullptr;
    QLineEdit* m_search = nullptr;
    QLabel* m_status = nullptr;

    QVector<ClipboardDataInfo> m_items;

    QColor m_bgColor;   // background colour
    QColor m_fgColor;  // foreground colour

    /// Track whether to capture clipboard changes (mirrors m_tracking).
    bool m_tracking = true;

    /// Skip next clipboard-changed signal (avoid recording our own paste).
    bool m_skipNext = false;
};
