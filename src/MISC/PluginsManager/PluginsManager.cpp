// PluginsManager.cpp - Qt6 port of Notepad++ plugin manager
#include "PluginsManager.h"
#include <QLibrary>
#include <QDir>
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>

bool PluginsManager::loadPlugins(const QString& dir, const PluginInterfaceInfo*, PluginInterfaceInfo*)
{
    if (dir.isEmpty()) {
        return false;
    }

    QDir pluginsDir(dir);
    if (!pluginsDir.exists())
        return false;

    QStringList pluginFiles = pluginsDir.entryList(QStringList() << "*.so", QDir::Files);
    
    for (const QString& pluginFile : pluginFiles) {
        QString fullPath = pluginsDir.absoluteFilePath(pluginFile);
        loadPluginFromPath(fullPath);
    }

    return true;
}

int PluginsManager::loadPluginFromPath(const QString& pluginFilePath)
{
    QLibrary lib(pluginFilePath);
    if (!lib.load()) {
        return -1;
    }

    auto* pluginInfo = new PluginInterfaceInfo();
    
    // Resolve plugin functions
    pluginInfo->_pFuncSetInfo = reinterpret_cast<PFUNCSETINFO>(
        lib.resolve("setInfo"));
    pluginInfo->_pFuncGetName = reinterpret_cast<PFUNCGETNAME>(
        lib.resolve("getName"));
    pluginInfo->_pFuncGetFuncsArray = reinterpret_cast<PFUNCGETFUNCSARRAY>(
        lib.resolve("getFuncsArray"));
    pluginInfo->_pBeNotified = reinterpret_cast<PBENOTIFIED>(
        lib.resolve("beNotified"));
    pluginInfo->_pMessageProc = reinterpret_cast<PMESSAGEPROC>(
        lib.resolve("messageProc"));
    pluginInfo->_pFuncIsUnicode = reinterpret_cast<PFUNCISUNICODE>(
        lib.resolve("isUnicode"));

    if (pluginInfo->_pFuncSetInfo && pluginInfo->_pFuncGetFuncsArray) {
        pluginInfo->_pFuncSetInfo(&_nppData);
        
        int numFuncs = 0;
        pluginInfo->_funcItems = pluginInfo->_pFuncGetFuncsArray(&numFuncs);
        pluginInfo->_nbFuncItem = numFuncs;

        if (pluginInfo->_pFuncGetName) {
            pluginInfo->_funcName = QString::fromWCharArray(pluginInfo->_pFuncGetName());
        }

        QFileInfo fi(pluginFilePath);
        pluginInfo->_moduleName = fi.fileName();
        
        _pluginInfos.emplace_back(pluginInfo);
        addInLoadedDlls(pluginFilePath, fi.fileName());
        
        return static_cast<int>(_pluginInfos.size() - 1);
    }

    delete pluginInfo;
    return -1;
}

void PluginsManager::runPluginCommand(size_t i)
{
    if (i >= _pluginInfos.size()) return;
    
    PluginInterfaceInfo* info = _pluginInfos[i].get();
    if (info && info->_funcItems) {
        for (int j = 0; j < info->_nbFuncItem; ++j) {
            if (info->_funcItems[j]._pFunc) {
                info->_funcItems[j]._pFunc();
            }
        }
    }
}

void PluginsManager::runPluginCommand(const QString& pluginName, int commandID)
{
    Q_UNUSED(pluginName);
    Q_UNUSED(commandID);
}

void PluginsManager::addInMenuFromPMIndex(int i)
{
    if (i < 0 || i >= static_cast<int>(_pluginInfos.size())) return;
}

void* PluginsManager::initMenu(void* hMenu, bool)
{
    QMenu* menu = static_cast<QMenu*>(hMenu);
    if (!menu) {
        menu = new QMenu("Plugins");
    }
    
    for (const auto& info : _pluginInfos) {
        QMenu* pluginMenu = menu->addMenu(info->_funcName);
        
        for (int i = 0; i < info->_nbFuncItem; ++i) {
            if (info->_funcItems[i]._pFunc) {
                QString name = QString::fromWCharArray(info->_funcItems[i]._itemName);
                pluginMenu->addAction(name);
            }
        }
    }
    
    return menu;
}

bool PluginsManager::allocateCmdID(int numberRequired, int* start)
{
    return _dynamicIDAlloc.allocate(numberRequired, start);
}

bool PluginsManager::allocateMarker(int numberRequired, int* start)
{
    return _markerAlloc.allocate(numberRequired, start);
}

bool PluginsManager::allocateIndicator(int numberRequired, int* start)
{
    return _indicatorAlloc.allocate(numberRequired, start);
}

QString PluginsManager::getLoadedPluginNames() const
{
    QStringList names;
    for (const auto& info : _pluginInfos) {
        names.append(info->_funcName);
    }
    return names.join(", ");
}

bool PluginsManager::getShortcutByCmdID(int, ShortcutKey*) { return false; }
bool PluginsManager::removeShortcutByCmdID(int) { return false; }
long long PluginsManager::comunicChecker(size_t, long long) const { return 0; }

void PluginsManager::notify(size_t, const void* notification)
{
    Q_UNUSED(notification);
}

void PluginsManager::notify(const void* notification)
{
    Q_UNUSED(notification);
}

void PluginsManager::relayNppMessages(unsigned int, unsigned long, long)
{
}

bool PluginsManager::relayPluginMessages(unsigned int, unsigned long, long)
{
    return false;
}