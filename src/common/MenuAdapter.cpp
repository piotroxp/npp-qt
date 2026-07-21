// MenuAdapter.cpp - Win32 → Qt6 menu adapter implementation
// Copyright (C) 2026 Agent Army / GPL v3

#include "MenuAdapter.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QWidget>
#include <QDebug>

// ============================================================================
// MenuAdapter — DATA-ONLY design
//
// Does NOT own any QMenu or QAction objects.
// Stores Win32 ID ↔ Qt command string mappings only.
// Actions are connected/discovered via connectMenuBar() but NOT owned here.
// ============================================================================

MenuAdapter::MenuAdapter(QObject* parent) : QObject(parent) {}

MenuAdapter::~MenuAdapter() = default;

void MenuAdapter::mapCommand(int win32ID, const QString& qtCommand) {
    if (win32ID <= 0 || qtCommand.isEmpty()) return;
    _idToCommand[win32ID] = qtCommand;
    _commandToId[qtCommand] = win32ID;
}

void MenuAdapter::registerItems(const QList<MenuItemUnit>& items) {
    for (const auto& item : items) {
        if (item.cmdID != 0 && !item.itemName.isEmpty()) {
            mapCommand(item.cmdID, QStringLiteral("cmd.%1").arg(item.cmdID));
        }
    }
}

QString MenuAdapter::commandString(int win32ID) const {
    return _idToCommand.value(win32ID, QString());
}

int MenuAdapter::commandID(const QString& qtCommand) const {
    return _commandToId.value(qtCommand, -1);
}

bool MenuAdapter::hasCommand(int win32ID) const {
    return _idToCommand.contains(win32ID);
}

bool MenuAdapter::hasCommand(const QString& qtCommand) const {
    return _commandToId.contains(qtCommand);
}

void MenuAdapter::registerAction(int win32ID, QAction* action) {
    if (!action) return;
    _idToAction[win32ID] = action;
}

void MenuAdapter::setEnabled(int cmdID, bool enabled) {
    if (QAction* action = _idToAction.value(cmdID, nullptr)) {
        action->setEnabled(enabled);
    }
}

void MenuAdapter::setChecked(int cmdID, bool checked) {
    if (QAction* action = _idToAction.value(cmdID, nullptr)) {
        action->setChecked(checked);
    }
}

void MenuAdapter::setMenuBar(QMenuBar* menuBar) {
    _menuBar = menuBar;
    if (menuBar) {
        connectMenuBar(menuBar);
    }
}

void MenuAdapter::connectMenuBar(QMenuBar* menuBar) {
    if (!menuBar) return;

    // Collect all menus (including submenus).
    QList<QMenu*> allMenus;
    for (QAction* menuAction : menuBar->actions()) {
        if (QMenu* menu = menuAction->menu()) {
            allMenus.append(menu);
        }
    }

    // BFS to find all submenus.
    for (int i = 0; i < allMenus.size(); ++i) {
        for (QAction* a : allMenus[i]->actions()) {
            if (QMenu* sub = a->menu()) {
                allMenus.append(sub);
            }
        }
    }

    // Register and connect every action whose data() matches a registered command.
    for (QMenu* menu : allMenus) {
        for (QAction* action : menu->actions()) {
            if (action->data().isNull()) continue;
            QString cmd = action->data().toString();
            if (_commandToId.contains(cmd)) {
                int id = _commandToId[cmd];
                // Register this action under all Win32 IDs that map to this command.
                for (auto it = _idToCommand.begin(); it != _idToCommand.end(); ++it) {
                    if (it.value() == cmd) {
                        _idToAction[it.key()] = action;
                    }
                }

                if (!_connectedActions.contains(action)) {
                    _connectedActions.insert(action);
                    connect(action, &QAction::triggered,
                            this, &MenuAdapter::onActionTriggered);
                }
            }
        }
    }
}

void MenuAdapter::onActionTriggered() {
    auto* action = qobject_cast<QAction*>(sender());
    if (!action) return;
    QString cmd = action->data().toString();
    int id = commandID(cmd);
    if (id >= 0) {
        emit command(id);
        emit commandString(cmd);
    }
}

