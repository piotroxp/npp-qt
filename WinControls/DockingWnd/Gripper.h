// This file is part of Notepad++ project
// Copyright (C)2006 Jens Lorenz <jens.plugin.npp@gmx.de>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QImage>
#include <QBrush>
#include <QStaticText>
#include <QObject>

#include "Docking.h"

// Forward declarations
class DockingCont;
class DockingManager;

/**
 * Gripper — Qt6 port of the Win32 drag-gripper for docking panels.
 *
 * Translates Win32 patterns:
 *   HWND           → QWidget*
 *   HDC/HDC memory → QPainter/QImage (double-buffered)
 *   WNDPROC        → Qt event handlers / signal-slot
 *   POINT/RECT     → QPointF/QRect
 *   GetWindowLong  → QWidget::windowFlags() / property()
 *   BitBlt dots    → QPainter drawPoint loop (dot pattern)
 *   Overlay window → QWidget overlay (for multi-monitor drag rectangle)
 */

// Dot pattern for drag rectangle (XOR raster ops → Qt painter)
static const unsigned short DotPattern[] = {
    0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
};

// Window class name for the gripper overlay
#define MDLG_CLASS_NAME QStringLiteral("moveDlg")


class Gripper final : public QObject
{
    Q_OBJECT

public:
    explicit Gripper(QObject* parent = nullptr);
    ~Gripper() override;

    void init(QWidget* parent);

    void startGrip(DockingCont* pCont, DockingManager* pDockMgr);

Q_SIGNALS:
    // Emitted when drag state changes
    void gripDragStarted();
    void gripDragEnded();
    void gripDragMoved(const QPoint& screenPos);

protected:
    // Qt event handlers (replaces WNDPROC)
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // Internal helpers
    void createGripper();
    void onMove(const QPoint& globalPos);
    void onButtonUp();

    void doTabReordering(const QPoint& pt);
    void drawRectangle(const QPoint& pt);
    void getMousePoints(const QPoint& pt, QPoint& ptPrev);
    void getMovingRect(const QPoint& pt, QRect* rc);
    DockingCont* contHitTest(const QPoint& pt);
    DockingCont* workHitTest(const QPoint& pt, QRect* rcCont = nullptr);

    void initTabInformation();

    // Geometry helpers (Win32 rect → Qt rect)
    void calcRectToScreen(QWidget* w, QRect* rc);
    void calcRectToClient(QWidget* w, QRect* rc);
    void shrinkRcToSize(QRect* rc);
    void doCalcGripperRect(QRect* rc, const QRect& rcCorr, const QPoint& pt);

    // Overlay window management (multi-monitor support)
    bool createOverlayWindow();
    void destroyOverlayWindow();
    bool eventFilterOverlay(QObject* watched, QEvent* event);

    // Dot pattern raster drawing (replaces BitBlt XOR)
    void drawDotPattern(QPainter* p, const QRect& rc, const QPoint& offset);
    QImage makeDotPatternImage(const QSize& size);

private:
    // Parent widget
    QWidget* _pParent = nullptr;

    // Docking data
    tDockMgr _dockData;
    DockingManager* _pDockMgr = nullptr;
    DockingCont* _pCont = nullptr;

    // Mouse offset in moving rectangle
    QPointF _ptOffset{0, 0};

    // Previous mouse point
    QPointF _ptOld{0, 0};
    bool _bPtOldValid = false;

    // Last drawn rectangle
    QRectF _rcPrev;

    // Tab info for sorting
    QWidget* _pTab = nullptr;
    QWidget* _pTabSource = nullptr;
    bool _startMovingFromTab = false;
    int _iItem = 0;
    QRectF _rcItem;
    // TCITEM data (port from Win32 tab control item)
    struct TcItemData {
        QString text;
        QVariant data;
    } _tcItemData;

    // Dot pattern resources
    QImage _dotPatternImage;
    QBrush _dotBrush;

    // Overlay window for multi-monitor drag rectangle
    QWidget* _pOverlayWnd = nullptr;
    QImage _overlayImage;
    QImage _overlayMemImage;

    // Layout direction (RTL support)
    bool _isRTL = false;

    // Static registration state
    static bool _isRegistered;
    static bool _isOverlayClassRegistered;
};