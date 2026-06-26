// Semantic Lift: Win32 TaskList → Qt6 implementation
// Original: PowerEditor/src/WinControls/TaskList/TaskList.h (64 lines)
// Target: npp-qt/src/WinControls/TaskList.h
//
// Lifts the document-switcher popup (taskbar-style list) from Win32 ListView
// with WM_MOUSEWHEEL scrolling to Qt6 QListWidget with wheel-event handling.
// Follows the npp-qt WinControls translation pattern.

#pragma once

#include "Window.h"
#include "StaticDialog.h"
#include <QListWidget>
#include <QFont>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QStyledItemDelegate>  // TaskListDelegate / ShortcutTableDelegate base class
#include <vector>
#include <string>

// Task status icon indices (mirrors Win32 icon mapping)
#define TASKLIST_ICON_UNICODE    0
#define TASKLIST_ICON_ANSI       1
#define TASKLIST_ICON_DIRTY      2
#define TASKLIST_ICON_READONLY   3

// Task status struct (mirrors Win32 TaskLstFnStatus)
struct TaskLstFnStatus {
    int _iView = -1;
    int _docIndex = 0;
    QString _fn;
    int _status = 0;
    void* _bufID = nullptr;
    int _docColor = -1;

    TaskLstFnStatus() = default;
    TaskLstFnStatus(const QString& str, int status) : _fn(str), _status(status) {}
    TaskLstFnStatus(int iView, int docIndex, const QString& str,
                    int status, void* bufID, int docColor)
        : _iView(iView), _docIndex(docIndex), _fn(str),
          _status(status), _bufID(bufID), _docColor(docColor) {}
};

// Task list info struct (mirrors Win32 TaskListInfo)
struct TaskListInfo {
    std::vector<TaskLstFnStatus> _tlfsLst;
    int _currentIndex = -1;
};

// Custom message for getting task list info from parent
// Win32: SendMessage(_hParent, WM_GETTASKLISTINFO, &TaskListInfo, 0)
// Qt: parent sends TaskListInfo via signal or direct method call
// WM_GETTASKLISTINFO is defined centrally in NppConstants.h

// =============================================================================
// TaskList — document switcher widget (mirrors Win32 TaskList)
// A single-column ListView with owner-draw rows, scroll on wheel, keyboard nav.
// =============================================================================

class TaskList : public QWidget
{
    Q_OBJECT

public:
    explicit TaskList(QWidget* parent = nullptr);
    ~TaskList() override;

    // init — mirrors Win32 TaskList::init()
    void init(QApplication* app, QWidget* parent, void* hImaLst, int nbItem, int index2set);

    void destroy();
    void setFont(int fontSize, const QString& fontName = QString());
    void destroyFont();

    // Mirrors adjustSize() — returns natural size for the list
    QSize adjustedSize() const;

    int getCurrentIndex() const { return _currentIndex; }
    void setCurrentIndex(int index);
    int updateCurrentIndex();
    QWidget* viewport() const { return _listWidget->viewport(); }

    // Populate items from TaskListInfo (called by parent after WM_GETTASKLISTINFO)
    void setItems(const TaskListInfo& info);

    // Signals for item interactions (mirrors Win32 NM_CLICK / NM_RCLICK / VK_RETURN)
    void setItemCount(int nbItem);

signals:
    void currentIndexChanged(int index);
    void itemActivated(int index);
    void idPickedUp(int index);

public slots:
    void onItemSelectionChanged();
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);

protected:
    void wheelEvent(QWheelEvent* event) override;
    bool event(QEvent* event) override;

private:
    void moveSelection(int direction);

    QListWidget* _listWidget = nullptr;
    QFont* _pFont = nullptr;
    QFont* _pFontSelected = nullptr;
    QWidget* _hParent = nullptr;
    void* _hImgLst = nullptr;
    int _nbItem = 0;
    int _currentIndex = 0;
    QSize _adjustedSize;
    QString _initDir;

    friend class TaskListDelegate;
};

// =============================================================================
// TaskListDelegate — owner-draw item delegate
// Mirrors Win32 LVS_OWNERDRAWFIXED + WM_DRAWITEM.
// Uses NppDarkMode for dark/light theming.
// =============================================================================

class TaskListDelegate : public QStyledItemDelegate
{
public:
    explicit TaskListDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

// =============================================================================
// TaskListDlg — popup document-switcher dialog (mirrors Win32 TaskListDlg)
// Shows a list of open documents; user picks one to activate.
// Handles WM_RBUTTONUP context menu and WM_MOUSEWHEEL passthrough.
// =============================================================================

class TaskListDlg : public StaticDialog
{
    Q_OBJECT

public:
    TaskListDlg();
    ~TaskListDlg() override = default;

    void init(QApplication* app, QWidget* parent, void* hImgLst, bool dir);
    int doDialog(bool isRTL = false);
    void destroy() override;

    // Provide task list info from the parent (mirrors WM_GETTASKLISTINFO)
    void setTaskListInfo(const TaskListInfo& info);

    static int _instanceCount;

signals:
    void itemPickedUp(int listIndex, int viewToSet, int docIndexToSwitch);

public slots:
    void onIdPickedUp(int listIndex);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    TaskList _taskList;
    TaskListInfo _taskListInfo;
    void* _hImgLst = nullptr;
    bool _initDir = false;  // false = dirDown, true = dirUp
    QLabel* _label = nullptr;  // Header label ("Switch To...")

    // Mouse event filter for right-click and wheel passthrough
    bool eventFilter(QObject* watched, QEvent* event) override;
};