// MISC/Common/PlatformInterface.h - Qt6 port of platform-specific abstractions
#pragma once

#include <QString>
#include <QStringList>
#include <QSettings>

class PlatformInterface
{
public:
    static PlatformInterface& getInstance();
    
    // Platform detection
    enum class Platform {
        Windows,
        Linux,
        MacOS,
        Other
    };
    
    Platform getPlatform() const;
    bool isWindows() const;
    bool isLinux() const;
    bool isMacOS() const;
    
    // System directories
    QString getSystemDirectory() const;
    QString getApplicationDirectory() const;
    QString getUserDirectory() const;
    QString getTempDirectory() const;
    QString getConfigDirectory() const;
    
    // Registry / Config equivalent
    void writeRegistry(const QString& key, const QString& value);
    QString readRegistry(const QString& key, const QString& defaultValue = QString()) const;
    void deleteRegistry(const QString& key);
    
    // Application paths
    QString getPluginsPath() const;
    QString getThemesPath() const;
    QString getLanguagesPath() const;
    QString getUpdatesPath() const;
    
    // System info
    QString getComputerName() const;
    QString getUserName() const;
    QString getOSVersion() const;
    
    // Environment
    QString getEnvironmentVariable(const QString& name) const;
    void setEnvironmentVariable(const QString& name, const QString& value);
    
private:
    PlatformInterface();
    ~PlatformInterface() = default;
    
    PlatformInterface(const PlatformInterface&) = delete;
    PlatformInterface& operator=(const PlatformInterface&) = delete;
    
    Platform _platform;
    QSettings _settings;
    
    void detectPlatform();
};