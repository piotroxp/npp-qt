// Semantic Lift: Win32 MFC → Qt6
// Original: PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.cpp
// Target: npp-qt/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.cpp
//
// NOTE: VerticalFileSwitcherListView functionality has been merged into
// VerticalFileSwitcher itself. This stub provides minimal implementations
// to satisfy compilation. All real logic lives on the VerticalFileSwitcher
// class using QListWidget.

#include "WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.h"
#include "WinControls/VerticalFileSwitcher/VerticalFileSwitcher.h"
#include "Buffer.h"

VerticalFileSwitcherListView::VerticalFileSwitcherListView(QWidget* parent)
    : QListWidget(parent)
{
    setObjectName(QStringLiteral("VerticalFileSwitcherListView"));
}

void VerticalFileSwitcherListView::init(HINSTANCE /*hInst*/, HWND /*parent*/, HIMAGELIST /*hImaLst*/)
{
    // Win32: creates the list-view control with LVS_REPORT style
    // Qt6: QListWidget is already constructed; configure column headers here
    setHeaderHidden(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setUniformItemSizes(true);
}

void VerticalFileSwitcherListView::destroy()
{
    // Win32: DestroyWindow(_hSelf) + ImageList_Destroy(_hImaLst)
    // Qt6: QListWidget destructor handles widget destruction
    clear();
}

void VerticalFileSwitcherListView::initList()
{
    // Win32: LVM_SETEXTENDEDLISTVIEWSTYLE + column setup
    clear();
}

BufferID VerticalFileSwitcherListView::getBufferInfoFromIndex(int /*index*/, int& /*view*/) const
{
    // Returns buffer ID and view for the item at index
    // Implementation lives in VerticalFileSwitcher
    return INVALID_BUFFERID;
}

int VerticalFileSwitcherListView::newItem(BufferID /*bufferID*/, int /*iView*/)
{
    // Adds a new file entry to the list
    // Implementation lives in VerticalFileSwitcher
    return -1;
}

int VerticalFileSwitcherListView::closeItem(BufferID /*bufferID*/, int /*iView*/)
{
    // Removes file entry from list
    // Implementation lives in VerticalFileSwitcher
    return -1;
}

void VerticalFileSwitcherListView::activateItem(BufferID /*bufferID*/, int /*iView*/)
{
    // Sets active/focused item in list
    // Implementation lives in VerticalFileSwitcher
}

void VerticalFileSwitcherListView::setItemIconStatus(BufferID /*bufferID*/)
{
    // Updates icon based on buffer state (modified, read-only, etc.)
    // Implementation lives in VerticalFileSwitcher
}

std::wstring VerticalFileSwitcherListView::getFullFilePath(size_t /*i*/) const
{
    // Returns full path for item at index i
    return std::wstring();
}

void VerticalFileSwitcherListView::setItemColor(BufferID /*bufferID*/)
{
    // Sets foreground/background color for the item
    // Implementation lives in VerticalFileSwitcher
}

void VerticalFileSwitcherListView::insertColumn(const wchar_t* /*name*/, int /*width*/, int /*index*/)
{
    // Win32: LVM_INSERTCOLUMN
    // Qt6: QTableWidget API — for QListWidget just add items
}

void VerticalFileSwitcherListView::resizeColumns(int /*totalWidth*/)
{
    // Win32: LVM_SETCOLUMNWIDTH
    // Qt6: sizeHintForColumn() or explicit setStyleSheet
}

void VerticalFileSwitcherListView::deleteColumn(size_t /*i*/)
{
    // Win32: LVM_DELETECOLUMN
    // Qt6: for QListWidget columns aren't exposed the same way
}

int VerticalFileSwitcherListView::nbSelectedFiles() const
{
    return selectedItems().size();
}

QVector<BufferViewInfo> VerticalFileSwitcherListView::getSelectedFiles(bool /*reverse*/) const
{
    // Returns selected file entries with buffer IDs and views
    // Implementation lives in VerticalFileSwitcher
    return {};
}

void VerticalFileSwitcherListView::reload()
{
    // Win32: WM_SETREDRAW = FALSE, reload items, WM_SETREDRAW = TRUE
    // Qt6: QAbstractItemModel::beginResetModel/endResetModel
    model()->beginResetModel();
    // TODO: re-populate items from buffers
    model()->endResetModel();
}

void VerticalFileSwitcherListView::redrawItems()
{
    // Win32: LVM_REDRAWITEMS
    // Qt6: update() on the viewport
    viewport()->update();
}

void VerticalFileSwitcherListView::ensureVisibleCurrentItem() const
{
    // Win32: LVM_ENSUREVISIBLE
    // Qt6: QAbstractItemView::scrollTo
    scrollTo(currentIndex(), QAbstractItemView::EnsureVisible);
}

void VerticalFileSwitcherListView::setBackgroundColor(COLORREF /*bgColour*/)
{
    // Win32: LVM_SETBKCOLOR + redraw
    // Qt6: setPalette or stylesheet
    QString style = QStringLiteral("QListWidget { background-color: #%1; }")
                        .arg(0, 6, 16, QChar('0'));
    setStyleSheet(style);
}

void VerticalFileSwitcherListView::setForegroundColor(COLORREF /*fgColour*/)
{
    // Win32: LVM_SETTEXTCOLOR + redraw
    // Qt6: setPalette or stylesheet
}

int VerticalFileSwitcherListView::find(BufferID /*bufferID*/, int /*iView*/) const
{
    return -1;
}

int VerticalFileSwitcherListView::add(BufferID /*bufferID*/, int /*iView*/)
{
    return -1;
}

void VerticalFileSwitcherListView::remove(int /*index*/, bool /*removeFromListview*/)
{
    // Win32: LVM_DELETEITEM
    // Qt6: takeItem(index)
    if (auto* item = takeItem(_currentIndex)) {
        delete item;
    }
}

void VerticalFileSwitcherListView::removeAll()
{
    // Win32: LVM_DELETEALLITEMS
    // Qt6: QListWidget::clear()
    clear();
}

void VerticalFileSwitcherListView::selectCurrentItem() const
{
    // Win32: LVM_SETITEMSTATE with LVIS_SELECTED | LVIS_FOCUSED
    // Qt6: QListWidget selection model
    if (currentIndex().isValid()) {
        selectionModel()->select(currentIndex(), QItemSelectionModel::SelectCurrent);
    }
}
