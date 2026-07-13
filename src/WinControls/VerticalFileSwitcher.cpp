// Semantic Lift: Win32 VerticalFileSwitcher → Qt6 VerticalFileSwitcher
// Original: PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcher.cpp
// Target: npp-qt/src/WinControls/VerticalFileSwitcher.cpp

#include "VerticalFileSwitcher.h"
#include "DockingWnd.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileInfo>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QSortFilterProxyModel>
#include <QTableWidgetSelectionRange>
#include <QFileDialog>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <functional>

// =============================================================================
// VerticalFileSwitcherListView — constructor and initialization
// Mirrors Win32 VerticalFileSwitcherListView::init() + initList().
// =============================================================================

VerticalFileSwitcherListView::VerticalFileSwitcherListView(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(COL_COUNT);
    setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Ext"), QStringLiteral("Path")});
    verticalHeader()->setVisible(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAlternatingRowColors(true);
    setShowGrid(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setAcceptDrops(false);
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Column widths (mirrors Win32 LVCOLUMN setup)
    horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(COL_EXT, QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(COL_PATH, QHeaderView::Stretch);
    setColumnWidth(COL_EXT, 60);

    // Sort on column header click
    setSortingEnabled(true);
    sortByColumn(COL_NAME, Qt::AscendingOrder);

    // Double-click → activate file
    connect(this, &QTableWidget::itemDoubleClicked,
            this, [this](QTableWidgetItem*) {
                QList<QTableWidgetItem*> sel = selectedItems();
                if (!sel.isEmpty())
                {
                    int row = sel.first()->row();
                    auto key = std::make_pair(
                        BufferID(reinterpret_cast<void*>(item(row, COL_NAME)->data(Qt::UserRole).toLongLong())),
                        item(row, COL_NAME)->data(Qt::UserRole + 1).toInt()
                    );
                    auto it = _entries.find(key);
                    if (it != _entries.end())
                        emit fileActivated(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
                }
            });

    // Middle button → close file
    connect(this, &QTableWidget::customContextMenuRequested,
            this, &VerticalFileSwitcherListView::showContextMenu);
}

void VerticalFileSwitcherListView::init(void*)
{
    // Called from VerticalFileSwitcher::init() which has _hImgLst
    // Image list is not used directly in Qt — icons come from DocTabView
}

void VerticalFileSwitcherListView::destroy()
{
    // Clean up allocated FileEntry data
    for (auto it = _entries.begin(); it != _entries.end(); ++it)
    {
        // FileEntry has no heap allocation, just cleanup if needed
    }
    _entries.clear();
}

void VerticalFileSwitcherListView::initList()
{
    setRowCount(0);
    _entries.clear();
    _currentIndex = -1;
    _nextInsertIndex = 0;

    // Full reload from DocTabView requires DocTabView lift
    // This is called on WM_INITDIALOG to repopulate from open buffers
}

// =============================================================================
// Item management — buffer/view operations
// Mirrors Win32 ListView messages for buffer management.
// =============================================================================

BufferID VerticalFileSwitcherListView::getBufferInfoFromIndex(int index, int& view) const
{
    if (index < 0 || index >= rowCount())
        return BufferID(nullptr);

    const QTableWidgetItem* nameItem = item(index, COL_NAME);
    if (!nameItem)
        return BufferID(nullptr);

    view = nameItem->data(Qt::UserRole + 1).toInt();
    return BufferID(reinterpret_cast<void*>(nameItem->data(Qt::UserRole).toLongLong()));
}

int VerticalFileSwitcherListView::find(BufferID bufferID, int iView) const
{
    for (int r = 0; r < rowCount(); ++r)
    {
        QTableWidgetItem* it = item(r, COL_NAME);
        if (it && it->data(Qt::UserRole).toLongLong() == reinterpret_cast<long long>(bufferID._p) &&
            it->data(Qt::UserRole + 1).toInt() == iView)
        {
            return r;
        }
    }
    return -1;
}

int VerticalFileSwitcherListView::newItem(BufferID bufferID, int iView)
{
    int existing = find(bufferID, iView);
    if (existing != -1)
        return existing;
    return add(bufferID, iView);
}

int VerticalFileSwitcherListView::add(BufferID bufferID, int iView)
{
    int row = rowCount();
    insertRow(row);

    // Query buffer info from DocTabView — stub requires DocTabView lift
    // For now, create a placeholder entry
    FileEntry entry(bufferID, iView, QStringLiteral(""), 0, -1);
    auto key = std::make_pair(bufferID, iView);
    _entries[key] = entry;

    QTableWidgetItem* nameItem = new QTableWidgetItem(QStringLiteral("(new file)"));
    nameItem->setData(Qt::UserRole, reinterpret_cast<long long>(bufferID._p));
    nameItem->setData(Qt::UserRole + 1, iView);
    setItem(row, COL_NAME, nameItem);

    QTableWidgetItem* extItem = new QTableWidgetItem(QStringLiteral(""));
    extItem->setTextAlignment(Qt::AlignCenter);
    setItem(row, COL_EXT, extItem);

    QTableWidgetItem* pathItem = new QTableWidgetItem(QStringLiteral(""));
    setItem(row, COL_PATH, pathItem);

    setCurrentCell(row, COL_NAME);
    _currentIndex = row;
    _nextInsertIndex = row + 1;

    return row;
}

int VerticalFileSwitcherListView::closeItem(BufferID bufferID, int iView)
{
    int index = find(bufferID, iView);
    if (index >= 0)
    {
        auto key = std::make_pair(bufferID, iView);
        _entries.erase(key);
        removeRow(index);
        if (_currentIndex >= index && _currentIndex > 0)
            --_currentIndex;
    }
    return index;
}

void VerticalFileSwitcherListView::activateItem(BufferID bufferID, int iView)
{
    int row = find(bufferID, iView);
    if (row >= 0)
    {
        setCurrentCell(row, COL_NAME);
        _currentIndex = row;
        ensureVisibleCurrentItem();
        auto key = std::make_pair(bufferID, iView);
        auto it = _entries.find(key);
        if (it != _entries.end())
            emit fileActivated(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
    }
}

void VerticalFileSwitcherListView::setItemIconStatus(BufferID bufferID)
{
    // Requires DocTabView to query buffer status (dirty/readonly/monitoring)
    // For now: update row if found
    for (int r = 0; r < rowCount(); ++r)
    {
        QTableWidgetItem* it = item(r, COL_NAME);
        if (it && it->data(Qt::UserRole).toLongLong() == reinterpret_cast<long long>(bufferID._p))
        {
            setItemIconStatus(bufferID, r);
            return;
        }
    }
}

void VerticalFileSwitcherListView::setItemIconStatus(BufferID bufferID, int row)
{
    // Requires DocTabView: get buffer status flags
    // 0 = clean, 1 = dirty, 2 = readonly, 3 = monitoring
    Q_UNUSED(bufferID);
    Q_UNUSED(row);
    // Icon/styling would be applied here when DocTabView is lifted
}

void VerticalFileSwitcherListView::setItemColor(BufferID bufferID)
{
    // Requires DocTabView: get buffer color ID
    Q_UNUSED(bufferID);
    viewport()->update();
}

QString VerticalFileSwitcherListView::getFullFilePath(size_t i) const
{
    if (i >= static_cast<size_t>(rowCount()))
        return QString();

    const QTableWidgetItem* pathItem = item(static_cast<int>(i), COL_PATH);
    const QTableWidgetItem* nameItem = item(static_cast<int>(i), COL_NAME);
    if (!pathItem || !nameItem)
        return QString();

    QString path = pathItem->text();
    if (!path.isEmpty() && !path.endsWith(QDir::separator()) && !path.endsWith('/'))
        path += QDir::separator();
    return path + nameItem->text();
}

// =============================================================================
// Column management
// Mirrors Win32 LVM_INSERTCOLUMN / LVM_SETCOLUMNWIDTH / LVM_DELETECOLUMN.
// =============================================================================

void VerticalFileSwitcherListView::insertColumn(const QString& name, int width, int index)
{
    Q_UNUSED(name);
    Q_UNUSED(width);
    Q_UNUSED(index);
    // Columns are fixed in constructor
}

void VerticalFileSwitcherListView::resizeColumns(int totalWidth)
{
    // Compute available width (accounting for scrollbar)
    bool hasVScroll = viewport()->height() < rowCount() * rowHeight(0) + horizontalHeader()->height();
    int scrollbarWidth = hasVScroll ? style()->pixelMetric(QStyle::PM_ScrollBarExtent) : 0;
    int available = totalWidth - scrollbarWidth;

    // Proportional widths matching Win32 layout
    int used = columnWidth(COL_EXT) + 4;
    int nameWidth = available - used;
    if (nameWidth < 50)
        nameWidth = 50;

    setColumnWidth(COL_NAME, nameWidth);
}

void VerticalFileSwitcherListView::deleteColumn(size_t)
{
    // Fixed column set — not supported
}

// =============================================================================
// Selection and navigation
// =============================================================================

std::vector<QPair<int, int>> VerticalFileSwitcherListView::getSelectedBuffers(bool reverse) const
{
    std::vector<QPair<int, int>> result;
    for (const QModelIndex& idx : selectionModel()->selectedRows())
    {
        int row = idx.row();
        QTableWidgetItem* it = item(row, COL_NAME);
        if (it)
        {
            int bufID = it->data(Qt::UserRole).toInt();
            int view = it->data(Qt::UserRole + 1).toInt();
            result.push_back(qMakePair(bufID, view));
        }
    }
    if (reverse)
        std::reverse(result.begin(), result.end());
    return result;
}

void VerticalFileSwitcherListView::reload()
{
    // Suppress repaints during bulk update
    setUpdatesEnabled(false);
    removeAllItems();
    initList();
    resizeColumns(viewport()->width());
    setUpdatesEnabled(true);
    viewport()->update();
}

void VerticalFileSwitcherListView::removeAllItems()
{
    setRowCount(0);
    _entries.clear();
    _currentIndex = -1;
    _nextInsertIndex = 0;
}

void VerticalFileSwitcherListView::redrawItems()
{
    viewport()->update();
}

void VerticalFileSwitcherListView::ensureVisibleCurrentItem()
{
    if (_currentIndex >= 0 && _currentIndex < rowCount())
    {
        this->setCurrentCell(_currentIndex, COL_NAME);
        scrollToItem(item(_currentIndex, 0), QAbstractItemView::EnsureVisible);
    }
}

void VerticalFileSwitcherListView::selectCurrentItem()
{
    if (_currentIndex >= 0 && _currentIndex < rowCount())
    {
        this->setCurrentCell(_currentIndex, COL_NAME);
    }
}

// =============================================================================
// Color theming
// Mirrors Win32 ListView_SetBkColor / ListView_SetTextColor / NM_CUSTOMDRAW.
// =============================================================================

void VerticalFileSwitcherListView::setBackgroundColor(const QColor& bg)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, bg);
    pal.setColor(QPalette::AlternateBase, bg);
    setPalette(pal);
}

void VerticalFileSwitcherListView::setForegroundColor(const QColor& fg)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, fg);
    setPalette(pal);
}

// =============================================================================
// Event handlers
// Mirrors Win32 WM_MBUTTONUP, WM_KEYDOWN, NM_CLICK, NM_RCLICK.
// =============================================================================

void VerticalFileSwitcherListView::wheelEvent(QWheelEvent* event)
{
    // Pass wheel to scroll
    QTableWidget::wheelEvent(event);
}

void VerticalFileSwitcherListView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        QTableWidgetItem* it = itemAt(event->pos());
        if (it)
        {
            int row = it->row();
            int bufID = item(row, COL_NAME)->data(Qt::UserRole).toInt();
            int view = item(row, COL_NAME)->data(Qt::UserRole + 1).toInt();
            emit middleButtonClicked(bufID, view);
            event->accept();
            return;
        }
    }
    QTableWidget::mousePressEvent(event);
}

