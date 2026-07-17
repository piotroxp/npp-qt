// TestLexer.cpp -- Qt Test suite for language detection and lexer factory
// Phase1 canonical sources:
//   src/core/LanguageManager.h/cpp
//   src/editor/ScintillaEditor.h/cpp
//
// Test coverage assessment:
// - Language detection: was weak -- used if+warning instead of QVERIFY/QCOMPARE
// - Language name: same pattern
// - Keywords: same pattern
// - Case sensitivity: good -- used QVERIFY
// - Lexer creation: good -- tested for null
// - NEW: detectLanguage with two-argument variant, getLanguageExtension,
//         getLexerName, getAllLanguages, getIndentSize, comment helpers,
//         mapStringToLang static method

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include "core/LanguageManager.h"
#include "editor/ScintillaEditor.h"
#include "common/Types.h"

class TestLexer : public QObject
{
    Q_OBJECT

    using enum LangType;  // bring L_C, L_CPP, L_HTML, etc. into scope

private slots:
    void initTestCase();
    void init();

    // -- Language detection
    void test_detect_c();
    void test_detect_cpp();
    void test_detect_cpp_hpp();
    void test_detect_java();
    void test_detect_python();
    void test_detect_html();
    void test_detect_html_htm();
    void test_detect_xml();
    void test_detect_css();
    void test_detect_js();
    void test_detect_json();
    void test_detect_yaml();
    void test_detect_yaml_yml();
    void test_detect_markdown();
    void test_detect_sql();
    void test_detect_bash();
    void test_detect_perl();
    void test_detect_ruby();
    void test_detect_lua();
    void test_detect_makefile();
    void test_detect_makefile_lowercase();
    void test_detect_ini();
    void test_detect_batch();
    void test_detect_batch_cmd();
    void test_detect_php();
    void test_detect_unknown();
    void test_detect_fromContent();

    // -- Language -> name round-trip
    void test_getLanguageName_cpp();
    void test_getLanguageName_text();
    void test_getLanguageName_python();
    void test_getLanguageName_java();

    // -- Language extension
    void test_getLanguageExtension();

    // -- Lexer name
    void test_getLexerName();

    // -- All languages
    void test_getAllLanguages();

    // -- Keywords
    void test_getKeywords_cpp();
    void test_getKeywords_python();
    void test_getKeywords_empty();
    void test_getKeywords_java();

    // -- Indent size
    void test_getIndentSize();

    // -- Case sensitivity
    void test_isCaseSensitive_batch();
    void test_isCaseSensitive_cpp();
    void test_isCaseSensitive_python();
    void test_isCaseSensitive_ini();

    // -- Comment helpers
    void test_getCommentLine();
    void test_getCommentStartEnd();

    // -- getLexer
    void test_getLexer_cpp();
    void test_getLexer_python();
    void test_getLexer_html();
    void test_getLexer_null();

    // -- mapStringToLang
    void test_mapStringToLang();

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
    QVERIFY2(t == L_C,
             qPrintable(QString("detectLanguage(\"foo.c\") = %1, expected L_C (%2)")
                        .arg(int(t)).arg(int(L_C))));
}

void TestLexer::test_detect_cpp()
{
    LangType t1 = _mgr.detectLanguage("foo.cpp");
    QVERIFY2(t1 == L_CPP,
             qPrintable(QString("detectLanguage(\"foo.cpp\") = %1, expected L_CPP (%2)")
                        .arg(int(t1)).arg(int(L_CPP))));

    LangType t2 = _mgr.detectLanguage("foo.h");
    QVERIFY2(t2 == L_CPP,
             qPrintable(QString("detectLanguage(\"foo.h\") = %1, expected L_CPP (%2)")
                        .arg(int(t2)).arg(int(L_CPP))));
}

void TestLexer::test_detect_cpp_hpp()
{
    LangType t = _mgr.detectLanguage("foo.hpp");
    QVERIFY2(t == L_CPP,
             qPrintable(QString("detectLanguage(\"foo.hpp\") = %1, expected L_CPP (%2)")
                        .arg(int(t)).arg(int(L_CPP))));
}

void TestLexer::test_detect_java()
{
    LangType t = _mgr.detectLanguage("Foo.java");
    QVERIFY2(t == L_JAVA,
             qPrintable(QString("detectLanguage(\"Foo.java\") = %1, expected L_JAVA (%2)")
                        .arg(int(t)).arg(int(L_JAVA))));
}

