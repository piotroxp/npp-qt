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

#include "PluginsManager.h"
#include "WinControls/PluginsAdmin.h"
#include "NppConstants.h"
#include "Parameters.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QPluginLoader>
#include <QMessageBox>
#include <QLibrary>

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

// Resolve plugin function pointers from a loaded library using QLibrary
static void* resolvePluginSymbol(QLibrary* lib, const char* sym, bool required = true)
{
    // QLibrary::resolve() returns QFunctionPointer (a function pointer)
    // Cast via QFunctionPointer->void* to avoid type mismatch
    QFunctionPointer fptr = lib->resolve(sym);
    void* ptr = reinterpret_cast<void*>(fptr);
    if (!ptr && required) {
        qWarning() << "Plugin is missing required symbol:" << sym;
    }
    return ptr;
}

PluginsManager::PluginsManager()
    : _dynamicIDAlloc(ID_PLUGINS_CMD_DYNAMIC, ID_PLUGINS_CMD_DYNAMIC_LIMIT)
{
}

PluginsManager::~PluginsManager()
{
    // Unload all plugins
    for (auto& info : _pluginInfos) {
        if (info && info->_pluginLoader) {
            info->_pluginLoader->unload();
            delete info->_pluginLoader;
        }
    }
    _pluginInfos.clear();
}

void PluginsManager::init(const NppData& nppData)
{
    _nppData = const_cast<NppData*>(&nppData);
}

bool PluginsManager::loadPlugins(const QString& dir, const std::vector<PluginUpdateInfo*>*,
                                 std::vector<PluginUpdateInfo*>*)
{
    if (_isDisabled)
        return false;

    QString pluginsFolder;
    if (dir.isEmpty()) {
        QString appDir = QCoreApplication::applicationDirPath();
        pluginsFolder = appDir + QStringLiteral("/plugins");
    } else {
        pluginsFolder = dir;
    }

    if (!QDir(pluginsFolder).exists())
        return true; // No plugin dir — not an error

    // Scan subdirectories for plugin DLLs/SOs
    QDirIterator it(pluginsFolder, QDir::Dirs | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        QString subDir = it.next();
        QString baseName = QFileInfo(subDir).fileName();
#ifdef Q_OS_WIN
        QString pluginFile = subDir + QStringLiteral("/") + baseName + QStringLiteral(".dll");
#else
        QString pluginFile = subDir + QStringLiteral("/") + baseName + QStringLiteral(".so");
#endif
        if (QFileInfo::exists(pluginFile)) {
            loadPluginFromPath(pluginFile);
        }
    }

    // Also scan the root plugins folder for flat plugins
    QStringList nameFilters;
#ifdef Q_OS_WIN
    nameFilters << QStringLiteral("*.dll");
#elif defined(Q_OS_MAC)
    nameFilters << QStringLiteral("*.dylib");
#else
    nameFilters << QStringLiteral("*.so");
#endif
    const QFileInfoList files = QDir(pluginsFolder).entryInfoList(nameFilters, QDir::Files);
    for (const QFileInfo& fi : files) {
        loadPluginFromPath(fi.absoluteFilePath());
    }

    return true;
}

