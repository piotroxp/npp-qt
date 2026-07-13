// npp-qt: LexerUDL — QsciLexerCustom subclass for Notepad++ User Defined Languages
#include <Qsci/qscilexercustom.h>
//
// Wraps a UserLangParser and uses QsciLexerCustom::styleText() to apply
// UDL keyword sets, delimiters, and styles to Scintilla text.
//
// The UDL lexer performs multi-pass highlighting:
//  1. Apply keyword sets (Comments, Keywords1-8, Numbers, Operators) via word boundary matching
//  2. Apply delimiter pairs (string delimiters, brackets) via character matching
//  3. Apply folder markers (code folding regions) via begin/end regex
//
// This replaces the raw Scintilla UDL lexer (SCE_USER_*) with a pure-Qt/QScintilla
// implementation that uses QsciLexerCustom for full QScintilla integration.
#pragma once

#include <QColor>
#include <QFont>
#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

class UserLangParser;

// Scintilla style IDs used by the UDL lexer (SCE_USER_* equivalent)
// These match the IDs in UserLangParser::mapStyleNameToId().
namespace UdlStyleId {
    constexpr int Default       = 0;
    constexpr int Comments      = 1;
    constexpr int LineComments  = 2;
    constexpr int Numbers       = 3;
    constexpr int Keywords1     = 4;
    constexpr int Keywords2     = 5;
    constexpr int Keywords3     = 6;
    constexpr int Keywords4     = 7;
    constexpr int Keywords5     = 8;
    constexpr int Keywords6     = 9;
    constexpr int Keywords7     = 10;
    constexpr int Keywords8     = 11;
    constexpr int Operators     = 12;
    constexpr int FolderCode1   = 13;
    constexpr int FolderCode2   = 14;
    constexpr int FolderComment = 15;
    constexpr int Delimiters1   = 16;
    constexpr int Delimiters2   = 17;
    constexpr int Delimiters3   = 18;
    constexpr int Delimiters4   = 19;
    constexpr int Delimiters5   = 20;
    constexpr int Delimiters6   = 21;
    constexpr int Delimiters7   = 22;
    constexpr int Delimiters8   = 23;
    // Extended styles (IDs 24-31) for custom UDL styles
    constexpr int CustomStart   = 24;
    constexpr int MaxStyle     = 31;
}

class LexerUDL : public QsciLexerCustom
{
    Q_OBJECT

public:
    // Construct a UDL lexer from an already-parsed UserLangParser.
    explicit LexerUDL(const UserLangParser& parser, QObject* parent = nullptr);
    ~LexerUDL() override;

    // QsciLexerCustom interface
    const char* language() const override { return "UDL"; }
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;

    // Access the underlying parser
    const UserLangParser& parser() const { return _parser; }
    QString languageName() const { return _parser.languageName(); }

private:
    // Highlight one segment of text using keyword matching
    void highlightWithKeywords(int startPos, int endPos,
                                const QString& text,
                                int defaultStyle);

    // Highlight text between delimiter pairs
    void highlightDelimiters(int startPos, int endPos,
                              const QString& text,
                              int defaultStyle);

    // Highlight comments (line comments and block comments)
    void highlightComments(int startPos, int endPos,
                           const QString& text,
                           int defaultStyle);

    // Highlight numbers (with prefix/suffix support)
    void highlightNumbers(int startPos, int endPos,
                          const QString& text,
                          int defaultStyle);

    // Highlight operators
    void highlightOperators(int startPos, int endPos,
                            const QString& text,
                            int defaultStyle);

    // Fold the document (return fold level changes at positions)
    int braceFlag(int style) const;

    // Internal helpers
    void buildKeywordSets();
    void buildDelimiters();

    // Is a character at given position inside a comment or string?
    bool isInCommentOrString(int pos) const;

    const UserLangParser& _parser;

    // Pre-processed keyword sets (lowercased for case-insensitive matching)
    // Map: styleId → set of keywords (or keyword regex patterns)
    struct KeywordSetData {
        QSet<QString> keywords;     // lowercased keywords
        QString rawKeywords;        // original case for display
        bool isPrefix = false;     // match at word start only
        bool caseIgnored = false;
    };
    QMap<int, KeywordSetData> _keywordSets;

    // Delimiter pairs: openChar → {closeChar, escapeChar, styleId}
    struct DelimiterData {
        QChar open;
        QChar close;
        QChar escape;
        int styleId;
        bool isString;  // true = strings (highlight content), false = brackets
    };
    QList<DelimiterData> _delimiters;

    // Compiled regexes for number detection
    QString _numberPrefixRE;
    QString _numberSuffixRE;
    QString _numberExtrasRE;

    // Comment markers
    struct CommentStyle {
        QString lineComment;      // e.g. "//" or "#"
        QString blockCommentStart; // e.g. "/*"
        QString blockCommentEnd;   // e.g. "*/"
        bool hasBlockComment = false;
        bool hasLineComment = false;
    };
    CommentStyle _commentStyle;

    // Qt regex for number detection
    QRegularExpression _numberRegex;

    // Case sensitivity
    Qt::CaseSensitivity _caseSensitivity = Qt::CaseSensitive;

    // Track "in comment" state across styleText calls
    bool _inBlockComment = false;
    int _blockCommentStartPos = 0;
    int _blockCommentEndPos = 0;
};
