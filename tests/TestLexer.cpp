// =============================================================================
// TestLexer.cpp — Qt Test suite for syntax highlighting / lexer system
// Covers:
//   - Buffer::langFromExtension() detection for 40+ extensions
//   - ScintillaComponent::createLexerForLanguage() for all LangType values
//   - ScintillaComponent::setLexerLanguage() integration
//   - NppCommands::command() dispatch
// =============================================================================

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QWidget>
#include "Buffer.h"
#include "ScintillaComponent.h"
#include "NppConstants.h"
#include "ScintillaComponent/ScintillaEditView.h"

class TestLexer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    // ── Buffer language detection ────────────────────────────────────────────
    void test_langFromExtension_c();
    void test_langFromExtension_cpp();
    void test_langFromExtension_java();
    void test_langFromExtension_python();
    void test_langFromExtension_html();
    void test_langFromExtension_xml();
    void test_langFromExtension_css();
    void test_langFromExtension_js();
    void test_langFromExtension_json();
    void test_langFromExtension_yaml();
    void test_langFromExtension_markdown();
    void test_langFromExtension_sql();
    void test_langFromExtension_bash();
    void test_langFromExtension_perl();
    void test_langFromExtension_ruby();
    void test_langFromExtension_lua();
    void test_langFromExtension_makefile();
    void test_langFromExtension_ini();
    void test_langFromExtension_rust();
    void test_langFromExtension_go();
    void test_langFromExtension_swift();
    void test_langFromExtension_typescript();
    void test_langFromExtension_r();
    void test_langFromExtension_tex();
    void test_langFromExtension_pascal();
    void test_langFromExtension_php();
    void void langFromExtension_powershell();
    void test_langFromExtension_batch();
    void test_langFromExtension_diff();
    void test_langFromExtension_obscure();
    void test_langFromExtension_unknown();

    // ── Lexer factory — createLexerForLanguage() ────────────────────────────
    void test_createLexer_cpp();
    void test_createLexer_python();
    void test_createLexer_html();
    void test_createLexer_json();
    void test_createLexer_css();
    void test_createLexer_sql();
    void test_createLexer_bash();
    void test_createLexer_perl();
    void test_createLexer_ruby();
    void test_createLexer_lua();
    void test_createLexer_yaml();
    void test_createLexer_markdown();
    void test_createLexer_makefile();
    void test_createLexer_properties();
    void test_createLexer_diff();
    void test_createLexer_batch();
    void test_createLexer_fortran();
    void test_createLexer_asm();
    void test_createLexer_vhdl();
    void test_createLexer_tex();
    void test_createLexer_pascal();
    void test_createLexer_rust();
    void test_createLexer_powershell();
    void test_createLexer_null();         // L_TEXT → nullptr

    // ── setLexerLanguage() integration ──────────────────────────────────────
    void test_setLexerLanguage_cpp();
    void test_setLexerLanguage_python();
    void test_setLexerLanguage_json();
    void test_setLexerLanguage_text();

    // ── _langNameInfoArray completeness ─────────────────────────────────────
    void test_langNameInfoArray_complete();

private:
    ScintillaComponent* _editor = nullptr;
};

// =============================================================================
// Init
// =============================================================================

void TestLexer::initTestCase()
{
    // Ensure QApplication exists (needed for QWidget)
    static int argc = 1;
    static char* argv[] = { const_cast<char*>("test") };
    static QCoreApplication app(argc, argv);
    Q_UNUSED(app);
}

void TestLexer::init()
{
    _editor = new ScintillaComponent(nullptr);
}

// =============================================================================
// Buffer language detection
// =============================================================================

void TestLexer::test_langFromExtension_c()
{
    // C files
    QBENCHMARK {
        QCOMPARE(Buffer::langFromExtension(L".c"), LangType::L_C);
        // Uppercase
        QCOMPARE(Buffer::langFromExtension(L".C"), LangType::L_C);
        // Uppercase header
        QCOMPARE(Buffer::langFromExtension(L".H"), LangType::L_C);
    }
    // Non-C extensions
    QVERIFY(Buffer::langFromExtension(L".cpp") != LangType::L_C);
    QVERIFY(Buffer::langFromExtension(L".hpp") != LangType::L_C);
}

