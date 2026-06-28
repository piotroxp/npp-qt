// Semantic Lift: Win32 Gripper → Qt6 drag overlay system
// Original: PowerEditor/src/WinControls/DockingWnd/Gripper.cpp
// Target: npp-qt/src/WinControls/DockingWnd/Gripper.cpp

#include "Gripper.h"
#include "Docking.h"
#include "DockingCont.h"
#include "DockingManager.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QCursor>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QTabWidget>
#include <QTabBar>

// =============================================================================
// Gripper — drag-to-dock overlay
//
// Win32 implementation:
//   - Creates full-screen WS_EX_LAYERED overlay window with magenta color-key
//   - Installs WH_MOUSE_LL and WH_KEYBOARD_LL low-level hooks for global tracking
//   - Draws drag rectangle using PatBlt with dotted pattern on overlay memory DC
//   - Handles TCM_* messages for tab reordering during drag
//   - On completion: MoveWindow + WM_SIZE to reposition dock containers
//
// Qt6 translation:
//   - Overlay widget: Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint
//     with transparent background and WA_TranslucentBackground
//   - Global mouse tracking: QApplication::overrideCursor + QDesktopWidget tracking
//   - Drag feedback: QRubberBand with custom styling instead of PatBlt pattern
//   - Tab reordering: QTabWidget signals (tabMoved, tabBarClicked, currentChanged)
//   - Dock container repositioning: QWidget::setGeometry() instead of MoveWindow
//
// Hard-migration signals flagged:
//   - Low-level hooks (WH_MOUSE_LL) → Qt overlay widget with mouse grab
//   - Overlay layered window (WS_EX_LAYERED) → Qt::FramelessWindowHint + WA_TranslucentBackground
//   - PatBlt with dot pattern → QPainter with QPen::setDashPattern
// =============================================================================

// Win32 static registration state (no-op in Qt6)
bool Gripper::_isRegistered = false;
bool Gripper::_isOverlayClassRegistered = false;

// =============================================================================
// Gripper::Gripper — constructor
// =============================================================================
Gripper::Gripper(QObject* parent)
    : QObject(parent)
    , _isDragging(false)
    , _isRTL(false)
{
}

Gripper::~Gripper() {
    destroyOverlay();
}

// =============================================================================
// init — store parent widget reference
// =============================================================================
void Gripper::init(QWidget* parent) {
    _hParent = parent;
    // Win32: DWORD hwndExStyle = GetWindowLongPtr(_hParent, GWL_EXSTYLE);
    //        _isRTL = hwndExStyle & WS_EX_LAYOUTRTL;
    _isRTL = (parent->layoutDirection() == Qt::RightToLeft);
}

// =============================================================================
// startGrip — begin drag operation
//
// Win32:
//   1. GetDockInfo from DockingManager
//   2. Register window class "moveDlg" (MDLG_CLASS_NAME)
//   3. CreateWindowEx overlay window
//   4. SetWindowsHookEx(WH_MOUSE_LL, hookProcMouse, ...) and WH_KEYBOARD_LL
//   5. SetCapture(_hSelf), SetWindowPos(_pCont->getHSelf(), HWND_TOPMOST, ...)
//   6. Init tab information from TCM_* messages
//
// Qt6:
//   1. Get dock info from DockingManager
//   2. Create overlay widget (no class registration needed)
//   3. Connect mouse/keyboard tracking
//   4. Start drag: grabMouse() + show overlay
// =============================================================================
void Gripper::startGrip(DockingCont* pCont, DockingManager* pDockMgr) {
    _pDockMgr   = pDockMgr;
    _pCont      = pCont;

    // Win32: _pDockMgr->getDockInfo(&_dockData);
    // Qt6: TODO: [unhandled] DockingManager::getDockInfo() → returns tDockMgr
    // The docking manager needs to be ported first to provide this data

    if (!_isRegistered) {
        _isRegistered = true;
        // Win32: RegisterClass(&clz) for MDLG_CLASS_NAME
        // Qt6: no registration needed — QWidget handles this
    }

    // Win32: _hSelf = CreateWindowEx(0, MDLG_CLASS_NAME, L"", 0, ...)
    // Qt6: create overlay widget
    createOverlay();

    if (!_hOverlayWnd) {
        qWarning() << "Gripper::startGrip: failed to create overlay widget";
        return;
    }

    // Initialize drag state
    _isDragging = true;
    _bPtOldValid = false;

    // Record initial mouse position (Win32: GetCursorPos(&pt))
    QPoint pt = QCursor::pos();
    _ptOld = pt;

    // Win32: SetCapture(_hSelf) — Qt equivalent: overlay grabs mouse
    _hOverlayWnd->grabMouse();

    // Win32: SetWindowPos(_pCont->getHSelf(), HWND_TOPMOST, ...) to topmost
    // Qt6: overlay is already WindowStaysOnTop — no explicit SetWindowPos needed

    // Initialize tab reordering information
    initTabInformation();

    // Calculate mouse offset within container rect
    QRect rc;
    if (_pCont->isFloating()) {
        // Win32: GetWindowRect(_pCont->getHSelf(), &rc)
        rc = _pCont->window()->geometry();
    } else {
        // Win32: _pCont->getClientRect(rc); ScreenToClient(...)
        rc = _pCont->window()->rect();
        QPoint local = _pCont->window()->mapFromGlobal(pt);
        _ptOffset.setX(local.x());
        _ptOffset.setY(local.y());
        return;
    }

    _ptOffset.setX(pt.x() - rc.left());
    _ptOffset.setY(pt.y() - rc.top());

    // Win32: create() sets hookMouse = SetWindowsHookEx(WH_MOUSE_LL, ...)
    // Qt6: QApplication::installEventFilter on QApplication instance
    //       to capture mouse events globally during drag
    qApp->installEventFilter(this);
}

