#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for R language
// Handles: <- assignment, function(){}, library(), data.frame(),
//          if/else/for/while, # comments, $ accessor, @ slot, strings, numbers

class LexerR : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Number = 4,
        Function = 5,
        Operator = 6,
        Identifier = 7,
        Dollar = 8,
        Special = 9,
    };

    explicit LexerR(QObject* parent = nullptr);
    ~LexerR() override;

    const char* language() const override { return "R"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;
    QSet<QString> _functions;

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_') || c == QLatin1Char('.');
    }
};
