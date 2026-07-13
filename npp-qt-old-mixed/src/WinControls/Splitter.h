// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: Splitter.h — Win32 Splitter → Qt6 QWidget with signals/slots
// Lifts WNDPROC + CS_* + WS_* + WM_* Win32 patterns to Qt6 equivalents.

#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QCursor>
#include <algorithm>
#include <cstdint>

// =============================================================================
// Win32 flag lift: SV_* → Qt-friendly member flags
// =============================================================================
#define SV_HORIZONTAL       0x00000001
#define SV_VERTICAL         0x00000002
#define SV_FIXED            0x00000004
#define SV_ENABLERDBLCLK    0x00000008   // enable right-button collapse zone
#define SV_ENABLELDBLCLK    0x00000010   // enable left-button collapse zone
#define SV_RESIZEWTHPERCNT  0x00000020   // resize child windows proportionally

// User message base (mirrors SPLITTER_USER in original Window.h)
#define SPLITTER_USER 0x0400

#define WM_GETSPLITTER_X     (SPLITTER_USER + 1)
#define WM_GETSPLITTER_Y     (SPLITTER_USER + 2)
#define WM_DOPOPUPMENU       (SPLITTER_USER + 3)
#define WM_RESIZE_CONTAINER  (SPLITTER_USER + 4)

// =============================================================================
// Local types lifted from original
// =============================================================================
enum class Arrow { left, up, right, down };
enum class WH { height, width };
enum class ZONE_TYPE { bottomRight, topLeft };
enum class SplitterMode : std::uint8_t { DYNAMIC, LEFT_FIX, RIGHT_FIX };

// =============================================================================
// Splitter — single resize handle / divider bar widget
// Semantic lift: HWND-based CS_* registered class → QWidget subclass
// =============================================================================

class Splitter : public QWidget
{
    Q_OBJECT

public:
    explicit Splitter(QWidget* parent = nullptr);
    ~Splitter() override = default;

    // init() mirrors Win32 CreateWindowEx + RegisterClass setup
    void init(QWidget* hParent, int splitterSize, double splitRatio, unsigned int flags);

    // Compatibility with existing Window interface
    void destroy();
    void display(bool show = true) { show ? QWidget::show() : QWidget::hide(); }

    void resizeSpliter(QRect* pRect = nullptr);

    // Accessors
    int splitterSize() const { return _splitterSize; }
    double splitPercent() const { return _splitPercent; }
    bool isVertical() const { return (_flags & SV_VERTICAL) != 0; }
    bool isFixed() const { return _isFixed; }
    unsigned int flags() const { return _flags; }
    QWidget* parentWindow() const { return _hParent; }

    void rotate();

signals:
    // Emitted when the splitter ratio changes (lifted from WM_RESIZE_CONTAINER)
    void splitterRatioChanged(double newRatio);
    // Emitted for right-click context (lifted from WM_DOPOPUPMENU)
    void popupMenuRequested(QMouseEvent* event);

public slots:
    // Collapse to top/left zone (SV_ENABLELDBLCLK)
    void gotoTopLeft();
    // Collapse to bottom/right zone (SV_ENABLERDBLCLK)
    void gotoRightBottom();

protected:
    // Qt event overrides — lifted from WNDPROC message handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // Core geometry helpers (lifted from original)
    int getClickZone(WH which) const;
    void adjustZoneToDraw(QRect& rc2def, ZONE_TYPE whichZone) const;
    void drawSplitter(QPainter& painter);
    void drawArrow(QPainter& painter, const QRect& rect, Arrow direction);
    bool isInLeftTopZone(const QPoint& p) const;
    bool isInRightBottomZone(const QPoint& p) const;

    // Fixed-mode position queries (lifted from WM_GETSPLITTER_X/Y)
    int getSplitterFixPosX() const;
    int getSplitterFixPosY() const;

    // Dark-mode cursor
    void updateCursor(const QPoint& pos);

    // Internal state
    QRect _rect;
    double _splitPercent = 0.0;
    int _splitterSize = 0;
    bool _isDraged = false;
    bool _isLeftButtonDown = false;
    unsigned int _flags = 0;
    bool _isFixed = false;
    QWidget* _hParent = nullptr;

    // Click zones (lifted from _clickZone2TL / _clickZone2BR)
    QRect _clickZone2TL;
    QRect _clickZone2BR;

    // Drag snapshot
    QPoint _dragStartPos;
    double _dragStartRatio = 0.5;
    int _dragStartTopLeft = 0;

    // Cursor state
    bool _isInZone = false;

    // Static registration flags — no-op in Qt, kept for binary compat
    static bool _isHorizontalRegistered;
    static bool _isVerticalRegistered;
    static bool _isHorizontalFixedRegistered;
    static bool _isVerticalFixedRegistered;
};
