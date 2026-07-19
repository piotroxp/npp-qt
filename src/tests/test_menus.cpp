// test_menus.cpp — MenuAdapter and context-menu unit tests
// Copyright (C) 2026 Agent Army / GPL v3
//
// Tests verify:
//   - MenuAdapter command mapping (Win32 IDs ↔ Qt command strings)
//   - MenuAdapter enable/check registration
//   - ContextMenuAdapter item building (separators, submenus)
//   - Win32 ID → MenuItemUnit roundtrip
//   - QMenu action lifecycle (add, exec, delete)
//   - MenuAdapter::applyMenuRoles() (no-op on Linux, safe to call)
//
// Run with:  QT_QPA_PLATFORM=offscreen ./build/src/tests/test_menus
// Compile:   cmake --build build --target test_menus

#include "common/MenuAdapter.h"
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <cassert>
#include <iostream>

// ========================================================================
// Test helpers
// ========================================================================

#define ASSERT_TRUE(x) \
    do { if (!(x)) { qWarning() << "FAILED: " << #x << " at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) \
    do { if (!!(x)) { qWarning() << "FAILED: " << #x << " at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define ASSERT_EQ(a, b) \
    do { auto _a = (a); auto _b = (b); \
         if (_a != _b) { qWarning() << "FAILED: " << #a << " == " << #b << " at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define TEST(name) void name()
#define RUN(name) \
    do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)

// ========================================================================
// Test 1 — MenuItemUnit construction (Win32 mirror)
// ========================================================================
TEST(test_menu_item_unit_default) {
    MenuItemUnit item;
    ASSERT_EQ(item.cmdID, 0);
    ASSERT_TRUE(item.itemName.isEmpty());
    ASSERT_TRUE(item.parentFolderName.isEmpty());
}

TEST(test_menu_item_unit_with_id) {
    MenuItemUnit item(41003, QStringLiteral("Cu&t"), QStringLiteral("Edit"));
    ASSERT_EQ(item.cmdID, 41003);
    ASSERT_EQ(item.itemName, QStringLiteral("Cu&t"));
    ASSERT_EQ(item.parentFolderName, QStringLiteral("Edit"));
}

TEST(test_menu_item_unit_separator) {
    MenuItemUnit sep;
    ASSERT_EQ(sep.cmdID, 0);  // cmdID==0 means separator in Win32 and Qt
}

TEST(test_menu_item_unit_explicit_separator) {
    MenuItemUnit sep(0);
    ASSERT_EQ(sep.cmdID, 0);
}

// ========================================================================
// Test 2 — MenuAdapter command mapping
// ========================================================================
TEST(test_adapter_map_command) {
    MenuAdapter adapter;
    adapter.mapCommand(41003, QStringLiteral("edit.cut"));
    adapter.mapCommand(41004, QStringLiteral("edit.copy"));
    adapter.mapCommand(41005, QStringLiteral("edit.paste"));

    // Forward lookup: Win32 ID → Qt command string
    ASSERT_EQ(adapter.commandString(41003), QStringLiteral("edit.cut"));
    ASSERT_EQ(adapter.commandString(41004), QStringLiteral("edit.copy"));
    ASSERT_EQ(adapter.commandString(41005), QStringLiteral("edit.paste"));

    // Reverse lookup: Qt command string → Win32 ID
    ASSERT_EQ(adapter.commandID(QStringLiteral("edit.cut")), 41003);
    ASSERT_EQ(adapter.commandID(QStringLiteral("edit.copy")), 41004);
    ASSERT_EQ(adapter.commandID(QStringLiteral("edit.paste")), 41005);

    // Unknown returns empty/-1
    ASSERT_EQ(adapter.commandString(99999), QString());
    ASSERT_EQ(adapter.commandID(QStringLiteral("unknown")), -1);

    // hasCommand
    ASSERT_TRUE(adapter.hasCommand(41003));
    ASSERT_TRUE(adapter.hasCommand(QStringLiteral("edit.cut")));
    ASSERT_FALSE(adapter.hasCommand(99999));
    ASSERT_FALSE(adapter.hasCommand(QStringLiteral("unknown")));
}

TEST(test_adapter_register_items) {
    MenuAdapter adapter;
    QList<MenuItemUnit> items = {
        MenuItemUnit(41003, QStringLiteral("Cu&t")),
        MenuItemUnit(41004, QStringLiteral("&Copy")),
        MenuItemUnit(41005, QStringLiteral("&Paste")),
        MenuItemUnit(0),  // separator
        MenuItemUnit(41006, QStringLiteral("Select &All")),
    };
    adapter.registerItems(items);

    // Each item should be registered under "cmd.<ID>"
    ASSERT_TRUE(adapter.hasCommand(41003));
    ASSERT_TRUE(adapter.hasCommand(41004));
    ASSERT_TRUE(adapter.hasCommand(41005));
    ASSERT_TRUE(adapter.hasCommand(41006));
    ASSERT_FALSE(adapter.hasCommand(0));  // separator — no mapping

    // Verify the generated command strings
    ASSERT_EQ(adapter.commandString(41003), QStringLiteral("cmd.41003"));
    ASSERT_EQ(adapter.commandString(41006), QStringLiteral("cmd.41006"));
}

// ========================================================================
// Test 3 — MenuAdapter enable/check with registerAction
// ========================================================================
TEST(test_adapter_enable_check) {
    // Create a real QMenuBar with one menu and one action.
    QMenuBar menuBar;
    QMenu* fileMenu = menuBar.addMenu(QStringLiteral("&File"));
    QAction* newAction = fileMenu->addAction(QStringLiteral("&New"));
    newAction->setData(QStringLiteral("file.new"));
    newAction->setCheckable(true);

    MenuAdapter adapter;
    adapter.mapCommand(40001, QStringLiteral("file.new"));
    adapter.registerAction(40001, newAction);

    // Action starts enabled.
    ASSERT_TRUE(newAction->isEnabled());

    // Disable via adapter.
    adapter.setEnabled(40001, false);
    ASSERT_FALSE(newAction->isEnabled());

    // Re-enable.
    adapter.setEnabled(40001, true);
    ASSERT_TRUE(newAction->isEnabled());

    // Checkable
    adapter.setChecked(40001, true);
    ASSERT_TRUE(newAction->isChecked());

    adapter.setChecked(40001, false);
    ASSERT_FALSE(newAction->isChecked());
}

// ========================================================================
// Test 4 — MenuAdapter::trigger() via registerAction
// ========================================================================
TEST(test_adapter_trigger) {
    QMenuBar menuBar;
    QMenu* editMenu = menuBar.addMenu(QStringLiteral("&Edit"));
    QAction* cutAction = editMenu->addAction(QStringLiteral("Cu&t"));
    cutAction->setData(QStringLiteral("edit.cut"));

    MenuAdapter adapter;
    adapter.mapCommand(41003, QStringLiteral("edit.cut"));
    adapter.registerAction(41003, cutAction);

    // Record whether the action fires when triggered via adapter.
    bool triggered = false;
    QObject::connect(cutAction, &QAction::triggered, cutAction, [&]() { triggered = true; });

    adapter.trigger(41003);
    ASSERT_TRUE(triggered);
    triggered = false;

    adapter.trigger(QStringLiteral("edit.cut"));
    ASSERT_TRUE(triggered);
}

// ========================================================================
// Test 5 — MenuAdapter::connectMenuBar() discovers actions
// ========================================================================
TEST(test_adapter_connect_menu_bar) {
    QMenuBar menuBar;
    QMenu* fileMenu = menuBar.addMenu(QStringLiteral("&File"));
    fileMenu->addAction(QStringLiteral("&New"))->setData(QStringLiteral("file.new"));
    fileMenu->addAction(QStringLiteral("&Open..."))->setData(QStringLiteral("file.open"));
    fileMenu->addSeparator();
    QMenu* recentMenu = fileMenu->addMenu(QStringLiteral("Open &Recent"));
    recentMenu->addAction(QStringLiteral("file1.txt"))->setData(QStringLiteral("recent.file1"));
    recentMenu->addAction(QStringLiteral("file2.txt"))->setData(QStringLiteral("recent.file2"));

    MenuAdapter adapter;
    adapter.mapCommand(40001, QStringLiteral("file.new"));
    adapter.mapCommand(40002, QStringLiteral("file.open"));
    adapter.mapCommand(40003, QStringLiteral("recent.file1"));
    adapter.mapCommand(40004, QStringLiteral("recent.file2"));

    adapter.connectMenuBar(&menuBar);

    // Forward and reverse lookups work through the adapter
    ASSERT_EQ(adapter.commandID(QStringLiteral("file.new")), 40001);
    ASSERT_EQ(adapter.commandID(QStringLiteral("file.open")), 40002);
    ASSERT_EQ(adapter.commandID(QStringLiteral("recent.file1")), 40003);
    ASSERT_EQ(adapter.commandID(QStringLiteral("recent.file2")), 40004);

    // Trigger via Win32 ID — should fire the action
    bool newFired = false;
    bool openFired = false;
    QObject::connect(fileMenu->actions()[0], &QAction::triggered, fileMenu,
                     [&]() { newFired = true; });
    QObject::connect(fileMenu->actions()[1], &QAction::triggered, fileMenu,
                     [&]() { openFired = true; });

    adapter.trigger(40001);
    ASSERT_TRUE(newFired);

    adapter.trigger(40002);
    ASSERT_TRUE(openFired);
}

// ========================================================================
// Test 6 — ContextMenuAdapter basic operations
// ========================================================================
TEST(test_context_menu_add_items) {
    ContextMenuAdapter ctx(nullptr);
    ctx.addItem(41003, QStringLiteral("Cu&t"));
    ctx.addItem(41004, QStringLiteral("&Copy"));
    ctx.addItem(41005, QStringLiteral("&Paste"));
    ctx.addSeparator();
    ctx.addItem(41006, QStringLiteral("Select &All"));

    QMenu* menu = ctx.build();
    ASSERT_TRUE(menu != nullptr);

    // Should have: 3 items + 1 sep + 1 item = 5 entries
    const auto actions = menu->actions();
    ASSERT_EQ(actions.size(), 5);
    ASSERT_EQ(actions[0]->data().toInt(), 41003);
    ASSERT_EQ(actions[1]->data().toInt(), 41004);
    ASSERT_EQ(actions[2]->data().toInt(), 41005);
    ASSERT_TRUE(actions[3]->isSeparator());
    ASSERT_EQ(actions[4]->data().toInt(), 41006);

    delete menu;
}

TEST(test_context_menu_submenu) {
    ContextMenuAdapter ctx(nullptr);
    ctx.addItem(43013, QStringLiteral("Toggle Folds"), QStringLiteral("Folding"));
    ctx.addItem(43032, QStringLiteral("Collapse All"), QStringLiteral("Folding"));
    ctx.addItem(43033, QStringLiteral("Uncollapse All"), QStringLiteral("Folding"));
    ctx.addSeparator();
    ctx.addItem(41003, QStringLiteral("Cu&t"));

    QMenu* menu = ctx.build();
    const auto actions = menu->actions();

    // First action should be a submenu named "Folding"
    ASSERT_TRUE(actions[0]->menu() != nullptr);
    ASSERT_EQ(actions[0]->menu()->title(), QStringLiteral("Folding"));

    // Submenu should contain 3 items
    const auto subActions = actions[0]->menu()->actions();
    ASSERT_EQ(subActions.size(), 3);
    ASSERT_EQ(subActions[0]->data().toInt(), 43013);
    ASSERT_EQ(subActions[1]->data().toInt(), 43032);
    ASSERT_EQ(subActions[2]->data().toInt(), 43033);

    // Then separator and Cut
    ASSERT_TRUE(actions[1]->isSeparator());
    ASSERT_EQ(actions[2]->data().toInt(), 41003);

    delete menu;
}

TEST(test_context_menu_clear) {
    ContextMenuAdapter ctx(nullptr);
    ctx.addItem(41003, QStringLiteral("Cu&t"));
    ctx.addSeparator();
    ctx.addItem(41004, QStringLiteral("&Copy"));

    QMenu* menu1 = ctx.build();
    ASSERT_EQ(menu1->actions().size(), 3);

    ctx.clear();
    QMenu* menu2 = ctx.build();
    ASSERT_EQ(menu2->actions().size(), 0);

    delete menu1;
    delete menu2;
}

TEST(test_context_menu_from_items) {
    QList<MenuItemUnit> items = {
        MenuItemUnit(41003, QStringLiteral("Cu&t")),
        MenuItemUnit(0),  // separator
        MenuItemUnit(41004, QStringLiteral("&Copy")),
        MenuItemUnit(41005, QStringLiteral("&Paste"), QStringLiteral("Edit")),
    };
    ContextMenuAdapter ctx(nullptr);
    ctx.fromItems(items);

    QMenu* menu = ctx.build();
    const auto actions = menu->actions();

    // Structure: Cut, sep, Copy, Edit(Paste) — mirrors Win32 ContextMenu exactly.
    // No auto-separator before folder groups; caller controls separators.
    ASSERT_EQ(actions.size(), 4);
    ASSERT_EQ(actions[0]->data().toInt(), 41003);  // Cut
    ASSERT_TRUE(actions[1]->isSeparator());        // explicit separator
    ASSERT_EQ(actions[2]->data().toInt(), 41004);  // Copy
    // 4th item: submenu "Edit" with Paste (parentFolder="Edit")
    ASSERT_TRUE(actions[3]->menu() != nullptr);
    ASSERT_EQ(actions[3]->menu()->title(), QStringLiteral("Edit"));
    ASSERT_EQ(actions[3]->menu()->actions().size(), 1);
    ASSERT_EQ(actions[3]->menu()->actions()[0]->data().toInt(), 41005);  // Paste

    delete menu;
}

TEST(test_context_menu_triggered_signal) {
    ContextMenuAdapter ctx(nullptr);
    ctx.addItem(41003, QStringLiteral("Cu&t"));
    ctx.addItem(41004, QStringLiteral("&Copy"));
    ctx.addSeparator();
    ctx.addItem(41006, QStringLiteral("Select &All"));

    // Track signals manually
    int triggeredCount = 0;
    int lastCmdID = -1;
    QObject::connect(&ctx, &ContextMenuAdapter::triggered, &ctx,
        [&](int cmdID) { triggeredCount++; lastCmdID = cmdID; });

    QMenu* menu = ctx.build();

    // Simulate clicking "Cut"
    for (QAction* action : menu->actions()) {
        if (action->data().toInt() == 41003) {
            action->trigger();
            break;
        }
    }

    ASSERT_EQ(triggeredCount, 1);
    ASSERT_EQ(lastCmdID, 41003);

    delete menu;
}

// ========================================================================
// Test 7 — Win32 ID range coverage
// ========================================================================
TEST(test_win32_id_ranges) {
    MenuAdapter adapter;
    // File: 40001-40029
    adapter.mapCommand(40001, QStringLiteral("file.new"));
    adapter.mapCommand(40002, QStringLiteral("file.open"));
    adapter.mapCommand(40005, QStringLiteral("file.save"));
    adapter.mapCommand(40006, QStringLiteral("file.saveAs"));
    adapter.mapCommand(40013, QStringLiteral("file.exit"));
    // Edit: 41001-41039
    adapter.mapCommand(41001, QStringLiteral("edit.undo"));
    adapter.mapCommand(41002, QStringLiteral("edit.redo"));
    adapter.mapCommand(41003, QStringLiteral("edit.cut"));
    adapter.mapCommand(41004, QStringLiteral("edit.copy"));
    adapter.mapCommand(41005, QStringLiteral("edit.paste"));
    adapter.mapCommand(41006, QStringLiteral("edit.selectAll"));
    adapter.mapCommand(41015, QStringLiteral("edit.lowercase"));
    adapter.mapCommand(41016, QStringLiteral("edit.uppercase"));
    // Search: 42001-42018
    adapter.mapCommand(42001, QStringLiteral("search.find"));
    adapter.mapCommand(42002, QStringLiteral("search.replace"));
    adapter.mapCommand(42003, QStringLiteral("search.findNext"));
    adapter.mapCommand(42007, QStringLiteral("search.goto"));
    // View: 43001-43059
    adapter.mapCommand(43050, QStringLiteral("view.lineNumbers"));
    adapter.mapCommand(43047, QStringLiteral("view.wrap"));
    // Encoding: 44001-44007
    adapter.mapCommand(44002, QStringLiteral("encoding.utf8"));
    adapter.mapCommand(44003, QStringLiteral("encoding.utf8bom"));
    // Language: 45001-45030
    adapter.mapCommand(45001, QStringLiteral("lang.c"));
    adapter.mapCommand(45002, QStringLiteral("lang.cpp"));
    adapter.mapCommand(45008, QStringLiteral("lang.python"));
    // Settings: 46001-46013
    adapter.mapCommand(46001, QStringLiteral("settings.preferences"));
    adapter.mapCommand(46002, QStringLiteral("settings.shortcutMapper"));

    // Verify all registered
    ASSERT_TRUE(adapter.hasCommand(40001));
    ASSERT_TRUE(adapter.hasCommand(41001));
    ASSERT_TRUE(adapter.hasCommand(42001));
    ASSERT_TRUE(adapter.hasCommand(43050));
    ASSERT_TRUE(adapter.hasCommand(44002));
    ASSERT_TRUE(adapter.hasCommand(45001));
    ASSERT_TRUE(adapter.hasCommand(46001));

    // Verify lookups
    ASSERT_EQ(adapter.commandString(42001), QStringLiteral("search.find"));
    ASSERT_EQ(adapter.commandID(QStringLiteral("lang.python")), 45008);
}

// ========================================================================
// Test 8 — applyMenuRoles() is safe to call (no-op on non-macOS)
// ========================================================================
TEST(test_apply_menu_roles_no_crash) {
    QMenuBar menuBar;
    QMenu* fileMenu = menuBar.addMenu(QStringLiteral("&File"));
    fileMenu->addAction(QStringLiteral("&New"));
    QMenu* editMenu = menuBar.addMenu(QStringLiteral("&Edit"));
    editMenu->addAction(QStringLiteral("&Undo"));
    QMenu* helpMenu = menuBar.addMenu(QStringLiteral("&Help"));
    helpMenu->addAction(QStringLiteral("&About"));

    MenuAdapter adapter;
    adapter.setMenuBar(&menuBar);

    // applyMenuRoles() must not throw or crash on any platform.
    adapter.applyMenuRoles();

    // Verify actions still exist after the call.
    ASSERT_EQ(menuBar.actions().size(), 3u);  // File, Edit, Help top-level menus

    std::cout << "  (roles applied — Qt::MenuRole is platform-dependent, no assertion on role values)\n";
}

// ========================================================================
// Test 9 — roundtrip Win32 ID → Qt command → Win32 ID
// ========================================================================
TEST(test_roundtrip_id_command) {
    MenuAdapter adapter;
    adapter.mapCommand(42001, QStringLiteral("search.find"));
    adapter.mapCommand(42002, QStringLiteral("search.replace"));
    adapter.mapCommand(42003, QStringLiteral("search.findNext"));
    adapter.mapCommand(42004, QStringLiteral("search.findPrev"));

    // Roundtrip: Win32 ID → Qt command → Win32 ID
    QString cmd = adapter.commandString(42001);
    ASSERT_EQ(adapter.commandID(cmd), 42001);

    // Multiple IDs
    cmd = adapter.commandString(42003);
    ASSERT_EQ(adapter.commandID(cmd), 42003);
}

// ========================================================================
// Test 10 — applyMenuRoles for Settings/About items
// ========================================================================
TEST(test_apply_menu_roles_preferences_and_about) {
    QMenuBar menuBar;
    QMenu* settingsMenu = menuBar.addMenu(QStringLiteral("Se&ttings"));
    settingsMenu->addAction(QStringLiteral("&Preferences"))->setData(QStringLiteral("settings.preferences"));
    settingsMenu->addAction(QStringLiteral("&Shortcut Mapper"))->setData(QStringLiteral("settings.shortcutMapper"));
    QMenu* helpMenu = menuBar.addMenu(QStringLiteral("&Help"));
    helpMenu->addAction(QStringLiteral("&About"))->setData(QStringLiteral("help.about"));
    helpMenu->addAction(QStringLiteral("&Documentation"))->setData(QStringLiteral("help.docs"));

    MenuAdapter adapter;
    adapter.setMenuBar(&menuBar);
    adapter.applyMenuRoles();

    // Menu bar still intact
    ASSERT_EQ(menuBar.actions().size(), 2u);

    std::cout << "  OK\n";
}

// ========================================================================
// Main
// ========================================================================
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    std::cout << "=== MenuAdapter / ContextMenu unit tests ===\n";

    RUN(test_menu_item_unit_default);
    RUN(test_menu_item_unit_with_id);
    RUN(test_menu_item_unit_separator);
    RUN(test_menu_item_unit_explicit_separator);
    RUN(test_adapter_map_command);
    RUN(test_adapter_register_items);
    RUN(test_adapter_enable_check);
    RUN(test_adapter_trigger);
    RUN(test_adapter_connect_menu_bar);
    RUN(test_context_menu_add_items);
    RUN(test_context_menu_submenu);
    RUN(test_context_menu_clear);
    RUN(test_context_menu_from_items);
    RUN(test_context_menu_triggered_signal);
    RUN(test_win32_id_ranges);
    RUN(test_apply_menu_roles_no_crash);
    RUN(test_roundtrip_id_command);
    RUN(test_apply_menu_roles_preferences_and_about);

    std::cout << "\nAll tests passed.\n";
    return 0;
}
