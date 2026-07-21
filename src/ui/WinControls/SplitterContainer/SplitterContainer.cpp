// SplitterContainer.cpp
#include "SplitterContainer.h"

SplitterContainer::SplitterContainer(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent) {
    setChildrenCollapsible(false);
    setHandleWidth(4);
}

QByteArray SplitterContainer::savePanelState() const {
    return QSplitter::saveState();
}

void SplitterContainer::restorePanelState(const QByteArray& state) {
    QSplitter::restoreState(state);
}

void SplitterContainer::addPanel(QWidget* w) {
    addWidget(w);
}

void SplitterContainer::removePanel(int index) {
    if (index >= 0 && index < count()) {
        widget(index)->hide();
        widget(index)->setParent(nullptr);
    }
}

void SplitterContainer::setActivePanel(int index) {
    if (index >= 0 && index < count()) {
        _activePanel = index;
        emit panelChanged(index);
    }
}
