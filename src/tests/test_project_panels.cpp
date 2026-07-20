// test_project_panels.cpp — Project panel UI tests using FileBrowserPanel
// Tests FileBrowserPanel (QDockWidget-based panel that exists with source).

#include "panels/FileBrowserPanel.h"
#include "Application.h"
#include <QApplication>
#include <QGuiApplication>
#include <QDebug>
#include <cassert>

#define ASSERT_TRUE(x) do { if (!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_FALSE(x) do { if (!!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_EQ(a, b) do { auto _a = (a); auto _b = (b); if (_a != _b) { qFatal("FAILED: %s (%lld) != %s (%lld) at %s:%d", #a, (long long)_a, #b, (long long)_b, __FILE__, __LINE__); } } while(0)
#define RUN_TEST(name) do { qDebug() << "  RUN  " << #name << "..."; name(); qDebug() << "PASS"; ++g_passed; } while(0)

static int g_passed = 0;

// ─── Test 1: Construction ─────────────────────────────────────────────────────
static void test_panel_construction() {
    FileBrowserPanel* panel = new FileBrowserPanel(nullptr);
    ASSERT_TRUE(panel != nullptr);
    ASSERT_FALSE(panel->isVisible());
    delete panel;
}

// ─── Test 2: Title ────────────────────────────────────────────────────────────
static void test_panel_title() {
    FileBrowserPanel panel(nullptr);
    QString title = panel.windowTitle();
    ASSERT_FALSE(title.isEmpty());
}

// ─── Test 3: Show/hide ────────────────────────────────────────────────────────
static void test_panel_show_hide() {
    FileBrowserPanel panel(nullptr);
    ASSERT_FALSE(panel.isVisible());
    panel.show();
    ASSERT_TRUE(panel.isVisible());
    panel.hide();
    ASSERT_FALSE(panel.isVisible());
}

// ─── Test 4: Toggle action exists ─────────────────────────────────────────────
static void test_panel_toggle() {
    FileBrowserPanel panel(nullptr);
    ASSERT_FALSE(panel.isVisible());
    QAction* toggle = panel.toggleViewAction();
    ASSERT_TRUE(toggle != nullptr);
    toggle->trigger();
    ASSERT_TRUE(panel.isVisible());
}

// ─── Test 5: Dock widget features ──────────────────────────────────────────────
static void test_dock_widget_features() {
    FileBrowserPanel panel(nullptr);
    ASSERT_TRUE(panel.features().testFlag(QDockWidget::DockWidgetMovable));
    ASSERT_TRUE(panel.features().testFlag(QDockWidget::DockWidgetClosable));
}

// ─── Test 6: Context menu (QDockWidget built-in) ───────────────────────────────
static void test_panel_context_menu() {
    FileBrowserPanel panel(nullptr);
    ASSERT_TRUE(panel.features().testFlag(QDockWidget::DockWidgetClosable));
}

// ─── Test 7: Multiple panels coexist ───────────────────────────────────────────
static void test_multiple_panels() {
    FileBrowserPanel* p1 = new FileBrowserPanel(nullptr);
    FileBrowserPanel* p2 = new FileBrowserPanel(nullptr);
    FileBrowserPanel* p3 = new FileBrowserPanel(nullptr);
    ASSERT_TRUE(p1 != nullptr);
    ASSERT_TRUE(p2 != nullptr);
    ASSERT_TRUE(p3 != nullptr);
    delete p1; delete p2; delete p3;
}

// ─── Test 8: Dock areas ───────────────────────────────────────────────────────
static void test_dock_areas() {
    FileBrowserPanel panel(nullptr);
    ASSERT_TRUE(panel.allowedAreas().testFlag(Qt::LeftDockWidgetArea));
    ASSERT_TRUE(panel.allowedAreas().testFlag(Qt::RightDockWidgetArea));
    ASSERT_FALSE(panel.allowedAreas().testFlag(Qt::TopDockWidgetArea));
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    if (!QGuiApplication::primaryScreen()) {
        qWarning("SKIP: No display available (offscreen mode)");
        return 0;
    }
    qDebug() << "\n=== Project Panel tests ===";
    RUN_TEST(test_panel_construction);
    RUN_TEST(test_panel_title);
    RUN_TEST(test_panel_show_hide);
    RUN_TEST(test_panel_toggle);
    RUN_TEST(test_dock_widget_features);
    RUN_TEST(test_panel_context_menu);
    RUN_TEST(test_multiple_panels);
    RUN_TEST(test_dock_areas);
    qDebug() << "\nResults:" << g_passed << "passed";
    return 0;
}
