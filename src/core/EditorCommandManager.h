// EditorCommandManager.h - Manages editor commands/keybindings
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class ScintillaEditor;
class Application;

enum EditorCommandId : int {
    // File
    CMD_FILE_NEW = 1001,
    CMD_FILE_OPEN,
    CMD_FILE_SAVE,
    CMD_FILE_SAVE_AS,
    CMD_FILE_SAVE_ALL,
    CMD_FILE_CLOSE,
    CMD_FILE_CLOSE_ALL,
    CMD_FILE_EXIT,
    CMD_FILE_CLEAR_RECENT,

    // Edit
    CMD_EDIT_UNDO = 1101,
    CMD_EDIT_REDO,
    CMD_EDIT_CUT,
    CMD_EDIT_COPY,
    CMD_EDIT_PASTE,
    CMD_EDIT_DELETE,
    CMD_EDIT_SELECT_ALL,
    CMD_EDIT_FIND,
    CMD_EDIT_REPLACE,
    CMD_EDIT_GOTO,

    // Search
    CMD_SEARCH_FIND_NEXT = 1201,
    CMD_SEARCH_FIND_PREV,
    CMD_SEARCH_FIND_IN_FILES,
    CMD_SEARCH_COUNT,
    CMD_SEARCH_MARK_ALL,

    // View
    CMD_VIEW_FULL_SCREEN = 1301,
    CMD_VIEW_DISTRACTION_FREE,
    CMD_VIEW_SHOW_TAB_BAR,
    CMD_VIEW_SHOW_STATUS_BAR,
    CMD_VIEW_SHOW_TOOL_BAR,

    // Encoding
    CMD_ENCODING_UTF8 = 1401,
    CMD_ENCODING_UTF8_BOM,
    CMD_ENCODING_UTF16_LE,
    CMD_ENCODING_UTF16_BE,

    // Language
    CMD_LANGUAGE_NORMAL = 1501,

    // Settings
    CMD_SETTINGS_PREFS = 1601,
    CMD_SETTINGS_SHORTCUT_MAPPER,
    CMD_SETTINGS_COMMAND_PALETTE,

    // Help
    CMD_HELP_ABOUT = 1701,
};

class EditorCommandManager : public NonCopyable {
public:
    EditorCommandManager();
    ~EditorCommandManager();

    // Execute by string name (primary dispatch path from menus)
    void execute(const std::string& commandName);
    // Execute by numeric ID
    void execute(int commandId);
    // Execute on specific editor
    void execute(ScintillaEditor* editor, int commandId);
    void execute(ScintillaEditor* editor, const std::string& commandName);

    // Register commands — call once at startup
    void registerCommand(int commandId, const std::string& name,
                         std::function<void(ScintillaEditor*)> action);

    // Register all menu commands at once — call once from Application::initialize()
    void registerAll(Application* app);

    void unregisterCommand(int commandId);
    bool isRegistered(int commandId) const;
    std::string getCommandName(int commandId) const;

    // Key bindings
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
    std::unordered_map<std::string, int> _nameToId;
    std::unordered_map<std::string, int> _keyBindings;
};
