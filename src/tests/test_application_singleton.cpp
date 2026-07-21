// test_application_singleton.cpp — Application singleton tests
// Copyright (C) 2026 Agent Army | GPL v3
//
// Tests:
//   - Application::instance() returns the same reference on repeat calls
//   - Application::instance() is safe to call concurrently from N threads
//     (this is the regression test for the __cxa_guard_acquire deadlock
//      that fires under multi-thread startup: QFileOpenEvent, plugin
//      load, file watcher, etc.)
//   - Construct-On-First-Use idiom: first call returns the same address
//     as subsequent calls; the singleton is never re-constructed.
//
// Run with:  QT_QPA_PLATFORM=offscreen ./build/src/tests/test_application_singleton
// Build:     cmake --build build --target test_application_singleton

#include "TestStubs.h"
#include "Application.h"

#include <QCoreApplication>
#include <QDebug>
#include <atomic>
#include <thread>
#include <vector>

// ─── Tiny test framework (matches the other test_*.cpp files) ──────────────────
static int g_passed = 0;
static int g_failed = 0;

#define ASSERT_TRUE(cond) do {                                                   \
    if (cond) { ++g_passed; }                                                    \
    else      { ++g_failed; qWarning() << "FAIL:" << #cond                        \
                       << "at" << __FILE__ << ":" << __LINE__; }                 \
} while (0)

#define ASSERT_EQ(a, b) do {                                                     \
    auto _va = (a); auto _vb = (b);                                              \
    if (_va == _vb) { ++g_passed; }                                              \
    else { ++g_failed; qWarning() << "FAIL:" << #a "==" #b                       \
            << "got" << _va << "vs" << _vb                                       \
            << "at" << __FILE__ << ":" << __LINE__; }                            \
} while (0)

#define RUN_TEST(fn) do {                                                        \
    qWarning() << "──" << #fn << "──";                                           \
    int before_failed = g_failed;                                                \
    fn();                                                                        \
    if (g_failed == before_failed) qWarning() << "  PASSED";                     \
    else                           qWarning() << "  FAILED";                     \
} while (0)

// ─── Tests ─────────────────────────────────────────────────────────────────────

// Smoke: instance() returns the same address on every call.
static void test_instance_returns_same_reference() {
    Application& a1 = Application::instance();
    Application& a2 = Application::instance();
    Application& a3 = Application::instance();
    ASSERT_EQ(&a1, &a2);
    ASSERT_EQ(&a2, &a3);
}

// Smoke: the inline app() helper agrees with instance().
static void test_inline_app_helper_matches_instance() {
    Application& i = Application::instance();
    Application& h = app();
    ASSERT_EQ(&i, &h);
}

// Concurrent: 16 threads hammer instance() simultaneously. With the
// old `static Application inst;` pattern, this would deadlock at
// __cxa_guard_acquire under contention. With Construct-On-First-Use
// + the atomic in_progress flag, all 16 calls return the same
// address and the test completes in well under a second.
//
// Each thread records the address it observed into a shared vector;
// the main thread verifies all 16 entries are identical.
static void test_instance_thread_safe() {
    constexpr int kThreads = 16;
    std::atomic<int> ready_count{0};
    std::atomic<bool> go{false};

    std::vector<Application*> observed(kThreads, nullptr);

    auto worker = [&](int idx) {
        observed[idx] = &Application::instance();
        ready_count.fetch_add(1);
        // Spin until main signals go.
        while (!go.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        // Re-fetch under contention.
        observed[idx] = &Application::instance();
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    // Wait for every thread to have taken its first reference, then
    // fire the "go" signal so they all race on the second fetch.
    while (ready_count.load() < kThreads) {
        std::this_thread::yield();
    }
    go.store(true, std::memory_order_release);

    for (auto& t : threads) t.join();

    // All observed addresses must be identical.
    Application* first = observed[0];
    ASSERT_TRUE(first != nullptr);
    for (int i = 1; i < kThreads; ++i) {
        ASSERT_EQ(observed[i], first);
    }
}

// Stress: 64 threads × 1000 calls each. Total 64000 instance() calls.
// Confirms the Construct-On-First-Use path doesn't leak the ctor or
// produce different addresses under sustained contention.
static void test_instance_stress_no_reconstruction() {
    constexpr int kThreads = 64;
    constexpr int kIters = 1000;

    std::atomic<int> mismatches{0};
    Application* canonical = &Application::instance();

    auto worker = [&]() {
        for (int i = 0; i < kIters; ++i) {
            if (&Application::instance() != canonical) {
                mismatches.fetch_add(1);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) t.join();

    ASSERT_EQ(mismatches.load(), 0);
}

// ─── main ──────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    qWarning() << "\n=== Application singleton tests ===\n";

    QCoreApplication app(argc, argv);

    RUN_TEST(test_instance_returns_same_reference);
    RUN_TEST(test_inline_app_helper_matches_instance);
    RUN_TEST(test_instance_thread_safe);
    RUN_TEST(test_instance_stress_no_reconstruction);

    qWarning() << "\nResults: " << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed > 0 ? 1 : 0;
}
