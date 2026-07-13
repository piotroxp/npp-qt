#pragma once

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>

class LexerGo : public QsciLexerCPP
{
    Q_OBJECT
    Q_PROPERTY(QStringList goKeywords READ goKeywords)
    Q_PROPERTY(QStringList goTypes READ goTypes)
    Q_PROPERTY(QStringList goBuiltins READ goBuiltins)

public:
    enum {
        GoLabel = 20,
        GoPackage = 21,
        GoImport = 22
    };

    LexerGo(QObject* parent = nullptr);
    ~LexerGo() override;

    const char* language() const override;
    const char* lexer() const override;
    QString description(int style) const override;
    void setKeywords(int set, const QString& keywords);
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;

    QStringList goKeywords() const;
    QStringList goTypes() const;
    QStringList goBuiltins() const;

private:
    QString _goKeywords;
    QString _goTypes;
    QString _goBuiltins;
};
