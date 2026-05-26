// TabBar.cpp — Qt6 translation of CTabCtrl → QTabWidget
#include "TabBar.h"
#include <QApplication>
#include <QPainter>
#include <QStyleOptionTab>
#include <QStyle>
#include <QDrag>
#include <QMimeData>

// ─── Base TabBar ───────────────────────────────────────────────────────────

TabBar::TabBar(QWidget* parent)
    : QTabBar(parent)
{
    setTabsClosable(false); // We'll handle ourselves
    setMovable(true);
    setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);

    // Track hover state
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
}

TabBar::~TabBar() = default;

int TabBar::insertAtEnd(const QString& tabName)
{
    int index = addTab(tabName);
    _nbItem = count();
    return index;
}

void TabBar::activateAt(int index)
{
    if (index >= 0 && index < count())
        setCurrentIndex(index);
}

void TabBar::deleteItemAt(size_t index)
{
    if (index < static_cast<size_t>(count()))
        removeTab(static_cast<int>(index));
    _nbItem = count();
}

void TabBar::deleteAllItems()
{
    while (count() > 0)
        removeTab(0);
    _nbItem = 0;
}

void TabBar::setImageList(const QVector<QIcon>& icons)
{
    _hasImgLst = true;
    for (int i = 0; i < qMin(count(), icons.size()); ++i)
        setTabIcon(i, icons[i]);
}

void TabBar::setCloseBtnImageList(const QVector<QIcon>& normalIcons)
{
    _closeBtnIcons = normalIcons;
}

void TabBar::setPinBtnImageList(const QVector<QIcon>& pinIcons)
{
    _pinBtnIcons = pinIcons;
}

int TabBar::getNextOrPrevTabIdx(bool isNext) const
{
    int lastTabIdx = count() - 1;
    int selTabIdx = currentIndex();
    if (isNext)
    {
        if (++selTabIdx > lastTabIdx)
            selTabIdx = 0;
    }
    else
    {
        if (--selTabIdx < 0)
            selTabIdx = lastTabIdx;
    }
    return selTabIdx;
}

void TabBar::resizeIconsDpi(int /*dpi*/)
{
    // DPI handled automatically in Qt via devicePixelRatio
}

void TabBar::mousePressEvent(QMouseEvent* event)
{
    int index = tabAt(event->pos());

    if (event->button() == Qt::MiddleButton && index >= 0)
    {
        // Middle-click close
        emit tabDelete(index);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        if (index >= 0)
        {
            _mightBeDragging = true;
            _dragCount = 0;
        }
    }

    QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (_mightBeDragging && !_isDragging)
    {
        ++_dragCount;
        if (_dragCount > 3)
        {
            int index = tabAt(event->pos());
            if (index >= 0)
            {
                _srcTab = index;
                _isDragging = true;
                startDrag(index);
            }
        }
    }

    if (_isDragging)
    {
        handleDrag(_srcTab, event->globalPos());
    }

    QTabBar::mouseMoveEvent(event);
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
    _mightBeDragging = false;
    _dragCount = 0;

    if (_isDragging)
    {
        _isDragging = false;
        endDrag();
        emit tabDropped(tabAt(event->pos()));
    }

    QTabBar::mouseReleaseEvent(event);
}

