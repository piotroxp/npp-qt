// Semantic Lift: Win32 VerticalFileSwitcherListView → Qt6 QTreeView
// Original:  PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.cpp
// Target:    npp-qt/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.cpp
//
// Win32 → Qt6 translation patterns:
//   ListView_SetItemState              → QItemSelectionModel::select()
//   ListView_EnsureVisible             → QTreeView::scrollTo(EnsureVisible)
//   ListView_RedrawItems               → viewport()->update()
//   ListView_GetItemCount              → total row count across all groups
//   ::PathFindFileName/Extension       → QFileInfo::fileName()/.suffix()
//   ::GetClientRect                     → parentWidget()->geometry()
//   LVITEM lParam                      → QStandardItem::data(Qt::UserRole).toLongLong()
//   ListView_InsertItem/SetItemText    → QStandardItemModel::appendRow/setItem
//   ListView_DeleteItem                → QStandardItem::removeRow
//   ListView_DeleteAllItems            → QStandardItemModel::clear()
//   ::SendMessage(hwnd,WM_SETREDRAW)   → setUpdatesEnabled(false/true)
//   LVGROUP (MAIN/SUB view)            → Non-selectable top-level QStandardItem roots

#include "VerticalFileSwitcherListView.h"
#include "Parameters.h"
#include "Notepad_plus.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QFont>
#include <QFileIconProvider>

// =============================================================================
// Helpers: count items across all groups, resolve icon status
// =============================================================================

namespace {

int countAllItemsInModel(QStandardItemModel* model)
{
    if (!model)
        return 0;
    int total = 0;
    QStandardItem* root = model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (groupRoot)
            total += groupRoot->rowCount();
    }
    return total;
}

} // anonymous namespace

// =============================================================================
// Construction / Destruction
// =============================================================================

VerticalFileSwitcherListView::VerticalFileSwitcherListView(QWidget* parent)
    : QTreeView(parent)
    , _model(nullptr)
    , _iconProvider(nullptr)
    , _rootActive(nullptr)
    , _rootInactive(nullptr)
    , _currentIndex(0)
    , _showExtColumn(true)
    , _showPathColumn(false)
    , _activeRootRowStart(0)
    , _inactiveRootRowStart(1)
{
}

VerticalFileSwitcherListView::~VerticalFileSwitcherListView()
{
    destroy();
}

// =============================================================================
// Lifecycle
// =============================================================================

void VerticalFileSwitcherListView::init(QWidget* parent)
{
    setParent(parent);

    // --- Model: 3 columns (Name, Ext, Path) ---
    _model = new QStandardItemModel(0, 3, this);
    _model->setHeaderData(FS_COL_NAME, Qt::Horizontal, QStringLiteral("Name"));
    _model->setHeaderData(FS_COL_EXT,  Qt::Horizontal, QStringLiteral("Ext."));
    _model->setHeaderData(FS_COL_PATH, Qt::Horizontal, QStringLiteral("Path"));
    setModel(_model);

    // --- File icon provider ---
    _iconProvider = new QFileIconProvider();

    // --- Header ---
    setHeaderHidden(false);
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setSortIndicatorShown(false);

    // --- Row appearance ---
    setAlternatingRowColors(false);
    setRootIsDecorated(true);          // show expand/collapse arrows for group roots
    setItemsExpandable(true);
    setExpandsOnDoubleClick(false);    // double-click = file activation, not expand

    // Selection: single row, full row
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // No drag — this is a display panel, not a source
    setDragEnabled(false);
    setAcceptDrops(false);
    setDropIndicatorShown(false);

    // No cell editing
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Smooth per-pixel scrolling
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // No word-wrap
    setWordWrap(false);

    // Frameless (border comes from parent docking panel)
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);

    // Context menu handled manually
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Hook up selection model for single-click selection
    connect(selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() { viewport()->update(); });

    // Hook up custom context menu signal
    connect(this, &QWidget::customContextMenuRequested,
            this, &VerticalFileSwitcherListView::contextMenuEvent);

    // Double-click → activated signal → open file
    connect(this, &QAbstractItemView::activated,
            this, [this](const QModelIndex& index) {
                Q_UNUSED(index);
                emit doubleClicked(index);
            });
}

