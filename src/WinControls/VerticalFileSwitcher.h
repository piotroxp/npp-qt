// Semantic Lift: Win32 VerticalFileSwitcher → Qt6 VerticalFileSwitcher
// Original: PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcher.h
// Target: npp-qt/src/WinControls/VerticalFileSwitcher.h

#pragma once

#include "DockingWnd.h"
#include "StaticDialog.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QVector>
#include <QMap>
#include <QPair>
#include <functional>

// Sort direction constants (mirrors Win32 SORT_DIRECTION_*)
const int SORT_DIRECTION_NONE  = -1;
const int SORT_DIRECTION_UP    =  0;
const int SORT_DIRECTION_DOWN  =  1;

// Column definitions (mirrors FS_CLMN* constants)
const int COL_NAME = 0;
const int COL_EXT  = 1;
const int COL_PATH = 2;
const int COL_COUNT = 3;

// Group IDs for file grouping by view
const int GROUP_MAIN = 1;
const int GROUP_SUB  = 2;

// Popup menu command IDs
const int CLMNEXT_ID  = 1;
const int CLMNPATH_ID = 2;
const int LVGROUPS_ID = 3;

// =============================================================================
// FileEntry — per-row metadata (mirrors Win32 TaskLstFnStatus)
// Stores all document state needed by the file switcher.
// =============================================================================

struct FileEntry {
    int _bufID = 0;         // Buffer ID
    int _iView = 0;         // MAIN_VIEW or SUB_VIEW
    QString _fn;            // Full file path
    int _status = 0;        // Icon status: 0=clean, 1=dirty, 2=readonly, 3=monitoring
    int _docColor = -1;     // Color ID (-1 = no color)

    FileEntry() = default;
    FileEntry(int bufID, int iView, const QString& fn, int status = 0, int docColor = -1)
        : _bufID(bufID), _iView(iView), _fn(fn), _status(status), _docColor(docColor) {}
};

// =============================================================================
// VerticalFileSwitcherListView — file list inside the switcher panel
// Mirrors Win32 VerticalFileSwitcherListView (ListView with groups + custom draw).
// =============================================================================

class VerticalFileSwitcherListView : public QTableWidget
{
    Q_OBJECT

public:
    explicit VerticalFileSwitcherListView(QWidget* parent = nullptr);
    ~VerticalFileSwitcherListView() override = default;

    void init(void* hImaLst);
    void destroy();
    void initList();

    // Buffer/view management (mirrors Win32 LVM_* + buffer management)
    int getBufferInfoFromIndex(int index, int& view) const;
    int newItem(int bufferID, int iView);
    int closeItem(int bufferID, int iView);
    void activateItem(int bufferID, int iView);
    void setItemIconStatus(int bufferID);
    QString getFullFilePath(size_t i) const;
    void setItemColor(int bufferID);

    // Column management
    void insertColumn(const QString& name, int width, int index);
    void resizeColumns(int totalWidth);
    void deleteColumn(size_t i);

    int nbSelectedFiles() const {
        return selectionModel()->selectedRows().count();
    }

    std::vector<QPair<int, int>> getSelectedBuffers(bool reverse = false) const;
    void reload();
    void redrawItems();
    void ensureVisibleCurrentItem();
    void removeAllItems();

    // Colors
    void setBackgroundColor(const QColor& bg);
    void setForegroundColor(const QColor& fg);

    // Context menu
    void showContextMenu(const QPoint& pos);

    // Drag/drop support
    Qt::DropActions supportedDropActions() const override { return Qt::MoveAction; }
    QStringList mimeTypes() const override;

signals:
    void fileActivated(int bufferID, int view);
    void fileClosed(int bufferID, int view);
    void columnSorted(int column, int direction);
    void middleButtonClicked(int bufferID, int view);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int _currentIndex = 0;
    void selectCurrentItem();
    int find(int bufferID, int iView) const;
    int add(int bufferID, int iView);
    void remove(int index, bool removeRow = true);
    void setItemIconStatus(int bufferID, int row);

    // Entries keyed by bufferID+view
    QMap<QPair<int, int>, FileEntry> _entries;
    int _nextInsertIndex = 0;
};

// =============================================================================
// VerticalFileSwitcher — Document List docking panel
// Mirrors Win32 VerticalFileSwitcher (DockingDlgInterface + ListView).
// =============================================================================

class VerticalFileSwitcher : public DockingDlgInterface
{
    Q_OBJECT

public:
    explicit VerticalFileSwitcher();
    ~VerticalFileSwitcher() override = default;

    void init(QApplication* app, QWidget* parent, void* hImaLst);

    void display(bool toShow = true) override;

    void activateDoc(int bufferID, int iView) const;
    void closeDoc(int bufferID, int iView) const;

    int newItem(int bufferID, int iView) {
        return _fileListView.newItem(bufferID, iView);
    }

    int closeItem(int bufferID, int iView) {
        return _fileListView.closeItem(bufferID, iView);
    }

    void activateItem(int bufferID, int iView) {
        _fileListView.activateItem(bufferID, iView);
    }

    void setItemIconStatus(int bufferID) {
        _fileListView.setItemIconStatus(bufferID);
    }

    void setItemColor(int bufferID) {
        _fileListView.setItemColor(bufferID);
    }

    QString getFullFilePath(size_t i) const {
        return _fileListView.getFullFilePath(i);
    }

    int setHeaderOrder(int columnIndex);
    void updateHeaderArrow();

    int nbSelectedFiles() const {
        return _fileListView.nbSelectedFiles();
    }

    std::vector<QPair<int, int>> getSelectedBuffers(bool reverse = false) const {
        return _fileListView.getSelectedBuffers(reverse);
    }

    void startColumnSort();
    void reload() {
        _fileListView.reload();
        startColumnSort();
    }

    void updateTabOrder() {
        if (_lastSortingDirection == SORT_DIRECTION_NONE) {
            _fileListView.reload();
        }
    }

    void setBackgroundColor(const QColor& bgColour) override {
        DockingDlgInterface::setBackgroundColor(bgColour);
        _fileListView.setBackgroundColor(bgColour);
    }

    void setForegroundColor(const QColor& fgColour) override {
        DockingDlgInterface::setForegroundColor(fgColour);
        _fileListView.setForegroundColor(fgColour);
    }

signals:
    void fileActivated(int bufferID, int view);
    void fileClosed(int bufferID, int view);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initPopupMenus();
    void popupMenuCmd(int cmdID);

    bool _colHeaderRClick = false;
    int _lastSortingColumn = 0;
    int _lastSortingDirection = SORT_DIRECTION_NONE;
    VerticalFileSwitcherListView _fileListView;
    void* _hImgLst = nullptr;

    // Column visibility flags (mirrors NppGUI settings)
    bool _withoutExtColumn = false;
    bool _withoutPathColumn = false;
    bool _disableListViewGroups = false;

    // Column widths (DPI-aware, mirrored from NppGUI)
    int _extColumnWidth = 60;
    int _pathColumnWidth = 200;

    // Popup menus
    QMenu* _contextMenu = nullptr;
    QAction* _openAction = nullptr;
    QAction* _closeAction = nullptr;
    QAction* _copyPathAction = nullptr;
    QAction* _showInExplorerAction = nullptr;
    QAction* _toggleExtAction = nullptr;
    QAction* _togglePathAction = nullptr;
    QAction* _toggleGroupsAction = nullptr;

    void applySorting(int column);
};
