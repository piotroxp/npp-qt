// =============================================================================
// TestColourPicker.cpp — Qt Test suite for ColourPicker colour utilities
// Covers: the 48 preset colour constants, colourRect geometry,
// colorAt() lookup, and ColourPopup internal helpers.
// =============================================================================

#include <QtTest/QtTest>
#include "ColourPicker.h"

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

    // ── ColourPopup helpers ─────────────────────────────────────────────────
    void test_colorRect_grid();
    void test_colorRect_bounds();
    void test_colorRect_allCellsInBounds();

    // ── ColourPopup::colorAt ────────────────────────────────────────────────
    void test_colorAt_insideGrid();
    void test_colorAt_outsideGrid();
    void test_colorAt_edgeCells();
};

// =============================================================================
// Tests
// =============================================================================

void TestColourPicker::test_colourItemsData_count()
{
    // colourItemsData must have exactly 48 entries (6 cols × 8 rows)
    const int count = sizeof(colourItemsData) / sizeof(colourItemsData[0]);
    if (count != 48)
        QFAIL(QString("colourItemsData: expected 48 entries, got %1").arg(count).toUtf8());
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
            QFAIL(QString("Duplicate colour at index %1: 0x%2").arg(i).arg(rgb, 6, 16).toUtf8());
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

// ── ColourPopup helpers (accessed via the ColourPopup instance) ───────────────

// Helper: create a popup on the heap and expose its private helpers
// We use a static proxy that invokes colourRect() and colorAt() on a live object
static QRect callColorRect(const ColourPopup& popup, int index)
{
    // colourRect is const in our implementation
    // Since colourRect() is const we can call it on a const ref
    return popup.colorRect(index);
}

static QColor callColorAt(const ColourPopup& popup, const QPoint& pt)
{
    return popup.colorAt(pt);
}

// We need a concrete instance; use a lambda-based workaround
// by creating a minimal concrete class in an anonymous namespace
namespace {
struct ColourPopupTestHelper : public ColourPopup {
    using ColourPopup::colorRect;
    using ColourPopup::colorAt;
    using ColourPopup::selectColor;
    ColourPopupTestHelper() : ColourPopup(Qt::red) {}
};
}

void TestColourPicker::test_colorRect_grid()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);

    // Cell 0 (row=0, col=0): x=7, y=7, w=16, h=8
    QRect r0 = popup.colorRect(0);
    if (r0.x() != 7 || r0.y() != 7)
        QFAIL(QString("Cell 0: expected (7,7), got (%1,%2)").arg(r0.x()).arg(r0.y()).toUtf8());

    // Cell 1 (row=0, col=1): x=26, y=7
    QRect r1 = popup.colorRect(1);
    if (r1.x() != 26 || r1.y() != 7)
        QFAIL(QString("Cell 1: expected (26,7), got (%1,%2)").arg(r1.x()).arg(r1.y()).toUtf8());

    // Cell 6 (row=1, col=0): x=7, y=17
    QRect r6 = popup.colorRect(6);
    if (r6.x() != 7 || r6.y() != 17)
        QFAIL(QString("Cell 6: expected (7,17), got (%1,%2)").arg(r6.x()).arg(r6.y()).toUtf8());
}

void TestColourPicker::test_colorRect_bounds()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);

    // All 48 cells must be within the dialog bounds (132x113)
    for (int i = 0; i < 48; ++i) {
        QRect r = popup.colorRect(i);
        if (r.right() > 132)
            QFAIL(QString("Cell %1 right edge %2 exceeds width 132").arg(i).arg(r.right()).toUtf8());
        if (r.bottom() > 113)
            QFAIL(QString("Cell %1 bottom edge %2 exceeds height 113").arg(i).arg(r.bottom()).toUtf8());
    }
}

void TestColourPicker::test_colorRect_allCellsInBounds()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);
    for (int i = 0; i < 48; ++i) {
        QRect r = popup.colorRect(i);
        QVERIFY2(r.left()   >= 0, "cell left must be >= 0");
        QVERIFY2(r.top()    >= 0, "cell top must be >= 0");
        QVERIFY2(r.width()  >  0, "cell width must be > 0");
        QVERIFY2(r.height() >  0, "cell height must be > 0");
    }
}

void TestColourPicker::test_colorAt_insideGrid()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);

    // Center of cell 0 = (7+8, 7+4) = (15, 11)
    QPoint center0(15, 11);
    QColor c0 = popup.colorAt(center0);
    if (!c0.isValid())
        QFAIL("colorAt center of cell 0 returned invalid colour");

    // Verify it matches colourItemsData[0]
    if (c0 != colourItemsData[0])
        QFAIL(QString("colorAt(cell 0 center): expected colourItemsData[0], got %1")
                 .arg(c0.name()).toUtf8());
}

void TestColourPicker::test_colorAt_outsideGrid()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);

    // Point far outside the grid
    QColor c = popup.colorAt(QPoint(0, 0));
    if (c.isValid())
        QFAIL("colorAt(0,0) should return invalid QColor (outside grid)");
}

void TestColourPicker::test_colorAt_edgeCells()
{
    ColourPopupTestHelper popup;
    popup.setFixedSize(132, 113);

    // Last cell = index 47 (row=7, col=5)
    // x = 7 + 5*19 = 102, y = 7 + 7*10 = 77
    QPoint lastCenter(102 + 8, 77 + 4);
    QColor c47 = popup.colorAt(lastCenter);
    if (!c47.isValid())
        QFAIL("colorAt last cell returned invalid colour");
    if (c47 != colourItemsData[47])
        QFAIL(QString("colorAt cell 47: expected colourItemsData[47]=%1, got %2")
                 .arg(colourItemsData[47].name()).arg(c47.name()).toUtf8());
}

QTEST_MAIN(TestColourPicker)
#include "TestColourPicker.moc"