// =============================================================================
// eventFilter — global mouse/keyboard event handler (replaces WH_MOUSE_LL / WH_KEYBOARD_LL hooks)
//
// Win32: hookProcMouse receives WM_MOUSEMOVE, WM_LBUTTONUP with nCode >= 0
//        hookProcKeyboard receives VK_ESCAPE → DMM_CANCEL_MOVE
// Qt6:   QObject::eventFilter on QApplication captures all events during drag
// =============================================================================
bool Gripper::eventFilter(QObject* watched, QEvent* event) {
    if (!_isDragging) {
        return QObject::eventFilter(watched, event);
    }

    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease: {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseButtonRelease) {
            onMouseRelease(me);
        } else {
            onMouseMove(me);
        }
        return true;  // Consume event
    }
    case QEvent::KeyRelease: {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        onKeyRelease(ke);
        return true;
    }
    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}

// =============================================================================
// onMouseMove — update drag rectangle during drag
// =============================================================================
void Gripper::onMouseMove(QMouseEvent* event) {
    QPoint pt = event->globalPos();
    QPoint ptBuf;

    getMousePoints(pt, ptBuf);

    // Tab reordering during drag
    if (_startMovingFromTab) {
        doTabReordering(pt);
    }

    // Update drag rectangle
    drawRectangle(&pt);
}

// =============================================================================
// onMouseRelease — complete drag operation
// =============================================================================
void Gripper::onMouseRelease(QMouseEvent* event) {
    Q_UNUSED(event);

    if (!_isDragging) {
        return;
    }

    // Win32: UnhookWindowsHookEx(hookMouse); UnhookWindowsHookEx(hookKeyboard);
    // Qt6: remove event filter
    qApp->removeEventFilter(this);

    // Win32: DestroyWindow(_hSelf) — Qt6: hide and delete overlay
    destroyOverlay();

    _isDragging = false;

    // Win32: onButtonUp() — determine drop target, update container positions
    onButtonUp();
}

// =============================================================================
// onKeyRelease — handle Escape to cancel drag
//
// Win32: hookProcKeyboard catches VK_ESCAPE → PostMessage(DMM_CANCEL_MOVE)
// Qt6:   eventFilter catches QEvent::KeyRelease with Escape key
// =============================================================================
void Gripper::onKeyRelease(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        // Cancel drag
        qApp->removeEventFilter(this);
        drawRectangle(nullptr);  // Erase rectangle
        destroyOverlay();
        _isDragging = false;
        emit dragCancelled();
    }
}

