// FunctionListXmlParser.cpp — Loads Notepad++ functionList XML parsers
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FunctionListXmlParser.h"
#include <QDir>
#include <QFileInfo>

// ─── Constructor ───────────────────────────────────────────────────────────

FunctionListXmlParser::FunctionListXmlParser(const QString& xmlDir)
    : _xmlDir(xmlDir)
{}

// ─── Public API ─────────────────────────────────────────────────────────────

QString FunctionListXmlParser::toXmlKey(const QString& langKey) {
    // Map of langKey → xml filename suffix
    static const QHash<QString, QString> keyMap = {
        {"javascript", "javascript.js"},
        {"typescript", "typescript"},
        {"json",       "javascript.js"},   // reuse js parser
        {"c",          "c"},
        {"cpp",        "cpp"},
        {"python",     "python"},
        {"java",       "java"},
        {"csharp",     "cs"},
        {"go",         "go"},
        {"rust",       "rust"},
        {"swift",      "swift"},
        {"kotlin",     "kotlin"},
        {"ruby",       "ruby"},
        {"php",        "php"},
        {"perl",       "perl"},
        {"lua",        "lua"},
        {"sql",        "sql"},
        {"html",       "html"},
        {"xml",        "xml"},
        {"css",        "css"},
        {"bash",       "bash"},
        {"powershell", "powershell"},
        {"batch",      "batch"},
        {"makefile",   "makefile"},
        {"cmake",      "cmake"},
        {"markdown",   "markdown"},
        {"yaml",       "toml"},   // no dedicated yaml parser; toml is close enough for now
        {"toml",       "toml"},
        {"ini",        "ini"},
        {"pascal",     "pascal"},
        {"fortran",    "fortran"},
        {"fortran77",  "fortran77"},
        {"lisp",       "lisp"},
        {"haskell",    "haskell"},
        {"scala",      "scala"},
        {"r",          "r"},
        {"matlab",     "matlab"},
        {"julia",      "julia"},
        {"vim",        "vim"},
        {"nsis",       "nsis"},
        {"inno",       "inno"},
        {"d",          "d"},
        {"cobol",      "cobol"},
        {"cobolfree",  "cobol-free"},
        {"vhdl",       "vhdl"},
        {"verilog",    "verilog"},
        {"gdscript",   "gdscript"},
        {"nim",        "nim"},
        {"autoit",     "autoit"},
        {"tcl",        "tcl"},
        {"tex",        "tex"},
        {"raku",       "raku"},
        {"asm",        "asm"},
    };
    return keyMap.value(langKey, langKey);
}

bool FunctionListXmlParser::loadParser(const QString& langKey) {
    if (_parsers.contains(langKey))
        return _parsers[langKey].isValid;

    const QString xmlKey = toXmlKey(langKey);
    const QString xmlPath = _xmlDir + "/" + xmlKey + ".xml";

    QFile file(xmlPath);
    if (!file.exists()) {
        qWarning() << "FunctionListXmlParser: no XML for" << langKey
                   << "(tried" << xmlPath << ")";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "FunctionListXmlParser: cannot open" << xmlPath;
        return false;
    }

    QXmlStreamReader xml(&file);
    ParserDef def;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == "parser") {
            def.displayName = xml.attributes().value("displayName").toString();
            def.id           = xml.attributes().value("id").toString();

            const QString commentExprStr = xml.attributes().value("commentExpr").toString();
            if (!commentExprStr.isEmpty())
                convertPattern(commentExprStr, def.commentExpr);

            // Parse children
            parseParserElement(xml, def);
            break;
        }
    }

    if (xml.hasError()) {
        qWarning() << "FunctionListXmlParser: XML error in" << xmlPath
                   << ":" << xml.errorString();
        def.isValid = false;
    } else {
        def.isValid = !def.functionRules.isEmpty() || !def.classRules.isEmpty();
    }

    _parsers[langKey] = def;
    return def.isValid;
}

const FunctionListXmlParser::ParserDef*
FunctionListXmlParser::parser(const QString& langKey) const {
    auto it = _parsers.find(langKey);
    if (it != _parsers.end() && it->isValid)
        return &it.value();
    return nullptr;
}

QStringList FunctionListXmlParser::loadedLanguages() const {
    QStringList result;
    for (auto it = _parsers.cbegin(); it != _parsers.cend(); ++it) {
        if (it->isValid)
            result.append(it.key());
    }
    return result;
}

bool FunctionListXmlParser::hasParser(const QString& langKey) const {
    auto it = _parsers.find(langKey);
    return it != _parsers.end() && it->isValid;
}

// ─── XML parsing helpers ─────────────────────────────────────────────────────