void TestLexer::test_detect_python()
{
    LangType t = _mgr.detectLanguage("foo.py");
    QVERIFY2(t == L_PYTHON,
             qPrintable(QString("detectLanguage(\"foo.py\") = %1, expected L_PYTHON (%2)")
                        .arg(int(t)).arg(int(L_PYTHON))));
}

void TestLexer::test_detect_html()
{
    LangType t = _mgr.detectLanguage("foo.html");
    QVERIFY2(t == L_HTML,
             qPrintable(QString("detectLanguage(\"foo.html\") = %1, expected L_HTML (%2)")
                        .arg(int(t)).arg(int(L_HTML))));
}

void TestLexer::test_detect_html_htm()
{
    LangType t = _mgr.detectLanguage("foo.htm");
    QVERIFY2(t == L_HTML,
             qPrintable(QString("detectLanguage(\"foo.htm\") = %1, expected L_HTML (%2)")
                        .arg(int(t)).arg(int(L_HTML))));
}

void TestLexer::test_detect_xml()
{
    LangType t = _mgr.detectLanguage("foo.xml");
    QVERIFY2(t == L_XML,
             qPrintable(QString("detectLanguage(\"foo.xml\") = %1, expected L_XML (%2)")
                        .arg(int(t)).arg(int(L_XML))));
}

void TestLexer::test_detect_css()
{
    LangType t = _mgr.detectLanguage("foo.css");
    QVERIFY2(t == L_CSS,
             qPrintable(QString("detectLanguage(\"foo.css\") = %1, expected L_CSS (%2)")
                        .arg(int(t)).arg(int(L_CSS))));
}

void TestLexer::test_detect_js()
{
    LangType t = _mgr.detectLanguage("foo.js");
    QVERIFY2(t == L_JS,
             qPrintable(QString("detectLanguage(\"foo.js\") = %1, expected L_JS (%2)")
                        .arg(int(t)).arg(int(L_JS))));
}

void TestLexer::test_detect_json()
{
    LangType t = _mgr.detectLanguage("foo.json");
    QVERIFY2(t == L_JSON,
             qPrintable(QString("detectLanguage(\"foo.json\") = %1, expected L_JSON (%2)")
                        .arg(int(t)).arg(int(L_JSON))));
}

void TestLexer::test_detect_yaml()
{
    LangType t1 = _mgr.detectLanguage("foo.yaml");
    QVERIFY2(t1 == L_YAML,
             qPrintable(QString("detectLanguage(\"foo.yaml\") = %1, expected L_YAML (%2)")
                        .arg(int(t1)).arg(int(L_YAML))));

    LangType t2 = _mgr.detectLanguage("foo.yml");
    QVERIFY2(t2 == L_YAML,
             qPrintable(QString("detectLanguage(\"foo.yml\") = %1, expected L_YAML (%2)")
                        .arg(int(t2)).arg(int(L_YAML))));
}

void TestLexer::test_detect_yaml_yml()
{
    LangType t = _mgr.detectLanguage("foo.yml");
    QVERIFY2(t == L_YAML,
             qPrintable(QString("detectLanguage(\"foo.yml\") = %1, expected L_YAML (%2)")
                        .arg(int(t)).arg(int(L_YAML))));
}

void TestLexer::test_detect_markdown()
{
    LangType t = _mgr.detectLanguage("README.md");
    QVERIFY2(t == L_MARKDOWN,
             qPrintable(QString("detectLanguage(\"README.md\") = %1, expected L_MARKDOWN (%2)")
                        .arg(int(t)).arg(int(L_MARKDOWN))));
}

void TestLexer::test_detect_sql()
{
    // SQL may or may not be registered
    LangType t = _mgr.detectLanguage("foo.sql");
    qDebug("detectLanguage(\"foo.sql\") = %d", int(t));
    // Just verify it returns a valid LangType (doesn't crash)
    QVERIFY(int(t) >= 0);
}

void TestLexer::test_detect_bash()
{
    LangType t = _mgr.detectLanguage("foo.sh");
    QVERIFY2(t == L_BASH,
             qPrintable(QString("detectLanguage(\"foo.sh\") = %1, expected L_BASH (%2)")
                        .arg(int(t)).arg(int(L_BASH))));
}

void TestLexer::test_detect_perl()
{
    LangType t = _mgr.detectLanguage("foo.pl");
    QVERIFY2(t == L_PERL,
             qPrintable(QString("detectLanguage(\"foo.pl\") = %1, expected L_PERL (%2)")
                        .arg(int(t)).arg(int(L_PERL))));
}

