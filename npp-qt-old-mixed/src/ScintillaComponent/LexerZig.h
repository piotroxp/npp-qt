#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for Zig
// Handles: fn, struct, enum, const, var, comptime, defer, suspend, resume,
//          @builtin, // and /* */ comments, strings, numbers

class LexerZig : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        String = 2,
        Keyword = 3,
        Number = 4,
        Builtin = 5,
        Type = 6,
        Attribute = 7,
        Function = 8,
        Operator = 9,
    };

    explicit LexerZig(QObject* parent = nullptr);
    ~LexerZig() override;

    const char* language() const override { return "Zig"; }
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
