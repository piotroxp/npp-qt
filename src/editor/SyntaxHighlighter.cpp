// SyntaxHighlighter.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "SyntaxHighlighter.h"
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QRegularExpression>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    setupDefaultFormat();
}

SyntaxHighlighter::~SyntaxHighlighter() = default;

void SyntaxHighlighter::setLanguage(LangType lang) {
    _language = lang;
    switch (lang) {
        case LangType::L_CPP: setupCppFormat(); break;
        case LangType::L_PYTHON: setupPythonFormat(); break;
        case LangType::L_HTML: setupHtmlFormat(); break;
        case LangType::L_JSON: setupJsonFormat(); break;
        default: setupDefaultFormat(); break;
    }
    rehighlight();
}

void SyntaxHighlighter::setTheme(const QString& themeName) {
    _themeName = themeName;
    setLanguage(_language);
}

void SyntaxHighlighter::setKeywords(const QStringList& keywords) {
    _keywords = keywords;
}

void SyntaxHighlighter::setCommentStart(const QString& start) { _commentStart = start; }
void SyntaxHighlighter::setCommentEnd(const QString& end) { _commentEnd = end; }
void SyntaxHighlighter::setSingleLineComment(const QString& comment) { _singleLineComment = comment; }

void SyntaxHighlighter::highlightBlock(const QString& text) {
    // Keywords
    if (!_keywords.isEmpty()) {
        for (const QString& kw : _keywords) {
            int index = 0;
            while ((index = text.indexOf(kw, index)) != -1) {
                applyFormat(index, kw.length(), _keywordFormat);
                index += kw.length();
            }
        }
    }
    // Single-line comment
    if (!_singleLineComment.isEmpty()) {
        int idx = text.indexOf(_singleLineComment);
        if (idx >= 0) applyFormat(idx, text.length() - idx, _commentFormat);
    }
    // Strings
    QRegularExpression stringRx("\"[^\"]*\"|'[^']*'");
    QRegularExpressionMatchIterator it = stringRx.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        applyFormat(m.capturedStart(), m.capturedLength(), _stringFormat);
    }
    // Numbers
    QRegularExpression numRx("\\b\\d+(\\.\\d+)?\\b");
    QRegularExpressionMatchIterator nit = numRx.globalMatch(text);
    while (nit.hasNext()) {
        QRegularExpressionMatch m = nit.next();
        applyFormat(m.capturedStart(), m.capturedLength(), _numberFormat);
    }
}

void SyntaxHighlighter::setupCppFormat() {
    QFont f;
    f.setBold(true);
    _keywordFormat.setFont(f);
    _keywordFormat.setForeground(Qt::blue);
    _stringFormat.setForeground(Qt::darkGreen);
    _commentFormat.setForeground(Qt::gray);
    _numberFormat.setForeground(Qt::magenta);
    _typeFormat.setForeground(Qt::darkRed);

    _keywords = QStringList() << "int" << "void" << "char" << "bool" << "float" << "double"
                              << "class" << "struct" << "enum" << "public" << "private"
                              << "protected" << "virtual" << "override" << "const" << "static"
                              << "if" << "else" << "for" << "while" << "do" << "switch"
                              << "case" << "break" << "continue" << "return" << "new" << "delete"
                              << "this" << "namespace" << "using" << "template" << "typename"
                              << "include" << "define" << "ifdef" << "ifndef" << "endif";
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";
}

void SyntaxHighlighter::setupPythonFormat() {
    _keywordFormat.setForeground(Qt::blue);
    _stringFormat.setForeground(Qt::darkGreen);
    _commentFormat.setForeground(Qt::gray);
    _numberFormat.setForeground(Qt::magenta);

    _keywords = QStringList() << "def" << "class" << "if" << "elif" << "else" << "for" << "while"
                              << "try" << "except" << "finally" << "with" << "as" << "import"
                              << "from" << "return" << "yield" << "raise" << "pass" << "break"
                              << "continue" << "and" << "or" << "not" << "in" << "is" << "lambda"
                              << "True" << "False" << "None" << "self" << "async" << "await";
    _singleLineComment = "#";
    _commentStart = "\"\"\"";
    _commentEnd = "\"\"\"";
}

void SyntaxHighlighter::setupHtmlFormat() {
    _keywordFormat.setForeground(Qt::red);
    _stringFormat.setForeground(Qt::darkGreen);
    _commentFormat.setForeground(Qt::gray);
    _numberFormat.setForeground(Qt::magenta);
    _singleLineComment = "";
}

void SyntaxHighlighter::setupJsonFormat() {
    _keywordFormat.setForeground(Qt::blue);
    _stringFormat.setForeground(Qt::darkGreen);
    _commentFormat.setForeground(Qt::gray);
    _numberFormat.setForeground(Qt::magenta);
}

void SyntaxHighlighter::setupDefaultFormat() {
    _keywordFormat.setForeground(Qt::blue);
    _stringFormat.setForeground(Qt::darkGreen);
    _commentFormat.setForeground(Qt::gray);
    _numberFormat.setForeground(Qt::magenta);
    _typeFormat.setForeground(Qt::darkRed);
}

void SyntaxHighlighter::applyFormat(int start, int length, const QTextCharFormat& format) {
    if (start >= 0 && length > 0) {
        setFormat(start, length, format);
    }
}
