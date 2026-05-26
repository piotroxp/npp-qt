// Splitter.h — Qt6 translation: Splitter window
#pragma once

#include <QSplitter>
#include <QWidget>

enum class SplitterMode { Dynamic, LeftFix, RightFix };

class Splitter : public QSplitter
{
    Q_OBJECT

public:
    Splitter(QWidget* parent = nullptr);
    Splitter(Qt::Orientation orientation, QWidget* parent = nullptr);

    void init(int splitterSize, double splitRatio = 50.0, unsigned flags = 0);

    // Enable double-click to reset to 50/50
    void setEnableResetOnDblClick(bool enable) { _resetOnDblClick = enable; }

    // Set fixed side
    void setMode(SplitterMode mode) { _mode = mode; }
    SplitterMode mode() const { return _mode; }

    // Rotation
    void rotate();

    // Arrow click zones
    enum class ArrowZone { None, TopLeft, BottomRight };
    ArrowZone hitTest(const QPoint& pos) const;
    void gotoTopLeft();
    void gotoBottomRight();

signals:
    void splitterMoved(int newPos);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    SplitterMode _mode = SplitterMode::Dynamic;
    bool _resetOnDblClick = true;
    int _splitterSize = 4;
    double _splitRatio = 50.0;
};

// Splitter container manages two windows with a splitter
class SplitterContainer : public QWidget
{
    Q_OBJECT

public:
    SplitterContainer(QWidget* parent = nullptr);
    ~SplitterContainer() override;

    void create(QWidget* win0, QWidget* win1, int splitterSize = 4,
               SplitterMode mode = SplitterMode::Dynamic,
               int ratio = 50, bool isVertical = true);

    void setWin0(QWidget* win) { _win0 = win; }
    void setWin1(QWidget* win) { _win1 = win; }

    bool isVertical() const { return _splitter && _splitter->orientation() == Qt::Vertical; }
    void rotateToLeft();
    void rotateToRight();

    void resizeSplitter(const QRect& rect);

signals:
    void splitterMoved(int pos);
    void popupMenuRequested(const QPoint& pos);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QWidget* _win0 = nullptr;
    QWidget* _win1 = nullptr;
    QSplitter* _splitter = nullptr;
    SplitterMode _mode = SplitterMode::Dynamic;
    int _ratio = 50;
    int _splitterSize = 4;
};
