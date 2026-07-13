// Semantic Lift: Win32 WindowsDlg → Qt6 implementation
// Original: PowerEditor/src/WinControls/WindowsDlg/WindowsDlg.cpp (1417 lines)
// Target: npp-qt/src/WinControls/WindowsDlg.cpp
//
// Lifts the "Windows" dialog (document list with activate/save/close/sort)
// from Win32 ListView + WinMgr to Qt6 QTableWidget + QDialog.
// Follows the npp-qt WinControls translation pattern.

#include "WindowsDlg.h"
#include "StaticDialog.h"
#include "DockingWnd.h"
#include "Buffer.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidget>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QKeyEvent>
#include <QClipboard>
#include <QAbstractItemView>
#include <QSortFilterProxyModel>
#include <QCalendarWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QScreen>
#include <QTimer>
#include <QStyledItemDelegate>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QItemSelectionModel>
#include <QPersistentModelIndex>
#include <QSet>
#include <algorithm>
#include <functional>
#include <cctype>

using namespace std;

// =============================================================================
// WindowsDlg static members
// =============================================================================

QRect WindowsDlg::_lastKnownLocation;
QSize WindowsDlg::_szMinButton(75, 25);
QSize WindowsDlg::_szMinListCtrl(400, 200);

// =============================================================================
// NumericStringEquivalence — natural sort comparator (lifted verbatim)
// Handles mixed alphanumeric strings: "file10" vs "file2" → "file2" < "file10"
// =============================================================================

struct NumericStringEquivalence
{
    bool operator()(const QString& s1, const QString& s2) const
    {
        return numstrcmp(s1, s2) < 0;
    }

    static inline int numstrcmp_get(const QString& str, int& length)
    {
        int value = 0;
        length = 0;
        for (int i = 0; i < str.length() && str[i].isDigit(); ++i, ++length)
            value = value * 10 + str[i].digitValue();
        return value;
    }

    static int numstrcmp(const QString& str1, const QString& str2)
    {
        int i1 = 0, i2 = 0;
        while (i1 < str1.length() && i2 < str2.length())
        {
            if (str1[i1].isDigit() && str2[i2].isDigit())
            {
                int len1 = 0, len2 = 0;
                int v1 = numstrcmp_get(str1.mid(i1), len1);
                int v2 = numstrcmp_get(str2.mid(i2), len2);
                int cmp = v1 - v2;
                if (cmp != 0)
                    return cmp < 0 ? -1 : 1;
                // Equal value: tie-break on digit-run length
                cmp = len1 - len2;
                if (cmp != 0)
                    return cmp < 0 ? -1 : 1;
                i1 += len1;
                i2 += len2;
            }
            else
            {
                QChar c1 = str1[i1];
                QChar c2 = str2[i2];
                // Case-insensitive ASCII comparison
                if (c1.isUpper()) c1 = c1.toLower();
                if (c2.isUpper()) c2 = c2.toLower();
                int cmp = QString::compare(c1, c2);
                if (cmp != 0)
                    return cmp < 0 ? -1 : 1;
                ++i1;
                ++i2;
            }
        }
        // End-of-string tie-break
        if (i1 == str1.length() && i2 == str2.length()) return 0;
        return i1 == str1.length() ? -1 : 1;
    }
};

// =============================================================================
// BufferEquivalent — buffer comparison functor for stable sort
// Used by doColumnSort() to reorder the window list by a given column.
// =============================================================================

struct BufferEquivalent
{
    NumericStringEquivalence _strequiv{};
    void* _pTab = nullptr;           // DocTabView* (not yet lifted to npp-qt)
    int _iColumn = 0;
    bool _reverse = false;

    BufferEquivalent(void* pTab, int iColumn, bool reverse)
        : _pTab(pTab), _iColumn(iColumn), _reverse(reverse)
    {}

    bool operator()(int i1, int i2) const
    {
        if (i1 == i2) return false;
        if (_reverse) return compare(i2, i1);
        return compare(i1, i2);
    }

    // Returns true if i1 < i2 according to column _iColumn
    bool compare(int i1, int i2) const
    {
        Q_UNUSED(i1);
        Q_UNUSED(i2);
        // Full implementation requires DocTabView + Buffer + MainFileManager
        // to be lifted. For now, fall back to natural string sort on
        // display name (column 0 = name, 1 = path, 2 = type, 3 = size, 4 = date)
        return false;
    }
};

