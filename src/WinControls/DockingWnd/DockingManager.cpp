// DockingManager.cpp — Qt6 translation
#include "DockingManager.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QStyle>
#include <QMouseEvent>

DockingManager::DockingManager(QMainWindow* mainWindow, QObject* parent)
    : QObject(parent), _mainWindow(mainWindow)
{
}

DockingManager::~DockingManager() = default;

void DockingManager::init()
{
    // Qt's QMainWindow already has built-in docking
    // Just verify the containers exist
}

void DockingManager::createDockableDlg(const DockedWidgetData& data, int container, bool isVisible)
{
    if (!data.client)
        return;

    QDockWidget* dock = new QDockWidget(data.name, _mainWindow);

    // Configure based on data.mask
    dock->setWidget(data.client);

    if (data.mask & DWS_ICONTAB)
        dock->setWindowIcon(data.iconTab);

    dock->setVisible(isVisible);

    // Dock position
    Qt::DockWidgetArea area;
    switch (container)
    {
        case CONT_LEFT: area = Qt::LeftDockWidgetArea; break;
        case CONT_RIGHT: area = Qt::RightDockWidgetArea; break;
        case CONT_TOP: area = Qt::TopDockWidgetArea; break;
        case CONT_BOTTOM: area = Qt::BottomDockWidgetArea; break;
        default: area = Qt::RightDockWidgetArea; break;
    }

    _mainWindow->addDockWidget(area, dock);
    _widgetData[data.client] = data;
}

void DockingManager::showDockableDlg(QWidget* client, bool show)
{
    if (!client)
        return;

    QDockWidget* dock = qobject_cast<QDockWidget*>(client->parent());
    if (dock)
        dock->setVisible(show);
}

void DockingManager::showDockableDlg(const QString& name, bool show)
{
    for (auto it = _widgetData.begin(); it != _widgetData.end(); ++it)
    {
        if (it.value().name == name)
        {
            showDockableDlg(it.key(), show);
            return;
        }
    }
}

void DockingManager::setActiveTab(int container, int tabIndex)
{
    Q_UNUSED(container);
    Q_UNUSED(tabIndex);
    // Tab handling done by QTabBar in containers
}

void DockingManager::showFloatingContainers(bool show)
{
    for (QDockWidget* dock : _floatingContainers)
    {
        dock->setVisible(show);
    }
}

void DockingManager::setStyleCaption(bool captionOnTop)
{
    Q_UNUSED(captionOnTop);
    // Qt handles caption automatically
}

int DockingManager::getDockedContSize(int container) const
{
    Qt::DockWidgetArea area;
    switch (container)
    {
        case CONT_LEFT: area = Qt::LeftDockWidgetArea; break;
        case CONT_RIGHT: area = Qt::RightDockWidgetArea; break;
        case CONT_TOP: area = Qt::TopDockWidgetArea; break;
        case CONT_BOTTOM: area = Qt::BottomDockWidgetArea; break;
        default: return -1;
    }

    QList<QDockWidget*> docks = _mainWindow->findChildren<QDockWidget*>();
    for (QDockWidget* dock : docks)
    {
        if (_mainWindow->dockWidgetArea(dock) == area)
            return dock->width();
    }
    return -1;
}

void DockingManager::setDockedContSize(int container, int size)
{
    Q_UNUSED(container);
    Q_UNUSED(size);
}

QByteArray DockingManager::saveLayout() const
{
    return _mainWindow->saveState();
}

bool DockingManager::restoreLayout(const QByteArray& state)
{
    return _mainWindow->restoreState(state);
}

// ─── DockingSplitter ───────────────────────────────────────────

DockingSplitter::DockingSplitter(QWidget* parent)
    : QWidget(parent)
{
    setCursor(Qt::SizeHorCursor);
}

void DockingSplitter::init(Direction dir)
{
    _direction = dir;
    setCursor(dir == Horizontal ? Qt::SizeHorCursor : Qt::SizeVerCursor);
    setMinimumSize(4, 4);
    setMaximumSize(4, 4);
}

void DockingSplitter::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _isDragging = true;
        _dragStart = event->globalPos();
        _startOffset = 0;
        event->accept();
    }
}

void DockingSplitter::mouseMoveEvent(QMouseEvent* event)
{
    if (_isDragging)
    {
        QPoint delta = event->globalPos() - _dragStart;
        int offset = (_direction == Horizontal) ? delta.x() : delta.y();
        emit splitterMoved(offset);
    }
}

void DockingSplitter::mouseReleaseEvent(QMouseEvent* event)
{
    _isDragging = false;
    event->accept();
}
