// Semantic Lift: Win32 Finder → Qt6 Find Result Panel
// Full implementation of FindResultPanel — dockable results tree for Find in Files.

#include "FindResultPanel.h"
#include "NppDarkMode.h"
#include "IconLoader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QFont>
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QColor>
#include <QTextStream>
#include <QUuid>

// =============================================================================
// Constants
// =============================================================================

// Minimum column widths
constexpr int COL_FILE_WIDTH_MIN  = 200;
constexpr int COL_LINE_WIDTH_MIN  = 60;
constexpr int COL_MATCH_WIDTH_MIN = 400;

// Maximum characters to show per result line before truncating
constexpr int MAX_LINE_PREVIEW = 500;

// =============================================================================
// FindResultTreeWidget
// =============================================================================

FindResultTreeWidget::FindResultTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    // Mono font for match content
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(f.pointSize() > 8 ? f.pointSize() : 9);
    setFont(f);

    setRootIsDecorated(true);
    setUniformRowHeights(false);
    setAllColumnsShowFocus(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAnimated(false);        // Disable animation for performance with large result sets
    setSortingEnabled(false);  // Results are ordered as found; re-sorting would lose grouping
    setExpandsOnDoubleClick(true);

    // Context menu via custom handler
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void FindResultTreeWidget::mousePressEvent(QMouseEvent* event)
{
    // Middle-click on a top-level item collapses/expands it
    if (event->button() == Qt::MiddleButton) {
        QTreeWidgetItem* item = itemAt(event->pos());
        if (item && item->parent() == nullptr) {
            if (item->isExpanded())
                collapseItem(item);
            else
                expandItem(item);
            return;
        }
    }
    QTreeWidget::mousePressEvent(event);
}

// =============================================================================
// FindResultPanel
// =============================================================================

FindResultPanel::FindResultPanel(QWidget* parent)
    : DockingWnd(parent)
{
    setWindowTitle(QStringLiteral("Find Result"));
    setupUi();
    applyDarkMode();

    // Register with dark mode so colours update if theme changes
    NppDarkMode::instance().registerChild(this);
}

void FindResultPanel::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Toolbar ---
    createToolbar();
    mainLayout->addWidget(_toolbar);

    // --- Filter bar ---
    QWidget* filterBar = new QWidget(this);
    QHBoxLayout* filterLayout = new QHBoxLayout(filterBar);
    filterLayout->setContentsMargins(4, 2, 4, 2);
    filterLayout->setSpacing(4);

    QLabel* filterLbl = new QLabel(QStringLiteral("Filter:"), filterBar);
    _filterEdit = new QLineEdit(filterBar);
    _filterEdit->setPlaceholderText(QStringLiteral("type to filter results..."));
    _filterEdit->setClearButtonEnabled(true);
    connect(_filterEdit, &QLineEdit::textChanged, this, &FindResultPanel::onFilterChanged);
    filterLayout->addWidget(filterLbl);
    filterLayout->addWidget(_filterEdit);
    mainLayout->addWidget(filterBar);

    // --- Tree widget ---
    _tree = new FindResultTreeWidget(this);
    _tree->setColumnCount(3);
    QStringList headers;
    headers << QStringLiteral("File") << QStringLiteral("Line") << QStringLiteral("Content");
    _tree->setHeaderLabels(headers);

    // Column sizing
    QHeaderView* hv = _tree->header();
    hv->setStretchLastSection(true);
    hv->setSectionResizeMode(COL_FILE,  QHeaderView::Interactive);
    hv->setSectionResizeMode(COL_LINE, QHeaderView::ResizeToContents);
    hv->setSectionResizeMode(COL_MATCH, QHeaderView::Stretch);
    hv->setMinimumSectionSize(COL_LINE_WIDTH_MIN);

    // Column widths
    _tree->setColumnWidth(COL_FILE,  COL_FILE_WIDTH_MIN);
    _tree->setColumnWidth(COL_LINE, COL_LINE_WIDTH_MIN);

    // Initial sorting: by file (col 0), ascending
    hv->setSortIndicatorShown(false);  // Sorting is disabled for grouped display

    // Connections
    connect(_tree, &QTreeWidget::itemDoubleClicked,
            this, &FindResultPanel::onResultDoubleClicked);
    connect(_tree, &QTreeWidget::customContextMenuRequested,
            this, &FindResultPanel::onContextMenuRequested);

    mainLayout->addWidget(_tree);

    // --- Status bar ---
    QWidget* statusBar = new QWidget(this);
    statusBar->setMinimumHeight(20);
    QHBoxLayout* statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(6, 1, 6, 1);
    statusLayout->setSpacing(4);
    _statusLabel = new QLabel(QStringLiteral("No results"), statusBar);
    _statusLabel->setStyleSheet(QStringLiteral("color: #888;"));
    statusLayout->addWidget(_statusLabel);
    statusLayout->addStretch();
    mainLayout->addWidget(statusBar);

    // Initial empty state
    _statusLabel->setText(QStringLiteral("Run a Find in Files search to see results here."));
}

