// Semantic Lift: Win32 TabBar → Qt6 QTabWidget
// Original: PowerEditor/src/WinControls/TabBar/TabBar.h
// Target: npp-qt/src/WinControls/TabBar.h

#pragma once

#include "Window.h"
#include <QTabBar>
#include <QTabBar>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QColor>
#include <QFont>
#include <QMap>

class TabBarPlus;

class TabBar : public QTabBar
{
    Q_OBJECT

public:
    TabBar(QWidget* parent = nullptr);
    ~TabBar() override;

    void destroy();

    void init(QApplication* app, QWidget* parent, bool isVertical, bool isMultiLine);

    void reSizeTo(QRect& rc2Adjust);

    int insertAtEnd(const QString& tabName);
    void activateAt(int index);
    QString getCurrentTitle() const;

    int getCurrentTabIndex() const { return currentIndex(); }
    int getItemCount() const { return count(); }

    void deletItemAt(size_t index);
    void deletAllItem();

    void setImageList(const QVector<QIcon>& icons);

    size_t nbItem() const { return count(); }

    void destroyFonts();
    void setFont(const QFont& font);

    QFont& getFont(bool isReduced = true) {
        return isReduced ? _font : _largeFont;
    }

    int getNextOrPrevTabIdx(bool isNext) const;

    class DPIManagerV2 {
    public:
        static int getDpiForWindow(QWidget* w) { return w->logicalDpiY(); }
        static int scale(int val, int dpi) { return val; }
        int getDpi() const { return _dpi; }
        void setDpi(int dpi) { _dpi = dpi; }
        int scale(int val) const { return val; }
    private:
        int _dpi = 96;
    };

    DPIManagerV2& dpiManager() { return _dpiManager; }

signals:
    void tabDropped(int index);
    void tabDelete(int index);
    void customStyleChanged(bool isVertical, bool isMultiLine);

protected:
    int getRowCount() const { return 1; }

    DPIManagerV2 _dpiManager;
    QFont _font;
    QFont _largeFont;
    QFont _verticalFont;
    QFont _verticalLargeFont;
    bool _hasImgLst = false;
    QVector<QIcon> _imageList;

    virtual int getTabIndexAt(const QPoint& p) const = 0;
    virtual int getIndividualTabColourId(int tabIndex) const = 0;

};

class TabButtonZone {
public:
    void init(QWidget* parent, int order) { _parent = parent; _order = order; }

    bool isHit(int x, int y, const QRect& tabRect, bool isVertical) const {
        QRect buttonRect = getButtonRectFrom(tabRect, isVertical);
        return buttonRect.contains(x, y);
    }

    QRect getButtonRectFrom(const QRect& tabRect, bool isVertical) const;
    void setOrder(int newOrder) { _order = newOrder; }

    QWidget* _parent = nullptr;
    int _width = 0;
    int _height = 0;
    int _order = -1;
};

class TabBarPlus : public TabBar
{
    Q_OBJECT

public:
    TabBarPlus(QWidget* parent = nullptr);

    enum class TabColourIndex {
        activeText, activeFocusedTop, activeUnfocusedTop, inactiveText, inactiveBg
    };

    enum class IndividualTabColourId {
        id0, id1, id2, id3, id4, id5, id6, id7, id8, id9
    };

    void init(QApplication* app, QWidget* parent, bool isVertical, bool isMultiLine,
              unsigned char buttonsStatus = 0);

    void destroy();

    QPoint getDraggingPoint() const { return _dragStartPos; }
    void resetDraggingPoint() { _dragStartPos = QPoint(0, 0); }

    static void triggerOwnerDrawTabbar();
    static void doVertical();
    static void doMultiLine();

    static void setColour(const QColor& colour2Set, TabColourIndex i);


    void tabToStart(int index = -1);
    void tabToEnd(int index = -1);

    void setCloseBtnImageList();
    void setPinBtnImageList();

    void setTabPinButtonOrder(int newOrder) { _pinButtonZone.setOrder(newOrder); }
    void setTabCloseButtonOrder(int newOrder) { _closeButtonZone.setOrder(newOrder); }

    void refresh();

protected:
    bool _mightBeDragging = false;
    int _dragCount = 0;
    bool _isDragging = false;
    bool _isDraggingInside = false;
    int _nSrcTab = -1;
    int _nTabDragged = -1;
    int _previousTabSwapped = -1;
    QPoint _dragStartPos;

    QRect _currentHoverTabRect;
    int _currentHoverTabItem = -1;

    TabButtonZone _closeButtonZone;
    TabButtonZone _pinButtonZone;

    QVector<QIcon> _closeButtonIcons;
    static const int _closeTabIdx = 0;
    static const int _closeTabInactIdx = 1;
    static const int _closeTabHoverInIdx = 2;
    static const int _closeTabHoverOnTabIdx = 3;
    static const int _closeTabPushIdx = 4;

    QVector<QIcon> _pinButtonIcons;
    static const int _unpinnedIdx = 0;
    static const int _unpinnedInactIdx = 1;
    static const int _unpinnedHoverInIdx = 2;
    static const int _unpinnedHoverOnTabIdx = 3;
    static const int _pinnedIdx = 4;
    static const int _pinnedHoverIdx = 5;
    static const int _unpinnedEmptyIdx = 6;

    bool _isCloseHover = false;
    bool _isPinHover = false;
    int _whichCloseClickDown = -1;
    int _whichPinClickDown = -1;
    bool _lmbdHit = false;

    static QColor _activeTextColour;
    static QColor _activeTopBarFocusedColour;
    static QColor _activeTopBarUnfocusedColour;
    static QColor _inactiveTextColour;
    static QColor _inactiveBgColour;

    static int _nbCtrl;
    static QWidget* _tabbrPlusInstanceHwndArray[10];

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void setActiveTab(int tabIndex);
    bool exchangeTabItemData(int oldTab, int newTab, bool setToActive = true);
    void exchangeItemData(const QPoint& screenPoint);

    int getTabIndexAt(const QPoint& p) const override;
    int getTabIndexAt(int x, int y) const;
    int getIndividualTabColourId(int tabIndex) const override;

    bool isPointInParentZone(const QPoint& screenPoint) const;

    void notify(int notifyCode, int tabIndex);

    void draggingCursor(const QPoint& screenPoint);

    friend class TabBar;
};
