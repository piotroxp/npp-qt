#include "PluginManager.h"
#include "core/Application.h"
#include <QLibrary>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QGuiApplication>
#include <QWindow>

PluginManager& PluginManager::instance() { 
    static PluginManager inst; 
    return inst; 
}

PluginManager::PluginManager() 
    : _pluginMenu(nullptr)
    , _menuBar(nullptr)
    , _pluginDir()
{
    // Initialize NppData stub
    _nppData._nppHandle = nullptr;
    _nppData._scintillaMainHandle = nullptr;
    _nppData._scintillaSecondHandle = nullptr;
}

PluginManager::~PluginManager() { 
    unloadPlugins(); 
    destroyPluginMenu();
}

QString PluginManager::getDefaultPluginDir() const {
    // Check environment variable first
    QString envPath = QString::fromLocal8Bit(qgetenv("NPPQT_PLUGINS_DIR"));
    if (!envPath.isEmpty() && QDir(envPath).exists()) {
        return envPath;
    }
    
    // Try ./plugins relative to working directory
    QString localPath = QDir::current().filePath("plugins");
    if (QDir(localPath).exists()) {
        return localPath;
    }
    
    // Try user's Notepad++ plugins directory (Wine/Windows compatibility)
    QString nppPath = QDir::home().filePath(".notepad-plus-plus/plugins");
    if (QDir(nppPath).exists()) {
        return nppPath;
    }
    
    // Fallback to ./plugins
    return QDir::current().filePath("plugins");
}

void PluginManager::initializeFuncs() {
    // Zero-initialize the function table
    memset(&_funcs, 0, sizeof(NppQtFuncs));
    
    // Set version
    _funcs.getVersion = []() -> int {
        return NPPQT_PLUGIN_API_VERSION;
    };
}

void PluginManager::loadPlugins(const QString& pluginDir) {
    QString dir = pluginDir.isEmpty() ? _pluginDir : pluginDir;
    
    qDebug() << "[PluginManager] Loading plugins from:" << dir;
    
    QDir pluginsDir(dir);
    if (!pluginsDir.exists()) {
        qDebug() << "[PluginManager] Plugin directory does not exist:" << dir;
        return;
    }

    // Scan for plugin files
    scanPluginDirectory(dir);
}

void PluginManager::scanPluginDirectory(const QString& dir) {
    QDir pluginsDir(dir);
    
    // Get all files and check for subdirectories (plugin bundles)
    QFileInfoList entries = pluginsDir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
    );
    
    for (const QFileInfo& entry : entries) {
        QString path = entry.absoluteFilePath();
        
        if (entry.isDir()) {
            // Check if it's a plugin bundle (folder with .dll/.so)
            scanPluginDirectory(path);
        } else if (entry.isFile()) {
            // Check if it's a library file
            if (QLibrary::isLibrary(path)) {
                loadPlugin(path);
            }
        }
    }
}

