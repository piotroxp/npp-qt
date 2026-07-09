// EditorCommandManager.h - Manages editor commands/keybindings
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class ScintillaEditor;

class EditorCommandManager : public NonCopyable {
public:
    EditorCommandManager();
    ~EditorCommandManager();

    void execute(ScintillaEditor* editor, int commandId);
    void execute(ScintillaEditor* editor, const std::string& commandName);
    void execute(int commandId);
    void execute(const std::string& commandName);

    void registerCommand(int commandId, const std::string& name,
                         std::function<void(ScintillaEditor*)> action);
    void unregisterCommand(int commandId);

    bool isRegistered(int commandId) const;
    std::string getCommandName(int commandId) const;

    void bindKey(int commandId, const std::string& keySequence);
    void unbindKey(const std::string& keySequence);
    int resolveKeyBinding(const std::string& keySequence) const;

    std::vector<int> getAllCommandIds() const;

private:
    struct CommandInfo {
        std::string name;
        std::function<void(ScintillaEditor*)> action;
    };
    std::unordered_map<int, CommandInfo> _commands;
    std::unordered_map<std::string, int> _keyBindings;
};
