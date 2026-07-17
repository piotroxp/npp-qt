// test_incremental_search.cpp - Unit tests for IncrementalSearchDialog
// Copyright (C) 2026 Agent Army
// GPL v3

#include "dialogs/IncrementalSearchDialog.h"
#include "editor/ScintillaEditor.h"
#include <QCoreApplication>
#include <QString>
#include <QWidget>
#include <cassert>
#include <iostream>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << " (should be false)\n"; exit(1); } } while(0)

// ─── Tests ────────────────────────────────────────────────────────────────────

TEST(test_incremental_search_construct) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);
    ASSERT_FALSE(dlg->isVisible());
    delete dlg;
}

TEST(test_incremental_search_show_at_top) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);
    // showAtTop positions the dialog relative to its parent
    dlg->showAtTop();
    ASSERT_TRUE(dlg->isVisible());
    delete dlg;
}

TEST(test_incremental_search_set_search_text) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);
    // setSearchText should not crash even without an editor
    dlg->setSearchText("test");
    ASSERT_FALSE(dlg->isVisible());  // still hidden
    delete dlg;
}

TEST(test_incremental_search_signal_emission) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);
    bool closeCalled = false;
    QObject::connect(dlg, &IncrementalSearchDialog::closeRequested, [&closeCalled]() {
        closeCalled = true;
    });

    // Simulate hiding
    dlg->hide();
    // closeRequested is only emitted by the event filter on Escape key,
    // not by hide(). Verify the dialog can be hidden without crash.
    ASSERT_FALSE(dlg->isVisible());

    delete dlg;
}

TEST(test_incremental_search_set_editor_null) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);
    // Setting null editor should not crash
    dlg->setEditor(nullptr);
    // Setting search text with null editor should not crash
    dlg->setSearchText("hello");
    delete dlg;
}

TEST(test_incremental_search_multiple_show_hide) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);

    dlg->show();
    ASSERT_TRUE(dlg->isVisible());

    dlg->hide();
    ASSERT_FALSE(dlg->isVisible());

    dlg->show();
    ASSERT_TRUE(dlg->isVisible());

    dlg->hide();
    ASSERT_FALSE(dlg->isVisible());

    delete dlg;
}

TEST(test_incremental_search_default_state) {
    IncrementalSearchDialog* dlg = new IncrementalSearchDialog(nullptr);

    // Dialog should be non-modal (tool window)
    ASSERT_FALSE(dlg->isModal());

    // Should have the frameless window hint
    ASSERT_TRUE(dlg->windowFlags().testFlag(Qt::Tool));
    ASSERT_TRUE(dlg->windowFlags().testFlag(Qt::FramelessWindowHint));

    delete dlg;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "=== IncrementalSearchDialog Tests ===\n";

    RUN(test_incremental_search_construct);
    RUN(test_incremental_search_show_at_top);
    RUN(test_incremental_search_set_search_text);
    RUN(test_incremental_search_signal_emission);
    RUN(test_incremental_search_set_editor_null);
    RUN(test_incremental_search_multiple_show_hide);
    RUN(test_incremental_search_default_state);

    std::cout << "\nAll IncrementalSearchDialog tests passed.\n";
    return 0;
}
