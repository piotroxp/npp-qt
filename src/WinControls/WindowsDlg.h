// Semantic Lift: Win32 WindowsDlg → Qt6 implementation
// Original: PowerEditor/src/WinControls/WindowsDlg/*.h/*.cpp
// Target: npp-qt/src/WinControls/WindowsDlg.h + .cpp

#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QRect>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QLabel>

#include "StaticDialog.h"
#include "Window.h"

// =============================================================================
// Constants (lifted from WindowsDlgRc.h)
// =============================================================================

constexpr int IDD_WINDOWS      = 7000;
constexpr int IDC_WINDOWS_LIST = IDD_WINDOWS + 1;
constexpr int IDC_WINDOWS_SAVE = IDD_WINDOWS + 2;
constexpr int IDC_WINDOWS_CLOSE = IDD_WINDOWS + 3;
constexpr int IDC_WINDOWS_SORT = IDD_WINDOWS + 4;

// =============================================================================
// WinDlgNotifyType — notification types sent to parent
// =============================================================================

enum WinDlgNotifyType {
    WDT_ACTIVATE = 1,
    WDT_SAVE     = 2,
    WDT_CLOSE    = 3,
    WDT_SORT     = 4,
};

// =============================================================================
// WindowsMenu — window list in the menu bar
// Populates the "Window" submenu with document entries showing filename,
// checkmark on the active document, and "*" suffix on dirty buffers.
// Mirrors Win32 WindowsMenu that uses HMENU / MENUITEMINFO.
// =============================================================================

class WindowsMenu
{
public:
    WindowsMenu() = default;
    ~WindowsMenu() = default;

    // init — set the "Window" submenu handle (called from MainWindow)
    void init(void* hMainMenu)
    {
        Q_UNUSED(hMainMenu);
        // Requires: QMenu* _windowMenu populated by MainWindow
        // Populates with document list from DocTabView
    }

    // initPopupMenu — rebuild menu items before showing
    // Called on menu bar hover / about-to-show.
    void initPopupMenu(void* hMenu, void* pTab)
    {
        Q_UNUSED(hMenu);
        Q_UNUSED(pTab);
        // Full implementation:
        // 1. Get DocTabView* from pTab
        // 2. Iterate all buffers: _pTab->getBufferByIndex(i)
        // 3. Build menu item text: filename (+ "*" if dirty, checked if active)
        // 4. Use QMenu::clear() + QMenu::addAction() to populate
    }

private:
    void* _hMenu = nullptr;      // HMENU* (Win32) / QMenu* (Qt bridge)
    void* _hMenuList = nullptr;   // HMENU* (Win32) / QMenu* (Qt bridge)
    unsigned int _limitPrev = 0;  // Previous limit (Win32 MRU tracking)
};

// =============================================================================
// WindowsDlg — dialog showing all open document windows
// Mimics Win32 dialog with ListView + buttons
// =============================================================================

class WindowsDlg : public StaticDialog
{
    Q_OBJECT

public:
    WindowsDlg();
    ~WindowsDlg() override = default;

    int doDialog();
    void init(QApplication* app, QWidget* parent, void* pTab);
    void doSortToTabs();
    void doSort();
    void sort(int columnID, bool reverseSort);
    void doRefresh(bool invalidate = false);

    // Sorting helpers
    void sortFileNameASC()   { sort(0, false); }
    void sortFileNameDSC()   { sort(0, true); }
    void sortFilePathASC()   { sort(1, false); }
    void sortFilePathDSC()   { sort(1, true); }
    void sortFileTypeASC()   { sort(2, false); }
    void sortFileTypeDSC()   { sort(2, true); }
    void sortFileSizeASC()   { sort(3, false); }
    void sortFileSizeDSC()   { sort(3, true); }
    void sortDateTimeASC()   { sort(4, false); }
    void sortDateTimeDSC()   { sort(4, true); }

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

signals:
    void windowActivated(int index);
    void windowSaved(int index);
    void windowClosed(QVector<int> indices);
    void windowSorted(QVector<int> newOrder);
    void closeRequested();

public slots:
    void onActivate();
    void onSave();
    void onClose();
    void onSort();
    void onContextMenu(const QPoint& pos);
    void onCopyName();
    void onCopyPath();

protected:
    bool event(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void updateColumnNames();
    void fitColumnsToSize();
    void resetSelection();
    void updateButtonState();
    void activateCurrent();
    void doColumnSort();
    void refreshMap();
    void putItemsToClipboard(bool isFullPath);

    QTableWidget* _list = nullptr;
    QPushButton* _btnActivate = nullptr;
    QPushButton* _btnSave = nullptr;
    QPushButton* _btnClose = nullptr;
    QPushButton* _btnSort = nullptr;

    void* _pTab = nullptr;         // DocTabView* (not yet lifted)
    QVector<int> _idxMap;          // Maps list row → tab index

    int _currentColumn = -1;        // Sort column (-1 = none)
    int _lastSort = -1;            // Previous sort column
    bool _reverseSort = false;     // Sort direction

    QMenu* _contextMenu = nullptr;
    WindowsMenu _windowsMenu;

    static QRect _lastKnownLocation;
    static QSize _szMinButton;
    static QSize _szMinListCtrl;
};
