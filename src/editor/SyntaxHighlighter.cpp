// SyntaxHighlighter.cpp — Full 50-language syntax highlighting implementation
// Copyright (C) 2026 Agent Army — GPL v3

#include "SyntaxHighlighter.h"
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QRegularExpression>
#include <QDebug>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
}

SyntaxHighlighter::~SyntaxHighlighter() = default;

// =====================================================================
// THEMES
// =====================================================================

void SyntaxHighlighter::applyDarkTheme() {
    _keywordFormat.setForeground(QColor("#569CD6"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#4EC9B0"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#9CDCFE"));
    _stringFormat.setForeground(QColor("#CE9178"));
    _commentFormat.setForeground(QColor("#6A9955"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#B5CEA8"));
    _functionFormat.setForeground(QColor("#DCDCAA"));
    _preprocessorFormat.setForeground(QColor("#C586C0"));
    _attributeFormat.setForeground(QColor("#9CDCFE"));
    _labelFormat.setForeground(QColor("#D7BA7D"));
    _operatorFormat.setForeground(QColor("#D4D4D4");
}

void SyntaxHighlighter::applyDraculaTheme() {
    _keywordFormat.setForeground(QColor("#FF79C6"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#8BE9FD"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#50FA7B"));
    _stringFormat.setForeground(QColor("#F1FA8C"));
    _commentFormat.setForeground(QColor("#6272A4"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#BD93F9"));
    _functionFormat.setForeground(QColor("#50FA7B"));
    _preprocessorFormat.setForeground(QColor("#FFB86C"));
    _attributeFormat.setForeground(QColor("#F1FA8C"));
    _labelFormat.setForeground(QColor("#FFB86C"));
    _operatorFormat.setForeground(QColor("#F8F8F2"));
}

void SyntaxHighlighter::applyLightTheme() {
    _keywordFormat.setForeground(QColor("#0000FF"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#267F99"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#0000FF"));
    _stringFormat.setForeground(QColor("#A31515"));
    _commentFormat.setForeground(QColor("#008000"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#098658"));
    _functionFormat.setForeground(QColor("#795E26"));
    _preprocessorFormat.setForeground(QColor("#9B2393"));
    _attributeFormat.setForeground(QColor("#74531F"));
    _labelFormat.setForeground(QColor("#795E26"));
    _operatorFormat.setForeground(QColor("#000000"));
}

void SyntaxHighlighter::applyMonokaiTheme() {
    _keywordFormat.setForeground(QColor("#F92672"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#66D9EF"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#AE81FF"));
    _stringFormat.setForeground(QColor("#E6DB74"));
    _commentFormat.setForeground(QColor("#75715E"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#AE81FF"));
    _functionFormat.setForeground(QColor("#A6E22E"));
    _preprocessorFormat.setForeground(QColor("#F92672"));
    _attributeFormat.setForeground(QColor("#A6E22E"));
    _labelFormat.setForeground(QColor("#E6DB74"));
    _operatorFormat.setForeground(QColor("#F92672"));
}

void SyntaxHighlighter::applyNordTheme() {
    _keywordFormat.setForeground(QColor("#81A1C1"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#8FBCBB"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#88C0D0"));
    _stringFormat.setForeground(QColor("#A3BE8C"));
    _commentFormat.setForeground(QColor("#616E88"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#B48EAD"));
    _functionFormat.setForeground(QColor("#88C0D0"));
    _preprocessorFormat.setForeground(QColor("#B48EAD"));
    _attributeFormat.setForeground(QColor("#EBCB8B"));
    _labelFormat.setForeground(QColor("#EBCB8B"));
    _operatorFormat.setForeground(QColor("#D8DEE9"));
}

void SyntaxHighlighter::applyOneDarkTheme() {
    _keywordFormat.setForeground(QColor("#C678DD"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#E5C07B"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#56B6C2"));
    _stringFormat.setForeground(QColor("#98C379"));
    _commentFormat.setForeground(QColor("#5C6370"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#D19A66"));
    _functionFormat.setForeground(QColor("#61AFEF"));
    _preprocessorFormat.setForeground(QColor("#C678DD"));
    _attributeFormat.setForeground(QColor("#E5C07B"));
    _labelFormat.setForeground(QColor("#D19A66"));
    _operatorFormat.setForeground(QColor("#ABB2BF"));
}

void SyntaxHighlighter::applySolarizedDarkTheme() {
    _keywordFormat.setForeground(QColor("#859900"));
    _keywordFormat.setFontWeight(QFont::Bold);
    _typeFormat.setForeground(QColor("#268BD2"));
    _typeFormat.setFontWeight(QFont::Bold);
    _builtInFormat.setForeground(QColor("#2AA198"));
    _stringFormat.setForeground(QColor("#2AA198"));
    _commentFormat.setForeground(QColor("#586E75"));
    _commentFormat.setFontItalic(true);
    _numberFormat.setForeground(QColor("#D33682"));
    _functionFormat.setForeground(QColor("#268BD2"));
    _preprocessorFormat.setForeground(QColor("#CB4B16"));
    _attributeFormat.setForeground(QColor("#B58900"));
    _labelFormat.setForeground(QColor("#B58900"));
    _operatorFormat.setForeground(QColor("#839496"));
}

// =====================================================================
// DISPATCHER
// =====================================================================

void SyntaxHighlighter::setupLanguage(LangType lang) {
    _rules.clear();
    _commentStart.clear();
    _commentEnd.clear();
    _singleLineComment.clear();
    _tripleQuoteSingle.clear();
    _tripleQuoteDouble.clear();
    _keywordSets.clear();
    _inBlockComment = false;
    _inString = false;
    _embeddedRegions.clear();
    _embeddedStart = -1;
    _embeddedLang = LangType::L_TEXT;

    switch (lang) {
        case LangType::L_TEXT:       setupText();       break;
        case LangType::L_C:          setupC();          break;
        case LangType::L_CPP:        setupCpp();        break;
        case LangType::L_OBJC:       setupObjectiveC(); break;
        case LangType::L_CS:         setupCSharp();     break;
        case LangType::L_JAVA:       setupJava();       break;
        case LangType::L_PYTHON:     setupPython();     break;
        case LangType::L_RUBY:       setupRuby();       break;
        case LangType::L_PERL:       setupPerl();       break;
        case LangType::L_PHP:        setupPhp();        break;
        case LangType::L_HTML:       setupHtml();       break;
        case LangType::L_XML:        setupXml();        break;
        case LangType::L_JSON:       setupJson();       break;
        case LangType::L_YAML:       setupYaml();       break;
        case LangType::L_MARKDOWN:   setupMarkdown();   break;
        case LangType::L_CSS:        setupCss();        break;
        case LangType::L_LUA:        setupLua();        break;
        case LangType::L_GO:         setupGo();         break;
        case LangType::L_RUST:       setupRust();       break;
        case LangType::L_SWIFT:      setupSwift();      break;
        case LangType::L_KOTLIN:     setupKotlin();    break;
        case LangType::L_SCALA:      setupScala();      break;
        case LangType::L_HASKELL:    setupHaskell();    break;
        case LangType::L_ERLANG:     setupErlang();     break;
        case LangType::L_ELIXIR:     setupElixir();     break;
        case LangType::L_CLOJURE:    setupClojure();    break;
        case LangType::L_FSHARP:     setupFSharp();     break;
        case LangType::L_LISP:       setupLisp();       break;
        case LangType::L_TEX:        setupTex();        break;
        case LangType::L_FORTRAN:    setupFortran();    break;
        case LangType::L_MAKEFILE:   setupMakefile();   break;
        case LangType::L_CMAKE:      setupCMake();      break;
        case LangType::L_DOCKERFILE: setupDockerfile(); break;
        case LangType::L_SQL:        setupSql();        break;
        case LangType::L_REGEX:      setupRegex();      break;
        case LangType::L_DIFF:       setupDiff();       break;
        case LangType::L_INI:        setupIni();        break;
        case LangType::L_NFO:        setupNfo();        break;
        case LangType::L_PROPERTIES: setupProperties(); break;
        case LangType::L_BATCH:      setupBatch();     break;
        case LangType::L_BASH:       setupBash();      break;
        case LangType::L_POWERSHELL: setupPowerShell(); break;
        case LangType::L_JS:         setupJavaScript(); break;
        case LangType::L_TS:         setupTypeScript(); break;
        case LangType::L_WEBASSEMBLY: setupWebAssembly(); break;
        case LangType::L_GROOVY:     setupGroovy();     break;
        case LangType::L_PASCAL:     setupPascal();     break;
        case LangType::L_ADA:        setupAda();        break;
        case LangType::L_VERILOG:    setupVerilog();    break;
        case LangType::L_VHDL:       setupVHDL();       break;
        case LangType::L_ASM:        setupAsm();        break;
        case LangType::L_GRAPHVIZ:   setupGraphviz();   break;
        case LangType::L_R:          setupR();          break;
        case LangType::L_JULIA:      setupJulia();      break;
        case LangType::L_MATLAB:     setupMatlab();     break;
        case LangType::L_NSIS:       setupNsis();       break;
        case LangType::L_NSIS2:      setupNsis();      break;
        case LangType::L_TCL:        setupTcl();        break;
        default:                     setupDefault();    break;
    }
}

// =====================================================================
// LANGUAGE SETUP METHODS
// =====================================================================


// -------------------------------------------------------------------------
// C / C++ / Objective-C family
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupC() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Preprocessor
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*#\w+)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while|_Alignas|_Alignof|_Atomic|_Bool|_Complex|_Generic|_Imaginary|_Noreturn|_Static_assert|_Thread_local|restrict)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|size_t|ssize_t|ptrdiff_t|intptr_t|uintptr_t|FILE|DIR|bool|char16_t|char32_t|wchar_t|nullptr_t)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F]+[uUlL]*|0[0-7]+[uUlL]*|0[bB][01]+[uUlL]*|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]*|[0-9]+[eE][+-]?[0-9]+[fFlL]*|[0-9]+[fFlL]|__[A-Z_]+__)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Labels
    rule = {}; rule.pattern = QRegularExpression(R"(\b[a-zA-Z_][a-zA-Z0-9_]*\s*:)");
    rule.format = _labelFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|<<=|>>=|->|\+\+|--|&&|\|\||==|!=|<=|>=|<<|>>)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupCpp() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Preprocessor
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*#\w+)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|intptr_t|uintptr_t|size_t|ssize_t|ptrdiff_t|max_align_t|nullptr_t|string|vector|map|set|list|deque|array|forward_list|unordered_map|unordered_set|shared_ptr|unique_ptr|weak_ptr|optional|variant|any|span|string_view|tuple|pair|queue|stack|priority_queue|FILE|DIR|regex|thread|mutex|lock_guard|unique_lock|condition_variable|future|promise|atomic|locale|ios_base|type_info)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F]+[uUlL]*|0[0-7]+[uUlL]*|0[bB][01]+[uUlL]*|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]*|[0-9]+[eE][+-]?[0-9]+[fFlL]*|[0-9]+[fFlL]|__[A-Z_]+__)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Labels
    rule = {}; rule.pattern = QRegularExpression(R"(\b[a-zA-Z_][a-zA-Z0-9_]*\s*:)");
    rule.format = _labelFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|<<=|>>=|->|\+\+|--|&&|\|\||==|!=|<=|>=|<<|>>)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupObjectiveC() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Preprocessor
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*#\w+)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while|@interface|@implementation|@end|@public|@private|@protected|@property|@synthesize|@dynamic|@selector|@protocol|@class|@try|@catch|@finally|@throw|@autoreleasepool|self|super|nil|NULL|YES|NO|id|Class|SEL|IMP|BOOL|instancetype|nullable|nonnull|nonatomic|retain|release|autorelease|assign|copy|strong|weak|readonly|readwrite|getter|setter)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int|long|float|double|char|BOOL|id|Class|SEL|IMP|instancetype|NSString|NSArray|NSDictionary|NSObject|NSNumber|NSData|NSMutableArray|NSMutableDictionary|NSMutableString|IMP|SEL|id)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings (@"" and "")
    rule = {}; rule.pattern = QRegularExpression(R"(@"(?:[^"\\]|\\.)*"|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F]+|0[0-7]+|0[bB][01]+|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Selectors [selector method]
    rule = {}; rule.pattern = QRegularExpression(R"(\[[^\]]+\])");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|->|\+\+|--|&&|\|\||==|!=|<=|>=)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// C# / Java / Kotlin / Scala / Groovy
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupCSharp() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|as|base|bool|break|byte|case|catch|char|checked|class|const|continue|decimal|default|delegate|do|double|else|enum|event|explicit|extern|false|finally|fixed|float|for|foreach|goto|if|implicit|in|int|interface|internal|is|lock|long|namespace|new|null|object|operator|out|override|params|private|protected|public|readonly|ref|return|sbyte|sealed|short|sizeof|stackalloc|static|string|struct|switch|this|throw|true|try|typeof|uint|ulong|unchecked|unsafe|ushort|using|var|virtual|void|volatile|while|yield|async|await|dynamic|nameof|when|where|orderby|join|let|partial|yield|get|set|value|add|remove|implicit|explicit|operator|static|extern|virtual|override|abstract|sealed|new)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Int32|Int64|Int16|IntPtr|UInt32|UInt64|UInt16|UIntPtr|SByte|Byte|Char|Boolean|Single|Double|Decimal|String|Object|Void|Task|List|Dictionary|HashSet|Queue|Stack|IEnumerable|IEnumerator|IList|IDictionary|ICollection|IEnumerable|Action|Func|Predicate|Lazy|Nullable|DateTime|TimeSpan|Guid|Version|Array|Type|Exception|Attribute|EventHandler|Delegate|Enum|Struct)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Attributes
    rule = {}; rule.pattern = QRegularExpression(R"(\[[\w]+(?:\([^)]*\))?\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"(@"(?:[^"]|"")*"|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F]+[uUlL]*|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[mMfFlLdD]?|[0-9]+[eE][+-]?[0-9]+[fFmMlLdD]?|[0-9]+[uUlL]*)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|\.\.|==|!=|<=|>=|=>|->|\?\?|\?\.|\?\:|\+\+|--|&&|\|\||->)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupJava() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|assert|boolean|break|byte|case|catch|char|class|const|continue|default|do|double|else|enum|extends|false|final|finally|float|for|goto|if|implements|import|instanceof|int|interface|long|native|new|null|package|private|protected|public|return|short|static|strictfp|super|switch|synchronized|this|throw|throws|transient|true|try|void|volatile|while|var|yield|record|sealed|permits|non-sealed)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(String|Object|Class|Integer|Long|Short|Byte|Float|Double|Character|Boolean|Void|List|ArrayList|LinkedList|HashMap|HashSet|TreeMap|TreeSet|Queue|Deque|Stack|Map|Set|Collection|Iterable|Iterator|Scanner|Optional|Stream|BufferedReader|InputStream|OutputStream|FileInputStream|FileOutputStream|FileReader|FileWriter|PrintWriter|HttpServlet|Exception|RuntimeException|Throwable|Error|Thread|Runnable|ClassLoader|Module|System|ThreadLocal|ConcurrentHashMap|CopyOnWriteArrayList|Arrays|Collections|Objects|OptionalInt|OptionalLong|OptionalDouble|IntStream|LongStream|DoubleStream)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Annotations
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_][a-zA-Z0-9_]*(?:\([^)]*\))?)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F]+[lL]?|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fFdDlL]?|[0-9]+[eE][+-]?[0-9]+[fFdDlL]?|[0-9]+[lL]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||->|\+\+|--|\.\.|::)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupKotlin() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|actual|annotation|as|break|by|catch|class|companion|const|constructor|continue|crossinline|data|delegate|do|dynamic|else|enum|expect|extension|external|false|final|finally|for|fun|get|if|import|in|infix|init|inline|inner|interface|internal|is|lateinit|noinline|null|object|open|operator|out|override|package|private|protected|public|reified|return|sealed|set|super|suspend|tailrec|this|throw|true|try|typealias|typeof|val|value|var|vararg|when|where|while|crossinline|inline|noinline|reified|internal|Companion|Dynamic|it|field|property|receiver|setparam|destructuring)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Int|Long|Short|Byte|Float|Double|Boolean|Char|String|Unit|Nothing|Any|Array|List|Map|Set|MutableList|MutableMap|MutableSet|Pair|Triple|Iterable|Sequence|Collection|MutableCollection|ListIterator|MutableListIterator|Iterator|MutableIterator|HashMap|LinkedHashMap|HashSet|LinkedHashSet|MutableStateFlow|StateFlow|Flow|SharedFlow|Sequence|Result|Enum|Annotation|Regex|Pattern|Matcher|Throwable|Exception|RuntimeException|Error|Lambda|Function|kclass|KClass)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Annotations
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_][a-zA-Z0-9_]*(?:\([^)]*\))?)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings with interpolation
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Triple-quoted strings
    rule = {}; rule.pattern = QRegularExpression(R"("""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+[lL]?|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fFlL]?|[0-9]+[eE][+-]?[0-9_]+[fFlL]?|[0-9]+[lLuU]*)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\<[A-Za-z_])");
    rule.format = _functionFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\([^)]*\)\s*(?::|:|$|\{))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||\?\:|\?\:|\+\+|--|::|\.\.|\.\.\.|\->)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupScala() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|assert|boolean|break|byte|case|catch|char|class|const|continue|default|do|double|else|enum|extends|false|final|finally|float|for|forSome|if|implicit|import|in|instanceof|int|interface|long|native|new|null|object|override|package|private|protected|public|return|sealed|short|static|strictfp|super|switch|synchronized|this|throw|throws|transient|true|try|val|var|void|volatile|while|type|def|class|trait|object|package|import|with|new|match|given|then|lazy|inline|match|case|class|enum|sealed|final|abstract|override|private|protected|public|override)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Int|Long|Short|Byte|Float|Double|Boolean|Char|String|Unit|Nothing|Any|AnyRef|AnyVal|Null|Nil|List|Set|Map|Tuple|Option|Try|Left|Right|Success|Failure|Future|Promise|Stream|Iterator|Seq|IndexedSeq|LinearSeq|Vector|Array|Buffer|ListBuffer|ArrayBuffer|HashMap|HashSet|TreeMap|TreeSet|LinkedList|DoubleLinkedList|ListMap|ListSet|Stack|Queue|Deque|PriorityQueue|BitSet|scala\.)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Triple-quoted strings
    rule = {}; rule.pattern = QRegularExpression(R"("""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Symbols
    rule = {}; rule.pattern = QRegularExpression(R"('[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+[lL]?|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fFdDlL]?|[0-9]+[eE][+-]?[0-9_]+[fFdDlL]?|[0-9]+[lLuU]*)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_`]*)\s*(?=\(|:|$))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||::|->|<-|<=|->|=>|\+\+|--|\+\=|#@|<%|<:|>:)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupGroovy() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|as|assert|boolean|break|byte|case|catch|char|class|const|continue|def|default|do|double|else|enum|extends|false|final|finally|float|for|goto|if|implements|import|in|instanceof|int|interface|long|native|new|null|package|private|protected|public|return|short|static|strictfp|super|switch|synchronized|this|throw|throws|transient|true|try|void|volatile|while|trait|record|sealed|permits)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // GStrings and strings
    rule = {}; rule.pattern = QRegularExpression(R"(@?[a-zA-Z_][a-zA-Z0-9_]*|"(?:[^"\\]|\\.)*"|'''[\s\S]*?'''|"[\s\S]*?")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+|[0-9]+[lLgG]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\<[^>]*\>\s*\(|def\s+[a-zA-Z_][a-zA-Z0-9_]*\s*(?:\(|=|$)|[a-zA-Z_][a-zA-Z0-9_]*\s*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||->|\.\\*|\?\:|\?\.)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Scripting: Python / Ruby / Perl / PHP / Lua
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupPython() {
    _singleLineComment = "#";
    _commentStart = "\"\"\"";
    _commentEnd = "\"\"\"";
    _tripleQuoteSingle = "'''";
    _tripleQuoteDouble = "\"\"\"";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(False|None|True|and|as|assert|async|await|break|class|continue|def|del|elif|else|except|finally|for|from|global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield|self|cls)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in types/functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int|float|str|bool|list|dict|set|tuple|bytes|bytearray|memoryview|range|slice|property|classmethod|staticmethod|abstractmethod|object|type|super|print|input|len|range|enumerate|zip|map|filter|sum|min|max|sorted|reversed|open|file|abs|round|hash|id|dir|vars|globals|locals|getattr|setattr|hasattr|delattr|isinstance|issubclass|callable|repr|chr|ord|hex|oct|bin|int|float|bool|complex|eval|exec|compile|format|round|divmod|pow|all|any|iter|next|slice|classmethod|staticmethod|property|object|type|super|print|input|len|bytes|bytearray|memoryview|dict|set|frozenset|tuple|list|str|int|float|complex|bool)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Decorators
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_][a-zA-Z0-9_.]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings (single/double, f-strings)
    rule = {}; rule.pattern = QRegularExpression(R"((?:f|rf|fr)?'''[\s\S]*?'''|f"""[\s\S]*?"""|(?:f|rf|fr)?"(?:[^"\\]|\\.)*"|(?:f|rf|fr)?'[^'\\]*(?:\\.[^'\\]*)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[jJ]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bdef\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"(\basync\s+def\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Class definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bclass\s+([a-zA-Z_][a-zA-Z0-9_]*(?:\s*\([^)]*\))?))");
    rule.format = _typeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~@:]+|==|!=|<=|>=|:=|\*\*|\/\/|->|\+=|-=|\*=|\/=)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupRuby() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(__ENCODING__|__END__|__FILE__|__LINE__|BEGIN|END|alias|and|begin|break|case|class|def|defined|do|else|elsif|end|ensure|false|for|if|in|module|next|nil|not|or|redo|rescue|retry|return|self|super|then|true|undef|unless|until|when|while|yield|attr_reader|attr_writer|attr_accessor|private|protected|public|include|extend|prepend|prepare|require|require_relative|gem|Bundler|loop|proc|lambda|throw|catch|load|test|setup|teardown|describe|context|it|expect|have_received|stub|double|spy|let|before|after|around|subject|shared_examples|shared_context|include_examples|include_context|match|allow|receive|have|matcher)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Symbols
    rule = {}; rule.pattern = QRegularExpression(R"(:[a-zA-Z_][a-zA-Z0-9_?!=]*|:[A-Z][A-Za-z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(
        R"(%[qQwWiIxsr]?(?:\([^()]*\)|\{\{\}\[\]<>|\|[^|]*\||[^a-zA-Z0-9\s][^\1]*\1)|"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Heredocs
    rule = {}; rule.pattern = QRegularExpression(R"(<<[-~]?["']?(\w+)["']?\n[\s\S]*?\n\1)");
    rule.format = _stringFormat; _rules.append(rule);

    // Instance/class/global variables
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-z_][a-zA-Z0-9_]*|@@[a-zA-Z_][a-zA-Z0-9_]*|\$[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _typeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+:[0-9_]+:[0-9_]+|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bdef\s+([a-zA-Z_][a-zA-Z0-9_?!]*|[^ \t\n]+))");
    rule.format = _functionFormat; _rules.append(rule);

    // Class definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bclass\s+([A-Z][a-zA-Z0-9_]*(?:\s*<\s*[A-Z][a-zA-Z0-9_]*)?))");
    rule.format = _typeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~]+|==|!=|<=|>=|&&|\|\||\.\.|===|!==|=\=|\+@|\-@|\!|~\|&|\||\^|<<|>>|\+\+|--|\+\+|--)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupPerl() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(BEGIN|END|CHECK|INIT|UNITCHECK|autouse|bless|caller|continue|dbmopen|dbmclose|defined|delete|die|do|dump|else|elsif|eval|exec|exit|exists|exp|for|foreach|format|goto|grep|if|import|int|join|keys|kill|last|length|link|listen|local|localtime|log|lstat|map|my|next|no|open|opendir|ord|our|pack|package|pop|pos|print|printf|push|quotemeta|rand|read|readdir|readline|readpipe|recv|redo|ref|rename|require|reset|return|reverse|rewinddir|rindex|say|seek|seekdir|select|semctl|semget|semop|send|setpgrp|setpriority|setsockopt|shift|shmctl|shmget|shmread|shmwrite|shutdown|sin|sleep|socket|socketpair|sort|splice|split|sprintf|sqrt|srand|stat|state|study|sub|substr|symlink|syscall|sysopen|sysread|sysseek|system|syswrite|tell|telldir|tie|tied|time|times|truncate|uc|ucfirst|umask|undef|unlink|unpack|unshift|untie|use|utime|values|vec|wait|waitpid|wantarray|warn|while|write)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(print|say|printf|sprintf|push|pop|shift|unshift|splice|reverse|sort|grep|map|caller|chomp|chop|chr|crypt|hex|index|lc|lcfirst|length|ord|pack|q|qq|qx|substr|tr|uc|ucfirst|hex|oct|abs|atan2|cos|exp|hex|int|log|oct|sin|sqrt|srand|hex|oct|vec|unpack|each|exists|keys|values|delete|incr| Tie|Alias)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Special variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[_\$\!@#%&\*\+\-=\.\/\:\;\,\|\`\'\"\~\?\[\]\{\}\(\)\\][\d\w]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings with qq(), q(), qx()
    rule = {}; rule.pattern = QRegularExpression(R"(\b(?:qq|q|qx|qw|qr|s|y|m)\b\s*([^\w\s][^\1]*\1|[^\s]+))");
    rule.format = _stringFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Regex
    rule = {}; rule.pattern = QRegularExpression(R"(/[^/\n]+/|m[^a-zA-Z0-9\n][^\n]*[^a-zA-Z0-9\n][^\n]*|s[^a-zA-Z0-9\n][^\n]*[^a-zA-Z0-9\n][^\n]*|tr[^a-zA-Z0-9\n][^\n]*[^a-zA-Z0-9\n][^\n]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[KMG]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bsub\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~]+|==|!=|<=|>=|&&|\|\||->|//|=>|<<|>>|\.\.\.|\.\.|\|>|<\s*>)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupPhp() {
    _singleLineComment = "//";
    HighlightRule rule;

    // Alternative syntax
    rule = {}; rule.pattern = QRegularExpression(R"(\b(endif|endwhile|endfor|endforeach|endswitch|enddeclare)\s*;)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(and|or|xor|array|as|break|callable|case|catch|class|clone|const|continue|declare|default|die|do|echo|else|elseif|empty|enddeclare|endfor|endforeach|endif|endswitch|endwhile|eval|exit|extends|final|finally|fn|for|foreach|function|global|goto|if|implements|include|include_once|instanceof|insteadof|interface|isset|list|match|namespace|new|or|print|private|protected|public|require|require_once|return|static|switch|throw|trait|try|unset|use|var|while|xor|yield|yield from|abstract|class|enum|extends|implements|interface|trait|final|public|protected|private|const|static|var|abstract|final|public|protected|private|readonly|mixed|void|null|false|true|never|iterable|parent|self|static|function|class)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(strlen|strpos|str_replace|substr|explode|implode|trim|ltrim|rtrim|strtolower|strtoupper|ucfirst|lcfirst|sprintf|printf|json_encode|json_decode|file_get_contents|file_put_contents|count|in_array|array_map|array_filter|array_reduce|array_merge|array_keys|array_values|is_array|is_string|is_int|is_bool|is_null|empty|isset|unset|htmlspecialchars|htmlentities|preg_match|preg_replace|strtotime|date|time|microtime|strlen|strpos|str_replace|substr|trim|ltrim|rtrim|strtolower|strtoupper|str_starts_with|str_ends_with|str_contains|str_pad|str_repeat|wordwrap|nl2br|strip_tags|addslashes|stripslashes|ord|chr|hex2bin|bin2hex|pack|unpack|count|sizeof|array_keys|array_values|array_merge|array_push|array_pop|array_shift|array_unshift|array_slice|array_splice|array_key_exists|key_exists|in_array|sort|rsort|asort|arsort|ksort|krsort|usort|uasort|uksort|shuffle|array_rand|range|floor|ceil|round|abs|min|max|rand|mt_rand|array_sum|array_product|array_reduce|array_filter|array_map|array_reverse|current|next|prev|reset|end|each|list|compact|extract|parse_url|http_build_query|parse_str|base64_encode|base64_decode|mail|header|headers_sent|setcookie|session_start|session_destroy|set_error_handler|set_exception_handler|register_shutdown_function|filter_var|filter_input|ob_start|ob_end_clean|ob_get_clean|var_dump|var_export|print_r|debug_zval_dump|spl_autoload_register|spl_classes)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*'|\`[^\`]*\`)");
    rule.format = _stringFormat; _rules.append(rule);

    // PHP tags
    rule = {}; rule.pattern = QRegularExpression(R"(<[?]php|^\?>|<[?=])");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Attributes
    rule = {}; rule.pattern = QRegularExpression(R"(#\[[a-zA-Z_][a-zA-Z0-9_]*(?:\([^)]*\))?)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[KMGTP]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfunction\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Class definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(?:final\s+)?(?:abstract\s+)?class\s+([A-Z][a-zA-Z0-9_]*))");
    rule.format = _typeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||->|=>|::|\.\.\.|\+\+|--)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupLua() {
    _singleLineComment = "--";
    _commentStart = "--[[";
    _commentEnd = "]]";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(and|break|do|else|elseif|end|false|for|function|if|in|local|nil|not|or|repeat|return|then|true|until|while|goto)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(_G|_VERSION|assert|collectgarbage|dofile|error|getfenv|getmetatable|ipairs|load|loadfile|loadstring|next|pairs|pcall|print|rawequal|rawget|rawlen|rawset|select|setfenv|setmetatable|tonumber|tostring|type|unpack|xpcall|pairs|ipairs|table|string|math|io|os|debug|coroutine|package|type|pcall|yield)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Built-in modules
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(table\.insert|table\.remove|table\.concat|table\.sort|table\.sort|table\.maxn|string\.len|string\.sub|string\.byte|string\.char|string\.find|string\.format|string\.gmatch|string\.gsub|string\.match|string\.rep|string\.reverse|string\.sub|string\.upper|string\.lower|math\.abs|math\.acos|math\.asin|math\.atan|math\.atan2|math\.ceil|math\.cos|math\.cosh|math\.deg|math\.exp|math\.floor|math\.fmod|math\.frexp|math\.huge|math\.ldexp|math\.log|math\.log10|math\.max|math\.min|math\.modf|math\.pi|math\.pow|math\.rad|math\.random|math\.randomseed|math\.sin|math\.sinh|math\.sqrt|math\.tan|math\.tanh|math\.tointeger|math|type|io\.close|io\.flush|io\.input|io\.lines|io\.open|io\.output|io\.popen|io\.read|io\.tmpfile|io\.type|io\.write|os\.clock|os\.date|os\.difftime|os\.execute|os\.exit|os\.getenv|os\.remove|os\.rename|os\.setlocale|os\.time|os\.tmpname)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(
        R"("[\s\S]*?"|\'[\s\S]*?\'|\[\[[\s\S]*?\]\]|==\[\[[\s\S]*?\]\])");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]*\.[0-9a-fA-F_]*([pP][+-]?[0-9a-fA-F_]+)?|0[xX][0-9a-fA-F_]+([pP][+-]?[0-9a-fA-F_]+)?|[0-9][0-9_]*\.[0-9_]*([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfunction\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Local function
    rule = {}; rule.pattern = QRegularExpression(
        R"(\blocal\s+function\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Table field access
    rule = {}; rule.pattern = QRegularExpression(R"(\.[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~#]+|==|!=|<=|>=|~=|//|\.\.\.)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Web: HTML / XML / CSS / JavaScript / TypeScript / WebAssembly
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupHtml() {
    HighlightRule rule;

    // HTML Tags
    rule = {}; rule.pattern = QRegularExpression(
        R"(<\/?[a-zA-Z][a-zA-Z0-9:]*)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Attributes
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[a-zA-Z_:][a-zA-Z0-9_:\-.]*(?=\s*=))");
    rule.format = _attributeFormat; _rules.append(rule);

    // Attribute values
    rule = {}; rule.pattern = QRegularExpression(
        R"((?:=\s*)(?:"[^"]*"|'[^']*'|[^\s>]+))");
    rule.format = _stringFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(<!--[\s\S]*?-->)");
    rule.format = _commentFormat; _rules.append(rule);

    // DOCTYPE
    rule = {}; rule.pattern = QRegularExpression(R"(<!DOCTYPE[^>]*>)", QRegularExpression::CaseInsensitiveOption);
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(R"(\b[0-9]+(?:\.[0-9]+)?(?:px|em|rem|%|vh|vw|pt|cm|mm|in)?)");
    rule.format = _numberFormat; _rules.append(rule);

    // Color codes
    rule = {}; rule.pattern = QRegularExpression(R"(#[0-9a-fA-F]{3,8})\b");
    rule.format = _numberFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupXml() {
    HighlightRule rule;

    // Tags
    rule = {}; rule.pattern = QRegularExpression(
        R"(<\/?[a-zA-Z_:][a-zA-Z0-9:_.\-]*)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Attributes
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[a-zA-Z_:][a-zA-Z0-9:_.\-]*(?=\s*=))");
    rule.format = _attributeFormat; _rules.append(rule);

    // Attribute values
    rule = {}; rule.pattern = QRegularExpression(
        R"((?:=\s*)"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // XML Declaration
    rule = {}; rule.pattern = QRegularExpression(R"(<\?xml[^?]*\?>)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // DOCTYPE
    rule = {}; rule.pattern = QRegularExpression(R"(<!DOCTYPE[^>]*>)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // CDATA
    rule = {}; rule.pattern = QRegularExpression(R"(<!\[CDATA\[[\s\S]*?\]\]>)");
    rule.format = _commentFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(<!--[\s\S]*?-->)");
    rule.format = _commentFormat; _rules.append(rule);

    // Processing instructions
    rule = {}; rule.pattern = QRegularExpression(R"(<\?[^?]*\?>)");
    rule.format = _attributeFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupCss() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords/At-rules
    rule = {}; rule.pattern = QRegularExpression(
        R"(@(import|media|keyframes|font-face|supports|charset|namespace|page|viewport|counter-style|property|document|font-face|swash|ornaments|annotation|stylistic|styleset|character-variant|import|layer|container|container-type|container-name|media|all|and|not|or|only|supports|selector|viewport|font-feature-values|交响|fontpalettecolors))\b|"
        R"(\{\s*\})");
    rule.format = _keywordFormat; _rules.append(rule);

    // Selectors
    rule = {}; rule.pattern = QRegularExpression(
        R"(\.[a-zA-Z_][a-zA-Z0-9_\-]*(?=\s*[,-9_\-]*(?=\s*[,{:]))");
    rule.format = _keywordFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"([a-zA-Z][a-zA-Z0-9_\-]*(?=\s*[,{:]))");
    rule.format = _keywordFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"(:[a-zA-Z_][a-zA-Z0-9_\-]*(?=\s*[,{:;)]))");
    rule.format = _attributeFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"(::[a-zA-Z_][a-zA-Z0-9_\-]*(?=\s*[,{:;)]))");
    rule.format = _attributeFormat; _rules.append(rule);

    // Properties
    rule = {}; rule.pattern = QRegularExpression(
        R"([a-zA-Z][a-zA-Z0-9\-]*(?=\s*:))");
    rule.format = _functionFormat; _rules.append(rule);

    // Property values
    rule = {}; rule.pattern = QRegularExpression(
        R"(:\s*[^;{}\n]+)");
    rule.format = _stringFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("[^"]*"|'[^']*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers with units
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9]+(?:\.[0-9]+)?(?:px|em|rem|%|vh|vw|pt|cm|mm|in|deg|s|ms|kHz|MHz|dpi|dpcm|dppx|ex|ch|lh|rlh|vmin|vmax|fr)?)");
    rule.format = _numberFormat; _rules.append(rule);

    // Hex colors
    rule = {}; rule.pattern = QRegularExpression(R"(#[0-9a-fA-F]{3,8})\b");
    rule.format = _numberFormat; _rules.append(rule);

    // Functions
    rule = {}; rule.pattern = QRegularExpression(
        R"([a-zA-Z_][a-zA-Z0-9_]*(?=\())");
    rule.format = _functionFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupJavaScript() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(async|await|break|case|catch|class|const|continue|debugger|default|delete|do|else|export|extends|false|finally|for|function|if|import|in|instanceof|let|new|null|of|return|static|super|switch|this|throw|true|try|typeof|undefined|var|void|while|with|yield|let|get|set|as|from|enum|implements|interface|package|private|protected|public|abstract|readonly|enum|declare|type|namespace|module|ambient|constructor|declare|export)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in objects
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Array|Object|String|Number|Boolean|Symbol|Function|Symbol|Map|Set|WeakMap|WeakSet|Proxy|Reflect|JSON|Math|Date|RegExp|Error|TypeError|Error|SyntaxError|URIError|RangeError|EvalError|Promise|Generator|GeneratorFunction|AsyncFunction|Intl|ArrayBuffer|TypedArray|DataView|SharedArrayBuffer|Atomics|BigInt|BigInt64Array|BigUint64Array|WebAssembly|console|window|document|navigator|location|history|localStorage|sessionStorage|fetch|process|require|module|exports|globalThis)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"(`[\s\S]*?`)");
    rule.format = _stringFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Template literals
    rule = {}; rule.pattern = QRegularExpression(R"(\$\{[^}]+\}|\$\w+)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9]+\.[0-9]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[n]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(async\s+)?function\s+([a-zA-Z_$][a-zA-Z0-9_$]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Arrow functions / class methods
    rule = {}; rule.pattern = QRegularExpression(
        R"((?:async\s+)?(?:(?:\([^)]*\)|[a-zA-Z_$][a-zA-Z0-9_$]*)\s*(?:=>|(?=\s*\{))))");
    rule.format = _functionFormat; _rules.append(rule);

    // Decorators
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_$][a-zA-Z0-9_$.]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Regex literals
    rule = {}; rule.pattern = QRegularExpression(R"(/[^/\n]+/[gimsuy]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||===|!==|=>|\+\+|--|\*\*|\.\.\.|\+\+|--|\?\?|\?\?=|?.?\.|\+=|-=|\*=|\/=)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupTypeScript() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(any|as|async|await|boolean|break|case|catch|class|const|constructor|continue|debugger|declare|default|delete|do|else|enum|export|extends|false|finally|for|from|function|get|if|implements|import|in|infer|instanceof|interface|is|keyof|let|module|namespace|never|new|null|number|object|of|package|private|protected|public|readonly|require|return|set|static|string|super|switch|symbol|this|throw|true|try|type|typeof|undefined|unique|unknown|var|void|while|with|yield|abstract|as|asserts|const|enum|namespace|type|readonly)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Array|Object|String|Number|Boolean|Symbol|Function|Promise|Map|Set|WeakMap|WeakSet|Record|Omit<|Partial<|Required<|Readonly<|Pick<|Extract<|Exclude<|NonNullable<|ReturnType<|Parameters<|InstanceType<|Intrinsic|Uppercase|Lowercase|Capitalize|Uncapitalize|keyof|infer)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"(`[\s\S]*?`)");
    rule.format = _stringFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Template literals
    rule = {}; rule.pattern = QRegularExpression(R"(\$\{[^}]+\})");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9]+\.[0-9]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[n]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(async\s+)?function\s+([a-zA-Z_$][a-zA-Z0-9_$]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Arrow functions
    rule = {}; rule.pattern = QRegularExpression(
        R"((?:async\s+)?\([^)]*\)\s*(?::\s*[^=]+)?=>|\w+(?:\s*<[^>]+>)?\s*\([^)]*\)\s*(?::\s*[^=]+)?=>)");
    rule.format = _functionFormat; _rules.append(rule);

    // Generics
    rule = {}; rule.pattern = QRegularExpression(R"(<[A-Za-z_][A-Za-z0-9_,\s:?|&|extends|in|keyof|as|infer]*>)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Decorators
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_$][a-zA-Z0-9_$.]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Regex literals
    rule = {}; rule.pattern = QRegularExpression(R"(/[^/\n]+/[gimsuy]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|===|!==|&&|\|\||=>|\+\+|--|\*\*|\.\.\.|\?\?|\?\?=|?.?\.|\+=|-=|\*=|\/=|<:|extends\s+)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupWebAssembly() {
    _singleLineComment = ";;";

    HighlightRule rule;

    // Keywords/instructions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(module|func|table|memory|global|export|import|type|param|result|local|global|block|loop|if|then|else|end|call|call_indirect|get_local|set_local|tee_local|get_global|set_global|load|store|i32\.load|i64\.load|f32\.load|f64\.load|i32\.store|i64\.store|f32\.store|f64\.store|i32\.const|i64\.const|f32\.const|f64\.const|i32\.add|i32\.sub|i32\.mul|i32\.div_s|i32\.div_u|i32\.rem_s|i32\.rem_u|i32\.and|i32\.or|i32\.xor|i32\.shl|i32\.shr_s|i32\.shr_u|i32\.rotl|i32\.rotr|i64\.add|i64\.sub|i64\.mul|i64\.div_s|i64\.div_u|i64\.rem_s|i64\.rem_u|i64\.and|i64\.or|i64\.xor|i64\.shl|i64\.shr_s|i64\.shr_u|i64\.rotl|i64\.rotr|f32\.add|f32\.sub|f32\.mul|f32\.div|f32\.sqrt|f32\.abs|f32\.neg|f32\.ceil|f32\.floor|f32\.trunc|f32\.nearest|f32\.min|f32\.max|f32\.copysign|f64\.add|f64\.sub|f64\.mul|f64\.div|f64\.sqrt|f64\.abs|f64\.neg|f64\.ceil|f64\.floor|f64\.trunc|f64\.nearest|f64\.min|f64\.max|f64\.copysign|i32\.eqz|i32\.eq|i32\.ne|i32\.lt_s|i32\.lt_u|i32\.le_s|i32\.le_u|i32\.gt_s|i32\.gt_u|i32\.ge_s|i32\.ge_u|i64\.eqz|i64\.eq|i64\.ne|i64\.lt_s|i64\.lt_u|i64\.le_s|i64\.le_u|i64\.gt_s|i64\.gt_u|i64\.ge_s|i64\.ge_u|f32\.eq|f32\.ne|f32\.lt|f32\.le|f32\.gt|f32\.ge|f64\.eq|f64\.ne|f64\.lt|f64\.le|f64\.gt|f64\.ge|i32\.wrap\/i64|i32\.trunc_s\/f32|i32\.trunc_u\/f32|i32\.trunc_s\/f64|i32\.trunc_u\/f64|i64\.extend_s\/i32|i64\.extend_u\/i32|i64\.trunc_s\/f32|i64\.trunc_u\/f32|i64\.trunc_s\/f64|i64\.trunc_u\/f64|f32\.demote\/f64|f64\.promote\/f32|f32\.convert_s\/i32|f32\.convert_u\/i32|f32\.convert_s\/i64|f32\.convert_u\/i64|f64\.convert_s\/i32|f64\.convert_u\/i32|f64\.convert_s\/i64|f64\.convert_u\/i64|i32\.reinterpret\/f32|i32\.reinterpret\/f32|i64\.reinterpret\/f64|i64\.reinterpret\/f64|f32\.reinterpret\/i32|f64\.reinterpret\/i64|nop|unreachable|br|br_if|return|call|call_indirect|memory\.size|memory\.grow|table\.size|table\.grow|table\.get|table\.set|table\.init|elem\.drop|data\.drop|memory\.init|unreachable|call_ref|call_indirect_ref|table\.fill|table\.copy|table\.grow|i32\.popcnt|i64\.popcnt|i32\.clz|i64\.clz|i32\.ctz|i64\.ctz|select|tuple\.make|tuple\.extract|ref\.null|ref\.func|ref\.is_null|ref\.eq|br_on_null)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(i32|i64|f32|f64|anyfunc|funcref|externref|i8|i16|blocktype|resulttype|v128|func|mut)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers (decimal and hex)
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[xX][0-9a-fA-F_]*\.[0-9a-fA-F_]*([pP][+-]?[0-9_]+)?|[0-9][0-9_]*\.[0-9_]*([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function/type names
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_$]*)");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([()\[\]{}:,=>])");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Systems: Go / Rust / Swift / Erlang / Elixir / F# / Lisp / Haskell
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupGo() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(break|case|chan|const|continue|default|defer|else|fallthrough|for|func|go|goto|if|import|interface|map|package|range|return|select|struct|switch|type|var|nil|true|false|iota)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int|int8|int16|int32|int64|uint|uint8|uint16|uint32|uint64|float32|float64|complex64|complex128|bool|byte|rune|string|error|any|uintptr|map|chan|func|interface|struct|array)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover|flush|itoa|min|max|clear|clone|conj)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Raw strings
    rule = {}; rule.pattern = QRegularExpression(R"(`[^`]*`)");
    rule.format = _stringFormat; _rules.append(rule);

    // Char/rune literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[i]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfunc\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Packages
    rule = {}; rule.pattern = QRegularExpression(
        R"([a-z][a-zA-Z0-9_]*\.[A-Z][a-zA-Z0-9_]*)");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~:]+|==|!=|<=|>=|&&|\|\||<-|\+\+|--|\+\+|--|\.\.\.|\.\.|\+=|-=|\*=|\/=)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupRust() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(as|async|await|break|const|continue|crate|dyn|else|enum|extern|false|fn|for|if|impl|in|let|loop|match|mod|move|mut|pub|ref|return|self|Self|static|struct|super|trait|true|type|unsafe|use|where|while|abstract|become|box|do|dynamic|final|macro|override|priv|try|typeof|unsized|virtual|yield|macro_rules)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(i8|i16|i32|i64|i128|isize|u8|u16|u32|u64|u128|usize|f32|f64|bool|char|str|String|Vec|Box|Option|Result|HashMap|HashSet|BTreeMap|BTreeSet|Rc<|Arc<|Cell<|RefCell<|Mutex<|RwLock<|Cow<|Rc|Arc|Cell|RefCell|Mutex|RwLock|Iterator|Item|Box|impl|Trait|Type)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Built-in macros
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(macro_rules!|println!|eprintln!|print!|eprint!|format!|vec!|stringify!|concat!|env!|option!|dbg!|todo!|unimplemented!|unreachable!|assert!|assert_eq!|assert_ne!|debug_assert!|mem::|println!|eprintln!|include!|include_str!|include_bytes!|concat_idents!|line!|column!|file!|module_path!|cfg!)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Doc comments
    rule = {}; rule.pattern = QRegularExpression(R"(///[\s\S]*?|^//![\s\S]*?)");
    rule.format = _commentFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Raw strings
    rule = {}; rule.pattern = QRegularExpression(R"(r#?"[^"#]*"#*)");
    rule.format = _stringFormat; _rules.append(rule);

    // Byte strings
    rule = {}; rule.pattern = QRegularExpression(R"(b"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Lifetimes
    rule = {}; rule.pattern = QRegularExpression(R"('[a-z_][a-zA-Z0-9_]*\b)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Macros
    rule = {}; rule.pattern = QRegularExpression(R"([a-z_][a-zA-Z0-9_]*!)");
    rule.format = _functionFormat; _rules.append(rule);

    // Turbofish
    rule = {}; rule.pattern = QRegularExpression(R"(::\s*<)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fFiI]?|[0-9]+[eE][+-]?[0-9_]+[fFiI]?|[0-9]+[uUiI8Ui16Ui32Ui64Uisize]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfn\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||=>|->|<<|>>|\.\.=?|\+=|-=|\*=|\/=|\|>|<\$)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupSwift() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(associatedtype|class|deinit|enum|extension|fileprivate|func|import|init|inout|internal|let|open|operator|private|protocol|public|static|struct|subscript|typealias|var|break|case|continue|default|defer|do|else|fallthrough|for|guard|if|in|repeat|return|switch|where|while|as|catch|is|nil|rethrows|super|self|Self|throw|throws|try|async|await|actor|any|some|protocol|associatedtype|convenience|didSet|get|willSet|indirect|lazy|nonisolated|optional|override|mutating|nonmutating|fileprivate|internal|public|open|final|required|static|weak|unowned|inout|dynamic|objc|IBOutlet|IBAction|nonobjc)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Int|Int8|Int16|Int32|Int64|UInt|UInt8|UInt16|UInt32|UInt64|Float|Double|Bool|String|Character|Array|Dictionary|Set|Optional|Any|AnyObject|Void|Never|Error|Result|URL|Data|Date|CGFloat|CGPoint|CGSize|CGRect|UUID|Decimal|RawRepresentable|Sequence|Collection|Iterable|Iterator|Generator|OptionSet|BitwiseOperations|SignedInteger|UnsignedInteger|FixedWidthInteger|Comparable|Equatable|Hashable|CustomStringConvertible|TextOutputStreamable|Error|LocalizedError|Identifiable|LosslessStringConvertible|Codable|Encodable|Decodable)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Multi-line strings
    rule = {}; rule.pattern = QRegularExpression(R"("""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Interpolation
    rule = {}; rule.pattern = QRegularExpression(R"(\$\{[a-zA-Z_][a-zA-Z0-9_]*\})");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+[eE][+-]?[0-9_]+|[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfunc\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||===|!==|===|!==|===|!==|\.\.=|\.\.<|->|>>=|<<=|\?\?|\?\.|\?\?=|..<|...)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupErlang() {
    _singleLineComment = "%";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(after|and|andalso|begin|case|catch|cond|end|fun|if|let|not|of|or|orelse|query|receive|rem|when|xor|export|module|record|define|ifdef|ifndef|else|endif|include|include_lib|author|vsn|behaviour|behavior|spec|type|opaque|export_type|callback|optional_callbacks|dynamic_function_translation|on_load|export_all|compile|module_info)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // BIFs
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abs/1|append/2|apply/2|atom_to_binary/1|atom_to_list/1|binary_to_atom/1|binary_to_existing_atom/1|binary_to_list/1|binary_to_term/1|bitstring_to_list/1|ceil/1|concat/2|date/0|delete_module/1|demonitor/1|display/1|element/2|erase/0|exit/1|float/1|float_to_list/1|floor/1|get/0|get_keys/0|group_leader/2|halt/0|hd/1|integer_to_list/1|iolist_to_binary/1|iolist_size/1|is_alive/0|is_atom/1|is_binary/1|is_bitstring/1|is_boolean/1|is_float/1|is_function/1|is_function/2|is_integer/1|is_list/1|is_map/1|is_number/1|is_pid/1|is_port/1|is_process_alive/1|is_record/2|is_record/3|is_reference/1|is_tuple/1|length/1|link/1|list_to_atom/1|list_to_binary/1|list_to_bitstring/1|list_to_existing_atom/1|list_to_float/1|list_to_integer/1|list_to_pid/1|list_to_tuple/1|load_module/2|make_ref/0|map_get/2|map_size/1|maps/1|max/2|min/2|module_loaded/1|monitor/2|node/0|node/1|nodes/0|now/0|open_port/2|pid_to_list/1|port_close/1|port_command/2|port_connect/2|port_control/3|ports/0|preg_replace/4|preg_match/2|preg_match_all/2|preg_split/2|process_flag/2|process_info/1|process_info/2|processes/0|purge_module/1|put/2|raise/3|register/2|registered/0|remove_handler/1|round/1|self/0|setelement/3|size/1|spawn/1|spawn/3|spawn_link/1|spawn_monitor/1|split_binary/2|start_timer/3|statistics/1|term_to_binary/1|tl/1|trunc/1|tuple_to_list/1|unique_integer/1|unlink/1|unregister/1|whereis/1|write_file/2|zip/2|zip/3|zipwith/3)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\[^u0-9]|\\x\{[0-9a-fA-F]+\}|\\[0-9]+|&[^"]|")*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char lists
    rule = {}; rule.pattern = QRegularExpression(R"(\$[^ \t\n]+|'\^[^']'|'\^[A-Z@\[\]\\\\]')");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+#?[0-9a-fA-F_]*|0[bB][01_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b([a-z][a-zA-Z0-9_]*)\s*(?=\(|/))");
    rule.format = _functionFormat; _rules.append(rule);

    // Module:Function
    rule = {}; rule.pattern = QRegularExpression(
        R"([A-Z][a-zA-Z0-9_]*:[a-z][a-zA-Z0-9_]*)");
    rule.format = _functionFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(%.+)");
    rule.format = _commentFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~:+@#$.?]+|==|=/=|=:=|=:=|=<|>=|<:|->|=>|\+\+|--|::|\|\||\#\#)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupElixir() {
    _singleLineComment = "#";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(alias|and|case|cond|def|defdelegate|defexception|defoverridable|defp|defprotocol|defstruct|do|else|end|fn|for|if|import|in|not|or|quote|raise|receive|require|reraise|return|from|rescue|throw|try|unless|unquote|unquote_splicing|use|when|with|after|else|catch|module|defimpl|defprotocol|defmacro|defmacrop|module_attribute|behaviour|behaviour_info|callback|macro_callback|optional_callbacks|overridable|super|Struct|Open|closed|Enum|Epoch|Date|Time|naive_date|datetime|chunk|chunk_every|chunk_by|concat|cycle|dedup|dedup_by|drop|drop_every|drop_while|each|encode|encode!\n|filter|find|first|flat_map|flat_map_reduce|frequencies|frequencies_by|group_by|intersperse|into|join|last|map|map_every|map_reduce|max|max_by|min|min_by|min_max|min_max_by|product|reduce|reduce_while|reject|reverse|scan|shuffle|slice|slice_every|sort|sort_by|split|split_while|sum|take|take_every|take_while|to_file|to_string|uniq|uniq_by|unzip|with_index|zip|zip_with)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Atoms
    rule = {}; rule.pattern = QRegularExpression(R"(:[a-zA-Z_][a-zA-Z0-9_@!?:]*|:\"[^\"]+\"|:\"[a-zA-Z_][a-zA-Z0-9_@!?:]*\")");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings/heredocs
    rule = {}; rule.pattern = QRegularExpression(R"("(?:"[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(R"('''[\s\S]*?'''|"""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Sigils
    rule = {}; rule.pattern = QRegularExpression(R"(~\[|\~\[|\{~|\)~|\{~|\)~|\/~|\|~|\<~)");
    rule.format = _stringFormat; _rules.append(rule);

    // Module names
    rule = {}; rule.pattern = QRegularExpression(
        R"([A-Z][a-zA-Z0-9_]*\.[A-Z][a-zA-Z0-9_]*\.[A-Z][a-zA-Z0-9_]*|[A-Z][a-zA-Z0-9_]*\.[A-Z][a-zA-Z0-9_]*)");
    rule.format = _functionFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-z_][a-zA-Z0-9_]*)");
    rule.format = _typeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|[0-9]+[jJ]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bdef(?:p|macro|macrop)?\s+([a-z_][a-zA-Z0-9_?!]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~.]+|==|!=|===|!==|<=|>=|&&|\|\||\+\+|--|->|=>|\.\.\.|\.\.|\|>|<<|>>|\|>|<\~|<\|>|<\|>|\+=|=|when|and|or|not|in|::|\.\/)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupFSharp() {
    _singleLineComment = "//";
    _commentStart = "(*";
    _commentEnd = "*)";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abstract|and|as|assert|base|begin|class|default|delegate|do|done|downcast|downto|elif|else|end|exception|extern|false|finally|for|fun|function|global|if|in|inherit|inline|interface|internal|let|lazy|match|member|module|mutable|namespace|new|not|null|of|open|or|override|private|public|rec|return|static|struct|then|to|true|try|type|upcast|use|val|void|when|while|with|yield|asr|land|lor|lsl|lsr|lxor|mod|&&|\|\||\||!=|==|<=|>=|->)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(int|int8|int16|int32|int64|uint|uint8|uint16|uint32|uint64|float|float32|double|decimal|bool|string|char|byte|sbyte|short|ushort|long|ulong|unit|option|list|array|seq|map|set|ResizeArray|List|Array|Map|Set|Seq|Option|Result|Choice|Task|Async|seq|list|array|Set|Map|Tree)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Active patterns
    rule = {}; rule.pattern = QRegularExpression(R"(\|[.*?]+\s*->)");
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Triple-quoted strings
    rule = {}; rule.pattern = QRegularExpression(R"("""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|[0-9][0-9_]*\.[0-9_]+[fF]?|[0-9]+[eE][+-]?[0-9_]+[fF]?|[0-9]+[uUlL]?[yUsSn]?)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\blet\s+(?:rec\s+)?([a-zA-Z_][a-zA-Z0-9']*))");
    rule.format = _functionFormat; _rules.append(rule);
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bmember\s+[a-zA-Z_][a-zA-Z0-9']*\s*\.\s*([a-zA-Z_][a-zA-Z0-9']*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|&&|\|\||->|<-|<>|>>|<<|>>>|>?<|<?>?|\|>|<\|)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Functional languages: Lisp, Clojure, Haskell
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupLisp() {
    _singleLineComment = ";";
    _commentStart = "#|";
    _commentEnd = "|#";

    HighlightRule rule;

    // Special forms and keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(def|defn|defmacro|defmethod|deftype|defstruct|defonce|defprotocol|defrecord|deftemplate|define|defvar|defparameter|defconstant|let|let\*|fn|fn\*|loop|doto|when|when-not|when-let|if|if-let|cond|condp|case|dotimes|doseq|dorun|doall|doseq|for|each|nth|loop|recur|try|catch|finally|throw|reify|proxy|deftype|extend-protocol|extend-type|extend|specification|method|protocol|record|type|struct|map|reduce|filter|mapcat|flatten|distinct|group-by|partition|partition-by|interleave|interpose|into|conj|concat|apply|partial|comp|memoize|futures|promise|deliver|realized\?)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Core functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(nil|true|false|and|or|not|=\|=\|<|>|<=|>=|inc|dec|pos\?|neg\?|zero\?|even\?|odd\?|count|first|rest|next|last|butlast|drop|drop-while|take|take-while|nth|assoc|dissoc|get|contains\?|keys|vals|merge|merge-with|select-keys|update-in|update|conj|into|seq|cons|list|list\*|vector|vec|hash-map|array-map|sorted-map|sorted-map-by|hash-set|sorted-set|sorted-set-by|range|repeat|repeatedly|iterate|cycle|interleave|interpose|partition|partition-all|group-by|reduce|fold|every\?|some|not-every\?|not-any\?|map|mapcat|filter|remove|keep|keep-indexed|complement|juxt|apply|partial|comp|trampoline|some-fn|every-pred|some-fn|fnil|ensure最低限|update)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("([^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"(\\#[a-zA-Z_][a-zA-Z0-9_]*|\\.|\\\\[0-9]+|\\space|\\newline|\\tab|\\return|\\backspace)");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[rRmMfFdDlL]?|[+-]?[0-9]+[eE][+-]?[0-9_]+[rRmMfFdDlL]?|[+-]?[0-9]+[rRmMfFdDlL]?|[+-]?0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|#[0-9a-fA-F_]+|#[bBoOdDxX][0-9a-fA-F_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(R"(:[a-zA-Z_][a-zA-Z0-9_?!-]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Function/macro definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\([a-zA-Z_][a-zA-Z0-9_?!-]*\s+)");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([()\[\]{}]|['`,@#~^])");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupClojure() {
    _singleLineComment = ";";
    _commentStart = "#|";
    _commentEnd = "|#";

    HighlightRule rule;

    // Special forms
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(def|defn|defmacro|defmethod|definterface|deftype|defrecord|defprotocol|deftemplate|defmulti|defmethod|defonce|defstruct|define|defvar|defparameter|defconstant|let|letfn|fn|loop|recur|if|if-let|when|when-not|when-let|when-first|cond|condp|case|try|catch|finally|throw|do|doseq|dotimes|dorun|doall|for|binding|with-local-vars|with-open|with-precision|locking|comment|declare|alter-var-root|set!\b|->|->>|as-\>)|doto|..|gen-class|proxy|reify|delay|future|cfuture|deref|with-redefs|with-redefs-fn|alter-meta!|reset!|swap!)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Core library
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(nil|true|false|and|or|not|=\|=\|<|>|<=|>=|inc|dec|pos\?|neg\?|zero\?|even\?|odd\?|count|first|rest|next|last|butlast|drop|drop-while|take|take-while|nth|assoc|dissoc|assoc-in|update-in|get|contains\?|keys|vals|merge|merge-with|select-keys|map|filter|reduce|into|conj|concat|apply|comp|memoize|partial|juxt|comp|complement|every\?|some|not-every\?|not-any\?|sequence|seq|cons|list|list\*|vector|vec|hash-map|array-map|sorted-map|sorted-map-by|hash-set|sorted-set|sorted-set-by|range|repeat|repeatedly|iterate|cycle|interleave|interpose|partition|partition-by|group-by|distinct|frequencies|remove|keep|keep-indexed|flatten|concat|mapcat|take-nth|shuffle|split-at|split-with|every-pred|some-fn|fnil|trampoline|pmap|pmap-indexed|seque|shuffle|into-array|to-array-2d|make-array|object-array|boolean-array|byte-array|char-array|short-array|int-array|long-array|float-array|double-array|object-array)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Java interop
    rule = {}; rule.pattern = QRegularExpression(R"(\.(?:\w+|\.[a-zA-Z])\s*[^()\s]*\s*\(|new\s+[A-Z][a-zA-Z0-9_]*\s*(?:\[|\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("([^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[NMyMRflL]?|[+-]?[0-9]+[eE][+-]?[0-9_]+[NMyMRflL]?|[+-]?[0-9]+[NMyMRflL]?|[+-]?0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|#[0-9a-fA-F_]+|#[bBoOdDxX][0-9a-fA-F_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(R"(:[a-zA-Z_][a-zA-Z0-9_?!-]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Reader macros
    rule = {}; rule.pattern = QRegularExpression(R"(['`,@#~^])");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Function calls
    rule = {}; rule.pattern = QRegularExpression(
        R"(\([a-zA-Z_][a-zA-Z0-9_?!-]*\s+)");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([()\[\]{}]|['`,@#~^])");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupHaskell() {
    _singleLineComment = "--";
    _commentStart = "{-";
    _commentEnd = "-}";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(as|case|class|data|default|deriving|do|else|family|forall|foreign|hiding|if|import|in|infix|infixl|infixr|instance|let|module|newtype|of|qualified|then|type|using|where|_)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Int|Integer|Float|Double|Bool|Char|String|Maybe|Either|IO|Void|Ordering|Unit|[]|List|Tuple|Function|Ptr|FunPtr|Array|Vector|HashMap|HashSet|Text|ByteString|LazyByteString|Map|Set|Sequence)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Data constructors (uppercase)
    rule = {}; rule.pattern = QRegularExpression(R"(\b[A-Z][A-Za-z0-9_']*\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Import and LANGUAGE pragmas
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*import\s.*|^{-#\s+LANGUAGE\s.*#-})");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([!#$%&*+./<=>?@^|~:\\-]+)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Quasi-quotes
    rule = {}; rule.pattern = QRegularExpression(R"(\[\s*\||\|\s*\])");
    rule.format = _builtInFormat; _rules.append(rule);
}

// -------------------------------------------------------------------------
// Hardware description: Verilog, VHDL, Assembly
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupVerilog() {
    _singleLineComment = "//";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(always|always_ff|always_comb|always_latch|and|assign|automatic|begin|buf|bufif0|bufif1|case|casex|casez|cmos|deassign|default|defparam|disable|edge|else|end|endcase|endfunction|endgenerate|endmodule|endprimitive|endspecify|endtable|endtask|event|for|force|forever|fork|function|generate|genvar|highz0|highz1|if|initial|inout|input|integer|join|join_any|join_none|large|liblist|library|localparam|macromodule|medium|module|nand|negedge|nmos|nor|not|notif0|notif1|or|output|parameter|pmos|posedge|primitive|pull0|pull1|pulldown|pullup|rand|randc|readmemh|readmemb|real|realtime|reg|release|repeat|return|rnmos|rpmos|rtran|rtranif0|rtranif1|scalared|shortint|showcancelled|signal|signed|small|specify|specparam|strength0|strength1|string|strong0|strong1|supply0|supply1|table|task|time|tri|tri0|tri1|triand|trior|tror|trireg|type|typedef|union|unique|unique0|unsigned|var|vectored|void|wait|wand|weak0|weak1|while|wire|wor|xnor|xor|alias|assume|bind|binding|clocking|constraint|cover|covergroup|coverpoint|cross|dist|expect|property|sequence|struct|union|package|interface|modport|checker|endchecker|let|context|ref|global)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(byte|shortint|int|longint|integer|int|real|realtime|time|bit|logic|reg|wire|tri|trireg|tri0|tri1|triand|trior|wand|worn|supply0|supply1|uwire|chandle|string|void|var|const|static|automatic|volatile|packed|unpacked|shortreal)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // System tasks
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][A-Za-z0-9_$]*)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Preprocessor
    rule = {}; rule.pattern = QRegularExpression(R"(`[a-zA-Z_][A-Za-z0-9_]*)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9]+'[bdhoBDHO][0-9a-fA-F_xXzZ_]+|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Signal widths
    rule = {}; rule.pattern = QRegularExpression(R"(\[[^\]]+\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Module names (uppercase)
    rule = {}; rule.pattern = QRegularExpression(R"(\b[A-Z][A-Za-z0-9_]*)");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%^~&|!:]+|==|!=|<=|>=|<|>|&&|\|\||===|!==|>>>|<<=|>>=|===|!==|###|##|->|=>)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupVHDL() {
    _singleLineComment = "--";
    _commentStart = "/*";
    _commentEnd = "*/";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abs|access|after|alias|all|and|architecture|array|assert|attribute|begin|block|body|buffer|bus|case|component|configuration|constant|disconnect|downto|else|elsif|end|entity|exit|file|for|function|generate|generic|group|guarded|if|impure|in|inertial|inout|input|is|label|library|linkage|literal|loop|map|mod|nand|new|next|nor|not|null|of|on|open|or|others|out|package|port|postponed|procedure|process|pure|range|record|register|reject|rem|report|return|rol|ror|select|severity|shared|signal|sla|sll|sra|srl|subtype|then|to|transport|type|unaffected|until|use|variable|wait|when|while|with|xnor|xor|alias|all|area|body|buffering|bus|file|group|guard|impure|inertial|linkage|memory|open|passive|pure|range|record|register|reject|robust|shared|signal|variable)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(bit|bit_vector|boolean|character|integer|natural|positive|real|string|time|text|severity_level|file_open_kind|file_open_status|std_logic|std_logic_vector|std_ulogic|std_ulogic_vector|unsigned|signed|word|byte|natural|positive|string)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char literals
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9]+#[0-9a-fA-F_]+#?|[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|[0-9]+[eE][+-]?[0-9_]+|X"[0-9a-fA-F_]+"|O"[0-9a-fA-F_]+"|B"[01_]+")\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Attribute
    rule = {}; rule.pattern = QRegularExpression(R"('[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/=<>&|:]+|=>|:=|\/=|<=|>=|:=|>>|<<)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupAsm() {
    _singleLineComment = ";";
    HighlightRule rule;

    // Instructions (x86-64, x86-32, ARM subset)
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(mov|movzx|movsx|movsb|movsw|movsd|movss|movaps|movups|movdqa|movdqu|lea|lds|les|lfs|lgs|lss|xchg|xadd|cmp|cmpxchg|cmpxchg8b|cmpxchg16b|add|sub|mul|imul|div|idiv|inc|dec|neg|adc|sbb|and|or|xor|not|shl|sal|shr|sar|shld|shrd|rol|ror|rcl|rcr|cmp|test|push|pop|pushf|pushfd|pushfq|popf|popfd|call|ret|jmp|je|jz|jne|jnz|jl|jnge|jle|jng|jg|jnle|jge|jnl|ja|jnbe|jb|jnae|jc|jnc|jo|jno|js|jns|jp|jnp|jcxz|jecxz|jrcxz|loop|loope|loopne|loopnz|loopz|int|into|iret|iretd|iretq|bound|enter|leave|nop|hlt|wait|fwait|cpuide|cpuid|rdtsc|rdtscp|movnti|prefetcht0|prefetcht1|prefetcht2|prefetchnta|lfence|mfence|sfence|pause|nop|xchg|bsf|bsr|bt|btc|btr|bts|popcnt|lzcnt|tzcnt|cmov|cmove|cmovz|cmovne|cmovnz|cmova|cmovnbe|cmovae|cmovnb|cmovb|cmovnae|cmovc|cmovnc|cmovo|cmovno|cmovs|cmovns|cmovp|cmovnp|cmovl|cmovnge|cmovle|cmovng|cmovg|cmovnle|cmovge|setcc|sete|setz|setne|setnz|seta|setnbe|setnc|setb|setnae|setc|setnbe|setae|setnb|setnb|setnc|setb|setc|setna|setnbe|setnb|setnbe|setnb|setge|setnl|sets|setns|seto|setno|setp|setpe|setnp|setpo|setl|setnge|setnle|setnge|setnle|setnle|jrcxz|jecxz|jmpf|ljmp|callf|retf|int3|into|bound|enter|leave|popa|pusha|pushad|popad|pusha|popa|wait|fwait|fnop|fld|fst|fstp|fild|fist|fistp|fadd|faddp|fiadd|fsub|fsubp|fisub|fisubr|fmul|fmulp|fimul|fdiv|fdivp|fidiv|fidivr|fsubr|fsubrp|fsqrt|fabs|fchs|frndint|fclex|fnclex|finit|fninit|fstcw|fstsw|fnstcw|fnstsw|fldcw|fstenv|fnstenv|fldenv|fxsave|fxrstor|fxsave64|fxrstor64|push|pop|mov|ldr|str|ldm|stm|add|sub|rsb|adc|sbc|and|orr|eor|mov|mvn|cmp|cmn|tst|asr|lsl|lsr|ror|rrx|mul|mla|umull|umlal|smull|smlal|b|bl|bx|blx|bxeq|beq|bne|bcs|bhs|blo|bcc|bhs|bcc|bpl|bmi|bpl|bge|blt|bgt|ble|bhi|bls|ble|bal|bal|svc|svc|halt|bkpt|wfi|wfe|sev|crc32|bfc|bfi|bfi|bfc|bfi|ubfx|sbfx|sxtb|sxth|sxtw|uxtb|uxth|pkhbt|pkhtb|usat|sadd16|sasx|ssub16|sasx|add16|qadd|qadd8|qadd16|qsub|qsub8|qsub16|sel|rev|rev16|revsh|usad8|usada8|smuad|smuadx|smlad|smladx|smlald|smlaldx|smlsd|smlsdx|smlsld|smlsldx|smmul|smmulr|smmla|smmlar|smmls|smmlsr|pkhbt|pkhtb|sxtb16|uxtb16|saturating)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Registers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(rax|rbx|rcx|rdx|rsi|rdi|rbp|rsp|r8|r9|r10|r11|r12|r13|r14|r15|eax|ebx|ecx|edx|esi|edi|ebp|esp|r8d|r9d|r10d|r11d|r12d|r13d|r14d|r15d|ax|bx|cx|dx|si|di|bp|sp|r8w|r9w|r10w|r11w|r12w|r13w|r14w|r15w|al|ah|bl|bh|cl|ch|dl|dh|sil|dil|bpl|spl|r8b|r9b|r10b|r11b|r12b|r13b|r14b|r15b|cs|ds|es|fs|gs|ss|rflags|eflags|efl|rFLAGS|cr0|cr1|cr2|cr3|cr4|cr8|dr0|dr1|dr2|dr3|dr4|dr5|dr6|dr7|st0|st1|st2|st3|st4|st5|st6|st7|mm0|mm1|mm2|mm3|mm4|mm5|mm6|mm7|xmm0|xmm1|xmm2|xmm3|xmm4|xmm5|xmm6|xmm7|xmm8|xmm9|xmm10|xmm11|xmm12|xmm13|xmm14|xmm15|xmm16|xmm17|xmm18|xmm19|xmm20|xmm21|xmm22|xmm23|xmm24|xmm25|xmm26|xmm27|xmm28|xmm29|xmm30|xmm31|ymm0|ymm1|ymm2|ymm3|ymm4|ymm5|ymm6|ymm7|ymm8|ymm9|ymm10|ymm11|ymm12|ymm13|ymm14|ymm15|ymm16|ymm17|ymm18|ymm19|ymm20|ymm21|ymm22|ymm23|ymm24|ymm25|ymm26|ymm27|ymm28|ymm29|ymm30|ymm31|zmm0|zmm1|zmm2|zmm3|zmm4|zmm5|zmm6|zmm7|zmm8|zmm9|zmm10|zmm11|zmm12|zmm13|zmm14|zmm15|zmm16|zmm17|zmm18|zmm19|zmm20|zmm21|zmm22|zmm23|zmm24|zmm25|zmm26|zmm27|zmm28|zmm29|zmm30|zmm31|r0|r1|r2|r3|r4|r5|r6|r7|r8|r9|r10|r11|r12|r13|r14|r15|pc|sp|lr|cpsr|spsr|r0|r1|r2|r3|r4|r5|r6|r7|r8|r9|r10|r11|r12|sp|lr|pc)\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Directives
    rule = {}; rule.pattern = QRegularExpression(
        R"(\.(global|globl|section|text|data|bss|rodata|comm|lcomm|align|balign|p2align|space|skip|byte|word|short|long|quad|ascii|asciz|string|float|double|equ|set|macro|endm|rept|endr|if|else|elif|endif|ifdef|ifndef|irp|irpc|func|endfunc|type|size|cfi_startproc|cfi_endproc|cfi_def_cfa|cfi_offset|cfi_rel_offset|cfi_register|file|loc|ln|weak|hidden|internal|protected|extern|public|abort|ascii|comm|ident|line|linkonce|previous|pushsection|popsection|reloc|size|type|val|weakref)\b)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Labels
    rule = {}; rule.pattern = QRegularExpression(R"(\b[a-zA-Z_][a-zA-Z0-9_.]*:)");
    rule.format = _functionFormat; _rules.append(rule);

    // Local labels (.L)
    rule = {}; rule.pattern = QRegularExpression(R"(\.L[0-9]+|\.L[a-zA-Z0-9_]+)");
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'[^']*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers (hex $0x, 0x, decimal, binary)
    rule = {}; rule.pattern = QRegularExpression(
        R"(\$[0-9a-fA-F]+|\b0[xX][0-9a-fA-F]+|\b0[bB][01_]+|\b[0-9][0-9_]*|\b[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)");
    rule.format = _numberFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(#.*)");
    rule.format = _commentFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%,()\[\]]+)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Shell / DevOps: Bash, PowerShell, TCL, Dockerfile, CMake, Makefile
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupBash() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(if|then|else|elif|fi|case|esac|for|select|while|until|do|done|in|function|time|coproc|\[\[|]]|\bdo\b|\bdone\b|\bfi\b|\bthen\b|\belse\b|\belif\b|\basac\b)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Builtins
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(alias|bg|bind|break|caller|cd|command|compgen|complete|compopt|continue|declare|dirs|disown|echo|enable|eval|exec|exit|export|false|fc|fg|getopts|hash|help|history|jobs|kill|let|local|logout|mapfile|popd|printf|pushd|pwd|read|readonly|return|set|shift|shopt|source|suspend|test|times|trap|true|type|typeset|ulimit|umask|unalias|unset|wait|whence|which)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Common commands
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(grep|sed|awk|find|xargs|sort|uniq|wc|head|tail|cut|tr|tee|cat|dd|du|df|free|ls|stat|touch|mkdir|rm|rmdir|cp|mv|ln|chmod|chown|chgrp|install|tar|gzip|bzip2|xz|zip|unzip|rsync|scp|ssh|sftp|curl|wget|ping|ps|kill|killall|top|nice|renice|nohup|jobs|fg|bg|wait|xargs|exec|trap|shift|getopts)\b)");
    rule.format = _functionFormat; _rules.append(rule);

    // Special parameters
    rule = {}; rule.pattern = QRegularExpression(R"(\$[0-9]|\$\#|\$\*|\$\@|\$\?|\$\!|\$\-|\$\$)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$\{?[a-zA-Z_][a-zA-Z0-9_]*\}?)");
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"(\$'(?:[^'\\]|\\.)*'|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Command substitution
    rule = {}; rule.pattern = QRegularExpression(R"(\$\([^)]*\)|`[^`]+`)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|[0-9][0-9_]*)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([|&;()<>!]+|&&|\|\||>>|<<|>>|2>|>&));
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupPowerShell() {
    _singleLineComment = "#";
    _commentStart = "<#";
    _commentEnd = "#>";

    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Begin|Break|Catch|Class|Continue|Data|Define|Do|DynamicParam|Else|ElseIf|End|Exit|Filter|Finally|For|ForEach|Function|If|In|InlineScript|Parallel|Param|Process|Return|Sequence|Static|Switch|Throw|Trap|Try|Until|While|Workflow|Until|Workflow)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*|\$\{[^}]+\}|\$[0-9]+|\$_(?:\.[a-zA-Z_][a-zA-Z0-9_]*)+)");
    rule.format = _typeFormat; _rules.append(rule);

    // Built-in cmdlets
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Get-Content|Set-Content|Add-Content|Clear-Content|Get-Item|Set-Item|New-Item|Remove-Item|Copy-Item|Move-Item|Get-ChildItem|Get-Date|Get-Random|Write-Host|Write-Output|Write-Error|Write-Warning|Write-Verbose|Write-Debug|Out-File|Import-Module|Export-Module|Get-Help|Get-Command|Get-Member|ForEach-Object|Where-Object|Select-Object|Sort-Object|Group-Object|Measure-Object|Compare-Object|ConvertTo-Json|ConvertFrom-Json|Invoke-RestMethod|Invoke-WebRequest|Start-Process|Stop-Process|Get-Process|Start-Job|Receive-Job|Test-Path|Join-Path|Split-Path|Get-Location|Set-Location|Push-Location|Pop-Location|Get-Variable|Set-Variable|New-Variable|Remove-Variable|Clear-Variable|Get-Acl|Set-Acl|Test-Connection|Resolve-DnsName|Get-NetIPAddress|Get-WmiObject|Get-CimInstance)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Comparison operators
    rule = {}; rule.pattern = QRegularExpression(
        R"(-eq|-ne|-gt|-lt|-ge|-le|-match|-notmatch|-like|-notlike|-in|-notin|-contains|-notcontains|-replace|-split|-join)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("@[\s\S]*?"@|"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Type accelerators
    rule = {}; rule.pattern = QRegularExpression(R"(\[[a-zA-Z_][a-zA-Z0-9_.]*\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Attributes
    rule = {}; rule.pattern = QRegularExpression(R"(\[CmdletBinding\]|\[Parameter\([^\]]*\)\]|\[ValidateSet\([^\]]*\)\]|\[AllowNull\]|\[AllowEmptyString\]|\[AllowEmptyCollection\]|\[OutputType\]|\[Alias\]|\[Required\]|\[ValidateRange\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Function definitions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\bfunction\s+([a-zA-Z_][a-zA-Z0-9_-]*))");
    rule.format = _functionFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||->|-replace|-match|-split|-join|>>|<<|\|>|&|&&|\|\||\?));
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupTcl() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(after|append|apply|array|auto_execok|auto_load|auto_mkindex|auto_path|bell|bind|binary|break|case|catch|cd|clock|close|concat|continue|error|eval|exec|exit|expr|fblocked|fconfigure|fcopy|file|fileevent|flush|for|foreach|format|gets|glob|global|history|http|if|incr|info|interp|join|lappend|library|lindex|linsert|list|llength|load|loadTk|lrange|lreplace|lsearch|lset|lsort|memory|msgcat|namespace|open|package|parray|pid|proc|puts|pwd|read|regexp|registry|regsub|rename|return|scan|seek|set|source|split|string|subst|switch|tcl_findLibrary|tell|time|trace|unknown|unset|update|uplevel|upvar|variable|vwait|while|zlib)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in commands
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(puts|gets|set|unset|incr|append|lappend|lindex|linsert|llength|lrange|lreplace|lsearch|lset|lsort|concat|join|split|string|subst|format|scan|regexp|regsub|switch|list|lrange|dict|lmap|array|dollarparens|upvar|global|variable|namespace|proc|lambda|tailcall|return|break|continue|error|try|throw|finally|catch)\b)");
    rule.format = _builtInFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*(?:\([^)]*\))?(?:\([a-zA-Z0-9_]+\))?|\$\{[a-zA-Z_][a-zA-Z0-9_]*\})");
    rule.format = _typeFormat; _rules.append(rule);

    // Command substitution
    rule = {}; rule.pattern = QRegularExpression(R"(\[[^\[\]]*\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"(\{(?:[^{}]|\{[^{}]*\})*\}|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+[eE][+-]?[0-9_]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|==|!=|<=|>=|&&|\|\||eq|ne|lt|gt|le|ge|in|ni|and|or|not|xor));
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupDockerfile() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Instructions
    rule = {}; rule.pattern = QRegularExpression(
        R"(^(FROM|RUN|CMD|LABEL|EXPOSE|ENV|ADD|COPY|ENTRYPOINT|VOLUME|USER|WORKDIR|ARG|MAINTAINER|ONBUILD|STOPSIGNAL|HEALTHCHECK|SHELL|ARG|IMPORT|EXPOSE|STOPSIGNAL)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Environment variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[a-zA-Z_][a-zA-Z0-9_]*|\$\{[a-zA-Z_][a-zA-Z0-9_]*\})");
    rule.format = _attributeFormat; _rules.append(rule);

    // Escape directive
    rule = {}; rule.pattern = QRegularExpression(R"(^#\s*escape\s*=\s*[\`\\])");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Paths
    rule = {}; rule.pattern = QRegularExpression(R"(\/[a-zA-Z0-9_.\-]+)+|\.[a-zA-Z0-9_.\-]+");
    rule.format = _functionFormat; _rules.append(rule);

    // Ports
    rule = {}; rule.pattern = QRegularExpression(R"(\b[0-9]{1,5}\b(?:\s*:\s*[0-9]{1,5})?)");
    rule.format = _numberFormat; _rules.append(rule);

    // Flags
    rule = {}; rule.pattern = QRegularExpression(R"(--[a-zA-Z][a-zA-Z0-9_-]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(R"(\b[0-9]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([=\[\]|])");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupCMake() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Commands
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(add_custom_command|add_custom_target|add_definitions|add_dependencies|add_executable|add_library|add_subdirectory|add_test|aux_source_directory|build_command|build_name|cmake_host_system_information|cmake_minimum_required|cmake_parse_arguments|configure_file|create_test_sourcelist|define_property|enable_language|enable_testing|endforeach|endfunction|endif|endmacro|endwhile|execute_process|export|export_library_dependencies|file|fltk_wrap_ui|function|get_cmake_property|get_directory_property|get_filename_component|get_property|get_source_file_property|get_target_property|get_test_property|include|include_directories|include_external_msproject|include_regular_expression|install|install_files|install_programs|install_targets|link_directories|link_libraries|linkify|list|load_cache|load_command|macro|make_directory|mark_as_advanced|math|message|option|project|remove|remove_definitions|separate_arguments|set|set_directory_properties|set_property|set_source_files_properties|set_target_properties|set_tests_properties|source_group|string|subdirs|target_compile_definitions|target_compile_features|target_compile_options|target_include_directories|target_link_directories|target_link_libraries|target_link_options|target_sources|try_compile|try_run|unset|variable_watch|while|write_file)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$\{?[A-Za-z_][A-Za-z0-9_]*\}?)");
    rule.format = _typeFormat; _rules.append(rule);

    // Generator expressions
    rule = {}; rule.pattern = QRegularExpression(R"(\$<[^>]+>)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Paths
    rule = {}; rule.pattern = QRegularExpression(R"([A-Za-z]:\\[^\s]+|\/[^\s]+)");
    rule.format = _functionFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|[0-9][0-9_]*\.[0-9_]+|[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^:]+|==|!=|<=|>=|\${|}|\$|<|>));
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupMakefile() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Targets (words before :)
    rule = {}; rule.pattern = QRegularExpression(
        R"(^[A-Za-z0-9_.-]+(?=\s*:)|^\t+[A-Za-z0-9_.-]+(?=\s*\())");
    rule.format = _functionFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$[A-Za-z_][A-Za-z0-9_]*|\$\{[A-Za-z_][A-Za-z0-9_]*\}|\$\([A-Za-z_][A-Za-z0-9_]*\))");
    rule.format = _typeFormat; _rules.append(rule);

    // Automatic variables
    rule = {}; rule.pattern = QRegularExpression(R"(\$@|\$<|\$^\|\$+|\$\?|\$\*|\$|)"));
    rule.format = _attributeFormat; _rules.append(rule);

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(ifndef|ifdef|ifeq|ifneq|else|endif|define|endef|override|export|unexport|include|-include|sinclude|vpath)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\$[A-Za-z][A-Za-z0-9_-]*\()");
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9][0-9_]*\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Line continuation
    rule = {}; rule.pattern = QRegularExpression(R"(\\$)");
    rule.format = _operatorFormat; _rules.append(rule);
}


// -------------------------------------------------------------------------
// Data / Config: JSON, YAML, Markdown, INI, Properties, NFO
// -------------------------------------------------------------------------

void SyntaxHighlighter::setupJson() {
    HighlightRule rule;

    // Strings (keys and values)
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b-?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|-?[0-9]+[eE][+-]?[0-9]+|-?[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Booleans and null
    rule = {}; rule.pattern = QRegularExpression(R"(\b(true|false|null)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Brackets and punctuation
    rule = {}; rule.pattern = QRegularExpression(R"([{}\[\]:,])");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupYaml() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Booleans and null
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(true|false|yes|no|on|off|~|null|Null|NULL|y|Y|n|N)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Document markers
    rule = {}; rule.pattern = QRegularExpression(R"(^---\s*$|^...\s*$)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Anchors and aliases
    rule = {}; rule.pattern = QRegularExpression(R"(&[a-zA-Z_][a-zA-Z0-9_]*|\*[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Tags
    rule = {}; rule.pattern = QRegularExpression(R"(!<!?>[\w:/.]+|!![\w:/.]+|\?!)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Keys (word before colon at start of line or after indent)
    rule = {}; rule.pattern = QRegularExpression(
        R"(^[\s]*[a-zA-Z_][a-zA-Z0-9_\-.](?=\s*:)|[\s:][a-zA-Z_][a-zA-Z0-9_\-.]+(?=\s*:))");
    rule.format = _functionFormat; _rules.append(rule);

    // Strings (quoted)
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0[xX][0-9a-fA-F_]+|0[bB][01_]+|0[oO][0-7_]+|-?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?|-?[0-9]+[eE][+-]?[0-9_]+|-?[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // List markers
    rule = {}; rule.pattern = QRegularExpression(R"(^[\s]*-\s+)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Colons and brackets
    rule = {}; rule.pattern = QRegularExpression(R"([{}\[\]:,\|>])");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupMarkdown() {
    _singleLineComment = "";

    HighlightRule rule;

    // ATX headings
    rule = {}; rule.pattern = QRegularExpression(R"(^#{1,6}\s+.*$)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Bold
    rule = {}; rule.pattern = QRegularExpression(R"(\*\*[^*]+\*\*|__[^_]+__)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Italic
    rule = {}; rule.pattern = QRegularExpression(R"(\*[^*]+\*|_[ ^_]+_)");
    rule.format = _stringFormat; _rules.append(rule);

    // Strikethrough
    rule = {}; rule.pattern = QRegularExpression(R"(~~[^~]+~~)");
    rule.format = _commentFormat; _rules.append(rule);

    // Inline code
    rule = {}; rule.pattern = QRegularExpression(R"(`[^`]+`)");
    rule.format = _stringFormat; _rules.append(rule);

    // Fenced code blocks
    rule = {}; rule.pattern = QRegularExpression(R"(```[\w]*[\s\S]*?```|^```\w*)");
    rule.format = _stringFormat; _rules.append(rule);

    // Links and images
    rule = {}; rule.pattern = QRegularExpression(R"(!?\[[^\]]*\]\([^\)]+\))");
    rule.format = _attributeFormat; _rules.append(rule);

    // Blockquote
    rule = {}; rule.pattern = QRegularExpression(R"(^>\s+.*$)");
    rule.format = _commentFormat; _rules.append(rule);

    // Horizontal rule
    rule = {}; rule.pattern = QRegularExpression(R"(^[-*_]{3,}\s*$)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Unordered list
    rule = {}; rule.pattern = QRegularExpression(R"(^[\s]*[-*+]\s+)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Ordered list
    rule = {}; rule.pattern = QRegularExpression(R"(^[\s]*\d+\.\s+)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Task list
    rule = {}; rule.pattern = QRegularExpression(R"(- \[[ x]\]\s)");
    rule.format = _attributeFormat; _rules.append(rule);

    // HTML tags
    rule = {}; rule.pattern = QRegularExpression(R"(<[/]?[a-zA-Z][^>]*>)");
    rule.format = _keywordFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupIni() {
    _singleLineComment = ";";
    HighlightRule rule;

    // Section headers
    rule = {}; rule.pattern = QRegularExpression(R"(^\[[^\]]+\])");
    rule.format = _keywordFormat; _rules.append(rule);

    // Keys
    rule = {}; rule.pattern = QRegularExpression(R"(^[a-zA-Z_][a-zA-Z0-9_\-.]*(?==|:))");
    rule.format = _functionFormat; _rules.append(rule);

    // Values
    rule = {}; rule.pattern = QRegularExpression(R"((?:=\s*)(.+)$)");
    rule.format = _stringFormat; _rules.append(rule);

    // Booleans
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(true|false|yes|no|on|off|null|nil|0|1)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b-?[0-9]+\.[0-9]+|-?[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupNfo() {
    HighlightRule rule;

    // ANSI/VT100 escape codes
    rule = {}; rule.pattern = QRegularExpression(R"(\x1b\[[0-9;]*m)");
    rule.format = _preprocessorFormat; _rules.append(rule);

    // Box drawing characters
    rule = {}; rule.pattern = QRegularExpression(
        R"([╔╗╚╝╠╣╦╩╬═║─│┌┐└┘├┤┬┴┼╭╮╯╰╱╲╳╴╵╶╷╸╼╽╾╿░▒▓█▄▀■□▪▫▬▲△▼▽◆◇○◎●◐◑◒◓◔◕◖◗◘◙◚◛◜◝◞◟◠◡⊙⊕⊖⊗⊘⊙⊚⊛⊜⊝⌀⌂⌁⌃⌄⌅⌆⌇⌈⌉⌊⌋⌌⌍⌎⌏⌐⌑⌒⌓⌔⌕⌖⌗⌘⌙⌚⌛⌍⌬⌭⌮⌯⌰⌱⌲⌳⌴⌵⌶⌷⌸⌹⌺⌻⌼⌽⌾⌿⍀⍁⍂⍃⍄⍅⍆⍇⍈⍉⍊⍋⍌⍍⍎⍏⍐⍑⍒⍓⍔⍕⍖⍗⍘⍙⍚⍛⍜⍝⍞⍟⍠⍡⍢⍣⍤⍥⍦⍧⍨⍩⍪⍫⍬⍭⍮⍯⍰⍱⍲⍳⍴⍵⍶⍷⍸⍹⍺⍻⎔])");
    rule.format = _keywordFormat; _rules.append(rule);

    // Block art
    rule = {}; rule.pattern = QRegularExpression(R"([█▀▄▌▐░▒▓▒░])");
    rule.format = _numberFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupProperties() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Comments including !MESSAGE
    rule = {}; rule.pattern = QRegularExpression(R"(^[#!][^\n]*)");
    rule.format = _commentFormat; _rules.append(rule);

    // Keys (before = or :)
    rule = {}; rule.pattern = QRegularExpression(
        R"(^[a-zA-Z_/][a-zA-Z0-9_\-./]+(?==|:))");
    rule.format = _functionFormat; _rules.append(rule);

    // Escape sequences
    rule = {}; rule.pattern = QRegularExpression(R"(\\[tbnrfo"'\\])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Unicode escapes
    rule = {}; rule.pattern = QRegularExpression(R"(\\u[0-9a-fA-F]{4}|\\x[0-9a-fA-F]{2})");
    rule.format = _numberFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b-?[0-9]+\.[0-9]+|-?[0-9]+)\b)");
    rule.format = _numberFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupDefault() {
    _singleLineComment = "";
    _commentStart = "";
    _commentEnd = "";
    HighlightRule rule;
    // Plain text: no highlighting rules, everything stays default
    Q_UNUSED(rule);
}

void SyntaxHighlighter::setupSql() {
    _singleLineComment = "--";
    _commentStart = "/*";
    _commentEnd = "*/";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(SELECT|FROM|WHERE|INSERT|UPDATE|DELETE|CREATE|DROP|ALTER|TABLE|INDEX|VIEW|DATABASE|SCHEMA|INTO|VALUES|SET|JOIN|LEFT|RIGHT|INNER|OUTER|FULL|CROSS|ON|AND|OR|NOT|IN|LIKE|BETWEEN|IS|NULL|TRUE|FALSE|AS|DISTINCT|ORDER|BY|ASC|DESC|GROUP|HAVING|LIMIT|OFFSET|UNION|ALL|EXISTS|CASE|WHEN|THEN|ELSE|END|PRIMARY|KEY|FOREIGN|REFERENCES|CONSTRAINT|DEFAULT|CHECK|UNIQUE|AUTO_INCREMENT|SERIAL|BIGSERIAL|UUID|TIMESTAMP|DATE|TIME|DATETIME|COMMIT|ROLLBACK|SAVEPOINT|BEGIN|TRANSACTION|WITH|RECURSIVE|OVER|PARTITION|WINDOW|RETURNING|USING|NATURAL|MATCHED|MERGE|UPSERT|ON CONFLICT|DO UPDATE|DO NOTHING|CASCADE|RESTRICT|TRUNCATE|GRANT|REVOKE)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(COUNT|SUM|AVG|MIN|MAX|COALESCE|NULLIF|CAST|CONVERT|CONCAT|SUBSTRING|TRIM|UPPER|LOWER|LENGTH|REPLACE|ROUND|FLOOR|CEIL|ABS|NOW|CURRENT_DATE|CURRENT_TIME|CURRENT_TIMESTAMP|EXTRACT|DATE_PART|TO_CHAR|TO_DATE|TO_NUMBER|JSON_AGG|JSONB_AGG|JSON_OBJECT_AGG|ARRAY_AGG|STRING_AGG|ROW_NUMBER|RANK|DENSE_RANK|NTILE|LAG|LEAD|FIRST_VALUE|LAST_VALUE|CUME_DIST|PERCENT_RANK)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(INT|INTEGER|BIGINT|SMALLINT|TINYINT|FLOAT|DOUBLE|DECIMAL|NUMERIC|REAL|SERIAL|BIGSERIAL|MONEY|BOOLEAN|BOOL|CHAR|VARCHAR|TEXT|BLOB|BYTEA|UUID|JSON|JSONB|XML|ARRAY|ENUM|POINT|LINE|CIRCLE|INET|CIDR|MACADDR|TSVECTOR|TSQUERY|TSRANGE|TSMULTIRANGE)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)*'|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b-?[0-9]+\.?[0-9]*\b|\b0x[0-9a-fA-F]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!|&^~]+|=>|->|::|\|\||&&)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupPascal() {
    _singleLineComment = "//";
    _commentStart = "(*";
    _commentEnd = "*)";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(program|uses|var|const|type|procedure|function|begin|end|if|then|else|case|of|for|to|downto|while|repeat|until|record|class|interface|implementation|initialization|finalization|inherited|raise|try|except|finally|with|as|is|nil|true|false|and|or|not|xor|shl|shr|div|mod|inherited|absolute|abstract|assembler|automated|cdecl|default|deprecated|dispid|dynamic|export|external|far|forward|generic|implements|index|message|near|on|out|overload|override|package|pascal|private|protected|public|published|read|readonly|register|reintroduce|requires|resident|safecall|softbool|stdcall|stored|volatile|write|writeonly)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(integer|int64|int32|int16|int8|cardinal|longint|shortint|smallint|byte|word|boolean|bytebool|wordbool|longbool|char|string|widestring|ansistring|unicodestring|real|double|single|extended|comp|currency|text|file|pointer|type|array|set|shortstring|pchar|pwidechar|array of|file of)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\$[0-9a-fA-F]+\b|\b[0-9]+\.?[0-9]*([eE][+-]?[0-9]+)?\b|%[01]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(\{[^}]*\})");
    rule.format = _commentFormat; _rules.append(rule);

    // Directives
    rule = {}; rule.pattern = QRegularExpression(R"(@[a-zA-Z_][a-zA-Z0-9_]*)");
    rule.format = _attributeFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupFortran() {
    _singleLineComment = "!";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(PROGRAM|SUBROUTINE|FUNCTION|MODULE|CONTAINS|END|USE|IMPLICIT|INCLUDE|PUBLIC|PRIVATE|ALLOCATABLE|ALLOCATE|DEALLOCATE|NULLIFY|IF|THEN|ELSE|ELSEIF|ENDIF|DO|WHILE|SELECT CASE|CASE|CASE DEFAULT|EXIT|CYCLE|STOP|RETURN|PAUSE|CONTINUE|GO TO|ASSIGN|ASSIGNMENT|POINTER|TARGET|SAVE|DIMENSION|INTENT|OPTIONAL|REQUIRE|RESULT|Volatile|ENTRY|DO WHILE|DO UNTIL|forall|where|elsewhere|block|data|format|common|equivalence|dimension|external|intrinsic|namelist|parameter|access|action|blank|direct|err|form|formatted|iostat|name|newunit|pad|position|readonly|record|recl|sequential|status|unformatted|unit|advance|比|Line|size|eor|end|file|fmt|iocontrol|keyval|msg|mult|number|pos|rec|rms|size|source|stat|unit)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Intrinsic functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(ABS|ACHAR|ACOS|ACOSH|ADJUSTL|ADJUSTR|AIMAG|AINT|ALOG|ALOG10|AMAX0|AMAX1|AMIN0|AMIN1|AMOD|ANINT|ASIN|ASINH|ATAN|ATAN2|ATANH|BTEST|CHAR|CMPLX|CONJG|COS|COSH|DOT_PRODUCT|DPROD|DSQRT|DSIN|DCOS|DTAN|EXP|FLOAT|FLOOR|ICHAR|INDEX|INT|ISNAN|LOG|LOG10|MAX|MAX0|MAX1|MIN|MIN0|MIN1|MOD|NINT|REAL|SIGN|SIN|SINH|SQRT|TAN|TANH|TINY|TRANSFER|TRIM|UBOUND|LBOUND|Shape|Size|associated|allocated|lbound|ubound|move_alloc|null|ref)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Intrinsic types
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(INTEGER|REAL|DOUBLE PRECISION|COMPLEX|LOGICAL|CHARACTER|TYPE|KIND|LEN|class|procedure|function|subroutine|interface|module)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _typeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)*'|"(?:[^"\\]|\\.)*")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9]+\.?[0-9]*([eEdD][+-]?[0-9]+)?\b|\b[0-9]+\.[0-9]*\b|\.[0-9]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Line continuation
    rule = {}; rule.pattern = QRegularExpression(R"(\&\s*$)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Labels (leading numbers)
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*[0-9]+(?=\s))", QRegularExpression::MultilineOption);
    rule.format = _labelFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|\.]+|==|\/=|> |<|>=|<=|\.AND\.|\.OR\.|\.NOT\.|\.EQ\.|\.NE\.|\.GT\.|\.LT\.|\.GE\.|\.LE\.|\.TRUE\.|\.FALSE\.)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupR() {
    _singleLineComment = "#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(if|else|ifelse|for|while|repeat|next|break|return|NULL|TRUE|FALSE|NA|NaN|Inf|NA_integer_|NA_real_|NA_complex_|NA_character_|function|library|require|attach|detach|setwd|getwd|source|save|load|save.image|saveRDS|readRDS|install.packages|requireNamespace|stop|warning|message|try|tryCatch|finally|return|switch|in|...)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(c|list|data.frame|matrix|array|factor|integer|numeric|character|logical|complex|raw|vector|matrix|array|table|ts|zoo|xts|strptime|as\.|is\.|seq|rep|seq_len|seq_along|which|which.min|which.max|order|sort|sample|set.seed|duplicated|unique|merge|outer|inner|crossprod|t|diag|lower.tri|upper.tri|sweep|apply|lapply|sapply|tapply|mapply|vapply|rep.int|rle|transform|within|by|aggregate|reshape|cbind|rbind|subset|transform|with|na\.|complete\.cases|scale|quantile|median|mean|sum|prod|min|max|range|var|sd|cov|cor|mad|abs|sign|sqrt|floor|ceiling|round|trunc|exp|log|log10|log2|sin|cos|tan|asin|acos|atan|sinh|cosh|tanh|asinh|acosh|atanh|factorial|choose|combn|pmax|pmin|is\.na|is\.null|is\.numeric|is\.character|is\.factor|is\.matrix|is\.data\.frame|is\.list|is\.logical|is\.integer|is\.double|as\.numeric|as\.character|as\.integer|as\.logical|as\.factor|as\.matrix|as\.data\.frame|as\.list|paste|substr|nchar|strsplit|gsub|sub|grep|grepl|fixed|regexpr|gregexpr|agrep|tolower|toupper|casefold|chartr|nrow|ncol|dim|dimnames|names|rownames|colnames|length|levels|unlist|attributes|attr|structure|deparse|substitute|bquote|expression|eval|parse|textConnection|url|download\.file|file|dir|dir\.create|file\.create|file\.remove|file\.rename|file\.copy|list\.files|list\.dir)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0x[0-9a-fA-F]+\b|\b[0-9]+\.?[0-9]*([eE][+-]?[0-9]+)?\b|\.nan\b|\.inf\b|\.NaN\b|\.Inf\b|\.Integer_\b|\.Real_\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Special operators
    rule = {}; rule.pattern = QRegularExpression(R"(<-|<<-|->|->>|::|:::|\$|@|%[^%\s]+%|~|<|>|<=|>=|==|!=|&&|&|\|\||\||:|=)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Comments (already via _singleLineComment but explicit helps)
    rule = {}; rule.pattern = QRegularExpression(R"(#[^\n]*)");
    rule.format = _commentFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupTex() {
    _singleLineComment = "%";
    HighlightRule rule;

    // Commands (backslash sequences)
    rule = {}; rule.pattern = QRegularExpression(
        R"(\\[a-zA-Z_][a-zA-Z0-9_]*(?:\[[^\]]*\])?|\%[a-zA-Z]+)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Environments
    rule = {}; rule.pattern = QRegularExpression(
        R"(\\begin\s*\{[^}]+\}|\\end\s*\{[^}]+\})");
    rule.format = _functionFormat; _rules.append(rule);

    // Groups
    rule = {}; rule.pattern = QRegularExpression(R"(\{[^{}]*\})");
    rule.format = _typeFormat; _rules.append(rule);

    // Math delimiters
    rule = {}; rule.pattern = QRegularExpression(R"(\$\$?|[{}\[\]()=])");
    rule.format = _operatorFormat; _rules.append(rule);

    // Special characters
    rule = {}; rule.pattern = QRegularExpression(R"(\\[#$%&_{}])");
    rule.format = _attributeFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(%[^\n]*)");
    rule.format = _commentFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupDiff() {
    _singleLineComment = "";
    HighlightRule rule;

    // File headers
    rule = {}; rule.pattern = QRegularExpression(R"(^(?:---|\+\+\+|@@|\=\=\=)\s)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Context lines
    rule = {}; rule.pattern = QRegularExpression(R"(^ )");
    rule.format = _stringFormat; _rules.append(rule);

    // Added lines
    rule = {}; rule.pattern = QRegularExpression(R"(^\+)");
    rule.format = _functionFormat; _rules.append(rule);

    // Removed lines
    rule = {}; rule.pattern = QRegularExpression(R"(^-)");
    rule.format = _commentFormat; _rules.append(rule);

    // Line numbers in unified diff
    rule = {}; rule.pattern = QRegularExpression(
        R"(-?\d+(?:,?\d+)?(?=\s))");
    rule.format = _numberFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupJulia() {
    _singleLineComment = "#";
    _commentStart = "#=";
    _commentEnd = "=#";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(begin|break|catch|continue|do|else|elseif|end|export|finally|for|function|global|if|import|in|let|local|macro|module|return|struct|mutable|abstract|primitive|type|using|where|while|yield|as|isa|thinarrow|dot|pair|@|?|:|;;|->|<:|>&))\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in types / values
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(nothing|true|false|Any|Nothing|Union|All|Nothing|Missing|UndefRefError|Core|Base|Main|IO|IOStream|String|Symbol|Number|Integer|Int|Int8|Int16|Int32|Int64|Int128|UInt8|UInt16|UInt32|UInt64|UInt128|Float16|Float32|Float64|Bool|Char|DataType|Union|AbstractArray|AbstractMatrix|AbstractVector|Array|DenseArray|Vector|Matrix|Tuple|NamedTuple|Dict|Pair|Set|BitSet|Channel|Condition|Ref|Ptr|Symbol|Exception|ErrorException|LoadError|Method|MethodError|TypeError|ArgumentError|BoundsError|DivideError|DomainError|InexactError|InterruptException|InvalidStateError|KeyError|OutOfMemoryError|ReadOnlyMemoryError|SetambiguousError|StackOverflowError|SystemError|UndefRefError|UndefVarError|ParseError|OverflowError|Meta|Lowered| gensym|code_typed|code_warntype|code_lowered|code_llvm|code_native|expand|eval|require|include|using|import|export|public|private|primitive|immutable|abstract|mutable)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _typeFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(print|println|printstyled|@printf|sprint|show|display|write|push!|pop!|append!|insert!|delete!|splice!|append!|prepend!|empty!|sizehint!|resize!|length|size|ndims|eltype|keytype|valtype|collect|iterate|next|skip|done|first|last|get|get!|setindex!|push|pop|shift!|unshift!|insert!|deleteat!|splice!|append!|prepend!|merge|merge!|union|intersect|setdiff|symdiff|issubset|issetequal|unique|nonunique|allunique|sort|sort!|sortperm|sortrows|sortcols|permutedims|invokelatest|applicable|isequal|isless|isidentical|isimmutable|ismutable|isconcretetype|isabstracttype|isdispatchtuple|subtypes|supertype|basesuper|traits|fieldnames|fieldoffset|fieldtype|typeof|sizeof|isdefined|isassigned|hasproperty|getproperty|setproperty!|propertynames|hasmethod|hasmethod|applicable|invoke)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(
        R"("(?:[^"\\]|\\.)*"|"""[\s\S]*?""")");
    rule.format = _stringFormat; _rules.append(rule);

    // Char
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0x[0-9a-fA-F_]+p[+-]?\d+|\b0x[0-9a-fA-F_]+|0b[01_]+|0o[0-7_]+|\b[0-9][0-9_]*\.?[0-9_]*([eE][+-]?[0-9_]+)?\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Macro
    rule = {}; rule.pattern = QRegularExpression(R"(@\w+)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~?:]+|=>|->|::|\.\.\.|\.\.|\\|\|\||&&|<<|>>|>>>|\bx\b)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupMatlab() {
    _singleLineComment = "%";
    HighlightRule rule;

    // Keywords
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(function|end|if|elseif|else|for|while|switch|case|otherwise|try|catch|global|persistent|parfor|spmd|return|break|continue|pause|quit|dbstep|dbstop|dbclear|dbcont|dbdown|dbstack|dbstatus|dbtype|dbup|keyboard|assert|error|warning|throw|MException|rethrow|addlistener|notify|delete|detachedConstructor|classdef|properties|methods|events|enumeration|import|export|meta\.|persistent|clear|clc|close|who|whos|what|which|pathdef|userpath|matlabpath|matlabroot|ans|eps|realmax|realmin|pi|intmax|intmin|inf|nan|true|false|yes|no)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Built-in functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(abs|acos|acosh|asin|asinh|atan|atan2|atanh|ceil|char|compan|conj|corrcoef|cos|cosh|cov|cross|cumprod|cumsum|deblank|det|diag|dot|eig|exp|expm|eye|factor|factorial|fft|fftn|fftshift|filter|floor|format|fprintf|full|function|funm|gcd|hess|hist|histc|hold|horzcat|ifft|ifftn|imag|imfilter|imread|imresize|imshow|int16|int32|int8|inv|ipermute|ischar|isempty|isequal|isequaln|isfield|isfloat|isinf|isinteger|isletter|islogical|ismatrix|isnan|isnumeric|isobject|isreal|isscalar|isspace|isstruct|isvector|length|linspace|log|log10|log2|logical|lookup|lscov|lsqnonneg|lu|max|mean|median|min|mod|mvnrnd|nanmax|nanmean|nanmedian|nanmin|nansum|nchoosek|nextpow2|nnz|norm|normest|null|num2str|numel|numerator|ones|orth|parse|perms|permute|pie|pinv|planerot|poly|polyder|polyeig|polyfit|polyint|polyval|polyvalm|pow2|primes|prod|qr|qrdelete|qrinsert|quad|quadl|qmr|qz|rand|randi|randn|randperm|range|rcond|rdivide|real|reallog|realpow|realsqrt|remove|repmat|reshape|residue|roots|rot90|round|rsf2csf|saddle|schur|setdiff|setfield|setxor|sign|sin|sinh|size|sqrt|sqrtm|squeeze|std|str2double|str2func|strcat|strcmp|strcmpi|strfind|strmatch|strncmp|strncmpi|strtok|strtrim|struct|struct2cell|subsasgn|subsindex|subspace|subsref|substruct|sub2ind|sum|suppressWarnings|svd|svds|sym|syminv|symvar|tan|tanh|tic|toc|toeplitz|trace|transpose|trapz|tril|triu|type|uint16|uint32|uint8|uminus|undo|unique|unix|unmkpp|unwrap|uplus|upper|vander|var|vecnorm|ver|version|vertcat|wait|waitbar|warn|warning|what|which|who|whos|wilkinson|word|write|xlable|xlabel|xlim|xlsread|xlswrite|xor|zeros|filedata|inputname|mfircl|\w+plot\w+|\w+read\w+|\w+write\w+|\w+open\w+|\w+save\w+)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"('(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b[0-9]+\.?[0-9]*([eE][+-]?[0-9]+)?\b|\b[0-9]+\.[0-9]*\b|\.[0-9]+\b|0x[0-9a-fA-F]+\b|i\b|j\b|Inf\b|NaN\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(%\{[\s\S]*?\%\}|%[^\n]*)");
    rule.format = _commentFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|^~':]+|&&|\|\||==|~=|<=|>=|:\s*|@\w+)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupNsis() {
    _singleLineComment = ";";
    _commentStart = "/*";
    _commentEnd = "*/";
    HighlightRule rule;

    // Compiler directives
    rule = {}; rule.pattern = QRegularExpression(
        R"(!include|!define|!undef|!if|!ifdef|!ifndef|!else|!endif|!macro|!macroend|!system|!error|!warning|!verbose|!addincludedir|!addplugindir|!cd|!packheader|!searchparse|!searchreplace|!appendfile|!echo|!execute|!packhdr|!finalize|!getdllversion|!getFileVersion|!getTLBVersion|!insertmacro|!define|!undef|!pragma)\b)");
    rule.format = _keywordFormat; _rules.append(rule);

    // Sections and functions
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Section|SectionEnd|SectionIn|SectionGroup|SectionGroupEnd|Function|FunctionEnd|PageCustom|PageEx|PageExEnd|LeaveFunction|FunctionLib|Callback|CallFunction|CallRegisterFunction|FunctionUsed)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _functionFormat; _rules.append(rule);

    // Variables and registers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(Var|Var/Global|IntFmt|IntCmp|IntCmpU|StrCmp|StrCpy|StrLen|Exch|Pop|Push|ClearErrors|SetErrors|FileOpen|FileClose|FileRead|FileWrite|FileSeek|ReadEnvStr|ExpandEnvStrings|ReadRegStr|WriteRegStr|ReadRegDWORD|WriteRegDWORD|DeleteRegKey|DeleteRegValue|Delete|ReserveFile|RMDir|CreateDirectory|CopyFiles|CreateShortCut|GetFullPathName|GetShortPathName|GetTempFileName|SearchPath|GetDLLVersion|GetFileTime|GetFileVersion|GetExeVersion|GetExeVersionNSIS|Exec|ExecShell|ExecWait|ShellLink|Abort|Quit|MessageBox|ShowInstDetails|ShowUninstDetails|ComponentText|TitleText|Background|SetFont|InstallColors|UninstallText|UninstallIcon|UninstallCaption|InstallDir|InstallDirRegKey|RequestExecutionLevel|OutFile|InstallButtonText|UninstallButtonText|DetailsButtonText|FinishText|BrandingText|ComponentAlias|Icon|Unicon|X坝|GPBanner|Manifest|streams)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _builtInFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0x[0-9a-fA-F]+\b|\b[0-9]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Comments
    rule = {}; rule.pattern = QRegularExpression(R"(#.*$|\/\/.*$)", QRegularExpression::MultilineOption);
    rule.format = _commentFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([+\-*/%=<>!&|\^~]+|\|\||&&|<<|>>)");
    rule.format = _operatorFormat; _rules.append(rule);
}

void SyntaxHighlighter::setupBatch() {
    _singleLineComment = "REM";
    HighlightRule rule;

    // Keywords / commands
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b(call|cd|chdir|cls|cmd|color|copy|del|dir|diskcomp|diskcopy|doskey|echo|endlocal|erase|exit|find|format|goto|if|keyb|label|md|mkdir|move|path|pause|popd|prompt|pushd|rd|ren|rename|rmdir|set|setlocal|shift|start|title|type|verify|vol|at|Attrib|Backup|Chkdsk|Choice|Cmp|Compare|Compact|Convert|Copy|Cscc|Cttune|Defrag|Deltree|Dir|H-diskcomp|Diskcopy|Doskey|Edit|Edlin|Expand|Extract|Fastopen|Fdisk|Find|Findstr|Format|Graftabl|Graphics|Interlnk|Intersvr|Keyb|Keybgr|Label|LH|Loadfix|Loadhigh|Lock|Mode|More|Move|Msbackup|Msdex|Mscdex|Nlsfunc|Power|Print|Prompt|Pt|QBASIC|Recover|Replace|Restore|Rmtool|Ro|ScanDisk|Setver|Share|Smartdrv|Sort|Spooler|St档 Nub|Subst|Sys|Tree|Undelete|Unformat|Vsafe|XCpy|XCopy)\b)",
        QRegularExpression::CaseInsensitiveOption);
    rule.format = _keywordFormat; _rules.append(rule);

    // Labels
    rule = {}; rule.pattern = QRegularExpression(R"(^\s*:[A-Za-z_][A-Za-z0-9_]*)");
    rule.format = _labelFormat; _rules.append(rule);

    // Labels (goto targets)
    rule = {}; rule.pattern = QRegularExpression(R"(:[A-Za-z_][A-Za-z0-9_]*\b)");
    rule.format = _functionFormat; _rules.append(rule);

    // Variables
    rule = {}; rule.pattern = QRegularExpression(
        R"(%[a-zA-Z_][a-zA-Z0-9_]*%|%[0-9]|\%[a-zA-Z_][a-zA-Z0-9_]*:\~[^%]*%|![^!]+!|\%\*|\%\#|\%\~[dpnfsx0-9]*\b)");
    rule.format = _typeFormat; _rules.append(rule);

    // Delayed expansion variables
    rule = {}; rule.pattern = QRegularExpression(R"(!\w+!)");
    rule.format = _attributeFormat; _rules.append(rule);

    // Strings
    rule = {}; rule.pattern = QRegularExpression(R"("(?:[^"]*)")");
    rule.format = _stringFormat; _rules.append(rule);

    // Numbers
    rule = {}; rule.pattern = QRegularExpression(
        R"(\b0x[0-9a-fA-F]+\b|\b[0-9]+\b)");
    rule.format = _numberFormat; _rules.append(rule);

    // Operators
    rule = {}; rule.pattern = QRegularExpression(R"([&|<>]+\s*|&&|\|\||==|EQU|NEQ|LSS|LEQ|GTR|GEQ)");
    rule.format = _operatorFormat; _rules.append(rule);

    // Comments (REM or ::)
    rule = {}; rule.pattern = QRegularExpression(
        R"(^(\s*)(REM\s.*|::.*$)", QRegularExpression::MultilineOption);
    rule.format = _commentFormat; _rules.append(rule);
}

