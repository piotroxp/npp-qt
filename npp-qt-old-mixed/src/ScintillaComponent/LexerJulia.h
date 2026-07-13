#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for Julia
// Handles: function, module, using, import, @macro, ::type,
//          # comments, strings, numbers, symbols

class LexerJulia : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Number = 4,
        Macro = 5,
        TypeAnnotation = 6,
        Operator = 7,
        Function = 8,
        Module = 9,
    };

    explicit LexerJulia(QObject* parent = nullptr);
    ~LexerJulia() override;

    const char* language() const override { return "Julia"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;
    QSet<QString> _functions;

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_') || c == QLatin1Char('!');
    }
};
