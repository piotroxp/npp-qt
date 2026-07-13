// Custom QsciLexer for Go
// Based on QsciLexerCPP with Go-specific keyword sets and custom styles

#include "LexerGo.h"

#include <QColor>
#include <QFont>

LexerGo::LexerGo(QObject* parent)
    : QsciLexerCPP(parent)
{
    // Set 0: Go keywords
    _goKeywords = QStringLiteral(
        "break case chan const continue default defer else fallthrough "
        "for func go goto if import interface map package range return "
        "select struct switch type var"
    );
    QByteArray kw0 = _goKeywords.toLatin1();
    setKeywords(0, kw0.constData());

    // Set 1: Go types
    _goTypes = QStringLiteral(
        "bool byte complex64 complex128 error float32 float64 "
        "int int8 int16 int32 int64 rune string "
        "uint uint8 uint16 uint32 uint64 uintptr"
    );
    QByteArray kw1 = _goTypes.toLatin1();
    setKeywords(1, kw1.constData());

    // Set 2: Go built-in functions
    _goBuiltins = QStringLiteral(
        "append cap close complex copy delete imag len make new panic print println recover real"
    );
    QByteArray kw2 = _goBuiltins.toLatin1();
    setKeywords(2, kw2.constData());
}

LexerGo::~LexerGo()
{
}

const char* LexerGo::language() const
{
    return "Go";
}

const char* LexerGo::lexer() const
{
    return "cpp"; // Uses CPP base lexer
}

QString LexerGo::description(int style) const
{
    switch (style) {
        case GoLabel:   return tr("Go Label");
        case GoPackage: return tr("Go Package");
        case GoImport:  return tr("Go Import");
        default:        return QsciLexerCPP::description(style);
    }
}

QColor LexerGo::defaultColor(int style) const
{
    switch (style) {
        case GoLabel:   return QColor(0x00, 0x00, 0x8B);   // dark blue
        case GoPackage: return QColor(0x00, 0x64, 0x00);   // dark green
        case GoImport:  return QColor(0x00, 0x80, 0x80);   // teal
        default:        return QsciLexerCPP::defaultColor(style);
    }
}

QFont LexerGo::defaultFont(int style) const
{
    // Use italic font for doc comments (style 15)
    if (style == 15) {
        QFont f = QsciLexerCPP::defaultFont(style);
        f.setItalic(true);
        return f;
    }
    return QsciLexerCPP::defaultFont(style);
}