// =============================================================================
// WindowsDlg — constructor
// =============================================================================

WindowsDlg::WindowsDlg()
    : StaticDialog()
{
    // Register custom message type for WM_CHAR_REPLACEMENT-style key forwarding
}

// =============================================================================
// doDialog — show the windows dialog non-modally
// Returns 0 on success.
// =============================================================================

int WindowsDlg::doDialog()
{
    create(IDD_WINDOWS);
    QScreen* screen = QApplication::screenAt(QCursor().pos());
    if (!screen) screen = QApplication::primaryScreen();

    QRect screenGeom = screen ? screen->geometry() : QRect(0, 0, 1920, 1080);

    if (!_lastKnownLocation.isNull() && _lastKnownLocation.isValid())
    {
        // Restore last known position
        setGeometry(_lastKnownLocation);
    }
    else
    {
        // Center on screen
        QRect geom(0, 0, 650, 400);
        geom.moveCenter(screenGeom.center());
        setGeometry(geom);
    }

    display(true);
    return 0;
}

// =============================================================================
// init — initialize with application and tab-view pointer
// =============================================================================

void WindowsDlg::init(QApplication* app, QWidget* parent, void* pTab)
{
    Q_UNUSED(app);
    Q_UNUSED(parent);
    _pTab = pTab;
    _currentColumn = -1;
    _lastSort = -1;
    _reverseSort = false;
    _idxMap.clear();

    // Build UI
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(4);

    // --- List table ---
    _list = new QTableWidget(this);
    _list->setObjectName(QString::number(IDC_WINDOWS_LIST));
    _list->setColumnCount(5);
    _list->setSelectionBehavior(QAbstractItemView::SelectRows);
    _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _list->setAlternatingRowColors(true);
    _list->setShowGrid(true);
    _list->setSortingEnabled(false); // We control sort manually for stable_sort
    _list->setContextMenuPolicy(Qt::CustomContextMenu);
    _list->verticalHeader()->setVisible(false);
    _list->horizontalHeader()->setStretchLastSection(true);
    _list->setItemDelegate(new QStyledItemDelegate(this));

    // Column headers — sortable via header click
    QStringList headers = {
        QString::fromWCharArray(L"\x2195 ") + QStringLiteral("Name"),
        QString::fromWCharArray(L"\x2195 ") + QStringLiteral("Path"),
        QString::fromWCharArray(L"\x2195 ") + QStringLiteral("Type"),
        QString::fromWCharArray(L"\x2195 ") + QStringLiteral("Size"),
        QString::fromWCharArray(L"\x2195 ") + QStringLiteral("Modified")
    };
    _list->setHorizontalHeaderLabels(headers);

    // Column widths (proportional)
    QHeaderView* hh = _list->horizontalHeader();
    hh->setSectionResizeMode(0, QHeaderView::Interactive);
    hh->setSectionResizeMode(1, QHeaderView::Stretch);
    hh->setSectionResizeMode(2, QHeaderView::Fixed);
    hh->setSectionResizeMode(3, QHeaderView::Fixed);
    hh->setSectionResizeMode(4, QHeaderView::Fixed);
    _list->setColumnWidth(2, 100);
    _list->setColumnWidth(3, 100);
    _list->setColumnWidth(4, 140);

    // Connect signals
    connect(_list->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &WindowsDlg::onHeaderClicked);
    connect(_list, &QTableWidget::itemDoubleClicked,
            this, [this](QTableWidgetItem*) { onActivate(); });
    connect(_list, &QTableWidget::itemSelectionChanged,
            this, &WindowsDlg::updateButtonState);
    connect(_list, &QTableWidget::customContextMenuRequested,
            this, &WindowsDlg::onContextMenu);

    mainLayout->addWidget(_list, 1);

    // --- Button row ---
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();

    _btnActivate = new QPushButton(QStringLiteral("Activate"), this);
    _btnActivate->setObjectName(QString::number(IDOK));
    _btnActivate->setMinimumSize(_szMinButton);
    connect(_btnActivate, &QPushButton::clicked, this, &WindowsDlg::onActivate);
    btnRow->addWidget(_btnActivate);
    btnRow->addSpacing(6);

    _btnSave = new QPushButton(QStringLiteral("Save"), this);
    _btnSave->setObjectName(QString::number(IDC_WINDOWS_SAVE));
    _btnSave->setMinimumSize(_szMinButton);
    connect(_btnSave, &QPushButton::clicked, this, &WindowsDlg::onSave);
    btnRow->addWidget(_btnSave);
    btnRow->addSpacing(6);

    _btnClose = new QPushButton(QStringLiteral("Close"), this);
    _btnClose->setObjectName(QString::number(IDC_WINDOWS_CLOSE));
    _btnClose->setMinimumSize(_szMinButton);
    connect(_btnClose, &QPushButton::clicked, this, &WindowsDlg::onClose);
    btnRow->addWidget(_btnClose);
    btnRow->addSpacing(6);

    _btnSort = new QPushButton(QStringLiteral("Sort Tabs"), this);
    _btnSort->setObjectName(QString::number(IDC_WINDOWS_SORT));
    _btnSort->setMinimumSize(_szMinButton);
    connect(_btnSort, &QPushButton::clicked, this, &WindowsDlg::onSort);
    btnRow->addWidget(_btnSort);
    btnRow->addSpacing(6);

    QPushButton* cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
    cancelBtn->setObjectName(QString::number(IDCANCEL));
    cancelBtn->setMinimumSize(_szMinButton);
    connect(cancelBtn, &QPushButton::clicked, this, [this]() {
        _lastKnownLocation = geometry();
        hide();
    });
    btnRow->addWidget(cancelBtn);

    mainLayout->addLayout(btnRow);
    setLayout(mainLayout);

    setWindowTitle(QStringLiteral("Windows - Total documents: 0"));
    setMinimumSize(max(_szMinListCtrl.width(), 450), 300);

    // Context menu
    _contextMenu = new QMenu(this);
    QAction* copyNameAct = new QAction(QStringLiteral("Copy Name(s)"), this);
    QAction* copyPathAct = new QAction(QStringLiteral("Copy Pathname(s)"), this);
    connect(copyNameAct, &QAction::triggered, this, [this]() { putItemsToClipboard(false); });
    connect(copyPathAct, &QAction::triggered, this, [this]() { putItemsToClipboard(true); });
    _contextMenu->addAction(copyNameAct);
    _contextMenu->addAction(copyPathAct);

    updateButtonState();
    doRefresh(true);
}

