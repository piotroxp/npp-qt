// Semantic lift: BoostRegexSearch — Win32 boost::wregex → Qt6 QRegularExpression
// Original: PowerEditor/src/MISC/PluginsManager/BoostRegexSearch.cxx
// This replaces the STUB in functionParser.cpp with a real implementation
// using Qt's QRegularExpression (thread-safe, no external deps)

#include "BoostRegexSearch.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>

BoostRegexSearch::BoostRegexSearch()
{
}

BoostRegexSearch::~BoostRegexSearch()
{
}

bool BoostRegexSearch::setRegex(const QString& pattern, bool isCaseSensitive, bool isWholeWord)
{
    QMutexLocker locker(&_mutex);

    QString p = pattern;
    if (isWholeWord) {
        p = QStringLiteral("\\b") + pattern + QStringLiteral("\\b");
    }

    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    if (!isCaseSensitive) {
        options |= QRegularExpression::CaseInsensitiveOption;
    }

    QRegularExpression re(p, options);
    if (!re.isValid()) {
        _lastError = re.errorString();
        return false;
    }

    _regex = re;
    _lastError.clear();
    return true;
}

QString BoostRegexSearch::getLastError() const
{
    return _lastError;
}

int BoostRegexSearch::match(const QString& text, int start, MatchResult& result)
{
    QMutexLocker locker(&_mutex);

    if (!_regex.isValid())
        return -1;

    QRegularExpressionMatch match = _regex.match(text, start);
    if (match.hasMatch()) {
        result.start = match.capturedStart();
        result.end = match.capturedEnd();
        result.length = match.capturedLength();
        result.subSpanCount = match.lastCapturedIndex();
        return match.capturedStart();
    }

    return -1;
}

int BoostRegexSearch::search(const QString& text, int start, int end, QVector<MatchResult>& results)
{
    QMutexLocker locker(&_mutex);

    if (!_regex.isValid())
        return 0;

    results.clear();
    int matchCount = 0;

    QRegularExpressionMatchIterator it = _regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        if (match.capturedStart() >= start) {
            if (end < 0 || match.capturedStart() <= end) {
                MatchResult r;
                r.start = match.capturedStart();
                r.end = match.capturedEnd();
                r.length = match.capturedLength();
                r.subSpanCount = match.lastCapturedIndex();
                results.append(r);
                ++matchCount;
            }
        }
    }

    return matchCount;
}

QStringList BoostRegexSearch::split(const QString& text)
{
    QMutexLocker locker(&_mutex);
    if (!_regex.isValid())
        return QStringList{text};
    return text.split(_regex, Qt::SkipEmptyParts);
}

QStringList BoostRegexSearch::caputure(const QString& text, int maxCount)
{
    QMutexLocker locker(&_mutex);
    if (!_regex.isValid())
        return QStringList{text};

    QRegularExpressionMatch match = _regex.match(text);
    if (!match.hasMatch())
        return QStringList{text};

    QStringList caps;
    int count = match.lastCapturedIndex() + 1;
    if (maxCount > 0 && count > maxCount)
        count = maxCount;

    for (int i = 0; i < count; ++i) {
        caps.append(match.captured(i));
    }
    return caps;
}
