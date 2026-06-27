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

// Splitter flags
constexpr int SV_HORIZONTAL       = 0x00000001;
constexpr int SV_VERTICAL         = 0x00000002;
constexpr int SV_FIXED           = 0x00000004;
constexpr int SV_ENABLERDBLCLK    = 0x00000008;  // enable right double-click
constexpr int SV_ENABLELDBLCLK    = 0x00000010;  // enable left double-click
constexpr int SV_RESIZEWTHPERCNT = 0x00000020;  // resize with percentage

enum class SplitterMode : quint8 {
    DYNAMIC,
    LEFT_FIX,
    RIGHT_FIX
};

enum class Arrow { left, up, right, down };

// =============================================================================
// Splitter — single resize handle widget (lifted from Splitter.h)
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
    void drawSplitter(QPainter& painter);
    void drawArrow(QPainter& painter, const QRect& rect, Arrow direction);

    Qt::Orientation _orientation;
    int _splitterSize = 4;
    double _splitRatio = 0.5;
    bool _isDragging = false;
    bool _isFixed = false;
    bool _draggable = true;
    unsigned int _flags = SV_ENABLERDBLCLK | SV_ENABLELDBLCLK;
    QPoint _dragStartPos;
    double _dragStartRatio;
};

// =============================================================================
// SplitterContainer — two-pane container with splitter (lifted from SplitterContainer.h)
// =============================================================================

class SplitterContainer : public Window
{
    Q_OBJECT

public:
    explicit SplitterContainer(QWidget* parent = nullptr);
    ~SplitterContainer() override = default;

    // Create with two windows
    void create(QWidget* pWin0, QWidget* pWin1, int splitterSize,
                SplitterMode mode = SplitterMode::DYNAMIC, int ratio = 50, bool isVertical = true);

    // Win32 API compatibility (not called in Qt6 but declared to satisfy callers)
    void init(QWidget* /*hwnd*/) {}  // no-op on Qt6

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

    // Visibility (not virtual in QWidget, just public methods)
    void show();
    void hide();

public slots:
    void rotate();

signals:
    void splitterMoved(double newRatio);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSplitterMoved(double newRatio);

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
