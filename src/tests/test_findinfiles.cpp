// test_findinfiles.cpp — FindInFilesWorker tests

#include "FindInFilesWorker.h"
#include "Types.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <cassert>

#define ASSERT_TRUE(x) do { if (!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_FALSE(x) do { if (!!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_EQ(a, b) do { auto _a = (a); auto _b = (b); if (_a != _b) { qFatal("FAILED: %s != %s at %s:%d", #a, #b, __FILE__, __LINE__); } } while(0)
#define RUN_TEST(name) do { qDebug() << "  RUN " << #name << "..."; name(); qDebug() << "  PASS"; ++g_passed; } while(0)

static int g_passed = 0;
static int counter = 0;

static QString makeTempDir() {
    QString dir = QStringLiteral("/tmp/fifindtest-%1-%2").arg(getpid()).arg(++counter);
    QDir().mkdir(dir);
    return dir;
}

static void makeFile(const QString& dir, const QString& name, const QString& content) {
    QFile f(dir + QStringLiteral("/") + name);
    f.open(QIODevice::WriteOnly);
    f.write(content.toUtf8());
    f.close();
}

static void test_worker_construct() {
    QString dir = makeTempDir();
    FindInFilesWorker* w = new FindInFilesWorker(dir, QStringLiteral("hello"), QStringLiteral("*"), true, false, false);
    ASSERT_TRUE(w != nullptr);
    delete w;
    QDir(dir).removeRecursively();
}

static void test_worker_finds_match() {
    QString dir = makeTempDir();
    makeFile(dir, QStringLiteral("sample.cpp"), QStringLiteral("int main() { return 0; }\n"));
    FindInFilesWorker worker(dir, QStringLiteral("return"), QStringLiteral("*"), true, false, false);
    QList<FindResult> results;
    QObject::connect(&worker, &FindInFilesWorker::resultsReady,
                     [&results](const QList<FindResult>& r) { results.append(r); });
    worker.run();
    QCoreApplication::processEvents();
    ASSERT_EQ(results.size(), 1);
    ASSERT_TRUE(results[0].filePath.contains(QStringLiteral("sample.cpp")));
    QDir(dir).removeRecursively();
}

static void test_worker_filter_cpp_vs_h() {
    QString dir = makeTempDir();
    makeFile(dir, QStringLiteral("test.cpp"), QStringLiteral("hello world"));
    makeFile(dir, QStringLiteral("test.h"), QStringLiteral("hello world"));
    FindInFilesWorker w_cpp(dir, QStringLiteral("hello"), QStringLiteral("*.cpp"), true, false, false);
    QList<FindResult> cpp_results;
    QObject::connect(&w_cpp, &FindInFilesWorker::resultsReady,
                     [&cpp_results](const QList<FindResult>& r) { cpp_results.append(r); });
    w_cpp.run();
    ASSERT_EQ(cpp_results.size(), 1);
    ASSERT_TRUE(cpp_results[0].filePath.endsWith(QStringLiteral(".cpp")));
    QDir(dir).removeRecursively();
}

static void test_worker_empty_dir() {
    QString dir = makeTempDir();
    FindInFilesWorker worker(dir, QStringLiteral("hello"), QStringLiteral("*"), true, false, false);
    QList<FindResult> results;
    QObject::connect(&worker, &FindInFilesWorker::resultsReady,
                     [&results](const QList<FindResult>& r) { results.append(r); });
    worker.run();
    ASSERT_EQ(results.size(), 0);
    QDir(dir).removeRecursively();
}

static void test_worker_case_sensitive() {
    QString dir = makeTempDir();
    makeFile(dir, QStringLiteral("test.cpp"), QStringLiteral("Hello World\nhello world"));
    FindInFilesWorker cs_worker(dir, QStringLiteral("Hello"), QStringLiteral("*"), true, false, false);
    FindInFilesWorker ci_worker(dir, QStringLiteral("Hello"), QStringLiteral("*"), false, false, false);
    QList<FindResult> cs_results, ci_results;
    QObject::connect(&cs_worker, &FindInFilesWorker::resultsReady,
                     [&cs_results](const QList<FindResult>& r) { cs_results.append(r); });
    QObject::connect(&ci_worker, &FindInFilesWorker::resultsReady,
                     [&ci_results](const QList<FindResult>& r) { ci_results.append(r); });
    cs_worker.run();
    ci_worker.run();
    ASSERT_EQ(cs_results.size(), 1); // only "Hello"
    ASSERT_EQ(ci_results.size(), 2); // both
    QDir(dir).removeRecursively();
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    qDebug() << "\n=== FindInFilesWorker tests ===";
    RUN_TEST(test_worker_construct);
    RUN_TEST(test_worker_finds_match);
    RUN_TEST(test_worker_filter_cpp_vs_h);
    RUN_TEST(test_worker_empty_dir);
    RUN_TEST(test_worker_case_sensitive);
    qDebug() << "\nResults:" << g_passed << "passed";
    return 0;
}
