// MISC/RegExpTrees/RegExpLib.cpp - Qt6 port of regular expression library
#include "RegExpLib.h"

RegExpLib::RegExpLib()
    : _isValid(false)
    , _caseSensitivity(Qt::CaseSensitive)
    , _matchOptions(0)
{
}

RegExpLib& RegExpLib::getInstance()
{
    static RegExpLib instance;
    return instance;
}

bool RegExpLib::compile(const QString& pattern, QString& error)
{
    _pattern = pattern;
    _regex.setPattern(pattern);
    
    QRegularExpression::PatternOptions options;
    if (_matchOptions & QRegularExpression::MultilineOption)
        options |= QRegularExpression::MultilineOption;
    
    _regex.setPatternOptions(options);
    
    if (!_regex.isValid()) {
        _isValid = false;
        _error = _regex.errorString();
        error = _error;
        return false;
    }
    
    _isValid = true;
    _error.clear();
    error.clear();
    return true;
}

RegExpResult RegExpLib::findAll(const QString& text, int maxMatches)
{
    RegExpResult result;
    result.isValid = _isValid;
    result.matchCount = 0;
    
    if (!_isValid) {
        result.errorMessage = _error;
        return result;
    }
    
    QRegularExpressionMatchIterator it = _regex.globalMatch(text);
    int matchNum = 0;
    
    while (it.hasNext()) {
        if (maxMatches > 0 && matchNum >= maxMatches)
            break;
        
        QRegularExpressionMatch match = it.next();
        
        RegExpMatch rm;
        rm.startPosition = match.capturedStart();
        rm.endPosition = match.capturedEnd();
        rm.capturedText = match.captured(0);
        rm.matchNumber = matchNum;
        
        // Capture groups
        for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
            rm.capturedGroups.append(match.captured(i));
        }
        
        result.matches.append(rm);
        ++matchNum;
    }
    
    result.matchCount = result.matches.size();
    return result;
}

RegExpMatch RegExpLib::findFirst(const QString& text)
{
    RegExpMatch match;
    
    if (!_isValid)
        return match;
    
    QRegularExpressionMatch regexMatch = _regex.match(text);
    
    if (regexMatch.hasMatch()) {
        match.startPosition = regexMatch.capturedStart();
        match.endPosition = regexMatch.capturedEnd();
        match.capturedText = regexMatch.captured(0);
        match.matchNumber = 0;
        
        for (int i = 1; i <= regexMatch.lastCapturedIndex(); ++i) {
            match.capturedGroups.append(regexMatch.captured(i));
        }
    }
    
    return match;
}

QString RegExpLib::replace(const QString& text, const QString& replacement, int maxReplacements)
{
    if (!_isValid)
        return text;
    
    if (maxReplacements > 0) {
        // Qt doesn't support max replacements directly, so we implement it
        QString result = text;
        for (int i = 0; i < maxReplacements; ++i) {
            QRegularExpressionMatch match = _regex.match(result);
            if (match.hasMatch()) {
                result.replace(match.capturedStart(), match.capturedLength(), replacement);
            } else {
                break;
            }
        }
        return result;
    } else {
        return text.replace(_regex, replacement);
    }
}

QStringList RegExpLib::split(const QString& text)
{
    if (!_isValid)
        return QStringList(text);
    
    return text.split(_regex, Qt::SkipEmptyParts);
}

void RegExpLib::setCaseSensitive(bool caseSensitive)
{
    _caseSensitivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    _regex.setCaseSensitivity(_caseSensitivity);
}

void RegExpLib::setMinimalMatch(bool minimal)
{
    QRegularExpression::PatternOptions options = _regex.patternOptions();
    if (minimal)
        options |= QRegularExpression::IngreedyMatchOption;
    else
        options &= ~QRegularExpression::IngreedyMatchOption;
    _regex.setPatternOptions(options);
}

void RegExpLib::setMultiLine(bool multiLine)
{
    QRegularExpression::PatternOptions options = _regex.patternOptions();
    if (multiLine)
        options |= QRegularExpression::MultilineOption;
    else
        options &= ~QRegularExpression::MultilineOption;
    _regex.setPatternOptions(options);
}

// Utility functions
namespace RegExpUtil {
    bool isValidPattern(const QString& pattern)
    {
        QRegularExpression regex(pattern);
        return regex.isValid();
    }
    
    QString escapeString(const QString& str)
    {
        return QRegularExpression::escape(str);
    }
    
    QString unescapeString(const QString& str)
    {
        QString result = str;
        result.replace("\\\\", "\\");
        result.replace("\\n", "\n");
        result.replace("\\r", "\r");
        result.replace("\\t", "\t");
        return result;
    }
    
    QStringList getCapturedGroups(const QString& pattern)
    {
        Q_UNUSED(pattern);
        // Group extraction would require parsing the pattern
        return QStringList();
    }
}