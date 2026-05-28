#include "DockingWnd/Docking.h"
#include <QResizeEvent>

DockingPanel::DockingPanel(const QString& title, QWidget* container, QMainWindow* parent)
    : QDockWidget(title, parent), _client(container)
{
    if (_client) setWidget(_client);
}

void DockingPanel::setCaptionTop(bool onTop) { _captionSide = onTop ? CaptionSide::Top : CaptionSide::Bottom; }
void DockingPanel::setActive(bool active) { _isActive = active; }
void DockingPanel::mousePressEvent(QMouseEvent* event) { QDockWidget::mousePressEvent(event); }
void DockingPanel::mouseDoubleClickEvent(QMouseEvent* event) { QDockWidget::mouseDoubleClickEvent(event); emit floatRequested(); }

DockingContainer::DockingContainer(QMainWindow* mainWindow, CaptionSide side, QWidget* parent)
    : QWidget(parent), _mainWindow(mainWindow), _captionSide(side) {}
DockingContainer::~DockingContainer() = default;
void DockingContainer::addPanel(QDockWidget* panel) { _activePanel = panel; }
void DockingContainer::removePanel(QDockWidget* panel) { if (_activePanel == panel) _activePanel = nullptr; }
void DockingContainer::showPanel(QDockWidget* panel, bool show) { if (panel) panel->setVisible(show); }
void DockingContainer::setFloating(bool floating) { _isFloating = floating; }
void DockingContainer::setCaptionTop(bool onTop) { _captionSide = onTop ? CaptionSide::Top : CaptionSide::Bottom; }

DockingManager::DockingManager(QMainWindow* mainWindow, QWidget* parent)
    : QWidget(parent), _mainWindow(mainWindow) {}
DockingManager::~DockingManager() = default;
void DockingManager::init() {}
void DockingManager::createDockablePanel(DockedWidgetData& data, int container) { (void)data; (void)container; }
void DockingManager::showDockablePanel(QWidget* client, bool show) { if (client) client->setVisible(show); }
void DockingManager::setActiveTab(int container, int tabIndex) { (void)container; (void)tabIndex; }
void DockingManager::resizeEvent(QResizeEvent* event) { QWidget::resizeEvent(event); emit layoutChanged(); }
void DockingManager::saveLayout(QByteArray& state) const { state.clear(); }
bool DockingManager::restoreLayout(const QByteArray& state) { (void)state; return true; }
Qt::DockWidgetArea DockingManager::getDockArea(int container) const
{
    switch (container) {
    case CONT_LEFT: return Qt::LeftDockWidgetArea;
    case CONT_RIGHT: return Qt::RightDockWidgetArea;
    case CONT_TOP: return Qt::TopDockWidgetArea;
    case CONT_BOTTOM: return Qt::BottomDockWidgetArea;
    default: return Qt::NoDockWidgetArea;
    }
}
