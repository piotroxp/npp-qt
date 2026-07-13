// Semantic lift: BoostRegexSearch.h — Win32 boost::wregex → Qt6 QRegularExpression
#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMutex>
#include <QRegularExpression>

struct MatchResult {
    int start = -1;
    int end = -1;
    int length = 0;
    int subSpanCount = 0;  // number of captured groups
};

class BoostRegexSearch
{
public:
    BoostRegexSearch();
    ~BoostRegexSearch();

    // Set the regex pattern. Returns false if pattern is invalid.
    bool setRegex(const QString& pattern, bool isCaseSensitive = true, bool isWholeWord = false);

    // Get the last error message
    QString getLastError() const;

    // Find first match starting at 'start'. Returns start position or -1.
    int match(const QString& text, int start, MatchResult& result);

    // Find all matches in range [start, end]. Returns count.
    int search(const QString& text, int start, int end, QVector<MatchResult>& results);

    // Split text by regex
    QStringList split(const QString& text);

    // Get captured groups (up to maxCount, 0 = all)
    QStringList caputure(const QString& text, int maxCount = 0);

private:
    QRegularExpression _regex;
    mutable QString _lastError;
    mutable QMutex _mutex;
};