void VerticalFileSwitcherListView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        QList<QTableWidgetItem*> sel = selectedItems();
        if (!sel.isEmpty())
        {
            int row = sel.first()->row();
            auto key = std::make_pair(
                BufferID(reinterpret_cast<void*>(item(row, COL_NAME)->data(Qt::UserRole).toLongLong())),
                item(row, COL_NAME)->data(Qt::UserRole + 1).toInt()
            );
            auto it = _entries.find(key);
            if (it != _entries.end())
                emit fileActivated(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
        }
    }
    else if (event->key() == Qt::Key_Delete)
    {
        QList<QTableWidgetItem*> sel = selectedItems();
        for (const QTableWidgetItem* s : sel)
        {
            int row = s->row();
            auto key = std::make_pair(
                BufferID(reinterpret_cast<void*>(item(row, COL_NAME)->data(Qt::UserRole).toLongLong())),
                item(row, COL_NAME)->data(Qt::UserRole + 1).toInt()
            );
            auto it = _entries.find(key);
            if (it != _entries.end())
                emit fileClosed(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
        }
    }
    else
    {
        QTableWidget::keyPressEvent(event);
    }
}

void VerticalFileSwitcherListView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTableWidget::mouseDoubleClickEvent(event);
}

QStringList VerticalFileSwitcherListView::mimeTypes() const
{
    return QStringList();
}

