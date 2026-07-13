// Semantic Lift: Win32 ContextMenu → Qt6 QMenu
// Original: PowerEditor/src/WinControls/ContextMenu/*.{h,cpp}
// Target: npp-qt/src/WinControls/ContextMenu.cpp

#include "ContextMenu.h"
#include <QCursor>
#include <QApplication>
#include <QMenuBar>

ContextMenu::ContextMenu(QObject* parent)
    : QObject(parent)
{
}

ContextMenu::~ContextMenu()
{
    destroy();
}

void ContextMenu::create(QWidget* parent, const QVector<MenuItemUnit>& menuItemArray)
{
    _parent = parent;
    _items = menuItemArray;
    _subMenus.clear();

    // Create the top-level popup menu
    _menu = new QMenu(parent);

    // Connect signals
    connect(_menu, &QMenu::aboutToShow, this, &ContextMenu::aboutToShow);
    connect(_menu, &QMenu::aboutToHide, this, &ContextMenu::aboutToHide);

    // Build menu from items
    QMenu* currentFolder = nullptr;
    QString currentFolderName;
    int folderIndex = 0;

    for (int i = 0; i < menuItemArray.size(); ++i) {
        const MenuItemUnit& item = menuItemArray[i];

        // Handle folder nesting
        if (item._parentFolderName.empty()) {
            currentFolderName.clear();
            currentFolder = nullptr;
            folderIndex = i;
        } else {
            if (QString::fromStdWString(item._parentFolderName) != currentFolderName) {
                currentFolderName = QString::fromStdWString(item._parentFolderName);
                currentFolder = new QMenu(currentFolderName, _menu);
                _subMenus.append(currentFolder);
                _menu->addMenu(currentFolder);
                folderIndex = 0;
            }
        }

        QMenu* targetMenu = currentFolder ? currentFolder : _menu;

        if (item._cmdID == 0) {
            // Separator (cmdID == 0 in Win32)
            // Don't add consecutive separators
            if (targetMenu->actions().isEmpty())
                continue;
            QAction* lastAction = targetMenu->actions().last();
            if (lastAction && !lastAction->isSeparator())
                targetMenu->addSeparator();
        } else {
            QAction* action = new QAction(QString::fromStdWString(item._itemName), _menu);
            action->setData(static_cast<int>(item._cmdID));
            action->setEnabled(true);
            connect(action, &QAction::triggered, this, [this, item]() {
                emit itemClicked(item._cmdID);
            });
            targetMenu->insertAction(nullptr, action);
            if (currentFolder && folderIndex > 0) {
                // Insert after last non-separator in folder
                folderIndex++;
            }
        }
    }
}

void ContextMenu::display(const QPoint& p) const
{
    if (!_menu) return;
    _menu->popup(p);
}

void ContextMenu::display(QWidget* anchorWidget) const
{
    if (!_menu || !anchorWidget) return;

    // Align to bottom-left of the anchor widget
    QPoint p = anchorWidget->mapToGlobal(QPoint(0, anchorWidget->height()));
    _menu->popup(p);
}

void ContextMenu::enableItem(int cmdID, bool doEnable) const
{
    QAction* action = findAction(cmdID);
    if (action) action->setEnabled(doEnable);
}

void ContextMenu::checkItem(int cmdID, bool doCheck) const
{
    QAction* action = findAction(cmdID);
    if (action) action->setCheckable(true);
    if (action) action->setChecked(doCheck);
}

QAction* ContextMenu::findAction(int cmdID) const
{
    if (!_menu) return nullptr;
    for (QAction* action : _menu->actions()) {
        if (action->data().toInt() == cmdID)
            return action;
        if (action->menu()) {
            for (QAction* subAction : action->menu()->actions()) {
                if (subAction->data().toInt() == cmdID)
                    return subAction;
            }
        }
    }
    return nullptr;
}

void ContextMenu::destroy()
{
    if (_menu) {
        delete _menu;
        _menu = nullptr;
    }
    for (QMenu* sub : _subMenus) {
        delete sub;
    }
    _subMenus.clear();
}

void ContextMenu::addAction(int cmdID, const QString& text, QObject* receiver, const char* member)
{
    if (!_menu) return;
    QAction* action = _menu->addAction(text, receiver, member);
    action->setData(cmdID);
}
