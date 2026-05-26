// MISC/RegExpTrees/RegExpLib.h - Qt6 port of regular expression library
#pragma once

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QVector>

struct RegExpMatch {
    int startPosition;
    int endPosition;
    QString capturedText;
    QStringList capturedGroups;
    int matchNumber;
};

struct RegExpResult {
    bool isValid;
    QString errorMessage;
    QList<RegExpMatch> matches;
    int matchCount;
};

class RegExpLib
{
public:
    static RegExpLib& getInstance();
    
    // Compile and validate a regular expression
    bool compile(const QString& pattern, QString& error);
    bool isValid() const { return _isValid; }
    
    // Find all matches in text
    RegExpResult findAll(const QString& text, int maxMatches = 0);
    
    // Find first match
    RegExpMatch findFirst(const QString& text);
    
    // Replace text
    QString replace(const QString& text, const QString& replacement, int maxReplacements = 0);
    
    // Split text by regex
    QStringList split(const QString& text);
    
    // Get pattern info
    QString getPattern() const { return _pattern; }
    QString getError() const { return _error; }
    
    // Set regex options
    void setCaseSensitive(bool caseSensitive);
    void setMinimalMatch(bool minimal);
    void setMultiLine(bool multiLine);
    
private:
    RegExpLib();
    ~RegExpLib() = default;
    
    RegExpLib(const RegExpLib&) = delete;
    RegExpLib& operator=(const RegExpLib&) = delete;
    
    bool _isValid;
    QString _pattern;
    QString _error;
    QRegularExpression _regex;
    
    Qt::CaseSensitivity _caseSensitivity;
    QRegularExpression::MatchOptions _matchOptions;
};

// Utility functions for regex operations
namespace RegExpUtil {
    bool isValidPattern(const QString& pattern);
    QString escapeString(const QString& str);
    QString unescapeString(const QString& str);
    QStringList getCapturedGroups(const QString& pattern);
}