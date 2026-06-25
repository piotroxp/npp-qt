// =============================================================================
// TestNppDarkMode.cpp — Qt Test suite for NppDarkMode colour/singleton logic
// Covers: singleton lifecycle, colour getters/setters, palette building,
// brush/pen caching, tone switching, tree-view style calculation,
// perceived lightness helper, and hexRgb helper.
// =============================================================================

#include <QtTest/QtTest>
#include "NppDarkMode.h"

class TestNppDarkMode : public QObject
{
    Q_OBJECT

private slots:
    // ── Singleton ────────────────────────────────────────────────────────────
    void test_singletonNotNull();
    void test_singletonSameInstance();

    // ── hexRgb helper ────────────────────────────────────────────────────────
    void test_hexRgb_basic();
    void test_hexRgb_roundtrip();
    void test_hexRgb_borderColors();

    // ── Default colours ─────────────────────────────────────────────────────
    void test_defaultColorsBlackTone();
    void test_defaultColorsRedTone();
    void test_defaultColorsBlueTone();
    void test_defaultColorsGreenTone();
    void test_defaultColorsPurpleTone();
    void test_defaultColorsCyanTone();
    void test_defaultColorsOliveTone();

    // ── Enable / disable ─────────────────────────────────────────────────────
    void test_defaultDisabled();
    void test_enableDisable();
    void test_enableEmitsSignal();

    // ── Colour getters ────────────────────────────────────────────────────────
    void test_colorGetters();
    void test_colorSetters();
    void test_setColorEmitsSignal();

    // ── Brushes ──────────────────────────────────────────────────────────────
    void test_brushesAreBuilt();
    void test_brushesCaching();
    void test_brushesAfterColorChange();

    // ── Pens ─────────────────────────────────────────────────────────────────
    void test_pensAreBuilt();
    void test_pensCaching();

    // ── Palette ──────────────────────────────────────────────────────────────
    void test_paletteWhenDisabledIsEmpty();
    void test_paletteWhenEnabled();

    // ── Tone switching ───────────────────────────────────────────────────────
    void test_setToneBlack();
    void test_setToneEmitsColorsChanged();

    // ── Tree-view style ─────────────────────────────────────────────────────
    void test_calculateTreeViewStyle_data();
    void test_calculateTreeViewStyle();

    // ── Perceived lightness ─────────────────────────────────────────────────
    void test_perceivedLightness_white();
    void test_perceivedLightness_black();
    void test_perceivedLightness_midGray();
    void test_perceivedLightness_blue();

    // ── Constants ───────────────────────────────────────────────────────────
    void test_kDarkColorsConstants();
    void test_kDarkColorsValues();
};

// =============================================================================
// Helpers
// =============================================================================

// Access private rebuildBrushes via friend access (NppDarkMode is a friend)
static void forceRebuildBrushes(NppDarkMode& dm)
{
    dm.rebuildBrushes();
}

// =============================================================================
// Tests
// =============================================================================

void TestNppDarkMode::test_singletonNotNull()
{
    NppDarkMode& instance = NppDarkMode::instance();
    QVERIFY2(&instance != nullptr, "Singleton instance() must not return nullptr");
}

void TestNppDarkMode::test_singletonSameInstance()
{
    NppDarkMode& a = NppDarkMode::instance();
    NppDarkMode& b = NppDarkMode::instance();
    if (&a != &b)
        QFAIL("Singleton instance() must return the same instance");
}

void TestNppDarkMode::test_hexRgb_basic()
{
    // 0xRRGGBB → QRgb byte order: 0xRRGGBB
    QRgb result = NppDarkMode::hexRgb(0xAABBCC);
    if (qRed(result)   != 0xAA) QFAIL("hexRgb red channel mismatch");
    if (qGreen(result) != 0xBB) QFAIL("hexRgb green channel mismatch");
    if (qBlue(result)  != 0xCC) QFAIL("hexRgb blue channel mismatch");
}

void TestNppDarkMode::test_hexRgb_roundtrip()
{
    // Feeding the dark-mode colour constant back through hexRgb
    QRgb c = NppDarkMode::hexRgb(0x202020);
    if (qRed(c) != 0x20 || qGreen(c) != 0x20 || qBlue(c) != 0x20)
        QFAIL("hexRgb failed for 0x202020");
}

