#include "DockingSplitter.h"

DockingSplitter::DockingSplitter(QWidget* parent) : QObject(parent), _target(parent) {}
void DockingSplitter::init(Direction dir) { _direction = dir; }

bool DockingSplitter::eventFilter(QObject* obj, QEvent* event)
{
    (void)obj;
    (void)event;
    return false;
}
