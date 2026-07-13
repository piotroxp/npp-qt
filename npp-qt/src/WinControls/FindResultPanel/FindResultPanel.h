// Semantic Lift: Win32 Finder → Qt6 Find Result Panel
// Original: PowerEditor/src/FindCharsInRange.* (Find Result window)
// Target: npp-qt/src/WinControls/FindResultPanel/
// A dockable panel showing Find-in-Files / Find-in-Projects results.
// Results are grouped by file: top-level items are file paths,
// child items are matching lines with line number and content.

#pragma once

#include "DockingWnd.h"
#include <QTreeWidget>
#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QTimer>

// Column indices for the result tree
constexpr int COL_FILE   = 0;   // File path (top-level) or blank
constexpr int COL_LINE   = 1;   // Line number
constexpr int COL_MATCH  = 2;   // Matched line content

// Icon indices for the result tree
constexpr int ICON_FILE   = 0;  // File icon
constexpr int ICON_MATCH  = 1;  // Match line icon

// =============================================================================
// FindResultTreeWidget — tree widget for search results
// =============================================================================
class FindResultTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit FindResultTreeWidget(QWidget* parent = nullptr);
    ~FindResultTreeWidget() override = default;

protected:
    // Handle middle-click to close file group
    void mousePressEvent(QMouseEvent* event) override;
};

// =============================================================================
// FindResultPanel — dockable panel showing Find in Files results
// =============================================================================
class FindResultPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FindResultPanel(QWidget* parent = nullptr);
    ~FindResultPanel() override = default;

    // --- Public API ---

    /// Add a single match result.
    void addResult(const QString& filePath, int lineNumber, const QString& lineContent);

    /// Add a result with character position info (for highlighting).
    void addResult(const QString& filePath, int lineNumber, int colStart, int colEnd,
                   const QString& lineContent);

    /// Clear all results and reset the panel.
    void clearResults();

    /// Called when a new search begins — clears results and updates status.
    void beginSearch(const QString& directory, const QString& filter,
                     const QString& searchText, int totalFilesEstimate = 0);

    /// Called when search finishes.
    void endSearch(int totalMatches, int totalFilesSearched);

    /// Set the current search directory (used to compute relative paths).
    void setSearchRoot(const QString& root) { _searchRoot = root; }

    /// Collapse all file groups.
    void collapseAll() { if (_tree) _tree->collapseAll(); }

    /// Expand all file groups.
    void expandAll() { if (_tree) _tree->expandAll(); }

    /// Auto-scroll to the latest result.
    void scrollToLastResult();

    // --- DockingWnd interface ---
    void display(bool doShow) ;

public slots:
    /// Invoked when the user double-clicks a result line → open file at line.
    void onResultDoubleClicked(QTreeWidgetItem* item, int column);

    /// Invoked when user right-clicks for context menu.
    void onContextMenuRequested(const QPoint& pos);

    /// Filter results by the text in the filter bar.
    void onFilterChanged(const QString& text);

    /// Copy the selected results to clipboard.
    void onCopyResults();

    /// Remove all results for a given file (e.g., when file is deleted/reloaded).
    void removeResultsForFile(const QString& filePath);

signals:
    /// Emitted when user selects a result to navigate.
    /// Not connected directly to file open — Notepad_plus connects this
    /// to its own open routine so it can handle BufferID management.
    void resultSelected(const QString& filePath, int lineNumber);

    /// Emitted when search completes.
    void searchFinished(int totalMatches, int totalFiles);

    /// Emitted during search with progress info.
    void progressUpdated(int currentFile, int totalFiles, const QString& currentFileName);

private:
    void setupUi();
    void createToolbar();
    void applyDarkMode();

    // --- Helpers ---
    QTreeWidgetItem* findOrCreateFileItem(const QString& filePath);
    QString relativePath(const QString& absolutePath) const;
    void updateStatusLabel();

    // State
    QString _searchRoot;           // Root directory of current search
    QString _currentSearchText;     // Pattern being searched
    int _totalMatches = 0;
    int _totalFilesSearched = 0;
    int _totalFilesMatched = 0;
    bool _searching = false;

    // UI members
    QToolBar* _toolbar = nullptr;
    FindResultTreeWidget* _tree = nullptr;
    QLabel* _statusLabel = nullptr;
    QLineEdit* _filterEdit = nullptr;
    QAction* _actExpandAll = nullptr;
    QAction* _actCollapseAll = nullptr;
    QAction* _actClear = nullptr;
    QAction* _actCopy = nullptr;

    // Map: absolute file path → top-level tree item
    // Used to avoid creating duplicate file-group nodes.
    QMap<QString, QTreeWidgetItem*> _fileItemMap;
};

// =============================================================================
// Inline implementation
// =============================================================================

inline void FindResultPanel::scrollToLastResult()
{
    if (!_tree) return;
    QTreeWidgetItem* last = _tree->topLevelItem(_tree->topLevelItemCount() - 1);
    if (last) {
        QTreeWidgetItem* lastChild = last->child(last->childCount() - 1);
        _tree->setCurrentItem(lastChild ? lastChild : last);
        _tree->scrollToItem(lastChild ? lastChild : last, QAbstractItemView::EnsureVisible);
    }
}

inline QString FindResultPanel::relativePath(const QString& absolutePath) const
{
    if (_searchRoot.isEmpty() || !absolutePath.startsWith(_searchRoot))
        return absolutePath;
    QString rel = absolutePath.mid(_searchRoot.length());
    if (rel.startsWith('/') || rel.startsWith('\\'))
        rel = rel.mid(1);
    return rel;
}

inline void FindResultPanel::updateStatusLabel()
{
    if (!_statusLabel) return;
    if (_searching) {
        _statusLabel->setText(QStringLiteral("Searching... %1 matches, %2 files")
            .arg(_totalMatches).arg(_totalFilesMatched));
    } else {
        _statusLabel->setText(QStringLiteral("%1 matches in %2 files")
            .arg(_totalMatches).arg(_totalFilesMatched));
    }
}
