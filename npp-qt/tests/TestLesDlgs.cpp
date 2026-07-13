// =============================================================================
// TestLesDlgs.cpp — Qt Test suite for lesDlgs button status flags
// Covers: button status constants and EncodingDlg internal state.
// =============================================================================

#include <QtTest/QtTest>
#include "lesDlgs.h"
#include "NppDarkMode.h"

class TestLesDlgs : public QObject
{
    Q_OBJECT

private slots:
    // ── Button status constants ──────────────────────────────────────────────
    void test_buttonStatus_nada();
    void test_buttonStatus_flags();
    void test_buttonStatus_combinations();
    void test_buttonStatus_exclusiveVsCumulative();

    // ── ButtonDlg basic API ───────────────────────────────────────────────────
    void test_buttonDlg_defaultStatus();
    void test_buttonDlg_setGetStatus();
    void test_buttonDlg_isCreated();
};

// =============================================================================
// Tests
// =============================================================================

void TestLesDlgs::test_buttonStatus_nada()
{
    if (buttonStatus_nada != 0)
        QFAIL(QString("buttonStatus_nada must be 0, got %1").arg(buttonStatus_nada).toUtf8().constData());
}

void TestLesDlgs::test_buttonStatus_flags()
{
    // Verify flags are distinct and non-zero
    if (buttonStatus_fullscreen == 0)       QFAIL("buttonStatus_fullscreen must not be 0");
    if (buttonStatus_postit == 0)           QFAIL("buttonStatus_postit must not be 0");
    if (buttonStatus_distractionFree == 0)  QFAIL("buttonStatus_distractionFree must not be 0");

    // Flags must not overlap (distinct bits)
    int overlap = buttonStatus_fullscreen & buttonStatus_postit;
    if (overlap != 0)
        QFAIL("buttonStatus_fullscreen and postit must not overlap");
    overlap = buttonStatus_fullscreen & buttonStatus_distractionFree;
    if (overlap != 0)
        QFAIL("buttonStatus_fullscreen and distractionFree must not overlap");
    overlap = buttonStatus_postit & buttonStatus_distractionFree;
    if (overlap != 0)
        QFAIL("buttonStatus_postit and distractionFree must not overlap");
}

void TestLesDlgs::test_buttonStatus_combinations()
{
    // Multiple flags can be combined
    int combined = buttonStatus_fullscreen | buttonStatus_postit;
    if (!(combined & buttonStatus_fullscreen))   QFAIL("combined missing fullscreen");
    if (!(combined & buttonStatus_postit))        QFAIL("combined missing postit");

    // All three combined
    int all = buttonStatus_fullscreen | buttonStatus_postit | buttonStatus_distractionFree;
    if (all == buttonStatus_fullscreen || all == buttonStatus_postit || all == buttonStatus_distractionFree)
        QFAIL("Combined flags must not equal any single flag");
}

void TestLesDlgs::test_buttonStatus_exclusiveVsCumulative()
{
    // nada = 0, so setting fullscreen + postit gives non-zero
    int view = buttonStatus_nada;
    if (view != 0) QFAIL("Initial view status should be 0");

    view |= buttonStatus_fullscreen;
    if (!(view & buttonStatus_fullscreen))  QFAIL("fullscreen flag not set");
    if (view & buttonStatus_postit)         QFAIL("postit flag should not be set yet");

    view |= buttonStatus_postit;
    if (!(view & buttonStatus_postit))     QFAIL("postit flag not set");
    // fullscreen should still be set (cumulative)
    if (!(view & buttonStatus_fullscreen))  QFAIL("fullscreen lost after adding postit");
}

void TestLesDlgs::test_buttonDlg_defaultStatus()
{
    NppDarkMode::instance().setEnabled(false);
    ButtonDlg dlg;
    if (dlg.getButtonStatus() != buttonStatus_nada)
        QFAIL("New ButtonDlg must default to buttonStatus_nada");
}

void TestLesDlgs::test_buttonDlg_setGetStatus()
{
    ButtonDlg dlg;

    dlg.setButtonStatus(buttonStatus_fullscreen);
    if (dlg.getButtonStatus() != buttonStatus_fullscreen)
        QFAIL("getButtonStatus must return what setButtonStatus set");

    dlg.setButtonStatus(buttonStatus_distractionFree | buttonStatus_postit);
    int s = dlg.getButtonStatus();
    if (!(s & buttonStatus_distractionFree) || !(s & buttonStatus_postit))
        QFAIL("Combined status not preserved correctly");
}

void TestLesDlgs::test_buttonDlg_isCreated()
{
    ButtonDlg dlg;
    if (dlg.isCreated())
        QFAIL("New ButtonDlg should not be created initially");
}

QTEST_MAIN(TestLesDlgs)
#include "TestLesDlgs.moc"