void FindResultPanel::createToolbar()
{
    _toolbar = new QToolBar(QStringLiteral("Find Result Toolbar"), this);
    _toolbar->setMovable(false);
    _toolbar->setFloatable(false);
    _toolbar->setIconSize(QSize(16, 16));

    // Collapse / Expand all
    _actCollapseAll = _toolbar->addAction(
        IconLoader::get(QStringLiteral("collapse")),
        QStringLiteral("Collapse All"),
        this, [this]() { collapseAll(); });
    _actCollapseAll->setToolTip(QStringLiteral("Collapse all file groups"));

    _actExpandAll = _toolbar->addAction(
        IconLoader::get(QStringLiteral("expand")),
        QStringLiteral("Expand All"),
        this, [this]() { expandAll(); });
    _actExpandAll->setToolTip(QStringLiteral("Expand all file groups"));

    _toolbar->addSeparator();

    // Clear
    _actClear = _toolbar->addAction(
        IconLoader::get(QStringLiteral("delete")),
        QStringLiteral("Clear"),
        this, [this]() { clearResults(); });
    _actClear->setToolTip(QStringLiteral("Clear all results"));

    // Copy
    _actCopy = _toolbar->addAction(
        IconLoader::get(QStringLiteral("copy")),
        QStringLiteral("Copy"),
        this, &FindResultPanel::onCopyResults);
    _actCopy->setToolTip(QStringLiteral("Copy selected results to clipboard"));
    _actCopy->setEnabled(false);
}

void FindResultPanel::applyDarkMode()
{
    NppDarkMode::instance().applyToWidget(this);
}

// =============================================================================
// Public API — search lifecycle
// =============================================================================

void FindResultPanel::beginSearch(const QString& directory, const QString& filter,
                                   const QString& searchText, int totalFilesEstimate)
{
    Q_UNUSED(totalFilesEstimate);
    _searchRoot = directory;
    _currentSearchText = searchText;
    _totalMatches = 0;
    _totalFilesSearched = 0;
    _totalFilesMatched = 0;
    _searching = true;
    _fileItemMap.clear();

    // Clear existing items
    _tree->clear();

    // Status
    _statusLabel->setText(QStringLiteral("Searching %1 ...").arg(directory));
    _statusLabel->setStyleSheet(QStringLiteral("color: #4a9eff;"));

    // Expand hint: if > 3 files, collapse by default to avoid GUI freeze
    // (we'll add items as they come in, so the tree stays responsive)
    Q_UNUSED(filter);
}

void FindResultPanel::endSearch(int totalMatches, int totalFilesSearched)
{
    _searching = false;
    _totalMatches = totalMatches;
    _totalFilesSearched = totalFilesSearched;

    updateStatusLabel();

    if (totalMatches == 0) {
        _statusLabel->setText(QStringLiteral("No matches found."));
        _statusLabel->setStyleSheet(QStringLiteral("color: #f0ad4e;"));
    } else {
        _statusLabel->setStyleSheet(QStringLiteral("color: #5cb85c;"));
        // Expand all file groups so results are immediately visible
        QTimer::singleShot(10, this, [this]() { expandAll(); });
    }

    emit searchFinished(totalMatches, totalFilesSearched);
}

