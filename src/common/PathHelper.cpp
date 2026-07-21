// PathHelper.cpp - Path manipulation utilities implementation
#include "PathHelper.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <climits>

QString PathHelper::doNormalize(const QString& path) {
    return QDir::cleanPath(path);
}

QString PathHelper::join(const QString& base, const QString& path) {
    QString cleanBase = base;
    if (cleanBase.endsWith('/') || cleanBase.endsWith('\\')) {
        cleanBase.chop(1);
    }
    QString cleanPath = path;
    if (cleanPath.startsWith('/') || cleanPath.startsWith('\\')) {
        return cleanBase + cleanPath;
    }
    return cleanBase + QDir::separator() + cleanPath;
}

QString PathHelper::join(const QStringList& parts) {
    if (parts.isEmpty()) return QString();
    QString result = parts.first();
    for (int i = 1; i < parts.size(); ++i) {
        result = join(result, parts[i]);
    }
    return result;
}

QString PathHelper::joinMultiple(const QString& base, ...) {
    QStringList parts;
    parts.append(base);
    va_list args;
    va_start(args, base);
    const char* part = va_arg(args, const char*);
    while (part) {
        parts.append(QString::fromUtf8(part));
        part = va_arg(args, const char*);
    }
    va_end(args);
    return join(parts);
}

QString PathHelper::normalize(const QString& path) {
    return QDir::cleanPath(path);
}

QString PathHelper::absolute(const QString& path) {
    if (isAbsolute(path)) return normalize(path);
    return absolute(QDir::currentPath(), path);
}

QString PathHelper::absolute(const QString& base, const QString& path) {
    if (isAbsolute(path)) return normalize(path);
    return normalize(join(base, path));
}

QString PathHelper::relative(const QString& path, const QString& base) {
    QString absPath = absolute(path);
    QString absBase = absolute(base);
    return QDir(absBase).relativeFilePath(absPath);
}

bool PathHelper::isRelative(const QString& path) {
    return QFileInfo(path).isRelative();
}

bool PathHelper::isAbsolute(const QString& path) {
    return QFileInfo(path).isAbsolute();
}

QString PathHelper::dirname(const QString& path) {
    QFileInfo info(path);
    return info.path();
}

QString PathHelper::basename(const QString& path) {
    QFileInfo info(path);
    return info.fileName();
}

QString PathHelper::extension(const QString& path) {
    QFileInfo info(path);
    return info.suffix();
}

QString PathHelper::stem(const QString& path) {
    QFileInfo info(path);
    return info.completeBaseName();
}

bool PathHelper::exists(const QString& path) {
    return QFileInfo::exists(path);
}

bool PathHelper::isDir(const QString& path) {
    QFileInfo info(path);
    return info.exists() && info.isDir();
}

bool PathHelper::isFile(const QString& path) {
    QFileInfo info(path);
    return info.exists() && info.isFile();
}

QString PathHelper::addExtension(const QString& path, const QString& ext) {
    QString cleanExt = ext;
    if (!cleanExt.startsWith('.')) {
        cleanExt = "." + cleanExt;
    }
    return path + cleanExt;
}

QString PathHelper::changeExtension(const QString& path, const QString& ext) {
    QFileInfo info(path);
    QString baseName = info.completeBaseName();
    QString newExt = ext;
    if (!newExt.isEmpty() && !newExt.startsWith('.')) {
        newExt = "." + newExt;
    }
    return join(info.path(), baseName + newExt);
}

QString PathHelper::absolutePath(const QString& path) {
    return QFileInfo(path).absoluteFilePath();
}

QString PathHelper::cleanPath(const QString& path) {
    return QDir::cleanPath(path);
}

bool PathHelper::equals(const QString& a, const QString& b) {
    // Normalize both paths and compare
    QString na = normalize(a);
    QString nb = normalize(b);
    // Handle Windows case insensitivity
#if defined(_WIN32) || defined(_WIN64)
    return na.compare(nb, Qt::CaseInsensitive) == 0;
#else
    return na == nb;
#endif
}

bool PathHelper::isChildOf(const QString& path, const QString& parent) {
    QString absPath = absolute(path);
    QString absParent = absolute(parent);
    // Ensure parent ends with separator for proper matching
    QString checkParent = absParent;
    if (!checkParent.endsWith('/') && !checkParent.endsWith('\\')) {
        checkParent += QDir::separator();
    }
    return absPath.startsWith(checkParent, Qt::CaseSensitive);
}

QString PathHelper::home() {
    return QDir::homePath();
}

QString PathHelper::temp() {
    return QDir::tempPath();
}

QString PathHelper::config() {
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}
