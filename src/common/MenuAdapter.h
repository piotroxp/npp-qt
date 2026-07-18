// MenuAdapter.h - Win32 → Qt6 menu adapter for Notepad--Qt
// Translates Win32 HMENU/MENUITEMINFO semantics and menu IDs into Qt6
// QMenuBar/QMenu/QAction equivalents while preserving full Notepad++ semantics.
// Copyright (C) 2026 Agent Army / GPL v3

#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMap>
#include <QSet>
#include <QString>
#include <QVariant>
#include <QList>
#include <QIcon>
#include <functional>

// ============================================================================
// MenuItemUnit — mirrors Win32 ContextMenu::MenuItemUnit
// Each item carries a Win32 command ID and optional parent-folder name
// (for submenu nesting that matches Notepad++'s structure).
// ============================================================================
struct MenuItemUnit {
    int cmdID = 0;
    QString itemName;
    QString parentFolderName;

    MenuItemUnit() = default;

    /// Separator constructor — cmdID==0 marks a separator in both Win32 and Qt.
    explicit MenuItemUnit(int cmdID)
        : cmdID(cmdID) {}

    MenuItemUnit(int cmdID, const QString& itemName,
                 const QString& parentFolderName = QString())
        : cmdID(cmdID), itemName(itemName), parentFolderName(parentFolderName) {}
};

// ============================================================================
// MenuAdapter — bridges Win32 menu command space (IDM_*) with Qt6 signals
//
// This class is DATA-ONLY.  It does NOT own any QMenu or QAction objects.
// It stores Win32 ID ↔ Qt command string mappings and enables lookups.
// Menu actions are created and owned by the MenuBar or ContextMenu.
//
// Usage:
//   MenuAdapter adapter;
//   adapter.mapCommand(IDM_FILE_NEW,  "file.new");
//   adapter.mapCommand(IDM_EDIT_COPY, "edit.copy");
//
// When an action is triggered, look up its Qt command string and dispatch:
//   QString cmd = adapter.commandString(action->data().toString());
//   if (!cmd.isEmpty()) emit command(cmd);
// ============================================================================
class MenuAdapter : public QObject {
    Q_OBJECT
public:
    explicit MenuAdapter(QObject* parent = nullptr);
    ~MenuAdapter() override;

    // ---- Win32 command ID → Qt command string mapping ----

    /// Map a Win32 menu command ID (e.g. IDM_EDIT_COPY = 41004) to a Qt
    /// command string (e.g. "edit.copy").
    void mapCommand(int win32ID, const QString& qtCommand);

    /// Convenience: register a batch of MenuItemUnits at once.
    void registerItems(const QList<MenuItemUnit>& items);

    // ---- Lookup ----

    /// Return the Qt command string for a Win32 command ID, or empty string.
    QString commandString(int win32ID) const;

    /// Return the Win32 command ID for a Qt command string, or -1.
    int commandID(const QString& qtCommand) const;

    /// Return true if the given Win32 ID is registered.
    bool hasCommand(int win32ID) const;

    /// Return true if the given Qt command string is registered.
    bool hasCommand(const QString& qtCommand) const;

    // ---- Enable/check state (for Win32 enable/check mirrors) ----

    /// Register an action for a Win32 command ID so enable/check can be
    /// applied to it.  The MenuAdapter does NOT take ownership.
    void registerAction(int win32ID, QAction* action);

    /// Enable or disable a menu item by Win32 command ID.
    void setEnabled(int cmdID, bool enabled);

    /// Check or uncheck a menu item by Win32 command ID.
    void setChecked(int cmdID, bool checked);

    // ---- MenuBar discovery ----

    /// Scan all QMenus and QActions inside `menuBar` and connect every
    /// action whose data() matches a registered Qt command string.
    void connectMenuBar(QMenuBar* menuBar);

    /// Attach a QMenuBar and automatically discover all its actions.
    void setMenuBar(QMenuBar* menuBar);

    // ---- macOS / Linux desktop environment integration ----

    /// Apply Qt::MenuRole to every top-level menu for proper macOS/Linux
    /// DE integration.  Call once after building the menu bar.
    ///
    /// Uses QAction::setMenuRole():
    ///   File / Edit / View / Encoding / Language → Qt::ApplicationSpecificRole
    ///   Settings → Qt::PreferencesRole
    ///   Window   → Qt::WindowRole
    ///   Help     → Qt::HelpRole
    void applyMenuRoles();

Q_SIGNALS:
    /// Emitted whenever a registered action is triggered.
    /// Contains the Win32 command ID.
    void command(int cmdID);

    /// Emitted whenever a registered action is triggered.
    /// Contains the Qt command string (e.g. "edit.copy").
    void commandString(const QString& command);

public Q_SLOTS:
    /// Trigger the action associated with `cmdID`.
    void trigger(int cmdID);

    /// Trigger the action associated with `qtCommand`.
    void trigger(const QString& qtCommand);

private Q_SLOTS:
    /// Handles QAction::triggered for all connected actions.
    void onActionTriggered();

private:
    void discoverActionsInMenu(QMenu* menu);

    // Win32 command ID ↔ Qt command string bidirectional map
    QMap<int, QString> _idToCommand;
    QMap<QString, int> _commandToId;

    // Win32 command ID → registered QAction (non-owning)
    QMap<int, QAction*> _idToAction;

    // Track which actions have been connected (prevent double-connect)
    QSet<QObject*> _connectedActions;

    // Current menu bar
    QMenuBar* _menuBar = nullptr;
};

// ============================================================================
// ContextMenuAdapter — creates Qt6 context menus from Win32 MenuItemUnit lists
//
// Mirrors the Win32 ContextMenu class exactly but uses QMenu instead of HMENU.
// This class OWNS the QMenu it creates (returns it for exec; caller deletes).
//
// Usage:
//   ContextMenuAdapter ctx(parentWidget);
//   ctx.addItem(41003, "Cu&t");
//   ctx.addItem(41004, "&Copy");
//   ctx.addSeparator();
//   ctx.addItem(41001, "&Undo");
//   ctx.exec(QCursor::pos());
// ============================================================================
class ContextMenuAdapter : public QObject {
    Q_OBJECT
public:
    explicit ContextMenuAdapter(QWidget* parent = nullptr);
    ~ContextMenuAdapter() override;

    /// Add a menu item.  cmdID==0 adds a separator.
    void addItem(int cmdID, const QString& text,
                 const QString& parentFolder = QString());
    void addItems(const QList<MenuItemUnit>& items);
    void addSeparator();

    /// Clear all items.
    void clear();

    /// Build and return the QMenu.  Caller OWNS the returned pointer.
    QMenu* build();

    /// Populate from a list of MenuItemUnits (matches Win32 ContextMenu API).
    void fromItems(const QList<MenuItemUnit>& items);

    /// Build and immediately show the menu at `pos` (widget coordinates) or
    /// at the cursor if pos is null.  Returns the triggered action or nullptr.
    QAction* exec(const QPoint& pos = QPoint());

Q_SIGNALS:
    void triggered(int cmdID);
    void triggeredString(const QString& command);

private:
    void rebuildMenu(QMenu* menu);

    struct Item {
        int cmdID = 0;
        QString text;
        QString parentFolder;
        bool isSeparator = false;
    };

    QList<Item> _items;
    QWidget* _parent = nullptr;
    mutable QMenu* _menu = nullptr;  // lazy-built, owned by caller
};
