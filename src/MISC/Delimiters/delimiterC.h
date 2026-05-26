// MISC/Delimiters/delimiterC.h - Qt6 port of comment/section delimiters
#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

// Delimiter configuration for various programming languages
struct DelimiterSet {
    QString commentLine;      // Single-line comment (e.g., // or #)
    QString commentStart;     // Multi-line comment start (e.g., /*)
    QString commentEnd;       // Multi-line comment end (e.g., */)
    QString stringChar;       // String delimiter (e.g., " or ')
    
    DelimiterSet() {}
    DelimiterSet(const QString& line, const QString& start, const QString& end, const QString& str)
        : commentLine(line), commentStart(start), commentEnd(end), stringChar(str) {}
};

class DelimiterC
{
public:
    static DelimiterC& getInstance();
    
    // Get delimiters for a language
    DelimiterSet getDelimiters(const QString& language) const;
    
    // Set custom delimiters for a language
    void setDelimiters(const QString& language, const DelimiterSet& delimiters);
    
    // Built-in delimiter sets
    static DelimiterSet cppDelimiters();
    static DelimiterSet pythonDelimiters();
    static DelimiterSet htmlDelimiters();
    static DelimiterSet xmlDelimiters();
    static DelimiterSet sqlDelimiters();
    static DelimiterSet bashDelimiters();
    static DelimiterSet pascalDelimiters();
    static DelimiterSet perlDelimiters();
    static DelimiterSet phpDelimiters();
    
    // Check if character is a delimiter
    bool isCommentLine(const QString& lang, const QString& line) const;
    bool isStringDelimiter(const QString& lang, QChar c) const;
    
private:
    DelimiterC();
    ~DelimiterC() = default;
    
    DelimiterC(const DelimiterC&) = delete;
    DelimiterC& operator=(const DelimiterC&) = delete;
    
    QMap<QString, DelimiterSet> _delimiterSets;
    
    void initializeBuiltInDelimiters();
};