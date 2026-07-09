// SyntaxHighlighter.h - QSyntaxHighlighter wrapper
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include "../common/Types.h"

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument* parent = nullptr);
    ~SyntaxHighlighter() override;

    void setLanguage(LangType lang);
    LangType language() const { return _language; }

    void setTheme(const QString& themeName);
    QString currentTheme() const { return _themeName; }

    void setKeywords(const QStringList& keywords);
    void setCommentStart(const QString& start);
    void setCommentEnd(const QString& end);
    void setSingleLineComment(const QString& comment);

protected:
    void highlightBlock(const QString& text) override;

private:
    void setupCppFormat();
    void setupPythonFormat();
    void setupHtmlFormat();
    void setupJsonFormat();
    void setupDefaultFormat();
    void applyFormat(int start, int length, const QTextCharFormat& format);

    LangType _language = LangType::L_TEXT;
    QString _themeName = "default";
    QVector<QTextCharFormat> _formats;
    QStringList _keywords;
    QString _commentStart;
    QString _commentEnd;
    QString _singleLineComment;
    QTextCharFormat _keywordFormat;
    QTextCharFormat _stringFormat;
    QTextCharFormat _commentFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _typeFormat;
};