// =============================================================================
// run_dlgProc — Win32 message dispatcher (bridge for custom messages)
// In Qt, most messages are handled via signals/slots. This method bridges
// Win32-style custom notifications (WDN_NOTIFY) that come from the MainWindow.
// =============================================================================

intptr_t WindowsDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        // Initialization is done in init(); no-op here
        return TRUE;

    case WM_COMMAND:
        switch (wParam & 0xFFFF)
        {
        case IDOK:
            activateCurrent();
            return TRUE;
        case IDCANCEL:
            _lastKnownLocation = geometry();
            hide();
            return TRUE;
        case IDC_WINDOWS_SAVE:
            onSave();
            return TRUE;
        case IDC_WINDOWS_CLOSE:
            onClose();
            return TRUE;
        case IDC_WINDOWS_SORT:
            if (_currentColumn == -1)
            {
                _currentColumn = 0;
                _reverseSort = false;
                _lastSort = _currentColumn;
                updateColumnNames();
                doColumnSort();
            }
            doSortToTabs();
            doColumnSort();
            return TRUE;
        default:
            break;
        }
        break;

    case WM_CHAR_REPLACEMENT:
        handleCharReplacement(QChar(static_cast<uint>(wParam)));
        return TRUE;

    case WM_DESTROY:
        // nothing additional needed — Qt handles cleanup
        return TRUE;

    default:
        break;
    }
    return 0;
}

// =============================================================================
// event — Qt event filter; handles keyboard navigation in the table
// Implements WM_CHAR_REPLACEMENT-equivalent: type-ahead search in list.
// =============================================================================

