// test_document_map.cpp — DocumentMapPanel construction and basic API tests

#include "panels/DocumentMapPanel.h"
#include <QCoreApplication>
#include <QDebug>

#define ASSERT_TRUE(x) do { if (!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define ASSERT_FALSE(x) do { if (!!(x)) { qFatal("FAILED: %s at %s:%d", #x, __FILE__, __LINE__); } } while(0)
#define RUN_TEST(name) do { qDebug() << "  RUN " << #name << "..."; name(); qDebug() << "  PASS"; ++g_passed; } while(0)

static int g_passed = 0;

static void test_construct() {
    DocumentMapPanel* panel = new DocumentMapPanel(nullptr);
    ASSERT_TRUE(panel != nullptr);
    ASSERT_FALSE(panel->isVisible());
    delete panel;
}

static void test_toggle_action() {
    DocumentMapPanel panel;
    QAction* act = panel.toggleViewAction();
    ASSERT_TRUE(act != nullptr);
}

static void test_dock_features() {
    DocumentMapPanel panel;
    panel.setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    ASSERT_TRUE(panel.features().testFlag(QDockWidget::DockWidgetMovable));
    ASSERT_TRUE(panel.features().testFlag(QDockWidget::DockWidgetClosable));
}

static void test_resize() {
    DocumentMapPanel panel;
    panel.resize(800, 600);  // public API
    ASSERT_TRUE(panel.width() > 0);
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    qDebug() << "\n=== DocumentMapPanel tests ===";
    RUN_TEST(test_construct);
    RUN_TEST(test_toggle_action);
    RUN_TEST(test_dock_features);
    RUN_TEST(test_resize);
    qDebug() << "\nResults:" << g_passed << "passed";
    return 0;
}
