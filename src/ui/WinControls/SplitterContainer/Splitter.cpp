// Splitter.cpp - Qt6 splitter widget
#include "Splitter.h"

Splitter::Splitter(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent) {
    setChildrenCollapsible(false);
    setHandleWidth(4);
}

void Splitter::saveStateTo(QByteArray& state) const {
    state = saveState();
}

void Splitter::restoreStateFrom(const QByteArray& state) {
    restoreState(state);
}

void Splitter::setEditorWidget(QWidget* editor) {
    _editorWidget = editor;
}

void Splitter::showPanel(int index) {
    if (index >= 0 && index < count()) {
        widget(index)->show();
    }
}

void Splitter::hidePanel(int index) {
    if (index >= 0 && index < count()) {
        widget(index)->hide();
    }
}