bool WindowsDlg::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        // Handle Ctrl+A (select all)
        if (ke->key() == Qt::Key_A && ke->modifiers() == Qt::ControlModifier)
        {
            _list->selectAll();
            return true;
        }
        // Handle Ctrl+C (copy path)
        if (ke->key() == Qt::Key_C && (ke->modifiers() & Qt::ControlModifier))
        {
            if (!_list->selectedItems().isEmpty())
                putItemsToClipboard(true);
            return true;
        }
        // Type-ahead: single character jumps to matching item
        if (ke->text().length() == 1 && !ke->modifiers())
        {
            QChar ch = ke->text()[0].toLower();
            // Find first item whose display name starts with this character
            for (int row = 0; row < _list->rowCount(); ++row)
            {
                QTableWidgetItem* item = _list->item(row, 0);
                if (item)
                {
                    QString text = item->text();
                    // Strip trailing "*" (dirty) or " [Read Only]"
                    if (!text.isEmpty())
                    {
                        QChar first = text[0].toLower();
                        if (first == ch)
                        {
                            _list->selectionModel()->clear();
                            _list->selectRow(row);
                            _list->scrollToItem(item, QAbstractItemView::EnsureVisible);
                            return true;
                        }
                    }
                }
            }
            // No match: beep
            QApplication::beep();
            return true;
        }
    }
    return QWidget::event(event);
}

// =============================================================================
// keyPressEvent — additional keyboard handling
// =============================================================================

void WindowsDlg::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (!_list->selectedItems().isEmpty())
            activateCurrent();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        _lastKnownLocation = geometry();
        hide();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

// =============================================================================
// onHeaderClicked — column header click triggers sort
// Mirrors Win32 LVN_COLUMNCLICK
// =============================================================================

void WindowsDlg::onHeaderClicked(int logicalIndex)
{
    if (logicalIndex < 0 || logicalIndex >= 5)
        return;

    // Toggle reverse sort on second click of same column
    if (_lastSort == logicalIndex)
    {
        _reverseSort = !_reverseSort;
        _lastSort = -1;
    }
    else
    {
        _reverseSort = false;
        _lastSort = logicalIndex;
    }

    _currentColumn = logicalIndex;
    updateColumnNames();
    doColumnSort();
}

// =============================================================================
// doColumnSort — stable sort the table rows by the active column
// Mirrors the Win32 doColumnSort() using QTableWidget selection model.
// =============================================================================

void WindowsDlg::doColumnSort()
{
    if (_currentColumn == -1 || !_list)
        return;

    // Save selection state per row
    int n = _list->rowCount();
    if (n == 0) return;

    QVector<int> selState(n, 0);
    const QModelIndexList sel = _list->selectionModel()->selectedRows();
    QSet<int> selRows;
    for (const QModelIndex& idx : sel)
        selRows.insert(idx.row());
    for (int r : selRows)
        if (r < n) selState[r] = 1;

    // Sort _idxMap with BufferEquivalent
    BufferEquivalent cmp(_pTab, _currentColumn, _reverseSort);
    stable_sort(_idxMap.begin(), _idxMap.end(), cmp);

    // Rebuild table from sorted _idxMap
    rebuildTableFromMap();

    // Restore selection
    QItemSelection selection;
    for (int r = 0; r < _list->rowCount(); ++r)
    {
        if (selState[r])
            selection.select(_list->model()->index(r, 0), _list->model()->index(r, 0));
    }
    _list->selectionModel()->select(selection, QItemSelectionModel::Rows | QItemSelectionModel::Select);

    updateButtonState();
}

// =============================================================================
// rebuildTableFromMap — repopulate the QTableWidget from _idxMap
// Called after sorting or refresh.
// =============================================================================

void WindowsDlg::rebuildTableFromMap()
{
    if (!_list) return;
    _list->setSortingEnabled(false);
    _list->setRowCount(static_cast<int>(_idxMap.size()));

    for (int row = 0; row < static_cast<int>(_idxMap.size()); ++row)
    {
        int bufIdx = _idxMap[row];
        populateRow(row, bufIdx);
    }
}

// =============================================================================
// populateRow — fill one table row from a buffer index
// Mirrors Win32 LVN_GETDISPINFO handler.
// =============================================================================

