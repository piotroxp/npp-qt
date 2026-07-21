// SyntaxHighlighterImpl.cpp — Separately-compiled implementations for methods that
// g++ silently discards when defined in the main TU (SyntaxHighlighter.cpp).
// Copyright (C) 2026 Agent Army — GPL v3

#include "SyntaxHighlighter.h"
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QRegularExpression>

// =====================================================================
// Core rule management
// =====================================================================

void SyntaxHighlighter::addRule(const QString& regex, const QTextCharFormat& format, int captureGroup) {
    HighlightRule rule;
    rule.pattern = QRegularExpression(regex);
    rule.format = format;
    rule.captureGroup = captureGroup;
    rule.enabled = true;
    _rules.append(rule);
}

void SyntaxHighlighter::clearRules() {
    _rules.clear();
}

void SyntaxHighlighter::setRules(const QVector<HighlightRule>& rules) {
    _rules = rules;
}

// =====================================================================
// Format helpers
// =====================================================================

void SyntaxHighlighter::applyFormat(int start, int length, const QTextCharFormat& format) {
    QTextCharFormat fmtCopy = format;
    setFormat(start, length, fmtCopy);
}

void SyntaxHighlighter::applyFormatRegex(const QString& text, const QRegularExpression& rx,
                                          const QTextCharFormat& format, int captureGroup) {
    QRegularExpressionMatchIterator it = rx.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        int start = m.capturedStart(captureGroup);
        int len = m.capturedLength(captureGroup);
        if (len > 0) {
            applyFormat(start, len, format);
        }
    }
}

// =====================================================================
// Keyword management
// =====================================================================

void SyntaxHighlighter::setKeywords(const QStringList& keywords) {
    _keywordSets[0] = keywords;
}

void SyntaxHighlighter::setKeywords(int setIndex, const QStringList& keywords) {
    _keywordSets[setIndex] = keywords;
}

void SyntaxHighlighter::addKeyword(int setIndex, const QString& keyword) {
    _keywordSets[setIndex].append(keyword);
}

// =====================================================================
// Comment configuration
// =====================================================================

void SyntaxHighlighter::setCommentStart(const QString& start) {
    _commentStart = start;
}

void SyntaxHighlighter::setCommentEnd(const QString& end) {
    _commentEnd = end;
}

void SyntaxHighlighter::setSingleLineComment(const QString& comment) {
    _singleLineComment = comment;
}

void SyntaxHighlighter::setTripleQuote(const QString& tripleSingle, const QString& tripleDouble) {
    _tripleQuoteSingle = tripleSingle;
    _tripleQuoteDouble = tripleDouble;
}

// =====================================================================
// UDL support
// =====================================================================

void SyntaxHighlighter::setUDLPatterns(const QVector<QRegularExpression>& patterns,
                                        const QVector<QTextCharFormat>& formats) {
    for (int i = 0; i < patterns.size() && i < formats.size(); ++i) {
        HighlightRule rule;
        rule.pattern = patterns[i];
        rule.format = formats[i];
        rule.captureGroup = 0;
        rule.enabled = true;
        _rules.append(rule);
    }
}

// =====================================================================
// Language / theme
// =====================================================================

void SyntaxHighlighter::setTheme(const QString& themeName) {
    _themeName = themeName;
    if (themeName == "dark") {
        applyDarkTheme();
    } else if (themeName == "dracula") {
        applyDraculaTheme();
    } else if (themeName == "monokai") {
        applyMonokaiTheme();
    } else if (themeName == "nord") {
        applyNordTheme();
    } else if (themeName == "solarized-dark") {
        applySolarizedDarkTheme();
    } else if (themeName == "one-dark") {
        applyOneDarkTheme();
    } else if (themeName == "light") {
        applyLightTheme();
    } else {
        applyDarkTheme();
    }
}

