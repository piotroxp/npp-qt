// Semantic Lift: Win32 TaskList → Qt6 implementation
// Original: PowerEditor/src/WinControls/TaskList/TaskList.cpp (291 lines)
// Target: npp-qt/src/WinControls/TaskList.cpp

#include "TaskList.h"
#include "DockingWnd.h"
#include "NppDarkMode.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>
#include <QPainter>
#include <QStyleOption>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScreen>

// =============================================================================
// TaskListDelegate — owner-draw item delegate
// Mirrors Win32 LVS_OWNERDRAWFIXED + WM_DRAWITEM.
// =============================================================================

void TaskListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    bool isDark = NppDarkMode::instance().isEnabled();
    bool isSelected = (opt.state & QStyle::StateFlag::State_Selected) != 0;

    // Background
    QColor bgColor;
    if (isSelected) {
        bgColor = isDark
            ? NppDarkMode::instance().ctrlBackgroundColor()
            : QColor(0, 120, 212);
    } else {
        bgColor = isDark
            ? NppDarkMode::instance().backgroundColor()
            : QColor(255, 255, 224); // lightYellow from Win32 colors.h
    }
    painter->fillRect(opt.rect, bgColor);

    // Icon
    int iconIndex = index.data(Qt::UserRole + 1).toInt();
    QIcon icon;
    if (iconIndex == TASKLIST_ICON_UNICODE)
        icon = QIcon::fromTheme("text-x-generic", QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    else if (iconIndex == TASKLIST_ICON_ANSI)
        icon = QIcon::fromTheme("text-plain", QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    else if (iconIndex == TASKLIST_ICON_DIRTY)
        icon = QIcon::fromTheme("document-modified", QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    else
        icon = QIcon::fromTheme("emblem-symbolic-link", QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon));

    QRect iconRect = opt.rect.adjusted(4, 0, 0, 0);
    iconRect.setWidth(16);
    icon.paint(painter, iconRect, Qt::AlignVCenter);

    // Text
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = opt.rect.adjusted(4 + 16, 0, -4, 0);

    QColor textColor;
    if (isSelected) {
        textColor = isDark
            ? NppDarkMode::instance().textColor()
            : Qt::white;
    } else {
        textColor = isDark
            ? NppDarkMode::instance().darkerTextColor()
            : QColor(0x22, 0x22, 0x22);
    }

    painter->setPen(textColor);
    painter->setFont(isSelected
        ? QFont("Segoe UI", 9, QFont::Bold)
        : QFont("Segoe UI", 9));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
}

QSize TaskListDelegate::sizeHint(const QStyleOptionViewItem&,
                                  const QModelIndex&) const
{
    return QSize(200, 20);
}

// =============================================================================
// TaskList — constructor
// =============================================================================

TaskList::TaskList(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _listWidget = new QListWidget(this);
    _listWidget->setItemDelegate(new TaskListDelegate(this));
    _listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    _listWidget->setUniformItemSizes(true);
    _listWidget->setFrameShape(QFrame::NoFrame);
    _listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _listWidget->setAlternatingRowColors(false);
    _listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connections
    connect(_listWidget, &QListWidget::itemClicked,
            this, &TaskList::onItemClicked);
    connect(_listWidget, &QListWidget::itemDoubleClicked,
            this, &TaskList::onItemDoubleClicked);
    connect(_listWidget->selectionModel(), &QSelectionModel::selectionChanged,
            this, &TaskList::onItemSelectionChanged);

    layout->addWidget(_listWidget);
    setLayout(layout);
    setFont(10);
}

TaskList::~TaskList()
{
    destroyFont();
}

// =============================================================================
// init — mirrors Win32 TaskList::init()
// =============================================================================

void TaskList::init(QApplication*, QWidget* parent, void*, int nbItem, int index2set)
{
    Q_UNUSED(parent);
    _currentIndex = index2set;
    _nbItem = nbItem;

    // setItemCount clears + repopulates items, then sets current row
    setItemCount(nbItem);
}

// =============================================================================
// destroy — mirrors Win32 TaskList::destroy()
// =============================================================================

void TaskList::destroy()
{
    destroyFont();
    delete _listWidget;
    _listWidget = nullptr;
}

// =============================================================================
// setItems — populate the list from TaskListInfo
// Mirrors Win32 LVN_GETDISPINFO: each row gets its text + icon from _tlfsLst.
// =============================================================================

void TaskList::setItems(const TaskListInfo& info)
{
    _listWidget->clear();
    _nbItem = static_cast<int>(info._tlfsLst.size());

    for (int i = 0; i < _nbItem; ++i)
    {
        const TaskLstFnStatus& item = info._tlfsLst[i];
        auto* listItem = new QListWidgetItem(item._fn, _listWidget);
        listItem->setData(Qt::UserRole, i);          // Document index
        listItem->setData(Qt::UserRole + 1, item._status); // Icon status
        listItem->setSizeHint(QSize(200, 20));
    }

    // Set current index AFTER items are populated
    if (info._currentIndex >= 0 && info._currentIndex < _nbItem)
        setCurrentIndex(info._currentIndex);
}

// =============================================================================
// setItemCount — set LVM_SETITEMCOUNT equivalent
// Creates placeholder items with icon + text.
// Mirrors Win32 ListView_SetItemCount.
// =============================================================================

void TaskList::setItemCount(int nbItem)
{
    _nbItem = nbItem;
    _listWidget->clear();
    for (int i = 0; i < nbItem; ++i)
    {
        auto* item = new QListWidgetItem(_listWidget);
        item->setData(Qt::UserRole, i);         // Document index
        item->setData(Qt::UserRole + 1, 0);     // Icon status (default)
        item->setData(Qt::DisplayRole, QStringLiteral("(Document %1)").arg(i + 1));
        item->setSizeHint(QSize(200, 20));
    }
    // Restore selection after clearing
    if (_currentIndex >= 0 && _currentIndex < _listWidget->count())
        _listWidget->setCurrentRow(_currentIndex);
}

// =============================================================================
// setCurrentIndex — mirrors ListView_SetItemState(..., LVIS_SELECTED|LVIS_FOCUSED)
// =============================================================================

void TaskList::setCurrentIndex(int index)
{
    if (index < 0 || index >= _listWidget->count()) return;
    _currentIndex = index;
    _listWidget->setCurrentRow(index);
}

// =============================================================================
// setFont — mirrors WM_SETFONT
// =============================================================================

void TaskList::setFont(int fontSize, const QString& fontName)
{
    destroyFont();
    QString face = fontName.isEmpty() ? QStringLiteral("Segoe UI") : fontName;
    _pFont = new QFont(face, fontSize);
    _pFontSelected = new QFont(face, fontSize, QFont::Bold);
    _listWidget->setFont(*_pFont);
}

// =============================================================================
// destroyFont — mirrors DeleteObject(_hFont / _hFontSelected)
// =============================================================================

void TaskList::destroyFont()
{
    delete _pFont; _pFont = nullptr;
    delete _pFontSelected; _pFontSelected = nullptr;
}

// =============================================================================
// adjustedSize — mirrors Win32 TaskList::adjustSize()
// =============================================================================

QSize TaskList::adjustedSize() const
{
    if (!_listWidget || _listWidget->count() == 0)
        return QSize(200, 100);

    int maxWidth = 0;
    QFontMetrics fm(_listWidget->font());
    for (int i = 0; i < _listWidget->count(); ++i)
    {
        QListWidgetItem* item = _listWidget->item(i);
        if (item)
        {
            int w = fm.horizontalAdvance(item->text()) + 16 + 20; // icon + padding
            maxWidth = qMax(maxWidth, w);
        }
    }

    int rowHeight = 20;
    int count = _listWidget->count();
    int height = rowHeight * qMin(count, 20); // max 20 visible rows

    QScreen* screen = QApplication::screenAt(QCursor().pos());
    if (screen)
        height = qMin(height, screen->geometry().height() - 120);

    return QSize(qMax(maxWidth, 150), height);
}

// =============================================================================
// updateCurrentIndex — mirrors Win32 TaskList::updateCurrentIndex()
// =============================================================================

int TaskList::updateCurrentIndex()
{
    QList<QListWidgetItem*> sel = _listWidget->selectedItems();
    if (!sel.isEmpty())
        _currentIndex = _listWidget->row(sel.first());
    return _currentIndex;
}

// =============================================================================
// moveSelection — mirrors Win32 TaskList::moveSelection(int)
// =============================================================================

void TaskList::moveSelection(int direction)
{
    int count = _listWidget->count();
    if (count == 0) return;

    int newIndex = _currentIndex + direction;
    if (newIndex < 0) newIndex = count - 1;
    if (newIndex >= count) newIndex = 0;

    setCurrentIndex(newIndex);
    _listWidget->scrollToItem(_listWidget->item(newIndex), QAbstractItemView::EnsureVisible);
}

// =============================================================================
// wheelEvent — mirrors WM_MOUSEWHEEL
// =============================================================================

void TaskList::wheelEvent(QWheelEvent* event)
{
    int delta = event->angleDelta().y();
    moveSelection(delta > 0 ? -1 : 1);
    event->accept();
}

// =============================================================================
// event — mirrors TaskListSelectProc (WM_KEYDOWN/WM_KEYUP/WM_GETDLGCODE)
// =============================================================================

bool TaskList::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto* ke = static_cast<QKeyEvent*>(event);
        // Ctrl+Tab / Ctrl+Down / Ctrl+Right — cycle forward
        if (ke->modifiers() & Qt::ControlModifier)
        {
            if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Down || ke->key() == Qt::Key_Right)
            {
                moveSelection(1);
                return true;
            }
            // Ctrl+Shift+Tab / Ctrl+Up / Ctrl+Left — cycle backward
            if (ke->key() == Qt::Key_Backtab || ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Left)
            {
                moveSelection(-1);
                return true;
            }
        }
        // Ctrl released — confirm selection (mirrors WM_KEYUP VK_CONTROL)
        if (ke->key() == Qt::Key_Control)
        {
            emit idPickedUp(_currentIndex);
            return true;
        }
        // Return/Enter — activate current (mirrors VK_RETURN in LVN_KEYDOWN)
        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
        {
            emit idPickedUp(_currentIndex);
            return true;
        }
    }
    return QWidget::event(event);
}