void VerticalFileSwitcherListView::destroy()
{
    if (!_model)
        return;

    // Walk every file item and delete its owned TaskLstFnStatus*
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            if (ptr != 0) {
                auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                delete tlfs;
            }
        }
    }

    delete _iconProvider;
    _iconProvider = nullptr;

    _model->clear();
    _model->deleteLater();
    _model = nullptr;

    _rootActive   = nullptr;
    _rootInactive = nullptr;
}

// =============================================================================
// DPI helpers (mirrors DPIManagerV2::scale / getDpiForWindow)
// =============================================================================

int VerticalFileSwitcherListView::dpiForWidget(QWidget* w)
{
    return w ? qRound(96.0 * w->devicePixelRatio()) : 96;
}

int VerticalFileSwitcherListView::scaleValue(int value, int dpi)
{
    return qRound(static_cast<qreal>(value) * (dpi > 0 ? dpi : 96) / 96.0);
}

// =============================================================================
// Internal: helpers
// =============================================================================

QStandardItem* VerticalFileSwitcherListView::pathToGroup(int iView) const
{
    return (iView == MAIN_VIEW) ? _rootActive : _rootInactive;
}

QStandardItem* VerticalFileSwitcherListView::itemFromBuffer(BufferID bufferID, int iView) const
{
    if (!_model)
        return nullptr;
    QStandardItem* groupRoot = pathToGroup(iView);
    if (!groupRoot)
        return nullptr;
    for (int r = 0; r < groupRoot->rowCount(); ++r) {
        QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
        if (!nameItem)
            continue;
        qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
        auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
        if (tlfs && tlfs->_bufID == bufferID.get())
            return nameItem;
    }
    return nullptr;
}

int VerticalFileSwitcherListView::resolveIconStatus(Buffer* buf) const
{
    if (!buf)
        return FS_ICON_NORMAL;
    if (buf->isMonitoringOn())
        return FS_ICON_MONITORING;
    if (buf->isReadOnly())
        return FS_ICON_READONLY;
    if (buf->isDirty())
        return FS_ICON_DIRTY;
    return FS_ICON_NORMAL;
}

// =============================================================================
// Model population: initList / reload / fetchTaskListInfo
// =============================================================================

void VerticalFileSwitcherListView::fetchTaskListInfo(TaskListInfo&)
{
    // Protected virtual stub: subclasses (VerticalFileSwitcher) override this
    // to call Notepad_plus::getTaskListInfo().  The base class does nothing —
    // callers must ensure the subclass is used, or initList() will leave the
    // list empty.
}