int PluginsManager::loadPluginFromPath(const QString& pluginFilePath)
{
    if (pluginFilePath.isEmpty())
        return -1;

    QString fileName = QFileInfo(pluginFilePath).fileName();

    if (isInLoadedDlls(fileName))
        return -1;

    auto lib = new QLibrary(pluginFilePath);
    if (!lib->load()) {
        qWarning() << "Failed to load plugin:" << fileName << lib->errorString();
        delete lib;
        return -1;
    }

    auto info = std::make_unique<PluginInfo>();
    info->_moduleName = fileName;
    info->_pluginLoader = lib;  // keep for unload on destruction

    // --- Required: isUnicode() ---
    info->_pFuncIsUnicode = reinterpret_cast<PFUNCISUNICODE>(
        resolvePluginSymbol(lib, "isUnicode", false));
    // If isUnicode is missing, assume unicode (modern plugin)
    if (info->_pFuncIsUnicode && !info->_pFuncIsUnicode()) {
        qWarning() << "ANSI plugin is not compatible with Unicode Notepad++:" << fileName;
        lib->unload();
        delete lib;
        return -1;
    }

    // --- Required: setInfo ---
    info->_pFuncSetInfo = reinterpret_cast<PFUNCSETINFO>(
        resolvePluginSymbol(lib, "setInfo", true));
    if (!info->_pFuncSetInfo) {
        lib->unload();
        delete lib;
        return -1;
    }

    // --- Required: getName ---
    info->_pFuncGetName = reinterpret_cast<PFUNCGETNAME>(
        resolvePluginSymbol(lib, "getName", true));
    if (!info->_pFuncGetName) {
        lib->unload();
        delete lib;
        return -1;
    }

    // Call setInfo to pass NppData to the plugin
    if (_nppData) {
        info->_pFuncSetInfo(*_nppData);
    }

    // Get the display name
    const char* namePtr = info->_pFuncGetName();
    if (namePtr) {
        info->_funcName = QString::fromUtf8(namePtr);
    } else {
        info->_funcName = fileName;
    }

    // --- Required: beNotified ---
    info->_pBeNotified = reinterpret_cast<PBENOTIFIED>(
        resolvePluginSymbol(lib, "beNotified", false)); // optional

    // --- Required: messageProc ---
    info->_pMessageProc = reinterpret_cast<PMESSAGEPROC>(
        resolvePluginSymbol(lib, "messageProc", false)); // optional

    // --- Required: getFuncsArray ---
    info->_pFuncGetFuncsArray = reinterpret_cast<PFUNCGETFUNCSARRAY>(
        resolvePluginSymbol(lib, "getFuncsArray", true));
    if (!info->_pFuncGetFuncsArray) {
        lib->unload();
        delete lib;
        return -1;
    }

    // Retrieve the FuncItem array
    int nbItem = 0;
    info->_funcItems = reinterpret_cast<FuncItem*>(
        info->_pFuncGetFuncsArray(&nbItem));
    info->_nbFuncItem = nbItem;

    if (!info->_funcItems || info->_nbFuncItem <= 0) {
        qWarning() << "Plugin has no FuncItems:" << fileName;
        lib->unload();
        delete lib;
        return -1;
    }

    // Build plugin command entries and assign static command IDs
    for (int j = 0; j < info->_nbFuncItem; ++j) {
        FuncItem& item = info->_funcItems[j];
        int cmdID = ID_PLUGINS_CMD + static_cast<int>(_pluginsCommands.size());
        item._cmdID = cmdID;
        // Item name is already QString (Unicode-capable Qt plugin)
        _pluginsCommands.push_back(
            PluginCommand(info->_funcName, j, item._pFunc));
    }

    addInLoadedDlls(pluginFilePath, fileName);

    // Check if this is a Lexilla lexer plugin (exports GetLexerCount)
    loadLexillaLexersFromPlugin(lib, fileName);

    int pluginIndex = static_cast<int>(_pluginInfos.size());
    _pluginInfos.push_back(QSharedPointer<PluginInfo>(info.release()));
    return pluginIndex;
}

void PluginsManager::loadLexillaLexersFromPlugin(QLibrary* lib, const QString& moduleName)
{
    // Resolve Lexilla functions — optional, so not required
    auto GetLexerCount = reinterpret_cast<Lexilla::GetLexerCountFn>(
        lib->resolve(LEXILLA_GETLEXERCOUNT));
    if (!GetLexerCount) {
        // Not a Lexilla plugin
        return;
    }

    auto GetLexerName = reinterpret_cast<Lexilla::GetLexerNameFn>(
        lib->resolve(LEXILLA_GETLEXERNAME));
    if (!GetLexerName) {
        qWarning() << "Lexilla plugin" << moduleName << "missing GetLexerName — skipping";
        return;
    }

    auto CreateLexerFn = reinterpret_cast<Lexilla::CreateLexerFn>(
        lib->resolve(LEXILLA_CREATELEXER));
    if (!CreateLexerFn) {
        qWarning() << "Lexilla plugin" << moduleName << "missing CreateLexer — skipping";
        return;
    }

    NppParameters& nppParams = NppParameters::getInstance();
    int numLexers = GetLexerCount();
    if (numLexers <= 0)
        return;

    constexpr int MAX_LEXER_NAME = 64;
    char lexName[MAX_LEXER_NAME + 1] = {};

    for (int i = 0; i < numLexers && nppParams.ExternalLangHasRoom(); ++i) {
        GetLexerName(i, lexName, MAX_LEXER_NAME);
        lexName[MAX_LEXER_NAME] = '\0';

        if (nppParams.isExistingExternalLangName(lexName)) {
            continue;  // already registered
        }

        auto extLang = std::make_unique<ExternalLangContainer>();
        extLang->_name = lexName;
        extLang->fnCL = reinterpret_cast<void*>(CreateLexerFn);
        // extLang->_fnGLPN / _fnSLP are optional — leave as nullptr

        nppParams.addExternalLangToEnd(std::move(extLang));
        qDebug() << "Loaded Lexilla lexer:" << lexName << "from" << moduleName;
    }
}

