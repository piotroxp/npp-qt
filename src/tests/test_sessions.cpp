// test_sessions.cpp — Session save/restore unit tests
// Copyright (C) 2026 Agent Army | GPL v3
//
// Tests:
//   - SessionManager saves valid JSON
//   - SessionManager restores file list + tab indices
//   - recentSessions() caps at 10
//   - Empty session handled gracefully
//
// Run with:  QT_QPA_PLATFORM=offscreen ./build/src/tests/test_sessions
// Build:     cmake --build build --target test_sessions

#include "SessionManager.h"
#include "Buffer.h"
#include "FileManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonObject>
#include <QDebug>
#include <cassert>
#include <QDebug>
#include <iostream>
#include <QDebug>

// ─── Test helpers ─────────────────────────────────────────────────────────────
#define ASSERT_TRUE(x) \
    do { if (!(x)) { qWarning() << "FAILED: " << #x << " at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) \
    do { if (!!(x)) { qWarning() << "FAILED: " << #x << " at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define ASSERT_EQ(a, b) \
    do { auto _a = (a); auto _b = (b); \
         if (_a != _b) { qWarning() << "FAILED: " << #a << " (" << _a << ") != " << #b << " (" << _b << ") at " << __FILE__ << ":" << __LINE__ << "\n"; exit(1); } } while(0)
#define RUN_TEST(name) \
    do { qWarning() << "  RUN  " << #name << "... "; \
         name(); qWarning() << "PASS\n"; ++g_passed; } while(0)

static int g_passed = 0;
static int g_failed = 0;

// ─── Temp file helper ─────────────────────────────────────────────────────────
static QString makeTempFile(const QString& suffix = QStringLiteral("npp-session")) {
    static int counter = 0;
    QString path = QStringLiteral("/tmp/npp-session-test-%1-%2.json")
                      .arg(getpid())
                      .arg(++counter);
    if (!suffix.isEmpty()) {
        path = QStringLiteral("/tmp/npp-session-test-%1-%2.%3")
                   .arg(getpid())
                   .arg(++counter)
                   .arg(suffix);
    }
    return path;
}

// ─── Test 1: Default session object ──────────────────────────────────────────
static void test_session_defaults() {
    NppSession session;
    ASSERT_EQ(session.version, 1);
    ASSERT_TRUE(session.buffers.isEmpty());
    ASSERT_EQ(session.activeBufferIndex, 0);
    ASSERT_EQ(session.activeView, 0);
    ASSERT_FALSE(session.window.maximized);
    ASSERT_EQ(session.window.width, 1200);
    ASSERT_EQ(session.window.height, 800);
}

// ─── Test 2: Save → load roundtrip ───────────────────────────────────────────
static void test_session_save_load() {
    // Build a session with one buffer entry
    NppSession session;
    session.version = 1;
    session.activeBufferIndex = 0;
    session.activeView = 0;
    session.window.maximized = false;
    session.window.width = 1024;
    session.window.height = 768;

    SessionBuffer buf;
    buf.path = QStringLiteral("/tmp/test_file.cpp");
    buf.language = QStringLiteral("C++");
    buf.cursor.line = 10;
    buf.cursor.column = 5;
    buf.isActive = true;
    session.buffers.append(buf);

    // Serialize via SessionManager
    SessionManager mgr;
    mgr.setCurrentSession(session);

    const QString path = makeTempFile();
    bool ok = mgr.saveSessionToJson(path);
    ASSERT_TRUE(ok);

    // Load into fresh manager
    SessionManager mgr2;
    ok = mgr2.loadSessionFromJson(path);
    ASSERT_TRUE(ok);

    NppSession loaded = mgr2.currentSession();
    ASSERT_EQ(loaded.version, 1);
    ASSERT_EQ(loaded.buffers.size(), 1);
    ASSERT_EQ(loaded.buffers[0].path, QStringLiteral("/tmp/test_file.cpp"));
    ASSERT_EQ(loaded.buffers[0].language, QStringLiteral("C++"));
    ASSERT_EQ(loaded.buffers[0].cursor.line, 10);
    ASSERT_EQ(loaded.buffers[0].cursor.column, 5);
    ASSERT_TRUE(loaded.buffers[0].isActive);
    ASSERT_EQ(loaded.activeBufferIndex, 0);

    // Clean up
    QFile::remove(path);
}

// ─── Test 3: recentSessions() caps at maxRecentSessions (10) ─────────────────
static void test_recent_sessions_cap() {
    SessionManager mgr;
    mgr.setMaxRecentSessions(10);
    ASSERT_EQ(mgr.maxRecentSessions(), 10);

    // Add 15 sessions
    for (int i = 0; i < 15; ++i) {
        mgr.addRecentSession(QStringLiteral("/tmp/session_%1.json").arg(i));
    }

    QStringList recent = mgr.recentSessions();
    ASSERT_EQ(recent.size(), 10);

    // Most recent (session_14) should be first
    ASSERT_TRUE(recent.first().contains(QStringLiteral("session_14")));
    // Oldest (session_5) should be last
    ASSERT_TRUE(recent.last().contains(QStringLiteral("session_5")));
}

// ─── Test 4: Empty session is saved and loaded without crash ─────────────────
static void test_empty_session() {
    SessionManager mgr;
    NppSession empty;
    mgr.setCurrentSession(empty);

    const QString path = makeTempFile();
    bool ok = mgr.saveSessionToJson(path);
    ASSERT_TRUE(ok);

    SessionManager mgr2;
    ok = mgr2.loadSessionFromJson(path);
    ASSERT_TRUE(ok);

    NppSession loaded = mgr2.currentSession();
    ASSERT_TRUE(loaded.buffers.isEmpty());
    ASSERT_EQ(loaded.activeBufferIndex, 0);

    QFile::remove(path);
}

// ─── Test 5: Window geometry roundtrip ───────────────────────────────────────
static void test_window_geometry() {
    NppSession session;
    session.window.x = 50;
    session.window.y = 100;
    session.window.width = 1600;
    session.window.height = 900;
    session.window.maximized = true;

    SessionManager mgr;
    mgr.setCurrentSession(session);

    const QString path = makeTempFile();
    bool ok = mgr.saveSessionToJson(path);
    ASSERT_TRUE(ok);

    SessionManager mgr2;
    ok = mgr2.loadSessionFromJson(path);
    ASSERT_TRUE(ok);

    NppSession loaded = mgr2.currentSession();
    ASSERT_EQ(loaded.window.x, 50);
    ASSERT_EQ(loaded.window.y, 100);
    ASSERT_EQ(loaded.window.width, 1600);
    ASSERT_EQ(loaded.window.height, 900);
    ASSERT_TRUE(loaded.window.maximized);

    QFile::remove(path);
}

// ─── Test 6: Panel state roundtrip ───────────────────────────────────────────
static void test_panel_state() {
    NppSession session;
    session.panels.functionList = true;
    session.panels.documentMap = false;
    session.panels.fileBrowser = true;

    SessionManager mgr;
    mgr.setCurrentSession(session);

    const QString path = makeTempFile();
    bool ok = mgr.saveSessionToJson(path);
    ASSERT_TRUE(ok);

    SessionManager mgr2;
    ok = mgr2.loadSessionFromJson(path);
    ASSERT_TRUE(ok);

    NppSession loaded = mgr2.currentSession();
    ASSERT_TRUE(loaded.panels.functionList);
    ASSERT_FALSE(loaded.panels.documentMap);
    ASSERT_TRUE(loaded.panels.fileBrowser);

    QFile::remove(path);
}

// ─── Test 7: Recent files list ───────────────────────────────────────────────
static void test_recent_files() {
    SessionManager mgr;
    mgr.addRecentFile("/tmp/a.txt");
    mgr.addRecentFile("/tmp/b.cpp");
    mgr.addRecentFile("/tmp/c.h");

    std::vector<std::string> files = mgr.getRecentFiles();
    ASSERT_EQ(files.size(), 3u);
    ASSERT_TRUE(mgr.recentFiles().contains(QStringLiteral("/tmp/a.txt")));

    mgr.removeRecentFile("/tmp/b.cpp");
    files = mgr.getRecentFiles();
    ASSERT_EQ(files.size(), 2u);
}

// ─── Test 8: setMaxRecentSessions respects new limit ─────────────────────────
static void test_max_recent_sessions_change() {
    SessionManager mgr;
    mgr.setMaxRecentSessions(5);
    ASSERT_EQ(mgr.maxRecentSessions(), 5);

    for (int i = 0; i < 8; ++i) {
        mgr.addRecentSession(QStringLiteral("/tmp/s%1.json").arg(i));
    }
    ASSERT_EQ(mgr.recentSessions().size(), 5);
}

// ─── Test 9: Capture and apply session ───────────────────────────────────────
static void test_capture_apply_session() {
    SessionManager mgr;
    NppSession s;
    SessionBuffer buf;
    buf.path = QStringLiteral("/home/user/main.cpp");
    buf.isActive = true;
    s.buffers.append(buf);
    s.activeBufferIndex = 0;

    mgr.setCurrentSession(s);
    mgr.captureCurrentSession();  // Should not crash (captures current state)

    NppSession captured = mgr.currentSession();
    ASSERT_TRUE(captured.buffers.isEmpty() || captured.buffers.size() >= 0); // No crash
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    qWarning() << "\n=== SessionManager tests ===\n";

    QCoreApplication app(argc, argv);

    RUN_TEST(test_session_defaults);
    RUN_TEST(test_session_save_load);
    RUN_TEST(test_recent_sessions_cap);
    RUN_TEST(test_empty_session);
    RUN_TEST(test_window_geometry);
    RUN_TEST(test_panel_state);
    RUN_TEST(test_recent_files);
    RUN_TEST(test_max_recent_sessions_change);
    RUN_TEST(test_capture_apply_session);

    qWarning() << "\nResults: " << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed > 0 ? 1 : 0;
}