// =============================================================================
// Slot handlers
// =============================================================================

void TaskList::onItemClicked(QListWidgetItem*)
{
    updateCurrentIndex();
}

void TaskList::onItemDoubleClicked(QListWidgetItem*)
{
    emit itemActivated(_currentIndex);
}

void TaskList::onItemSelectionChanged()
{
    updateCurrentIndex();
    emit currentIndexChanged(_currentIndex);
}

// =============================================================================
// TaskListDlg
// =============================================================================

int TaskListDlg::_instanceCount = 0;

TaskListDlg::TaskListDlg()
    : StaticDialog()
{
    ++_instanceCount;
}

void TaskListDlg::init(QApplication* app, QWidget* parent, void* hImgLst, bool dir)
{
    Q_UNUSED(app);
    StaticDialog::create(0);
    _hParent = parent;
    _hImgLst = hImgLst;
    _initDir = dir;

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_taskList);

    // TaskList init (mirrors Win32 _taskList.init(_hInst, _hSelf, _hImalist, nbTotal, i2set))
    _taskList.init(app, this, hImgLst, 0, 0);

    // Connect TaskList signals → TaskListDlg slots
    connect(&_taskList, &TaskList::idPickedUp,
            this, &TaskListDlg::onIdPickedUp);

    connect(&_taskList, &TaskList::itemActivated,
            this, [this](int) { onIdPickedUp(_taskList.getCurrentIndex()); });

    // Install event filter on TaskList to intercept right-click and wheel
    _taskList.installEventFilter(this);

    setLayout(layout);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setFocusPolicy(Qt::NoFocus);

    _isCreated = true;
}

