// Semantic Lift: Win32 Gripper → Qt6 drag overlay system
// Original: PowerEditor/src/WinControls/DockingWnd/Gripper.h
// Target: npp-qt/src/WinControls/DockingWnd/Gripper.h

#pragma once

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QCursor>
#include <QRect>
#include <QPoint>
#include <QRubberBand>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QTabWidget>
#include <QTabBar>

// Forward declarations
class DockingCont;
class DockingManager;
class DockingSplitter;

// =============================================================================
// Gripper — drag-to-dock overlay widget
//
// Win32: Creates a full-screen overlay window with magenta color-key transparency
//        (WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT).
//        Uses low-level mouse/keyboard hooks (WH_MOUSE_LL, WH_KEYBOARD_LL) to
//        track mouse even outside the app window. Draws dotted drag rectangle
//        via PatBlt on memory DC. Handles TCM_* tab messages for tab reordering.
//
// Qt6:   Uses a transparent overlay QWidget with QRubberBand for drag feedback.
//        QApplication::setOverrideCursor / mouseGrabbing handles cross-app tracking.
//        Tab reordering via QTabWidget signals (tabMoved, tabBarClicked).
//        Drag rectangle drawn with QPainter in paintEvent.
//
// Hard-migration signals (from npp-qt.md reference):
//   - Low-level hooks (WH_MOUSE_LL, WH_KEYBOARD_LL) → Qt::FramelessWindowHint +
//     grabMouse() / setMouseTracking(true) on overlay
//   - Tab control (TCM_*) → QTabWidget API
// =============================================================================

// Win32: Used by drawRectangle for dotted border pattern
static const unsigned short DotPattern[] = {
    0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
};

#define MDLG_CLASS_NAME "moveDlg"

// =============================================================================
// Gripper — drag-to-dock manager
// =============================================================================
class Gripper final : public QObject
{
    Q_OBJECT

public:
    Gripper(QObject* parent = nullptr);
    ~Gripper() override;

    // Win32: init(HINSTANCE, HWND hParent) → store handles
    // Qt6:   init(QWidget* parent) → store parent widget reference
    void init(QWidget* parent);

    // Win32: startGrip(DockingCont*, DockingManager*) → creates overlay window,
    //        sets up hooks, begins drag tracking
    // Qt6:   startGrip() → creates overlay widget, shows it, connects signals
    void startGrip(DockingCont* pCont, DockingManager* pDockMgr);

signals:
    // Emitted when drag completes — informs DockingManager of final placement
    void dragCompleted(DockingCont* targetCont, const QRect& rc);
    void dragCancelled();

private slots:
    // Internal drag tracking
    void onMouseMove(QMouseEvent* event);
    void onMouseRelease(QMouseEvent* event);
    void onKeyRelease(QKeyEvent* event);

private:
    // Win32: create() → creates overlay window, installs hooks
    // Qt6:   createOverlay() → creates transparent overlay widget
    void createOverlay();

    // Win32: destroyOverlayWindow() → releases DCs, destroys overlay HWND
    // Qt6:   destroyOverlay() → hides/deletes overlay widget
    void destroyOverlay();

    // Win32: drawRectangle(const POINT*) → draws drag rectangle on overlay DC
    // Qt6:   drawRectangle(const QPoint*) → updates rubber band geometry
    void drawRectangle(const QPoint* pPt);

    // Hit testing
    DockingCont* contHitTest(const QPoint& pt);
    DockingCont* workHitTest(const QPoint& pt, QRect* rc = nullptr);

    // Tab reordering
    void doTabReordering(const QPoint& pt);
    void initTabInformation();

    // Geometry helpers
    void getMovingRect(const QPoint& pt, QRect* rc);
    void getMousePoints(const QPoint& pt, QPoint& ptPrev);
    void shrinkRectToSize(QRect* rc);
    void calcRectToScreen(QWidget* hWnd, QRect* rc);
    void doCalcGripperRect(QRect* rc, const QRect& rcCorr, const QPoint& pt);

    // =====================================================================
    // Private data members
    // =====================================================================

    QWidget* _hParent = nullptr;           // Win32: HWND _hParent
    QWidget* _hSelf = nullptr;             // Win32: HWND _hSelf (overlay window)

    // Dock manager references
    tDockMgr _dockData;                    // Win32: tDockMgr _dockData
    DockingManager* _pDockMgr = nullptr;  // Win32: DockingManager*
    DockingCont* _pCont = nullptr;        // Win32: DockingCont*

    // Mouse offset from drag start
    QPoint _ptOffset{0, 0};

    // Previous mouse position for delta calculation
    QPoint _ptOld{0, 0};
    bool _bPtOldValid = false;

    // Previous rubber band rect (for efficient update)
    QRect _rcPrev;

    // Tab reordering state
    QWidget* _hTab = nullptr;              // Win32: HWND _hTab
    QWidget* _hTabSource = nullptr;        // Win32: HWND _hTabSource
    bool _startMovingFromTab = false;
    int _iItem = 0;
    QRect _rcItem;

    // TODO: [unhandled] TCITEM _tcItem — Win32 tab control item data
    // Qt6 equivalent: QTabWidget::tabData() / setTabData()

    // Drag overlay
    QWidget* _hOverlayWnd = nullptr;       // Win32: HWND _hOverlayWnd
    QRubberBand* _rubberBand = nullptr;    // Qt6: replaces Win32 overlay DC drawing

    // Drag state
    bool _isDragging = false;

    // RTL layout detection
    bool _isRTL = false;

    // Class registration state (Win32 only — no-op in Qt6)
    static bool _isRegistered;
    static bool _isOverlayClassRegistered;
};