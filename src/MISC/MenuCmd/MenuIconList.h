// MISC/MenuCmd/MenuIconList.h - Qt6 port of menu with icons
#pragma once

#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QMap>
#include <QPixmap>

class MenuIconList
{
public:
    static MenuIconList& getInstance();
    
    // Icon management
    void addIcon(int iconId, const QIcon& icon);
    void addIcon(int iconId, const QPixmap& pixmap);
    QIcon getIcon(int iconId) const;
    bool hasIcon(int iconId) const;
    
    // Clear icons
    void clear();
    void clearIcon(int iconId);
    
    // Create icon from file
    bool loadIconFromFile(int iconId, const QString& filePath);
    
    // Standard toolbar icons
    enum StandardIcon {
        IconNew = 0,
        IconOpen = 1,
        IconSave = 2,
        IconSaveAs = 3,
        IconPrint = 4,
        IconCut = 5,
        IconCopy = 6,
        IconPaste = 7,
        IconUndo = 8,
        IconRedo = 9,
        IconFind = 10,
        IconReplace = 11,
        IconHelp = 12
    };
    
private:
    MenuIconList();
    ~MenuIconList() = default;
    
    MenuIconList(const MenuIconList&) = delete;
    MenuIconList& operator=(const MenuIconList&) = delete;
    
    QMap<int, QIcon> _icons;
    
    void initializeStandardIcons();
};

// Helper class to add icons to menu items
class IconMenuHelper
{
public:
    static void setActionIcon(QAction* action, int iconId);
    static void setMenuIcon(QMenu* menu, const QIcon& icon);
    static QAction* addIconAction(QMenu* menu, const QString& text, int iconId, 
                                   const QObject* receiver, const char* member);
};