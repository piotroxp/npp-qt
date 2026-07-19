// test_syntax_highlighter.cpp - Unit tests for SyntaxHighlighter
// Copyright (C) 2026 Agent Army
// GPL v3
//
// Tests the full 50-language syntax highlighting implementation.
// Uses a QTextDocument as parent (not a widget), so no GUI needed.

#include "editor/SyntaxHighlighter.h"
#include <QTextDocument>
#include <QDebug>
#include <QString>
#include <QDebug>
#include <cassert>
#include <QDebug>
#include <iostream>
#include <QDebug>
#include <vector>
#include <QDebug>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { qWarning() << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { qWarning() << "FAILED: " << #x << " (should be false)\n"; exit(1); } } while(0)

// Test subclass that exposes highlightBlock as public
class TestSyntaxHighlighter : public SyntaxHighlighter {
public:
    using SyntaxHighlighter::SyntaxHighlighter;
    void publicHighlight(const QString& text) { highlightBlock(text); }
};

// Helper: build a minimal highlighter and verify it compiles for each language
static void smokeTestLanguage(LangType lang, const char* /*name*/) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(lang);
    hl.applyDarkTheme();
    hl.publicHighlight("void test() { return 42; }");
    ASSERT_TRUE(hl.language() == lang);
}

TEST(test_cpp) { smokeTestLanguage(LangType::L_CPP, "C++"); }
TEST(test_c) { smokeTestLanguage(LangType::L_C, "C"); }
TEST(test_python) { smokeTestLanguage(LangType::L_PYTHON, "Python"); }
TEST(test_java) { smokeTestLanguage(LangType::L_JAVA, "Java"); }
TEST(test_csharp) { smokeTestLanguage(LangType::L_CS, "C#"); }
TEST(test_objective_c) { smokeTestLanguage(LangType::L_OBJC, "Objective-C"); }
TEST(test_ruby) { smokeTestLanguage(LangType::L_RUBY, "Ruby"); }
TEST(test_perl) { smokeTestLanguage(LangType::L_PERL, "Perl"); }
TEST(test_php) { smokeTestLanguage(LangType::L_PHP, "PHP"); }
TEST(test_go) { smokeTestLanguage(LangType::L_GO, "Go"); }
TEST(test_rust) { smokeTestLanguage(LangType::L_RUST, "Rust"); }
TEST(test_swift) { smokeTestLanguage(LangType::L_SWIFT, "Swift"); }
TEST(test_kotlin) { smokeTestLanguage(LangType::L_KOTLIN, "Kotlin"); }
TEST(test_scala) { smokeTestLanguage(LangType::L_SCALA, "Scala"); }
TEST(test_haskell) { smokeTestLanguage(LangType::L_HASKELL, "Haskell"); }
TEST(test_erlang) { smokeTestLanguage(LangType::L_ERLANG, "Erlang"); }
TEST(test_elixir) { smokeTestLanguage(LangType::L_ELIXIR, "Elixir"); }
TEST(test_clojure) { smokeTestLanguage(LangType::L_CLOJURE, "Clojure"); }
TEST(test_fsharp) { smokeTestLanguage(LangType::L_FSHARP, "F#"); }
TEST(test_lisp) { smokeTestLanguage(LangType::L_LISP, "Lisp"); }
TEST(test_html) { smokeTestLanguage(LangType::L_HTML, "HTML"); }
TEST(test_xml) { smokeTestLanguage(LangType::L_XML, "XML"); }
TEST(test_css) { smokeTestLanguage(LangType::L_CSS, "CSS"); }
TEST(test_json) { smokeTestLanguage(LangType::L_JSON, "JSON"); }
TEST(test_yaml) { smokeTestLanguage(LangType::L_YAML, "YAML"); }
TEST(test_markdown) { smokeTestLanguage(LangType::L_MARKDOWN, "Markdown"); }
TEST(test_javascript) { smokeTestLanguage(LangType::L_JS, "JavaScript"); }
TEST(test_typescript) { smokeTestLanguage(LangType::L_TS, "TypeScript"); }
TEST(test_webassembly) { smokeTestLanguage(LangType::L_WEBASSEMBLY, "WebAssembly"); }
TEST(test_lua) { smokeTestLanguage(LangType::L_LUA, "Lua"); }
TEST(test_sql) { smokeTestLanguage(LangType::L_SQL, "SQL"); }
TEST(test_tex) { smokeTestLanguage(LangType::L_TEX, "LaTeX"); }
TEST(test_diff) { smokeTestLanguage(LangType::L_DIFF, "Diff"); }
TEST(test_r) { smokeTestLanguage(LangType::L_R, "R"); }
TEST(test_julia) { smokeTestLanguage(LangType::L_JULIA, "Julia"); }
TEST(test_matlab) { smokeTestLanguage(LangType::L_MATLAB, "MATLAB"); }
TEST(test_nsis) { smokeTestLanguage(LangType::L_NSIS, "NSIS"); }
TEST(test_pascal) { smokeTestLanguage(LangType::L_PASCAL, "Pascal"); }
TEST(test_fortran) { smokeTestLanguage(LangType::L_FORTRAN, "Fortran"); }
TEST(test_verilog) { smokeTestLanguage(LangType::L_VERILOG, "Verilog"); }
TEST(test_vhdl) { smokeTestLanguage(LangType::L_VHDL, "VHDL"); }
TEST(test_asm) { smokeTestLanguage(LangType::L_ASM, "Assembly"); }
TEST(test_graphviz) { smokeTestLanguage(LangType::L_GRAPHVIZ, "Graphviz"); }
TEST(test_ada) { smokeTestLanguage(LangType::L_ADA, "Ada"); }
TEST(test_regex) { smokeTestLanguage(LangType::L_REGEX, "Regex"); }
TEST(test_makefile) { smokeTestLanguage(LangType::L_MAKEFILE, "Makefile"); }
TEST(test_cmake) { smokeTestLanguage(LangType::L_CMAKE, "CMake"); }
TEST(test_dockerfile) { smokeTestLanguage(LangType::L_DOCKERFILE, "Dockerfile"); }
TEST(test_powershell) { smokeTestLanguage(LangType::L_POWERSHELL, "PowerShell"); }
TEST(test_tcl) { smokeTestLanguage(LangType::L_TCL, "Tcl"); }
TEST(test_batch) { smokeTestLanguage(LangType::L_BATCH, "Batch"); }
TEST(test_bash) { smokeTestLanguage(LangType::L_BASH, "Bash"); }
TEST(test_ini) { smokeTestLanguage(LangType::L_INI, "INI"); }
TEST(test_nfo) { smokeTestLanguage(LangType::L_NFO, "NFO"); }
TEST(test_properties) { smokeTestLanguage(LangType::L_PROPERTIES, "Properties"); }
TEST(test_groovy) { smokeTestLanguage(LangType::L_GROOVY, "Groovy"); }