void VerticalFileSwitcherListView::initList()
{
    NppParameters& nppParams = NppParameters::getInstance();
    const NppGUI& nppGUI = nppParams.getNppGUI();

    _showExtColumn  = !nppGUI._fileSwitcherWithoutExtColumn;
    _showPathColumn = !nppGUI._fileSwitcherWithoutPathColumn;

    int dpi = dpiForWidget(this);
    const int extWidthDyn  = scaleValue(nppGUI._fileSwitcherExtWidth,  dpi);
    const int pathWidthDyn = scaleValue(nppGUI._fileSwitcherPathWidth, dpi);

    // Available width for the Name column
    QWidget* parentW = parentWidget();
    int totalWidth = (parentW && parentW->width() > 0) ? parentW->width() : 400;

    int nameWidth = totalWidth;
    if (_showExtColumn)
        nameWidth -= extWidthDyn;
    if (_showPathColumn)
        nameWidth -= pathWidthDyn;

    // --- Localised column labels ---
    QString nameLabel = nppParams.getNativeLangSpeaker()
        ? nppParams.getNativeLangSpeaker()->getAttrNameStr(
              QStringLiteral("Name"), QStringLiteral("DocList"), QStringLiteral("ColumnName"))
        : QStringLiteral("Name");
    QString extLabel = nppParams.getNativeLangSpeaker()
        ? nppParams.getNativeLangSpeaker()->getAttrNameStr(
              QStringLiteral("Ext."), QStringLiteral("DocList"), QStringLiteral("ColumnExt"))
        : QStringLiteral("Ext.");
    QString pathLabel = nppParams.getNativeLangSpeaker()
        ? nppParams.getNativeLangSpeaker()->getAttrNameStr(
              QStringLiteral("Path"), QStringLiteral("DocList"), QStringLiteral("ColumnPath"))
        : QStringLiteral("Path");

    // --- Add columns with widths ---
    insertColumn(nameLabel, qMax(nameWidth, 20), FS_COL_NAME);
    if (_showExtColumn)
        insertColumn(extLabel, extWidthDyn, FS_COL_EXT);
    else
        hideColumn(FS_COL_EXT);
    if (_showPathColumn)
        insertColumn(pathLabel, pathWidthDyn, FS_COL_PATH);
    else
        hideColumn(FS_COL_PATH);

    // --- Create group-root items ---
    _rootActive   = new QStandardItem(QStringLiteral("Active Files"));
    _rootInactive = new QStandardItem(QStringLiteral("Inactive Files"));

    _rootActive->setSelectable(false);
    _rootInactive->setSelectable(false);
    _rootActive->setEditable(false);
    _rootInactive->setEditable(false);

    QFont boldFont = QApplication::font();
    boldFont.setBold(true);
    _rootActive->setFont(boldFont);
    _rootInactive->setFont(boldFont);

    _rootActive->setData(FS_GROUP_ACTIVE,    Qt::UserRole + 1);
    _rootInactive->setData(FS_GROUP_INACTIVE, Qt::UserRole + 1);

    _model->appendRow(_rootActive);
    _model->appendRow(_rootInactive);

    // Expand both groups so the file list is immediately visible
    setExpanded(_model->indexFromItem(_rootActive),   true);
    setExpanded(_model->indexFromItem(_rootInactive), true);

    _activeRootRowStart   = 0;
    _inactiveRootRowStart = 1;

    // --- Fetch document list from parent ---
    TaskListInfo taskListInfo;
    fetchTaskListInfo(taskListInfo);

    // --- Populate file entries ---
    for (size_t i = 0; i < taskListInfo._tlfsLst.size(); ++i) {
        const TaskLstFnStatus& src = taskListInfo._tlfsLst[i];

        // Ownership: create a clone that lives in the model
        auto* tl = new TaskLstFnStatus(src);

        QString fullPath = src._fn;
        QFileInfo fi(fullPath);
        QString fileName = fi.fileName();
        QString extension = fi.suffix();
        if (!extension.isEmpty())
            extension = QStringLiteral(".") + extension;
        QString dirPath = fi.path();

        // If extension column is hidden, strip extension from display name
        if (!_showExtColumn && !fileName.isEmpty()) {
            int dot = fileName.lastIndexOf(QLatin1Char('.'));
            if (dot > 0)
                fileName = fileName.left(dot);
        }

        // Resolve file-type icon
        QPixmap iconPix;
        if (_iconProvider && !fullPath.isEmpty() && fi.exists()) {
            iconPix = _iconProvider->icon(fi).pixmap(16, 16);
        }

        // Determine which view group
        QStandardItem* group = (src._iView == MAIN_VIEW) ? _rootActive : _rootInactive;

        // Build 3-column row; UserRole (TaskLstFnStatus*) lives in the name column
        QList<QStandardItem*> row;
        QStandardItem* nameItem = new QStandardItem();
        nameItem->setEditable(false);
        nameItem->setSelectable(true);
        nameItem->setData(reinterpret_cast<qintptr>(tl), Qt::UserRole);
        nameItem->setText(fileName);
        nameItem->setToolTip(fullPath);
        if (!iconPix.isNull())
            nameItem->setIcon(QIcon(iconPix));

        QStandardItem* extItem  = new QStandardItem();
        extItem->setEditable(false);
        extItem->setSelectable(true);
        extItem->setText(extension);
        extItem->setToolTip(extension);

        QStandardItem* pathItem = new QStandardItem();
        pathItem->setEditable(false);
        pathItem->setSelectable(true);
        pathItem->setText(dirPath);
        pathItem->setToolTip(dirPath);

        row << nameItem << extItem << pathItem;
        group->appendRow(row);
    }

    // --- Restore selection state ---
    _currentIndex = taskListInfo._currentIndex;
    selectCurrentItem();
    ensureVisibleCurrentItem();

    // Apply dark/light mode colours via NppDarkMode singleton
    if (NppDarkMode::instance().isEnabled()) {
        QPalette pal = palette();
        pal.setColor(QPalette::Base, QColor::fromRgb(NppDarkMode::instance().backgroundColor()));
        pal.setColor(QPalette::Text, QColor::fromRgb(NppDarkMode::instance().textColor()));
        pal.setColor(QPalette::HighlightedText, QColor::fromRgb(NppDarkMode::instance().textColor()));
        pal.setColor(QPalette::Highlight, QColor::fromRgb(NppDarkMode::instance().hotBackgroundColor()));
        setPalette(pal);
        viewport()->setPalette(pal);
    }
}

