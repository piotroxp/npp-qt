// EditorCommandManager.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "EditorCommandManager.h"
#include "../editor/ScintillaEditor.h"

EditorCommandManager::EditorCommandManager() = default;
EditorCommandManager::~EditorCommandManager() = default;

void EditorCommandManager::execute(ScintillaEditor* editor, int commandId) {
    auto it = _commands.find(commandId);
    if (it != _commands.end() && it->second.action) {
        it->second.action(editor);
    }
}

void EditorCommandManager::execute(ScintillaEditor* editor, const std::string& commandName) {
    for (auto& [id, info] : _commands) {
        if (info.name == commandName) {
            execute(editor, id);
            return;
        }
    }
}

void EditorCommandManager::execute(int commandId) {
    execute(nullptr, commandId);
}

void EditorCommandManager::execute(const std::string& commandName) {
    execute(nullptr, commandName);
}

void EditorCommandManager::registerCommand(int commandId, const std::string& name,
                                            std::function<void(ScintillaEditor*)> action) {
    _commands[commandId] = {name, std::move(action)};
}

void EditorCommandManager::unregisterCommand(int commandId) {
    _commands.erase(commandId);
}

bool EditorCommandManager::isRegistered(int commandId) const {
    return _commands.find(commandId) != _commands.end();
}

std::string EditorCommandManager::getCommandName(int commandId) const {
    auto it = _commands.find(commandId);
    return it != _commands.end() ? it->second.name : "";
}

void EditorCommandManager::bindKey(int commandId, const std::string& keySequence) {
    _keyBindings[keySequence] = commandId;
}

void EditorCommandManager::unbindKey(const std::string& keySequence) {
    _keyBindings.erase(keySequence);
}

int EditorCommandManager::resolveKeyBinding(const std::string& keySequence) const {
    auto it = _keyBindings.find(keySequence);
    return it != _keyBindings.end() ? it->second : -1;
}

std::vector<int> EditorCommandManager::getAllCommandIds() const {
    std::vector<int> ids;
    ids.reserve(_commands.size());
    for (auto& [id, _] : _commands) ids.push_back(id);
    return ids;
}