void SyntaxHighlighter::setThemeColors(const QString& foreground, const QString& background,
                                         const QString& keyword, const QString& string,
                                         const QString& comment, const QString& number,
                                         const QString& type, const QString& preprocessor) {
    QColor kw(keyword);
    QColor str(string);
    QColor cmt(comment);
    QColor num(number);
    QColor tp(type);
    QColor pre(preprocessor);

    if (kw.isValid()) {
        _keywordFormat.setForeground(kw);
        _keywordFormat.setFontWeight(QFont::Bold);
    }
    if (str.isValid()) { _stringFormat.setForeground(str); }
    if (cmt.isValid()) {
        _commentFormat.setForeground(cmt);
        _commentFormat.setFontItalic(true);
    }
    if (num.isValid()) { _numberFormat.setForeground(num); }
    if (tp.isValid()) {
        _typeFormat.setForeground(tp);
        _typeFormat.setFontWeight(QFont::Bold);
    }
    if (pre.isValid()) { _preprocessorFormat.setForeground(pre); }
    Q_UNUSED(foreground);
    Q_UNUSED(background);
}

// =====================================================================
// Content-based language detection
// =====================================================================

void SyntaxHighlighter::autoDetect(const QString& text) {
    Q_UNUSED(text);
    // Content-based detection: extend with heuristics per language
}

// =====================================================================
// Embedded language support
// =====================================================================

void SyntaxHighlighter::setEmbeddedLanguage(int start, int end, LangType embeddedLang) {
    EmbeddedRegion region;
    region.start = start;
    region.end = end;
    region.lang = embeddedLang;
    _embeddedRegions.append(region);
}

// =====================================================================
// State machine (multiline comment/string support)
// =====================================================================

int SyntaxHighlighter::getStateForNextBlock() const {
    // Returns the state to pass to the next block's setState()
    // 0 = normal, positive = inside comment, negative = inside string
    int state = 0;
    if (_inBlockComment) state = 1;
    if (_inString) state = -1;
    return state;
}

// =====================================================================
// Language setup methods (moved from main TU to avoid g++ silent-discard)
// =====================================================================