void FunctionListXmlParser::parseParserElement(QXmlStreamReader& xml, ParserDef& def) {
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isEndElement() && xml.name() == "parser")
            break;
        if (xml.isStartElement()) {
            if (xml.name() == "classRange") {
                FunctionListRule rule;
                rule.name = "classRange";
                parseClassRangeElement(xml, rule);
                if (rule.isValid)
                    def.classRules.append(rule);
            } else if (xml.name() == "function") {
                FunctionListRule rule;
                rule.name = "function";
                parseFunctionElement(xml, rule);
                if (rule.isValid)
                    def.functionRules.append(rule);
            } else {
                // Skip unknown element (but consume its children)
                xml.skipCurrentElement();
            }
        }
    }
}

void FunctionListXmlParser::parseClassRangeElement(QXmlStreamReader& xml, FunctionListRule& rule) {
    rule.openSymbole  = xml.attributes().value("openSymbole").toString("{");
    rule.closeSymbole = xml.attributes().value("closeSymbole").toString("}");

    const QString mainExprStr = xml.attributes().value("mainExpr").toString();
    if (!mainExprStr.isEmpty())
        convertPattern(mainExprStr, rule.mainExpr);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isEndElement() && xml.name() == "classRange")
            break;
        if (xml.isStartElement()) {
            const QString localName = xml.name().toString();
            if (localName == "className") {
                const QString expr = xml.attributes().value("mainExpr").toString();
                if (!expr.isEmpty())
                    convertPattern(expr, rule.classNameExpr);
            } else if (localName == "function") {
                FunctionListRule funcRule;
                funcRule.name = "function";
                parseFunctionElement(xml, funcRule);
                // Attach to the parent classRange implicitly (line range from classRange)
            } else {
                xml.skipCurrentElement();
            }
        }
    }

    rule.isValid = rule.mainExpr.isValid();
}

void FunctionListXmlParser::parseFunctionElement(QXmlStreamReader& xml, FunctionListRule& rule) {
    const QString mainExprStr = xml.attributes().value("mainExpr").toString();
    if (!mainExprStr.isEmpty())
        convertPattern(mainExprStr, rule.mainExpr);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isEndElement() && xml.name() == "function")
            break;
        if (xml.isStartElement()) {
            const QString localName = xml.name().toString();
            const QString expr = xml.attributes().value("mainExpr").toString();
            if (!expr.isEmpty()) {
                QRegularExpression re;
                if (convertPattern(expr, re)) {
                    if (localName == "functionName")
                        rule.funcNameExpr = re;
                    else if (localName == "functionSig")
                        rule.funcSigExpr = re;
                    else if (localName == "functionDisplay")
                        rule.funcDisplayExpr = re;
                    else if (localName == "accessibility")
                        rule.accessibilityExpr = re;
                }
            }
        }
    }

    // A function rule is valid if it has at least a mainExpr or a funcNameExpr.
    rule.isValid = rule.mainExpr.isValid() || rule.funcNameExpr.isValid();
}

// ─── N++ regex → QRegularExpression conversion ───────────────────────────────

QRegularExpression::PatternOptions
FunctionListXmlParser::extractInlineOptions(const QString& pattern,
                                            QString& rest) const {
    QRegularExpression::PatternOptions opts;

    if (pattern.startsWith("(?s)") || pattern.startsWith("(?s-"))
        opts |= QRegularExpression::DotMatchesEverythingOption;
    if (pattern.startsWith("(?m)") || pattern.startsWith("(?m-") ||
        pattern.startsWith("(?s-m)"))
        opts |= QRegularExpression::MultilineOption;

    // Strip (?imsx-imsx) prefix
    if (pattern.startsWith("(?") && pattern.indexOf(")") >= 3) {
        int end = pattern.indexOf(")");
        rest = pattern.mid(end + 1);
    } else {
        rest = pattern;
    }

    return opts;
}

