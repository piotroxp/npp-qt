// ToolBar.cpp — Qt6 translation of CToolBar → QToolBar
#include "ToolBar.h"
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QMainWindow>

ToolBar::ToolBar(QWidget* parent)
    : QToolBar(parent)
{
    setMovable(false);
    setFloatable(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QToolBar::customContextMenuRequested,
            this, &ToolBar::onCustomContextMenuRequested);

    // Flat style + tooltips
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setStyleSheet(R"(
        QToolBar {
            background: transparent;
            border: none;
            spacing: 2px;
        }
        QToolButton {
            border: none;
            background: transparent;
        }
        QToolButton:hover {
            background: rgba(128,128,128,40);
            border-radius: 2px;
        }
        QToolButton:pressed {
            background: rgba(128,128,128,60);
        }
    )");
}

ToolBar::~ToolBar() = default;

void ToolBar::init(const QVector<ToolBarButtonUnit>& buttonArray)
{
    _buttonArray = buttonArray;
    rebuildActions(buttonArray);
}

void ToolBar::rebuildActions(const QVector<ToolBarButtonUnit>& buttonArray)
{
    clear();
    _actionMap.clear();

    for (const auto& unit : buttonArray)
    {
        QAction* action = nullptr;
        if (unit._cmdID == 0)
        {
            // Separator
            action = addSeparator();
        }
        else
        {
            action = new QAction(this);
            action->setIcon(unit._defaultIcon);
            action->setToolTip(unit._tooltip);
            action->setData(unit._cmdID);
            action->setEnabled(true);
            connect(action, &QAction::triggered, this, &ToolBar::onActionTriggered);
        }

        if (action && unit._cmdID != 0)
        {
            _actionMap[unit._cmdID] = action;
            addAction(action);
        }
    }
}

void ToolBar::onActionTriggered()
{
    auto* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        int cmdID = action->data().toInt();
        emit buttonClicked(cmdID);
    }
}

void ToolBar::onCustomContextMenuRequested(const QPoint& pos)
{
    // Show popup menu of hidden/reachable actions if needed
    QMenu menu(this);
    for (int hiddenID : _hiddenActions)
    {
        if (_actionMap.contains(hiddenID))
        {
            menu.addAction(_actionMap[hiddenID]);
        }
    }
    if (!menu.isEmpty())
    {
        menu.exec(mapToGlobal(pos));
    }
}

bool ToolBar::replaceIcon(size_t listIndex, size_t iconIndex, const QString& iconPath)
{
    Q_UNUSED(listIndex);
    Q_UNUSED(iconIndex);
    QIcon icon = loadIconFromPath(iconPath);
    if (icon.isNull())
        return false;

    // For now, just update all icons — would need proper indexing
    for (auto* act : findChildren<QAction*>())
    {
        if (!act->icon().isNull())
        {
            act->setIcon(icon);
            break;
        }
    }
    return true;
}

void ToolBar::enable(int cmdID, bool doEnable)
{
    if (_actionMap.contains(cmdID))
    {
        _actionMap[cmdID]->setEnabled(doEnable);
    }
}

bool ToolBar::getCheckState(int cmdID) const
{
    if (_actionMap.contains(cmdID))
        return _actionMap[cmdID]->isChecked();
    return false;
}

void ToolBar::setCheck(int cmdID, bool willBeChecked)
{
    if (_actionMap.contains(cmdID))
        _actionMap[cmdID]->setChecked(willBeChecked);
}

void ToolBar::setState(ToolBarStatusType state)
{
    _state = state;
    switch (state)
    {
        case ToolBarStatusType::TB_SMALL:
        case ToolBarStatusType::TB_SMALL2:
            _currentIconSize = 16;
            break;
        case ToolBarStatusType::TB_LARGE:
        case ToolBarStatusType::TB_LARGE2:
            _currentIconSize = 32;
            break;
        case ToolBarStatusType::TB_STANDARD:
            // Standard bitmap icons — keep current size
            break;
    }
    applyIconSet();
}

void ToolBar::reduce()
{
    setState(ToolBarStatusType::TB_SMALL);
}

void ToolBar::enlarge()
{
    setState(ToolBarStatusType::TB_LARGE);
}

void ToolBar::reduceToSet2()
{
    setState(ToolBarStatusType::TB_SMALL2);
}

void ToolBar::enlargeToSet2()
{
    setState(ToolBarStatusType::TB_LARGE2);
}

void ToolBar::setToBmpIcons()
{
    setState(ToolBarStatusType::TB_STANDARD);
}

void ToolBar::setDarkMode(bool enabled)
{
    _isDarkMode = enabled;
    if (enabled)
    {
        setStyleSheet(styleSheet() + R"(
            QToolBar {
                background: #1e1e1e;
            }
        )");
    }
    else
    {
        setStyleSheet(styleSheet() + R"(
            QToolBar {
                background: #f0f0f0;
            }
        )");
    }
}

QIcon ToolBar::loadIconFromPath(const QString& path, int size) const
{
    QPixmap pm(size, size);
    if (pm.load(path))
        return QIcon(pm.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    return QIcon();
}

void ToolBar::applyIconSet()
{
    // Reconstruct actions with new icon sizes
    rebuildActions(_buttonArray);
}

void ToolBar::registerDynBtn(int message, const QPixmap& icon, const QIcon& /*absentIcon*/)
{
    Q_UNUSED(message);
    Q_UNUSED(icon);
    // Plugin button registration — would extend button array
}

void ToolBar::registerDynBtnDarkMode(int message, const QPixmap& icon, const QPixmap& /*iconDark*/)
{
    Q_UNUSED(message);
    Q_UNUSED(icon);
}

void ToolBar::doPopup(const QPoint& chevPoint)
{
    // Show popup menu for hidden toolbar items
    QMenu popup(this);
    for (auto* act : findChildren<QAction*>())
    {
        if (!act->isVisible() || !act->isEnabled())
            popup.addAction(act);
    }
    popup.exec(chevPoint);
}

void ToolBar::resizeIconsDpi(int dpi)
{
    Q_UNUSED(dpi);
    // In Qt, DPI scaling is automatic via devicePixelRatio
}


// ─── ReBar ───────────────────────────────────────────────────────────────────────

ReBar::ReBar(QWidget* parent)
    : QWidget(parent)
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    setLayout(_layout);
}

ReBar::~ReBar() = default;

void ReBar::addWidget(QWidget* widget, int id)
{
    if (id == -1)
        id = _bandWidgets.size() + 1;
    _bandWidgets[id] = widget;
    _layout->addWidget(widget);
}

void ReBar::removeWidget(QWidget* widget)
{
    // Remove from layout
    _layout->removeWidget(widget);
    // Remove from map
    for (auto it = _bandWidgets.begin(); it != _bandWidgets.end(); )
    {
        if (it.value() == widget)
            it = _bandWidgets.erase(it);
        else
            ++it;
    }
}

void ReBar::setBandVisible(int id, bool visible)
{
    if (_bandWidgets.contains(id))
        _bandWidgets[id]->setVisible(visible);
}

bool ReBar::isBandVisible(int id) const
{
    if (_bandWidgets.contains(id))
        return _bandWidgets[id]->isVisible();
    return false;
}