bool PluginManager::loadPlugin(const QString& path) {
    QFileInfo fileInfo(path);
    QString name = fileInfo.baseName();
    
    qDebug() << "[PluginManager] Attempting to load:" << path;
    
    // Check if already loaded
    if (_pluginHandles.contains(name)) {
        qDebug() << "[PluginManager] Plugin already loaded:" << name;
        return false;
    }
    
    // Load the library
    QLibrary* lib = new QLibrary(path);
    
    // Try to load with platform-specific options
    lib->setLoadHints(QLibrary::ExportExternalSymbolsHint);
    
    if (!lib->load()) {
        qWarning() << "[PluginManager] Failed to load library:" << path 
                   << "Error:" << lib->errorString();
        emit pluginError(name, lib->errorString());
        delete lib;
        return false;
    }
    
    // Try to resolve the plugin entry function
    // Support both our custom API and legacy NPP API
    NppQtPlugin* plugin = nullptr;
    
    // Try our custom API first
    auto getPluginFn = lib->resolve("nppqt_getPlugin");
    if (!getPluginFn) {
        getPluginFn = lib->resolve("_nppqt_getPlugin");
    }
    
    if (getPluginFn) {
        plugin = reinterpret_cast<NppQtPlugin*(*)()>(getPluginFn)();
    }
    
    // Validate plugin struct
    if (!plugin) {
        qWarning() << "[PluginManager] No nppqt_getPlugin symbol found in:" << path;
        lib->unload();
        delete lib;
        return false;
    }
    
    // Validate plugin
    if (!validatePlugin(plugin, path)) {
        lib->unload();
        delete lib;
        return false;
    }
    
    // Create plugin info
    PluginInfo* info = new PluginInfo();
    info->name = QString::fromUtf8(plugin->pluginName);
    info->path = path;
    info->library = lib;
    info->plugin = plugin;
    info->enabled = true;
    
    // Call plugin initialization
    if (plugin->init) {
        qDebug() << "[PluginManager] Initializing plugin:" << info->name;
        try {
            plugin->init(&_funcs);
        } catch (const std::exception& e) {
            qWarning() << "[PluginManager] Exception during plugin init:" << info->name << e.what();
            emit pluginError(info->name, QString("Initialization failed: %1").arg(e.what()));
            delete info;
            lib->unload();
            delete lib;
            return false;
        } catch (...) {
            qWarning() << "[PluginManager] Unknown exception during plugin init:" << info->name;
            emit pluginError(info->name, "Unknown initialization error");
            delete info;
            lib->unload();
            delete lib;
            return false;
        }
    }
    
    // Check for legacy NPP menu items
    auto getFuncItems = lib->resolve("npp_getFunctionItems");
    if (getFuncItems) {
        FuncItem* items = reinterpret_cast<FuncItem*(*)()>(getFuncItems)();
        if (items) {
            // Count items (null terminator or until MAX_FUNCITEMS)
            int count = 0;
            while (items[count]._itemName[0] != '\0' && count < 256) {
                info->menuItems.append(items[count]);
                count++;
            }
            qDebug() << "[PluginManager] Found" << count << "legacy menu items for:" << info->name;
        }
    }
    
    // Check for NPP-style setInfo
    auto setInfoFn = lib->resolve("npp_setInfo");
    if (setInfoFn) {
        qDebug() << "[PluginManager] Calling npp_setInfo for:" << info->name;
        reinterpret_cast<void(*)(NppData*)>(setInfoFn)(&_nppData);
    }
    
    // Store plugin
    _plugins.append(info);
    _pluginMap[info->name] = info;
    _pluginNames.append(info->name);
    _pluginHandles[name] = lib;
    
    // Update plugin menu
    if (_pluginMenu) {
        createPluginMenu();
    }
    
    qDebug() << "[PluginManager] Successfully loaded plugin:" << info->name;
    emit pluginLoaded(info->name);
    
    return true;
}

