#pragma once

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>

class LexerTypeScript : public QsciLexerCPP
{
    Q_OBJECT
    Q_PROPERTY(QStringList typeScriptKeywords READ keywords)
    Q_PROPERTY(QStringList tsBuiltins READ tsBuiltins)

public:
    enum {
        TypeScriptReserved = 20,
        TypeScriptMember = 21,
        TypeScriptTemplate = 22,
        TypeScriptAlias = 23
    };

    LexerTypeScript(QObject* parent = nullptr);
    ~LexerTypeScript() override;

    const char* language() const override;
    const char* lexer() const override;
    QString description(int style) const override;

    void setKeywords(int set, const QString& keywords);

    QStringList keywords() const { return _typescriptKeywords.split(QLatin1Char(' ')); }
    QStringList tsBuiltins() const;

    QSet<QString> typeScriptBuiltins() const;

private:
    QString _typescriptKeywords;
};
