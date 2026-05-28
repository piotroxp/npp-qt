// TabBar.h — Qt6 translation of CTabCtrl → QTabWidget / custom QTabBar
#pragma once

#include <QTabWidget>
#include <QTabBar>
#include <QMouseEvent>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QIcon>
#include <QPoint>
#include <QAction>

// Tab bar notification messages
#define TCN_TABDROPPED (QEvent::Type(QEvent::User + 100))
#define TCN_TABDROPPEDOUTSIDE (QEvent::Type(QEvent::User + 101))
#define TCN_TABDELETE         (QEvent::Type(QEvent::User + 102))
#define TCN_MOUSEHOVERING     (QEvent::Type(QEvent::User + 103))
#define TCN_MOUSELEAVING      (QEvent::Type(QEvent::User + 104))
#define TCN_TABPINNED         (QEvent::Type(QEvent::User + 105))

struct TabBarNotification
{
    int tabIndex = -1;
    int tabOrigin = 0;
    QPoint globalPos;
};

// Tab button zone info
struct TabButtonZone
{
    void init(QWidget* parent, int order)
    {
        _parent = parent;
        _order = order;
    }

    bool isHit(int x, int y, const QRect& tabRect, bool isVertical) const;
    QRect getButtonRectFrom(const QRect& tabRect, bool isVertical) const;
    void setOrder(int newOrder) { _order = newOrder; }

    QWidget* _parent = nullptr;
    int _width = 0;
    int _height = 0;
    int _order = -1; // from right to left: 0, 1
};

// Base tab bar
class TabBar : public QTabBar
{
    Q_OBJECT

public:
    explicit TabBar(QWidget* parent = nullptr);
    ~TabBar() override;

    // Insert tab at end
    int insertAtEnd(const QString& tabName);

    // Activate/select tab
    void activateAt(int index);
    int getCurrentTabIndex() const { return currentIndex(); }

    // Get/set tab count
    int getItemCount() const { return count(); }
    size_t nbItem() const { return static_cast<size_t>(count()); }

    // Delete tab
    void deleteItemAt(size_t index);

    // Delete all tabs
    void deleteAllItems();

    // Set image list (icons per tab)
    void setImageList(const QVector<QIcon>& icons);

    // Set custom close button icons
    void setCloseBtnImageList(const QVector<QIcon>& normalIcons);
    void setPinBtnImageList(const QVector<QIcon>& pinIcons);

    // Tab navigation
    int getNextOrPrevTabIdx(bool isNext) const;

    // DPI
    void resizeIconsDpi(int dpi);
    int getCurrentIconSize() const { return _iconSize; }

signals:
    void tabDropped(int tabIndex);
    void tabDroppedOutside(int tabIndex);
    void tabDelete(int tabIndex);
    void tabPinned(int tabIndex);
    void tabMouseHovering(int tabIndex);
    void tabMouseLeaving();
    void tabReordered(int from, int to);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    void startDrag(int tabIndex);
    void handleDrag(int tabIndex, const QPoint& globalPos);
    void endDrag();

    int _nbItem = 0;
    int _iconSize = 16;
    bool _hasImgLst = false;

    // Drag state
    bool _mightBeDragging = false;
    int _dragCount = 0;
    bool _isDragging = false;
    int _srcTab = -1;

    QVector<QIcon> _closeBtnIcons;
    QVector<QIcon> _pinBtnIcons;

    TabButtonZone _closeButtonZone;
    TabButtonZone _pinButtonZone;

    int _currentHoverTab = -1;
    QRect _currentHoverTabRect;
    bool _isCloseHover = false;
    bool _isPinHover = false;
    int _whichCloseClickDown = -1;
    int _whichPinClickDown = -1;
};


// ControlsTab is defined in ControlsTab.h
#include "ControlsTab.h"

// Tab bar for document windows with close buttons, pin buttons, drag-and-drop
class TabBarPlus : public TabBar
{
    Q_OBJECT

public:
    TabBarPlus(QWidget* parent = nullptr);
    ~TabBarPlus() override;

    // Colors
    enum class ColorIndex { ActiveText, ActiveTopFocused, ActiveTopUnfocused,
                            InactiveText, InactiveBg };

    static void setColor(QRgb color, ColorIndex index);

    // Init with close/pin button options
    void init(bool isVertical, bool isMultiLine, unsigned char buttonsStatus = 0);

    // Tab drag-to-reorder
    void tabToStart(int index = -1);
    void tabToEnd(int index = -1);

    // Refresh tabs (after width change)
    void refresh();

    // Set close/pin button order
    void setTabCloseButtonOrder(int order) { _closeButtonZone.setOrder(order); }
    void setTabPinButtonOrder(int order) { _pinButtonZone.setOrder(order); }

    // Theme changes
    static void triggerOwnerDraw();
    static void doVertical(bool enabled);
    static void doMultiLine(bool enabled);

    // Colors
    static QRgb activeTextColor();
    static QRgb activeTopBarFocusedColor();
    static QRgb activeTopBarUnfocusedColor();
    static QRgb inactiveTextColor();
    static QRgb inactiveBgColor();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool _isCloseHover = false;
    bool _isPinHover = false;

    static QVector<QRgb> _colors;
};

// Custom tab bar subclass for owner-drawn tabs
class OwnerDrawTabBar : public TabBarPlus
{
    Q_OBJECT

public:
    explicit OwnerDrawTabBar(QWidget* parent = nullptr);

    // Override to provide per-tab color
    virtual int getIndividualTabColorId(int /*tabIndex*/) { return -1; }
};
