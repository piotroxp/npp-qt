#include "MISC/Common/shortcut.h"

Accelerator::Accelerator(QObject* parent) : QObject(parent) {}
Accelerator::~Accelerator() = default;
void Accelerator::init(QMenu*, QWidget*) {}
void Accelerator::updateShortcuts() {}
void Accelerator::updateFullMenu() {}
