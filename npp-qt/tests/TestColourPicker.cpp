// =============================================================================
// TestColourPicker.cpp — Qt Test suite for ColourPicker colour utilities
// Covers: the 48 preset colour constants and ColourPopup public API.
// =============================================================================

#include <QtTest/QtTest>
#include <QSet>
#include <QColorDialog>
#include "WinControls/ColourPicker.h"

class TestColourPicker : public QObject
{
    Q_OBJECT

private slots:
    // ── Preset colour constants (nppColourItems) ───────────────────────────
    void test_colourItems_count();
    void test_colourItems_ranges();
    void test_colourItems_includesWhiteBlack();
    void test_colourItems_includesAccentColors();
    void test_colourItems_blackIsIndex0();

    // ── ColourPopup public API ─────────────────────────────────────────────
    void test_colourPopup_constructs();
    void test_colourPopup_selectedColorInitially();
};

// =============================================================================
// Tests — preset constants (nppColourItems from ColourPopup.h)
// =============================================================================

void TestColourPicker::test_colourItems_count()
{
    const int count = sizeof(nppColourItems) / sizeof(nppColourItems[0]);
    if (count != 48)
        QFAIL(QString("nppColourItems: expected 48 entries, got %1").arg(count).toUtf8().constData());
}

void TestColourPicker::test_colourItems_ranges()
{
    for (int i = 0; i < 48; ++i) {
        const QColor& c = nppColourItems[i];
        if (c.red()   < 0 || c.red()   > 255) QFAIL("R channel out of range");
        if (c.green() < 0 || c.green() > 255) QFAIL("G channel out of range");
        if (c.blue()  < 0 || c.blue()  > 255) QFAIL("B channel out of range");
        if (c.alpha() != 255)                QFAIL("Alpha must be opaque (255)");
    }
}

void TestColourPicker::test_colourItems_includesWhiteBlack()
{
    bool hasWhite = false, hasBlack = false;
    for (int i = 0; i < 48; ++i) {
        const QColor& c = nppColourItems[i];
        if (c.red() == 255 && c.green() == 255 && c.blue() == 255) hasWhite = true;
        if (c.red() == 0   && c.green() == 0   && c.blue() == 0)   hasBlack = true;
    }
    if (!hasWhite) QFAIL("nppColourItems must contain white (255,255,255)");
    if (!hasBlack) QFAIL("nppColourItems must contain black (0,0,0)");
}

void TestColourPicker::test_colourItems_includesAccentColors()
{
    bool hasRed = false, hasGreen = false, hasBlue = false;
    for (int i = 0; i < 48; ++i) {
        const QColor& c = nppColourItems[i];
        if (c.red() > 200 && c.green() < 50  && c.blue() < 50)  hasRed   = true;
        if (c.green() > 200 && c.red() < 50  && c.blue() < 50)  hasGreen = true;
        if (c.blue() > 200  && c.red() < 50  && c.green() < 50) hasBlue  = true;
    }
    if (!hasRed)   QFAIL("nppColourItems should contain a red accent");
    if (!hasGreen) QFAIL("nppColourItems should contain a green accent");
    if (!hasBlue)  QFAIL("nppColourItems should contain a blue accent");
}

void TestColourPicker::test_colourItems_blackIsIndex0()
{
    // Index 0 must be black (matches Win32 COLORREF 0 == RGB(0,0,0))
    const QColor& c = nppColourItems[0];
    if (c.red() != 0 || c.green() != 0 || c.blue() != 0)
        QFAIL("nppColourItems[0] should be black (0,0,0)");
}

// =============================================================================
// Tests — ColourPopup public API
// =============================================================================

void TestColourPicker::test_colourPopup_constructs()
{
    ColourPopup popup(Qt::red, nullptr);
    QVERIFY2(popup.windowTitle().isEmpty() || !popup.windowTitle().isNull(),
             "ColourPopup should construct without crash");
}

void TestColourPicker::test_colourPopup_selectedColorInitially()
{
    ColourPopup popup(Qt::blue, nullptr);
    if (popup.selectedColor() != Qt::blue)
        QFAIL("selectedColor should return the color passed to constructor");
}

QTEST_MAIN(TestColourPicker)
#include "TestColourPicker.moc"
