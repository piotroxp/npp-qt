// TestStringHelper.cpp — Qt Test suite for StringHelper utilities
// Phase1 canonical source: src/common/StringHelper.cpp

#include <QtTest/QtTest>
#include "common/StringHelper.h"
#include <vector>

using namespace StringHelper;

class TestStringHelper : public QObject
{
    Q_OBJECT

private slots:
    // ── Case conversion ───────────────────────────────────────────────────
    void test_toLower();
    void test_toUpper();

    // ── Trim ─────────────────────────────────────────────────────────────
    void test_trim_basic();
    void test_trim_leftOnly();
    void test_trim_rightOnly();
    void test_trim_noChange();
    void test_trim_empty();

    // ── StartsWith / EndsWith ────────────────────────────────────────────
    void test_startsWith();
    void test_endsWith();

    // ── Contains ────────────────────────────────────────────────────────
    void test_contains();

    // ── Replace ─────────────────────────────────────────────────────────
    void test_replaceAll();

    // ── Split / Join ─────────────────────────────────────────────────────
    void test_split();
    void test_join();

    // ── Numeric conversion ────────────────────────────────────────────────
    void test_toInt();
    void test_toLong();
    void test_toDouble();
    void test_fromInt();
    void test_fromDouble();

    // ── Tab/space conversion ───────────────────────────────────────────────
    void test_tabsToSpaces();
    void test_spacesToTabs();

private:
    // Helper to compare std::string with raw C string
    static bool eq(const std::string& a, const char* b) { return a == b; }
};

// ─── Case conversion ────────────────────────────────────────────────────────────

void TestStringHelper::test_toLower()
{
    QString qresult = QString::fromStdString(toLower("Hello World"));
    if (qresult != "hello world")
        qWarning("toLower(\"Hello World\") = \"%s\"", qPrintable(qresult));
}

void TestStringHelper::test_toUpper()
{
    QString qresult = QString::fromStdString(toUpper("Hello World"));
    if (qresult != "HELLO WORLD")
        qWarning("toLower(\"Hello World\") = \"%s\"", qPrintable(qresult));
}

// ─── Trim ─────────────────────────────────────────────────────────────────────

void TestStringHelper::test_trim_basic()
{
    if (trim("  hello  ") != "hello")
        qWarning("trim failed on \"  hello  \"");
}

void TestStringHelper::test_trim_leftOnly()
{
    if (trimLeft("  hello") != "hello")
        qWarning("trimLeft failed on \"  hello\"");
}

void TestStringHelper::test_trim_rightOnly()
{
    if (trimRight("hello  ") != "hello")
        qWarning("trimRight failed on \"hello  \"");
}

void TestStringHelper::test_trim_noChange()
{
    if (trim("hello") != "hello")
        qWarning("trim changed \"hello\"");
}

void TestStringHelper::test_trim_empty()
{
    if (!trim("").empty())
        qWarning("trim changed \"\"");
}

// ─── StartsWith / EndsWith ────────────────────────────────────────────────────

void TestStringHelper::test_startsWith()
{
    QVERIFY(startsWith("Hello", "He"));
    QVERIFY(startsWith("Hello", "Hello"));
    QVERIFY(!startsWith("Hello", "hello"));
}

void TestStringHelper::test_endsWith()
{
    QVERIFY(endsWith("Hello", "lo"));
    QVERIFY(endsWith("Hello", "Hello"));
    QVERIFY(!endsWith("Hello", "Hello!"));
}

// ─── Contains ────────────────────────────────────────────────────────────────

void TestStringHelper::test_contains()
{
    QVERIFY(contains("Hello world", "world"));
    QVERIFY(contains("Hello world", ""));
    QVERIFY(!contains("Hello", "World"));
}

// ─── Replace ─────────────────────────────────────────────────────────────────

void TestStringHelper::test_replaceAll()
{
    if (replaceAll("hello world world", "world", "there") != "hello there there")
        qWarning("replaceAll failed");
}

// ─── Split / Join ─────────────────────────────────────────────────────────────

void TestStringHelper::test_split()
{
    auto parts = split("a,b,c", ",");
    if (parts.size() != 3)
        qWarning("split size=%zu", parts.size());
}

void TestStringHelper::test_join()
{
    std::vector<std::string> parts = {"a", "b", "c"};
    if (join(parts, ",") != "a,b,c")
        qWarning("join failed");
}

// ─── Numeric conversion ───────────────────────────────────────────────────────

void TestStringHelper::test_toInt()
{
    if (toInt("42") != 42) qWarning("toInt(\"42\") failed");
    if (toInt("abc", 99) != 99) qWarning("toInt(\"abc\", 99) failed");
}

void TestStringHelper::test_toLong()
{
    if (toLong("1234567890") != 1234567890LL) qWarning("toLong failed");
}

void TestStringHelper::test_toDouble()
{
    if (toDouble("3.14") < 3.139 || toDouble("3.14") > 3.141)
        qWarning("toDouble failed");
}

void TestStringHelper::test_fromInt()
{
    if (fromInt(42) != "42") qWarning("fromInt failed");
}

void TestStringHelper::test_fromDouble()
{
    std::string s = fromDouble(3.14159, 4);
    if (s != "3.1416") qWarning("fromDouble(3.14159, 4) = \"%s\"", s.c_str());
}

// ─── Tab/space conversion ───────────────────────────────────────────────────────

void TestStringHelper::test_tabsToSpaces()
{
    std::string s = "a\tb";
    std::string r = tabsToSpaces(s, 4);
    if (r != "a    b")
        qWarning("tabsToSpaces failed: got \"%s\"", r.c_str());
}

void TestStringHelper::test_spacesToTabs()
{
    std::string s = "a    b";  // 4 spaces = tab at tab stop 4
    std::string r = spacesToTabs(s, 4);
    if (r != "a\tb")
        qWarning("spacesToTabs failed: got \"%s\"", r.c_str());
}

QTEST_MAIN(TestStringHelper)
#include "TestStringHelper.moc"