void TaskListDlg::setTaskListInfo(const TaskListInfo& info)
{
    _taskListInfo = info;

    int nbTotal = static_cast<int>(info._tlfsLst.size());
    int i2set = info._currentIndex + (_initDir ? -1 : 1);
    if (i2set < 0) i2set = nbTotal - 1;
    if (i2set > nbTotal - 1) i2set = 0;

    _taskList.init(nullptr, this, _hImgLst, nbTotal, i2set);
    _taskList.setFont(10);
    _taskList.setItems(info);

    // Size to fit content
    QSize sz = _taskList.adjustedSize();
    _taskList.setMinimumSize(sz);
    _taskList.setMaximumSize(sz);
    resize(sz);
}

int TaskListDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    if (!isVisible())
    {
        // Center on cursor
        QScreen* screen = QApplication::screenAt(QCursor::pos());
        if (!screen) screen = QApplication::primaryScreen();
        if (screen)
        {
            QPoint center = screen->geometry().center();
            move(center.x() - width() / 2, center.y() - height() / 2);
        }
    }
    show();
    raise();
    activateWindow();
    return 0;
}

void TaskListDlg::destroy()
{
    _taskList.destroy();
    --_instanceCount;
}

void TaskListDlg::onIdPickedUp(int listIndex)
{
    // Mirrors Win32: SendMessage(_hParent, NPPM_ACTIVATEDOC, view, index)
    if (listIndex < 0 || listIndex >= static_cast<int>(_taskListInfo._tlfsLst.size()))
        return;

    const TaskLstFnStatus& item = _taskListInfo._tlfsLst[listIndex];
    emit itemPickedUp(listIndex, item._iView, item._docIndex);

    hide();
}

bool TaskListDlg::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);

    // Right-click: mirror Win32 WM_RBUTTONUP → ID_PICKEDUP
    if (event->type() == QEvent::MouseButtonRelease)
    {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::RightButton)
        {
            onIdPickedUp(_taskList.getCurrentIndex());
            return true;
        }
    }

    // Mouse wheel: pass through to TaskList wheel event
    if (event->type() == QEvent::Wheel)
    {
        // Let the TaskList wheel event handle it
        return false;
    }

    return false;
}

intptr_t TaskListDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            return true;

        case WM_DESTROY:
            destroy();
            return true;

        case NPPM_INTERNAL_REFRESHDARKMODE:
            // Force delegate repaint on dark mode change
            _taskList.viewport()->update();
            return true;

        default:
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}

