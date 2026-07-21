// test_lexer_regression.cpp — Regression tests for the syntax-highlighting
// extension-normalization fix (commit 26fe60e).
//
// Bug history: QFileInfo::suffix() returns the extension WITHOUT the leading dot
// ("py", not ".py"). LanguageManager's internal map uses keys WITH the dot
// (".py", ".cpp", ...). Every Buffer ctor / FileManager::detectLanguage caller
// fed the dot-less suffix into getLanguageForExtension(), which then missed
// and returned L_TEXT for every file. With lang == L_TEXT, createLexer()
// returned nullptr, so ScintillaEditor::setLanguage attached no QsciLexer
// and every buffer was rendered as plain text. The bug was completely silent.
//
// These tests pin down the fix so a future regression (e.g., someone calling
// _extToLang with raw suffixes in a new code path) is caught immediately.

#include "core/LanguageManager.h"
#include "common/Types.h"
#include "editor/ScintillaEditor.h"
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

using enum LangType;

#define TEST(name) static void name()
#define RUN(name) do { std::cout << "  " #name "... "; name(); std::cout << "OK\n"; } while(0)
#define EXPECT(x, msg) do { \
        if (!(x)) { std::fprintf(stderr, "\nFAIL: %s\n  %s\n", #x, (msg)); std::exit(1); } \
    } while(0)

// ---------------------------------------------------------------------------
// Regression: LanguageManager::getLanguageForExtension must accept the bare
// suffix from QFileInfo::suffix() (no dot) and the dotted form equally. Also
// case-insensitive. Otherwise every file maps to L_TEXT and no lexer attaches.
// ---------------------------------------------------------------------------

TEST(test_detect_dotless_suffix_python) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("py") == L_PYTHON,
           "bare suffix 'py' must map to L_PYTHON (was the user-visible bug)");
}

TEST(test_detect_dotless_suffix_cpp) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("cpp") == L_CPP,
           "bare suffix 'cpp' must map to L_CPP");
    EXPECT(mgr.getLanguageForExtension("h") == L_CPP,
           "bare suffix 'h' (C/C++ header) must map to L_CPP");
}

TEST(test_detect_dotless_suffix_js) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("js") == L_JS,
           "bare suffix 'js' must map to L_JS");
}

TEST(test_detect_dotless_suffix_json) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("json") == L_JSON,
           "bare suffix 'json' must map to L_JSON");
}

TEST(test_detect_dotless_suffix_markdown) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("md") == L_MARKDOWN,
           "bare suffix 'md' must map to L_MARKDOWN");
}

TEST(test_detect_dotless_suffix_bash) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("sh") == L_BASH,
           "bare suffix 'sh' must map to L_BASH (was mapped to L_BATCH before)");
}

TEST(test_detect_dotted_form_still_works) {
    // The internal map keys include the dot. Some callers might still prepend
    // it manually. Both forms must work.
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension(".py") == L_PYTHON, "dotted '.py' must map to L_PYTHON");
    EXPECT(mgr.getLanguageForExtension(".cpp") == L_CPP,   "dotted '.cpp' must map to L_CPP");
    EXPECT(mgr.getLanguageForExtension(".sh") == L_BASH,   "dotted '.sh' must map to L_BASH");
}

TEST(test_detect_case_insensitive) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("PY")    == L_PYTHON, "'PY' uppercase");
    EXPECT(mgr.getLanguageForExtension(".CPP")  == L_CPP,    "'.CPP' uppercase");
    EXPECT(mgr.getLanguageForExtension("Json")  == L_JSON,   "'Json' mixed case");
}

TEST(test_detect_empty_and_unknown) {
    LanguageManager mgr;
    EXPECT(mgr.getLanguageForExtension("")     == L_TEXT, "empty extension must return L_TEXT");
    EXPECT(mgr.getLanguageForExtension("xyzzy") == L_TEXT, "unknown extension must return L_TEXT");
    EXPECT(mgr.getLanguageForExtension(".")    == L_TEXT, "just a dot must return L_TEXT");
}

// ---------------------------------------------------------------------------
// Regression: createLexer() must produce a real QsciLexer for languages with
// QScintilla coverage. Was returning nullptr for L_BASH, L_SQL, L_TEX,
// L_DIFF, L_PHP, etc., so even after extension normalization worked, those
// file types still rendered as plain text.
// ---------------------------------------------------------------------------

TEST(test_createLexer_python_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_PYTHON);
    EXPECT(lex != nullptr, "L_PYTHON must produce a non-null QsciLexer");
    delete lex;
}

TEST(test_createLexer_cpp_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_CPP);
    EXPECT(lex != nullptr, "L_CPP must produce a non-null QsciLexer");
    delete lex;
}

TEST(test_createLexer_javascript_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_JS);
    EXPECT(lex != nullptr, "L_JS must produce a non-null QsciLexer");
    delete lex;
}

TEST(test_createLexer_json_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_JSON);
    EXPECT(lex != nullptr, "L_JSON must produce a non-null QsciLexer");
    delete lex;
}

