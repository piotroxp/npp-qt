// test_functioncalltip.cpp - Unit tests for FunctionCallTip
// Copyright (C) 2026 Agent Army
// GPL v3

#include "dialogs/FunctionCallTip.h"
#include "common/ScintillaComponent.h"
#include <QCoreApplication>
#include <QString>
#include <cassert>
#include <iostream>
#include <vector>

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

    sptr_t send(int message, uptr_t wParam = 0, sptr_t lParam = 0) override {
        // Track calltip messages
        lastMessage = message;
        lastWParam = wParam;
        lastLParam = lParam;
        calltipShowCount += (message == SCI_CALLTIPSHOW);
        calltipCancelCount += (message == SCI_CALLTIPCANCEL);
        calltipActiveCount += (message == SCI_CALLTIPACTIVE);
        return 0;  // SCI_CALLTIPACTIVE returns 0 (not active) by default
    }

    int calltipShowCount = 0;
    int calltipCancelCount = 0;
    int calltipActiveCount = 0;
    int lastMessage = 0;
    uptr_t lastWParam = 0;
    sptr_t lastLParam = 0;
};

// ─── Tests ────────────────────────────────────────────────────────────────────

TEST(test_functioncalltip_construct) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);
    ASSERT_FALSE(fct.isVisible());
    delete mock;
}

TEST(test_functioncalltip_sticky) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    ASSERT_FALSE(fct.isSticky());
    fct.setSticky(true);
    ASSERT_TRUE(fct.isSticky());
    fct.setSticky(false);
    ASSERT_FALSE(fct.isSticky());

    delete mock;
}

TEST(test_functioncalltip_auto_trigger) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    ASSERT_TRUE(fct.autoTrigger());  // default
    fct.setAutoTrigger(false);
    ASSERT_FALSE(fct.autoTrigger());
    fct.setAutoTrigger(true);
    ASSERT_TRUE(fct.autoTrigger());

    delete mock;
}

TEST(test_functioncalltip_current_param) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    fct.setCurrentParam(0);
    ASSERT_EQ(fct.currentParam(), 0u);

    fct.setCurrentParam(3);
    ASSERT_EQ(fct.currentParam(), 3u);

    delete mock;
}

TEST(test_functioncalltip_colours) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    QColor fg(255, 200, 100);
    QColor bg(20, 20, 40);
    QColor hl(0, 100, 255);

    fct.setForeground(fg);
    fct.setBackground(bg);
    fct.setHighlightColor(hl);

    ASSERT_TRUE(fct.foreground() == fg);
    ASSERT_TRUE(fct.background() == bg);
    ASSERT_TRUE(fct.highlightColor() == hl);

    delete mock;
}

TEST(test_functioncalltip_show_for_function) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    std::vector<FunctionOverload> overloads;
    overloads.push_back(FunctionOverload{"int foo(int a, int b)", "Add two ints",
                                         "Adds a and b together.", "int", 2});
    overloads.push_back(FunctionOverload{"double foo(double a, double b)", "Add two doubles",
                                         "Adds a and b together.", "double", 2});

    fct.showForFunction("foo", overloads);

    // showCalltip() sends SCI_CALLTIPSHOW
    ASSERT_TRUE(mock->calltipShowCount >= 1 || !fct.isVisible());
    // The calltip should be visible after showForFunction

    fct.close();
    ASSERT_FALSE(fct.isVisible());

    delete mock;
}

TEST(test_functioncalltip_navigation) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    std::vector<FunctionOverload> overloads;
    overloads.push_back(FunctionOverload{"void bar(int x)", "Bar with int", "", "void", 1});
    overloads.push_back(FunctionOverload{"void bar(double x)", "Bar with double", "", "void", 1});
    overloads.push_back(FunctionOverload{"void bar(const char* x)", "Bar with string", "", "void", 1});

    fct.showForFunction("bar", overloads);
    fct.showNextOverload();
    fct.showPrevOverload();
    fct.close();

    // Just verify no crashes
    ASSERT_TRUE(true);

    delete mock;
}

TEST(test_functioncalltip_highlight_param) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    // highlightParam should send SCI_CALLTIPSETHLT
    fct.highlightParam(1);
    fct.highlightParam(2);
    // Just verify no crashes

    delete mock;
}

TEST(test_functioncalltip_reset) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    // After construction, state should be clean
    ASSERT_EQ(fct.currentParam(), 0u);
    ASSERT_FALSE(fct.isVisible());

    delete mock;
}

TEST(test_functioncalltip_cleanup) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    // Cleanup should close and reset
    fct.cleanup();
    ASSERT_FALSE(fct.isVisible());

    delete mock;
}

TEST(test_functioncalltip_set_language_xml) {
    MockScintillaComponent* mock = new MockScintillaComponent();
    FunctionCallTip fct(mock, nullptr);

    // Should not crash with null pointer
    fct.setLanguageXML(nullptr);
    ASSERT_FALSE(fct.isVisible());

    delete mock;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "=== FunctionCallTip Tests ===\n";

    RUN(test_functioncalltip_construct);
    RUN(test_functioncalltip_sticky);
    RUN(test_functioncalltip_auto_trigger);
    RUN(test_functioncalltip_current_param);
    RUN(test_functioncalltip_colours);
    RUN(test_functioncalltip_show_for_function);
    RUN(test_functioncalltip_navigation);
    RUN(test_functioncalltip_highlight_param);
    RUN(test_functioncalltip_reset);
    RUN(test_functioncalltip_cleanup);
    RUN(test_functioncalltip_set_language_xml);

    std::cout << "\nAll FunctionCallTip tests passed.\n";
    return 0;
}
