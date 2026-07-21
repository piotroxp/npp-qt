// FunctionListXmlParser.h — Loads Notepad++ functionList XML parsers
// Copyright (C) 2026 Agent Army
// GPL v3
//
// Parses PowerEditor/installer/functionList/*.xml files from the upstream
// notepad-plus-plus-translation repository and converts them into Qt regex
// patterns for use by FunctionListPanel.
//
// N++ XML schema summary
// ─────────────────────
// <parser displayName="…" id="…" commentExpr="…">
//   <classRange mainExpr="…" openSymbole="{" closeSymbole="}">
//     <className mainExpr="…"/>
//     <function mainExpr="…">          ← one or more
//       <functionName mainExpr="…"/>
//       <functionSig   mainExpr="…"/>
//       <functionDisplay mainExpr="…"/>
//     </function>
//   </classRange>
//   <function mainExpr="…">            ← top-level (no class)
//     <functionName mainExpr="…"/>
//     <functionSig   mainExpr="…"/>
//     <functionDisplay mainExpr="…"/>
//   </function>
// </parser>
//
// Each mainExpr uses Oniguruma/PCRE2 syntax.  We convert the most common
// constructs to QRegularExpression, falling back to a simplified pattern
// when full conversion is not possible.

#pragma once

#include <QHash>
#include <QList>
#include <QRegularExpression>
#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

// Forward-declare for the Rule struct below
class FunctionListXmlParser;

// ─── Rule — shared between FunctionListXmlParser and its consumers ───────────
// Declared here so FunctionListPanel.h can use QList<Rule> without including
// the full FunctionListXmlParser.cpp (avoids pulling QDomDocument into the
// header's transitive includes).

struct FunctionListRule {
    QString name;                 // "function" | "classRange"
    QRegularExpression mainExpr; // compiled pattern (empty if skipped)
    QString openSymbole;         // "{" etc.
    QString closeSymbole;        // "}" etc.
    QRegularExpression classNameExpr;
    QRegularExpression funcNameExpr;
    QRegularExpression funcSigExpr;
    QRegularExpression funcDisplayExpr;
    QRegularExpression accessibilityExpr;
    bool isValid = false;
};

class FunctionListXmlParser {

public:
    // ─── Represents one parsed <function> or <classRange> rule ───────────
    struct Rule {
        QString name;                 // "function" | "classRange"
        QRegularExpression mainExpr; // compiled pattern (empty if skipped)
        QString openSymbole;          // "{" etc.
        QString closeSymbole;        // "}" etc.
        QRegularExpression classNameExpr;  // for classRange: extracts class name
        QRegularExpression funcNameExpr;    // function name extractor
        QRegularExpression funcSigExpr;     // signature extractor
        QRegularExpression funcDisplayExpr;// display string extractor
        QRegularExpression accessibilityExpr;// access modifier extractor
        bool isValid = false;        // true only if mainExpr is non-empty
    };

    using FunctionListRule = Rule;

    // ─── One parser language (corresponds to one <parser> element) ─────────
    struct ParserDef {
        QString displayName;
        QString id;
        QRegularExpression commentExpr;  // for stripping comments
        QList<Rule> classRules;           // classRange rules (nested)
        QList<Rule> functionRules;        // top-level function rules
        bool isValid = false;
    };

    explicit FunctionListXmlParser(const QString& xmlDir);

    // Load a specific parser by language extension key (e.g. "cpp", "python").
    // Returns true if XML was found and parsed successfully.
    bool loadParser(const QString& langKey);

    // Returns the parser def for a loaded language, or nullptr.
    const ParserDef* parser(const QString& langKey) const;

    // Returns all language keys that have a valid parser loaded.
    QStringList loadedLanguages() const;

    // Returns true if we have a parser for the given language key.
    bool hasParser(const QString& langKey) const;

    // Translate N++ language key to the filename key used in functionList/.
    // e.g. "cpp" → "cpp", "c" → "c", "javascript" → "javascript.js"
    static QString toXmlKey(const QString& langKey);

private:
    QString _xmlDir;
    QHash<QString, ParserDef> _parsers;

    // ─── XML parsing ─────────────────────────────────────────────────────
    void parseParserElement(QXmlStreamReader& xml, ParserDef& def);
    void parseClassRangeElement(QXmlStreamReader& xml, FunctionListRule& rule);
    void parseFunctionElement(QXmlStreamReader& xml, FunctionListRule& rule);

    // ─── N++ regex → QRegularExpression conversion ────────────────────────
    // Returns true if conversion was successful.
    bool convertPattern(const QString& nppPattern, QRegularExpression& out) const;

    // Applies common N++ regex → Qt regex transformations:
    //   (?s:…)       → Qt::DotMatchesEverythingOption
    //   (?m)         → Qt::MultilineOption
    //   (?-m)        → remove MultilineOption
    //   (?x)         → Qt::ExtendedPatternOption (with cleanup)
    //   (?i)         → QRegularExpression::CaseInsensitiveOption
    //   \x{…}        → \xbehind (Qt supports fixed-length)
    //   (?<!…)       → lookbehind (Qt supports fixed-length)
    //   [[:space:]]  → [ \t\r\n]  (character classes)
    //   [[:word:]]   → [\w]  (word characters)
    //   [[:digit:]]   → [0-9]
    //   &lt;          → <
    //   &gt;          → >
    //   &amp;         → &
    //   &apos;       → '
    //   &#x0D;&#x0A; → \r?\n
    //   \R           → \r?\n|\r
    //   \h           → [ \t]
    //   \v           → [\v\f]
    //   \d           → [0-9]
    //   \w           → [a-zA-Z0-9_]
    //   \s           → [ \t\r\n\v\f]
    QString convertNppRegex(const QString& npp) const;

    // Returns Qt::Match flags derived from the inline (?imsx-imsx) prefix.
    QRegularExpression::PatternOptions extractInlineOptions(const QString& pattern,
                                                            QString& rest) const;
};
