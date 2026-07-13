// Custom QsciLexer for TypeScript
// Based on QsciLexerCPP with TypeScript-specific keyword additions
// Original: lexilla/lexers/LexTypeScript.cxx

#include "LexerTypeScript.h"

LexerTypeScript::LexerTypeScript(QObject* parent)
    : QsciLexerCPP(parent)
{
}

LexerTypeScript::~LexerTypeScript()
{
}

const char* LexerTypeScript::language() const
{
    return "TypeScript";
}

const char* LexerTypeScript::lexer() const
{
    return "cpp"; // Uses CPP base lexer
}

QString LexerTypeScript::description(int style) const
{
    switch (style) {
        case TypeScriptReserved:      return tr("TypeScript Reserved");
        case TypeScriptMember:       return tr("TypeScript Member");
        case TypeScriptTemplate:      return tr("TypeScript Template");
        case TypeScriptAlias:        return tr("TypeScript Alias");
        default: return QsciLexerCPP::description(style);
    }
}

void LexerTypeScript::setKeywords(int set, const QString& keywords)
{
    if (set == 4) {
        // TypeScript-specific keywords
        _typescriptKeywords = keywords;
    }
    QsciLexerCPP::setKeywords(set, keywords);
}

QSet<QString> LexerTypeScript::typeScriptBuiltins() const
{
    return {
        QStringLiteral("abstract"), QStringLiteral("any"), QStringLiteral("as"),
        QStringLiteral("async"), QStringLiteral("await"), QStringLiteral("boolean"),
        QStringLiteral("break"), QStringLiteral("case"), QStringLiteral("catch"),
        QStringLiteral("class"), QStringLiteral("const"), QStringLiteral("constructor"),
        QStringLiteral("continue"), QStringLiteral("debugger"), QStringLiteral("declare"),
        QStringLiteral("default"), QStringLiteral("delete"), QStringLiteral("do"),
        QStringLiteral("else"), QStringLiteral("enum"), QStringLiteral("export"),
        QStringLiteral("extends"), QStringLiteral("false"), QStringLiteral("finally"),
        QStringLiteral("for"), QStringLiteral("from"), QStringLiteral("function"),
        QStringLiteral("get"), QStringLiteral("implements"), QStringLiteral("import"),
        QStringLiteral("in"), QStringLiteral("infer"), QStringLiteral("instanceof"),
        QStringLiteral("interface"), QStringLiteral("is"), QStringLiteral("keyof"),
        QStringLiteral("let"), QStringLiteral("module"), QStringLiteral("namespace"),
        QStringLiteral("never"), QStringLiteral("new"), QStringLiteral("null"),
        QStringLiteral("number"), QStringLiteral("object"), QStringLiteral("of"),
        QStringLiteral("package"), QStringLiteral("private"), QStringLiteral("protected"),
        QStringLiteral("public"), QStringLiteral("readonly"), QStringLiteral("require"),
        QStringLiteral("return"), QStringLiteral("set"), QStringLiteral("static"),
        QStringLiteral("string"), QStringLiteral("super"), QStringLiteral("switch"),
        QStringLiteral("symbol"), QStringLiteral("this"), QStringLiteral("throw"),
        QStringLiteral("true"), QStringLiteral("try"), QStringLiteral("type"),
        QStringLiteral("typeof"), QStringLiteral("undefined"), QStringLiteral("unique"),
        QStringLiteral("unknown"), QStringLiteral("var"), QStringLiteral("void"),
        QStringLiteral("while"), QStringLiteral("with"), QStringLiteral("yield"),
        // Common TS types
        QStringLiteral("Partial"), QStringLiteral("Required"), QStringLiteral("Readonly"),
        QStringLiteral("Record"), QStringLiteral("Pick"), QStringLiteral("Omit"),
        QStringLiteral("Exclude"), QStringLiteral("Extract"), QStringLiteral("NonNullable"),
        QStringLiteral("Parameters"), QStringLiteral("ReturnType"), QStringLiteral("InstanceType"),
        QStringLiteral("Promise"), QStringLiteral("Array"), QStringLiteral("Map"), QStringLiteral("Set"),
        QStringLiteral("console"), QStringLiteral("window"), QStringLiteral("document"),
    };
}
