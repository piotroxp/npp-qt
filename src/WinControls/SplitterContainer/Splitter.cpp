// Splitter.cpp — Qt6 translation
#include "Splitter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>

// ─── Splitter ─────────────────────────────────────────────────

Splitter::Splitter(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent)
{
}

Splitter::Splitter(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
{
}

void Splitter::init(int splitterSize, double splitRatio, unsigned /*flags*/)
{
    _splitterSize = splitterSize;
    _splitRatio = splitRatio;
    setHandleWidth(splitterSize);

    // Initial sizes
    QList<int> sizes;
    int total = 1000;
    int first = int(total * splitRatio / 100.0);
    sizes << first << (total - first);
    setSizes(sizes);

    connect(this, &QSplitter::splitterMoved, this, &Splitter::splitterMoved);
}

void Splitter::rotate()
{
    if (orientation() == Qt::Horizontal)
        setOrientation(Qt::Vertical);
    else
        setOrientation(Qt::Horizontal);
}

Splitter::ArrowZone Splitter::hitTest(const QPoint& pos) const
{
    // Check corner arrows
    // Simplified: would need style-aware hit testing
    Q_UNUSED(pos);
    return ArrowZone::None;
}

void Splitter::gotoTopLeft()
{
    QList<int> sizes;
    int total = width();
    sizes << 1 << (total - 1);
    setSizes(sizes);
}

void Splitter::gotoBottomRight()
{
    QList<int> sizes;
    int total = width();
    sizes << (total - 1) << 1;
    setSizes(sizes);
}

void Splitter::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (_resetOnDblClick)
    {
        QList<int> sizes;
        int total = orientation() == Qt::Horizontal ? width() : height();
        int half = total / 2;
        sizes << half << (total - half);
        setSizes(sizes);
        QSplitter::mouseDoubleClickEvent(event);
    }
}

void Splitter::wheelEvent(QWheelEvent* event)
{
    QSplitter::wheelEvent(event);
}

// ─── SplitterContainer ───────────────────────────────────────

SplitterContainer::SplitterContainer(QWidget* parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
}

SplitterContainer::~SplitterContainer() = default;

void SplitterContainer::create(QWidget* win0, QWidget* win1, int splitterSize,
                             SplitterMode mode, int ratio, bool isVertical)
{
    _win0 = win0;
    _win1 = win1;
    _splitterSize = splitterSize;
    _mode = mode;
    _ratio = ratio;

    // Remove old splitter if any
    if (_splitter)
    {
        layout()->removeWidget(_splitter);
        delete _splitter;
    }

    // Create new splitter
    _splitter = new Splitter(isVertical ? Qt::Vertical : Qt::Horizontal, this);
    _splitter->init(splitterSize, ratio);

    // Handle right-click menu
    _splitter->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_splitter, &Splitter::customContextMenuRequested,
            this, &SplitterContainer::popupMenuRequested);

    _splitter->addWidget(_win0);
    _splitter->addWidget(_win1);

    layout()->addWidget(_splitter);
}

void SplitterContainer::rotateToLeft()
{
    if (_splitter)
    {
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2)
        {
            _splitter->setOrientation(Qt::Vertical);
            // Swap widget order
            _splitter->addWidget(_splitter->widget(1));
            _splitter->addWidget(_splitter->widget(0));
        }
    }
}

void SplitterContainer::rotateToRight()
{
    if (_splitter)
    {
        _splitter->setOrientation(Qt::Horizontal);
        QList<int> sizes = _splitter->sizes();
        if (sizes.size() == 2)
        {
            _splitter->addWidget(_splitter->widget(0));
            _splitter->addWidget(_splitter->widget(1));
        }
    }
}

void SplitterContainer::resizeSplitter(const QRect& rect)
{
    setGeometry(rect);
    if (_splitter)
        _splitter->resize(rect.size());
}

void SplitterContainer::contextMenuEvent(QContextMenuEvent* event)
{
    emit popupMenuRequested(event->globalPos());
}
