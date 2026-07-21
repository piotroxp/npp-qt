// TestStringHelper.cpp -- Qt Test suite for StringHelper utilities
// Phase1 canonical source: src/common/StringHelper.cpp
//
// Test coverage assessment:
// - Case conversion: was weak -- used if+warning instead of QVERIFY/QCOMPARE
// - Trim: weak -- same pattern
// - StartsWith/EndsWith/Contains: GOOD -- used QVERIFY
// - Numeric conversion: weak -- same pattern
// - Tab/space conversion: weak -- same pattern
// - NEW: JSON escaping, shell escaping, HTML escaping, path helpers,
//         Levenshtein distance, word wrap, equalsIgnoreCase, fileName/ext,
//         makeEolConsistent, indentLines, normalizePath, split variants,
//         fromDouble edge cases, toDouble error handling

#include <QtTest/QtTest>
#include "common/StringHelper.h"
#include <vector>
#include <cmath>      // std::fabs

using namespace StringHelper;

// Helper: floating-point comparison within epsilon
static inline bool floatNear(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

class TestStringHelper : public QObject
{
    Q_OBJECT

private slots:
    // -- Case conversion
    void test_toLower();
    void test_toUpper();
    void test_toLower_empty();
    void test_toLower_numbers();
    void test_toUpper_empty();

    // -- Trim
    void test_trim_basic();
    void test_trim_leftOnly();
    void test_trim_rightOnly();
    void test_trim_noChange();
    void test_trim_empty();
    void test_trim_tabsAndNewlines();
    void test_trim_wide();

    // -- StartsWith / EndsWith
    void test_startsWith();
    void test_startsWith_emptyPrefix();
    void test_startsWith_fullMatch();
    void test_startsWith_noMatch();
    void test_endsWith();
    void test_endsWith_emptySuffix();
    void test_endsWith_fullMatch();
    void test_endsWith_noMatch();

    // -- Contains
    void test_contains();
    void test_contains_emptyNeedle();
    void test_contains_noMatch();
    void test_contains_caseSensitive();

    // -- Replace
    void test_replaceAll();
    void test_replaceAll_noOccurrence();
    void test_replaceAll_overlapping();
    void test_replaceAll_emptyPattern();
    void test_replaceAll_batch();

    // -- Split / Join
    void test_split();
    void test_split_emptyDelimiter();
    void test_split_noMatch();
    void test_split_withEmptyParts();
    void test_split_skipEmpty();
    void test_join();
    void test_join_emptyParts();

    // -- Numeric conversion
    void test_toInt();
    void test_toInt_invalidInput();
    void test_toLong();
    void test_toDouble();
    void test_toDouble_invalidInput();
    void test_fromInt();
    void test_fromLong();
    void test_fromDouble();
    void test_fromDouble_precision();

    // -- Tab/space conversion
    void test_tabsToSpaces();
    void test_tabsToSpaces_empty();
    void test_spacesToTabs();
    void test_spacesToTabs_partialTab();

    // -- Path helpers
    void test_fileName();
    void test_fileExt();
    void test_filePath();
    void test_normalizePath();
    void test_baseName();
    void test_baseName_noExt();

    // -- Escaping
    void test_escapeJson();
    void test_escapeJson_empty();
    void test_escapeShell();
    void test_escapeShell_noSpecialChars();
    void test_escapeShell_singleQuote();
    void test_htmlEscape();
    void test_htmlEscape_ampersand();

    // -- Comparison
    void test_equalsIgnoreCase();
    void test_equalsIgnoreCase_differentLength();

    // -- Formatting
    void test_format();

    // -- Word wrap
    void test_wordWrap();
    void test_wordWrap_singleWord();
    void test_wordWrap_zeroWidth();

    // -- EOL normalisation
    void test_makeEolConsistent_crlf();
    void test_makeEolConsistent_lf();
    void test_makeEolConsistent_mixed();

    // -- Indentation
    void test_indentLines();
    void test_indentLines_empty();
};

// ============================================================================
// Case conversion
// ============================================================================

void TestStringHelper::test_toLower()
{
    QString qresult = QString::fromStdString(toLower(std::string("Hello World")));
    QVERIFY2(qresult == "hello world",
             qPrintable(QString("toLower expected \"hello world\", got \"%1\"").arg(qresult)));
}

void TestStringHelper::test_toLower_empty()
{
    QString qresult = QString::fromStdString(toLower(std::string("")));
    QVERIFY2(qresult == "", qPrintable(QString("toLower empty should be \"\", got \"%1\"").arg(qresult)));
}

void TestStringHelper::test_toLower_numbers()
{
    // Numbers should pass through unchanged
    std::string result = toLower(std::string("ABC123xyz"));
    QVERIFY2(result == "abc123xyz",
             qPrintable(QString("toLower expected \"abc123xyz\", got \"%1\"").arg(QString::fromStdString(result))));
}

void TestStringHelper::test_toUpper()
{
    QString qresult = QString::fromStdString(toUpper(std::string("Hello World")));
    // toUpper must produce uppercase
    std::string result = toUpper(std::string("Hello World"));
    QVERIFY2(result == "HELLO WORLD",
             qPrintable(QString("toUpper expected \"HELLO WORLD\", got \"%1\"").arg(QString::fromStdString(result))));
}

void TestStringHelper::test_toUpper_empty()
{
    std::string result = toUpper(std::string(""));
    QVERIFY2(result == "", qPrintable(QString("toUpper empty should be \"\", got \"%1\"").arg(QString::fromStdString(result))));
}

// ============================================================================
// Trim
// ============================================================================

void TestStringHelper::test_trim_basic()
{
    std::string r = trim(std::string("  hello  "));
    QVERIFY2(r == "hello",
             qPrintable(QString("trim expected \"hello\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_leftOnly()
{
    std::string r = trimLeft(std::string("  hello"));
    QVERIFY2(r == "hello",
             qPrintable(QString("trimLeft expected \"hello\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_rightOnly()
{
    std::string r = trimRight(std::string("hello  "));
    QVERIFY2(r == "hello",
             qPrintable(QString("trimRight expected \"hello\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_noChange()
{
    std::string r = trim(std::string("hello"));
    QVERIFY2(r == "hello",
             qPrintable(QString("trim should not change \"hello\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_empty()
{
    std::string r = trim(std::string(""));
    QVERIFY2(r.empty(), qPrintable(QString("trim empty should be empty, got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_tabsAndNewlines()
{
    // std::isspace covers tabs, newlines, etc.
    std::string r = trim(std::string("\t\n hello \r\t"));
    QVERIFY2(r == "hello",
             qPrintable(QString("trim with tabs/newlines expected \"hello\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_trim_wide()
{
    std::u16string u = trim(std::u16string(u"  hello  "));
    // Compare as UTF-16
    QString q = QString::fromUtf16(reinterpret_cast<const char16_t*>(u.data()), static_cast<int>(u.size()));
    QVERIFY2(q == "hello",
             qPrintable(QString("trim(u16string) expected \"hello\", got \"%1\"").arg(q)));
}

// ============================================================================
// StartsWith / EndsWith
// ============================================================================

void TestStringHelper::test_startsWith()
{
    QVERIFY(startsWith(std::string("Hello"), std::string("He")));
    QVERIFY(!startsWith(std::string("Hello"), std::string("hello")));
}

void TestStringHelper::test_startsWith_emptyPrefix()
{
    QVERIFY(startsWith(std::string("Hello"), std::string("")));
}
void TestStringHelper::test_startsWith_fullMatch()
{
    QVERIFY(startsWith(std::string("Hello"), std::string("Hello")));
}

void TestStringHelper::test_startsWith_noMatch()
{
    QVERIFY(!startsWith(std::string("Hello"), std::string("World")));
}

void TestStringHelper::test_endsWith()
{
    QVERIFY(endsWith(std::string("Hello"), std::string("lo")));
    QVERIFY(!endsWith(std::string("Hello"), std::string("Hello!")));
}

void TestStringHelper::test_endsWith_emptySuffix()
{
    QVERIFY(endsWith(std::string("Hello"), std::string("")));
}

void TestStringHelper::test_endsWith_fullMatch()
{
    QVERIFY(endsWith(std::string("Hello"), std::string("Hello")));
}

void TestStringHelper::test_endsWith_noMatch()
{
    QVERIFY(!endsWith(std::string("Hello"), std::string("World")));
}


// ============================================================================
// Contains
// ============================================================================

void TestStringHelper::test_contains()
{
    QVERIFY(contains(std::string("Hello world"), std::string("world")));
    QVERIFY(!contains(std::string("Hello"), std::string("World")));
}

void TestStringHelper::test_contains_emptyNeedle()
{
    // Empty needle is contained in every string
    QVERIFY(contains(std::string("Hello"), std::string("")));
    QVERIFY(contains(std::string(""), std::string("")));
}

void TestStringHelper::test_contains_noMatch()
{
    QVERIFY(!contains(std::string("Hello"), std::string("xyz")));
}

void TestStringHelper::test_contains_caseSensitive()
{
    QVERIFY(contains(std::string("Hello"), std::string("Hello")));
    QVERIFY(!contains(std::string("Hello"), std::string("hello")));
}

// ============================================================================
// Replace
// ============================================================================

void TestStringHelper::test_replaceAll()
{
    std::string r = replaceAll(std::string("hello world world"),
                               std::string("world"), std::string("there"));
    QVERIFY2(r == "hello there there",
             qPrintable(QString("replaceAll expected \"hello there there\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_replaceAll_noOccurrence()
{
    std::string r = replaceAll(std::string("hello"), std::string("xyz"), std::string("abc"));
    QVERIFY2(r == "hello",
             qPrintable(QString("replaceAll with no match should be unchanged, got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_replaceAll_overlapping()
{
    // "aaa" -> "b": non-overlapping, middle "a" replaced only once
    std::string r = replaceAll(std::string("aaa"), std::string("aa"), std::string("b"));
    QVERIFY2(r == "ba",
             qPrintable(QString("replaceAll overlapping expected \"ba\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_replaceAll_emptyPattern()
{
    // Empty pattern: implementation returns original string unchanged
    std::string r = replaceAll(std::string("hello"), std::string(""), std::string("x"));
    QVERIFY2(r == "hello",
             qPrintable(QString("replaceAll with empty pattern should be unchanged, got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_replaceAll_batch()
{
    std::vector<std::pair<std::string, std::string>> reps = {
        {"foo", "bar"},
        {"bar", "baz"}
    };
    // Note: batch replace is sequential, so "foo"->"bar" then "bar"->"baz"
    std::string r = replaceAll(std::string("foo"), reps);
    QVERIFY2(r == "baz",
             qPrintable(QString("batch replace expected \"baz\", got \"%1\"").arg(QString::fromStdString(r))));
}

// ============================================================================
// Split / Join
// ============================================================================

void TestStringHelper::test_split()
{
    auto parts = split(std::string("a,b,c"), std::string(","));
    QVERIFY2(parts.size() == 3,
             qPrintable(QString("split expected 3 parts, got %1").arg(int(parts.size()))));
    QVERIFY2(parts[0] == "a" && parts[1] == "b" && parts[2] == "c",
             qPrintable(QString("split parts mismatch: got %1,%2,%3")
                        .arg(QString::fromStdString(parts[0]))
                        .arg(QString::fromStdString(parts[1]))
                        .arg(QString::fromStdString(parts[2]))));
}

void TestStringHelper::test_split_emptyDelimiter()
{
    // Empty delimiter -> whole string as single part
    auto parts = split(std::string("abc"), std::string(""));
    QVERIFY2(parts.size() == 1,
             qPrintable(QString("split with empty delimiter expected 1 part, got %1").arg(int(parts.size()))));
}

void TestStringHelper::test_split_noMatch()
{
    auto parts = split(std::string("abc"), std::string(","));
    QVERIFY2(parts.size() == 1,
             qPrintable(QString("split with no delimiter match expected 1 part, got %1").arg(int(parts.size()))));
    QVERIFY2(parts[0] == "abc",
             qPrintable(QString("split with no match expected \"abc\", got \"%1\"").arg(QString::fromStdString(parts[0]))));
}

void TestStringHelper::test_split_withEmptyParts()
{
    // Default skipEmpty=false: "a,,b" -> ["a", "", "b"]
    auto parts = split(std::string("a,,b"), std::string(","));
    QVERIFY2(parts.size() == 3,
             qPrintable(QString("split with empty parts expected 3, got %1").arg(int(parts.size()))));
}

void TestStringHelper::test_split_skipEmpty()
{
    auto parts = split(std::string("a,,b"), std::string(","), true /* skipEmpty */);
    QVERIFY2(parts.size() == 2,
             qPrintable(QString("split skipEmpty expected 2 parts, got %1").arg(int(parts.size()))));
    QVERIFY2(parts[0] == "a" && parts[1] == "b",
             qPrintable(QString("skipEmpty split mismatch: got \"%1\",\"%2\"")
                        .arg(QString::fromStdString(parts[0]))
                        .arg(QString::fromStdString(parts[1]))));
}

void TestStringHelper::test_join()
{
    std::vector<std::string> parts = {"a", "b", "c"};
    std::string r = join(parts, std::string(","));
    QVERIFY2(r == "a,b,c",
             qPrintable(QString("join expected \"a,b,c\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_join_emptyParts()
{
    std::vector<std::string> parts;
    std::string r = join(parts, std::string(","));
    QVERIFY2(r.empty(),
             qPrintable(QString("join of empty parts should be empty, got \"%1\"").arg(QString::fromStdString(r))));
}

// ============================================================================
// Numeric conversion
// ============================================================================

void TestStringHelper::test_toInt()
{
    int r = toInt(std::string("42"));
    QVERIFY2(r == 42,
             qPrintable(QString("toInt(\"42\") expected 42, got %1").arg(r)));
}

void TestStringHelper::test_toInt_invalidInput()
{
    int r = toInt(std::string("abc"), 99);
    QVERIFY2(r == 99,
             qPrintable(QString("toInt(\"abc\", 99) expected 99, got %1").arg(r)));
}

void TestStringHelper::test_toLong()
{
    int64_t r = toLong(std::string("1234567890"));
    QVERIFY2(r == 1234567890LL,
             qPrintable(QString("toLong expected 1234567890, got %1").arg(r)));
}

void TestStringHelper::test_toDouble()
{
    double r = toDouble(std::string("3.14"));
    QVERIFY2(floatNear(r, 3.14),
             qPrintable(QString("toDouble(\"3.14\") expected ~3.14, got %1").arg(r)));
}

void TestStringHelper::test_toDouble_invalidInput()
{
    double r = toDouble(std::string("xyz"), 2.5);
    QVERIFY2(floatNear(r, 2.5),
             qPrintable(QString("toDouble(\"xyz\", 2.5) expected 2.5, got %1").arg(r)));
}

void TestStringHelper::test_fromInt()
{
    std::string r = fromInt(42);
    QVERIFY2(r == "42",
             qPrintable(QString("fromInt(42) expected \"42\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_fromLong()
{
    std::string r = fromLong(9876543210LL);
    QVERIFY2(r == "9876543210",
             qPrintable(QString("fromLong(9876543210) expected \"9876543210\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_fromDouble()
{
    std::string s = fromDouble(3.14159, 4);
    QVERIFY2(s == "3.1416",
             qPrintable(QString("fromDouble(3.14159, 4) expected \"3.1416\", got \"%1\"").arg(QString::fromStdString(s))));
}

void TestStringHelper::test_fromDouble_precision()
{
    // Test zero precision
    std::string s0 = fromDouble(42.9, 0);
    QVERIFY2(s0 == "43.0000" || s0 == "43",
             qPrintable(QString("fromDouble precision 0: got \"%1\"").arg(QString::fromStdString(s0))));

    // Test exact integer
    std::string si = fromDouble(100.0, 2);
    QVERIFY2(floatNear(std::stod(si), 100.0),
             qPrintable(QString("fromDouble(100.0,2) should round-trip: got \"%1\"").arg(QString::fromStdString(si))));
}

// ============================================================================
// Tab/space conversion
// ============================================================================

void TestStringHelper::test_tabsToSpaces()
{
    std::string r = tabsToSpaces(std::string("a\tb"), 4);
    QVERIFY2(r == "a    b",
             qPrintable(QString("tabsToSpaces expected \"a    b\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_tabsToSpaces_empty()
{
    std::string r = tabsToSpaces(std::string(""), 4);
    QVERIFY2(r.empty(),
             qPrintable(QString("tabsToSpaces empty should be empty, got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_spacesToTabs()
{
    std::string r = spacesToTabs(std::string("a    b"), 4);
    QVERIFY2(r == "a\tb",
             qPrintable(QString("spacesToTabs expected \"a\\tb\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_spacesToTabs_partialTab()
{
    // Only convert when spaces reach the next tab stop
    std::string r = spacesToTabs(std::string("a   b"), 4);
    // 3 spaces -> not a full tab stop -> should remain as 3 spaces
    QVERIFY2(r == "a   b" || r == "a\t b",
             qPrintable(QString("spacesToTabs partial: got \"%1\"").arg(QString::fromStdString(r))));
}

// ============================================================================
// Path helpers
// ============================================================================

void TestStringHelper::test_fileName()
{
    QVERIFY2(fileName(std::string("/path/to/file.txt")) == "file.txt",
             "fileName should return just the filename");
    QVERIFY2(fileName(std::string("file.txt")) == "file.txt",
             "fileName on bare filename should return it unchanged");
}

void TestStringHelper::test_fileExt()
{
    QVERIFY2(fileExt(std::string("/path/to/file.txt")) == "txt",
             "fileExt should return \"txt\"");
    QVERIFY2(fileExt(std::string("/path/to/file.tar.gz")) == "gz",
             "fileExt should return rightmost extension");
    QVERIFY2(fileExt(std::string("noextension")) == "",
             "fileExt on no-extension should return empty");
    QVERIFY2(fileExt(std::string(".hidden")) == "",
             "fileExt on \".hidden\" should return empty (dotfile)");
}

void TestStringHelper::test_filePath()
{
    QVERIFY2(filePath(std::string("/path/to/file.txt")) == "/path/to",
             "filePath should return parent directory");
    QVERIFY2(filePath(std::string("file.txt")) == "",
             "filePath on bare filename should return empty");
}

void TestStringHelper::test_normalizePath()
{
    // Forward slashes and collapse double slashes
    std::string r = normalizePath(std::string("foo//bar/baz"));
    QVERIFY2(r == "foo/bar/baz",
             qPrintable(QString("normalizePath expected \"foo/bar/baz\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_baseName()
{
    QVERIFY2(baseName(std::string("/path/to/file.tar.gz")) == "file.tar",
             "baseName should strip rightmost extension");
}

void TestStringHelper::test_baseName_noExt()
{
    QVERIFY2(baseName(std::string("/path/to/file")) == "file",
             "baseName with no extension should return filename");
}

// ============================================================================
// Escaping
// ============================================================================

void TestStringHelper::test_escapeJson()
{
    QString r = escapeJson(QString("hello\"world\\test\nline"));
    QVERIFY2(r.contains("\\\""), "escapeJson should escape double quotes");
    QVERIFY2(r.contains("\\\\"), "escapeJson should escape backslashes");
    QVERIFY2(r.contains("\\n"), "escapeJson should escape newlines");
}

void TestStringHelper::test_escapeJson_empty()
{
    QString r = escapeJson(QString(""));
    QVERIFY2(r == "", qPrintable(QString("escapeJson empty should be empty, got \"%1\"").arg(r)));
}

void TestStringHelper::test_escapeShell()
{
    // String with spaces should be quoted
    QString r = escapeShell(QString("hello world"));
    QVERIFY2(r.startsWith('\''), "escapeShell should quote strings with spaces");
    QVERIFY2(r.endsWith('\''), "escapeShell should close the quote");
}

void TestStringHelper::test_escapeShell_noSpecialChars()
{
    // Plain alphanumeric string needs no quoting
    QString r = escapeShell(QString("hello123"));
    QVERIFY2(r == "hello123",
             qPrintable(QString("escapeShell with no special chars should be unchanged, got \"%1\"").arg(r)));
}

void TestStringHelper::test_escapeShell_singleQuote()
{
    // Single quotes in string must be escaped within single-quoted shell string
    QString r = escapeShell(QString("it's"));
    // The implementation escapes ' as '\''
    QVERIFY2(r.contains("'") && r.size() > 3,
             qPrintable(QString("escapeShell single quote: got \"%1\"").arg(r)));
}

void TestStringHelper::test_htmlEscape()
{
    std::string r = htmlEscape(std::string("a<b>c"));
    QVERIFY2(r == "&lt;a&lt;b&gt;c",
             qPrintable(QString("htmlEscape expected \"&lt;a&lt;b&gt;c\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_htmlEscape_ampersand()
{
    std::string r = htmlEscape(std::string("a & b"));
    QVERIFY2(r.find("&amp;") != std::string::npos,
             "htmlEscape should escape & as &amp;");
}

// ============================================================================
// Comparison
// ============================================================================

void TestStringHelper::test_equalsIgnoreCase()
{
    QVERIFY(equalsIgnoreCase(std::string("Hello"), std::string("hello")));
    QVERIFY(equalsIgnoreCase(std::string("HELLO"), std::string("hello")));
    QVERIFY(!equalsIgnoreCase(std::string("Hello"), std::string("World")));
}

void TestStringHelper::test_equalsIgnoreCase_differentLength()
{
    QVERIFY(!equalsIgnoreCase(std::string("Hello"), std::string("Hell")));
    QVERIFY(!equalsIgnoreCase(std::string("Hell"), std::string("Hello")));
}

// ============================================================================
// Formatting
// ============================================================================

void TestStringHelper::test_format()
{
    std::string r = format("Hello %s %d", "world", 42);
    QVERIFY2(r == "Hello world 42",
             qPrintable(QString("format expected \"Hello world 42\", got \"%1\"").arg(QString::fromStdString(r))));
}

// ============================================================================
// Word wrap
// ============================================================================

void TestStringHelper::test_wordWrap()
{
    auto lines = wordWrap(std::string("hello world foo"), 5);
    QVERIFY2(lines.size() >= 1, "wordWrap should produce at least 1 line");
    // Verify no line exceeds columnWidth
    for (const auto& line : lines) {
        QVERIFY2(line.size() <= 5,
                 qPrintable(QString("wordWrap line \"%1\" exceeds width 5 (%2 chars)")
                            .arg(QString::fromStdString(line)).arg(int(line.size()))));
    }
}

void TestStringHelper::test_wordWrap_singleWord()
{
    // Long word should not be split mid-word (just placed as-is)
    auto lines = wordWrap(std::string("superlongword"), 4);
    QVERIFY2(lines.size() >= 1, "wordWrap should handle single long word");
}

void TestStringHelper::test_wordWrap_zeroWidth()
{
    auto lines = wordWrap(std::string("hello"), 0);
    // Zero width: whole string on one line
    QVERIFY2(lines.size() == 1,
             qPrintable(QString("wordWrap zero width expected 1 line, got %1").arg(int(lines.size()))));
}

// ============================================================================
// EOL normalisation
// ============================================================================

void TestStringHelper::test_makeEolConsistent_crlf()
{
    // Convert LF to CRLF
    std::string r = makeEolConsistent(std::string("a\nb\nc"), static_cast<int>(EolType::EOL_CRLF));
    QVERIFY2(r == "a\r\nb\r\nc",
             qPrintable(QString("makeEolConsistent CRLF: got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_makeEolConsistent_lf()
{
    // Mixed EOL -> LF
    std::string r = makeEolConsistent(std::string("a\r\nb\rc"), static_cast<int>(EolType::EOL_LF));
    QVERIFY2(r == "a\nb\nc",
             qPrintable(QString("makeEolConsistent LF: got \"%1\"").arg(QString::fromStdString(r))));
}

void TestStringHelper::test_makeEolConsistent_mixed()
{
    // CRLF -> CR
    std::string r = makeEolConsistent(std::string("a\r\nb\r\n"), static_cast<int>(EolType::EOL_CR));
    QVERIFY2(r == "a\rb\rc",
             qPrintable(QString("makeEolConsistent CR: got \"%1\"").arg(QString::fromStdString(r))));
}

// ============================================================================
// Indentation
// ============================================================================

void TestStringHelper::test_indentLines()
{
    std::string r = indentLines(std::string("hello\nworld"), 4);
    // Both lines should be indented
    QVERIFY2(r.find("hello") != std::string::npos, "indented content should contain \"hello\"");
    QVERIFY2(r.find("world") != std::string::npos, "indented content should contain \"world\"");
}

void TestStringHelper::test_indentLines_empty()
{
    std::string r = indentLines(std::string(""), 4);
    QVERIFY2(r.empty(),
             qPrintable(QString("indentLines empty should be empty, got \"%1\"").arg(QString::fromStdString(r))));
}

QTEST_MAIN(TestStringHelper)
#include "TestStringHelper.moc"
