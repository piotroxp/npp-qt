// PathHelper.h - Path manipulation utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <QStringList>
#include <cstdarg>

class PathHelper {
public:
    // === Path composition ===
    static QString join(const QString& base, const QString& path);
    static QString join(const QStringList& parts);
    static QString joinMultiple(const QString& base, ...);  // varargs version

    // === Normalization ===
    static QString normalize(const QString& path);
    static QString absolute(const QString& path);
    static QString absolute(const QString& base, const QString& path);

    // === Relative paths ===
    static QString relative(const QString& path, const QString& base);
    static bool isRelative(const QString& path);
    static bool isAbsolute(const QString& path);

    // === Path components ===
    static QString dirname(const QString& path);
    static QString basename(const QString& path);
    static QString extension(const QString& path);
    static QString stem(const QString& path);  // basename without extension

    // === Path properties ===
    static bool exists(const QString& path);
    static bool isDir(const QString& path);
    static bool isFile(const QString& path);

    // === Path modification ===
    static QString addExtension(const QString& path, const QString& ext);
    static QString changeExtension(const QString& path, const QString& ext);
    static QString absolutePath(const QString& path);
    static QString cleanPath(const QString& path);

    // === Comparisons ===
    static bool equals(const QString& a, const QString& b);  // normalized comparison
    static bool isChildOf(const QString& path, const QString& parent);

    // === Common operations ===
    static QString home();
    static QString temp();
    static QString config();

private:
    static QString doNormalize(const QString& path);
};
