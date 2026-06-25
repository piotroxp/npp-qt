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

#include "PluginsManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QPluginLoader>
#include <QMessageBox>

#include <algorithm>
#include <stdexcept>

PluginsManager::PluginsManager()
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

bool PluginsManager::loadPlugins(const QString& dir, const PluginViewList*, PluginViewList*)
{
    if (dir.isEmpty()) {
        // Default plugin directory
        QString appDir = QCoreApplication::applicationDirPath();
        QString pluginDir = appDir + QStringLiteral("/plugins");
        if (!QDir(pluginDir).exists())
            return true; // No plugin dir — not an error

        QDirIterator it(pluginDir, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            QString subDir = it.next();
            QString pluginFile = subDir + QStringLiteral("/") + QFileInfo(subDir).fileName();
#ifdef Q_OS_WIN
            pluginFile += QStringLiteral(".dll");
#else
            pluginFile += QStringLiteral(".so");
#endif
            if (QFileInfo::exists(pluginFile)) {
                loadPluginFromPath(pluginFile);
            }
        }
    } else {
        // Scan for plugins in specified directory
        QDir pluginDir(dir);
        QStringList nameFilters;
#ifdef Q_OS_WIN
        nameFilters << QStringLiteral("*.dll");
#elif defined(Q_OS_MAC)
        nameFilters << QStringLiteral("*.dylib");
#else
        nameFilters << QStringLiteral("*.so");
#endif
        const QFileInfoList files = pluginDir.entryInfoList(nameFilters, QDir::Files);
        for (const QFileInfo& fi : files) {
            loadPluginFromPath(fi.absoluteFilePath());
        }
    }
    return true;
}

int PluginsManager::loadPluginFromPath(const QString& pluginFilePath)
{
    if (pluginFilePath.isEmpty())
        return -1;

    QString fileName = QFileInfo(pluginFilePath).fileName();

    // Check if already loaded
    if (isInLoadedDlls(fileName))
        return -1;

    // Check architecture compatibility
    // On Win32: use PE headers. On Qt: just try to load.
    QPluginLoader* loader = new QPluginLoader(pluginFilePath);
    if (!loader->load()) {
        qWarning() << "Failed to load plugin:" << fileName
                   << loader->errorString();
        delete loader;
        return -1;
    }

    // Get plugin interface
    QObject* instance = loader->instance();
    if (!instance) {
        qWarning() << "Plugin instance is null:" << fileName;
        loader->unload();
        delete loader;
        return -1;
    }

    auto info = std::make_unique<PluginInfo>();
    info->_pluginLoader = loader;
    info->_moduleName = fileName;

    int pluginIndex = static_cast<int>(_pluginInfos.size());
    _pluginInfos.push_back(QSharedPointer<PluginInfo>(info.release()));
    addInLoadedDlls(pluginFilePath, fileName);

    return pluginIndex;
}

void PluginsManager::runPluginCommand(size_t i)
{
    if (i >= static_cast<size_t>(_pluginsCommands.size()))
        return;

    // BLOCKED: depends on plugin DLL function table and NppCommands command dispatch
    const PluginCommand& cmd = _pluginsCommands[static_cast<int>(i)];
    Q_UNUSED(cmd);
}

void PluginsManager::runPluginCommand(const QString& pluginName, int commandID)
{
    Q_UNUSED(pluginName);
    Q_UNUSED(commandID);
    // BLOCKED: depends on PluginInfo module-name lookup + func table
}

void PluginsManager::addInMenuFromPMIndex(int i)
{
    Q_UNUSED(i);
    // BLOCKED: depends on PluginMenu population from plugin's funcsetInfo structure
}

QMenu* PluginsManager::initMenu(QMenu* hMenu, bool enablePluginAdmin)
{
    Q_UNUSED(enablePluginAdmin);
    _hPluginsMenu = new QMenu(QObject::tr("Plugins"));

    if (hMenu) {
        hMenu->addMenu(_hPluginsMenu);
    }
    return _hPluginsMenu;
}

bool PluginsManager::allocateCmdID(int, int*)
{
    // BLOCKED: depends on IDAllocator (MISC/Common/IDAllocator.h) + command ID range management
    return false;
}

bool PluginsManager::allocateMarker(int, int*)
{
    return false;
}

bool PluginsManager::allocateIndicator(int, int*)
{
    return false;
}

QString PluginsManager::getLoadedPluginNames() const
{
    QStringList names;
    for (const auto& info : _pluginInfos) {
        if (info)
            names << info->_moduleName;
    }
    return names.join(u',');
}

void PluginsManager::notify(size_t indexPluginInfo, const SCNotification*)
{
    if (indexPluginInfo >= static_cast<size_t>(_pluginInfos.size()))
        return;
    // Plugin notification dispatch: requires plugin function table lookup
}

void PluginsManager::notify(const SCNotification*)
{
    // Broadcast to all plugins: requires plugin function table lookup
}

void PluginsManager::relayNppMessages(unsigned int, uintptr_t, intptr_t)
{
    // TODO: Convert SendMessage to Qt signal/slot
}

bool PluginsManager::relayPluginMessages(unsigned int, uintptr_t, intptr_t)
{
    return false;
}

void PluginsManager::pluginCrashAlert(const QString& pluginName,
                                      const QString& funcSignature)
{
    QString msg = QStringLiteral("%1 just crashed in %2")
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
