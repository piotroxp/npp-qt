// UrlHelper.cpp - URL handling utilities implementation
#include "UrlHelper.h"
#include "StringHelper.h"
#include <QUrlQuery>

QUrl UrlHelper::toQUrl(const QString& url) {
    return QUrl(url);
}

bool UrlHelper::isValidUrl(const QString& url) {
    QUrl qurl = QUrl::fromUserInput(url);
    return qurl.isValid() && !qurl.scheme().isEmpty();
}

bool UrlHelper::isValidUrl(const std::string& url) {
    return isValidUrl(QString::fromUtf8(url.c_str()));
}

QString UrlHelper::getScheme(const QString& url) {
    return toQUrl(url).scheme();
}

QString UrlHelper::getHost(const QString& url) {
    return toQUrl(url).host();
}

int UrlHelper::getPort(const QString& url) {
    QUrl qurl = toQUrl(url);
    if (qurl.port() > 0) return qurl.port();
    return getPort(qurl.scheme(), 80);
}

int UrlHelper::getPort(const QString& scheme, int defaultPort) {
    QString s = scheme.toLower();
    if (s == "https" || s == "wss") return defaultPort == 80 ? 443 : defaultPort;
    if (s == "http" || s == "ws") return defaultPort == 443 ? 80 : defaultPort;
    return defaultPort;
}

QString UrlHelper::getPath(const QString& url) {
    return toQUrl(url).path();
}

QString UrlHelper::getQuery(const QString& url) {
    return toQUrl(url).query();
}

QString UrlHelper::getFragment(const QString& url) {
    return toQUrl(url).fragment();
}

QStringList UrlHelper::getQueryItems(const QString& url) {
    QUrl qurl = toQUrl(url);
    QUrlQuery query(qurl);
    QStringList result;
    for (const QPair<QString, QString>& item : query.queryItems()) {
        result << item.first << item.second;
    }
    return result;
}

QString UrlHelper::urlEncode(const QString& s, bool encodeSlash) {
    return QString::fromUtf8(QUrl::toPercentEncoding(s, encodeSlash ? QByteArray() : "/").constData());
}

QString UrlHelper::urlDecode(const QString& s) {
    return QUrl::fromPercentEncoding(s.toUtf8());
}

bool UrlHelper::isLocalFile(const QString& urlOrPath) {
    if (urlOrPath.startsWith("file://", Qt::CaseInsensitive) ||
        urlOrPath.startsWith("/") ||
        (urlOrPath.length() >= 2 && urlOrPath[1] == ':')) { // Windows drive letter
        return true;
    }
    return false;
}

QString UrlHelper::fromLocalFile(const QString& path) {
    return QUrl::fromLocalFile(path).toString();
}

QString UrlHelper::toLocalFile(const QString& url) {
    QUrl qurl = url.startsWith("file://", Qt::CaseInsensitive) ?
                QUrl(url) : QUrl::fromUserInput(url);
    return qurl.toLocalFile();
}

QString UrlHelper::buildUrl(const QString& scheme, const QString& host, int port,
                            const QString& path, const QString& query) {
    QUrl url;
    url.setScheme(scheme);
    url.setHost(host);
    if (port > 0) url.setPort(port);
    url.setPath(path);
    if (!query.isEmpty()) url.setQuery(query);
    return url.toString();
}

QString UrlHelper::addQueryParam(const QString& url, const QString& key, const QString& value) {
    QUrl qurl = toQUrl(url);
    QUrlQuery query(qurl);
    query.addQueryItem(key, value);
    qurl.setQuery(query);
    return qurl.toString();
}

bool UrlHelper::equals(const QString& a, const QString& b) {
    QUrl qa = toQUrl(a);
    QUrl qb = toQUrl(b);
    // Normalize and compare
    qa.normalize();
    qb.normalize();
    return qa == qb;
}
