// EditorCommandManager.cpp - Manages editor commands/keybindings
// Copyright (C) 2026 Agent Army
// GPL v3

#include "EditorCommandManager.h"
#include "Application.h"
#include "../editor/ScintillaEditor.h"
#include <cassert>

EditorCommandManager* EditorCommandManager::_singleton = nullptr;

EditorCommandManager& EditorCommandManager::instance() {
    if (!_singleton) _singleton = new EditorCommandManager();
    return *_singleton;
}

EditorCommandManager::EditorCommandManager() = default;
EditorCommandManager::~EditorCommandManager() = default;

void EditorCommandManager::execute(int commandId) {
    execute(nullptr, commandId);
}

void EditorCommandManager::execute(const std::string& commandName) {
    auto it = _nameToId.find(commandName);
    if (it != _nameToId.end()) {
        execute(it->second);
    }
    // else: unknown command — silently ignore (matches Notepad++ behavior)
}

void EditorCommandManager::execute(ScintillaEditor* editor, int commandId) {
    auto it = _commands.find(commandId);
    if (it != _commands.end() && it->second.action) {
        it->second.action(editor);
    }
}

void EditorCommandManager::execute(ScintillaEditor* editor, const std::string& commandName) {
    auto it = _nameToId.find(commandName);
    if (it != _nameToId.end()) {
        execute(editor, it->second);
    }
}

void EditorCommandManager::registerCommand(int commandId, const std::string& name,
                                            std::function<void(ScintillaEditor*)> action) {
    _commands[commandId] = {name, std::move(action)};
    _nameToId[name] = commandId;
}

void EditorCommandManager::unregisterCommand(int commandId) {
    auto it = _commands.find(commandId);
    if (it != _commands.end()) {
        _nameToId.erase(it->second.name);
        _commands.erase(it);
    }
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
    // Also register with ShortcutManager for conflict detection and persistence
    ShortcutManager::instance().bindKey(commandId, QString::fromStdString(keySequence));
}

void EditorCommandManager::unbindKey(const std::string& keySequence) {
    _keyBindings.erase(keySequence);
    ShortcutManager::instance().unbindKey(QString::fromStdString(keySequence));
}

int EditorCommandManager::resolveKeyBinding(const std::string& keySequence) const {
    auto it = _keyBindings.find(keySequence);
    return it != _keyBindings.end() ? it->second : -1;
}

std::vector<int> EditorCommandManager::getAllCommandIds() const {
    std::vector<int> ids;
    ids.reserve(_commands.size());
    for (const auto& [id, _] : _commands) ids.push_back(id);
    return ids;
}

void EditorCommandManager::loadShortcuts(const QString& path) {
    ShortcutManager::instance().loadFromJson(path);
}

void EditorCommandManager::saveShortcuts(const QString& path) const {
    ShortcutManager::instance().saveToJson(path);
}

QVector<KeyBinding> EditorCommandManager::getAllBindings() const {
    return ShortcutManager::instance().allBindings();
}

QVector<ShortcutManager::Conflict> EditorCommandManager::getConflicts() const {
    return ShortcutManager::instance().getConflicts();
}

QVector<EditorCommandManager::CommandEntry> EditorCommandManager::getAllCommands() const {
    QVector<CommandEntry> entries;
    entries.reserve(_commands.size());
    for (const auto& [id, info] : _commands) {
        entries.append({id, info.name});
    }
    return entries;
}