void SyntaxHighlighter::setupXml() {
    _singleLineComment = "//";
    _commentStart = "<!--";
    _commentEnd = "-->";
    HighlightRule rule;

    // Tags
    rule = {};
    rule.pattern = QRegularExpression(R"(<\/?[a-zA-Z_:][a-zA-Z0-9:_.\-]*)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attributes
    rule = {};
    rule.pattern = QRegularExpression(R"(\b[a-zA-Z_:][a-zA-Z0-9:_.\-]*(?=\s*=))");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attribute values
    rule = {};
    rule.pattern = QRegularExpression(R"%((?:=\s*)"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')%");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // XML Declaration
    rule = {};
    rule.pattern = QRegularExpression(R"(<\?xml[^?]*\?>)");
    rule.format = _preprocessorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // DOCTYPE
    rule = {};
    rule.pattern = QRegularExpression(R"(<!DOCTYPE[^>]*>)");
    rule.format = _preprocessorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // CDATA
    rule = {};
    rule.pattern = QRegularExpression(R"(<!\[CDATA\[[\s\S]*?\]\]>)");
    rule.format = _commentFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Comments
    rule = {};
    rule.pattern = QRegularExpression(R"(<!--[\s\S]*?-->)");
    rule.format = _commentFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Processing instructions
    rule = {};
    rule.pattern = QRegularExpression(R"(<\?[^?]*\?>)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);
}

void SyntaxHighlighter::setupMakefile() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Targets
    rule = {};
    rule.pattern = QRegularExpression(R"(^[A-Za-z0-9_.\-]+(?=\s*:)|^\t+[A-Za-z0-9_.\-]+(?=\s*\())");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Variables
    rule = {};
    rule.pattern = QRegularExpression(R"(\$[A-Za-z_][A-Za-z0-9_]*|\$\{[A-Za-z_][A-Za-z0-9_]*\}|\$\([A-Za-z_][A-Za-z0-9_]*\))");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Automatic variables
    rule = {};
    rule.pattern = QRegularExpression(R"%(\$@|\$<|\$^\|\$+|\$\?|\$\*|\$|)%");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Keywords
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(ifndef|ifdef|ifeq|ifneq|else|endif|define|endef|override|export|unexport|include|-include|sinclude|vpath)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Functions
    rule = {};
    rule.pattern = QRegularExpression(R"(\$[A-Za-z][A-Za-z0-9_-]*\()");
    rule.format = _builtInFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(R"(\b[0-9][0-9_]*\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Line continuation
    rule = {};
    rule.pattern = QRegularExpression(R"(\\$)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

void SyntaxHighlighter::setupDockerfile() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Instructions
    rule = {};
    rule.pattern = QRegularExpression(
        R"(^(FROM|RUN|CMD|LABEL|EXPOSE|ENV|ADD|COPY|ENTRYPOINT|VOLUME|USER|WORKDIR|ARG|MAINTAINER|ONBUILD|STOPSIGNAL|HEALTHCHECK|SHELL)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Environment variables
    rule = {};
    rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*|\$\{[a-zA-Z_][a-zA-Z0-9_]*\})");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Escape directive
    rule = {};
    rule.pattern = QRegularExpression(R"(^#\s*escape\s*=\s*[\`\\])");
    rule.format = _preprocessorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Paths
    rule = {};
    rule.pattern = QRegularExpression(R"(\/[a-zA-Z0-9_.\-]+)+|\.[a-zA-Z0-9_.\-]+");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Ports
    rule = {};
    rule.pattern = QRegularExpression(R"(\b[0-9]{1,5}\b(?:\s*:\s*[0-9]{1,5})?)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Flags
    rule = {};
    rule.pattern = QRegularExpression(R"(--[a-zA-Z][a-zA-Z0-9_-]*)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(R"(\b[0-9]+\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Operators
    rule = {};
    rule.pattern = QRegularExpression(R"([=\[\]|])");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

void SyntaxHighlighter::setupPowerShell() {
    _singleLineComment = "#";
    _commentStart = "<#";
    _commentEnd = "#>";
    HighlightRule rule;

    // Keywords
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(Begin|Break|Catch|Class|Continue|Data|Define|Do|DynamicParam|Else|ElseIf|End|Exit|Filter|Finally|For|ForEach|Function|If|In|InlineScript|Parallel|Param|Process|Return|Sequence|Static|Switch|Throw|Trap|Try|Until|While|Workflow)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Variables
    rule = {};
    rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*|\$\{[^}]+\}|\$[0-9]+|\$_(?:\.[a-zA-Z_][a-zA-Z0-9_]*)+)");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Built-in cmdlets
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(Get-Content|Set-Content|Add-Content|Get-Item|Set-Item|New-Item|Remove-Item|"
        R"(Get-ChildItem|Get-Date|Write-Host|Write-Output|Out-File|Import-Module|"
        R"(ForEach-Object|Where-Object|Select-Object|Sort-Object|Compare-Object|"
        R"(ConvertTo-Json|ConvertFrom-Json|Invoke-RestMethod|Invoke-WebRequest|"
        R"(Start-Process|Stop-Process|Get-Process|Test-Path|Join-Path|Split-Path|"
        R"(Get-Variable|Set-Variable|New-Variable|Remove-Variable)\b)");
    rule.format = _builtInFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Comparison operators
    rule = {};
    rule.pattern = QRegularExpression(
        R"(-eq|-ne|-gt|-lt|-ge|-le|-match|-notmatch|-like|-notlike|-in|-notin|-contains|-notcontains)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("@[\s\S]*?"@|"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Type accelerators
    rule = {};
    rule.pattern = QRegularExpression(R"(\[[a-zA-Z_][a-zA-Z0-9_.]*\])");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attributes
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\[CmdletBinding\]|\[Parameter\([^\]]*\)\]|\[ValidateSet\([^\]]*\)\]|\[OutputType\]|\[Alias\])");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Function definitions
    rule = {};
    rule.pattern = QRegularExpression(R"(\bfunction\s+([a-zA-Z_][a-zA-Z0-9_-]*))");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Operators
    rule = {};
    rule.pattern = QRegularExpression(
        R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||->|-replace|-match|-split|-join|>>|<<|>>|<<|\|>|&|&&|\|\||\?)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

void SyntaxHighlighter::setupTcl() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(after|append|apply|array|bell|bind|binary|break|case|catch|cd|clock|close|concat|continue|error|eval|exec|exit|expr|fblocked|fconfigure|fcopy|file|fileevent|flush|for|foreach|format|gets|glob|global|history|http|if|incr|info|interp|join|lappend|library|lindex|linsert|list|llength|load|loadTk|lrange|lreplace|lsearch|lset|lsort|memory|msgcat|namespace|open|package|parray|pid|proc|puts|pwd|read|regexp|registry|regsub|rename|return|scan|seek|set|source|split|string|subst|switch|tcl_findLibrary|tell|time|trace|unknown|unset|update|uplevel|upvar|variable|vwait|while|zlib)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Built-in commands
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(puts|gets|set|unset|incr|append|lappend|lindex|linsert|llength|lrange|lreplace|lsearch|lset|lsort|concat|join|split|string|subst|format|scan|regexp|regsub|switch|list|lrange|dict|lmap|array|upvar|global|variable|namespace|proc|lambda|tailcall|return|break|continue|error|try|throw|finally|catch)\b)");
    rule.format = _builtInFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Variables
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\$[a-zA-Z_][a-zA-Z0-9_]*(?:\([^)]*\))?(?:\([a-zA-Z0-9_]+\))?|\$\{[a-zA-Z_][a-zA-Z0-9_]*\})");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Command substitution
    rule = {};
    rule.pattern = QRegularExpression(R"(\[[^\[\]]*\])");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\{(?:[^{}]|\{[^{}]*\})*\}|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Operators
    rule = {};
    rule.pattern = QRegularExpression(
        R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||eq|ne|lt|gt|le|ge|in|ni|and|or|not|xor)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupCss — CSS3 highlighting
// =====================================================================
void SyntaxHighlighter::setupCss() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";
    HighlightRule rule;

    // Keywords/At-rules
    rule = {};
    rule.pattern = QRegularExpression(
        R"(@(import|media|keyframes|font-face|supports|charset|namespace|page|viewport|property|document|layer|container|container-type|container-name|all|and|not|or|only|selector|font-feature-values))\b|"
        R"(\{\s*\})");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Selectors
    rule = {};
    rule.pattern = QRegularExpression(R"(\.[a-zA-Z_][a-zA-Z0-9_\-]*)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);
    rule = {};
    rule.pattern = QRegularExpression(R"([a-zA-Z][a-zA-Z0-9_\-]*(?=\s*[,{:]))");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);
    rule = {};
    rule.pattern = QRegularExpression(R"(:[a-zA-Z_][a-zA-Z0-9_\-]*(?=\s*[,{:;)]))");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Properties
    rule = {};
    rule.pattern = QRegularExpression(R"([a-zA-Z][a-zA-Z0-9\-]*(?=\s*:))");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Property values
    rule = {};
    rule.pattern = QRegularExpression(R"(:\s*[^;{}\n]+)");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("[^"]*"|'[^']*')");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers with units
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b[0-9]+(?:\.[0-9]+)?(?:px|em|rem|%|vh|vw|pt|cm|mm|in|deg|s|ms|kHz|MHz|dpi|dpcm|dppx|ex|ch|lh|rlh|vmin|vmax|fr)?)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Hex colors
    rule = {};
    rule.pattern = QRegularExpression(R"(#[0-9a-fA-F]{3,8})\b");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Functions
    rule = {};
    rule.pattern = QRegularExpression(R"([a-zA-Z_$][a-zA-Z0-9_$]*(?=\())");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupCMake — CMakeLists.txt highlighting
