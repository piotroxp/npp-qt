// UrlHelper.h - URL handling utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <QUrl>
#include <QStringList>

class UrlHelper {
public:
    // === Validation ===
    static bool isValidUrl(const QString& url);
    static bool isValidUrl(const std::string& url);

    // === Scheme ===
    static QString getScheme(const QString& url);
    static bool isHttp(const QString& url) { return getScheme(url).toLower() == "http"; }
    static bool isHttps(const QString& url) { return getScheme(url).toLower() == "https"; }
    static bool isFileUrl(const QString& url) { return getScheme(url).toLower() == "file"; }

    // === Host/Port ===
    static QString getHost(const QString& url);
    static int getPort(const QString& url);
    static int getPort(const QString& scheme, int defaultPort = 80);

    // === Path/Query/Fragment ===
    static QString getPath(const QString& url);
    static QString getQuery(const QString& url);
    static QString getFragment(const QString& url);
    static QStringList getQueryItems(const QString& url);

    // === Encoding ===
    static QString urlEncode(const QString& s, bool encodeSlash = false);
    static QString urlDecode(const QString& s);

    // === File URL conversion ===
    static bool isLocalFile(const QString& urlOrPath);
    static QString fromLocalFile(const QString& path);
    static QString toLocalFile(const QString& url);

    // === URL building ===
    static QString buildUrl(const QString& scheme, const QString& host, int port,
                            const QString& path = QString(),
                            const QString& query = QString());
    static QString addQueryParam(const QString& url, const QString& key, const QString& value);

    // === Comparison ===
    static bool equals(const QString& a, const QString& b);

private:
    static QUrl toQUrl(const QString& url);
};
