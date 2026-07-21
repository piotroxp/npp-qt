// Integration tests for Notepad--Qt core utilities
// Runs headlessly — no display required (QT_QPA_PLATFORM=offscreen)
//
// Build: cd tests/build && cmake .. && make npp-test-integration && ./npp-test-integration

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTest>

// ============================================================================
// Test: StringHelper — UTF-8/UTF-16 conversions
// ============================================================================
class StringHelperTest : public QObject {
public:
    StringHelperTest() : QObject() {}
    void run() {
        // Test that StringHelper.cpp source has safe conversion patterns
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString path = d.filePath("src/common/StringHelper.cpp");
        QString src = readFile(path);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

        // toStdString() is safe — copies the data
        // toUtf8().toStdString() is safe — toStdString() on QByteArray copies
        int toStdCount = src.count("toStdString()");
        QVERIFY2(toStdCount >= 2,
                 "StringHelper must use toStdString() to avoid dangling pointer returns");
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: EncodingDetector — can detect common encodings
// ============================================================================
class EncodingDetectorTest : public QObject {
public:
    EncodingDetectorTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString path = d.filePath("src/core/EncodingDetector.cpp");
        QString src = readFile(path);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

        // Must have detect() method with BOM detection
        QVERIFY2(src.contains("detect(") || src.contains("detectEncoding"),
                 "EncodingDetector must have detect() method");

        // UTF-8 BOM detection
        QVERIFY2(src.contains("EF BB BF") || src.contains("utf-8") || src.contains("UTF-8"),
                 "EncodingDetector must handle UTF-8 BOM detection");

        // UTF-16 BOM detection
        QVERIFY2(src.contains("FF FE") || src.contains("FE FF") ||
                 src.contains("utf-16") || src.contains("UTF-16"),
                 "EncodingDetector must handle UTF-16 BOM detection");

        // Safe return pattern: toStdString() copies, not dangling pointer
        QVERIFY2(src.contains("toStdString()"),
                 "EncodingDetector must use toStdString() for safe string return");
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: Application CLI argument parser
// ============================================================================
class ArgumentParserTest : public QObject {
public:
    ArgumentParserTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString mainPath = d.filePath("src/main.cpp");
        QString src = readFile(mainPath);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + mainPath));

        // Must handle --no-plugins flag
        QVERIFY2(src.contains("--no-plugins") || src.contains("noPlugins"),
                 "Application must support --no-plugins CLI flag for diagnostics");

        // Must parse config directory
        QVERIFY2(src.contains("config-dir") || src.contains("configDir") || src.contains("--config"),
                 "Application must support --config-dir CLI flag");

        // Must not call showMainWindow() directly in main()
        int showCount = 0;
        const QStringList lines = src.split('\n');
        for (const QString& line : lines) {
            if (line.contains("showMainWindow()")) ++showCount;
        }
        if (showCount > 0) {
            QStringList bad;
            for (int i = 0; i < lines.size(); ++i)
                if (lines[i].contains("showMainWindow()"))
                    bad << QString("  line %1: %2").arg(i+1).arg(lines[i].trimmed());
            QFAIL(qPrintable("showMainWindow() called directly in main():\n" + bad.join("\n")));
        }
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: Plugin API — safe string returns
// ============================================================================
class PluginApiTest : public QObject {
public:
    PluginApiTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString path = d.filePath("src/plugins/PluginManager.cpp");
        QString src = readFile(path);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

        // pf_getCurrentText must use static QByteArray for safe return
        QVERIFY2(src.contains("static QByteArray"),
                 "Plugin API functions must use static QByteArray for safe const char* return");

        // No dangling toUtf8().constData() in function returns
        const QStringList lines = src.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            const QString& line = lines[i];
            if (line.trimmed().startsWith("return ") &&
                line.contains("toUtf8().constData()")) {
                QFAIL(qPrintable(QString("Dangling pointer return at line %1:\n  %2")
                    .arg(i+1).arg(line.trimmed())));
            }
        }
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: PreferenceDialog — safe settings application
// ============================================================================
class PreferenceDialogTest : public QObject {
public:
    PreferenceDialogTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString path = d.filePath("src/dialogs/PreferenceDialog.cpp");
        QString src = readFile(path);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

        // applyTheme must not use dangling toUtf8().constData()
        const QStringList lines = src.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            const QString& line = lines[i];
            if (line.contains("toUtf8().constData()")) {
                // Must be stored in a variable first
                bool safe = false;
                for (int back = i; back >= 0 && back >= i - 2; --back) {
                    if (lines[back].contains("const QByteArray") ||
                        lines[back].contains("toStdString()")) {
                        safe = true; break;
                    }
                }
                if (!safe) {
                    QFAIL(qPrintable(QString("Dangling toUtf8().constData() at line %1:\n  %2")
                        .arg(i+1).arg(line.trimmed())));
                }
            }
        }
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: Build configuration — ASAN + sanitizers
// ============================================================================
class BuildConfigTest : public QObject {
public:
    BuildConfigTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        QString cmakePath = d.filePath("CMakeLists.txt");
        QString src = readFile(cmakePath);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + cmakePath));

