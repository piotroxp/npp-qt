// Gripper.cpp
#include "Gripper.h"
#include <QApplication>
#include <QScreen>

Gripper::Gripper(QWidget* parent)
    : QWidget(parent)
{
    setCursor(Qt::SizeAllCursor);
}

Gripper::~Gripper() = default;

void Gripper::startGrip(QWidget* panel)
{
    _panel = panel;
    _startPos = QCursor::pos();
    _isGripping = true;
    grabMouse(Qt::CrossCursor);
}

void Gripper::endGrip()
{
    releaseMouse();
    _isGripping = false;
    _panel = nullptr;
}

void Gripper::mouseMoveEvent(QMouseEvent* event)
{
    if (_isGripping && _panel)
    {
        QPoint currentPos = QCursor::pos();
        QPoint delta = currentPos - _startPos;
        emit gripMoved(delta);
    }
}

void Gripper::mouseReleaseEvent(QMouseEvent* event)
{
    if (_isGripping)
    {
        emit gripFinished();
        endGrip();
    }
}

void Gripper::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && _isGripping)
    {
        emit gripCancelled();
        endGrip();
        event->accept();
    }
}
