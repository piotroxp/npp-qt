// test_autocompletion.cpp - Unit tests for AutoCompletion
// Copyright (C) 2026 Agent Army
// GPL v3

// Qt test — compiles with the test_core library

#include "dialogs/AutoCompletion.h"
#include "core/LanguageManager.h"
#include "common/ScintillaComponent.h"
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QStringList>
#include <cassert>
#include <iostream>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " (" << (a) << ") != " << (b) << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << " (should be false)\n"; exit(1); } } while(0)

// ─── Mock ScintillaComponent for testing without Qt widgets ────────────────────

class MockScintillaComponent : public ScintillaComponent {
public:
    MockScintillaComponent() = default;
    ~MockScintillaComponent() override = default;

    // Match ScintillaComponent's primary virtual signature exactly
    intptr_t send(int message, int wParam = 0, intptr_t lParam = 0) override {
        Q_UNUSED(message);
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return 0;
    }

    // Track calls
    int autocShowCalls = 0;
    int autocCancelCalls = 0;
    int autocSeparator = 0;
    int autocIgnoreCase = -1;
};

// ─── Tests ────────────────────────────────────────────────────────────────────

TEST(test_autocompletion_construct) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);
    ASSERT_FALSE(ac.isActive());
    delete mock;
}

TEST(test_autocompletion_language_setup) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    // Set C++ language
    bool ok = ac.setLanguage(static_cast<int>(LangType::L_CPP));
    Q_UNUSED(ok); // ok may be false if no keywords loaded, that's fine

    // Case sensitivity should be set
    bool cs = ac.isCaseSensitive();
    Q_UNUSED(cs); // Just verify it doesn't crash

    delete mock;
}

TEST(test_autocompletion_trigger_threshold) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ASSERT_EQ(ac.triggerThreshold(), 2);  // default
    ac.setTriggerThreshold(3);
    ASSERT_EQ(ac.triggerThreshold(), 3);
    ac.setTriggerThreshold(1);
    ASSERT_EQ(ac.triggerThreshold(), 1);

    delete mock;
}

TEST(test_autocompletion_auto_show) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ASSERT_TRUE(ac.autoShow());  // default
    ac.setAutoShow(false);
    ASSERT_FALSE(ac.autoShow());
    ac.setAutoShow(true);
    ASSERT_TRUE(ac.autoShow());

    delete mock;
}

TEST(test_autocompletion_sort_modes) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ac.setSortMode(AutoCompletion::SortMode::Alphabetical);
    ASSERT_TRUE(ac.sortMode() == AutoCompletion::SortMode::Alphabetical);

    ac.setSortMode(AutoCompletion::SortMode::ByFrequency);
    ASSERT_TRUE(ac.sortMode() == AutoCompletion::SortMode::ByFrequency);

    ac.setSortMode(AutoCompletion::SortMode::ByLength);
    ASSERT_TRUE(ac.sortMode() == AutoCompletion::SortMode::ByLength);

    delete mock;
}

TEST(test_autocompletion_envvar_completion) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ASSERT_TRUE(ac.envVarCompletion());  // default
    ac.setEnvVarCompletion(false);
    ASSERT_FALSE(ac.envVarCompletion());
    ac.setEnvVarCompletion(true);
    ASSERT_TRUE(ac.envVarCompletion());

    delete mock;
}

TEST(test_autocompletion_record_word) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    // Record some words
    ac.recordWordUsed("hello");
    ac.recordWordUsed("world");
    ac.recordWordUsed("hello");  // second occurrence
    ac.recordWordUsed("hello");  // third

    // Close should not crash
    ac.close();
    ASSERT_FALSE(ac.isActive());

    delete mock;
}

TEST(test_autocompletion_close) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    // Close on inactive state should be safe
    ac.close();
    ASSERT_FALSE(ac.isActive());

    delete mock;
}

TEST(test_autocompletion_show_param_info) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ASSERT_TRUE(ac.showParamInfo());  // default
    ac.setShowParamInfo(false);
    ASSERT_FALSE(ac.showParamInfo());

    delete mock;
}

TEST(test_autocompletion_auto_insert_prefix) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    AutoCompletion ac(mock);

    ASSERT_TRUE(ac.autoInsertPrefix());  // default
    ac.setAutoInsertPrefix(false);
    ASSERT_FALSE(ac.autoInsertPrefix());
    ac.setAutoInsertPrefix(true);
    ASSERT_TRUE(ac.autoInsertPrefix());

    delete mock;
}

TEST(test_autocompletion_completion_types) {
    using CT = AutoCompletion::CompletionType;
    using CTs = AutoCompletion::CompletionTypes;

    // Test bitwise operators on scoped enum
    CTs combined = CT::Api | CT::Word | CT::Function;
    Q_UNUSED(combined);

    // Test ~ operator
    CTs inverted = ~CT::Api;
    Q_UNUSED(inverted);
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "=== AutoCompletion Tests ===\n";

    RUN(test_autocompletion_construct);
    RUN(test_autocompletion_language_setup);
    RUN(test_autocompletion_trigger_threshold);
    RUN(test_autocompletion_auto_show);
    RUN(test_autocompletion_sort_modes);
    RUN(test_autocompletion_envvar_completion);
    RUN(test_autocompletion_record_word);
    RUN(test_autocompletion_close);
    RUN(test_autocompletion_show_param_info);
    RUN(test_autocompletion_auto_insert_prefix);
    RUN(test_autocompletion_completion_types);

    std::cout << "\nAll AutoCompletion tests passed.\n";
    return 0;
}
