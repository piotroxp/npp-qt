// Semantic Lift: ContextMenu — Win32 HMENU + TrackPopupMenu → Qt6 QMenu + exec()
// Original: PowerEditor/src/WinControls/ContextMenu/ContextMenu.cpp (160 lines)
// Win32 → Qt6: HMENU + TrackPopupMenu + GetCursorPos → QMenu::exec(QCursor::pos())

#include "ContextMenu.h"
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QApplication>
#include <QWidget>

ContextMenu::ContextMenu()
    : _hMenu(nullptr)
{
}

ContextMenu::~ContextMenu()
{
    // QMenu auto-deletes children
}

void ContextMenu::init(QWidget* parent)
{
    _parent = parent;
}

void ContextMenu::addItem(const QString& text, int cmdId, bool isEnabled)
{
    QAction* action = new QAction(text, _parent);
    action->setData(cmdId);
    action->setEnabled(isEnabled);
    connect(action, &QAction::triggered, this, [this, cmdId]() {
        emit menuCommand(cmdId);
    });
    _menu.addAction(action);
}

void ContextMenu::addSeparator()
{
    _menu.addSeparator();
}

void ContextMenu::addSubMenu(const QString& text, QMenu* subMenu)
{
    QAction* action = _menu.addMenu(text);
    // Copy submenu actions if needed
    Q_UNUSED(subMenu);
}

void ContextMenu::removeItem(int cmdId)
{
    for (QAction* action : _menu.actions()) {
        if (action->data().toInt() == cmdId) {
            _menu.removeAction(action);
            delete action;
            break;
        }
    }
}

void ContextMenu::enableItem(int cmdId, bool enable)
{
    for (QAction* action : _menu.actions()) {
        if (action->data().toInt() == cmdId) {
            action->setEnabled(enable);
            break;
        }
    }
}

void ContextMenu::checkItem(int cmdId, bool check)
{
    for (QAction* action : _menu.actions()) {
        if (action->data().toInt() == cmdId) {
            action->setCheckable(true);
            action->setChecked(check);
            break;
        }
    }
}

void ContextMenu::setItemText(int cmdId, const QString& text)
{
    for (QAction* action : _menu.actions()) {
        if (action->data().toInt() == cmdId) {
            action->setText(text);
            break;
        }
    }
}

void ContextMenu::trackPopupMenu()
{
    // Qt's QMenu::exec() shows menu at cursor position
    QAction* selected = _menu.exec(QCursor::pos());
    if (selected && selected->data().isValid()) {
        emit menuCommand(selected->data().toInt());
    }
}

void ContextMenu::trackPopupMenu(const QPoint& pt, QWidget* parent)
{
    Q_UNUSED(parent);
    QAction* selected = _menu.exec(pt);
    if (selected && selected->data().isValid()) {
        emit menuCommand(selected->data().toInt());
    }
}

void ContextMenu::destroy()
{
    // QMenu auto-cleans up
    // Re-create for reuse
}

QMenu& ContextMenu::menu()
{
    return _menu;
}

void ContextMenu::clear()
{
    _menu.clear();
}
