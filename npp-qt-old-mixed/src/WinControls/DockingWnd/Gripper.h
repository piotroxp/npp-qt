// npp-qt: Gripper — drag handle for docking panels
// Translated from Win32 Gripper.h (Jens Lorenz, Don Ho)
// This class handles drag-and-drop of docking containers.
// Qt6 stub: uses Qt mouse events and signals instead of Win32 WNDPROC.

#pragma once

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QMouseEvent>

// Forward declarations
class DockingCont;
class DockingManager;

// Dot pattern for drawing gripper handle (Win32 GDI dots)
static const unsigned short DotPattern[] = {
    0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
};

// Tab reordering state
struct TabInfo {
    int index = 0;
    QWidget* hTab = nullptr;
    QRect rcItem;
};


class Gripper : public QObject
{
    Q_OBJECT

public:
    Gripper();
    ~Gripper();

    void init(QWidget* parent);
    void startGrip(DockingCont* pCont, DockingManager* pDockMgr);

    // For Win32 compatibility shim — maps parent HWND to Qt parent widget
    QWidget* parentWidget() const { return _pParent; }

    // Trigger a redraw of the gripper handle
    void update();

Q_SIGNALS:
    // Emitted when drag operation completes
    void gripComplete(DockingCont* cont, bool cancelled);
    // Emitted when tab reordering happens
    void tabReordered(int fromIndex, int toIndex);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void createGripperWindow();
    void destroyGripperWindow();
    void onMouseMove(QMouseEvent* event);
    void onMouseRelease(QMouseEvent* event);
    void drawRectangle(const QPoint& pt);
    void clearRectangle();
    void doTabReordering(const QPoint& pt);
    DockingCont* containerHitTest(const QPoint& pt) const;
    void initTabInformation();

    // Screen-space rect helpers
    QRect rectToScreen(const QRect& rc) const;
    QRect rectToClient(const QRect& rc) const;
    void shrinkRect(QRect* rc);
    void calcGripperRect(QRect* rc, const QRect& rcCorr, const QPoint& pt);

private:
    // Parent widget
    QWidget* _pParent = nullptr;

    // Container and manager references
    DockingCont* _pCont = nullptr;
    DockingManager* _pDockMgr = nullptr;

    // Mouse drag offset
    QPoint _ptOffset{};
    QPoint _ptOld{};
    bool _bPtOldValid = false;

    // Last drawn rectangle (for erase)
    QRect _rcPrev{};

    // Tab reordering state
    QWidget* _hTab = nullptr;
    QWidget* _hTabSource = nullptr;
    bool _startMovingFromTab = false;
    int _iItem = 0;
    QRect _rcItem{};
    TabInfo _tcItem{};

    // Gripper handle widget (small overlay)
    QWidget* _pGripperWidget = nullptr;

    // RTL layout direction
    bool _isRTL = false;

    // Whether the Win32 window class was registered
    static bool _isRegistered;
};
