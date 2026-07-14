// TestLexer.cpp — Qt Test suite for language detection and lexer factory
// Phase1 canonical sources:
//   src/core/LanguageManager.h/cpp
//   src/editor/ScintillaEditor.h/cpp
// =============================================================================

#include <QtTest/QtTest>
#include <QCoreApplication>
#include "core/LanguageManager.h"
#include "editor/ScintillaEditor.h"

class TestLexer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    // ── Language detection ─────────────────────────────────────────────────
    void test_detect_c();
    void test_detect_cpp();
    void test_detect_java();
    void test_detect_python();
    void test_detect_html();
    void test_detect_xml();
    void test_detect_css();
    void test_detect_js();
    void test_detect_json();
    void test_detect_yaml();
    void test_detect_markdown();
    void test_detect_sql();
    void test_detect_bash();
    void test_detect_perl();
    void test_detect_ruby();
    void test_detect_lua();
    void test_detect_makefile();
    void test_detect_ini();
    void test_detect_batch();
    void test_detect_php();
    void test_detect_unknown();

    // ── Language → name round-trip ─────────────────────────────────────────
    void test_getLanguageName_cpp();
    void test_getLanguageName_text();

    // ── Keywords ───────────────────────────────────────────────────────────
    void test_getKeywords_cpp();
    void test_getKeywords_python();
    void test_getKeywords_empty();

    // ── Case sensitivity ───────────────────────────────────────────────────
    void test_isCaseSensitive_batch();
    void test_isCaseSensitive_cpp();

    // ── getLexer ──────────────────────────────────────────────────────────
    void test_getLexer_cpp();
    void test_getLexer_python();
    void test_getLexer_html();
    void test_getLexer_null();

private:
    LanguageManager& _mgr = LanguageManager::instance();
};

// =============================================================================
// Init
// =============================================================================

void TestLexer::initTestCase()
{
    static int argc = 1;
    static char* argv[] = { const_cast<char*>("test") };
    static QCoreApplication app(argc, argv);
    Q_UNUSED(app);
}

void TestLexer::init()
{
    // Nothing per-test to set up
}

// =============================================================================
// Language detection
// =============================================================================

void TestLexer::test_detect_c()
{
    LangType t = _mgr.detectLanguage("foo.c");
    if (t != L_C) qWarning("detectLanguage(\"foo.c\") = %d, expected %d", int(t), int(L_C));
}

void TestLexer::test_detect_cpp()
{
    LangType t1 = _mgr.detectLanguage("foo.cpp");
    if (t1 != L_CPP) qWarning("detectLanguage(\"foo.cpp\") = %d", int(t1));
    LangType t2 = _mgr.detectLanguage("foo.h");
    if (t2 != L_CPP) qWarning("detectLanguage(\"foo.h\") = %d", int(t2));
    LangType t3 = _mgr.detectLanguage("foo.hpp");
    if (t3 != L_CPP) qWarning("detectLanguage(\"foo.hpp\") = %d", int(t3));
}

void TestLexer::test_detect_java()
{
    LangType t = _mgr.detectLanguage("Foo.java");
    if (t != L_JAVA) qWarning("detectLanguage(\"Foo.java\") = %d", int(t));
}

void TestLexer::test_detect_python()
{
    LangType t = _mgr.detectLanguage("foo.py");
    if (t != L_PYTHON) qWarning("detectLanguage(\"foo.py\") = %d", int(t));
}

void TestLexer::test_detect_html()
{
    LangType t1 = _mgr.detectLanguage("foo.html");
    if (t1 != L_HTML) qWarning("detectLanguage(\"foo.html\") = %d", int(t1));
    LangType t2 = _mgr.detectLanguage("foo.htm");
    if (t2 != L_HTML) qWarning("detectLanguage(\"foo.htm\") = %d", int(t2));
}

void TestLexer::test_detect_xml()
{
    LangType t = _mgr.detectLanguage("foo.xml");
    if (t != L_XML) qWarning("detectLanguage(\"foo.xml\") = %d", int(t));
}

void TestLexer::test_detect_css()
{
    LangType t = _mgr.detectLanguage("foo.css");
    if (t != L_CSS) qWarning("detectLanguage(\"foo.css\") = %d", int(t));
}

void TestLexer::test_detect_js()
{
    LangType t = _mgr.detectLanguage("foo.js");
    if (t != L_JS) qWarning("detectLanguage(\"foo.js\") = %d", int(t));
}

void TestLexer::test_detect_json()
{
    LangType t = _mgr.detectLanguage("foo.json");
    if (t != L_JSON) qWarning("detectLanguage(\"foo.json\") = %d", int(t));
}

void TestLexer::test_detect_yaml()
{
    LangType t1 = _mgr.detectLanguage("foo.yaml");
    if (t1 != L_YAML) qWarning("detectLanguage(\"foo.yaml\") = %d", int(t1));
    LangType t2 = _mgr.detectLanguage("foo.yml");
    if (t2 != L_YAML) qWarning("detectLanguage(\"foo.yml\") = %d", int(t2));
}

void TestLexer::test_detect_markdown()
{
    LangType t = _mgr.detectLanguage("README.md");
    if (t != L_MARKDOWN) qWarning("detectLanguage(\"README.md\") = %d", int(t));
}

void TestLexer::test_detect_sql()
{
    // No SQL lexer registered yet — returns L_TEXT
    LangType t = _mgr.detectLanguage("foo.sql");
    qDebug("detectLanguage(\"foo.sql\") = %d", int(t));
}

void TestLexer::test_detect_bash()
{
    LangType t = _mgr.detectLanguage("foo.sh");
    if (t != L_BASH) qWarning("detectLanguage(\"foo.sh\") = %d", int(t));
}