void FindResultPanel::clearResults()
{
    _tree->clear();
    _fileItemMap.clear();
    _totalMatches = 0;
    _totalFilesMatched = 0;
    _totalFilesSearched = 0;
    _searching = false;

    _statusLabel->setText(QStringLiteral("No results"));
    _statusLabel->setStyleSheet(QStringLiteral("color: #888;"));
}

// =============================================================================
// Public API — add results
// =============================================================================

void FindResultPanel::addResult(const QString& filePath, int lineNumber,
                                 const QString& lineContent)
{
    addResult(filePath, lineNumber, -1, -1, lineContent);
}

void FindResultPanel::addResult(const QString& filePath, int lineNumber,
                                 int /*colStart*/, int /*colEnd*/,
                                 const QString& lineContent)
{
    if (filePath.isEmpty()) return;

    // Find or create the file-group node
    QTreeWidgetItem* fileItem = findOrCreateFileItem(filePath);

    // Truncate long lines for display performance
    QString displayContent = lineContent;
    if (displayContent.length() > MAX_LINE_PREVIEW) {
        displayContent.truncate(MAX_LINE_PREVIEW);
        displayContent += QStringLiteral("...");
    }

    // Create the line result child item
    QTreeWidgetItem* matchItem = new QTreeWidgetItem(fileItem);
    matchItem->setData(COL_FILE,   Qt::UserRole, filePath);     // full path
    matchItem->setData(COL_LINE,   Qt::UserRole, lineNumber);    // line number
    matchItem->setText(COL_LINE,   QString::number(lineNumber));
    matchItem->setText(COL_MATCH,  displayContent);
    matchItem->setToolTip(COL_MATCH, lineContent);  // Full text in tooltip

    // Colour coding: file items bold, match items normal
    // (NppDarkMode re-colours these via stylesheet)
    matchItem->setFont(COL_LINE,  QFont(QStringLiteral("monospace")));
    matchItem->setFont(COL_MATCH, QFont(QStringLiteral("monospace")));

    // Store the line number in column 0 for display (shown in file group row)
    fileItem->setText(COL_LINE, QString::number(fileItem->childCount()));

    // Expand the file group when first match added
    if (fileItem->childCount() == 1) {
        fileItem->setExpanded(true);
        ++_totalFilesMatched;
        updateStatusLabel();
    }

    ++_totalMatches;
    updateStatusLabel();
}

// =============================================================================
// Private helpers
// =============================================================================

QTreeWidgetItem* FindResultPanel::findOrCreateFileItem(const QString& filePath)
{
    // Check cache first
    auto it = _fileItemMap.find(filePath);
    if (it != _fileItemMap.end()) {
        return it.value();
    }

    // Create new top-level file group item
    QTreeWidgetItem* item = new QTreeWidgetItem(_tree);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);  // File rows are not selectable
    item->setText(COL_FILE, relativePath(filePath));
    item->setToolTip(COL_FILE, filePath);
    item->setFont(COL_FILE, QFont(QStringLiteral("sans-serif")));

    // Make file name bold to distinguish from match rows
    QFont boldFont = item->font(COL_FILE);
    boldFont.setBold(true);
    item->setFont(COL_FILE, boldFont);

    // Store full path for reference
    item->setData(COL_FILE, Qt::UserRole + 1, filePath);

    _fileItemMap.insert(filePath, item);
    return item;
}

// =============================================================================
// Slots
// =============================================================================

void FindResultPanel::onResultDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;

    // Only match rows (children of file groups) trigger navigation
    QTreeWidgetItem* parent = item->parent();
    if (parent != nullptr) {
        // It's a match line item
        QString filePath = item->data(COL_FILE, Qt::UserRole).toString();
        int lineNumber = item->data(COL_LINE, Qt::UserRole).toInt();
        if (!filePath.isEmpty() && lineNumber > 0) {
            emit resultSelected(filePath, lineNumber);
        }
    }
}