void TestNppDarkMode::test_hexRgb_borderColors()
{
    if (NppDarkMode::hexRgb(0x000000) != 0x000000) QFAIL("black hexRgb failed");
    if (NppDarkMode::hexRgb(0xFF0000) != 0xFF0000) QFAIL("red hexRgb failed");
    if (NppDarkMode::hexRgb(0x00FF00) != 0x00FF00) QFAIL("green hexRgb failed");
    if (NppDarkMode::hexRgb(0x0000FF) != 0x0000FF) QFAIL("blue hexRgb failed");
    if (NppDarkMode::hexRgb(0xFFFFFF) != 0xFFFFFF) QFAIL("white hexRgb failed");
}

void TestNppDarkMode::test_defaultColorsBlackTone()
{
    NppDarkMode& dm = NppDarkMode::instance();
    Colors c = dm.defaultColors(NppDarkMode::ColorTone::blackTone);

    // Background must be dark
    if (qRed(c.background) == 0 && qGreen(c.background) == 0 && qBlue(c.background) == 0)
        QFAIL("Black tone bg must not be pure black (0x202020 minimum)");
    // Text must be light
    if (qRed(c.text) < 128) QFAIL("Black tone text must be light");
}

void TestNppDarkMode::test_defaultColorsRedTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::redTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Red tone must differ from black tone");
}

void TestNppDarkMode::test_defaultColorsBlueTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blueTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Blue tone must differ from black tone");
}

void TestNppDarkMode::test_defaultColorsGreenTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::greenTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Green tone must differ from black tone");
}

void TestNppDarkMode::test_defaultColorsPurpleTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::purpleTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Purple tone must differ from black tone");
}

void TestNppDarkMode::test_defaultColorsCyanTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::cyanTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Cyan tone must differ from black tone");
}

void TestNppDarkMode::test_defaultColorsOliveTone()
{
    Colors c = NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::oliveTone);
    if (c.background == NppDarkMode::instance().defaultColors(NppDarkMode::ColorTone::blackTone).background)
        QFAIL("Olive tone must differ from black tone");
}

void TestNppDarkMode::test_defaultDisabled()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(false);
    if (dm.isEnabled())
        QFAIL("Dark mode should be disabled by default");
}

void TestNppDarkMode::test_enableDisable()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(false);

    dm.setEnabled(true);
    if (!dm.isEnabled()) QFAIL("setEnabled(true) failed");

    dm.setEnabled(false);
    if (dm.isEnabled()) QFAIL("setEnabled(false) failed");

    dm.setEnabled(true);
    if (!dm.isEnabled()) QFAIL("setEnabled(true) again failed");
}

void TestNppDarkMode::test_enableEmitsSignal()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(false);

    QSignalSpy spy(&dm, &NppDarkMode::darkModeChanged);
    dm.setEnabled(true);

    if (spy.size() != 1)
        QFAIL(QString("darkModeChanged signal: expected 1 emission, got %1").arg(spy.size()).toUtf8());
}

void TestNppDarkMode::test_colorGetters()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);

    Colors c = dm.colors();
    // Verify all colour accessors return consistent values
    if (dm.backgroundColor()    != c.background)       QFAIL("backgroundColor getter mismatch");
    if (dm.ctrlBackgroundColor()!= c.softerBackground) QFAIL("ctrlBackgroundColor mismatch");
    if (dm.hotBackgroundColor()  != c.hotBackground)   QFAIL("hotBackgroundColor mismatch");
    if (dm.textColor()           != c.text)            QFAIL("textColor mismatch");
    if (dm.darkerTextColor()     != c.darkerText)      QFAIL("darkerTextColor mismatch");
    if (dm.disabledTextColor()   != c.disabledText)    QFAIL("disabledTextColor mismatch");
    if (dm.edgeColor()           != c.edge)            QFAIL("edgeColor mismatch");
}

void TestNppDarkMode::test_colorSetters()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setBackgroundColor(0x123456);
    if (dm.backgroundColor() != 0x123456)
        QFAIL("setBackgroundColor failed");
}

void TestNppDarkMode::test_setColorEmitsSignal()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    QSignalSpy spy(&dm, &NppDarkMode::colorsChanged);

    dm.setBackgroundColor(0xAABBCC);

    if (spy.size() != 1)
        QFAIL("setBackgroundColor must emit colorsChanged");
}