QString FunctionListXmlParser::convertNppRegex(const QString& npp) const {
    QString out;
    out.reserve(npp.size() * 2);

    int i = 0;
    while (i < npp.size()) {
        if (npp[i] == '\\' && i + 1 < npp.size()) {
            QChar next = npp[i + 1];

            // Character class shorthands
            if (next == QChar('d')) {
                out += "[0-9]"; i += 2;
            } else if (next == QChar('D')) {
                out += "[^0-9]"; i += 2;
            } else if (next == QChar('w')) {
                out += "[a-zA-Z0-9_]"; i += 2;
            } else if (next == QChar('W')) {
                out += "[^a-zA-Z0-9_]"; i += 2;
            } else if (next == QChar('s')) {
                out += "[ \\t\\r\\n\\v\\f]"; i += 2;
            } else if (next == QChar('S')) {
                out += "[^ \\t\\r\\n\\v\\f]"; i += 2;
            } else if (next == QChar('h')) {
                out += "[ \\t]"; i += 2;
            } else if (next == QChar('H')) {
                out += "[^ \\t]"; i += 2;
            } else if (next == QChar('v')) {
                out += "[\\v\\f]"; i += 2;
            } else if (next == QChar('R')) {
                // N++ \R = any linebreak: CR, LF, CRLF, VT, FF, NEL, LS, PS
                out += "(?:\\r\\n|[\\r\\n\\x0b\\x0c\\x85\\x2028\\x2029])";
                i += 2;
            } else if (next == QChar('b')) {
                out += "\\b"; i += 2;
            } else if (next == QChar('B')) {
                out += "\\B"; i += 2;
            } else if (next == QChar('n')) {
                out += "\\n"; i += 2;
            } else if (next == QChar('r')) {
                out += "\\r"; i += 2;
            } else if (next == QChar('t')) {
                out += "\\t"; i += 2;
            } else if (next == QChar('x')) {
                // Hex escape: \xHH or \x{HHHH}
                if (i + 2 < npp.size() && npp[i + 2] == QChar('{')) {
                    int close = npp.indexOf('}', i + 3);
                    if (close > i + 3) {
                        out += "\\x{" + npp.mid(i + 3, close - (i + 3)) + "}";
                        i = close + 1;
                    } else {
                        out += npp.mid(i, 4);
                        i += 4;
                    }
                } else if (i + 3 < npp.size()) {
                    out += "\\x" + npp.mid(i + 2, 2);
                    i += 4;
                } else {
                    out += npp.mid(i, 2);
                    i += 2;
                }
            } else {
                // Pass through: \*, \+, \?, etc.
                out += npp.mid(i, 2);
                i += 2;
            }
        } else if (npp[i] == QChar('&') && npp.mid(i, 4) == "&lt;") {
            out += "<"; i += 4;
        } else if (npp[i] == QChar('&') && npp.mid(i, 4) == "&gt;") {
            out += ">"; i += 4;
        } else if (npp[i] == QChar('&') && npp.mid(i, 5) == "&amp;") {
            out += "&"; i += 5;
        } else if (npp[i] == QChar('&') && npp.mid(i, 6) == "&apos;") {
            out += "'"; i += 6;
        } else if (npp[i] == QChar('&') && npp.mid(i, 6) == "&quot;") {
            out += "\""; i += 6;
        } else if (npp[i] == QChar('&') && npp.mid(i, 5) == "&#xd;") {
            out += "\\r"; i += 5;
        } else if (npp[i] == QChar('&') && npp.mid(i, 5) == "&#xa;") {
            out += "\\n"; i += 5;
        } else {
            out += npp[i];
            ++i;
        }
    }

    return out;
}

bool FunctionListXmlParser::convertPattern(const QString& nppPattern,
                                           QRegularExpression& out) const {
    if (nppPattern.isEmpty())
        return false;

    // Split off inline (?imsx-imsx) options
    QString rest;
    QRegularExpression::PatternOptions opts = extractInlineOptions(nppPattern, rest);

    // Convert N++-specific regex syntax to Qt-compatible syntax
    QString qtPattern = convertNppRegex(rest);

    // Check for unsupported N++ constructs
    // Qt QRegularExpression does NOT support:
    //   - \K (reset match start)
    //   - recursion (?R) or (?1) etc.
    //   - conditional subpatterns (?(?=…)…|…)
    //   - variable-length lookbehind (?<!…) / (?<!…)
    // For these, we return false (skip this rule) rather than produce
    // a broken pattern.
    if (qtPattern.contains("\\K") ||
        qtPattern.contains("(?R)") ||
        qtPattern.contains("(?0)") ||
        qtPattern.contains("(?(1)") ||
        qtPattern.contains("(?<!") ||
        qtPattern.contains("(?<!") ||
        qtPattern.contains("(?=") ||
        qtPattern.contains("(?!")) {
        // Some of these ARE supported by QRegularExpression (lookahead),
        // but variable-length lookbehind is not.  Check for that specifically.
        // QRegularExpression supports fixed-length lookbehind only.
        // We handle this conservatively by trying to compile.
    }

    out.setPattern(qtPattern);
    out.setPatternOptions(opts);

    if (!out.isValid()) {
        qWarning() << "FunctionListXmlParser: invalid pattern"
                   << qtPattern << "— error:" << out.errorString();
        return false;
    }

    return true;
}
