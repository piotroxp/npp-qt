// Semantic Lift: Win32 TabBar → Qt6 QTabWidget Implementation
// Original: PowerEditor/src/WinControls/TabBar/TabBar.cpp
// Target: npp-qt/src/WinControls/TabBar.cpp

#include "TabBar.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>

QColor TabBarPlus::_activeTextColour = QColor(0, 0, 0);
QColor TabBarPlus::_activeTopBarFocusedColour = QColor(250, 170, 60);
QColor TabBarPlus::_activeTopBarUnfocusedColour = QColor(250, 210, 150);
QColor TabBarPlus::_inactiveTextColour = QColor(128, 128, 128);
QColor TabBarPlus::_inactiveBgColour = QColor(192, 192, 192);

int TabBarPlus::_nbCtrl = 0;
QWidget* TabBarPlus::_tabbrPlusInstanceHwndArray[10] = {nullptr};

TabBar::TabBar(QWidget* parent) : QTabBar(parent)
{
    connect(this, &QTabBar::currentChanged, this, [this](int index) {
        if (index >= 0) emit tabDelete(index);
    });
}

TabBar::~TabBar() = default;

void TabBar::destroy() { destroyFonts(); }

void TabBar::init(QApplication* app, QWidget* parent, bool isVertical, bool isMultiLine)
{
    setParent(parent);
    if (isVertical) {
        this->setShape(isMultiLine ? QTabBar::RoundedWest : QTabBar::TriangularWest);
        this->setUsesScrollButtons(true);
    } else {
        this->setShape(isMultiLine ? QTabBar::RoundedSouth : QTabBar::TriangularSouth);
    }
    if (_font.pointSize() < 0) {
        QFont defaultFont = QApplication::font();
        defaultFont.setPointSize(8);
        _font = defaultFont;
        _largeFont = defaultFont;
        _largeFont.setPointSize(10);
        _largeFont.setWeight(QFont::Bold);
        setFont(_font);
    }
}

void TabBar::destroyFonts()
{
    _font = QFont();
    _largeFont = QFont();
    _verticalFont = QFont();
    _verticalLargeFont = QFont();
}

void TabBar::setFont(const QFont& font)
{
    _font = font;
    QTabBar::setFont(font);
}

int TabBar::insertAtEnd(const QString& tabName)
{
    return addTab(tabName);
}

void TabBar::activateAt(int index)
{
    if (index >= 0 && index < count()) setCurrentIndex(index);
}

QString TabBar::getCurrentTitle() const
{
    int idx = currentIndex();
    return idx >= 0 ? tabText(idx) : QString();
}

void TabBar::deletItemAt(size_t index)
{
    if (index < static_cast<size_t>(count())) removeTab(static_cast<int>(index));
}

void TabBar::deletAllItem() { while (count() > 0) removeTab(0); }

void TabBar::setImageList(const QVector<QIcon>& icons)
{
    _hasImgLst = true;
    _imageList = icons;
}

void TabBar::reSizeTo(QRect& rc2Adjust)
{
    int tabBarHeight = this->sizeHint().height();
    if (this->shape() == QTabBar::RoundedWest || this->shape() == QTabBar::TriangularWest) {
        rc2Adjust.adjust(tabBarHeight, 0, -tabBarHeight, 0);
    } else {
        rc2Adjust.adjust(0, tabBarHeight, 0, -tabBarHeight);
    }
    resize(rc2Adjust.size());
    move(rc2Adjust.topLeft());
}

int TabBar::getNextOrPrevTabIdx(bool isNext) const
{
    int current = currentIndex();
    int last = count() - 1;
    return isNext ? (current >= last ? 0 : current + 1) : (current <= 0 ? last : current - 1);
}

TabBarPlus::TabBarPlus(QWidget* parent) : TabBar(parent)
{
    for (int i = 0; i < 10; ++i) {
        if (!_tabbrPlusInstanceHwndArray[i]) {
            _tabbrPlusInstanceHwndArray[i] = this;
            ++_nbCtrl;
            break;
        }
    }
    setMouseTracking(true);
    installEventFilter(this);
}

void TabBarPlus::init(QApplication* app, QWidget* parent, bool isVertical, bool isMultiLine, unsigned char buttonsStatus)
{
    TabBar::init(app, parent, isVertical, isMultiLine);

    int closeOrder = -1, pinOrder = -1;
    if (buttonsStatus == 1) closeOrder = 0;
    else if (buttonsStatus == 2) pinOrder = 0;
    else if (buttonsStatus == 3) { closeOrder = 0; pinOrder = 1; }

    _closeButtonZone.init(parent, closeOrder);
    _pinButtonZone.init(parent, pinOrder);
    setCloseBtnImageList();
    setPinBtnImageList();
}

void TabBarPlus::destroy()
{
    TabBar::destroy();
    for (int i = 0; i < 10; ++i) {
        if (_tabbrPlusInstanceHwndArray[i] == this) {
            _tabbrPlusInstanceHwndArray[i] = nullptr;
            --_nbCtrl;
            break;
        }
    }
}

void TabBarPlus::refresh()
{
    int index = addTab("");
    removeTab(index);
}

void TabBarPlus::triggerOwnerDrawTabbar()
{
    for (int i = 0; i < 10; ++i) {
        if (_tabbrPlusInstanceHwndArray[i]) _tabbrPlusInstanceHwndArray[i]->update();
    }
}

void TabBarPlus::doVertical()
{
    for (int i = 0; i < 10; ++i) {
        if (_tabbrPlusInstanceHwndArray[i]) {
            if (auto* tb = qobject_cast<TabBarPlus*>(_tabbrPlusInstanceHwndArray[i]))
                tb->setShape(QTabBar::RoundedWest);
        }
    }
}

