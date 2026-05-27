// DockingSplitter.h — Qt6 translation
#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QObject>

class DockingSplitter : public QObject
{
    Q_OBJECT

public:
    enum Direction { Horizontal, Vertical };

    explicit DockingSplitter(QWidget* parent = nullptr);
    void init(Direction dir);

signals:
    void splitterOffsetChanged(int offset);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Direction _direction = Horizontal;
    QWidget* _target = nullptr;
};