// =============================================================================
// createOverlay — create transparent fullscreen overlay widget
//
// Win32:
//   - RegisterClassEx for "NppGripperOverlay"
//   - CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT, ...)
//   - SetLayeredWindowAttributes(clrMagenta, 0, LWA_COLORKEY)
//   - CreateCompatibleDC, CreateCompatibleBitmap for drawing
//
// Qt6:
//   - QWidget with FramelessWindowHint | Tool | WindowStaysOnTopHint
//   - WA_TranslucentBackground for color-key-like transparency
//   - setAttribute(Qt::WA_PaintOnScreen) = false
//   - Covers all screens via QDesktopWidget::screenGeometry()
// =============================================================================
void Gripper::createOverlay() {
    if (_hOverlayWnd) {
        return;
    }

    // Win32: virtual screen metrics → Qt6: QDesktopWidget
    QRect virtualScreen;
    const QList<QScreen*> screens = qApp->screens();
    if (screens.isEmpty()) {
        qWarning() << "Gripper: no screens available";
        return;
    }

    // Calculate virtual screen bounding rect
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (QScreen* screen : screens) {
        QRect sg = screen->geometry();
        minX = std::min(minX, sg.left());
        minY = std::min(minY, sg.top());
        maxX = std::max(maxX, sg.right());
        maxY = std::max(maxY, sg.bottom());
    }
    virtualScreen = QRect(minX, minY, maxX - minX + 1, maxY - minY + 1);

    // Win32: _hOverlayWnd = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT, ...)
    // Qt6: Frameless + Tool (always on top) + transparent background
    _hOverlayWnd = new QWidget(nullptr,
                               Qt::FramelessWindowHint |
                               Qt::Tool |
                               Qt::WindowStaysOnTopHint |
                               Qt::X11BypassWindowManagerHint);
    _hOverlayWnd->setAttribute(Qt::WA_TranslucentBackground, true);
    _hOverlayWnd->setAttribute(Qt::WA_NoSystemBackground, true);
    _hOverlayWnd->setGeometry(virtualScreen);
    _hOverlayWnd->show();
    _hOverlayWnd->raise();

    // Win32: _hdcOverlay = GetDC(_hOverlayWnd) — used for BitBlt
    // Qt6: not needed — QPainter paints directly on the overlay widget

    // Win32: hookMouse = SetWindowsHookEx(WH_MOUSE_LL, hookProcMouse, ...)
    // Qt6: event filter is installed in startGrip()

    // Create rubber band for drag feedback (replaces overlay DC drawing)
    _rubberBand = new QRubberBand(QRubberBand::Rectangle, _hOverlayWnd);
}

// =============================================================================
// destroyOverlay — clean up overlay resources
// =============================================================================
void Gripper::destroyOverlay() {
    if (_rubberBand) {
        _rubberBand->deleteLater();
        _rubberBand = nullptr;
    }

    if (_hOverlayWnd) {
        _hOverlayWnd->hide();
        _hOverlayWnd->deleteLater();
        _hOverlayWnd = nullptr;
    }
}

// =============================================================================
// drawRectangle — update drag feedback rectangle
//
// Win32: Uses layered overlay with magenta color-key. Draws dotted border using
//        PatBlt with DotPattern brush. Erases old rect with magenta fill.
// Qt6:   QRubberBand with custom styling — QPalette highlight color, 3px border.
//        Updates geometry on each move. Hidden when pPt == nullptr (erase).
// =============================================================================
void Gripper::drawRectangle(const QPoint* pPt) {
    if (!_rubberBand) {
        return;
    }

    QRect rcNew;

    if (pPt != nullptr) {
        // Calculate new rectangle
        QRect rc;
        getMovingRect(*pPt, &rc);
        rcNew = rc;

        // Skip if same as previous
        if (_bPtOldValid && rcNew == _rcPrev) {
            return;
        }

        _rcPrev = rcNew;

        // Win32: erase old rectangle (fill with magenta), draw new
        // Qt6: update rubber band geometry
        _rubberBand->setGeometry(rcNew);
        _rubberBand->show();
        _rubberBand->raise();

        _bPtOldValid = true;
    } else {
        // Erase — Win32: destroyOverlayWindow() called
        // Qt6: hide rubber band
        _rubberBand->hide();
        _bPtOldValid = false;
    }
}

// =============================================================================
// getMousePoints — track mouse position history
//
// Win32: ptPrev = _ptOld; _ptOld = pt;
// Qt6:   same logic with QPoint
// =============================================================================
void Gripper::getMousePoints(const QPoint& pt, QPoint& ptPrev) {
    ptPrev = _ptOld;
    _ptOld = pt;
}

