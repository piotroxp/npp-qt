// PluginsManager.h - Qt port
#pragma once
#include <QMap>
#include <QSignalMapper>
#include <QMenu>
#include <memory>
#include <map>
#include <string>
#include "PluginInterface.h"
#include "ScintillaEditView.h"

struct PluginCommand { int _cmdID = 0; PFUNCPLUGINCMD _pFunc = nullptr; const wchar_t* _pluginName = nullptr; ShortcutKey* _pShortcut = nullptr; bool _wasChecked = false; };
struct ShortcutKeyAndCmdID { int _key = 0; int _cmd = 0; const wchar_t* _pluginName = nullptr; };

struct DynamicIDAlloc {
    int _nextID = 1000;
    bool allocate(int numberRequired, int* start) {
        *start = _nextID;
        _nextID += numberRequired;
        return true;
    }
};

// Renamed to avoid conflict with pluginsAdmin.h's PluginInfo
struct PluginInterfaceInfo {
    QString _funcName;
    QString _moduleName;
    QString _modulePath;
    PFUNCSETINFO _pFuncSetInfo = nullptr;
    PFUNCGETNAME _pFuncGetName = nullptr;
    PFUNCGETFUNCSARRAY _pFuncGetFuncsArray = nullptr;
    PBENOTIFIED _pBeNotified = nullptr;
    PMESSAGEPROC _pMessageProc = nullptr;
    PFUNCISUNICODE _pFuncIsUnicode = nullptr;
    FuncItem* _funcItems = nullptr;
    int _nbFuncItem = 0;
};

class PluginsManager {
public:
    static PluginsManager& getInstance() { static PluginsManager pm; return pm; }
    
    void initHMenus(intptr_t pfn);
    int loadPluginFromPath(const QString& pluginFilePath);
    bool loadPlugins(const QString& dir, const void* pluginList, void* outList);
    
    void runPluginCommand(size_t i);
    void runPluginCommand(const QString& pluginName, int commandID);
    void runFuncCommand(const wchar_t* funcName, ScintillaEditView** ppEditView);
    
    void* initMenu(void* hMenu, bool);
    void addInMenuFromPMIndex(int i);
    
    bool allocateCmdID(int numberRequired, int* start);
    DynamicIDAlloc _markerAlloc;
    DynamicIDAlloc _indicatorAlloc;
    bool allocateMarker(int numberRequired, int* start);
    bool allocateIndicator(int numberRequired, int* start);
    
    void notify(size_t eventType, const void* notification);
    void notify(const void* notification);
    void relayNppMessages(unsigned int msg, size_t wParam, long lParam);
    bool relayPluginMessages(unsigned int msg, size_t wParam, long lParam);
    
    bool getShortcutByCmdID(int, ShortcutKey*);
    bool removeShortcutByCmdID(int);
    long long comunicChecker(size_t, long long) const;
    
    QString getLoadedPluginNames() const;
    int getCurrentScratchTBVisible() const { return _isCurrentScratchVisible; }
    void setCurrentScratchTBVisible() { _isCurrentScratchVisible = false; }
    
    void addInLoadedDlls(const QString& path, const QString& moduleName) { Q_UNUSED(path); Q_UNUSED(moduleName); }
    
private:
    std::vector<std::unique_ptr<PluginInterfaceInfo>> _pluginInfos;
    std::map<int, std::wstring> _shortcuts;
    int _nbShortcuts = 0;
    intptr_t _pfnHMENU = 0;
    bool _isCurrentScratchVisible = false;
    DynamicIDAlloc _dynamicIDAlloc;
    NppData _nppData{};
};

#define NB_MAX_UPDATES 10