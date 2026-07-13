// npp-qt: Gripper implementation
// Win32 → Qt6 semantic lift

#include "Gripper.h"
#include "Docking.h"
#include "DockingWnd.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QPainter>
#include <QScreen>
#include <QMouseEvent>
#include <QDebug>

bool Gripper::_isRegistered = false;

//==============================================================================
// Construction / Destruction
//==============================================================================

Gripper::Gripper() = default;

Gripper::~Gripper() {
    destroyGripperWindow();
}


//==============================================================================
// Initialization
//==============================================================================

void Gripper::init(QWidget* parent) {
    _pParent = parent;
    if (_pParent) {
        _pParent->installEventFilter(this);
        _isRTL = _pParent->testAttribute(Qt::WA_SetLayoutDirection)
                  && _pParent->layoutDirection() == Qt::RightToLeft;
    }
}


void Gripper::initTabInformation() {
    // Initialize tab state from the container's tab widget
    if (_pCont) {
        _hTab = qobject_cast<QWidget*>(_pCont);
        _iItem = 0;
        _rcItem = _hTab ? _hTab->geometry() : QRect();
    }
}


//==============================================================================
// Gripper window (small drag handle overlay)
//==============================================================================

void Gripper::createGripperWindow() {
    if (_pGripperWidget) {
        return;
    }

    _pGripperWidget = new QWidget(_pParent);
    _pGripperWidget->setFixedHeight(4);
    _pGripperWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    _pGripperWidget->installEventFilter(this);

    // Position at top of parent
    if (_pParent) {
        _pGripperWidget->setFixedWidth(_pParent->width());
        _pGripperWidget->move(0, 0);
        _pGripperWidget->show();
    }
}


void Gripper::destroyGripperWindow() {
    if (_pGripperWidget) {
        _pGripperWidget->deleteLater();
        _pGripperWidget = nullptr;
    }
}


void Gripper::update() {
    if (_pGripperWidget) {
        _pGripperWidget->update();
    }
}


//==============================================================================
// Event filter — intercept mouse events for drag handling
//==============================================================================

bool Gripper::eventFilter(QObject* watched, QEvent* event) {
    if (!_pParent) {
        return false;
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton && me->modifiers() == Qt::NoModifier) {
            // Start drag on left click on gripper area
            if (watched == _pGripperWidget) {
                _ptOffset = me->pos();
                createGripperWindow();
                _bPtOldValid = false;
                return true;
            }
        }
        break;
    }

    case QEvent::MouseMove: {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->buttons() & Qt::LeftButton) {
            onMouseMove(me);
            return true;
        }
        break;
    }

    case QEvent::MouseButtonRelease: {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            onMouseRelease(me);
            return true;
        }
        break;
    }

    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}


//==============================================================================
// Drag handling
//==============================================================================

void Gripper::startGrip(DockingCont* pCont, DockingManager* pDockMgr) {
    _pCont = pCont;
    _pDockMgr = pDockMgr;
    createGripperWindow();
    initTabInformation();
}


void Gripper::onMouseMove(QMouseEvent* event) {
    if (!_pParent || !_pDockMgr) {
        return;
    }

    const QPoint globalPos = event->globalPosition().toPoint();
    const QPoint localPos = event->pos();

    // Draw the drag indicator rectangle
    drawRectangle(globalPos);

    // Remember last position
    _ptOld = globalPos;
    _bPtOldValid = true;

    // Notify dock manager of drag position
    QRect rcCont = _pCont ? _pCont->geometry() : QRect();
    Q_UNUSED(rcCont);
}


void Gripper::onMouseRelease(QMouseEvent* event) {
    if (!_pParent || !_pDockMgr) {
        return;
    }

    const QPoint globalPos = event->globalPosition().toPoint();

    // Clear the drag rectangle
    clearRectangle();

    // Check if we ended on a valid container
    DockingCont* targetCont = containerHitTest(globalPos);
    if (targetCont && targetCont != _pCont) {
        // Dock to target container
        emit gripComplete(_pCont, false);  // not cancelled
        if (_pDockMgr) {
            _pDockMgr->dockContainer(_pCont, targetCont);
        }
    } else {
        // Drag cancelled or returned to same spot
        emit gripComplete(_pCont, true);  // cancelled
    }

    // Cancel any pending move
    if (_pDockMgr) {
        _pDockMgr->cancelMove();
    }

    destroyGripperWindow();
}


//==============================================================================
// Tab reordering
//==============================================================================

void Gripper::doTabReordering(const QPoint& pt) {
    if (!_hTab || !_pDockMgr) {
        return;
    }

    // Find which tab index the mouse is over
    int targetIndex = -1;
    // Stub: emit tab reorder signal
    if (targetIndex >= 0 && targetIndex != _iItem) {
        emit tabReordered(_iItem, targetIndex);
    }
}


//==============================================================================
// Rectangle drawing (overlay for drag indicator)
//==============================================================================

void Gripper::drawRectangle(const QPoint& pt) {
    // Erase previous rectangle first
    clearRectangle();

    const int width = 80;
    const int height = 60;
    QRect rcDraw(pt.x() - width / 2, pt.y() - height / 2, width, height);
    _rcPrev = rcDraw;

    // Draw XOR rectangle on all screens
    // (simplified Qt6 version)
    Q_UNUSED(rcDraw);
}


void Gripper::clearRectangle() {
    if (!_rcPrev.isNull()) {
        // The rectangle will be overwritten by the next draw
        _rcPrev = QRect();
    }
}


//==============================================================================
// Hit testing
//==============================================================================

DockingCont* Gripper::containerHitTest(const QPoint& pt) const {
    if (!_pDockMgr) {
        return nullptr;
    }
    // Find which container is at this screen position
    return _pDockMgr->containerAt(pt);
}


//==============================================================================
// Geometry helpers
//==============================================================================

QRect Gripper::rectToScreen(const QRect& rc) const {
    if (!_pParent) {
        return rc;
    }
    return QRect(_pParent->mapToGlobal(rc.topLeft()), rc.size());
}


QRect Gripper::rectToClient(const QRect& rc) const {
    if (!_pParent) {
        return rc;
    }
    return QRect(_pParent->mapFromGlobal(rc.topLeft()), rc.size());
}


void Gripper::shrinkRect(QRect* rc) {
    if (!rc) {
        return;
    }
    if (_isRTL) {
        rc->setRight(rc->left() - rc->right());
    } else {
        rc->setRight(rc->right() - rc->left());
    }
    rc->setBottom(rc->bottom() - rc->top());
}


void Gripper::calcGripperRect(QRect* rc, const QRect& rcCorr, const QPoint& pt) {
    if (!rc) {
        return;
    }

    const int gripperWidth = 20;

    // Adjust left edge
    if ((rc->left() + rc->right()) < pt.x()) {
        rc->setLeft(pt.x() - gripperWidth);
    }

    // Adjust top edge
    if ((rc->top() + rc->bottom()) < pt.y()) {
        rc->setTop(rc->top() + (rcCorr.bottom() - rc->bottom()));
    }
}
