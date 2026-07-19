// NppBigSwitch.h — Command ID dispatch table for Notepad--Qt
// Maps integer menu command IDs to Application handler method calls
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <functional>

// Use canonical Notepad++ command IDs from menuCmdID.h
// (all constants are constexpr int, no preprocessor macros)
#include "menuCmdID.h"

class Application;

class NppCommands;

// QMap<int, std::function<void()>> dispatch table built once at startup.
// Commands registered by category: File, Edit, Search, View, Encoding, EOL, Macro, Run.
class NppBigSwitch : public QObject {
    Q_OBJECT
public:
    explicit NppBigSwitch(Application* app, QObject* parent = nullptr);
    ~NppBigSwitch() = default;

    // Populates m_dispatch with all registered commands. Called once after
    // Application and all panels/views are initialized.
    void init();

    // Dispatches a command by its integer ID. Returns true if the command
    // was found and executed; false otherwise.
    bool dispatch(int commandId);

    // Returns true if the given ID has a registered handler.
    bool hasCommand(int commandId) const;

    // Returns a list of all registered command IDs.
    QList<int> registeredCommands() const;

    // Human-readable name for a command, for menus and status bar.
    QString commandName(int id) const;

    // Look up a command name by ID — uses the names table.
    static const char* idToName(int id);

signals:
    // Emitted after a command is successfully dispatched.
    void commandExecuted(int commandId);

    // Emitted when dispatch() is called for an unregistered ID.
    void commandNotFound(int commandId);

private:
    // Registers a handler for a command ID.
    void reg(int id, std::function<void()> handler);

    Application* m_app = nullptr;
    QMap<int, std::function<void()>> m_dispatch;
    QMap<int, QString> m_names;
};