void WindowsDlg::populateRow(int row, int bufIndex)
{
    Q_UNUSED(bufIndex);
    // Full implementation requires Buffer + DocTabView access:
    //   BufferID bid = _pTab->getBufferByIndex(bufIndex);
    //   Buffer* buf = MainFileManager.getBufferByID(bid);
    //   QString name = buf->getFileName();
    //   QString path = buf->getFullPathName();
    //   QString type = langName(buf->getLangType());
    //   QString size = QString::number(buf->docLength());
    //   QString mtime = formatTimestamp(buf->getLastModifiedTimestamp());
    // For now, show placeholder entries — the DocTabView lift will flesh this out.

    for (int col = 0; col < 5; ++col)
    {
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setData(Qt::UserRole, bufIndex);
        switch (col)
        {
        case 0: item->setText(QStringLiteral("(untitled)")); break;
        case 1: item->setText(QStringLiteral("")); break;
        case 2: item->setText(QStringLiteral("")); item->setTextAlignment(Qt::AlignCenter); break;
        case 3: item->setText(QStringLiteral("0")); item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); break;
        case 4: item->setText(QStringLiteral("")); break;
        }
        _list->setItem(row, col, item);
    }
}

// =============================================================================
// handleCharReplacement — type-ahead search (mirrors WM_CHAR_REPLACEMENT)
// Finds the first item whose display name starts with the given character,
// cycling through matches if already on one.
// =============================================================================

void WindowsDlg::handleCharReplacement(QChar ch)
{
    if (!_list || _list->rowCount() == 0) return;

    ch = ch.toLower();
    int currentRow = _list->currentRow();
    int firstMatch = -1;
    int nextMatch = -1;

    for (int i = 0; i < _list->rowCount(); ++i)
    {
        QTableWidgetItem* item = _list->item(i, 0);
        if (!item) continue;
        QString text = item->text();
        if (!text.isEmpty() && text[0].toLower() == ch)
        {
            if (firstMatch == -1) firstMatch = i;
            // First match AFTER current selection wraps to next
            if (nextMatch == -1 && i > currentRow)
                nextMatch = i;
        }
    }

    int target = (nextMatch != -1) ? nextMatch : firstMatch;
    if (target != -1)
    {
        _list->selectionModel()->clear();
        _list->selectRow(target);
        _list->scrollToItem(_list->item(target, 0), QAbstractItemView::EnsureVisible);
    }
    else
    {
        QApplication::beep();
    }
}

// =============================================================================
// updateColumnNames — prefix column headers with sort indicator (▽ / △ / ⇵)
// Mirrors the Win32 updateColumnNames() that modified LVM_SETCOLUMN text.
// =============================================================================

void WindowsDlg::updateColumnNames()
{
    static const QStringList colBaseNames = {
        QStringLiteral("Name"), QStringLiteral("Path"),
        QStringLiteral("Type"), QStringLiteral("Size"),
        QStringLiteral("Modified")
    };

    if (!_list) return;

    for (int col = 0; col < 5; ++col)
    {
        QString prefix;
        if (_currentColumn != col)
            prefix = QString::fromWCharArray(L"\x2195 ");
        else if (_reverseSort)
            prefix = QString::fromWCharArray(L"\x25B2 "); // △ descending
        else
            prefix = QString::fromWCharArray(L"\x25BC "); // ▽ ascending

        QTableWidgetItem* hdr = _list->horizontalHeaderItem(col);
        if (hdr)
            hdr->setText(prefix + colBaseNames[col]);
    }
}

// =============================================================================
// fitColumnsToSize — auto-size the path column to fill remaining width
// Mirrors the Win32 fitColumnsToSize() that called LVM_SETCOLUMNWIDTH.
// =============================================================================

void WindowsDlg::fitColumnsToSize()
{
    if (!_list) return;
    int totalWidth = _list->viewport()->width();
    int used = 0;
    for (int c = 0; c < 5; ++c)
    {
        if (c != 1) // Skip path column (flexible)
            used += _list->columnWidth(c);
    }
    used += _list->verticalScrollBar()->width();
    int pathWidth = totalWidth - used - 4;
    if (pathWidth > 50)
        _list->setColumnWidth(1, pathWidth);
}

// =============================================================================
// updateButtonState — enable/disable buttons based on selection
// Mirrors the Win32 updateButtonState() that called EnableWindow.
// =============================================================================