void VerticalFileSwitcherListView::reload()
{
    // Suppress redraws for the duration of the reload
    setUpdatesEnabled(false);

    removeAll();
    initList();

    // Re-apply column widths after the parent has been resized
    QWidget* parentW = parentWidget();
    int totalWidth = (parentW && parentW->width() > 0) ? parentW->width() : 400;
    if (totalWidth > 0)
        resizeColumns(totalWidth);

    setUpdatesEnabled(true);
    redrawItems();
}

// =============================================================================
// Redraw
// =============================================================================

void VerticalFileSwitcherListView::redrawItems()
{
    viewport()->update();  // mirrors ListView_RedrawItems
}

// =============================================================================
// Buffer operations
// =============================================================================

BufferID VerticalFileSwitcherListView::getBufferInfoFromIndex(int index, int& view) const
{
    if (!_model || index < 0)
        return BUFFER_INVALID;

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        int rows = groupRoot->rowCount();
        if (index < itemCount + rows) {
            int rowInGroup = index - itemCount;
            QStandardItem* nameItem = groupRoot->child(rowInGroup, FS_COL_NAME);
            if (!nameItem)
                return BUFFER_INVALID;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
            if (!tlfs)
                return BUFFER_INVALID;
            view = tlfs->_iView;
            return BufferID(tlfs->_bufID);
        }
        itemCount += rows;
    }
    return BUFFER_INVALID;
}

int VerticalFileSwitcherListView::newItem(BufferID bufferID, int iView)
{
    int existing = find(bufferID, iView);
    return (existing != -1) ? existing : add(bufferID, iView);
}

void VerticalFileSwitcherListView::setItemIconStatus(BufferID bufferID)
{
    if (!_model || !bufferID)
        return;

    Buffer* buf = bufferID;
    QString fullPath = QString::fromWCharArray(buf->getFullPathName());
    QFileInfo fi(fullPath);

    QString fileName = fi.fileName();
    QString extension;
    if (!fi.suffix().isEmpty())
        extension = QStringLiteral(".") + fi.suffix();
    QString dirPath = fi.path();

    if (!_showExtColumn && !fileName.isEmpty()) {
        int dot = fileName.lastIndexOf(QLatin1Char('.'));
        if (dot > 0)
            fileName = fileName.left(dot);
    }

    // Find the row for this buffer and update it
    for (int g = 0; g < _model->invisibleRootItem()->rowCount(); ++g) {
        QStandardItem* groupRoot = _model->invisibleRootItem()->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
            if (!tlfs || tlfs->_bufID != bufferID.get())
                continue;

            // Update stored path in the TaskLstFnStatus
            tlfs->_fn = fullPath;

            // Update name column
            nameItem->setText(fileName);
            nameItem->setToolTip(fullPath);
            if (_iconProvider && fi.exists()) {
                QPixmap pix = _iconProvider->icon(fi).pixmap(16, 16);
                if (!pix.isNull())
                    nameItem->setIcon(QIcon(pix));
            }

            // Update extension column
            if (_showExtColumn) {
                QStandardItem* extItem = groupRoot->child(r, FS_COL_EXT);
                if (extItem)
                    extItem->setText(extension);
            }

            // Update path column
            if (_showPathColumn) {
                QStandardItem* pathItem = groupRoot->child(r, FS_COL_PATH);
                if (pathItem)
                    pathItem->setText(dirPath);
            }

            viewport()->update();
            return;
        }
    }
}

void VerticalFileSwitcherListView::setItemColor(BufferID bufferID)
{
    if (!_model || !bufferID)
        return;

    Buffer* buf = bufferID;
    int docColor = buf->getDocColorId();

    for (int g = 0; g < _model->invisibleRootItem()->rowCount(); ++g) {
        QStandardItem* groupRoot = _model->invisibleRootItem()->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
            if (!tlfs || tlfs->_bufID != bufferID.get())
                continue;

            tlfs->_docColor = docColor;
            viewport()->update();
            return;
        }
    }
}