void MenuAdapter::applyMenuRoles() {
    if (!_menuBar) return;

    static const struct { const char* pattern; QAction::MenuRole role; } roleMap[] = {
        { "file",       QAction::NoRole },
        { "edit",       QAction::TextHeuristicRole },
        { "view",       QAction::ApplicationSpecificRole },
        { "encoding",   QAction::ApplicationSpecificRole },
        { "language",   QAction::ApplicationSpecificRole },
        { "settings",   QAction::PreferencesRole },
        { "preference", QAction::PreferencesRole },
        { "window",     QAction::ApplicationSpecificRole },
        { "help",       QAction::ApplicationSpecificRole },
        { "tools",      QAction::ApplicationSpecificRole },
        { "macro",      QAction::ApplicationSpecificRole },
        { "run",        QAction::ApplicationSpecificRole },
    };
    static constexpr int roleMapCount = sizeof(roleMap) / sizeof(roleMap[0]);

    for (QAction* menuAction : _menuBar->actions()) {
        QMenu* menu = menuAction->menu();
        if (!menu) continue;

        QString titleLower = menu->title().toLower();
        QAction::MenuRole role = QAction::ApplicationSpecificRole;

        for (int i = 0; i < roleMapCount; ++i) {
            if (titleLower.contains(QLatin1String(roleMap[i].pattern))) {
                role = roleMap[i].role;
                break;
            }
        }

        menuAction->setMenuRole(role);

        for (QAction* a : menu->actions()) {
            if (a->isSeparator()) continue;
            QString textLower = a->text().toLower();

            if (titleLower.contains("file")) {
                if (textLower.contains("exit") || textLower.contains("quit")) {
                    a->setMenuRole(QAction::QuitRole);
                }
            } else if (titleLower.contains("help")) {
                if (textLower.contains("about")) {
                    a->setMenuRole(QAction::AboutRole);
                }
            }
        }
    }
}

void MenuAdapter::trigger(int cmdID) {
    if (QAction* action = _idToAction.value(cmdID, nullptr)) {
        action->trigger();
    }
}

void MenuAdapter::trigger(const QString& qtCommand) {
    int id = commandID(qtCommand);
    if (id >= 0) trigger(id);
}

// ---------------------------------------------------------------------------
// ContextMenuAdapter
// ---------------------------------------------------------------------------

ContextMenuAdapter::ContextMenuAdapter(QWidget* parent)
    : QObject(parent), _parent(parent), _menu(nullptr) {}

ContextMenuAdapter::~ContextMenuAdapter() = default;

void ContextMenuAdapter::addItem(int cmdID, const QString& text,
                                 const QString& parentFolder) {
    Item item;
    item.cmdID = cmdID;
    item.text = text;
    item.parentFolder = parentFolder;
    // cmdID==0 marks a separator, mirroring Win32 ContextMenu behavior.
    item.isSeparator = (cmdID == 0);
    _items.append(item);
    _menu = nullptr;
}

void ContextMenuAdapter::addSeparator() {
    Item item;
    item.isSeparator = true;
    _items.append(item);
    _menu = nullptr;
}

void ContextMenuAdapter::clear() {
    _items.clear();
    delete _menu;
    _menu = nullptr;
}

void ContextMenuAdapter::fromItems(const QList<MenuItemUnit>& items) {
    clear();
    for (const auto& item : items) {
        addItem(item.cmdID, item.itemName, item.parentFolderName);
    }
}

QMenu* ContextMenuAdapter::build() {
    if (!_menu) {
        _menu = new QMenu(_parent);
        rebuildMenu(_menu);
    }
    QMenu* result = _menu;
    // Caller takes ownership — null out _menu so the same menu object is not
    // returned again (which would cause double-free).
    _menu = nullptr;
    return result;
}

void ContextMenuAdapter::rebuildMenu(QMenu* menu) {
    if (!menu) return;
    menu->clear();

    QMenu* currentSubmenu = nullptr;
    QString currentFolder;

    for (const Item& item : _items) {
        if (item.isSeparator) {
            menu->addSeparator();
            currentFolder.clear();
            currentSubmenu = nullptr;
            continue;
        }

        // Mirrors Win32 ContextMenu::create():
        // If this item has a parent folder name and it differs from the current
        // folder, create a new submenu. No auto-separator is inserted before
        // the submenu — the caller controls separators explicitly.
        if (!item.parentFolder.isEmpty()) {
            if (item.parentFolder != currentFolder) {
                currentFolder = item.parentFolder;
                currentSubmenu = menu->addMenu(currentFolder);
            }
        } else {
            currentFolder.clear();
            currentSubmenu = nullptr;
        }

        QAction* act = nullptr;
        if (currentSubmenu) {
            act = currentSubmenu->addAction(item.text);
        } else {
            act = menu->addAction(item.text);
        }

        act->setData(item.cmdID);

        int capturedID = item.cmdID;
        connect(act, &QAction::triggered, menu,
            [this, capturedID, act]() {
                emit triggered(capturedID);
                emit triggeredString(act->data().toString());
            });
    }
}

QAction* ContextMenuAdapter::exec(const QPoint& pos) {
    QMenu* menu = build();
    return menu->exec(pos.isNull() ? QCursor::pos() : pos);
}
