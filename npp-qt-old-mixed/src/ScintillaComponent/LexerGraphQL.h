#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for GraphQL
// Handles: query, mutation, subscription, type, fragment, input, enum, union,
//          interface, directive, extend, { } braces, : arguments, # comments, strings

class LexerGraphQL : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        TypeName = 4,
        Field = 5,
        Argument = 6,
        Variable = 7,
        Directive = 8,
        Type = 9,
    };

    explicit LexerGraphQL(QObject* parent = nullptr);
    ~LexerGraphQL() override;

    const char* language() const override { return "GraphQL"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;
    QSet<QString> _types;

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_');
    }
};
