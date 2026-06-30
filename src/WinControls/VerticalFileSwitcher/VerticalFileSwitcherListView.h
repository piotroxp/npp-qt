// Semantic Lift: Win32 VerticalFileSwitcherListView → Qt6 QTreeView
// Original:  PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.h
// Target:    npp-qt/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.h
//
// Win32 → Qt6 translation:
//   WC_LISTVIEW + LVS_REPORT          → QTreeView + QStandardItemModel
//   LVGROUP (MAIN_VIEW / SUB_VIEW)     → Non-selectable top-level QStandardItem roots
//   LVITEM + lParam                    → QStandardItem::data(Qt::UserRole) → qintptr → TaskLstFnStatus*
//   ListView_SetItemState              → QItemSelectionModel::select()
//   ListView_EnsureVisible             → QTreeView::scrollTo(EnsureVisible)
//   ListView_RedrawItems               → viewport()->update()
//   ListView_GetItemCount              → _model->rowCount()
//   ::PathFindFileName/Extension       → QFileInfo::fileName()/.suffix()
//   ::GetClientRect                     → geometry()
//   HIMAGELIST                         → QFileIconProvider (QPixmap stored in model)
//   HWND window handle                 → QWidget* parent
//   LRESULT WindowProc return           → Qt event override return

#pragma once

#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMenu>
#include <QAction>
#include <QItemSelectionModel>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QVector>
#include <QMap>

#include "TaskList.h"          // for TaskLstFnStatus*, TaskListInfo
#include "Buffer.h"            // for Buffer, BufferID, MAIN_VIEW, SUB_VIEW
#include "NppConstants.h"       // for MAIN_VIEW, SUB_VIEW
#include "ScintillaComponent.h" // for BufferID shim

// Column indices
#define FS_COL_NAME  0
#define FS_COL_EXT  1
#define FS_COL_PATH 2

// File status → icon index (mirrors Win32 icon mapping)
#define FS_ICON_NORMAL      0
#define FS_ICON_DIRTY       1
#define FS_ICON_READONLY    2
#define FS_ICON_MONITORING  3

// Group IDs
#define FS_GROUP_ACTIVE   1
#define FS_GROUP_INACTIVE  2


// =============================================================================
// VerticalFileSwitcherListView
//
// Qt6 semantic lift of the Win32 list-view document panel.
// Displays open files grouped into "Active Files" (MAIN_VIEW) and
// "Inactive Files" (SUB_VIEW), each in a non-selectable root row.
// File items carry a TaskLstFnStatus* in Qt::UserRole for O(1) lookup.
// =============================================================================

class VerticalFileSwitcherListView : public QTreeView
{
    Q_OBJECT

public:
    explicit VerticalFileSwitcherListView(QWidget* parent = nullptr);
    ~VerticalFileSwitcherListView() override;

    // --- Lifecycle ---

    // init — mirrors Win32 init(hInst, hParent, hImaLst).
    // HINSTANCE unused in Qt6; hImaLst (Win32 image list) replaced by
    // QFileIconProvider (stored as member).
    void init(QWidget* parent);

    // destroy — delete all owned TaskLstFnStatus objects, clear the model.
    void destroy();

    // initList — populate the model from the parent's TaskListInfo.
    // Called once on startup and after reload.
    void initList();

    // --- Buffer operations ---

    // getBufferInfoFromIndex — retrieve (BufferID, view) for a model row index.
    // Returns BUFFER_INVALID if index is out of range.
    BufferID getBufferInfoFromIndex(int index, int& view) const;

    // setBgColour — legacy alias (Win32 ListView_SetItemState with colour index).
    // In Qt6: colour is driven by the NppDarkMode palette on the viewport.
    void setBgColour(int /*colourIndex*/) { /* handled by dark mode */ }

    // newItem — add a file entry if not already present; return its row index.
    int newItem(BufferID bufferID, int iView);

    // closeItem — remove the entry for (bufferID, iView); return its old index.
    int closeItem(BufferID bufferID, int iView);