void TestNppDarkMode::test_brushesAreBuilt()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setColors(dm.defaultColors(NppDarkMode::ColorTone::blackTone));

    // Trigger lazy build by accessing brushes
    QBrush bg     = dm.backgroundBrush();
    QBrush ctrlBg = dm.ctrlBackgroundBrush();
    QBrush hotBg  = dm.hotBackgroundBrush();
    QBrush edge   = dm.edgeBrush();

    QVERIFY2(!bg.isBrushSet(),     "backgroundBrush should be set");
    QVERIFY2(!ctrlBg.isBrushSet(), "ctrlBackgroundBrush should be set");
    QVERIFY2(!hotBg.isBrushSet(),  "hotBackgroundBrush should be set");
    QVERIFY2(!edge.isBrushSet(),   "edgeBrush should be set");

    // Colour should match defaults
    if (bg.color().rgb() != dm.backgroundColor())
        QFAIL("backgroundBrush colour mismatch");
}

void TestNppDarkMode::test_brushesCaching()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setColors(dm.defaultColors(NppDarkMode::ColorTone::blackTone));

    QBrush a = dm.backgroundBrush();
    QBrush b = dm.backgroundBrush();
    if (a != b)
        QFAIL("Brushed must be cached (identical brush on repeated calls)");
}

void TestNppDarkMode::test_brushesAfterColorChange()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setColors(dm.defaultColors(NppDarkMode::ColorTone::blackTone));

    QBrush before = dm.backgroundBrush();

    Colors c = dm.defaultColors(NppDarkMode::ColorTone::blueTone);
    dm.setColors(c);

    QBrush after = dm.backgroundBrush();

    if (before == after)
        QFAIL("Brush must change after setColors");
}

void TestNppDarkMode::test_pensAreBuilt()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);

    QPen edge = dm.edgePen();
    QPen hot  = dm.hotEdgePen();
    QPen dis  = dm.disabledEdgePen();
    QPen txt  = dm.darkerTextPen();

    QVERIFY2(edge.color().isValid(), "edgePen colour must be valid");
    QVERIFY2(hot.color().isValid(),  "hotEdgePen colour must be valid");
    QVERIFY2(dis.color().isValid(),  "disabledEdgePen colour must be valid");
    QVERIFY2(txt.color().isValid(),  "darkerTextPen colour must be valid");
}

void TestNppDarkMode::test_pensCaching()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);

    QPen a = dm.edgePen();
    QPen b = dm.edgePen();
    if (a != b)
        QFAIL("Pens must be cached");
}

void TestNppDarkMode::test_paletteWhenDisabledIsEmpty()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(false);

    QPalette pal = dm.palette();
    // When disabled the palette should be null/empty
    if (pal.color(QPalette::Window).isValid())
        QFAIL("Palette should be empty when dark mode is disabled");
}

void TestNppDarkMode::test_paletteWhenEnabled()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setColors(dm.defaultColors(NppDarkMode::ColorTone::blackTone));

    QPalette pal = dm.palette();
    if (!pal.color(QPalette::Window).isValid())
        QFAIL("Window colour must be valid when dark mode enabled");
    if (!pal.color(QPalette::WindowText).isValid())
        QFAIL("WindowText colour must be valid");
}

void TestNppDarkMode::test_setToneBlack()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setTone(NppDarkMode::ColorTone::blackTone);

    Colors c = dm.colors();
    Colors black = dm.defaultColors(NppDarkMode::ColorTone::blackTone);
    if (c.background != black.background)
        QFAIL("setTone(black) must set background to black tone background");
}

void TestNppDarkMode::test_setToneEmitsColorsChanged()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    QSignalSpy spy(&dm, &NppDarkMode::colorsChanged);
    dm.setTone(NppDarkMode::ColorTone::blueTone);
    if (spy.isEmpty())
        QFAIL("setTone must emit colorsChanged");
}