QString VerticalFileSwitcherListView::getFullFilePath(size_t index) const
{
    if (!_model || index < 0)
        return QString();

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        int rows = groupRoot->rowCount();
        if (size_t(index) < size_t(itemCount + rows)) {
            int rowInGroup = static_cast<int>(index) - itemCount;
            QStandardItem* nameItem = groupRoot->child(rowInGroup, FS_COL_NAME);
            if (!nameItem)
                return QString();
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
            return tlfs ? tlfs->_fn : QString();
        }
        itemCount += rows;
    }
    return QString();
}

int VerticalFileSwitcherListView::closeItem(BufferID bufferID, int iView)
{
    int idx = find(bufferID, iView);
    if (idx != -1)
        remove(idx, true);
    return idx;
}

void VerticalFileSwitcherListView::activateItem(BufferID bufferID, int iView)
{
    // Suppress redraws while resetting all states
    setUpdatesEnabled(false);

    // Clear every selection
    clearAllSelections();

    // Add the item if not already in the list
    _currentIndex = newItem(bufferID, iView);

    // Re-enable drawing
    setUpdatesEnabled(true);

    selectCurrentItem();
    ensureVisibleCurrentItem();
    redrawItems();
}

// =============================================================================
// Internal: add / remove / find
// =============================================================================

int VerticalFileSwitcherListView::add(BufferID bufferID, int iView)
{
    if (!_model || !bufferID)
        return -1;

    Buffer* buf = bufferID;
    QString fullPath = QString::fromWCharArray(buf->getFullPathName());
    QFileInfo fi(fullPath);

    QString fileName = fi.fileName();
    QString extension;
    if (!fi.suffix().isEmpty())
        extension = QStringLiteral(".") + fi.suffix();
    QString dirPath = fi.path();

    if (!_showExtColumn && !fileName.isEmpty()) {
        int dot = fileName.lastIndexOf(QLatin1Char('.'));
        if (dot > 0)
            fileName = fileName.left(dot);
    }

    // Owning clone of TaskLstFnStatus
    auto* tl = new TaskLstFnStatus(iView, 0, fullPath, 0, bufferID.get(), -1);

    // File icon
    QPixmap iconPix;
    if (_iconProvider && !fullPath.isEmpty() && fi.exists()) {
        iconPix = _iconProvider->icon(fi).pixmap(16, 16);
    }

    QStandardItem* group = pathToGroup(iView);
    if (!group)
        group = _rootActive;

    QStandardItem* nameItem = new QStandardItem();
    nameItem->setEditable(false);
    nameItem->setSelectable(true);
    nameItem->setData(reinterpret_cast<qintptr>(tl), Qt::UserRole);
    nameItem->setText(fileName);
    nameItem->setToolTip(fullPath);
    if (!iconPix.isNull())
        nameItem->setIcon(QIcon(iconPix));

    QStandardItem* extItem = new QStandardItem();
    extItem->setEditable(false);
    extItem->setSelectable(true);
    extItem->setText(extension);

    QStandardItem* pathItem = new QStandardItem();
    pathItem->setEditable(false);
    pathItem->setSelectable(true);
    pathItem->setText(dirPath);

    group->appendRow(QList<QStandardItem*>() << nameItem << extItem << pathItem);

    // _currentIndex = total items before this new one was added
    _currentIndex = countAllItemsInModel(_model);

    selectCurrentItem();
    viewport()->update();

    return _currentIndex;
}

void VerticalFileSwitcherListView::remove(int index, bool removeFromModel)
{
    if (!_model || index < 0)
        return;

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        int rows = groupRoot->rowCount();
        if (index < itemCount + rows) {
            int rowInGroup = index - itemCount;
            QStandardItem* nameItem = groupRoot->child(rowInGroup, FS_COL_NAME);
            if (nameItem) {
                qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
                if (ptr != 0) {
                    auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                    delete tlfs;
                }
            }
            if (removeFromModel)
                groupRoot->removeRow(rowInGroup);
            return;
        }
        itemCount += rows;
    }
}