// ============================================================================
// registerAll — wire up all menu commands with Application actions
// Call this once from Application::initialize()
// ============================================================================
void EditorCommandManager::registerAll(Application* app) {
    assert(app);

    // ---- File commands ----
    registerCommand(CMD_FILE_NEW, "file.new",
        [app](ScintillaEditor*) { app->onNewFile(); });

    registerCommand(CMD_FILE_OPEN, "file.open",
        [app](ScintillaEditor*) { app->onOpenFile(); });

    registerCommand(CMD_FILE_SAVE, "file.save",
        [app](ScintillaEditor*) { app->onSaveFile(); });

    registerCommand(CMD_FILE_SAVE_AS, "file.saveAs",
        [app](ScintillaEditor*) { app->onSaveFileAs(); });

    registerCommand(CMD_FILE_SAVE_ALL, "file.saveAll",
        [app](ScintillaEditor*) { app->onSaveAll(); });

    registerCommand(CMD_FILE_CLOSE, "file.close",
        [app](ScintillaEditor*) { app->onCloseFile(); });

    registerCommand(CMD_FILE_CLOSE_ALL, "file.closeAll",
        [app](ScintillaEditor*) { app->onCloseAll(); });

    registerCommand(CMD_FILE_EXIT, "file.exit",
        [app](ScintillaEditor*) { app->onExit(); });

    registerCommand(CMD_FILE_CLEAR_RECENT, "file.clearRecent",
        [app](ScintillaEditor*) { app->onClearRecentFiles(); });

    // ---- Edit commands ----
    registerCommand(CMD_EDIT_UNDO, "edit.undo",
        [app](ScintillaEditor*) { app->onUndo(); });

    registerCommand(CMD_EDIT_REDO, "edit.redo",
        [app](ScintillaEditor*) { app->onRedo(); });

    registerCommand(CMD_EDIT_CUT, "edit.cut",
        [app](ScintillaEditor*) { app->onCut(); });

    registerCommand(CMD_EDIT_COPY, "edit.copy",
        [app](ScintillaEditor*) { app->onCopy(); });

    registerCommand(CMD_EDIT_PASTE, "edit.paste",
        [app](ScintillaEditor*) { app->onPaste(); });

    registerCommand(CMD_EDIT_DELETE, "edit.delete",
        [app](ScintillaEditor*) { app->onDelete(); });

    registerCommand(CMD_EDIT_SELECT_ALL, "edit.selectAll",
        [app](ScintillaEditor*) { app->onSelectAll(); });

    registerCommand(CMD_EDIT_FIND, "edit.find",
        [app](ScintillaEditor*) { app->onFind(); });

    registerCommand(CMD_EDIT_REPLACE, "edit.replace",
        [app](ScintillaEditor*) { app->onReplace(); });

    registerCommand(CMD_EDIT_GOTO, "edit.goto",
        [app](ScintillaEditor*) { app->onGotoLine(); });

    // ---- Search commands ----
    registerCommand(CMD_SEARCH_FIND_NEXT, "search.findNext",
        [app](ScintillaEditor*) { app->onFindNext(); });

    registerCommand(CMD_SEARCH_FIND_PREV, "search.findPrev",
        [app](ScintillaEditor*) { app->onFindPrev(); });

    registerCommand(CMD_SEARCH_FIND_IN_FILES, "search.findInFiles",
        [app](ScintillaEditor*) { app->onFindInFiles(); });

    registerCommand(CMD_SEARCH_COUNT, "search.count",
        [app](ScintillaEditor*) { app->onCount(); });

    registerCommand(CMD_SEARCH_MARK_ALL, "search.markAll",
        [app](ScintillaEditor*) { app->onMarkAll(); });

    // ---- View commands ----
    registerCommand(CMD_VIEW_FULL_SCREEN, "view.fullScreen",
        [app](ScintillaEditor*) { app->onToggleFullScreen(); });

    registerCommand(CMD_VIEW_DISTRACTION_FREE, "view.distractionFree",
        [app](ScintillaEditor*) { app->onToggleDistractionFree(); });

    registerCommand(CMD_VIEW_SHOW_TAB_BAR, "view.showTabBar",
        [app](ScintillaEditor*) { app->onToggleTabBar(); });

    registerCommand(CMD_VIEW_SHOW_STATUS_BAR, "view.showStatusBar",
        [app](ScintillaEditor*) { app->onToggleStatusBar(); });

    registerCommand(CMD_VIEW_SHOW_TOOL_BAR, "view.showToolBar",
        [app](ScintillaEditor*) { app->onToggleToolBar(); });

    // ---- Encoding commands ----
    registerCommand(CMD_ENCODING_UTF8, "encoding.utf8",
        [app](ScintillaEditor*) { app->onConvertEncoding(EncodingType::UTF_8); });

    registerCommand(CMD_ENCODING_UTF8_BOM, "encoding.utf8bom",
        [app](ScintillaEditor*) { app->onConvertEncoding(EncodingType::UTF_8_BOM); });

    registerCommand(CMD_ENCODING_UTF16_LE, "encoding.utf16le",
        [app](ScintillaEditor*) { app->onConvertEncoding(EncodingType::UTF_16_LE); });

    registerCommand(CMD_ENCODING_UTF16_BE, "encoding.utf16be",
        [app](ScintillaEditor*) { app->onConvertEncoding(EncodingType::UTF_16_BE); });

    // ---- Language commands ----
    registerCommand(CMD_LANGUAGE_NORMAL, "language.normal_text",
        [app](ScintillaEditor*) { app->onSetLanguage(LangType::L_TEXT); });

    // ---- Settings commands ----
    registerCommand(CMD_SETTINGS_PREFS, "settings.preferences",
        [app](ScintillaEditor*) { app->onShowPreferences(); });

    registerCommand(CMD_SETTINGS_SHORTCUT_MAPPER, "settings.shortcutMapper",
        [app](ScintillaEditor*) { app->onShowShortcutMapper(); });

    registerCommand(CMD_SETTINGS_COMMAND_PALETTE, "settings.commandPalette",
        [app](ScintillaEditor*) { app->onShowCommandPalette(); });

    // ---- Help commands ----
    registerCommand(CMD_HELP_ABOUT, "help.about",
        [app](ScintillaEditor*) { app->onShowAbout(); });
}

// ---------------------------------------------------------------------------
// Shortcut read/write
// ---------------------------------------------------------------------------
QString EditorCommandManager::getShortcut(const QString& name) const {
    auto it = _shortcuts.find(name.toStdString());
    if (it != _shortcuts.end())
        return it->second;
    return QString();
}

void EditorCommandManager::setShortcut(const QString& name, const QString& shortcut) {
    _shortcuts[name.toStdString()] = shortcut;
}