void TabBarPlus::doMultiLine()
{
    for (int i = 0; i < 10; ++i) {
        if (_tabbrPlusInstanceHwndArray[i]) {
            if (auto* tb = qobject_cast<TabBarPlus*>(_tabbrPlusInstanceHwndArray[i]))
                tb->setShape(QTabBar::RoundedSouth);
        }
    }
}

void TabBarPlus::setColour(const QColor& colour2Set, TabColourIndex i)
{
    switch (i) {
        case TabColourIndex::activeText: _activeTextColour = colour2Set; break;
        case TabColourIndex::activeFocusedTop: _activeTopBarFocusedColour = colour2Set; break;
        case TabColourIndex::activeUnfocusedTop: _activeTopBarUnfocusedColour = colour2Set; break;
        case TabColourIndex::inactiveText: _inactiveTextColour = colour2Set; break;
        case TabColourIndex::inactiveBg: _inactiveBgColour = colour2Set; break;
    }
    triggerOwnerDrawTabbar();
}

void TabBarPlus::tabToStart(int index)
{
    if (index < 0) index = currentIndex();
    if (index <= 0) return;
    QString text = tabText(index);
    QIcon icon = tabIcon(index);
    removeTab(index);
    insertTab(0, icon, text);
    setCurrentIndex(0);
}

void TabBarPlus::tabToEnd(int index)
{
    if (index < 0) index = currentIndex();
    if (index >= count() - 1) return;
    QString text = tabText(index);
    QIcon icon = tabIcon(index);
    removeTab(index);
    addTab(icon, text);
    setCurrentIndex(count() - 1);
}

void TabBarPlus::setCloseBtnImageList() { _closeButtonIcons.clear(); }
void TabBarPlus::setPinBtnImageList() { _pinButtonIcons.clear(); }

void TabBarPlus::setActiveTab(int tabIndex)
{
    if (tabIndex >= 0 && tabIndex < count()) setCurrentIndex(tabIndex);
}

bool TabBarPlus::exchangeTabItemData(int oldTab, int newTab, bool setToActive)
{
    if (oldTab < 0 || oldTab >= count() || newTab < 0 || newTab >= count()) return false;
    QString oldText = tabText(oldTab);
    QIcon oldIcon = tabIcon(oldTab);
    QString newText = tabText(newTab);
    QIcon newIcon = tabIcon(newTab);
    removeTab(oldTab);
    insertTab(oldTab, newIcon, newText);
    removeTab(newTab);
    insertTab(newTab, oldIcon, oldText);
    if (setToActive) setCurrentIndex(newTab);
    return true;
}

void TabBarPlus::exchangeItemData(const QPoint& screenPoint)
{
    int targetTab = getTabIndexAt(screenPoint);
    if (targetTab >= 0 && targetTab != _nTabDragged) {
        exchangeTabItemData(_nTabDragged, targetTab);
        _nTabDragged = targetTab;
    }
}

int TabBarPlus::getTabIndexAt(const QPoint& p) const
{
    QPoint local = this->mapFromGlobal(p);
    for (int i = 0; i < count(); ++i) {
        if (this->tabRect(i).contains(local)) return i;
    }
    return -1;
}

int TabBarPlus::getTabIndexAt(int x, int y) const
{
    return getTabIndexAt(QPoint(x, y));
}

void TabBarPlus::notify(int notifyCode, int tabIndex)
{
    Q_UNUSED(notifyCode);
    Q_UNUSED(tabIndex);
}

void TabBarPlus::mousePressEvent(QMouseEvent* event)
{
    int nTab = getTabIndexAt(this->mapFromParent(event->pos()));
    if (nTab >= 0) {
        _nSrcTab = nTab;
        _mightBeDragging = true;
    }
    QTabBar::mousePressEvent(event);
}

void TabBarPlus::mouseReleaseEvent(QMouseEvent* event)
{
    if (_isDragging) {
        _isDragging = false;
        _mightBeDragging = false;
    }
    QTabBar::mouseReleaseEvent(event);
}

void TabBarPlus::mouseMoveEvent(QMouseEvent* event)
{
    if (_mightBeDragging && !_isDragging) {
        if (++_dragCount > 2) {
            _isDragging = true;
            _nTabDragged = _nSrcTab;
        }
    }
    if (_isDragging) exchangeItemData(event->globalPos());
    QTabBar::mouseMoveEvent(event);
}

void TabBarPlus::wheelEvent(QWheelEvent* event)
{
    int delta = event->angleDelta().y();
    if (delta > 0) setCurrentIndex((currentIndex() - 1 + count()) % count());
    else if (delta < 0) setCurrentIndex((currentIndex() + 1) % count());
    event->accept();
}

QRect TabButtonZone::getButtonRectFrom(const QRect& tabRect, bool isVertical) const
{
    const int buttonSize = 16;
    if (isVertical) {
        int x = tabRect.left() + 2;
        int y = _order >= 0 ? tabRect.top() + 2 + _order * (buttonSize + 2) : tabRect.bottom() - buttonSize - 2;
        return QRect(x, y, buttonSize, buttonSize);
    } else {
        int x = _order >= 0 ? tabRect.right() - buttonSize - 2 - _order * (buttonSize + 2) : tabRect.left() + 2;
        int y = tabRect.top() + 2;
        return QRect(x, y, buttonSize, buttonSize);
    }
}


int TabBarPlus::getIndividualTabColourId(int tabIndex) const
{
    if (tabIndex < 0 || tabIndex >= count()) return -1;
    QColor tabColor = tabData(tabIndex).value<QColor>();
    if (!tabColor.isValid()) return -1;
    return 0;
}
