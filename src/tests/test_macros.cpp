// test_macros.cpp — Macro recording and playback tests

#include "core/MacroManager.h"
#include <QCoreApplication>
#include <QDebug>

#define ASSERT_TRUE(x) do { if (!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_FALSE(x) do { if (!!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_EQ(a, b) do { auto _a = (a); auto _b = (b); if (_a != _b) { qFatal("FAILED: %s != %s at %s:%d", #a, #b, __FILE__, __LINE__); } } while(0)
#define RUN_TEST(name) do { qDebug() << "  RUN " << #name << "..."; name(); qDebug() << "  PASS"; ++g_passed; } while(0)

static int g_passed = 0;

static void test_record_start_stop() {
    MacroManager& mgr = MacroManager::instance();
    bool wasRecording = mgr.isRecording();
    Q_UNUSED(wasRecording);  // might already be recording
    mgr.startRecording();
    ASSERT_TRUE(mgr.isRecording());
    mgr.stopRecording();
    ASSERT_FALSE(mgr.isRecording());
}

static void test_recorded_commands() {
    MacroManager& mgr = MacroManager::instance();
    size_t before = mgr.macroCount();
    mgr.startRecording();
    mgr.recordCommand("edit.cut");
    mgr.recordCommand("edit.copy");
    mgr.stopRecording();
    mgr.saveMacro(QStringLiteral("TestMacro"));
    ASSERT_EQ(mgr.macroCount(), before + 1);
    ASSERT_TRUE(mgr.macroNames().contains(QStringLiteral("TestMacro")));
}

static void test_delete_macro() {
    MacroManager& mgr = MacroManager::instance();
    size_t before = mgr.macroCount();
    if (before > 0) {
        mgr.deleteMacro(0);
        ASSERT_EQ(mgr.macroCount(), before - 1);
    }
}

static void test_signals() {
    MacroManager& mgr = MacroManager::instance();
    // Smoke test: isRecording() callable
    bool r = mgr.isRecording();
    Q_UNUSED(r);
}

static void test_macro_names() {
    MacroManager& mgr = MacroManager::instance();
    QStringList names = mgr.macroNames();
    Q_UNUSED(names);
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    qDebug() << "\n=== MacroManager tests ===";
    RUN_TEST(test_record_start_stop);
    RUN_TEST(test_recorded_commands);
    RUN_TEST(test_delete_macro);
    RUN_TEST(test_signals);
    RUN_TEST(test_macro_names);
    qDebug() << "\nResults:" << g_passed << "passed";
    return 0;
}