    // activateItem — make (bufferID, iView) the current selection.
    void activateItem(BufferID bufferID, int iView);

    // setItemIconStatus — refresh icon and text for an existing buffer entry.
    void setItemIconStatus(BufferID bufferID);

    // getFullFilePath — return the full path string for model row i.
    QString getFullFilePath(size_t i) const;

    // setItemColor — apply document colour to a buffer entry.
    void setItemColor(BufferID bufferID);

    // --- Column management ---

    void insertColumn(const QString& name, int width, int index);
    void resizeColumns(int totalWidth);
    void deleteColumn(size_t index);

    // --- Selection helpers ---

    int nbSelectedFiles() const;

    // getSelectedFiles — return all selected BufferViewInfo entries.
    // If reverse is true, return deselected entries instead.
    std::vector<BufferViewInfo> getSelectedFiles(bool reverse = false) const;

    // --- Refresh ---

    void reload();
    void redrawItems();

    // --- Visibility ---

    void ensureVisibleCurrentItem() const;

    // --- Colour theming (NppDarkMode) ---

    // setBackgroundColor — set viewport background (drives dark/light mode).
    void setBackgroundColor(const QColor& bgColour);

    // setForegroundColor — set item foreground/text colour.
    void setForegroundColor(const QColor& fgColour);

    // --- DPI helpers (mirrors DPIManagerV2::scale / getDpiForWindow) ---
    static int scaleValue(int value, int dpi);
    static int dpiForWidget(QWidget* widget);

signals:
    // Emitted when the user double-clicks (or presses Enter on) a file row.
    // Connected by VerticalFileSwitcher to activate the document.
    void fileActivated(BufferID bufferID, int iView);

    // Emitted when the user right-clicks a file row and chooses "Close".
    // Connected by VerticalFileSwitcher to close the specific document.
    void requestClose(BufferID bufferID, int iView);

    // Emitted for "Close All" context menu action.
    void requestCloseAll();

    // Emitted for "Close Other Files" action.
    void requestCloseOthers(BufferID keepBufferID, int keepView);

    // Emitted for right-click context menu on a file row.
    // Can be used by the parent for additional context actions.
    void contextMenuRequested(const QPoint& globalPos, BufferID bufferID, int iView);

public slots:
    // --- Context menu action handlers (called by parent) ---
    void onCloseFile();
    void onCloseAll();
    void onCloseOthers();

protected:
    // --- Event handlers ---
    bool event(QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    // --- Internal helpers ---
    int find(BufferID bufferID, int iView) const;
    int add(BufferID bufferID, int iView);
    void remove(int index, bool removeFromModel = true);
    void removeAll();
    void selectCurrentItem() const;
    void clearAllSelections();

    // fetchTaskListInfo — protected virtual called by initList().
    // Subclasses override to fetch TaskListInfo from Notepad_plus core.
    // The base class implementation does nothing (leaves info empty).
    virtual void fetchTaskListInfo(TaskListInfo& info);

    // resolveIconStatus — map Buffer state to FS_ICON_* index.
    int resolveIconStatus(Buffer* buf) const;

    // pathToGroup — return the group root item for a given view (MAIN/SUB).
    QStandardItem* pathToGroup(int iView) const;

    // itemFromBuffer — O(n) lookup of the QStandardItem for (bufferID, iView).
    QStandardItem* itemFromBuffer(BufferID bufferID, int iView) const;

private:
    QStandardItemModel* _model = nullptr;
    QFileIconProvider* _iconProvider = nullptr;

    QStandardItem* _rootActive = nullptr;    // "Active Files" group root
    QStandardItem* _rootInactive = nullptr;  // "Inactive Files" group root

    int _currentIndex = 0;

    // Column visibility flags (mirror NppGUI._fileSwitcherWithoutExtColumn / PathColumn)
    bool _showExtColumn = true;
    bool _showPathColumn = false;

    // Track which row index each root's items start at so we can map row→item
    int _activeRootRowStart = 0;
    int _inactiveRootRowStart = 0;
};