// =====================================================================
void SyntaxHighlighter::setupCMake() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Commands
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(add_custom_command|add_custom_target|add_definitions|add_dependencies|add_executable|add_library|add_subdirectory|add_test|aux_source_directory|build_command|cmake_host_system_information|cmake_minimum_required|cmake_parse_arguments|configure_file|create_test_sourcelist|define_property|enable_language|enable_testing|endforeach|endfunction|endif|endmacro|endwhile|execute_process|export|file|function|get_cmake_property|get_directory_property|get_filename_component|get_property|get_source_file_property|get_target_property|get_test_property|include|include_directories|install|link_directories|link_libraries|list|macro|make_directory|mark_as_advanced|math|message|option|project|remove|set|set_property|set_target_properties|set_tests_properties|source_group|string|subdirs|target_compile_definitions|target_compile_features|target_compile_options|target_include_directories|target_link_directories|target_link_libraries|target_sources|try_compile|try_run|unset|variable_watch|while|write_file)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Variables
    rule = {};
    rule.pattern = QRegularExpression(R"(\$\{?[A-Za-z_][A-Za-z0-9_]*\}?)");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Generator expressions
    rule = {};
    rule.pattern = QRegularExpression(R"(\$<[^>]+>)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Paths
    rule = {};
    rule.pattern = QRegularExpression(R"([A-Za-z]:\\[^\s]+|\/[^\s]+)");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+)\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Operators
    rule = {};
    rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^:]+|==|!=|<=|>=|\${|}|\$|<|>)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupJson — JSON highlighting
