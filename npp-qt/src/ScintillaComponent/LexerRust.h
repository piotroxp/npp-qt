#pragma once

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>

class LexerRust : public QsciLexerCPP
{
    Q_OBJECT
    Q_PROPERTY(QStringList rustKeywords READ rustKeywords)
    Q_PROPERTY(QStringList rustTypes READ rustTypes)
    Q_PROPERTY(QStringList rustBuiltins READ rustBuiltins)
    Q_PROPERTY(QStringList rustAttributes READ rustAttributes)

public:
    enum {
        RustAttribute = 20,
        RustLifetime = 21,
        RustMacro = 22
    };

    LexerRust(QObject* parent = nullptr);
    ~LexerRust() override;

    const char* language() const override;
    const char* lexer() const override;
    QString description(int style) const override;
    void setKeywords(int set, const QString& keywords);
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;

    QStringList rustKeywords() const;
    QStringList rustTypes() const;
    QStringList rustBuiltins() const;
    QStringList rustAttributes() const;

private:
    QByteArray _rustKeywords;
    QByteArray _rustTypes;
    QByteArray _rustBuiltins;
    QByteArray _rustAttributes;
};
