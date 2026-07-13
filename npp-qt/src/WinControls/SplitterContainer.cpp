// Semantic Lift: Win32 SplitterContainer → Qt6 implementation
// Original: PowerEditor/src/WinControls/SplitterContainer/
// Target: npp-qt/src/WinControls/SplitterContainer.cpp

#include "SplitterContainer.h"
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QLayout>

// =============================================================================
// SplitterContainer
// =============================================================================

SplitterContainer::SplitterContainer(QWidget* parent)
    : Window(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

void SplitterContainer::create(QWidget* pWin0, QWidget* pWin1, int splitterSize,
                                SplitterMode mode, int ratio, bool isVertical) {
    _pWin0 = pWin0;
    _pWin1 = pWin1;
    _splitterSize = splitterSize;
    _splitterMode = mode;
    _ratio = ratio / 100.0;

    // Remove old splitter if any
    if (_splitter) {
        layout()->removeWidget(_splitter);
        delete _splitter;
    }

    // Create new splitter
    _splitter = new QSplitter(isVertical ? Qt::Vertical : Qt::Horizontal, this);
    _splitter->setChildrenCollapsible(false);
    _splitter->setHandleWidth(splitterSize);

    if (pWin0) _splitter->addWidget(pWin0);
    if (pWin1) _splitter->addWidget(pWin1);

    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 1);

    layout()->addWidget(_splitter);

    // Set initial ratio
    QList<int> sizes;
    int total = isVertical ? height() : width();
    if (total <= 0) total = 500;
    int s0 = static_cast<int>(total * _ratio);
    int s1 = total - s0;
    sizes << s0 << s1;
    _splitter->setSizes(sizes);

    connect(_splitter, &QSplitter::splitterMoved, this, &SplitterContainer::onSplitterMoved);
}

void SplitterContainer::setSplitterSize(int size) {
    _splitterSize = size;
    if (_splitter) {
        _splitter->setHandleWidth(size);
    }
}

void SplitterContainer::setRatio(double ratio) {
    _ratio = qBound(0.05, ratio, 0.95);
    if (_splitter) {
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2) {
            int total = sizes[0] + sizes[1];
            if (total > 0) {
                sizes[0] = static_cast<int>(total * _ratio);
                sizes[1] = total - sizes[0];
                _splitter->setSizes(sizes);
            }
        }
    }
}

double SplitterContainer::ratio() const {
    if (_splitter) {
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2 && (sizes[0] + sizes[1]) > 0) {
            return static_cast<double>(sizes[0]) / (sizes[0] + sizes[1]);
        }
    }
    return _ratio;
}

void SplitterContainer::rotate() {
    if (!_splitter) return;
    Qt::Orientation newOrient = (_splitter->orientation() == Qt::Vertical) ? Qt::Horizontal : Qt::Vertical;
    _splitter->setOrientation(newOrient);
    relayout();
}

void SplitterContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayout();
}

void SplitterContainer::relayout() {
    // QSplitter handles its own layout, this is for future custom handling
}

void SplitterContainer::onSplitterMoved(double newRatio) {
    _ratio = newRatio;
    emit splitterMoved(newRatio);
}


