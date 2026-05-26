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

void ControlsTab::createTabs(const WindowVector& winVector)
{
    _winVector = winVector;

    for (const auto& info : winVector)
    {
        addTab(info._widget, info._name);
    }

    activateWindowAt(0);
}

void ControlsTab::activateWindowAt(int index)
{
    if (index == _current)
        return;

    if (_current >= 0 && _current < _winVector.size())
        _winVector[_current]._widget->hide();

    _current = index;

    if (_current >= 0 && _current < _winVector.size())
    {
        _winVector[_current]._widget->show();
        // Re-size child widget
        QRect r = contentsRect();
        r.adjust(8, 8, -20, -55);
        _winVector[_current]._widget->setGeometry(r);
    }
}

void ControlsTab::resizeTo(const QRect& rc)
{
    setGeometry(rc);

    QRect childRect = rc;
    childRect.adjust(8, 8, -20, -55);

    if (_current >= 0 && _current < _winVector.size())
        _winVector[_current]._widget->setGeometry(childRect);
}

bool ControlsTab::renameTab(const QString& internalName, const QString& newName)
{
    for (int i = 0; i < _winVector.size(); ++i)
    {
        if (_winVector[i]._internalName == internalName)
        {
            renameTab(i, newName);
            return true;
        }
    }
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

    if (_current >= 0 && _current < _winVector.size())
        _winVector[_current]._widget->setGeometry(childRect);
}
