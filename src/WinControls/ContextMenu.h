// Semantic Lift: Win32 ContextMenu → Qt6 QMenu
// Original: PowerEditor/src/WinControls/ContextMenu/*.{h,cpp}
// Target: npp-qt/src/WinControls/ContextMenu.h

#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QString>
#include <QVector>
#include <string>

// =============================================================================
// MenuItemUnit — represents a single menu item (lifted from ContextMenu.h)
// =============================================================================

struct MenuItemUnit
{
    unsigned long _cmdID = 0;
    std::wstring _itemName;
    std::wstring _parentFolderName;

    MenuItemUnit() = default;
    MenuItemUnit(unsigned long cmdID, const std::wstring& itemName, const std::wstring& parentFolderName = std::wstring())
        : _cmdID(cmdID), _itemName(itemName), _parentFolderName(parentFolderName) {}
};

// =============================================================================
// ContextMenu — context/popup menu (lifted from ContextMenu.h)
// Translates TrackPopupMenu / CreatePopupMenu → QMenu
// =============================================================================

class ContextMenu : public QObject
{
    Q_OBJECT

public:
    explicit ContextMenu(QObject* parent = nullptr);
    ~ContextMenu() override;

    // Create menu from array of items
    // Translates Win32 CreatePopupMenu + InsertMenu → QMenu construction
    void create(QWidget* parent, const QVector<MenuItemUnit>& menuItemArray);

    // Check if menu has been created
    bool isCreated() const { return _menu != nullptr; }

    // Display at screen point
    void display(const QPoint& p) const;

    // Display anchored to a widget (aligns to bottom-left)
    void display(QWidget* anchorWidget) const;

    // Enable/disable an item by command ID
    void enableItem(int cmdID, bool doEnable) const;

    // Check/uncheck an item
    void checkItem(int cmdID, bool doCheck) const;

    // Get underlying QMenu
    QMenu* menuHandle() const { return _menu; }

    // Destroy the menu
    void destroy();

    // Add action with command ID
    void addAction(int cmdID, const QString& text, QObject* receiver, const char* member);

signals:
    void itemClicked(int cmdID);
    void aboutToShow();
    void aboutToHide();

private:
    void populateMenu(QMenu* menu, const QVector<MenuItemUnit>& items, QMenu* parentFolder = nullptr);
    QAction* findAction(int cmdID) const;

    QWidget* _parent = nullptr;
    QMenu* _menu = nullptr;
    QVector<QMenu*> _subMenus;
    QVector<MenuItemUnit> _items;
};