void VerticalFileSwitcherListView::removeAll()
{
    if (!_model)
        return;

    // Delete every owned TaskLstFnStatus* and all rows
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            if (ptr != 0) {
                auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                delete tlfs;
            }
        }
        groupRoot->removeRows(0, groupRoot->rowCount());
    }

    // clear() wipes all rows and headers
    _model->clear();

    // Re-insert empty group roots
    delete _rootActive;
    delete _rootInactive;
    _rootActive   = new QStandardItem(QStringLiteral("Active Files"));
    _rootInactive = new QStandardItem(QStringLiteral("Inactive Files"));
    _rootActive->setSelectable(false);
    _rootInactive->setSelectable(false);
    _rootActive->setEditable(false);
    _rootInactive->setEditable(false);
    QFont boldFont = QApplication::font();
    boldFont.setBold(true);
    _rootActive->setFont(boldFont);
    _rootInactive->setFont(boldFont);
    _rootActive->setData(FS_GROUP_ACTIVE,    Qt::UserRole + 1);
    _rootInactive->setData(FS_GROUP_INACTIVE, Qt::UserRole + 1);

    // Re-create headers (clear() nuked them)
    _model->setHeaderData(FS_COL_NAME, Qt::Horizontal, QStringLiteral("Name"));
    _model->setHeaderData(FS_COL_EXT,  Qt::Horizontal, QStringLiteral("Ext."));
    _model->setHeaderData(FS_COL_PATH, Qt::Horizontal, QStringLiteral("Path"));

    _model->appendRow(_rootActive);
    _model->appendRow(_rootInactive);
    setExpanded(_model->indexFromItem(_rootActive),   true);
    setExpanded(_model->indexFromItem(_rootInactive), true);

    _currentIndex = 0;
}

int VerticalFileSwitcherListView::find(BufferID bufferID, int iView) const
{
    if (!_model)
        return -1;

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
            auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
            if (tlfs && tlfs->_bufID == bufferID.get() && tlfs->_iView == iView)
                return itemCount;
            ++itemCount;
        }
    }
    return -1;
}

// =============================================================================
// Column management
// =============================================================================

void VerticalFileSwitcherListView::insertColumn(const QString& name, int width, int index)
{
    if (!_model)
        return;
    // Ensure there are enough columns
    while (_model->columnCount() <= index)
        _model->insertColumn(_model->columnCount());
    _model->setHeaderData(index, Qt::Horizontal, name);
    if (header())
        header()->resizeSection(index, qMax(width, 10));
}

void VerticalFileSwitcherListView::resizeColumns(int totalWidth)
{
    if (!_model || totalWidth <= 0)
        return;

    NppParameters& nppParams = NppParameters::getInstance();
    const NppGUI& nppGUI = nppParams.getNppGUI();

    int dpi = dpiForWidget(this);
    const int extWidthDyn  = scaleValue(nppGUI._fileSwitcherExtWidth,  dpi);
    const int pathWidthDyn = scaleValue(nppGUI._fileSwitcherPathWidth, dpi);

    int totalExceptName = 0;
    int col = 0;  // tracks the current column for resize (0=name already set)

    if (_showExtColumn) {
        totalExceptName += extWidthDyn;
        header()->resizeSection(++col, extWidthDyn);
    }
    if (_showPathColumn) {
        totalExceptName += pathWidthDyn;
        header()->resizeSection(++col, pathWidthDyn);
    }

    // If a vertical scrollbar is (or will be) visible, account for its width
    if (verticalScrollBar()->isVisible())
        totalExceptName += verticalScrollBar()->width();

    header()->resizeSection(FS_COL_NAME, qMax(totalWidth - totalExceptName, 10));
}

void VerticalFileSwitcherListView::deleteColumn(size_t /*index*/)
{
    // Not currently called from the npp-qt codebase; stub for API parity.
    // In Qt6 QTreeView you cannot delete individual columns — the model
    // always has all 3 columns; visibility is controlled by hideColumn().
}

// =============================================================================
// Selection helpers
// =============================================================================

void VerticalFileSwitcherListView::selectCurrentItem() const
{
    if (!_model || _currentIndex < 0)
        return;

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        int rows = groupRoot->rowCount();
        if (_currentIndex < itemCount + rows) {
            int rowInGroup = _currentIndex - itemCount;
            QModelIndex idx = _model->index(rowInGroup, FS_COL_NAME, groupRoot->index());
            selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect
                                               | QItemSelectionModel::Rows);
            return;
        }
        itemCount += rows;
    }
}

