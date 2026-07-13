// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Ported to Qt6

#pragma once

#include <QtCore>
#include <QString>
#include <QVector>
#include <QPluginLoader>
#include <QMenu>
#include <QPointer>

// Use NppSciCompat instead of raw Scintilla.h to avoid macro conflicts with Qsci
#include "NppSciCompat.h"
// SCNotification is defined in Scintilla.h (via NppNotification.h)
#include <Qsci/qsciscintilla.h>
// Provides SCNotification struct from Scintilla
#include "NppNotification.h"

#include "menuCmdID.h"
#include "MISC/Common/IDAllocator.h"
#include "Notepad_plus_msgs.h"
#include "Lexilla.h"
#include "ILexer.h"

// Full definition (NppData struct is defined in PluginInterface.h)
#include "PluginInterface.h"

// Forward declarations for types not needed as full definitions
struct PluginUpdateInfo;

struct PluginCommand
{
    QString _pluginName;
    int _funcID = 0;
    // PFUNCPLUGINCMD — plugin function pointer type
    PFUNCPLUGINCMD _pFunc = nullptr;

    PluginCommand(const QString& pluginName, int funcID, PFUNCPLUGINCMD pFunc)
        : _pluginName(pluginName), _funcID(funcID), _pFunc(pFunc) {}

    bool operator==(const PluginCommand& other) const
    {
        return _pluginName == other._pluginName
            && _funcID == other._funcID
            && _pFunc == other._pFunc;
    }
};

struct PluginInfo
{
    PluginInfo() = default;
    ~PluginInfo()
    {
        if (_pluginLoader)
        {
            _pluginLoader->unload();
            delete _pluginLoader;
        }
    }

    PluginInfo(const PluginInfo&) = delete;
    PluginInfo& operator=(const PluginInfo&) = delete;

    QLibrary* _pluginLoader = nullptr;
    QString _moduleName;
    QString _displayName;

    // setInfo function
    PFUNCSETINFO _pFuncSetInfo = nullptr;
    // getName function
    PFUNCGETNAME _pFuncGetName = nullptr;
    // beNotified function
    PBENOTIFIED _pBeNotified = nullptr;
    // getFuncsArray function
    PFUNCGETFUNCSARRAY _pFuncGetFuncsArray = nullptr;
    // messageProc function
    PMESSAGEPROC _pMessageProc = nullptr;
    // isUnicode function
    PFUNCISUNICODE _pFuncIsUnicode = nullptr;

    FuncItem* _funcItems = nullptr;
    int _nbFuncItem = 0;

    QString _funcName;
};

struct LoadedDllInfo
{
    QString _fullFilePath;
    QString _fileName;
    QString _displayName;

    LoadedDllInfo(const QString& fullFilePath, const QString& fileName)
        : _fullFilePath(fullFilePath), _fileName(fileName)
    {
        int dot = fileName.lastIndexOf('.');
        _displayName = (dot > 0) ? fileName.left(dot) : fileName;
    }
};

// Plugin command ID range (mirrored from Win32 resource.h)
inline constexpr int ID_PLUGINS_CMD          = 22000;
inline constexpr int ID_PLUGINS_CMD_LIMIT    = 22999;
inline constexpr int ID_PLUGINS_CMD_DYNAMIC   = 23000;
inline constexpr int ID_PLUGINS_CMD_DYNAMIC_LIMIT = 24999;

// Plugin marker/indicator ranges
inline constexpr int MARKER_PLUGINS_LIMIT    = 24 + 15;   // MARKER_PLUGINS (24) + 15 slots
inline constexpr int INDICATOR_PLUGINS_LIMIT  = 30 + 20;   // INDICATOR_PLUGINS (30) + 20 slots

class PluginsManager
{
    friend class PluginsAdminDlg;
public:
    PluginsManager();
    ~PluginsManager();

    void init(const NppData& nppData);

    bool loadPlugins(const QString& dir = QString(), const std::vector<PluginUpdateInfo*>* pluginUpdateInfoList = nullptr,
                     std::vector<PluginUpdateInfo*>* pluginIncompatibleList = nullptr);

    void runPluginCommand(size_t i);
    void runPluginCommand(const QString& pluginName, int commandID);

    void addInMenuFromPMIndex(int i);
    QMenu* initMenu(QMenu* hMenu, bool enablePluginAdmin = false);
    static bool getShortcutByCmdID(int cmdID, ShortcutKey* sk);
    static bool removeShortcutByCmdID(int cmdID);

    void notify(size_t indexPluginInfo, const SCNotification* notification);
    void notify(const SCNotification* notification);
    void relayNppMessages(unsigned int message, uintptr_t wParam, intptr_t lParam);
    bool relayPluginMessages(unsigned int message, uintptr_t wParam, intptr_t lParam);

    QMenu* getMenuHandle() const { return _hPluginsMenu; }

    void disable() { _isDisabled = true; }
    bool hasPlugins() const { return !_pluginInfos.empty(); }

    bool allocateCmdID(int numberRequired, int* start);
    bool inDynamicRange(int id) const;

    bool allocateMarker(int numberRequired, int* start);
    bool allocateIndicator(int numberRequired, int* start);
    QString getLoadedPluginNames() const;

private:
    NppData* _nppData = nullptr;
    QPointer<QMenu> _hPluginsMenu;

    QVector<QSharedPointer<PluginInfo>> _pluginInfos;
    QVector<PluginCommand> _pluginsCommands;
    QVector<LoadedDllInfo> _loadedDlls;
    bool _isDisabled = false;

    // ID allocation for plugin commands
    IDAllocator _dynamicIDAlloc;
    bool _noMoreNotification = false;

    int loadPluginFromPath(const QString& pluginFilePath);
    void loadLexillaLexersFromPlugin(QLibrary* lib, const QString& moduleName);

    void pluginCrashAlert(const QString& pluginName, const QString& funcSignature);
    void pluginExceptionAlert(const QString& pluginName, const QString& reason);

    bool isInLoadedDlls(const QString& fn) const;
    void addInLoadedDlls(const QString& fullPath, const QString& fn);
};

// Plugin interface IDs (mirrored from PluginInterface.h)
#define ER_PLUGINSVIEW_UPDATED 0
#define ER_DOCKABLESUBCLASSED 1
