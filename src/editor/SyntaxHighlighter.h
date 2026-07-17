// SyntaxHighlighter.h - Comprehensive syntax highlighting for all supported languages
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include <QStringList>
#include <QRegularExpression>
#include <QHash>
#include "../common/Types.h"

// ============================================================================
// HighlightRule — a single pattern → format rule
// ============================================================================
struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
    int captureGroup = 0;   // 0 = whole match, 1+ = capture group
    bool enabled = true;
};

// ============================================================================
// HighlightBlockState — per-line state machine state
// ============================================================================
struct HighlightBlockState {
    int prevState = 0;    // Lexer/sub-lexer state from previous line
    int braceDepth = 0;   // For brace-matching highlights
    int commentDepth = 0; // For nested comment tracking
};

// ============================================================================
// SyntaxHighlighter — comprehensive syntax highlighter
// ============================================================================
class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument* parent = nullptr);
    ~SyntaxHighlighter() override;

    // ---- Language / theme control ----

    void setLanguage(LangType lang);
    LangType language() const { return _language; }

    void setupLanguage(LangType lang);

    void setTheme(const QString& themeName);
    void applyDarkTheme();
    void applyDraculaTheme();
    void applyLightTheme();
    void applyMonokaiTheme();
    void applyNordTheme();
    void applyOneDarkTheme();
    void applySolarizedDarkTheme();
    QString currentTheme() const { return _themeName; }

    void setThemeColors(const QString& foreground, const QString& background,
                        const QString& keyword, const QString& string,
                        const QString& comment, const QString& number,
                        const QString& type, const QString& preprocessor);

    // ---- Keyword management ----

    void setKeywords(const QStringList& keywords);
    void setKeywords(int setIndex, const QStringList& keywords);
    void addKeyword(int setIndex, const QString& keyword);

    // ---- Comment configuration ----

    void setCommentStart(const QString& start);
    void setCommentEnd(const QString& end);
    void setSingleLineComment(const QString& comment);

    /// Set triple-quote strings (Python docstrings, etc.)
    void setTripleQuote(const QString& tripleSingle, const QString& tripleDouble);

    // ---- Custom rules ----

    void addRule(const QString& regex, const QTextCharFormat& format,
                 int captureGroup = 0);
    void clearRules();
    void setRules(const QVector<HighlightRule>& rules);

    // ---- UDL support ----

    void setUDLPatterns(const QVector<QRegularExpression>& patterns,
                        const QVector<QTextCharFormat>& formats);

    // ---- Multi-language / embedded language support ----

    /// Detect and switch language based on content
    void autoDetect(const QString& text);

    /// Set a nested language region (e.g., PHP inside HTML)
    void setEmbeddedLanguage(int start, int end, LangType embeddedLang);

    // ---- State queries ----

    bool isInComment() const { return _inBlockComment; }
    bool isInString() const { return _inString; }

signals:
    void languageChanged(LangType newLang);

protected:
    void highlightBlock(const QString& text) override;
    int highlightBlockWithState(const QString& text, int prevState);

private:
    // Language setup methods
    void setupDefault();
    void setupCpp();
    void setupPython();
    void setupHtml();
    void setupJson();
    void setupYaml();
    void setupMarkdown();
    void setupBatch();
    void setupMakefile();
    void setupSql();
    void setupLua();
    void setupRuby();
    void setupPerl();
    void setupCss();
    void setupXml();
    void setupTex();
    void setupDiff();
    void setupR();
    void setupJavaScript();
    void setupJava();
    void setupCSharp();
    void setupObjectiveC();
    void setupGo();
    void setupRust();
    void setupSwift();
    void setupKotlin();
    void setupTypeScript();
    void setupPhp();
    void setupPascal();
    void setupFortran();
    void setupLisp();
    void setupHaskell();
    void setupAda();
    void setupRegex();
    void setupGraphviz();
    void setupVerilog();
    void setupVHDL();
    void setupAsm();
    void setupTcl();
    void setupBash();
    void setupPowerShell();
    void setupNsis();
    void setupIni();
    void setupDockerfile();
    void setupCMake();
    void setupJulia();
    void setupMatlab();
    void setupC();
    void setupGroovy();
    void setupScala();
    void setupErlang();
    void setupElixir();
    void setupFSharp();
    void setupClojure();
    void setupNfo();
    void setupProperties();
    void setupWebAssembly();

    // Utility methods
    void applyFormat(int start, int length, const QTextCharFormat& format);
    void applyFormatRegex(const QString& text, const QRegularExpression& rx,
                          const QTextCharFormat& format, int captureGroup = 0);
    int getStateForNextBlock() const;

    // Per-language format objects
    QTextCharFormat _keywordFormat;
    QTextCharFormat _stringFormat;
    QTextCharFormat _commentFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _typeFormat;
    QTextCharFormat _preprocessorFormat;
    QTextCharFormat _functionFormat;
    QTextCharFormat _attributeFormat;
    QTextCharFormat _labelFormat;
    QTextCharFormat _operatorFormat;
    QTextCharFormat _builtInFormat;

    QString _commentStart;
    QString _commentEnd;
    QString _singleLineComment;
    QString _tripleQuoteSingle;
    QString _tripleQuoteDouble;

    LangType _language = LangType::L_TEXT;
    QString _themeName = "default";
    QVector<HighlightRule> _rules;

    // Multi-keyword sets (for different keyword sets per language)
    QHash<int, QStringList> _keywordSets;

    // Block comment / string state
    bool _inBlockComment = false;
    bool _inString = false;
    QString _stringQuote;
    int _stringContinueFrom = -1;

    // Embedded language regions
    struct EmbeddedRegion { int start; int end; LangType lang; };
    QVector<EmbeddedRegion> _embeddedRegions;
    int _embeddedStart = -1;
    LangType _embeddedLang = LangType::L_TEXT;
};
