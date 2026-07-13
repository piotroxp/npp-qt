#include "PluginManager.h"
#include "PluginInterface.h"
#include "core/Application.h"
#include <QLibrary>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

PluginManager& PluginManager::instance() { static PluginManager inst; return inst; }

PluginManager::~PluginManager() { unloadPlugins(); }

void PluginManager::loadPlugins(const QString& pluginDir) {
    QDir dir(pluginDir);
    if (!dir.exists()) return;

    for (const QString& entry : dir.entryList(QDir::Files)) {
        QString path = dir.filePath(entry);
        if (QLibrary::isLibrary(path)) {
            loadPlugin(path);
        }
    }
}

void PluginManager::loadPlugin(const QString& path) {
    QLibrary lib(path);
    if (!lib.load()) {
        qDebug() << "[plugin] Failed to load:" << path << lib.errorString();
        emit pluginError(QFileInfo(path).baseName(), lib.errorString());
        return;
    }

    // Resolve the plugin entry function
    auto getPlugin = lib.resolve("nppqt_getPlugin");
    if (!getPlugin) {
        getPlugin = lib.resolve("_nppqt_getPlugin");
    }

    if (!getPlugin) {
        qDebug() << "[plugin] No nppqt_getPlugin symbol in:" << path;
        lib.unload();
        return;
    }

    NppQtPlugin* plugin = reinterpret_cast<NppQtPlugin*(*)()>(getPlugin)();
    if (!plugin) {
        qDebug() << "[plugin] null plugin from:" << path;
        lib.unload();
        return;
    }

    if (plugin->apiVersion != NPPQT_PLUGIN_API_VERSION) {
        qDebug() << "[plugin] API version mismatch in" << path
                 << ": got" << plugin->apiVersion << "expected" << NPPQT_PLUGIN_API_VERSION;
        lib.unload();
        return;
    }

    // Initialize the plugin
    if (plugin->init) {
        plugin->init(&_funcs);
    }

    QString name = QString::fromUtf8(plugin->pluginName);
    _pluginNames.push_back(name);
    _pluginHandles[name] = lib.handle();
    qDebug() << "[plugin] Loaded:" << name;
    emit pluginLoaded(name);
}

void PluginManager::unloadPlugins() {
    for (auto it = _pluginHandles.begin(); it != _pluginHandles.end(); ++it) {
        QLibrary lib(it.value());
        QString name = it.key();
        lib.unload();
        qDebug() << "[plugin] Unloaded:" << name;
        emit pluginUnloaded(name);
    }
    _pluginHandles.clear();
    _pluginNames.clear();
}

bool PluginManager::isLoaded(const QString& name) const {
    return _pluginHandles.contains(name);
}