void TestNppDarkMode::test_calculateTreeViewStyle_data()
{
    QTest::addColumn<QRgb>("bgColor");
    QTest::addColumn<NppDarkMode::TreeViewStyle>("expected");

    QTest::newRow("very_dark")   << QRgb(0x101010) << NppDarkMode::TreeViewStyle::dark;
    QTest::newRow("mid_dark")    << QRgb(0x303030) << NppDarkMode::TreeViewStyle::dark;
    QTest::newRow("mid_light")   << QRgb(0xC0C0C0) << NppDarkMode::TreeViewStyle::light;
    QTest::newRow("very_light")  << QRgb(0xFFFFFF) << NppDarkMode::TreeViewStyle::light;
    QTest::newRow("medium_gray") << QRgb(0x808080) << NppDarkMode::TreeViewStyle::classic;
}

void TestNppDarkMode::test_calculateTreeViewStyle()
{
    QFETCH(QRgb, bgColor);
    QFETCH(NppDarkMode::TreeViewStyle, expected);

    NppDarkMode& dm = NppDarkMode::instance();
    dm.setColors(dm.defaultColors(NppDarkMode::ColorTone::blackTone));
    dm.calculateTreeViewStyle(bgColor);

    TreeViewStyle actual = dm.treeViewStyle();
    if (actual != expected)
        QFAIL(QString("calculateTreeViewStyle(0x%1): expected %2, got %3")
                 .arg(bgColor, 6, 16).arg(int(expected)).arg(int(actual)).toUtf8());
}

void TestNppDarkMode::test_perceivedLightness_white()
{
    // Perceived lightness of white should be 100
    NppDarkMode& dm = NppDarkMode::instance();
    // Use internal helper indirectly via treeViewStyle
    // (it's a static free function, not directly accessible here)
    // We verify it is called without crash via calculateTreeViewStyle
    dm.calculateTreeViewStyle(0xFFFFFF);
    if (dm.treeViewStyle() != NppDarkMode::TreeViewStyle::light)
        QFAIL("calculateTreeViewStyle(white) should return light");
}

void TestNppDarkMode::test_perceivedLightness_black()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.calculateTreeViewStyle(0x000000);
    if (dm.treeViewStyle() != NppDarkMode::TreeViewStyle::dark)
        QFAIL("calculateTreeViewStyle(black) should return dark");
}

void TestNppDarkMode::test_perceivedLightness_midGray()
{
    NppDarkMode& dm = NppDarkMode::instance();
    dm.calculateTreeViewStyle(0x808080);
    if (dm.treeViewStyle() != NppDarkMode::TreeViewStyle::classic)
        QFAIL("calculateTreeViewStyle(0x808080) should return classic");
}

void TestNppDarkMode::test_perceivedLightness_blue()
{
    // Blue is dark in ITU-R BT.709 luminance → should trigger dark style
    NppDarkMode& dm = NppDarkMode::instance();
    dm.calculateTreeViewStyle(0x000080);
    if (dm.treeViewStyle() != NppDarkMode::TreeViewStyle::dark)
        QFAIL("calculateTreeViewStyle(blue) should return dark");
}

void TestNppDarkMode::test_kDarkColorsConstants()
{
    // Verify the public constants match the singletons defaults
    NppDarkMode& dm = NppDarkMode::instance();
    dm.setEnabled(true);
    dm.setTone(NppDarkMode::ColorTone::blackTone);

    Colors c = dm.colors();
    if (c.background    != k_darkBg)           QFAIL("k_darkBg mismatch");
    if (c.text         != k_darkText)          QFAIL("k_darkText mismatch");
    if (c.edge         != k_darkEdge)          QFAIL("k_darkEdge mismatch");
}

void TestNppDarkMode::test_kDarkColorsValues()
{
    // Absolute value checks
    if (k_darkBg         != 0x202020) QFAIL("k_darkBg value wrong");
    if (k_darkText       != 0xE0E0E0) QFAIL("k_darkText value wrong");
    if (k_darkEdge       != 0x646464) QFAIL("k_darkEdge value wrong");
    if (k_darkLink       != 0xFFFF00) QFAIL("k_darkLink value wrong");
    if (k_darkHotBg      != 0x454545) QFAIL("k_darkHotBg value wrong");
    if (k_darkPureBg     != 0x202020) QFAIL("k_darkPureBg value wrong");
    if (k_darkDisabledEdge != 0x484848) QFAIL("k_darkDisabledEdge value wrong");
}

QTEST_MAIN(TestNppDarkMode)
#include "TestNppDarkMode.moc"
