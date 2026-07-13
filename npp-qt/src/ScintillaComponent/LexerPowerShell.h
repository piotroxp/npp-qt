#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for PowerShell
// Inherits from QsciLexerCustom to handle PowerShell's unique syntax:
//   - Variables: $varname
//   - Operators: -eq -ne -gt -lt -match -replace -like -contains -in -notin ...
//   - Cmdlets: Verb-Noun pattern (Get-Item, Set-Content, Where-Object, ...)
//   - Comments: # and <# #>
//   - Strings: "..." '...' @"..."@ @'...'@

class LexerPowerShell : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Cmdlet = 4,
        Variable = 5,
        OperatorPS = 6,
        Number = 7,
        HereString = 8,
        BlockComment = 9,
    };

    explicit LexerPowerShell(QObject* parent = nullptr);
    ~LexerPowerShell() override;

    const char* language() const override { return "PowerShell"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    // Keyword sets
    QSet<QString> _keywords;
    QSet<QString> _cmdlets;
    QSet<QString> _operators;
    QSet<QString> _variables;  // true/false/null constants

    // Helper: is character a word character (alphanumeric + underscore)
    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_');
    }

    // Helper: is character whitespace
    static inline bool isWhiteSpace(QChar c) {
        return c.isSpace();
    }

    // Helper: is character a digit
    static inline bool isDigit(QChar c) {
        return c.isDigit();
    }

    // Core tokenizer
    void tokenize(const char* text, int len, int startPos);
};