void TabBar::wheelEvent(QWheelEvent* event)
{
    // Allow scroll through tabs
    if (event->angleDelta().y() > 0)
        activateAt(getNextOrPrevTabIdx(false));
    else
        activateAt(getNextOrPrevTabIdx(true));
    event->accept();
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::leaveEvent(QEvent* event)
{
    if (_currentHoverTab >= 0)
    {
        _currentHoverTab = -1;
        emit tabMouseLeaving();
    }
    QTabBar::leaveEvent(event);
}

void TabBar::startDrag(int tabIndex)
{
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/x-npp-tab", QByteArray::number(tabIndex));
    // Drag would be initiated here
}

void TabBar::handleDrag(int, const QPoint&)
{
    // Handle tab reordering during drag
}

void TabBar::endDrag()
{
    // Finalize drag operation
}


// ─── TabButtonZone ───────────────────────────────────────────────────────

bool TabButtonZone::isHit(int x, int y, const QRect& tabRect, bool isVertical) const
{
    QRect btnRect = getButtonRectFrom(tabRect, isVertical);
    return btnRect.contains(x, y);
}

QRect TabButtonZone::getButtonRectFrom(const QRect& tabRect, bool isVertical) const
{
    Q_UNUSED(isVertical);
    // Return close button rect on the right side
    int btnSize = qMin(_width, _height);
    if (btnSize <= 0)
        btnSize = 16;

    return QRect(tabRect.right() - btnSize - 2,
                tabRect.top() + (tabRect.height() - btnSize) / 2,
                btnSize, btnSize);
}


// ─── ControlsTab ──────────────────────────────────────────────────────────

ControlsTab::ControlsTab(QWidget* parent)
    : QTabWidget(parent)
{
    connect(this, &QTabWidget::currentChanged, this, &ControlsTab::tabChanged);
    setMovable(true);
}

ControlsTab::~ControlsTab() = default;

void ControlsTab::createTabs(const QVector<QPair<QString, QWidget*>>& tabs)
{
    _tabs = tabs;
    for (const auto& tab : tabs)
    {
        addTab(tab.second, tab.first);
    }
    activateWindowAt(0);
}

void ControlsTab::activateWindowAt(int index)
{
    if (index == _current)
        return;

    // Hide current, show new
    if (_current >= 0 && _current < _tabs.size())
        _tabs[_current].second->hide();

    _current = index;

    if (_current >= 0 && _current < _tabs.size())
        _tabs[_current].second->show();
}

void ControlsTab::resizeEvent(QResizeEvent* event)
{
    QTabWidget::resizeEvent(event);
    if (_current >= 0 && _current < _tabs.size())
    {
        QWidget* child = _tabs[_current].second;
        QRect r = contentsRect();
        r.adjust(8, 8, -20, -55);
        child->setGeometry(r);
    }
}

void ControlsTab::renameTab(size_t index, const QString& newName)
{
    if (index < static_cast<size_t>(count()))
        setTabText(static_cast<int>(index), newName);
}

bool ControlsTab::renameTab(const QString& internalName, const QString& newName)
{
    for (int i = 0; i < _tabs.size(); ++i)
    {
        if (_tabs[i].first == internalName)
        {
            renameTab(i, newName);
            return true;
        }
    }
    return false;
}


// ─── TabBarPlus ─────────────────────────────────────────────────────────

TabBarPlus::TabBarPlus(QWidget* parent)
    : TabBar(parent)
{
    _colors.resize(5);
    _colors[static_cast<int>(ColorIndex::ActiveText)] = qRgb(0, 0, 0);
    _colors[static_cast<int>(ColorIndex::ActiveTopFocused)] = qRgb(250, 170, 60);
    _colors[static_cast<int>(ColorIndex::ActiveTopUnfocused)] = qRgb(250, 210, 150);
    _colors[static_cast<int>(ColorIndex::InactiveText)] = qRgb(128, 128, 128);
    _colors[static_cast<int>(ColorIndex::InactiveBg)] = qRgb(192, 192, 192);

    // Install hover tracking
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
}

TabBarPlus::~TabBarPlus() = default;

void TabBarPlus::init(bool isVertical, bool isMultiLine, unsigned char buttonsStatus)
{
    setShape(isVertical ? QTabBar::RoundedWest : QTabBar::RoundedNorth);

    if (isMultiLine)
        setElideMode(Qt::ElideNone);

    // Parse button status
    int closeOrder = -1;
    int pinOrder = -1;

    if (buttonsStatus == 0) { closeOrder = -1; pinOrder = -1; }
    else if (buttonsStatus == 1) { closeOrder = 0; pinOrder = -1; }
    else if (buttonsStatus == 2) { closeOrder = -1; pinOrder = 0; }
    else if (buttonsStatus == 3) { closeOrder = 0; pinOrder = 1; }

    _closeButtonZone.init(this, closeOrder);
    _pinButtonZone.init(this, pinOrder);
}

void TabBarPlus::setColor(QRgb color, ColorIndex index)
{
    _colors[static_cast<int>(index)] = color;
}

QRgb TabBarPlus::activeTextColor() { return _colors.value(static_cast<int>(ColorIndex::ActiveText)); }
QRgb TabBarPlus::activeTopBarFocusedColor() { return _colors.value(static_cast<int>(ColorIndex::ActiveTopFocused)); }
QRgb TabBarPlus::activeTopBarUnfocusedColor() { return _colors.value(static_cast<int>(ColorIndex::ActiveTopUnfocused)); }
QRgb TabBarPlus::inactiveTextColor() { return _colors.value(static_cast<int>(ColorIndex::InactiveText)); }
QRgb TabBarPlus::inactiveBgColor() { return _colors.value(static_cast<int>(ColorIndex::InactiveBg)); }

void TabBarPlus::tabToStart(int index)
{
    if (index < 0)
        index = currentIndex();
    if (index <= 0)
        return;

    // Move tab to position 0
    QString text = tabText(index);
    QIcon icon = tabIcon(index);
    QVariant data = tabData(index);
    removeTab(index);
    insertTab(0, icon, text);
    setTabData(0, data);
    setCurrentIndex(0);
}

void TabBarPlus::tabToEnd(int index)
{
    if (index < 0)
        index = currentIndex();
    if (index >= count())
        return;

    QString text = tabText(index);
    QIcon icon = tabIcon(index);
    QVariant data = tabData(index);
    removeTab(index);
    insertTab(count(), icon, text);
    setTabData(count() - 1, data);
    setCurrentIndex(count() - 1);
}

void TabBarPlus::refresh()
{
    // Force tab redraw
    update();
}

void TabBarPlus::triggerOwnerDraw()
{
    update();
}

void TabBarPlus::doVertical(bool enabled)
{
    setShape(enabled ? QTabBar::RoundedWest : QTabBar::RoundedNorth);
}

void TabBarPlus::doMultiLine(bool enabled)
{
    // Qt doesn't support multi-line tabs natively
    // Would need custom implementation
    Q_UNUSED(enabled);
}

void TabBarPlus::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    // Draw each tab
    for (int i = 0; i < count(); ++i)
    {
        QStyleOptionTab opt;
        initStyleOption(&opt, i);

        opt.text = tabText(i);

        // Custom colors
        if (i == currentIndex())
        {
            opt.palette.setColor(QPalette::WindowText,
                                 QColor::fromRgb(activeTextColor()));
        }
        else
        {
            opt.palette.setColor(QPalette::WindowText,
                                 QColor::fromRgb(inactiveTextColor()));
        }

        QRect tabRect = style()->subElementRect(QStyle::SE_TabBarTabContents, &opt, this);
        style()->drawControl(QStyle::CE_TabBarTabLabel, &opt, &p, this);
    }
}

OwnerDrawTabBar::OwnerDrawTabBar(QWidget* parent)
    : TabBarPlus(parent)
{}

// ─── ControlsTab ──────────────────────────────────────────────────────────