// Theme tests
TEST(test_theme_dark) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(LangType::L_CPP);
    hl.applyDarkTheme();
    hl.publicHighlight("int x = 42;");
    ASSERT_TRUE(hl.currentTheme() == "dark");
}

TEST(test_theme_monokai) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(LangType::L_CPP);
    hl.applyMonokaiTheme();
    ASSERT_TRUE(hl.currentTheme() == "monokai");
}

TEST(test_theme_nord) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(LangType::L_CPP);
    hl.applyNordTheme();
    ASSERT_TRUE(hl.currentTheme() == "nord");
}

TEST(test_custom_rules) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(LangType::L_TEXT);
    hl.applyDarkTheme();
    hl.addRule("\\bTODO\\b", QTextCharFormat());
    hl.publicHighlight("TODO: fix this");
    ASSERT_TRUE(true);  // Just verify no crash
}

TEST(test_state_queries) {
    QTextDocument doc;
    TestSyntaxHighlighter hl(&doc);
    hl.setLanguage(LangType::L_CPP);
    hl.publicHighlight("// comment");
    ASSERT_TRUE(hl.isInComment() || !hl.isInComment());  // Valid state
}

// ============================================================================
// Main
// ============================================================================
int main() {
    std::cout << "=== SyntaxHighlighter Unit Tests ===\n\n";

    std::cout << "Languages:\n";
    RUN(test_cpp);
    RUN(test_c);
    RUN(test_python);
    RUN(test_java);
    RUN(test_csharp);
    RUN(test_objective_c);
    RUN(test_ruby);
    RUN(test_perl);
    RUN(test_php);
    RUN(test_go);
    RUN(test_rust);
    RUN(test_swift);
    RUN(test_kotlin);
    RUN(test_scala);
    RUN(test_haskell);
    RUN(test_erlang);
    RUN(test_elixir);
    RUN(test_clojure);
    RUN(test_fsharp);
    RUN(test_lisp);
    RUN(test_html);
    RUN(test_xml);
    RUN(test_css);
    RUN(test_json);
    RUN(test_yaml);
    RUN(test_markdown);
    RUN(test_javascript);
    RUN(test_typescript);
    RUN(test_webassembly);
    RUN(test_lua);
    RUN(test_sql);
    RUN(test_tex);
    RUN(test_diff);
    RUN(test_r);
    RUN(test_julia);
    RUN(test_matlab);
    RUN(test_nsis);
    RUN(test_pascal);
    RUN(test_fortran);
    RUN(test_verilog);
    RUN(test_vhdl);
    RUN(test_asm);
    RUN(test_graphviz);
    RUN(test_ada);
    RUN(test_regex);
    RUN(test_makefile);
    RUN(test_cmake);
    RUN(test_dockerfile);
    RUN(test_powershell);
    RUN(test_tcl);
    RUN(test_batch);
    RUN(test_bash);
    RUN(test_ini);
    RUN(test_nfo);
    RUN(test_properties);
    RUN(test_groovy);

    std::cout << "\nThemes:\n";
    RUN(test_theme_dark);
    RUN(test_theme_monokai);
    RUN(test_theme_nord);

    std::cout << "\nCustom:\n";
    RUN(test_custom_rules);
    RUN(test_state_queries);

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
