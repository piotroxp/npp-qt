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
#include "Splitter.h"
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

    int splitterFixX(int s) const { return s>0 ? s : width() / 2; }
    int splitterFixY(int s) const { return s>0 ? s : height() / 2; }

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