void VerticalFileSwitcherListView::showContextMenu(const QPoint& pos)
{
    QMenu menu(this);

    menu.addAction(QStringLiteral("Open"), this, [this]() {
        auto sel = getSelectedBuffers();
        if (!sel.empty())
        {
            auto key = std::make_pair(
                BufferID(reinterpret_cast<void*>(static_cast<long long>(sel[0].first))),
                sel[0].second
            );
            auto it = _entries.find(key);
            if (it != _entries.end())
                emit fileActivated(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
        }
    });

    menu.addAction(QStringLiteral("Close"), this, [this]() {
        auto sel = getSelectedBuffers();
        for (auto& p : sel)
        {
            auto key = std::make_pair(
                BufferID(reinterpret_cast<void*>(static_cast<long long>(p.first))),
                p.second
            );
            auto it = _entries.find(key);
            if (it != _entries.end())
                emit fileClosed(static_cast<int>(reinterpret_cast<intptr_t>(it->second._bufID._p)), it->second._iView);
        }
    });

    menu.addSeparator();

    menu.addAction(QStringLiteral("Copy Path"), this, [this]() {
        auto sel = getSelectedBuffers();
        if (!sel.empty())
        {
            QString path = getFullFilePath(static_cast<size_t>(sel[0].first));
            QApplication::clipboard()->setText(path);
        }
    });

    menu.addAction(QStringLiteral("Show in Explorer"), this, [this]() {
        auto sel = getSelectedBuffers();
        if (!sel.empty())
        {
            QString path = getFullFilePath(static_cast<size_t>(sel[0].first));
            QFileInfo fi(path);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
        }
    });

    menu.exec(mapToGlobal(pos));
}

// =============================================================================
// VerticalFileSwitcher — constructor and initialization
// =============================================================================

VerticalFileSwitcher::VerticalFileSwitcher()
    : DockingDlgInterface(0)
{
    setWindowTitle(QStringLiteral("Document List"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(&_fileListView);
    setLayout(mainLayout);

    connect(&_fileListView, &VerticalFileSwitcherListView::fileActivated,
            this, &VerticalFileSwitcher::activateDoc);
    connect(&_fileListView, &VerticalFileSwitcherListView::fileClosed,
            this, &VerticalFileSwitcher::closeDoc);
    connect(&_fileListView, &VerticalFileSwitcherListView::middleButtonClicked,
            this, &VerticalFileSwitcher::closeDoc);

    initPopupMenus();
}

void VerticalFileSwitcher::init(QApplication*, QWidget*, void* hImgLst)
{
    _hImgLst = hImgLst;
    _fileListView.init(hImgLst);
    startColumnSort();
}

void VerticalFileSwitcher::display(bool toShow)
{
    DockingDlgInterface::display(toShow);
    if (toShow)
        const_cast<VerticalFileSwitcherListView&>(_fileListView).ensureVisibleCurrentItem();
}

// =============================================================================
// Popup menus
// Mirrors Win32 initPopupMenus() + popupMenuCmd().
// =============================================================================

void VerticalFileSwitcher::initPopupMenus()
{
    _contextMenu = new QMenu(this);

    _toggleExtAction = _contextMenu->addAction(QStringLiteral("Show/Hide Ext. Column"),
        this, [this]() {
            _withoutExtColumn = !_withoutExtColumn;
            _toggleExtAction->setChecked(!_withoutExtColumn);
            reload();
        });
    _toggleExtAction->setCheckable(true);
    _toggleExtAction->setChecked(!_withoutExtColumn);

    _togglePathAction = _contextMenu->addAction(QStringLiteral("Show/Hide Path Column"),
        this, [this]() {
            _withoutPathColumn = !_withoutPathColumn;
            _togglePathAction->setChecked(!_withoutPathColumn);
            reload();
        });
    _togglePathAction->setCheckable(true);
    _togglePathAction->setChecked(!_withoutPathColumn);

    _toggleGroupsAction = _contextMenu->addAction(QStringLiteral("Group by View"),
        this, [this]() {
            _disableListViewGroups = !_disableListViewGroups;
            _toggleGroupsAction->setChecked(!_disableListViewGroups);
            reload();
        });
    _toggleGroupsAction->setCheckable(true);
    _toggleGroupsAction->setChecked(!_disableListViewGroups);

    _contextMenu->addSeparator();

    _openAction = _contextMenu->addAction(QStringLiteral("Open"), this, [this]() {
        auto sel = _fileListView.getSelectedBuffers();
        if (!sel.empty())
            activateDoc(sel[0].first, sel[0].second);
    });

    _closeAction = _contextMenu->addAction(QStringLiteral("Close"), this, [this]() {
        auto sel = _fileListView.getSelectedBuffers();
        for (auto& p : sel)
            closeDoc(p.first, p.second);
    });

    _contextMenu->addSeparator();

    _copyPathAction = _contextMenu->addAction(QStringLiteral("Copy Path"), this, [this]() {
        auto sel = _fileListView.getSelectedBuffers();
        if (!sel.empty())
        {
            QString path = _fileListView.getFullFilePath(static_cast<size_t>(sel[0].first));
            QApplication::clipboard()->setText(path);
        }
    });

    _showInExplorerAction = _contextMenu->addAction(QStringLiteral("Show in Explorer"), this, [this]() {
        auto sel = _fileListView.getSelectedBuffers();
        if (!sel.empty())
        {
            QString path = _fileListView.getFullFilePath(static_cast<size_t>(sel[0].first));
            QFileInfo fi(path);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
        }
    });
}

void VerticalFileSwitcher::popupMenuCmd(int cmdID)
{
    switch (cmdID)
    {
    case CLMNEXT_ID:
        _withoutExtColumn = !_withoutExtColumn;
        reload();
        break;
    case CLMNPATH_ID:
        _withoutPathColumn = !_withoutPathColumn;
        reload();
        break;
    case LVGROUPS_ID:
        _disableListViewGroups = !_disableListViewGroups;
        reload();
        break;
    }
}

// =============================================================================
// Column sorting
// Mirrors Win32 LVM_SORTITEMSEx + setHeaderOrder + updateHeaderArrow.
// =============================================================================

void VerticalFileSwitcher::applySorting(int column)
{
    Qt::SortOrder order = (_lastSortingDirection == SORT_DIRECTION_DOWN)
        ? Qt::DescendingOrder : Qt::AscendingOrder;
    _fileListView.sortByColumn(column, order);
}

void VerticalFileSwitcher::startColumnSort()
{
    // Set initial sort from persisted settings (NppGUI defaults)
    _lastSortingColumn = 0;
    _lastSortingDirection = SORT_DIRECTION_UP;
    updateHeaderArrow();
    applySorting(_lastSortingColumn);
}

int VerticalFileSwitcher::setHeaderOrder(int columnIndex)
{
    // Toggle direction on same column, or switch column with UP
    if (columnIndex == _lastSortingColumn)
    {
        if (_lastSortingDirection == SORT_DIRECTION_UP)
            _lastSortingDirection = SORT_DIRECTION_DOWN;
        else if (_lastSortingDirection == SORT_DIRECTION_DOWN)
            _lastSortingDirection = SORT_DIRECTION_NONE;
        else
            _lastSortingDirection = SORT_DIRECTION_UP;
    }
    else
    {
        _lastSortingColumn = columnIndex;
        _lastSortingDirection = SORT_DIRECTION_UP;
    }

    updateHeaderArrow();
    return _lastSortingDirection;
}

void VerticalFileSwitcher::updateHeaderArrow()
{
    QHeaderView* hh = _fileListView.horizontalHeader();
    if (!hh)
        return;

    // Qt handles sort indicators natively via setSortIndicator
    hh->setSortIndicatorShown(true);
    hh->setSortIndicator(_lastSortingColumn,
        _lastSortingDirection == SORT_DIRECTION_DOWN ? Qt::DescendingOrder : Qt::AscendingOrder);
}

// =============================================================================
// File operations — emit signals to parent
// Mirrors Win32 NPPM_ACTIVATEDOC / NPPM_INTERNAL_CLOSEDOC.
// =============================================================================

void VerticalFileSwitcher::activateDoc(int bufferID, int iView) const
{
    emit const_cast<VerticalFileSwitcher*>(this)->fileActivated(bufferID, iView);
}

void VerticalFileSwitcher::closeDoc(int bufferID, int iView) const
{
    emit const_cast<VerticalFileSwitcher*>(this)->fileClosed(bufferID, iView);
}

// =============================================================================
// run_dlgProc — message dispatcher
// Mirrors Win32 VerticalFileSwitcher::run_dlgProc.
// =============================================================================

intptr_t VerticalFileSwitcher::run_dlgProc(unsigned int, intptr_t, intptr_t)
{
    return DockingDlgInterface::run_dlgProc(0, 0, 0);
}

void VerticalFileSwitcher::showEvent(QShowEvent* event)
{
    DockingDlgInterface::showEvent(event);
    if (!event->spontaneous()) {
        _fileListView.initList();
        _fileListView.reload();
        startColumnSort();
    }
}

void VerticalFileSwitcher::resizeEvent(QResizeEvent* event)
{
    DockingDlgInterface::resizeEvent(event);
    _fileListView.resizeColumns(event->size().width());
}

