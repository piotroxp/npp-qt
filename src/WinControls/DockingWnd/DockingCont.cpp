// DockingCont.cpp — Qt6 translation
#include "DockingCont.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabBar>
#include <QStyle>
#include <QStyleOptionDockWidget>
#include <QMouseEvent>
#include <QContextMenuEvent>

DockingCont::DockingCont(QWidget* parent)
    : QDockWidget(parent)
{
    setFeatures(QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetFloatable |
                QDockWidget::DockWidgetClosable);

    _contentStack = new QStackedWidget(this);
    setWidget(_contentStack);

    setTitleBarWidget(nullptr); // Use default

    setStyleSheet(R"(
        QDockWidget {
            titlebar-close-button: true;
        }
        QDockWidget::title {
            background: palette(window);
            padding: 4px;
        }
    )");
}

DockingCont::~DockingCont()
{
    // Clean up
}

void DockingCont::addPanel(QDockWidget* panel)
{
    if (!panel || _panels.contains(panel))
        return;

    _panels.append(panel);
    panel->setParent(_contentStack);
    _contentStack->addWidget(panel);

    updateTabBar();

    if (_panels.size() == 1)
        setActivePanel(panel);
}

void DockingCont::removePanel(QDockWidget* panel)
{
    if (!panel || !_panels.contains(panel))
        return;

    int idx = _panels.indexOf(panel);
    _panels.removeAt(idx);
    _contentStack->removeWidget(panel);

    updateTabBar();

    if (_activePanel == panel)
    {
        if (!_panels.isEmpty())
            setActivePanel(_panels[qMin(idx, _panels.size() - 1)]);
        else
            _activePanel = nullptr;
    }
}

void DockingCont::showPanel(QDockWidget* panel, bool show)
{
    if (!panel || !_panels.contains(panel))
        return;

    panel->setVisible(show);
    if (show)
        setVisible(true);
}

void DockingCont::setActivePanel(QDockWidget* panel)
{
    if (!_panels.contains(panel))
        return;

    _activePanel = panel;
    _contentStack->setCurrentWidget(panel);

    // Update tab selection if using tab bar
    if (_tabBar)
    {
        int idx = _panels.indexOf(panel);
        if (idx >= 0)
            _tabBar->setCurrentIndex(idx);
    }

    setWindowTitle(panel->windowTitle());
}

void DockingCont::createTabBar()
{
    if (_tabBar)
        return;

    _tabBar = new QTabBar(this);
    _tabBar->setShape(QTabBar::RoundedSouth);
    _tabBar->setDocumentMode(true);
    _tabBar->setMovable(true);
    _tabBar->setTabsClosable(true);

    connect(_tabBar, &QTabBar::currentChanged, this, [this](int index) {
        if (index >= 0 && index < _panels.size())
            setActivePanel(_panels[index]);
    });

    connect(_tabBar, &QTabBar::tabCloseRequested, this, [this](int index) {
        if (index >= 0 && index < _panels.size())
            emit panelCloseRequested(_panels[index]);
    });
}

void DockingCont::updateTabBar()
{
    if (_panels.size() <= 1)
    {
        // Hide tab bar for single panel
        if (_tabBar)
            _tabBar->hide();
        return;
    }

    if (!_tabBar)
        createTabBar();

    _tabBar->show();
    _tabBar->clear();

    for (QDockWidget* panel : _panels)
    {
        _tabBar->addTab(panel->windowIcon(), panel->windowTitle());
    }

    if (_activePanel)
    {
        int idx = _panels.indexOf(_activePanel);
        if (idx >= 0)
            _tabBar->setCurrentIndex(idx);
    }
}

void DockingCont::setFloating(bool floating)
{
    _isFloating = floating;
    setWindowFlags(floating ? Qt::Tool : Qt::Widget);
    show();
}

void DockingCont::setActive(bool active)
{
    _isActive = active;
    update();
}

void DockingCont::mousePressEvent(QMouseEvent* event)
{
    QDockWidget::mousePressEvent(event);
}

void DockingCont::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit panelFloatRequested(_activePanel);
}

void DockingCont::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    menu.addAction("Float", this, [this]() { emit panelFloatRequested(_activePanel); });
    menu.addAction("Dock", this, [this]() { emit panelDockRequested(_activePanel); });
    menu.addAction("Close", this, [this]() { emit panelCloseRequested(_activePanel); });
    menu.exec(event->globalPos());
}

QRect DockingCont::getCaptionRect() const
{
    QStyleOptionDockWidget opt;
    opt.initFrom(this);
    QRect rect = style()->subElementRect(QStyle::SE_DockWidgetTitleBar, &opt, this);
    return rect;
}