TEST(test_createLexer_sql_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_SQL);
    EXPECT(lex != nullptr, "L_SQL must produce a non-null QsciLexer (was nullptr before fix)");
    delete lex;
}

TEST(test_createLexer_bash_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_BASH);
    EXPECT(lex != nullptr, "L_BASH must produce a non-null QsciLexer (was nullptr before fix)");
    delete lex;
}

TEST(test_createLexer_tex_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_TEX);
    EXPECT(lex != nullptr, "L_TEX must produce a non-null QsciLexer (was nullptr before fix)");
    delete lex;
}

TEST(test_createLexer_diff_not_null) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_DIFF);
    EXPECT(lex != nullptr, "L_DIFF must produce a non-null QsciLexer (was nullptr before fix)");
    delete lex;
}

TEST(test_createLexer_text_is_null_by_design) {
    // L_TEXT is plain text by definition — no lexer is the correct behavior.
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_TEXT);
    EXPECT(lex == nullptr, "L_TEXT must produce nullptr (plain text)");
}

// ---------------------------------------------------------------------------
// Regression: full filename -> LangType detection path (Buffer ctor uses
// this via FileManager::loadFile). Was the user-visible bug.
// ---------------------------------------------------------------------------

TEST(test_detect_from_full_filename) {
    LanguageManager mgr;
    EXPECT(mgr.detectLanguage("/tmp/foo.py")      == L_PYTHON, "foo.py -> L_PYTHON");
    EXPECT(mgr.detectLanguage("/tmp/main.cpp")    == L_CPP,    "main.cpp -> L_CPP");
    EXPECT(mgr.detectLanguage("/tmp/app.js")      == L_JS,     "app.js -> L_JS");
    EXPECT(mgr.detectLanguage("/tmp/data.json")   == L_JSON,   "data.json -> L_JSON");
    EXPECT(mgr.detectLanguage("/tmp/build.sh")    == L_BASH,   "build.sh -> L_BASH (was L_BATCH)");
    EXPECT(mgr.detectLanguage("/tmp/q.sql")       == L_SQL,    "q.sql -> L_SQL");
    EXPECT(mgr.detectLanguage("/tmp/p.tex")       == L_TEX,    "p.tex -> L_TEX");
    EXPECT(mgr.detectLanguage("/tmp/p.patch")     == L_DIFF,   "p.patch -> L_DIFF");
    EXPECT(mgr.detectLanguage("/tmp/README.md")   == L_MARKDOWN, "README.md -> L_MARKDOWN");
    EXPECT(mgr.detectLanguage("/tmp/note.txt")    == L_TEXT,   "note.txt -> L_TEXT");
    EXPECT(mgr.detectLanguage("/tmp/Makefile")    == L_MAKEFILE, "Makefile -> L_MAKEFILE");
}

// ---------------------------------------------------------------------------
// Regression: L_OBJ should not be aliased to QsciLexerBash. QScintilla
// ships no ObjC lexer, so the fallback should be the closest C-family lexer
// (QsciLexerCPP), not QsciLexerBash which miscolors Objective-C keywords.
// ---------------------------------------------------------------------------

TEST(test_createLexer_objc_is_not_bash) {
    LanguageManager mgr;
    QsciLexer* lex = mgr.getLexer(L_OBJC);
    EXPECT(lex != nullptr, "L_OBJC must produce a non-null QsciLexer");
    // Best-effort type check via dynamic_cast — the lexer is a QsciLexerCPP
    // subclass in current QScintilla builds.
    auto* cppLex = dynamic_cast<QsciLexerCPP*>(lex);
    EXPECT(cppLex != nullptr, "L_OBJC should fall back to QsciLexerCPP (not QsciLexerBash)");
    delete lex;
}

int main() {
    std::cout << "Lexer regression tests:\n";
    std::cout << "  Dot-less suffix detection:\n";
    RUN(test_detect_dotless_suffix_python);
    RUN(test_detect_dotless_suffix_cpp);
    RUN(test_detect_dotless_suffix_js);
    RUN(test_detect_dotless_suffix_json);
    RUN(test_detect_dotless_suffix_markdown);
    RUN(test_detect_dotless_suffix_bash);
    RUN(test_detect_dotted_form_still_works);
    RUN(test_detect_case_insensitive);
    RUN(test_detect_empty_and_unknown);

    std::cout << "  Lexer creation:\n";
    RUN(test_createLexer_python_not_null);
    RUN(test_createLexer_cpp_not_null);
    RUN(test_createLexer_javascript_not_null);
    RUN(test_createLexer_json_not_null);
    RUN(test_createLexer_sql_not_null);
    RUN(test_createLexer_bash_not_null);
    RUN(test_createLexer_tex_not_null);
    RUN(test_createLexer_diff_not_null);
    RUN(test_createLexer_text_is_null_by_design);

    std::cout << "  End-to-end filename detection:\n";
    RUN(test_detect_from_full_filename);

    std::cout << "  Aliases:\n";
    RUN(test_createLexer_objc_is_not_bash);

    std::cout << "\n=== All lexer regression tests passed! ===\n";
    return 0;
}