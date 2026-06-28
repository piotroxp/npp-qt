// Semantic Lift: Win32 SplitterContainer → Qt6 QSplitter-based implementation
// Original: PowerEditor/src/WinControls/SplitterContainer/
// Target: npp-qt/src/WinControls/SplitterContainer.h + .cpp

#pragma once

#include <QSplitter>
#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QVector>
#include <QPair>
#include "Window.h"
#include "SplitterContainer/Splitter.h"

// =============================================================================
// Splitter — single resize handle widget (lifted from Splitter.h + Splitter.cpp)
// =============================================================================

class Splitter : public QWidget
{
    Q_OBJECT

public:
    explicit Splitter(Qt::Orientation orient, QWidget* parent = nullptr);
    ~Splitter() override = default;

    void setSize(int size) { _splitterSize = size; update(); }
    int size() const { return _splitterSize; }

    void setRatio(double ratio) { _splitRatio = ratio; }
    double ratio() const { return _splitRatio; }

    void setFixed(bool fixed) { _isFixed = fixed; }
    bool isFixed() const { return _isFixed; }

    void setDraggable(bool draggable) { _draggable = draggable; }
    bool isDraggable() const { return _draggable; }

    void setFlags(unsigned int flags) { _flags = flags; }
    unsigned int flags() const { return _flags; }

    bool isVertical() const { return _orientation == Qt::Vertical; }

    void updateClickZones();

signals:
    void splitterMoved(double newRatio);
    void doubleClicked(Qt::MouseButton button);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int getClickZone(WH which) const;
    void adjustZoneToDraw(QRect& rc2def, ZONE_TYPE whichZone) const;
    void drawSplitter(QPainter& painter);
    void drawArrow(QPainter& painter, const QRect& rect, Arrow direction);
    bool isInLeftTopZone(const QPoint& p) const;
    bool isInRightBottomZone(const QPoint& p) const;
    void gotoTopLeft();
    void gotoRightBottom();

    Qt::Orientation _orientation;
    int _splitterSize = 4;
    double _splitRatio = 0.5;
    bool _isDraged = false;
    bool _isFixed = false;
    bool _draggable = true;
    unsigned int _flags = SV_ENABLERDBLCLK | SV_ENABLELDBLCLK;
    QPoint _dragStartPos;
    double _dragStartRatio = 0.5;
    bool _isLeftButtonDown = false;

    // Click zones for collapse arrows
    QRect _clickZone2TL;
    QRect _clickZone2BR;

    // Static registration flags (legacy Win32 compatibility — no-op in Qt)
    static bool _isHorizontalRegistered;
    static bool _isVerticalRegistered;
    static bool _isHorizontalFixedRegistered;
    static bool _isVerticalFixedRegistered;
};

// =============================================================================
// SplitterContainer — two-pane container with splitter (lifted from SplitterContainer.h)
// =============================================================================

class SplitterContainer : public WindowBase
{
    Q_OBJECT

public:
    explicit SplitterContainer(QWidget* parent = nullptr);
    ~SplitterContainer() override = default;

    // Window interface
    void display(bool toShow = true) override { toShow ? show() : hide(); }
    void show() override { QWidget::show(); }
    void hide() override { QWidget::hide(); }
    int getHeight() const override { return rect().height(); }
    int getWidth() const override { return rect().width(); }
    QRect getClientRect() const override { return rect(); }
    void destroy() override { deleteLater(); }
    void init(void*, QWidget*) override {}
    void redraw(bool forceUpdate = false) override { update(); if (forceUpdate) repaint(); }
    bool isVisible() const override { return QWidget::isVisible(); }
    QWidget* getHParent() const override { return parentWidget(); }

    // Create with two windows
    void create(QWidget* pWin0, QWidget* pWin1, int splitterSize,
                SplitterMode mode = SplitterMode::DYNAMIC, int ratio = 50, bool isVertical = true);

    // Window management
    void setWin0(QWidget* pWin) { _pWin0 = pWin; relayout(); }
    void setWin1(QWidget* pWin) { _pWin1 = pWin; relayout(); }
    QWidget* win0() const { return _pWin0; }
    QWidget* win1() const { return _pWin1; }

    // Splitter control
    void setSplitterSize(int size);
    int splitterSize() const { return _splitterSize; }

    // Ratio (0.0 - 1.0)
    void setRatio(double ratio);
    double ratio() const;

    // Orientation
    bool isVertical() const { return _splitter->orientation() == Qt::Vertical; }

public slots:
    void rotate();
    void onSplitterMoved(double newRatio);

signals:
    void splitterMoved(double newRatio);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void relayout();

    QWidget* _pWin0 = nullptr;
    QWidget* _pWin1 = nullptr;
    QSplitter* _splitter = nullptr;
    Splitter* _customSplitter = nullptr;
    int _splitterSize = 4;
    double _ratio = 0.5;
    SplitterMode _splitterMode = SplitterMode::DYNAMIC;
};
