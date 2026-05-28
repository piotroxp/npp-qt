// ControlsTab.cpp — Qt6 translation of ControlsTab
#include "ControlsTab.h"
#include <QResizeEvent>

ControlsTab::ControlsTab(QWidget* parent)
    : QTabWidget(parent)
{
    setMovable(true);
    setTabsClosable(false);
    connect(this, &QTabWidget::currentChanged, this, &ControlsTab::clickedUpdate);
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

    if (_current >= 0 && _current < _tabs.size())
        _tabs[_current].second->hide();

    _current = index;

    if (_current >= 0 && _current < _tabs.size())
    {
        _tabs[_current].second->show();
        // Re-size child widget
        QRect r = contentsRect();
        r.adjust(8, 8, -20, -55);
        _tabs[_current].second->setGeometry(r);
    }
}

bool ControlsTab::renameTab(const QString& internalName, const QString& newName)
{
    Q_UNUSED(internalName);
    Q_UNUSED(newName);
    // Tab name lookup by internal name not yet implemented in this port
    return false;
}

void ControlsTab::renameTab(size_t index, const QString& newName)
{
    if (index < static_cast<size_t>(count()))
        setTabText(static_cast<int>(index), newName);
}

void ControlsTab::resizeEvent(QResizeEvent* event)
{
    QTabWidget::resizeEvent(event);

    QRect childRect = contentsRect();
    childRect.adjust(8, 8, -20, -55);

    if (_current >= 0 && _current < _tabs.size())
        _tabs[_current].second->setGeometry(childRect);
}