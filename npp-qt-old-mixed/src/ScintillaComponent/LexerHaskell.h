#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for Haskell
// Handles: module, import, data, type, class, instance, deriving,
//          -> => :: kinds, -- and {- -} comments, strings, numbers

class LexerHaskell : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Number = 4,
        Type = 5,
        Class = 6,
        Function = 7,
        Operator = 8,
        Import = 9,
    };

    explicit LexerHaskell(QObject* parent = nullptr);
    ~LexerHaskell() override;

    const char* language() const override { return "Haskell"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;
    QSet<QString> _classes;
    QSet<QString> _builtins;

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('\'') || c == QLatin1Char('_');
    }
};
