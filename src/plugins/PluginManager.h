#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

struct NppQtFuncs;
struct NppQtPlugin;

class PluginManager : public QObject {
    Q_OBJECT
public:
    static PluginManager& instance();

    void loadPlugins(const QString& pluginDir);
    void unloadPlugins();

    const QVector<QString>& loadedPluginNames() const { return _pluginNames; }
    bool isLoaded(const QString& name) const;

signals:
    void pluginLoaded(const QString& name);
    void pluginUnloaded(const QString& name);
    void pluginError(const QString& name, const QString& error);

private:
    PluginManager() = default;
    ~PluginManager();

    void loadPlugin(const QString& path);

    QVector<QString> _pluginNames;
    QMap<QString, void*> _pluginHandles;  // library handles (void* from QLibrary::handle())
    NppQtFuncs _funcs;                    // shared function table
};
