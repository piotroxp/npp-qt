// ScintillaAccelerator.cpp - Qt port
#include "ScintillaAccelerator.h"

ScintillaAccelerator::ScintillaAccelerator(QObject* parent) : QObject(parent) {}

bool ScintillaAccelerator::init(QVector<QWidget*>* /*views*/, QMenuBar* /*menu*/, QWidget* /*parent*/) {
    return true;
}

void ScintillaAccelerator::updateKeys() {
    // Scintilla handles its own key bindings via SCI commands
}