void FindResultPanel::onContextMenuRequested(const QPoint& pos)
{
    QTreeWidgetItem* item = _tree->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    menu.addAction(QStringLiteral("Copy"),
                   this, &FindResultPanel::onCopyResults);
    menu.addSeparator();
    menu.addAction(QStringLiteral("Expand All"),
                   this, [this]() { expandAll(); });
    menu.addAction(QStringLiteral("Collapse All"),
                   this, [this]() { collapseAll(); });
    menu.addSeparator();
    menu.addAction(QStringLiteral("Clear All"),
                   this, [this]() { clearResults(); });

    // If right-clicking a file group, add "Remove file's results"
    if (item->parent() == nullptr) {
        QString fp = item->data(COL_FILE, Qt::UserRole + 1).toString();
        if (!fp.isEmpty()) {
            menu.addSeparator();
            QAction* removeAct = menu.addAction(
                QStringLiteral("Remove this file's results"),
                this, [this, fp]() { removeResultsForFile(fp); });
            Q_UNUSED(removeAct);
        }
    }

    menu.exec(_tree->mapToGlobal(pos));
}

void FindResultPanel::onCopyResults()
{
    QString clipboardText;
    QTextStream ts(&clipboardText);

    for (int i = 0; i < _tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* fileItem = _tree->topLevelItem(i);
        QString filePath = fileItem->data(COL_FILE, Qt::UserRole + 1).toString();
        ts << filePath << QStringLiteral(":\n");
        for (int j = 0; j < fileItem->childCount(); ++j) {
            QTreeWidgetItem* matchItem = fileItem->child(j);
            ts << QStringLiteral("  ") << matchItem->text(COL_LINE) << QStringLiteral(": ")
               << matchItem->text(COL_MATCH) << QStringLiteral("\n");
        }
        ts << QStringLiteral("\n");
    }

    QApplication::clipboard()->setText(clipboardText);

    // Update copy action enabled state
    _actCopy->setEnabled(_tree->topLevelItemCount() > 0);
}

void FindResultPanel::removeResultsForFile(const QString& filePath)
{
    auto it = _fileItemMap.find(filePath);
    if (it == _fileItemMap.end()) return;

    // Subtract match count from totals
    QTreeWidgetItem* item = it.value();
    _totalMatches -= item->childCount();
    _totalFilesMatched = qMax(0, _totalFilesMatched - 1);

    int idx = _tree->indexOfTopLevelItem(item);
    if (idx >= 0) {
        delete _tree->takeTopLevelItem(idx);
    }
    _fileItemMap.erase(it);
    updateStatusLabel();
}

void FindResultPanel::onFilterChanged(const QString& text)
{
    if (text.isEmpty()) {
        // Show all items
        for (int i = 0; i < _tree->topLevelItemCount(); ++i) {
            _tree->topLevelItem(i)->setHidden(false);
        }
        return;
    }

    // Filter: show only file groups and match lines that contain the filter text
    // Case-insensitive substring match
    QString lc = text.toLower();
    for (int i = 0; i < _tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* fileItem = _tree->topLevelItem(i);
        bool fileMatches = fileItem->text(COL_FILE).toLower().contains(lc);
        bool anyChildMatches = false;

        for (int j = 0; j < fileItem->childCount(); ++j) {
            QTreeWidgetItem* child = fileItem->child(j);
            bool childMatches = child->text(COL_MATCH).toLower().contains(lc) ||
                                child->text(COL_LINE).toLower().contains(lc);
            child->setHidden(!childMatches);
            if (childMatches) anyChildMatches = true;
        }

        // Show file group if itself matches or any child matches
        fileItem->setHidden(!(fileMatches || anyChildMatches));
    }
}

void FindResultPanel::display(bool doShow)
{
    if (doShow) {
        QWidget::show();
        raise();
    } else {
        hide();
    }
}
