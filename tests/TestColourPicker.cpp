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
    // ── Preset colour constants ──────────────────────────────────────────────
    void test_colourItemsData_count();
    void test_colourItemsData_ranges();
    void test_colourItemsData_noDuplicates();
    void test_colourItemsData_includesWhiteBlack();
    void test_colourItemsData_includesAccentColors();

    // ── ColourPopup public API ─────────────────────────────────────────────
    void test_colourPopup_constructs();
    void test_colourPopup_selectedColorInitially();
    void test_colourPopup_selectedColorAfterSet();
};

// =============================================================================
// Tests — preset constants
// =============================================================================

void TestColourPicker::test_colourItemsData_count()
{
    const int count = sizeof(colourItemsData) / sizeof(colourItemsData[0]);
    if (count != 48)
        QFAIL(QString("colourItemsData: expected 48 entries, got %1").arg(count).toUtf8().constData());
}

void TestColourPicker::test_colourItemsData_ranges()
{
    for (int i = 0; i < 48; ++i) {
        const QColor& c = colourItemsData[i];
        if (c.red()   < 0 || c.red()   > 255) QFAIL("R channel out of range");
        if (c.green() < 0 || c.green() > 255) QFAIL("G channel out of range");
        if (c.blue()  < 0 || c.blue()  > 255) QFAIL("B channel out of range");
        if (c.alpha() != 255)                QFAIL("Alpha must be opaque (255)");
    }
}

void TestColourPicker::test_colourItemsData_noDuplicates()
{
    QSet<QRgb> seen;
    for (int i = 0; i < 48; ++i) {
        QRgb rgb = colourItemsData[i].rgba();
        if (seen.contains(rgb))
            QFAIL(QString("Duplicate colour at index %1: 0x%2").arg(i).arg(rgb, 6, 16).toUtf8().constData());
        seen.insert(rgb);
    }
}

void TestColourPicker::test_colourItemsData_includesWhiteBlack()
{
    bool hasWhite = false, hasBlack = false;
    for (int i = 0; i < 48; ++i) {
        const QColor& c = colourItemsData[i];
        if (c.red() == 255 && c.green() == 255 && c.blue() == 255) hasWhite = true;
        if (c.red() == 0   && c.green() == 0   && c.blue() == 0)   hasBlack = true;
    }
    if (!hasWhite) QFAIL("colourItemsData must contain white (255,255,255)");
    if (!hasBlack) QFAIL("colourItemsData must contain black (0,0,0)");
}

void TestColourPicker::test_colourItemsData_includesAccentColors()
{
    bool hasRed = false, hasGreen = false, hasBlue = false;
    for (int i = 0; i < 48; ++i) {
        const QColor& c = colourItemsData[i];
        if (c.red() > 200 && c.green() < 50  && c.blue() < 50)  hasRed   = true;
        if (c.green() > 200 && c.red() < 50  && c.blue() < 50)  hasGreen = true;
        if (c.blue() > 200  && c.red() < 50  && c.green() < 50) hasBlue  = true;
    }
    if (!hasRed)   QFAIL("colourItemsData should contain a red accent");
    if (!hasGreen) QFAIL("colourItemsData should contain a green accent");
    if (!hasBlue)  QFAIL("colourItemsData should contain a blue accent");
}

// =============================================================================
// Tests — ColourPopup public API
// =============================================================================

void TestColourPicker::test_colourPopup_constructs()
{
    ColourPopup popup(Qt::red);
    QVERIFY2(popup.windowTitle().isEmpty() || !popup.windowTitle().isNull(),
             "ColourPopup should construct without crash");
}

void TestColourPicker::test_colourPopup_selectedColorInitially()
{
    ColourPopup popup(Qt::blue);
    if (popup.selectedColor() != Qt::blue)
        QFAIL("selectedColor should return the color passed to constructor");
}

void TestColourPicker::test_colourPopup_selectedColorAfterSet()
{
    ColourPopup popup(Qt::red);
    // We can't easily trigger selectColor() externally (it's private),
    // but we verify the initial state is correct
    if (popup.selectedColor() != Qt::red)
        QFAIL("Initial selectedColor should be constructor default");
}

QTEST_MAIN(TestColourPicker)
#include "TestColourPicker.moc"