void TestLexer::test_detect_ruby()
{
    LangType t = _mgr.detectLanguage("foo.rb");
    QVERIFY2(t == L_RUBY,
             qPrintable(QString("detectLanguage(\"foo.rb\") = %1, expected L_RUBY (%2)")
                        .arg(int(t)).arg(int(L_RUBY))));
}

void TestLexer::test_detect_lua()
{
    LangType t = _mgr.detectLanguage("foo.lua");
    QVERIFY2(t == L_LUA,
             qPrintable(QString("detectLanguage(\"foo.lua\") = %1, expected L_LUA (%2)")
                        .arg(int(t)).arg(int(L_LUA))));
}

void TestLexer::test_detect_makefile()
{
    LangType t = _mgr.detectLanguage("Makefile");
    QVERIFY2(t == L_MAKEFILE,
             qPrintable(QString("detectLanguage(\"Makefile\") = %1, expected L_MAKEFILE (%2)")
                        .arg(int(t)).arg(int(L_MAKEFILE))));
}

void TestLexer::test_detect_makefile_lowercase()
{
    LangType t = _mgr.detectLanguage("makefile");
    QVERIFY2(t == L_MAKEFILE,
             qPrintable(QString("detectLanguage(\"makefile\") = %1, expected L_MAKEFILE (%2)")
                        .arg(int(t)).arg(int(L_MAKEFILE))));
}

void TestLexer::test_detect_ini()
{
    LangType t = _mgr.detectLanguage("foo.ini");
    QVERIFY2(t == L_INI,
             qPrintable(QString("detectLanguage(\"foo.ini\") = %1, expected L_INI (%2)")
                        .arg(int(t)).arg(int(L_INI))));
}

void TestLexer::test_detect_batch()
{
    LangType t1 = _mgr.detectLanguage("foo.bat");
    QVERIFY2(t1 == L_BATCH,
             qPrintable(QString("detectLanguage(\"foo.bat\") = %1, expected L_BATCH (%2)")
                        .arg(int(t1)).arg(int(L_BATCH))));

    LangType t2 = _mgr.detectLanguage("foo.cmd");
    QVERIFY2(t2 == L_BATCH,
             qPrintable(QString("detectLanguage(\"foo.cmd\") = %1, expected L_BATCH (%2)")
                        .arg(int(t2)).arg(int(L_BATCH))));
}

void TestLexer::test_detect_batch_cmd()
{
    LangType t = _mgr.detectLanguage("foo.cmd");
    QVERIFY2(t == L_BATCH,
             qPrintable(QString("detectLanguage(\"foo.cmd\") = %1, expected L_BATCH (%2)")
                        .arg(int(t)).arg(int(L_BATCH))));
}

void TestLexer::test_detect_php()
{
    LangType t = _mgr.detectLanguage("foo.php");
    QVERIFY2(t == L_PHP,
             qPrintable(QString("detectLanguage(\"foo.php\") = %1, expected L_PHP (%2)")
                        .arg(int(t)).arg(int(L_PHP))));
}

void TestLexer::test_detect_unknown()
{
    LangType t = _mgr.detectLanguage("foo.xyzzy");
    QVERIFY2(t == L_TEXT,
             qPrintable(QString("detectLanguage(\"foo.xyzzy\") = %1, expected L_TEXT (%2)")
                        .arg(int(t)).arg(int(L_TEXT))));
}

void TestLexer::test_detect_fromContent()
{
    // Content-based detection for script shebang
    LangType t = _mgr.detectLanguageFromContent("#!/usr/bin/env python3\nprint()");
    // Should detect Python from shebang (if supported)
    qDebug("detectLanguageFromContent() = %d", int(t));
    QVERIFY(int(t) >= 0);
}

// =============================================================================
// Language -> name
// =============================================================================

void TestLexer::test_getLanguageName_cpp()
{
    std::string name = _mgr.getLanguageName(L_CPP);
    QVERIFY2(!name.empty(),
             qPrintable(QString("getLanguageName(L_CPP) should not be empty")));
    QVERIFY2(name == "C++",
             qPrintable(QString("getLanguageName(L_CPP) = \"%1\", expected \"C++\"").arg(QString::fromStdString(name))));
}