void PluginsManager::runPluginCommand(size_t i)
{
    if (i >= static_cast<size_t>(_pluginsCommands.size()))
        return;

    const PluginCommand& cmd = _pluginsCommands[static_cast<int>(i)];
    if (cmd._pFunc) {
        try {
            cmd._pFunc();
        } catch (const std::exception& e) {
            pluginExceptionAlert(cmd._pluginName, QString::fromLocal8Bit(e.what()));
        } catch (...) {
            pluginCrashAlert(cmd._pluginName,
                QStringLiteral("runPluginCommand(size_t i: %1)").arg(static_cast<int>(i)));
        }
    }
}

void PluginsManager::runPluginCommand(const QString& pluginName, int commandID)
{
    for (size_t i = 0, len = _pluginsCommands.size(); i < len; ++i) {
        if (_pluginsCommands[i]._pluginName == pluginName) {
            if (_pluginsCommands[i]._funcID == commandID) {
                try {
                    _pluginsCommands[i]._pFunc();
                } catch (const std::exception& e) {
                    pluginExceptionAlert(_pluginsCommands[i]._pluginName,
                        QString::fromLocal8Bit(e.what()));
                } catch (...) {
                    pluginCrashAlert(_pluginsCommands[i]._pluginName,
                        QStringLiteral("runPluginCommand(pluginName: %1, commandID: %2)")
                            .arg(pluginName).arg(commandID));
                }
                return;
            }
        }
    }
}

void PluginsManager::addInMenuFromPMIndex(int i)
{
    if (i < 0 || i >= static_cast<int>(_pluginInfos.size()))
        return;

    const QSharedPointer<PluginInfo>& info = _pluginInfos[i];
    if (!info || !_hPluginsMenu)
        return;

    // Create a submenu for this plugin
    QMenu* pluginMenu = new QMenu(info->_funcName);
    _hPluginsMenu->addMenu(pluginMenu);

    for (int j = 0; j < info->_nbFuncItem; ++j) {
        FuncItem& item = info->_funcItems[j];
        if (!item._pFunc) {
            // Separator
            pluginMenu->addSeparator();
            continue;
        }

        QString itemName = item._itemName;
        if (itemName.isEmpty())
            itemName = QStringLiteral("Item %1").arg(j);

        QAction* action = pluginMenu->addAction(itemName);
        action->setData(QVariant::fromValue(static_cast<size_t>(
            ID_PLUGINS_CMD + _pluginsCommands.indexOf(PluginCommand(info->_funcName, j, item._pFunc)))));
        // Note: actual connection to NppCommands::command(int) is done by
        // Notepad_plus::setupMenu() which iterates plugin menu actions
    }
}

QMenu* PluginsManager::initMenu(QMenu* hMenu, bool enablePluginAdmin)
{
    if (_hPluginsMenu)
        return _hPluginsMenu;  // already initialized

    if (hMenu) {
        _hPluginsMenu = hMenu->addMenu(QStringLiteral("Plugins"));
    } else {
        _hPluginsMenu = new QMenu(QStringLiteral("Plugins"));
    }

    // Add Plugins Admin entry if enabled
    if (enablePluginAdmin) {
        _hPluginsMenu->addSeparator();
        _hPluginsMenu->addAction(QStringLiteral("Plugins Admin..."));
    }

    // Add separator before plugin items
    if (!_pluginInfos.isEmpty())
        _hPluginsMenu->addSeparator();

    // Build submenus for each loaded plugin
    for (int i = 0; i < static_cast<int>(_pluginInfos.size()); ++i) {
        addInMenuFromPMIndex(i);
    }

    return _hPluginsMenu;
}

bool PluginsManager::allocateCmdID(int numberRequired, int* start)
{
    if (numberRequired <= 0 || !start)
        return false;

    int result = _dynamicIDAlloc.allocate(numberRequired, start);
    return result != -1;
}

bool PluginsManager::inDynamicRange(int id) const
{
    return _dynamicIDAlloc.isInRange(id);
}

bool PluginsManager::allocateMarker(int numberRequired, int* start)
{
    if (numberRequired <= 0 || !start)
        return false;

    // Simple contiguous allocation for markers
    // MARKER_PLUGINS = 24, LIMIT = 24+15-1 = 38
    static int nextMarker = MARKER_PLUGINS;
    if (nextMarker + numberRequired - 1 > MARKER_PLUGINS_LIMIT) {
        *start = -1;
        return false;
    }
    *start = nextMarker;
    nextMarker += numberRequired;
    return true;
}

