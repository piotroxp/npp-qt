// PluginsManager.h - Qt port
#pragma once
#include <QMap>
#include <QSignalMapper>
#include "PluginInterface.h"
#include "NppNotepad_plus.h"
#include "ScintillaEditView.h"
struct PluginCommand { int _cmdID = 0; PFUNCPLUGINCMD _pFunc = nullptr; const wchar_t* _pluginName = nullptr; ShortcutKey* _pShortcut = nullptr; bool _wasChecked = false; };
struct ShortcutKeyAndCmdID { int _key = 0; int _cmd = 0; const wchar_t* _pluginName = nullptr; };
class PluginsManager {
public:
    static PluginsManager& getInstance() { static PluginsManager pm; return pm; }
    void initHMenus(HMENUFunctions pfn);
    void loadPlugins(const wchar_t* pluginsDirectoryPath, int nbTotalPlugins);
    void loadPlugin(const wchar_t* pluginModuleName, bool isEnabled = true, bool orderByName = true);
    void unloadPlugin(const wchar_t* pluginModuleName, int unloadFlags = 0, HMENUFunctions pfnHMENU = 0);
    void disablePlugin(const wchar_t* pluginModuleName, bool doDisable);
    std::vector<PluginCommand> getPluginCommands() const { return _pluginsCommands; }
    std::vector<PluginCommand> getPluginCommadsByShortcutKey(size_t ScintillaHandle, int shortcutKey);
    void runPluginCommand(const wchar_t* pluginModuleName);
    void runFuncCommand(size_t cmdID, ScintillaEditView** ppEditView);
    void notify(long long bufferID, int message) const;
    long long comunicChecker(size_t wParam, long long lParam) const;
    void setPluginType(const wchar_t* pluginModuleName, bool legacyDll);
    bool isLegacyPlugin(const wchar_t* pluginModuleName);
    bool unregisterShortkey(int key, int cmd);
    void registerShortkey(int key, int cmd, const wchar_t* pluginName);
    bool allocateMarker(HMENUFunctions pFnAllocMarkers);
    bool allocateCmdId(HMENUFunctions pFnAllocCmdIds);
    int getCurrentScratchTBVisible() const { return _isCurrentScratchVisible; }
    void setCurrentScratchTBVisible() { _isCurrentScratchVisible = false; }
private:
    PluginsManager() = default;
    std::vector<PluginDLLHandle> _vecPluginDLLHandles;
    std::map<std::wstring, Path> _pluginPaths;
    std::vector<PluginCommand> _pluginsCommands;
    std::map<int, std::wstring> _shortcuts;
    int _nbShortcuts = 0;
    HMENUFunctions _pfnHMENU = nullptr;
    bool _isCurrentScratchVisible = false;
};
#define NB_MAX_UPDATES 10