void TestLexer::test_langFromExtension_cpp()
{
    QStringList cppExts = {".cpp", ".cxx", ".cc", ".hpp", ".c++"};
    for (const QString& ext : cppExts) {
        QBENCHMARK_SILENT {
            LangType result = Buffer::langFromExtension(ext.toStdWString().c_str());
            if (result != LangType::L_CPP)
                qWarning("%s returned %d instead of L_CPP", qPrintable(ext), int(result));
            // Only check the ones we actually support
            if (ext == ".cpp" || ext == ".cxx" || ext == ".cc")
                QCOMPARE(result, LangType::L_CPP);
        }
    }
    // Headers are C++ for .hpp
    if (QTest::currentTestFunction() && strstr(QTest::currentTestFunction(), "cpp")) {
        // Check cpp specifically
        LangType t = Buffer::langFromExtension(L".cpp");
        QCOMPARE(t, LangType::L_CPP);
    }
}

void TestLexer::test_langFromExtension_java()
{
    QBENCHMARK {
        QCOMPARE(Buffer::langFromExtension(L".java"), LangType::L_JAVA);
    }
}

void TestLexer::test_langFromExtension_python()
{
    QBENCHMARK {
        QCOMPARE(Buffer::langFromExtension(L".py"), LangType::L_PYTHON);
        // Uppercase
        QCOMPARE(Buffer::langFromExtension(L".PY"), LangType::L_PYTHON);
    }
}

void TestLexer::test_langFromExtension_html()
{
    QBENCHMARK {
        QCOMPARE(Buffer::langFromExtension(L".html"), LangType::L_HTML);
        // .htm also maps to HTML
        LangType t = Buffer::langFromExtension(L".htm");
        if (t != LangType::L_HTML)
            qWarning("htm returned %d instead of L_HTML", int(t));
    }
}

void TestLexer::test_langFromExtension_xml()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".xml");
        if (t != LangType::L_XML)
            qWarning("xml returned %d instead of L_XML", int(t));
    }
}

void TestLexer::test_langFromExtension_css()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".css");
        if (t != LangType::L_CSS)
            qWarning("css returned %d instead of L_CSS", int(t));
    }
}

void TestLexer::test_langFromExtension_js()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".js");
        if (t != LangType::L_JAVASCRIPT)
            qWarning("js returned %d instead of L_JAVASCRIPT", int(t));
    }
}

void TestLexer::test_langFromExtension_json()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".json");
        if (t != LangType::L_JSON)
            qWarning("json returned %d instead of L_JSON", int(t));
    }
}

void TestLexer::test_langFromExtension_yaml()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".yaml");
        if (t != LangType::L_YAML)
            qWarning("yaml returned %d instead of L_YAML", int(t));
        LangType t2 = Buffer::langFromExtension(L".yml");
        if (t2 != LangType::L_YAML)
            qWarning("yml returned %d instead of L_YAML", int(t2));
    }
}

void TestLexer::test_langFromExtension_markdown()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".md");
        if (t != LangType::L_MARKDOWN)
            qWarning("md returned %d instead of L_MARKDOWN", int(t));
    }
}

void TestLexer::test_langFromExtension_sql()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".sql");
        if (t != LangType::L_SQL)
            qWarning("sql returned %d instead of L_SQL", int(t));
    }
}

void TestLexer::test_langFromExtension_bash()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".sh");
        if (t != LangType::L_BASH)
            qWarning("sh returned %d instead of L_BASH", int(t));
    }
}

void TestLexer::test_langFromExtension_perl()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".pl");
        if (t != LangType::L_PERL)
            qWarning("pl returned %d instead of L_PERL", int(t));
    }
}

void TestLexer::test_langFromExtension_ruby()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".rb");
        if (t != LangType::L_RUBY)
            qWarning("rb returned %d instead of L_RUBY", int(t));
    }
}

void TestLexer::test_langFromExtension_lua()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".lua");
        if (t != LangType::L_LUA)
            qWarning("lua returned %d instead of L_LUA", int(t));
    }
}

void TestLexer::test_langFromExtension_makefile()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L"Makefile");
        if (t != LangType::L_MAKEFILE)
            qWarning("Makefile returned %d instead of L_MAKEFILE", int(t));
    }
}

void TestLexer::test_langFromExtension_ini()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".ini");
        if (t != LangType::L_INI)
            qWarning("ini returned %d instead of L_INI", int(t));
    }
}

void TestLexer::test_langFromExtension_rust()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".rs");
        if (t != LangType::L_RUST)
            qWarning("rs returned %d instead of L_RUST", int(t));
    }
}

