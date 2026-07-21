#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QLibrary>
#include <QDir>
#include <QFileInfo>
#include "PluginInterface.h"

// Plugin metadata and state
struct PluginInfo {
    QString name;                  // Plugin display name
    QString path;                 // Full path to the plugin DLL/SO
    QLibrary* library;            // Handle to the loaded library
    bool enabled;                 // Whether the plugin is active
    QVector<FuncItem> menuItems;   // Menu items exposed by the plugin
    NppQtPlugin* plugin;          // Pointer to plugin struct
    
    PluginInfo() : library(nullptr), enabled(false), plugin(nullptr) {}
    
    ~PluginInfo() {
        if (library) {
            library->unload();
            delete library;
        }
    }
};

class PluginManager : public QObject {
    Q_OBJECT
public:
    static PluginManager& instance();

    // Plugin lifecycle
    void loadPlugins(const QString& pluginDir = QString());
    void unloadPlugins();
    bool loadPlugin(const QString& path);
    bool unloadPlugin(const QString& name);

    // Plugin state
    const QVector<QString>& loadedPluginNames() const { return _pluginNames; }
    bool isLoaded(const QString& name) const;
    bool isEnabled(const QString& name) const;
    
    // Plugin access
    PluginInfo* getPluginInfo(const QString& name);
    const QVector<PluginInfo*>& allPlugins() const { return _plugins; }

    // Menu integration
    QMenu* pluginMenu() { return _pluginMenu; }
    void hookIntoMenu(QMenuBar* menuBar);

    // Plugin API
    NppQtFuncs* functions() { return &_funcs; }
    NppData* nppData() { return &_nppData; }

    // Menu registration (called by plugin API)
    void registerPluginMenuItem(const QString& label, void (*callback)());
    void unregisterPluginMenuItem(const QString& label);

    // Configuration
    QString pluginDirectory() const { return _pluginDir; }
    void setPluginDirectory(const QString& dir) { _pluginDir = dir; }

signals:
    void pluginLoaded(const QString& name);
    void pluginUnloaded(const QString& name);
    void pluginError(const QString& name, const QString& error);
    void pluginEnabled(const QString& name, bool enabled);

public slots:
    void enablePlugin(const QString& name, bool enable = true);
    void disablePlugin(const QString& name);

private:
    PluginManager();
    ~PluginManager();

    // Internal methods
    QString getDefaultPluginDir() const;
    void scanPluginDirectory(const QString& dir);
    void initializeFuncs();
    bool validatePlugin(NppQtPlugin* plugin, const QString& path);
    void createPluginMenu();
    void destroyPluginMenu();
    
    // Plugin data
    QVector<QString> _pluginNames;
    QMap<QString, QLibrary*> _pluginHandles;
    QVector<PluginInfo*> _plugins;
    QMap<QString, PluginInfo*> _pluginMap;
    
    // Menu integration
    QMenu* _pluginMenu;
    QMenuBar* _menuBar;
    QMap<QString, void (*)()> _registeredMenuCallbacks;  // label -> callback
    
    // Configuration
    QString _pluginDir;
    
    // Plugin API
    NppQtFuncs _funcs;
    NppData _nppData;
};