bool PluginManager::validatePlugin(NppQtPlugin* plugin, const QString& path) {
    if (!plugin) {
        qWarning() << "[PluginManager] Null plugin struct from:" << path;
        return false;
    }
    
    // Check API version
    if (plugin->apiVersion != NPPQT_PLUGIN_API_VERSION) {
        qWarning() << "[PluginManager] API version mismatch in" << path
                   << "- got" << plugin->apiVersion 
                   << "expected" << NPPQT_PLUGIN_API_VERSION;
        emit pluginError(QFileInfo(path).baseName(), 
            QString("API version mismatch: got %1, expected %2")
                .arg(plugin->apiVersion)
                .arg(NPPQT_PLUGIN_API_VERSION));
        return false;
    }
    
    // Check plugin name
    if (!plugin->pluginName || plugin->pluginName[0] == '\0') {
        qWarning() << "[PluginManager] Invalid plugin name in:" << path;
        emit pluginError(QFileInfo(path).baseName(), "Invalid plugin name");
        return false;
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const QString& name) {
    // Find plugin by name
    PluginInfo* info = _pluginMap.value(name, nullptr);
    if (!info) {
        qDebug() << "[PluginManager] Plugin not found:" << name;
        return false;
    }
    
    qDebug() << "[PluginManager] Unloading plugin:" << name;
    
    // Call cleanup function
    if (info->plugin && info->plugin->cleanup) {
        try {
            info->plugin->cleanup();
        } catch (const std::exception& e) {
            qWarning() << "[PluginManager] Exception during plugin cleanup:" << name << e.what();
        } catch (...) {
            qWarning() << "[PluginManager] Unknown exception during plugin cleanup:" << name;
        }
    }
    
    // Call destroy if exported (NPP compatibility)
    if (info->library) {
        auto destroyFn = info->library->resolve("npp_destroy");
        if (destroyFn) {
            reinterpret_cast<void(*)()>(destroyFn)();
        }
    }
    
    // Remove from collections
    _pluginNames.removeAll(name);
    _plugins.removeAll(info);
    _pluginMap.remove(name);
    _pluginHandles.remove(name);
    
    // Unload library
    if (info->library) {
        info->library->unload();
        delete info->library;
    }
    
    delete info;
    
    // Update menu
    if (_pluginMenu) {
        createPluginMenu();
    }
    
    emit pluginUnloaded(name);
    return true;
}

void PluginManager::unloadPlugins() {
    qDebug() << "[PluginManager] Unloading all plugins...";
    
    // Unload in reverse order (LIFO)
    while (!_pluginNames.isEmpty()) {
        QString name = _pluginNames.last();
        unloadPlugin(name);
    }
    
    qDebug() << "[PluginManager] All plugins unloaded";
}

bool PluginManager::isLoaded(const QString& name) const {
    return _pluginHandles.contains(name) || _pluginMap.contains(name);
}

bool PluginManager::isEnabled(const QString& name) const {
    PluginInfo* info = _pluginMap.value(name, nullptr);
    return info ? info->enabled : false;
}

PluginInfo* PluginManager::getPluginInfo(const QString& name) {
    return _pluginMap.value(name, nullptr);
}

void PluginManager::createPluginMenu() {
    if (!_pluginMenu) {
        return;
    }
    
    // Clear existing menu
    _pluginMenu->clear();
    
    if (_plugins.isEmpty()) {
        _pluginMenu->addAction("No plugins loaded")->setEnabled(false);
        return;
    }
    
    // Add menu items for each plugin
    for (PluginInfo* info : _plugins) {
        if (!info || !info->enabled) {
            continue;
        }
        
        // Create submenu for plugin with menu items
        if (!info->menuItems.isEmpty()) {
            QMenu* subMenu = _pluginMenu->addMenu(info->name);
            
            for (const FuncItem& item : info->menuItems) {
                if (item._itemName[0] == '\0') {
                    subMenu->addSeparator();
                } else {
                    QAction* action = subMenu->addAction(item._itemName);
                    QObject::connect(action, &QAction::triggered, this, [info, item]() {
                        qDebug() << "[PluginManager] Plugin action triggered:"
                                 << info->name << "::" << item._itemName;
                        if (item._pFunc) {
                            item._pFunc();
                        }
                    });
                }
            }
        } else {
            // Add simple action for plugins without menu items
            QAction* action = _pluginMenu->addAction(info->name);
            action->setEnabled(false);
        }
    }
    
    _pluginMenu->addSeparator();
    
    // Add reload plugins action
    QAction* reloadAction = _pluginMenu->addAction("Reload Plugins");
    QObject::connect(reloadAction, &QAction::triggered, this, [this]() {
        unloadPlugins();
        loadPlugins();
    });
    
    // Add open plugins folder action
    QAction* openFolderAction = _pluginMenu->addAction("Open Plugins Folder");
    QObject::connect(openFolderAction, &QAction::triggered, this, [this]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(_pluginDir));
    });
}

void PluginManager::destroyPluginMenu() {
    if (_pluginMenu) {
        _pluginMenu->deleteLater();
        _pluginMenu = nullptr;
    }
}

void PluginManager::hookIntoMenu(QMenuBar* menuBar) {
    if (!menuBar) {
        qWarning() << "[PluginManager] Cannot hook into null menu bar";
        return;
    }
    
    _menuBar = menuBar;
    
    // Create or recreate plugin menu
    if (!_pluginMenu) {
        _pluginMenu = new QMenu("Plugins");
        createPluginMenu();
    }
    
    // Find the "Tools" or last menu and insert before it, or append
    QList<QMenu*> menus = menuBar->findChildren<QMenu*>();
    int insertIndex = menus.size(); // Default: append at end
    
    // Try to insert before "Tools" or similar
    for (QMenu* menu : menus) {
        QString title = menu->title().toLower();
        if (title.contains("tools") || title.contains("settings")) {
            insertIndex = menuBar->actions().indexOf(menu->menuAction());
            break;
        }
    }
    
    menuBar->insertMenu(menuBar->actions().value(insertIndex), _pluginMenu);
    
    qDebug() << "[PluginManager] Hooked into menu bar at position:" << insertIndex;
}

void PluginManager::enablePlugin(const QString& name, bool enable) {
    PluginInfo* info = _pluginMap.value(name, nullptr);
    if (!info) {
        return;
    }
    
    info->enabled = enable;
    
    if (enable && info->plugin && info->plugin->init) {
        // Re-initialize if enabling
        info->plugin->init(&_funcs);
    }
    
    emit pluginEnabled(name, enable);
    createPluginMenu();
}

void PluginManager::disablePlugin(const QString& name) {
    enablePlugin(name, false);
}