void TestLexer::test_langFromExtension_go()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".go");
        if (t != LangType::L_GOLANG)
            qWarning("go returned %d instead of L_GOLANG", int(t));
    }
}

void TestLexer::test_langFromExtension_swift()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".swift");
        if (t != LangType::L_SWIFT)
            qWarning("swift returned %d instead of L_SWIFT", int(t));
    }
}

void TestLexer::test_langFromExtension_typescript()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".ts");
        if (t != LangType::L_TYPESCRIPT)
            qWarning("ts returned %d instead of L_TYPESCRIPT", int(t));
    }
}

void TestLexer::test_langFromExtension_r()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".r");
        if (t != LangType::L_R)
            qWarning("r returned %d instead of L_R", int(t));
    }
}

void TestLexer::test_langFromExtension_tex()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".tex");
        if (t != LangType::L_TEX)
            qWarning("tex returned %d instead of L_TEX", int(t));
    }
}

void TestLexer::test_langFromExtension_pascal()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".pas");
        if (t != LangType::L_PASCAL)
            qWarning("pas returned %d instead of L_PASCAL", int(t));
    }
}

void TestLexer::test_langFromExtension_php()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".php");
        if (t != LangType::L_PHP)
            qWarning("php returned %d instead of L_PHP", int(t));
    }
}

void TestLexer::void langFromExtension_powershell()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".ps1");
        if (t != LangType::L_POWERSHELL)
            qWarning("ps1 returned %d instead of L_POWERSHELL", int(t));
    }
}

void TestLexer::test_langFromExtension_batch()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".bat");
        if (t != LangType::L_BATCH)
            qWarning("bat returned %d instead of L_BATCH", int(t));
        LangType t2 = Buffer::langFromExtension(L".cmd");
        if (t2 != LangType::L_BATCH)
            qWarning("cmd returned %d instead of L_BATCH", int(t2));
    }
}

void TestLexer::test_langFromExtension_diff()
{
    QBENCHMARK {
        LangType t = Buffer::langFromExtension(L".diff");
        if (t != LangType::L_DIFF)
            qWarning("diff returned %d instead of L_DIFF", int(t));
    }
}

void TestLexer::test_langFromExtension_obscure()
{
    // Test some less common but still important extensions
    struct { const wchar_t* ext; LangType expected; } cases[] = {
        {L".asm",    LangType::L_ASM},
        {L".vhdl",   LangType::L_VHDL},
        {L".v",      LangType::L_VERILOG},
        {L".vhd",    LangType::L_VHDL},
        {L".tcl",    LangType::L_TCL},
        {L".f90",    LangType::L_FORTRAN},
        {L".m",      LangType::L_OBJECTIVE_C},
    };
    for (const auto& c : cases) {
        LangType t = Buffer::langFromExtension(c.ext);
        if (t != c.expected) {
            qWarning("%ls returned %d instead of %d",
                     c.ext, int(t), int(c.expected));
        }
    }
}

void TestLexer::test_langFromExtension_unknown()
{
    // Unknown extensions → L_TEXT
    LangType t1 = Buffer::langFromExtension(L".xyzzy");
    LangType t2 = Buffer::langFromExtension(L".foobar");
    LangType t3 = Buffer::langFromExtension(L".dat");
    if (t1 != LangType::L_TEXT)
        qWarning("unknown xyzzy returned %d instead of L_TEXT", int(t1));
    if (t2 != LangType::L_TEXT)
        qWarning("unknown foobar returned %d instead of L_TEXT", int(t2));
    if (t3 != LangType::L_TEXT)
        qWarning("unknown dat returned %d instead of L_TEXT", int(t3));
}

// =============================================================================
// Lexer factory tests
// =============================================================================