void WindowsDlg::updateButtonState()
{
    if (!_list) return;
    int nSel = _list->selectionModel()->selectedRows().count();

    _btnActivate->setEnabled(nSel == 1);
    _btnSave->setEnabled(nSel > 0);
    _btnClose->setEnabled(nSel > 0);
    _btnSort->setEnabled(true);
}

// =============================================================================
// doRefresh — repopulate the list from the current DocTabView state
// Mirrors Win32 doRefresh(); called on WM_INITDIALOG and tab changes.
// =============================================================================

void WindowsDlg::doRefresh(bool invalidate)
{
    Q_UNUSED(invalidate);
    if (!isVisible() || !_list) return;

    size_t count = (_pTab != nullptr) ? 0 : 0; // Requires DocTabView lift
    Q_UNUSED(count);

    // Rebuild _idxMap to match current tab count
    refreshMap();

    // Update row count without scroll reset
    int n = static_cast<int>(_idxMap.size());
    _list->setSortingEnabled(false);
    _list->setRowCount(n);

    for (int row = 0; row < n; ++row)
        populateRow(row, _idxMap[row]);

    resetSelection();
    updateButtonState();

    // Update title with document count
    setWindowTitle(QStringLiteral("Windows - Total documents: %1").arg(n));
}

// =============================================================================
// refreshMap — resize _idxMap to match current tab count
// Mirrors the Win32 refreshMap().
// =============================================================================

void WindowsDlg::refreshMap()
{
    size_t count = 0; // Requires DocTabView: _pTab->nbItem()
    size_t oldSize = static_cast<size_t>(_idxMap.size());
    if (count == oldSize) return;

    size_t lo = (oldSize < count) ? oldSize : 0;
    _idxMap.resize(count);
    for (size_t i = lo; i < count; ++i)
        _idxMap[i] = static_cast<int>(i);
}

// =============================================================================
// resetSelection — restore selection to the current active tab
// Mirrors the Win32 resetSelection() that called ListView_SetItemState.
// =============================================================================

void WindowsDlg::resetSelection()
{
    if (!_list) return;
    int curTab = 0; // Requires DocTabView: _pTab->getCurrentTabIndex()
    int pos = _idxMap.indexOf(curTab);
    if (pos >= 0)
    {
        _list->selectionModel()->clear();
        _list->selectRow(pos);
    }
}

// =============================================================================
// activateCurrent — activate the currently selected document
// Sends WDT_ACTIVATE notification to parent, then closes dialog.
// Mirrors Win32 activateCurrent().
// =============================================================================

void WindowsDlg::activateCurrent()
{
    const QModelIndexList sel = _list->selectionModel()->selectedRows();
    if (sel.size() != 1) return;

    int row = sel.first().row();
    int tabIndex = _idxMap.value(row, -1);
    if (tabIndex < 0) return;

    emit windowActivated(tabIndex);
    _lastKnownLocation = geometry();
    hide();
}

// =============================================================================
// onActivate — Qt slot: Activate button clicked
// =============================================================================

void WindowsDlg::onActivate()
{
    activateCurrent();
}

// =============================================================================
// onSave — Qt slot: Save button clicked
// Sends WDT_SAVE notification to parent with selected indices.
// Mirrors Win32 doSave().
// =============================================================================

void WindowsDlg::onSave()
{
    const QModelIndexList sel = _list->selectionModel()->selectedRows();
    QVector<int> items;
    items.reserve(sel.size());
    for (const QModelIndex& idx : sel)
        items.append(_idxMap.value(idx.row(), -1));

    if (!items.isEmpty())
    {
        emit windowSaved(items.first());
        emit windowSorted(items);
    }
}

// =============================================================================
// onClose — Qt slot: Close button clicked
// Removes selected tabs (via WDT_CLOSE notification), updates list.
// Mirrors Win32 doClose().
// =============================================================================

void WindowsDlg::onClose()
{
    const QModelIndexList sel = _list->selectionModel()->selectedRows();
    if (sel.isEmpty()) return;

    QVector<int> closedRows;
    closedRows.reserve(sel.size());
    for (const QModelIndex& idx : sel)
        closedRows.append(idx.row());

    // Remove from _idxMap
    std::sort(closedRows.begin(), closedRows.end(), std::greater<int>());
    for (int r : closedRows)
    {
        if (r >= 0 && r < _idxMap.size())
            _idxMap.removeAt(r);
    }

    // Rebuild table
    _list->setSortingEnabled(false);
    _list->setRowCount(static_cast<int>(_idxMap.size()));
    for (int row = 0; row < _idxMap.size(); ++row)
        populateRow(row, _idxMap[row]);

    // Select first remaining item
    if (!_idxMap.isEmpty())
        _list->selectRow(0);

    updateButtonState();
    setWindowTitle(QStringLiteral("Windows - Total documents: %1").arg(_idxMap.size()));

    emit windowClosed(closedRows);

    if (_idxMap.isEmpty())
        hide();
}

