#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

// Custom QsciLexer for Dockerfile
// Handles: FROM, RUN, CMD, LABEL, ENV, ADD, COPY, ENTRYPOINT,
//          EXPOSE, VOLUME, USER, WORKDIR, ARG, MAINTAINER, ONBUILD, etc.
//          Comments (#), shell variables (${VAR}), strings

class LexerDockerfile : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        Keyword = 2,
        Instruction = 3,
        String = 4,
        Variable = 5,
        Number = 6,
        Flag = 7,
    };

    explicit LexerDockerfile(QObject* parent = nullptr);
    ~LexerDockerfile() override;

    const char* language() const override { return "Dockerfile"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

private:
    QSet<QString> _keywords;      // instructions (FROM, RUN, CMD, etc.)
    QSet<QString> _flags;         // common flags (-t, -p, -v, etc.)

    static inline bool isWordChar(QChar c) {
        return c.isLetterOrNumber() || c == QLatin1Char('_') || c == QLatin1Char('-');
    }
};
