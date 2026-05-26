// MISC/Common/PlatformInterface.cpp - Qt6 port of platform-specific abstractions
#include "PlatformInterface.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QProcess>
#include <QHostInfo>

PlatformInterface::PlatformInterface()
    : _settings(QSettings::IniFormat, QSettings::UserScope, "Notepad++", "config")
{
    detectPlatform();
}

PlatformInterface& PlatformInterface::getInstance()
{
    static PlatformInterface instance;
    return instance;
}

void PlatformInterface::detectPlatform()
{
#if defined(Q_OS_WIN)
    _platform = Platform::Windows;
#elif defined(Q_OS_LINUX)
    _platform = Platform::Linux;
#elif defined(Q_OS_MAC)
    _platform = Platform::MacOS;
#else
    _platform = Platform::Other;
#endif
}

PlatformInterface::Platform PlatformInterface::getPlatform() const
{
    return _platform;
}

bool PlatformInterface::isWindows() const
{
    return _platform == Platform::Windows;
}

bool PlatformInterface::isLinux() const
{
    return _platform == Platform::Linux;
}

bool PlatformInterface::isMacOS() const
{
    return _platform == Platform::MacOS;
}

QString PlatformInterface::getSystemDirectory() const
{
    if (isWindows()) {
        return QString::fromLocal8Bit(qgetenv("SystemRoot")) + "/System32";
    } else if (isLinux()) {
        return "/usr/lib";
    } else if (isMacOS()) {
        return "/System/Library";
    }
    return QString();
}

QString PlatformInterface::getApplicationDirectory() const
{
    return QCoreApplication::applicationDirPath();
}

QString PlatformInterface::getUserDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

QString PlatformInterface::getTempDirectory() const
{
    return QDir::tempPath();
}

QString PlatformInterface::getConfigDirectory() const
{
    if (isWindows()) {
        return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    } else {
        return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    }
}

void PlatformInterface::writeRegistry(const QString& key, const QString& value)
{
    // On non-Windows, we use QSettings as the registry equivalent
    _settings.setValue(key, value);
    _settings.sync();
}

QString PlatformInterface::readRegistry(const QString& key, const QString& defaultValue) const
{
    return _settings.value(key, defaultValue).toString();
}

void PlatformInterface::deleteRegistry(const QString& key)
{
    _settings.remove(key);
    _settings.sync();
}

QString PlatformInterface::getPluginsPath() const
{
    if (isWindows()) {
        return getApplicationDirectory() + "/plugins";
    } else if (isLinux()) {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/plugins";
    } else if (isMacOS()) {
        return getApplicationDirectory() + "/../PlugIns";
    }
    return QString();
}

QString PlatformInterface::getThemesPath() const
{
    if (isWindows()) {
        return getApplicationDirectory() + "/themes";
    } else if (isLinux()) {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/themes";
    } else if (isMacOS()) {
        return getApplicationDirectory() + "/../Resources/themes";
    }
    return QString();
}

QString PlatformInterface::getLanguagesPath() const
{
    if (isWindows()) {
        return getApplicationDirectory() + "/nativeLang";
    } else if (isLinux()) {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/languages";
    } else if (isMacOS()) {
        return getApplicationDirectory() + "/../Resources/languages";
    }
    return QString();
}

QString PlatformInterface::getUpdatesPath() const
{
    return getConfigDirectory() + "/updates";
}

QString PlatformInterface::getComputerName() const
{
#if defined(Q_OS_WIN)
    return QString::fromLocal8Bit(qgetenv("COMPUTERNAME"));
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    return QHostInfo::localHostName();
#else
    return QString();
#endif
}

QString PlatformInterface::getUserName() const
{
#if defined(Q_OS_WIN)
    return QString::fromLocal8Bit(qgetenv("USERNAME"));
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    return QString::fromLocal8Bit(qgetenv("USER"));
#else
    return QString();
#endif
}

QString PlatformInterface::getOSVersion() const
{
#if defined(Q_OS_WIN)
    QSysInfo sysInfo;
    return sysInfo.productType() + " " + sysInfo.productVersion();
#elif defined(Q_OS_LINUX)
    QProcess proc;
    proc.start("lsb_release", QStringList() << "-d");
    proc.waitForFinished();
    return QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
#elif defined(Q_OS_MAC)
    QSysInfo sysInfo;
    return sysInfo.productType() + " " + sysInfo.productVersion();
#else
    return QString();
#endif
}

QString PlatformInterface::getEnvironmentVariable(const QString& name) const
{
    return QString::fromLocal8Bit(qgetenv(name.toLocal8Bit().constData()));
}

void PlatformInterface::setEnvironmentVariable(const QString& name, const QString& value)
{
    qputenv(name.toLocal8Bit().constData(), value.toLocal8Bit());
}