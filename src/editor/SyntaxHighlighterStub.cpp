// Stub implementation for SyntaxHighlighter
// Full implementation lives in SyntaxHighlighter.cpp (disabled until fixed)
#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {}

SyntaxHighlighter::~SyntaxHighlighter() = default;

void SyntaxHighlighter::highlightBlock(const QString& /*text*/) {}

void SyntaxHighlighter::setLanguage(LangType /*lang*/) {}