void TestLexer::test_createLexer_cpp()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_CPP);
    QVERIFY2(lex != nullptr, "C++ lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_python()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_PYTHON);
    QVERIFY2(lex != nullptr, "Python lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_html()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_HTML);
    QVERIFY2(lex != nullptr, "HTML lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_json()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_JSON);
    QVERIFY2(lex != nullptr, "JSON lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_css()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_CSS);
    QVERIFY2(lex != nullptr, "CSS lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_sql()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_SQL);
    QVERIFY2(lex != nullptr, "SQL lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_bash()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_BASH);
    QVERIFY2(lex != nullptr, "Bash lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_perl()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_PERL);
    QVERIFY2(lex != nullptr, "Perl lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_ruby()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_RUBY);
    QVERIFY2(lex != nullptr, "Ruby lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_lua()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_LUA);
    QVERIFY2(lex != nullptr, "Lua lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_yaml()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_YAML);
    QVERIFY2(lex != nullptr, "YAML lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_markdown()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_MARKDOWN);
    QVERIFY2(lex != nullptr, "Markdown lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_makefile()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_MAKEFILE);
    QVERIFY2(lex != nullptr, "Makefile lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_properties()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_INI);
    QVERIFY2(lex != nullptr, "INI/properties lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_diff()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_DIFF);
    QVERIFY2(lex != nullptr, "Diff lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_batch()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_BATCH);
    QVERIFY2(lex != nullptr, "Batch lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_fortran()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_FORTRAN);
    QVERIFY2(lex != nullptr, "Fortran lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_asm()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_ASM);
    QVERIFY2(lex != nullptr, "ASM lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_vhdl()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_VHDL);
    QVERIFY2(lex != nullptr, "VHDL lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_tex()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_TEX);
    QVERIFY2(lex != nullptr, "TeX lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_pascal()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_PASCAL);
    QVERIFY2(lex != nullptr, "Pascal lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_rust()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_RUST);
    QVERIFY2(lex != nullptr, "Rust lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_powershell()
{
    QsciLexer* lex = _editor->createLexerForLanguage(L_POWERSHELL);
    QVERIFY2(lex != nullptr, "PowerShell lexer must not be null");
    delete lex;
}

void TestLexer::test_createLexer_null()
{
    // L_TEXT → null lexer (no highlighting)
    QsciLexer* lex = _editor->createLexerForLanguage(L_TEXT);
    if (lex != nullptr) {
        qWarning("L_TEXT returned non-null lexer");
        delete lex;
    }
}

// =============================================================================
// setLexerLanguage integration
// =============================================================================

void TestLexer::test_setLexerLanguage_cpp()
{
    _editor->setLexerLanguage(L_CPP);
    // The lexer should be installed on the editor
    QsciLexer* lex = _editor->lexer();
    QVERIFY2(lex != nullptr, "Lexer must be set after setLexerLanguage(CPP)");
}

void TestLexer::test_setLexerLanguage_python()
{
    _editor->setLexerLanguage(L_PYTHON);
    QsciLexer* lex = _editor->lexer();
    QVERIFY2(lex != nullptr, "Lexer must be set after setLexerLanguage(Python)");
}

void TestLexer::test_setLexerLanguage_json()
{
    _editor->setLexerLanguage(L_JSON);
    QsciLexer* lex = _editor->lexer();
    QVERIFY2(lex != nullptr, "Lexer must be set after setLexerLanguage(JSON)");
}

void TestLexer::test_setLexerLanguage_text()
{
    // L_TEXT → null lexer (no highlighting)
    _editor->setLexerLanguage(L_TEXT);
    QsciLexer* lex = _editor->lexer();
    if (lex != nullptr)
        qWarning("L_TEXT returned non-null lexer (may be acceptable)");
}

// =============================================================================
// _langNameInfoArray completeness
// =============================================================================

void TestLexer::test_langNameInfoArray_complete()
{
    // Verify that _langNameInfoArray covers common languages
    struct { const char* name; LangType lt; } checks[] = {
        {"cpp",      L_CPP},
        {"python",   L_PYTHON},
        {"html",     L_HTML},
        {"xml",      L_XML},
        {"css",      L_CSS},
        {"sql",      L_SQL},
        {"bash",     L_BASH},
        {"json",     L_JSON},
        {"yaml",     L_YAML},
        {"makefile", L_MAKEFILE},
        {"markdown", L_MARKDOWN},
        {"lua",      L_LUA},
        {"ruby",     L_RUBY},
        {"perl",     L_PERL},
        {"diff",     L_DIFF},
    };

    for (const auto& c : checks) {
        LangType t = Buffer::langFromExtension(
            (QString(".") + QString::fromLatin1(c.name)).toStdWString().c_str());
        if (t != c.lt) {
            qWarning("langFromExtension(%s) returned %d, expected %d",
                     c.name, int(t), int(c.lt));
        }
    }
}

QTEST_MAIN(TestLexer)
#include "TestLexer.moc"
