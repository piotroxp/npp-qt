// =============================================================================
// TestPluginsManager.cpp — Qt Test suite for PluginsManager API
// Tests: allocation, command registration, plugin info management,
// ID range checks, menu initialization, and notification relay.
// =============================================================================

#include <QtTest/QtTest>
#include "MISC/PluginsManager/PluginsManager.h"
#include "MISC/PluginsManager/PluginInterface.h"

class TestPluginsManager : public QObject
{
    Q_OBJECT

private slots:
    void init() {
        m_pm = new PluginsManager();
        NppData nd;
        nd._nppHandle = nullptr;
        nd._scintillaMainHandle = nullptr;
        nd._scintillaSecondHandle = nullptr;
        m_pm->init(nd);
    }

    void cleanup() {
        delete m_pm;
    }

    // --- ID Allocation ---
    void test_allocateCmdID_basic() {
        int start = -1;
        bool ok = m_pm->allocateCmdID(5, &start);
        QVERIFY(ok);
        QVERIFY(start >= ID_PLUGINS_CMD_DYNAMIC);
        QVERIFY(start + 5 <= ID_PLUGINS_CMD_DYNAMIC_LIMIT);
    }

    void test_allocateCmdID_exhausted() {
        // Exhaust the dynamic range (ID_PLUGINS_CMD_DYNAMIC_LIMIT - ID_PLUGINS_CMD_DYNAMIC = 2000 slots)
        int start = -1;
        bool ok = m_pm->allocateCmdID(2001, &start);
        QVERIFY(!ok);
        QVERIFY(start == -1);
    }

    void test_inDynamicRange() {
        QVERIFY(m_pm->inDynamicRange(ID_PLUGINS_CMD_DYNAMIC));
        QVERIFY(m_pm->inDynamicRange(ID_PLUGINS_CMD_DYNAMIC + 100));
        QVERIFY(!m_pm->inDynamicRange(ID_PLUGINS_CMD));
        QVERIFY(!m_pm->inDynamicRange(ID_PLUGINS_CMD_DYNAMIC_LIMIT));
        QVERIFY(m_pm->inDynamicRange(ID_PLUGINS_CMD_DYNAMIC_LIMIT - 1));
    }

    // --- Marker allocation ---
    void test_allocateMarker_basic() {
        int start = -1;
        bool ok = m_pm->allocateMarker(3, &start);
        QVERIFY(ok);
        QVERIFY(start >= MARKER_PLUGINS);
        QVERIFY(start + 3 <= MARKER_PLUGINS + 15);
    }

    // --- Indicator allocation ---
    void test_allocateIndicator_basic() {
        int start = -1;
        bool ok = m_pm->allocateIndicator(5, &start);
        QVERIFY(ok);
        QVERIFY(start >= INDICATOR_PLUGINS);
        QVERIFY(start + 5 <= INDICATOR_PLUGINS + 20);
    }

    // --- Plugin info management ---
    void test_getLoadedPluginNames_empty() {
        QString names = m_pm->getLoadedPluginNames();
        QVERIFY(names.isEmpty());
    }

    void test_hasPlugins_empty() {
        QVERIFY(!m_pm->hasPlugins());
    }

    void test_getMenuHandle() {
        QVERIFY(m_pm->getMenuHandle() == nullptr);
    }

    // --- initMenu ---
    void test_initMenu_createsMenu() {
        QMenu parentMenu;
        QMenu* hMenu = m_pm->initMenu(&parentMenu, false);
        QVERIFY(hMenu != nullptr);
        QVERIFY(hMenu->title() == QStringLiteral("Plugins"));
        delete hMenu; // clean up (parentMenu takes ownership via addMenu)
    }

    void test_initMenu_withPluginAdmin() {
        QMenu parentMenu;
        QMenu* hMenu = m_pm->initMenu(&parentMenu, true);
        QVERIFY(hMenu != nullptr);
        // "Plugins Admin..." action should be added
        QVERIFY(!hMenu->actions().isEmpty());
        delete hMenu;
    }

    // --- Notification relay (no plugins loaded, should be no-op) ---
    void test_notify_noPlugins() {
        SCNotification scn;
        scn.nmhdr.hwndFrom = nullptr;
        scn.nmhdr.idFrom = 0;
        scn.nmhdr.code = NPPN_READY;
        // Should not crash
        m_pm->notify(&scn);
    }

    void test_relayNppMessages_noPlugins() {
        // Should not crash
        m_pm->relayNppMessages(NPPN_READY, 0, 0);
    }

    void test_relayPluginMessages_noPlugins() {
        bool result = m_pm->relayPluginMessages(0, 0, 0);
        QVERIFY(!result);
    }

    // --- loadPlugins with non-existent directory ---
    void test_loadPlugins_nonexistentDir() {
        bool ok = m_pm->loadPlugins(QStringLiteral("/nonexistent/path/"));
        QVERIFY(ok); // Should return true (not an error)
    }

private:
    PluginsManager* m_pm = nullptr;
};

QTEST_MAIN(TestPluginsManager)
#include "TestPluginsManager.moc"