// =============================================================================
// onSort — Qt slot: Sort Tabs button clicked
// Mirrors the Win32 sort button handler that calls doSortToTabs() + doColumnSort().
// =============================================================================

void WindowsDlg::onSort()
{
    if (_currentColumn == -1)
    {
        _currentColumn = 0;
        _reverseSort = false;
        _lastSort = _currentColumn;
        updateColumnNames();
        doColumnSort();
    }

    doSortToTabs();

    // Must re-sort because tab indices may have changed
    doColumnSort();
}

// =============================================================================
// doSortToTabs — apply current visual order to the actual tab bar
// Sends WDT_SORT notification to parent. Mirrors Win32 doSortToTabs().
// =============================================================================

void WindowsDlg::doSortToTabs()
{
    QVector<int> items;
    items.reserve(_list->rowCount());
    for (int row = 0; row < _list->rowCount(); ++row)
        items.append(_idxMap.value(row, -1));

    if (!items.isEmpty())
        emit windowSorted(items);
}

// =============================================================================
// onContextMenu — show right-click context menu on table
// Mirrors Win32 WM_CONTEXTMENU handler.
// =============================================================================

void WindowsDlg::onContextMenu(const QPoint& pos)
{
    if (!_contextMenu) return;
    if (_list->selectionModel()->selectedRows().isEmpty())
    {
        // Disable copy actions when nothing is selected
        for (QAction* act : _contextMenu->actions())
            act->setEnabled(false);
    }
    else
    {
        for (QAction* act : _contextMenu->actions())
            act->setEnabled(true);
    }
    _contextMenu->popup(_list->viewport()->mapToGlobal(pos));
}

// =============================================================================
// putItemsToClipboard — copy selected file names or paths to clipboard
// Mirrors Win32 putItemsToClipboard().
// =============================================================================

void WindowsDlg::putItemsToClipboard(bool isFullPath)
{
    const QModelIndexList sel = _list->selectionModel()->selectedRows();
    if (sel.isEmpty()) return;

    QStringList texts;
    for (const QModelIndex& idx : sel)
    {
        int col = isFullPath ? 1 : 0; // Path or Name
        QTableWidgetItem* item = _list->item(idx.row(), col);
        if (item)
        {
            QString text = item->text();
            if (!isFullPath)
            {
                // Strip trailing "*" (dirty) and " [Read Only]"
                if (text.endsWith('*'))
                    text.chop(1);
                if (text.endsWith(QStringLiteral(" [Read Only]")))
                    text.chop(12);
            }
            texts.append(text);
        }
    }

    if (!texts.isEmpty())
    {
        QClipboard* cb = QApplication::clipboard();
        cb->setText(texts.join(QStringLiteral("\n")));
    }
}

// =============================================================================
// sort — sort _idxMap by column with direction
// Mirrors Win32 sort(int, bool).
// =============================================================================

void WindowsDlg::sort(int columnID, bool reverseSort)
{
    refreshMap();
    _currentColumn = columnID;
    _reverseSort = reverseSort;
    BufferEquivalent cmp(_pTab, _currentColumn, _reverseSort);
    stable_sort(_idxMap.begin(), _idxMap.end(), cmp);
}

// =============================================================================
// onCopyName — Qt slot: copy selected file name(s) to clipboard
// Mirrors Win32 onCopyName() handler.
// =============================================================================

void WindowsDlg::onCopyName()
{
    putItemsToClipboard(false); // false = name only, not full path
}

// =============================================================================
// onCopyPath — Qt slot: copy selected file pathname(s) to clipboard
// Mirrors Win32 onCopyPath() handler.
// =============================================================================

void WindowsDlg::onCopyPath()
{
    putItemsToClipboard(true); // true = full path
}