// =============================================================================
// getMovingRect — calculate the drag rectangle for current mouse position
//
// Win32:
//   1. contHitTest(pt) → if hit, get container rect + correction
//   2. workHitTest(pt, rc) → if hit work region, use that rect
//   3. Otherwise use floating rect or window rect, apply offset + correction
// Qt6: same logic, HWND → QWidget*, RECT → QRect, GetWindowRect → QWidget::geometry()
// =============================================================================
void Gripper::getMovingRect(const QPoint& pt, QRect* rc) {
    QRect rcCorr;
    DockingCont* pContHit = nullptr;

    // Test if mouse hits a container
    pContHit = contHitTest(pt);

    if (pContHit != nullptr) {
        // Win32: GetWindowRect(pContHit->getHSelf(), rc);
        rc->setTopLeft(pContHit->window()->mapToGlobal(QPoint(0, 0)));
        rc->setSize(pContHit->window()->size());

        // Get correction rect
        if (_pCont->isFloating()) {
            // TODO: [unhandled] _pCont->getDataOfActiveTb()->rcFloat
            rcCorr = rc->normalized();
        } else {
            rcCorr = _pCont->window()->rect();
        }

        shrinkRectToSize(rc);
        shrinkRectToSize(&rcCorr);
        DoCalcGripperRect(rc, rcCorr, pt);
    } else {
        // Test if mouse is within work area
        pContHit = workHitTest(pt, rc);

        if (pContHit == nullptr) {
            // Use floating rect or window rect
            if (!_pCont->isFloating()) {
                // TODO: [unhandled] _pCont->getDataOfActiveTb()->rcFloat
                *rc = _pCont->window()->geometry();
            } else {
                *rc = _pCont->window()->geometry();
            }
            rcCorr = _pCont->window()->rect();

            calcRectToScreen(nullptr, rc);
            calcRectToScreen(nullptr, &rcCorr);

            rc->moveTo(pt.x() - _ptOffset.x(), pt.y() - _ptOffset.y());

            DoCalcGripperRect(rc, rcCorr, pt);
        }
    }
}

// =============================================================================
// contHitTest — test if point hits a docking container
//
// Win32: iterates vCont[], checks WindowFromPoint against getCaptionWnd(),
//        IsWindowVisible + PtInRect for tabs, TCM_* for tab hit testing.
// Qt6:   similar iteration over containers, QWidget::underMouse() or
//        QApplication::widgetAt() for hit testing.
// =============================================================================
DockingCont* Gripper::contHitTest(const QPoint& pt) {
    // TODO: [unhandled] _pDockMgr->getContainerInfo() → returns vector<DockingCont*>
    // Requires DockingManager and DockingCont to be ported first
    Q_UNUSED(pt);
    return nullptr;
}

// =============================================================================
// workHitTest — test if point hits a potential docking area (the 4 work regions)
//
// Win32: iterates DOCKCONT_MAX regions from _dockData.rcRegion[], applies
//        HIT_TEST_THICKNESS padding, MapWindowPoints to screen coords.
// Qt6:   same logic with QRect and QDesktopWidget coordinate mapping.
// =============================================================================
DockingCont* Gripper::workHitTest(const QPoint& pt, QRect* rc) {
    Q_UNUSED(pt);
    Q_UNUSED(rc);
    // TODO: [unhandled] requires _dockData from DockingManager
    return nullptr;
}

// =============================================================================
// onButtonUp — complete drag, reposition or dock container
//
// Win32: erase rectangle, hit-test drop target, then:
//   - If no target: calculate new float rect, toggleActiveWidget/toggleVisWidget → DMM_FLOAT
//   - If target != self: toggleActiveWidget/toggleVisWidget → DMM_DOCK
//   - MoveWindow to final position
// Qt6:   same logic, QWidget::setGeometry() instead of MoveWindow,
//        QDockWidget API for dock/undock (or direct container repositioning)
// =============================================================================
void Gripper::onButtonUp() {
    QPoint pt = QCursor::pos();

    if (!_bPtOldValid) {
        return;
    }

    // Erase drag rectangle
    drawRectangle(nullptr);

    // Hit test for drop target
    DockingCont* pDockCont = contHitTest(pt);
    if (pDockCont == nullptr) {
        pDockCont = workHitTest(pt);
    }

    if (pDockCont == nullptr) {
        // No target — float the container
        QRect rc;
        // TODO: [unhandled] calculate float rect from _pCont->getDataOfActiveTb()->rcFloat
        // rc = _pCont->getDataOfActiveTb()->rcFloat;

        if (_startMovingFromTab) {
            // TODO: [unhandled] _pDockMgr->toggleActiveWidget(_pCont, DMM_FLOAT, TRUE, &rc)
        } else if (!_pCont->isFloating()) {
            // TODO: [unhandled] _pDockMgr->toggleVisWidget(_pCont, DMM_FLOAT, &rc)
        }

        // Win32: MoveWindow(pContMove->getHSelf(), rc.left, ...)
        // Qt6: _pCont->window()->setGeometry(rc);
        // TODO: [unhandled] reposition the container window
    } else if (_pCont != pDockCont) {
        // Dock to target container
        if (_startMovingFromTab) {
            // TODO: [unhandled] _pDockMgr->toggleActiveWidget(_pCont, pDockCont);
        } else {
            // TODO: [unhandled] _pDockMgr->toggleVisWidget(_pCont, pDockCont);
        }
    }

    emit dragCompleted(pDockCont, QRect());
}

