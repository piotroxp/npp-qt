#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for Nim
// Handles: proc, template, macro, {.pragma.}, import, type, let/var/const,
//          # comments, strings, numbers, @ptr[T], `backtick operators`

class LexerNim : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Number = 4,
        Pragma = 5,
        Type = 6,
        Procedure = 7,
        StringLiteral = 8,
        Builtin = 9,
    };

    explicit LexerNim(QObject* parent = nullptr);
    ~LexerNim() override;

    const char* language() const override { return "Nim"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;
    QSet<QString> _types;
    QSet<QString> _builtins;

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_');
    }
};
