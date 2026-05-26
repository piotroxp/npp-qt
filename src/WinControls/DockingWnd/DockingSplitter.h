// DockingSplitter.h — Qt6 translation
#pragma once

#include <QWidget>

class DockingSplitter : public QWidget
{
    Q_OBJECT

public:
    enum Direction { Horizontal, Vertical };

    explicit DockingSplitter(QWidget* parent = nullptr);
    void init(Direction dir);

signals:
    void splitterMoved(int offset);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Direction _direction = Horizontal;
    bool _isDragging = false;
    QPoint _dragStart;
    int _startOffset = 0;
};
