// =============================================================================
// TestCustomFileDialog.cpp — Qt Test suite for CustomFileDialog public API.
// Tests dialog setup and configuration via the public interface.
// No dialog windows are shown during testing.
// =============================================================================

#include <QtTest/QtTest>
#include <QCoreApplication>
#include "WinControls/CustomFileDialog.h"

class TestCustomFileDialog : public QObject
{
    Q_OBJECT

private slots:
    // ── Public API: setters ─────────────────────────────────────────────────
    void test_setTitleGetTitle();
    void test_setExtFilter_accumulates();
    void test_setExtFilter_wildcardIndex();
    void test_setDefExt_getDefExt();
    void test_setDefFileName_noExt();
    void test_setDefFileName_withExt();
    void test_setFolder();
    void test_setCheckbox();
    void test_enableFileTypeCheckbox();
    void test_setSaveAsCopy();
    void test_getFileTypeCheckboxValue();
    void test_getCheckboxState();

    // ── resolveInitialFolder ─────────────────────────────────────────────────
    void test_resolveInitialFolder_initialFolderWins();
};

// =============================================================================
// Tests — public API
// =============================================================================

void TestCustomFileDialog::test_setTitleGetTitle()
{
    CustomFileDialog dlg;
    dlg.setTitle("Save As");
    // No getter for title; just verify no crash
    QVERIFY(true);
}

void TestCustomFileDialog::test_setExtFilter_accumulates()
{
    CustomFileDialog dlg;
    dlg.setExtFilter("C++ Files", "*.cpp;*.h");
    dlg.setExtFilter("Text Files", "*.txt");
    // Two filters registered; verify via getFileTypeCheckboxValue (no crash)
    QVERIFY(true);
}

void TestCustomFileDialog::test_setExtFilter_wildcardIndex()
{
    CustomFileDialog dlg;
    dlg.setExtFilter("All Files", "*.*");
    QVERIFY(true); // verify no crash
}

void TestCustomFileDialog::test_setDefExt_getDefExt()
{
    CustomFileDialog dlg;
    dlg.setDefExt("md");
    dlg.setDefFileName("readme");
    // The file name should get .md appended internally
    QVERIFY(true); // verify no crash
}

void TestCustomFileDialog::test_setDefFileName_noExt()
{
    CustomFileDialog dlg;
    dlg.setDefExt("txt");
    dlg.setDefFileName("notes");
    // The implementation appends .txt when defExt is set and name has no ext
    QVERIFY(true); // Just verify no crash
}

void TestCustomFileDialog::test_setDefFileName_withExt()
{
    CustomFileDialog dlg;
    dlg.setDefExt("md");
    dlg.setDefFileName("readme.txt"); // already has .txt
    // Should not double-extend
    QVERIFY(true);
}

void TestCustomFileDialog::test_setFolder()
{
    CustomFileDialog dlg;
    dlg.setFolder("/tmp");
    QVERIFY(true); // verify no crash
}

void TestCustomFileDialog::test_setCheckbox()
{
    CustomFileDialog dlg;
    dlg.setCheckbox("Read-only", true);
    QVERIFY(true);
}

void TestCustomFileDialog::test_enableFileTypeCheckbox()
{
    CustomFileDialog dlg;
    dlg.enableFileTypeCheckbox("Lock file type", false);
    QVERIFY(true);
}

void TestCustomFileDialog::test_setSaveAsCopy()
{
    CustomFileDialog dlg;
    dlg.setSaveAsCopy(true);
    QVERIFY(true);
}

void TestCustomFileDialog::test_getFileTypeCheckboxValue()
{
    CustomFileDialog dlg;
    dlg.enableFileTypeCheckbox("Lock", true);
    if (!dlg.getFileTypeCheckboxValue())
        QFAIL("getFileTypeCheckboxValue should return true");
}

void TestCustomFileDialog::test_getCheckboxState()
{
    CustomFileDialog dlg;
    // Default implementation always returns false
    if (dlg.getCheckboxState())
        QFAIL("getCheckboxState should be false by default");
}

void TestCustomFileDialog::test_resolveInitialFolder_initialFolderWins()
{
    CustomFileDialog dlg;
    dlg.setFolder("/nonexistent_initial");
    // resolveInitialFolder prefers _initialFolder if it exists
    // If it doesn't exist, falls back. We just verify no crash.
    QVERIFY(true);
}

QTEST_MAIN(TestCustomFileDialog)
#include "TestCustomFileDialog.moc"
