// DockingCont.h — Qt6 translation of DockingCont
#pragma once

#include <QDockWidget>
#include <QStackedWidget>
#include <QTabBar>
#include <QMenu>
#include <QVector>

class DockedWidgetData;

class DockingCont : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockingCont(QWidget* parent = nullptr);
    ~DockingCont() override;

    // Panel management
    void addPanel(QDockWidget* panel);
    void removePanel(QDockWidget* panel);
    void showPanel(QDockWidget* panel, bool show);

    // Active panel
    void setActivePanel(QDockWidget* panel);
    QDockWidget* getActivePanel() const { return _activePanel; }

    // Tab bar access
    QTabBar* getTabBar() const { return _tabBar; }

    // Floating state
    bool isFloating() const { return _isFloating; }
    void setFloating(bool floating);

    // Caption position
    void setCaptionTop(bool onTop) { _isTopCaption = onTop; update(); }
    bool isCaptionTop() const { return _isTopCaption; }

    // Active state
    void setActive(bool active);
    bool isActive() const { return _isActive; }

signals:
    void panelCloseRequested(QDockWidget* panel);
    void panelFloatRequested(QDockWidget* panel);
    void panelDockRequested(QDockWidget* panel);
    void panelMoved(const QPoint& globalPos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void createTabBar();
    void updateTabBar();
    QRect getCaptionRect() const;

    QStackedWidget* _contentStack = nullptr;
    QTabBar* _tabBar = nullptr;
    QDockWidget* _activePanel = nullptr;
    QVector<QDockWidget*> _panels;
    bool _isFloating = false;
    bool _isTopCaption = true;
    bool _isActive = false;

    // Close button hover state
    bool _isCloseHover = false;
    int _closeButtonArea = 12;
    int _captionHeight = 24;
};
