// MISC/MenuCmd/MenuIconList.cpp - Qt6 port of menu with icons
#include "MenuIconList.h"
#include <QFile>
#include <QStyle>

MenuIconList::MenuIconList()
{
    initializeStandardIcons();
}

MenuIconList& MenuIconList::getInstance()
{
    static MenuIconList instance;
    return instance;
}

void MenuIconList::initializeStandardIcons()
{
    // Standard icons are loaded from resources or system theme
    // This is a placeholder - real implementation would load from files
}

void MenuIconList::addIcon(int iconId, const QIcon& icon)
{
    _icons[iconId] = icon;
}

void MenuIconList::addIcon(int iconId, const QPixmap& pixmap)
{
    _icons[iconId] = QIcon(pixmap);
}

QIcon MenuIconList::getIcon(int iconId) const
{
    auto it = _icons.find(iconId);
    if (it != _icons.end())
        return it.value();
    return QIcon();
}

bool MenuIconList::hasIcon(int iconId) const
{
    return _icons.contains(iconId);
}

void MenuIconList::clear()
{
    _icons.clear();
}

void MenuIconList::clearIcon(int iconId)
{
    _icons.remove(iconId);
}

bool MenuIconList::loadIconFromFile(int iconId, const QString& filePath)
{
    if (!QFile::exists(filePath))
        return false;
    
    QPixmap pixmap(filePath);
    if (pixmap.isNull())
        return false;
    
    addIcon(iconId, pixmap);
    return true;
}

// IconMenuHelper implementation
void IconMenuHelper::setActionIcon(QAction* action, int iconId)
{
    if (!action) return;
    QIcon icon = MenuIconList::getInstance().getIcon(iconId);
    if (!icon.isNull())
        action->setIcon(icon);
}

void IconMenuHelper::setMenuIcon(QMenu* menu, const QIcon& icon)
{
    if (menu)
        menu->setIcon(icon);
}

QAction* IconMenuHelper::addIconAction(QMenu* menu, const QString& text, int iconId,
                                       const QObject* receiver, const char* member)
{
    if (!menu) return nullptr;
    
    QAction* action = menu->addAction(text);
    setActionIcon(action, iconId);
    
    if (receiver && member)
        QObject::connect(action, SIGNAL(triggered()), receiver, member);
    
    return action;
}