void VerticalFileSwitcherListView::clearAllSelections()
{
    if (selectionModel())
        selectionModel()->clearSelection();
}

void VerticalFileSwitcherListView::ensureVisibleCurrentItem() const
{
    if (!_model || _currentIndex < 0)
        return;

    int itemCount = 0;
    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        int rows = groupRoot->rowCount();
        if (_currentIndex < itemCount + rows) {
            int rowInGroup = _currentIndex - itemCount;
            QModelIndex idx = _model->index(rowInGroup, FS_COL_NAME, groupRoot->index());
            scrollTo(idx, QAbstractItemView::EnsureVisible);
            return;
        }
        itemCount += rows;
    }
}

int VerticalFileSwitcherListView::nbSelectedFiles() const
{
    if (!selectionModel())
        return 0;
    return selectionModel()->selectedRows(FS_COL_NAME).count();
}

std::vector<BufferViewInfo> VerticalFileSwitcherListView::getSelectedFiles(bool reverse) const
{
    std::vector<BufferViewInfo> result;
    if (!_model)
        return result;

    QItemSelectionModel* sel = selectionModel();
    int itemCount = 0;

    QStandardItem* root = _model->invisibleRootItem();
    for (int g = 0; g < root->rowCount(); ++g) {
        QStandardItem* groupRoot = root->child(g);
        if (!groupRoot)
            continue;
        for (int r = 0; r < groupRoot->rowCount(); ++r) {
            QStandardItem* nameItem = groupRoot->child(r, FS_COL_NAME);
            if (!nameItem)
                continue;
            QModelIndex idx = _model->index(r, FS_COL_NAME, groupRoot->index());
            bool isSelected = sel && sel->isSelected(idx);
            bool chosen = reverse ? !isSelected : isSelected;
            if (chosen) {
                qintptr ptr = nameItem->data(Qt::UserRole).toLongLong();
                auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                if (tlfs)
                    result.emplace_back(BufferID(tlfs->_bufID), tlfs->_iView);
            }
            ++itemCount;
        }
    }

    return result;
}

// =============================================================================
// Colour theming (NppDarkMode integration)
// =============================================================================

void VerticalFileSwitcherListView::setBackgroundColor(const QColor& bg)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, bg);
    setPalette(pal);
    viewport()->setPalette(pal);
    redrawItems();
}

void VerticalFileSwitcherListView::setForegroundColor(const QColor& fg)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, fg);
    pal.setColor(QPalette::WindowText, fg);
    setPalette(pal);
    redrawItems();
}

// =============================================================================
// Event handlers
// =============================================================================

bool VerticalFileSwitcherListView::event(QEvent* event)
{
    // Translate mouse double-click on a file row into a doubleClicked signal.
    // This mirrors the Win32 LVN_ITEMACTIVATE / NM_DBLCLK notification.
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        QModelIndex idx = indexAt(me->pos());
        if (idx.isValid()) {
            // Only emit for file rows, not group roots
            QStandardItem* item = _model->itemFromIndex(idx);
            if (item && item->data(Qt::UserRole + 1).toInt() == 0) {
                // Not a group root (group roots store their ID in UserRole+1)
                emit doubleClicked(idx);
            }
        }
    }
    return QTreeView::event(event);
}

void VerticalFileSwitcherListView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex idx = indexAt(event->pos());

    // If right-clicked on a group root, just ignore
    if (!idx.isValid()) {
        event->ignore();
        return;
    }

    QStandardItem* item = _model ? _model->itemFromIndex(idx) : nullptr;
    if (!item)
        return;

    // Ignore right-click on group roots
    if (item->data(Qt::UserRole + 1).toInt() != 0)
        return;  // is a group root

    // Select the right-clicked row
    selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    // Find the TaskLstFnStatus* for the right-clicked item
    qintptr ptr = item->data(Qt::UserRole).toLongLong();
    auto* clickedTlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);

    // Build context menu
    QMenu menu(this);

    QAction* actClose = menu.addAction(QStringLiteral("Close"));
    QAction* actCloseAll = menu.addAction(QStringLiteral("Close All"));
    QAction* actCloseOthers = menu.addAction(QStringLiteral("Close Other Files"));

    // Disable "Close Others" if there are ≤1 file items
    int totalItems = countAllItemsInModel(_model);
    actCloseOthers->setEnabled(totalItems > 1);

    QAction* chosen = menu.exec(event->globalPos());
    if (!chosen)
        return;

    if (chosen == actClose) {
        if (clickedTlfs && clickedTlfs->_bufID) {
            emit requestClose(BufferID(clickedTlfs->_bufID), clickedTlfs->_iView);
        }
    } else if (chosen == actCloseAll) {
        emit requestCloseAll();
    } else if (chosen == actCloseOthers) {
        if (!clickedTlfs || !clickedTlfs->_bufID)
            return;
        BufferID keepId(clickedTlfs->_bufID);
        int keepView = clickedTlfs->_iView;
        emit requestCloseOthers(keepId, keepView);
    }
}