// =====================================================================
void SyntaxHighlighter::setupJson() {
    HighlightRule rule;

    // Strings (keys and values)
    rule = {};
    rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b-?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|-?[0-9]+[eE][+-]?[0-9]+|-?[0-9]+)\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Booleans and null
    rule = {};
    rule.pattern = QRegularExpression(R"(\b(true|false|null)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Brackets and punctuation
    rule = {};
    rule.pattern = QRegularExpression(R"([{}\[\]:,])");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupRegex — Regex pattern highlighting
// =====================================================================
void SyntaxHighlighter::setupRegex() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Character classes [...]
    rule = {};
    rule.pattern = QRegularExpression(R"(\[[\^]?(?:[^\]\\]|\\.)*\])");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Quantifiers
    rule = {};
    rule.pattern = QRegularExpression(R"([*+?]|\{[0-9,]+\})\??");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Anchors
    rule = {};
    rule.pattern = QRegularExpression(R"([\^$]|\\[bBAZzG])");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Groups (...)
    rule = {};
    rule.pattern = QRegularExpression(R"(\([?]:?[><!=]?[^)]*\))");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Alternation
    rule = {};
    rule.pattern = QRegularExpression(R"(\|)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Backreferences
    rule = {};
    rule.pattern = QRegularExpression(R"(\\\[0-9]+\b)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Escape sequences
    rule = {};
    rule.pattern = QRegularExpression(R"(\\.)");
    rule.format = _preprocessorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Literal text fallback
    rule = {};
    rule.pattern = QRegularExpression(R"([a-zA-Z0-9_]+)");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupAda — Ada/SPARK highlighting
// =====================================================================
void SyntaxHighlighter::setupAda() {
    _singleLineComment = "--";
    _commentStart = "/*";
    _commentEnd = "*/";
    HighlightRule rule;

    // Keywords
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(abort|abs|abstract|accept|access|aliased|all|and|array|at|begin|body|case|constant|declare|delay|delta|digits|do|else|elsif|end|entry|exit|for|function|generic|goto|if|in|interface|is|limited|loop|mod|new|not|null|of|or|others|out|overriding|package|pragma|private|procedure|protected|raise|range|record|rem|renames|requeue|return|reverse|select|separate|subtype|synchronized|tagged|task|terminate|then|type|until|use|when|while|with|xor)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Types
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(Boolean|Character|String|Integer|Natural|Positive|Float|Duration|Text_IO|Ada|Standard)\b)");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attributes
    rule = {};
    rule.pattern = QRegularExpression(R"('[A-Za-z_][A-Za-z0-9_]*)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Strings
    rule = {};
    rule.pattern = QRegularExpression(R"("[^"]*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b[0-9][0-9_]*#?[0-9a-fA-F_]+#?([eE][+-]?[0-9_]+)?|\b[0-9]+\.[0-9_]+([eE][+-]?[0-9_]+)?\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Operators
    rule = {};
    rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^:]+|=>|&|'|@)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);
}

// =====================================================================
// setupGraphviz — Graphviz DOT language highlighting
// =====================================================================
void SyntaxHighlighter::setupGraphviz() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";
    HighlightRule rule;

    // Keywords / node statements
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(digraph|graph|subgraph|node|edge|strict)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Directions
    rule = {};
    rule.pattern = QRegularExpression(R"(\b(directed|undirected|forward|back|both|none)\b)");
    rule.format = _keywordFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Node IDs
    rule = {};
    rule.pattern = QRegularExpression(R"([a-zA-Z_][a-zA-Z0-9_]*(?=\s*[;\[\],=:\-\>])|"[^\"]+"(?=\s*[;\[\],=:\-\>]))");
    rule.format = _typeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Edge operators
    rule = {};
    rule.pattern = QRegularExpression(R"(--|->)");
    rule.format = _operatorFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attributes in [...]
    rule = {};
    rule.pattern = QRegularExpression(R"(\[|\])");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attribute names
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(label|shape|style|color|fontname|fontsize|fillcolor|bgcolor|tooltip|url|penwidth|arrowhead|arrowtail|dir|constraint|len|weight|area|peripheries|sides|orientation|margin|padding|width|height|fixedsize|scale|image|latexmode|root|compound|cluster|rank|rankdir|ranksep|nodesep|node|edge|graph|minlen)\b(?=\s*=))");
    rule.format = _functionFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Attribute values (strings)
    rule = {};
    rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat;
    rule.enabled = true;
    _rules.append(rule);

    // HTML-like labels
    rule = {};
    rule.pattern = QRegularExpression(R"(<[^>]+>)");
    rule.format = _attributeFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Numbers
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b[0-9]+\.[0-9_]+|[0-9_]+\b)");
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);

    // Colors (named)
    rule = {};
    rule.pattern = QRegularExpression(
        R"(\b(black|white|red|green|blue|yellow|cyan|magenta|gray|grey|orange|purple|brown|pink|aqua|navy|olive|maroon|teal|silver|lime|olive|aquamarine|chartreuse|coral|cornflowerblue|cornsilk|crimson|darkblue|darkgreen|darkorange|firebrick|floralwhite|forestgreen|gainsboro|ghostwhite|gold|goldenrod|greenyellow|honeydew|hotpink|indianred|indigo|ivory|khaki|lavender|lavenderblush|lawngreen|lemonchiffon|lightblue|lightcoral|lightcyan|lightgoldenrodyellow|lightgray|lightgrey|lightgreen|lightpink|lightsalmon|lightseagreen|lightskyblue|lightslategray|lightslategrey|lightsteelblue|lightyellow|limegreen|linen|magenta|mediumaquamarine|mediumblue|mediumorchid|mediumpurple|mediumseagreen|mediumslateblue|mediumspringgreen|mediumturquoise|mediumvioletred|mintcream|mistyrose|moccasin|navajowhite|oldlace|olivedrab|orangered|orchid|palegoldenrod|palegreen|paleturquoise|palevioletred|papayawhip|peachpuff|peru|plum|powderblue|rosybrown|royalblue|saddlebrown|salmon|sandybrown|seagreen|seashell|sienna|skyblue|slateblue|slategray|slategrey|snow|springgreen|steelblue|tan|thistle|tomato|turquoise|violet|wheat|whitesmoke|yellowgreen)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _numberFormat;
    rule.enabled = true;
    _rules.append(rule);
}
