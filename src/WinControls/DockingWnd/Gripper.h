// Gripper.h — Qt6 translation (drag handle for panels)
#pragma once

#include <QWidget>
#include <QMouseEvent>

class Gripper : public QWidget
{
    Q_OBJECT

public:
    explicit Gripper(QWidget* parent = nullptr);
    ~Gripper() override;

    void startGrip(QWidget* panel);
    void endGrip();

signals:
    void gripMoved(const QPoint& delta);
    void gripFinished();
    void gripCancelled();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QWidget* _panel = nullptr;
    QPoint _startPos;
    bool _isGripping = false;
};