// =============================================================================
// doTabReordering — reorder tabs during drag
//
// Win32: iterates containers, checks TCM_HITTEST for tab under cursor,
//        TCM_INSERTITEM / TCM_DELETEITEM / TCM_GETITEMCOUNT / TCM_SETCURSEL
//        for reordering.
// Qt6:   QTabWidget::tabAt(), insertTab(), removeTab(), count(), setCurrentIndex()
// =============================================================================
void Gripper::doTabReordering(const QPoint& pt) {
    // TODO: [unhandled] requires QTabWidget integration
    // Win32 TCM_* equivalents in Qt6:
    //   TCM_HITTEST      → QTabBar::tabAt(pt)
    //   TCM_INSERTITEM   → QTabWidget::insertTab(index, ...)
    //   TCM_DELETEITEM   → QTabWidget::removeTab(index)
    //   TCM_GETITEMCOUNT → QTabWidget::count()
    //   TCM_GETITEMRECT  → QTabBar::tabRect(index)
    //   TCM_GETCURSEL    → QTabWidget::currentIndex()
    //   TCM_SETCURSEL    → QTabWidget::setCurrentIndex(index)
    //   TCM_GETITEM      → QTabWidget::tabData(index)
    Q_UNUSED(pt);
}

// =============================================================================
// initTabInformation — capture tab state at drag start
//
// Win32: TCM_GETITEMCOUNT, TCM_GETCURSEL, TCM_GETITEMRECT, TCM_GETITEM
// Qt6:   QTabWidget::count(), currentIndex(), tabBar()->tabRect(), tabData()
// =============================================================================
void Gripper::initTabInformation() {
    // TODO: [unhandled] requires _pCont->getTabWnd() → QTabWidget*
    // Win32: _hTabSource = _pCont->getTabWnd();
    //        _startMovingFromTab = _pCont->startMovingFromTab();
    //        TCM_GETITEMCOUNT, TCM_GETCURSEL, TCM_GETITEMRECT, TCM_GETITEM
}

// =============================================================================
// Geometry helpers (mirror Win32 functions)
// =============================================================================

// Win32: ShrinkRcToSize — convert rect (left,top,right,bottom) to (x,y,w,h)
void Gripper::shrinkRectToSize(QRect* rc) {
    if (_isRTL) {
        rc->setRight(rc->left() - rc->right());
    } else {
        rc->setRight(rc->width());
    }
    rc->setBottom(rc->height());
}

// Win32: ClientRectToScreenRect + ShrinkRcToSize
void Gripper::calcRectToScreen(QWidget* hWnd, QRect* rc) {
    Q_UNUSED(hWnd);
    // TODO: [unhandled] map from client to screen coordinates
    shrinkRectToSize(rc);
}

// Win32: DoCalcGripperRect — adjust rect when mouse is outside
void Gripper::doCalcGripperRect(QRect* rc, const QRect& rcCorr, const QPoint& pt) {
    if ((rc->left() + rc->width()) < pt.x()) {
        rc->setLeft(pt.x() - 20);
    }
    if ((rc->top() + rc->height()) < pt.y()) {
        rc->setTop(rc->top() + rcCorr.bottom() - rc->bottom());
    }
}

// Note: Drag feedback is handled by QRubberBand in _hOverlayWnd.
// The rubber band is styled by the overlay widget's stylesheet.
// Win32's PatBlt + dotted pattern → QPainter with QPen::setDashPattern
// (drawn on the rubber band via its internal paint; QRubberBand handles this)

// Note: Q_OBJECT moc is generated automatically by CMake's AUTOMOC.
// No manual moc include needed.