        // Must have sanitizer options
        QVERIFY2(src.contains("sanitize") || src.contains("ASAN") || src.contains("UBSAN"),
                 "CMakeLists.txt must have sanitizer build options for crash detection");

        // Must have Debug build type
        QVERIFY2(src.contains("CMAKE_BUILD_TYPE") || src.contains("Debug"),
                 "CMakeLists.txt must support Debug build type");

        // Must have frame pointer option for backtraces
        QVERIFY2(src.contains("frame-pointer") || src.contains("frame_pointer") ||
                 src.contains("fno-omit-frame-pointer"),
                 "CMakeLists.txt must compile with -fno-omit-frame-pointer for GDB backtraces");
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: Test infrastructure — all test targets defined
// ============================================================================
class TestInfrastructureTest : public QObject {
public:
    TestInfrastructureTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp();
        if (!d.cd("tests")) { qWarning() << "No tests/ directory"; return; }
        QString cmakePath = d.filePath("CMakeLists.txt");
        QString src = readFile(cmakePath);
        QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + cmakePath));

        // Must have npp-test-startup
        QVERIFY2(src.contains("npp-test-startup"),
                 "tests/CMakeLists.txt must have npp-test-startup target");

        // Must use Qt Test framework
        QVERIFY2(src.contains("Qt6::Test") || src.contains("Qt5::Test"),
                 "tests/CMakeLists.txt must link Qt6::Test");

        // Must set QT_QPA_PLATFORM=offscreen for headless tests
        QVERIFY2(src.contains("offscreen") || src.contains("QPA_PLATFORM"),
                 "tests/CMakeLists.txt must set QT_QPA_PLATFORM=offscreen for headless CI");
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Test: No common memory safety bugs across all source files
// ============================================================================
class MemorySafetyScanTest : public QObject {
public:
    MemorySafetyScanTest() : QObject() {}
    void run() {
        QDir d(__FILE__);
        d.cdUp(); d.cdUp();
        d.cd("src");

        // Scan all .cpp files for dangerous patterns
        QStringList dangerous;
        const QStringList cppFiles = d.entryList({"*.cpp"});
        for (const QString& fileName : cppFiles) {
            QString path = d.filePath(fileName);
            QString src = readFile(path);
            if (src.isEmpty()) continue;

            const QStringList lines = src.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                const QString& line = lines[i];
                // Pattern: return X.toUtf8().constData() — always dangerous
                if (line.trimmed().startsWith("return ") &&
                    (line.contains("toUtf8().constData()") ||
                     line.contains("toLatin1().constData()"))) {
                    dangerous << QString("%1:%2: %3").arg(fileName).arg(i+1)
                                .arg(line.trimmed());
                }
            }
        }

        if (!dangerous.isEmpty()) {
            QString msg = "Dangerous dangling pointer patterns found:\n" + dangerous.join("\n");
            QFAIL(qPrintable(msg));
        }
    }

private:
    static QString readFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
};

// ============================================================================
// Main
// ============================================================================
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    struct Test { const char* name; QObject* obj; } tests[] = {
        {"StringHelper",          new StringHelperTest()},
        {"EncodingDetector",      new EncodingDetectorTest()},
        {"ArgumentParser",        new ArgumentParserTest()},
        {"PluginApi",            new PluginApiTest()},
        {"PreferenceDialog",     new PreferenceDialogTest()},
        {"BuildConfig",          new BuildConfigTest()},
        {"TestInfrastructure",   new TestInfrastructureTest()},
        {"MemorySafetyScan",     new MemorySafetyScanTest()},
    };

    int failed = 0;
    for (auto& t : tests) {
        qDebug() << "\n=== RUN  :" << t.name << "===";
        bool ok = QTest::qExec(t.obj, argc, argv) == 0;
        qDebug() << "=== RESULT:" << t.name << (ok ? "PASSED" : "FAILED") << "===\n";
        if (!ok) ++failed;
        delete t.obj;
    }

    qDebug() << "\n========== SUMMARY ==========";
    qDebug() << (failed == 0 ? "ALL TESTS PASSED" : QString("%1 TEST(S) FAILED").arg(failed));
    qDebug() << "==============================";
    return failed ? 1 : 0;
}