void VerticalFileSwitcherListView::mousePressEvent(QMouseEvent* event)
{
    // Middle-mouse-button on a file row → close that file (mirrors Win32
    // MK_MBUTTON handling in the list-view subclass WindowProc).
    if (event->button() == Qt::MiddleButton) {
        QModelIndex idx = indexAt(event->pos());
        if (idx.isValid()) {
            QStandardItem* item = _model ? _model->itemFromIndex(idx) : nullptr;
            if (item) {
                // Skip group roots
                if (item->data(Qt::UserRole + 1).toInt() == 0) {
                    qintptr ptr = item->data(Qt::UserRole).toLongLong();
                    auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                    if (tlfs && tlfs->_bufID) {
                        emit requestClose(BufferID(tlfs->_bufID), tlfs->_iView);
                        event->accept();
                        return;
                    }
                }
            }
        }
    }
    QTreeView::mousePressEvent(event);
}

void VerticalFileSwitcherListView::mouseDoubleClickEvent(QMouseEvent* event)
{
    // Double-click on a file row → open/activate that file.
    // We check before the default QTreeView handler so we can emit the right signal.
    if (event->button() == Qt::LeftButton) {
        QModelIndex idx = indexAt(event->pos());
        if (idx.isValid()) {
            QStandardItem* item = _model ? _model->itemFromIndex(idx) : nullptr;
            if (item) {
                // Group roots: expand/collapse (default QTreeView behaviour)
                if (item->data(Qt::UserRole + 1).toInt() != 0) {
                    QTreeView::mouseDoubleClickEvent(event);
                    return;
                }
                // File rows: emit activation signal
                qintptr ptr = item->data(Qt::UserRole).toLongLong();
                auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
                if (tlfs && tlfs->_bufID) {
                    emit fileActivated(BufferID(tlfs->_bufID), tlfs->_iView);
                    event->accept();
                    return;
                }
            }
        }
    }
    QTreeView::mouseDoubleClickEvent(event);
}

// =============================================================================
// Context menu action handlers (called by VerticalFileSwitcher)
// =============================================================================

void VerticalFileSwitcherListView::onCloseFile()
{
    // Called by parent (VerticalFileSwitcher) when the context-menu "Close"
    // action is triggered.  Uses the current selection.
    QModelIndexList selected = selectionModel()->selectedRows(FS_COL_NAME);
    if (selected.isEmpty())
        return;
    QStandardItem* item = _model->itemFromIndex(selected.first());
    if (!item)
        return;
    qintptr ptr = item->data(Qt::UserRole).toLongLong();
    auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
    if (tlfs && tlfs->_bufID)
        emit requestClose(BufferID(tlfs->_bufID), tlfs->_iView);
}

void VerticalFileSwitcherListView::onCloseAll()
{
    emit requestCloseAll();
}

void VerticalFileSwitcherListView::onCloseOthers()
{
    // Close all files except the current selection
    QModelIndexList selected = selectionModel()->selectedRows(FS_COL_NAME);
    if (selected.isEmpty())
        return;
    QStandardItem* item = _model->itemFromIndex(selected.first());
    if (!item)
        return;
    qintptr ptr = item->data(Qt::UserRole).toLongLong();
    auto* tlfs = reinterpret_cast<TaskLstFnStatus*>(ptr);
    if (!tlfs || !tlfs->_bufID)
        return;
    emit requestCloseOthers(BufferID(tlfs->_bufID), tlfs->_iView);
}