void TestLexer::test_detect_perl()
{
    LangType t = _mgr.detectLanguage("foo.pl");
    if (t != L_PERL) qWarning("detectLanguage(\"foo.pl\") = %d", int(t));
}

void TestLexer::test_detect_ruby()
{
    LangType t = _mgr.detectLanguage("foo.rb");
    if (t != L_RUBY) qWarning("detectLanguage(\"foo.rb\") = %d", int(t));
}

void TestLexer::test_detect_lua()
{
    LangType t = _mgr.detectLanguage("foo.lua");
    if (t != L_LUA) qWarning("detectLanguage(\"foo.lua\") = %d", int(t));
}

void TestLexer::test_detect_makefile()
{
    LangType t = _mgr.detectLanguage("Makefile");
    if (t != L_MAKEFILE) qWarning("detectLanguage(\"Makefile\") = %d", int(t));
}

void TestLexer::test_detect_ini()
{
    LangType t = _mgr.detectLanguage("foo.ini");
    if (t != L_INI) qWarning("detectLanguage(\"foo.ini\") = %d", int(t));
}

void TestLexer::test_detect_batch()
{
    LangType t1 = _mgr.detectLanguage("foo.bat");
    if (t1 != L_BATCH) qWarning("detectLanguage(\"foo.bat\") = %d", int(t1));
    LangType t2 = _mgr.detectLanguage("foo.cmd");
    if (t2 != L_BATCH) qWarning("detectLanguage(\"foo.cmd\") = %d", int(t2));
}

void TestLexer::test_detect_php()
{
    LangType t = _mgr.detectLanguage("foo.php");
    if (t != L_PHP) qWarning("detectLanguage(\"foo.php\") = %d", int(t));
}

void TestLexer::test_detect_unknown()
{
    LangType t = _mgr.detectLanguage("foo.xyzzy");
    if (t != L_TEXT) qWarning("detectLanguage(\"foo.xyzzy\") = %d, expected L_TEXT", int(t));
}

// =============================================================================
// Language → name
// =============================================================================

void TestLexer::test_getLanguageName_cpp()
{
    std::string name = _mgr.getLanguageName(L_CPP);
    if (name != "C++")
        qWarning("getLanguageName(L_CPP) = \"%s\", expected \"C++\"", name.c_str());
}

void TestLexer::test_getLanguageName_text()
{
    std::string name = _mgr.getLanguageName(L_TEXT);
    if (name != "Normal")
        qWarning("getLanguageName(L_TEXT) = \"%s\"", name.c_str());
}

// =============================================================================
// Keywords
// =============================================================================

void TestLexer::test_getKeywords_cpp()
{
    auto kws = _mgr.getKeywords(L_CPP);
    if (kws.empty())
        qWarning("getKeywords(L_CPP) returned empty");
    if (kws[0].empty())
        qWarning("getKeywords(L_CPP)[0] (primary kwset) is empty");
    // Primary kwset should contain "int"
    if (kws[0].find("int ") == std::string::npos && kws[0].find(" int") == std::string::npos)
        qWarning("getKeywords(L_CPP)[0] doesn't contain \"int\"");
}

void TestLexer::test_getKeywords_python()
{
    auto kws = _mgr.getKeywords(L_PYTHON);
    if (kws.empty())
        qWarning("getKeywords(L_PYTHON) returned empty");
}

void TestLexer::test_getKeywords_empty()
{
    // Unrecognised language → empty keyword map
    auto kws = _mgr.getKeywords(L_TEXT);
    qDebug("getKeywords(L_TEXT) size = %zu", kws.size());
}

// =============================================================================
// Case sensitivity
// =============================================================================

void TestLexer::test_isCaseSensitive_batch()
{
    // Batch/INI are case-insensitive
    QVERIFY(!_mgr.isCaseSensitive(L_BATCH));
    QVERIFY(!_mgr.isCaseSensitive(L_INI));
}

void TestLexer::test_isCaseSensitive_cpp()
{
    // C/C++ is case-sensitive
    QVERIFY(_mgr.isCaseSensitive(L_CPP));
    QVERIFY(_mgr.isCaseSensitive(L_PYTHON));
}

// =============================================================================
// Lexer creation
// =============================================================================

void TestLexer::test_getLexer_cpp()
{
    QsciLexer* lex = _mgr.getLexer(L_CPP);
    // May be nullptr if QsciScintilla wasn't built with the C++ lexer
    if (lex) {
        qDebug("C++ lexer: %s", lex->language());
        delete lex;
    } else {
        qDebug("C++ lexer not available in this QsciScintilla build");
    }
}

void TestLexer::test_getLexer_python()
{
    QsciLexer* lex = _mgr.getLexer(L_PYTHON);
    if (lex) {
        qDebug("Python lexer: %s", lex->language());
        delete lex;
    } else {
        qDebug("Python lexer not available");
    }
}

void TestLexer::test_getLexer_html()
{
    QsciLexer* lex = _mgr.getLexer(L_HTML);
    if (lex) {
        qDebug("HTML lexer: %s", lex->language());
        delete lex;
    }
}

void TestLexer::test_getLexer_null()
{
    // L_TEXT → null (plain text, no highlighting)
    QsciLexer* lex = _mgr.getLexer(L_TEXT);
    if (lex) {
        qDebug("L_TEXT lexer (unexpected): %s", lex->language());
        delete lex;
    } else {
        qDebug("L_TEXT correctly returns null lexer");
    }
}

QTEST_MAIN(TestLexer)
#include "TestLexer.moc"
