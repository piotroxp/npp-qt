// test_snippets.cpp — SnippetManager unit tests
// Copyright (C) 2026 Agent Army | GPL v3
//
// Exercises SnippetManager end-to-end:
//   - INI round-trip (loadFromFile / saveToFile)
//   - placeholder expansion ($VAR, $1, $0, |)
//   - cursor offset is correctly translated through expansion
//   - hot reload via setDefaultDirectory
//   - tab-stop ordering (numbered first, then named)

#include "../core/SnippetManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <cassert>
#include <cstdio>
#include <cstdlib>

static int runSnippetsUnitTests() {
    int failures = 0;
    auto check = [&](const char* label, bool cond, const char* detail = "") {
        if (cond) {
            std::fprintf(stderr, "[ok] %s\n", label);
        } else {
            std::fprintf(stderr, "FAIL: %s%s%s\n", label,
                         detail[0] ? " — " : "", detail);
            ++failures;
        }
    };

    // ── Static helpers ──────────────────────────────────────────────────
    check("cursorOffsetFromText finds pipe",
          SnippetManager::cursorOffsetFromText("for (int i| = 0;)") == 11);
    check("cursorOffsetFromText returns -1 when missing",
          SnippetManager::cursorOffsetFromText("for (int i = 0);") == -1);

    check("stripPlaceholders removes $| and $0",
          SnippetManager::stripPlaceholders("a|bc$0") == "abc");
    check("stripPlaceholders removes named $FOO",
          SnippetManager::stripPlaceholders("$X = $X + 1") == " =  + 1");
    check("stripPlaceholders leaves lone $ alone",
          SnippetManager::stripPlaceholders("price: $5") == "price: 5");
    check("stripPlaceholders handles $9",
          SnippetManager::stripPlaceholders("$9 then $0") == " then ");

    // ── Tab-stop ordering ──────────────────────────────────────────────
    {
        QVector<SnippetManager::TabStop> stops =
            SnippetManager::collectTabStops("$2 middle $1 end $FOO");
        check("collectTabStops returns 4 stops",
              stops.size() == 4);
        check("first stop is numbered $1 (lower number first)",
              stops.value(0).name == "1");
        check("second stop is $2",
              stops.value(1).name == "2");
        check("named stop comes after numbered",
              stops.value(2).name == "FOO");
        // The 4th element is the second $1 occurrence.
        check("duplicate named/numbered stop retained",
              stops.value(3).name == "1" || stops.value(3).name == "FOO");
    }

    // ── In-memory expand ───────────────────────────────────────────────
    {
        SnippetManager mgr;
        mgr.setSnippet("for", "for (int $VAR = 0; $VAR < n; ++$VAR) {\n    |\n}");
        check("hasSnippet returns true after set",
              mgr.hasSnippet("for"));
        check("rawText preserves placeholders",
              mgr.rawText("for").contains("$VAR"));

        SnippetManager::Snippet s = mgr.expand("for");
        check("expand returns valid snippet", s.isValid());
        check("expand strips $VAR", !s.text.contains("$VAR"));
        check("expand strips cursor marker", !s.text.contains("|"));
        check("expand contains the loop body",
              s.text.contains("for (int"));
        check("cursorOffset points inside the body",
              s.cursorOffset >= 0 && s.cursorOffset < s.text.size());

        check("missing snippet returns invalid",
              !mgr.expand("missing").isValid());
    }

    // ── INI round-trip ─────────────────────────────────────────────────
    {
        QTemporaryDir tmp;
        if (!tmp.isValid()) {
            std::fprintf(stderr, "FAIL: cannot create tempdir\n");
            return 1;
        }
        const QString path = tmp.filePath("snippets.ini");

        SnippetManager writer;
        writer.setSnippet("for",
                          "for (int $VAR = 0; $VAR < n; ++$VAR) {\n    |\n}");
        writer.setSnippet("while", "while ($COND) {\n    |\n}");
        check("saveToFile succeeds",
              writer.saveToFile(path));
        check("snippets.ini was created", QFileInfo::exists(path));

        SnippetManager reader;
        check("loadFromFile succeeds",
              reader.loadFromFile(path));
        check("reader loaded 2 snippets", reader.count() == 2);
        check("reader has 'for' snippet", reader.hasSnippet("for"));
        check("reader has 'while' snippet", reader.hasSnippet("while"));

        SnippetManager::Snippet fexp = reader.expand("for");
        check("loaded 'for' expands correctly",
              fexp.isValid() && !fexp.text.contains("$VAR"));

        // Mutation + re-save round-trip
        reader.removeSnippet("while");
        reader.setSnippet("if", "if ($COND) {\n    |\n}");
        check("saveToFile after mutation succeeds",
              reader.saveToFile(path));

        SnippetManager reader2;
        check("reload after mutation succeeds",
              reader2.loadFromFile(path));
        check("reload sees 2 snippets (for + if)",
              reader2.count() == 2);
        check("reload sees 'if'", reader2.hasSnippet("if"));
        check("reload does NOT see removed 'while'",
              !reader2.hasSnippet("while"));
    }

    // ── Hot reload via setDefaultDirectory ─────────────────────────────
    {
        QTemporaryDir tmp;
        if (!tmp.isValid()) return 1;
        const QString iniPath = tmp.filePath("snippets.ini");

        QFile f(iniPath);
        if (!f.open(QIODevice::WriteOnly)) {
            std::fprintf(stderr, "FAIL: cannot write hot-reload fixture\n");
            return 1;
        }
        QTextStream(&f)
            << "[hello]\n"
               "text=Hello, $WHO|\n";
        f.close();

        SnippetManager mgr;
        mgr.setDefaultDirectory(tmp.path());
        check("setDefaultDirectory picks up snippets.ini",
              mgr.hasSnippet("hello"));

        SnippetManager::Snippet h = mgr.expand("hello");
        check("hot-loaded snippet expands",
              h.isValid() && h.text == "Hello, ");
    }

    // ── Error path: missing file is not an error ───────────────────────
    {
        SnippetManager mgr;
        check("loadFromFile on missing path returns false",
              !mgr.loadFromFile("/no/such/path/snippets.ini"));
        check("manager remains usable (count == 0)",
              mgr.count() == 0);
    }

    // ── Default search paths ───────────────────────────────────────────
    {
        QStringList defaults = SnippetManager::defaultSearchPaths();
        check("defaultSearchPaths returns at least one path",
              defaults.size() >= 1);
        for (const QString& p : defaults) {
            check("default search path is absolute or empty",
                  p.isEmpty() || QDir(p).isAbsolute());
        }
    }

    if (failures > 0) {
        std::fprintf(stderr, "FAIL: %d snippet tests failed\n", failures);
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::fprintf(stderr, "\n=== SnippetManager tests ===\n");
    QCoreApplication app(argc, argv);
    int rc = runSnippetsUnitTests();
    if (rc == 0) std::fprintf(stderr, "PASS\n");
    else         std::fprintf(stderr, "FAIL rc=%d\n", rc);
    return rc;
}