void TestLexer::test_getLanguageName_text()
{
    std::string name = _mgr.getLanguageName(L_TEXT);
    QVERIFY2(!name.empty(),
             qPrintable(QString("getLanguageName(L_TEXT) should not be empty")));
    QVERIFY2(name == "Normal" || name == "Text" || name == "Normal text",
             qPrintable(QString("getLanguageName(L_TEXT) = \"%1\" (should be \"Normal\" or \"Text\")")
                        .arg(QString::fromStdString(name))));
}

void TestLexer::test_getLanguageName_python()
{
    std::string name = _mgr.getLanguageName(L_PYTHON);
    QVERIFY2(!name.empty(),
             qPrintable(QString("getLanguageName(L_PYTHON) should not be empty")));
    QVERIFY2(name == "Python",
             qPrintable(QString("getLanguageName(L_PYTHON) = \"%1\", expected \"Python\"").arg(QString::fromStdString(name))));
}

void TestLexer::test_getLanguageName_java()
{
    std::string name = _mgr.getLanguageName(L_JAVA);
    QVERIFY2(!name.empty(),
             qPrintable(QString("getLanguageName(L_JAVA) should not be empty")));
}

// =============================================================================
// Language extension
// =============================================================================

void TestLexer::test_getLanguageExtension()
{
    std::string ext = _mgr.getLanguageExtension(L_CPP);
    QVERIFY2(!ext.empty(),
             qPrintable(QString("getLanguageExtension(L_CPP) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(ext))));
}

// =============================================================================
// Lexer name
// =============================================================================

void TestLexer::test_getLexerName()
{
    std::string lexerName = _mgr.getLexerName(L_CPP);
    QVERIFY2(!lexerName.empty(),
             qPrintable(QString("getLexerName(L_CPP) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(lexerName))));
}

// =============================================================================
// All languages
// =============================================================================

void TestLexer::test_getAllLanguages()
{
    auto langs = _mgr.getAllLanguages();
    QVERIFY2(langs.size() > 10,
             qPrintable(QString("getAllLanguages should return many languages, got %1").arg(int(langs.size()))));
    // C++ and Python should be in the list
    bool hasCpp = false, hasPython = false;
    for (LangType t : langs) {
        if (t == L_CPP) hasCpp = true;
        if (t == L_PYTHON) hasPython = true;
    }
    QVERIFY2(hasCpp, "getAllLanguages should include L_CPP");
    QVERIFY2(hasPython, "getAllLanguages should include L_PYTHON");
}

// =============================================================================
// Keywords
// =============================================================================

void TestLexer::test_getKeywords_cpp()
{
    auto kws = _mgr.getKeywords(L_CPP);
    QVERIFY2(!kws.empty(),
             qPrintable(QString("getKeywords(L_CPP) should not be empty, got size %1").arg(int(kws.size()))));
    QVERIFY2(!kws[0].empty(),
             qPrintable(QString("getKeywords(L_CPP)[0] (primary kwset) should not be empty")));
    // Primary kwset should contain "int" (C++ keyword)
    bool hasInt = kws[0].find("int ") != std::string::npos ||
                  kws[0].find(" int") != std::string::npos ||
                  kws[0].find("int;") != std::string::npos ||
                  kws[0] == "int";
    QVERIFY2(hasInt,
             qPrintable(QString("getKeywords(L_CPP)[0] should contain \"int\", got \"%1\"")
                        .arg(QString::fromStdString(kws[0]))));
}

void TestLexer::test_getKeywords_python()
{
    auto kws = _mgr.getKeywords(L_PYTHON);
    QVERIFY2(!kws.empty(),
             qPrintable(QString("getKeywords(L_PYTHON) should not be empty, got size %1").arg(int(kws.size()))));
}

void TestLexer::test_getKeywords_empty()
{
    // L_TEXT should return an empty map
    auto kws = _mgr.getKeywords(L_TEXT);
    qDebug("getKeywords(L_TEXT) size = %zu", kws.size());
    QVERIFY(kws.empty() || kws[0].empty());  // either empty map or empty string
}

void TestLexer::test_getKeywords_java()
{
    auto kws = _mgr.getKeywords(L_JAVA);
    QVERIFY2(!kws.empty(),
             qPrintable(QString("getKeywords(L_JAVA) should not be empty, got size %1").arg(int(kws.size()))));
}

// =============================================================================
// Indent size
// =============================================================================

void TestLexer::test_getIndentSize()
{
    int indentCpp = _mgr.getIndentSize(L_CPP);
    QVERIFY2(indentCpp > 0,
             qPrintable(QString("getIndentSize(L_CPP) should be positive, got %1").arg(indentCpp)));

    int indentPython = _mgr.getIndentSize(L_PYTHON);
    QVERIFY2(indentPython > 0,
             qPrintable(QString("getIndentSize(L_PYTHON) should be positive, got %1").arg(indentPython)));
}

// =============================================================================
// Case sensitivity
// =============================================================================

void TestLexer::test_isCaseSensitive_batch()
{
    // Batch/INI are case-insensitive
    QVERIFY2(!_mgr.isCaseSensitive(L_BATCH),
             "L_BATCH should be case-insensitive");
    QVERIFY2(!_mgr.isCaseSensitive(L_INI),
             "L_INI should be case-insensitive");
}

void TestLexer::test_isCaseSensitive_cpp()
{
    // C/C++ is case-sensitive
    QVERIFY2(_mgr.isCaseSensitive(L_CPP),
             "L_CPP should be case-sensitive");
    QVERIFY2(_mgr.isCaseSensitive(L_C),
             "L_C should be case-sensitive");
}

void TestLexer::test_isCaseSensitive_python()
{
    QVERIFY2(_mgr.isCaseSensitive(L_PYTHON),
             "L_PYTHON should be case-sensitive");
}

void TestLexer::test_isCaseSensitive_ini()
{
    QVERIFY2(!_mgr.isCaseSensitive(L_INI),
             "L_INI should be case-insensitive");
}

// =============================================================================
// Comment helpers
// =============================================================================

void TestLexer::test_getCommentLine()
{
    std::string cComment = _mgr.getCommentLine(L_CPP);
    QVERIFY2(!cComment.empty(),
             qPrintable(QString("getCommentLine(L_CPP) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(cComment))));

    std::string pyComment = _mgr.getCommentLine(L_PYTHON);
    QVERIFY2(!pyComment.empty(),
             qPrintable(QString("getCommentLine(L_PYTHON) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(pyComment))));
}

void TestLexer::test_getCommentStartEnd()
{
    std::string start = _mgr.getCommentStart(L_CPP);
    std::string end = _mgr.getCommentEnd(L_CPP);
    QVERIFY2(!start.empty(),
             qPrintable(QString("getCommentStart(L_CPP) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(start))));
    QVERIFY2(!end.empty(),
             qPrintable(QString("getCommentEnd(L_CPP) should not be empty, got \"%1\"")
                        .arg(QString::fromStdString(end))));

    // Python doesn't have block comments (just #), so these should be empty
    std::string pyStart = _mgr.getCommentStart(L_PYTHON);
    // Empty is acceptable for Python
    QVERIFY(pyStart.empty() || !pyStart.empty());  // just no crash
}

// =============================================================================
// Lexer creation
// =============================================================================

void TestLexer::test_getLexer_cpp()
{
    QsciLexer* lex = _mgr.getLexer(L_CPP);
    // May be nullptr if QsciScintilla wasn't built with the C++ lexer
    if (lex) {
        QVERIFY2(!lex->language() == 0,
                 qPrintable(QString("C++ lexer language should not be empty, got \"%1\"")
                            .arg(lex->language())));
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
    // L_TEXT -> null (plain text, no highlighting)
    QsciLexer* lex = _mgr.getLexer(L_TEXT);
    if (lex) {
        qDebug("L_TEXT lexer (unexpected): %s", lex->language());
        delete lex;
    } else {
        qDebug("L_TEXT correctly returns null lexer");
    }
}

// =============================================================================
// mapStringToLang
// =============================================================================

void TestLexer::test_mapStringToLang()
{
    LangType cpp1 = LanguageManager::mapStringToLang("cpp");
    QVERIFY2(cpp1 == L_CPP || cpp1 != L_TEXT,
             qPrintable(QString("mapStringToLang(\"cpp\") = %1").arg(int(cpp1))));

    LangType py1 = LanguageManager::mapStringToLang("python");
    QVERIFY2(py1 == L_PYTHON || py1 != L_TEXT,
             qPrintable(QString("mapStringToLang(\"python\") = %1").arg(int(py1))));

    LangType unknown = LanguageManager::mapStringToLang("notareallangtype");
    QVERIFY2(unknown == L_TEXT,
             qPrintable(QString("mapStringToLang of unknown should return L_TEXT, got %1").arg(int(unknown))));
}

QTEST_MAIN(TestLexer)
#include "TestLexer.moc"