bool PluginsManager::allocateIndicator(int numberRequired, int* start)
{
    if (numberRequired <= 0 || !start)
        return false;

    // Simple contiguous allocation for indicators
    // INDICATOR_PLUGINS = 30, LIMIT = 30+20-1 = 49
    static int nextIndicator = INDICATOR_PLUGINS;
    if (nextIndicator + numberRequired - 1 > INDICATOR_PLUGINS_LIMIT) {
        *start = -1;
        return false;
    }
    *start = nextIndicator;
    nextIndicator += numberRequired;
    return true;
}

QString PluginsManager::getLoadedPluginNames() const
{
    QStringList names;
    for (const auto& info : _pluginInfos) {
        if (info) {
            QString name = info->_funcName.isEmpty() ? info->_moduleName : info->_funcName;
            names << name;
        }
    }
    return names.join(u',');
}

void PluginsManager::notify(size_t indexPluginInfo, const SCNotification* notification)
{
    if (indexPluginInfo >= static_cast<size_t>(_pluginInfos.size()))
        return;

    const QSharedPointer<PluginInfo>& info = _pluginInfos[indexPluginInfo];
    if (info && info->_pBeNotified) {
        try {
            info->_pBeNotified(const_cast<SCNotification*>(notification));
        } catch (const std::exception& e) {
            pluginExceptionAlert(info->_moduleName, QString::fromLocal8Bit(e.what()));
        } catch (...) {
            pluginCrashAlert(info->_moduleName,
                QStringLiteral("notify(SCNotification) code=%1").arg(notification->nmhdr.code));
        }
    }
}

void PluginsManager::notify(const SCNotification* notification)
{
    if (_noMoreNotification)
        return;
    _noMoreNotification = (notification->nmhdr.code == NPPN_SHUTDOWN);

    for (int i = 0; i < _pluginInfos.size(); ++i) {
        notify(static_cast<size_t>(i), notification);
    }
}

void PluginsManager::relayNppMessages(unsigned int message, uintptr_t wParam, intptr_t lParam)
{
    for (int i = 0; i < _pluginInfos.size(); ++i) {
        const QSharedPointer<PluginInfo>& info = _pluginInfos[i];
        if (info && info->_pMessageProc) {
            try {
                info->_pMessageProc(message,
                    static_cast<long long>(wParam),
                    static_cast<long long>(lParam));
            } catch (const std::exception& e) {
                pluginExceptionAlert(info->_moduleName, QString::fromLocal8Bit(e.what()));
            } catch (...) {
                pluginCrashAlert(info->_moduleName,
                    QStringLiteral("relayNppMessages(msg=%1)").arg(message));
            }
        }
    }
}

bool PluginsManager::relayPluginMessages(unsigned int message, uintptr_t wParam, intptr_t lParam)
{
    const char* moduleName = reinterpret_cast<const char*>(wParam);
    if (!moduleName || !moduleName[0] || !lParam)
        return false;

    for (int i = 0; i < _pluginInfos.size(); ++i) {
        const QSharedPointer<PluginInfo>& info = _pluginInfos[i];
        if (info && info->_moduleName == QString::fromUtf8(moduleName)) {
            if (info->_pMessageProc) {
                try {
                    info->_pMessageProc(message,
                        static_cast<long long>(wParam),
                        static_cast<long long>(lParam));
                } catch (const std::exception& e) {
                    pluginExceptionAlert(info->_moduleName,
                        QString::fromLocal8Bit(e.what()));
                } catch (...) {
                    pluginCrashAlert(info->_moduleName,
                        QStringLiteral("relayPluginMessages(msg=%1)").arg(message));
                }
                return true;
            }
        }
    }
    return false;
}

void PluginsManager::pluginCrashAlert(const QString& pluginName,
                                      const QString& funcSignature)
{
    QString msg = QStringLiteral("%1 just crashed in %2\n\n"
                                 "Do you want to continue running Notepad++?")
                      .arg(pluginName, funcSignature);
    QMessageBox::critical(nullptr, QStringLiteral("Plugin Crash"), msg);
}

void PluginsManager::pluginExceptionAlert(const QString& pluginName,
                                         const QString& reason)
{
    QString msg = QStringLiteral("An exception occurred due to plugin: %1\n\n"
                                 "Reason: %2")
                      .arg(pluginName, reason);
    QMessageBox::critical(nullptr, QStringLiteral("Plugin Exception"), msg);
}

bool PluginsManager::isInLoadedDlls(const QString& fn) const
{
    return std::any_of(_loadedDlls.begin(), _loadedDlls.end(),
        [&fn](const LoadedDllInfo& dll) {
            return dll._fileName.compare(fn, Qt::CaseInsensitive) == 0;
        });
}

void PluginsManager::addInLoadedDlls(const QString& fullPath, const QString& fn)
{
    _loadedDlls.push_back(LoadedDllInfo(fullPath, fn));
}
