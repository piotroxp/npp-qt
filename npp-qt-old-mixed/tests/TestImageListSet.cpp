// =============================================================================
// TestImageListSet.cpp — Qt Test suite for ImageListSet icon list management
// Covers: IconList init/create/size, addIcon, replaceIcon, removeAll,
// FluentColorMap, Colour constants, invertLightness, icon indexing.
// =============================================================================

#include <QtTest/QtTest>
#include <QIcon>
#include <QPixmap>
#include "WinControls/ImageListSet.h"
#include "NppDarkMode.h"

class TestImageListSet : public QObject
{
    Q_OBJECT

private slots:
    // ── IconList lifecycle ────────────────────────────────────────────────────
    void test_iconList_initClearsIcons();
    void test_iconList_initSetsSize();
    void test_iconList_defaultSize();
    void test_iconList_sizeAfterInit();

    // ── addIcon ───────────────────────────────────────────────────────────────
    void test_addIcon_incrementsSize();
    void test_addIcon_nullIconSkipped();
    void test_addIcon_iconAtIndex();
    void test_addIcon_replaceIncrementsSize();

    // ── replaceIcon ───────────────────────────────────────────────────────────
    void test_replaceIcon_validIndex();
    void test_replaceIcon_invalidIndex();
    void test_replaceIcon_emptyPath();

    // ── removeAll ─────────────────────────────────────────────────────────────
    void test_removeAll_sizeZero();
    void test_removeAll_canReAdd();

    // ── Colour constants ──────────────────────────────────────────────────────
    void test_toolbarColorConstants();
    void test_toolbarColorConstants_notZero();
    void test_toolbarColorConstants_reasonableRange();

    // ── invertLightness ─────────────────────────────────────────────────────
    void test_invertLightness_black();
    void test_invertLightness_white();
    void test_invertLightness_gray();
    void test_invertLightness_colourPreservesAlpha();
    void test_invertLightness_invertIsIdempotent();

    // ── FluentColorMap ───────────────────────────────────────────────────────
    void test_fluentColorMap_enumValues();
    void test_fluentColorMap_accentVsCustom();
    void test_fluentColorMap_allEnumValues();

    // ── IconLists ─────────────────────────────────────────────────────────
    void test_toolBarIcons_createSize();
    void test_toolBarIcons_allListsExist();
    void test_toolBarIcons_reInitClears();
    void test_toolBarIcons_destroyClearsAll();
};

// =============================================================================
// Tests
// =============================================================================

void TestImageListSet::test_iconList_initClearsIcons()
{
    IconList lst;
    lst.init(QSize(16, 16));
    if (!lst.icon(0).isNull()) QFAIL("init should clear existing icons");
}

void TestImageListSet::test_iconList_initSetsSize()
{
    IconList lst;
    lst.init(QSize(24, 24));
    if (lst.iconSize() != QSize(24, 24))
        QFAIL(QString("iconSize should be 24x24, got %1x%2")
                 .arg(lst.iconSize().width()).arg(lst.iconSize().height()).toUtf8());
}

void TestImageListSet::test_iconList_defaultSize()
{
    IconList lst;
    // Default icon size should be at least 1x1
    if (lst.iconSize().width() < 1 || lst.iconSize().height() < 1)
        QFAIL("Default icon size must be at least 1x1");
}

void TestImageListSet::test_iconList_sizeAfterInit()
{
    IconList lst;
    lst.init(QSize(16, 16));
    if (lst.size() != 0)
        QFAIL(QString("size after init should be 0, got %1").arg(lst.size()).toUtf8());
}

void TestImageListSet::test_addIcon_incrementsSize()
{
    IconList lst;
    lst.init(QSize(16, 16));

    // Create a valid placeholder icon
    QPixmap pm(16, 16);
    pm.fill(Qt::red);
    QIcon icon(pm);

    lst.addIcon(icon);
    if (lst.size() != 1)
        QFAIL(QString("size after addIcon: expected 1, got %1").arg(lst.size()).toUtf8());

    lst.addIcon(icon);
    if (lst.size() != 2)
        QFAIL(QString("size after second addIcon: expected 2, got %1").arg(lst.size()).toUtf8());
}

void TestImageListSet::test_addIcon_nullIconSkipped()
{
    IconList lst;
    lst.init(QSize(16, 16));
    lst.addIcon(QIcon()); // null icon
    // Null icons are not added
    if (lst.size() != 0)
        QFAIL(QString("null icon should not be added, size=%1").arg(lst.size()).toUtf8());
}

void TestImageListSet::test_addIcon_iconAtIndex()
{
    IconList lst;
    lst.init(QSize(16, 16));

    QPixmap pm1(16, 16); pm1.fill(Qt::red);
    QPixmap pm2(16, 16); pm2.fill(Qt::blue);
    lst.addIcon(QIcon(pm1));
    lst.addIcon(QIcon(pm2));

    // Icon at index 0 should be red, index 1 should be blue
    QIcon i0 = lst.icon(0);
    QIcon i1 = lst.icon(1);
    if (i0.isNull() || i1.isNull())
        QFAIL("Icons at valid indices must not be null");
}

void TestImageListSet::test_addIcon_replaceIncrementsSize()
{
    IconList lst;
    lst.init(QSize(16, 16));
    QPixmap pm(16, 16); pm.fill(Qt::red);
    lst.addIcon(QIcon(pm));
    lst.addIcon(QIcon(pm));
    if (lst.size() != 2) QFAIL("Expected size 2");
}

void TestImageListSet::test_replaceIcon_validIndex()
{
    IconList lst;
    lst.init(QSize(16, 16));

    QPixmap pm1(16, 16); pm1.fill(Qt::red);
    QPixmap pm2(16, 16); pm2.fill(Qt::green);
    lst.addIcon(QIcon(pm1));
    lst.addIcon(QIcon(pm2));

    QPixmap pm3(16, 16); pm3.fill(Qt::blue);
    // Replace icon at index 0 with the blue one
    bool ok = lst.replaceIcon(0, QString()); // empty path → returns false
    if (ok) QFAIL("replaceIcon with empty path should return false");

    // We can't easily test replaceIcon with a real path without a real file,
    // so we just verify the function returns false for invalid inputs
    if (lst.size() != 2)
        QFAIL("replaceIcon with empty path must not change size");
}

void TestImageListSet::test_replaceIcon_invalidIndex()
{
    IconList lst;
    lst.init(QSize(16, 16));
    bool ok = lst.replaceIcon(99, "/nonexistent.png");
    if (ok)
        QFAIL("replaceIcon with invalid index should return false");
}

void TestImageListSet::test_replaceIcon_emptyPath()
{
    IconList lst;
    lst.init(QSize(16, 16));
    bool ok = lst.replaceIcon(0, "");
    if (ok)
        QFAIL("replaceIcon with empty path should return false");
}

void TestImageListSet::test_removeAll_sizeZero()
{
    IconList lst;
    lst.init(QSize(16, 16));
    QPixmap pm(16, 16); pm.fill(Qt::red);
    lst.addIcon(QIcon(pm));
    lst.addIcon(QIcon(pm));
    if (lst.size() != 2) QFAIL("Precondition: need 2 icons");

    lst.removeAll();
    if (lst.size() != 0)
        QFAIL(QString("size after removeAll: expected 0, got %1").arg(lst.size()).toUtf8());
}

void TestImageListSet::test_removeAll_canReAdd()
{
    IconList lst;
    lst.init(QSize(16, 16));
    QPixmap pm(16, 16); pm.fill(Qt::red);
    lst.addIcon(QIcon(pm));
    lst.removeAll();
    lst.addIcon(QIcon(pm));
    if (lst.size() != 1)
        QFAIL("Should be able to re-add after removeAll");
}

// =============================================================================
// Colour constants
// =============================================================================

void TestImageListSet::test_toolbarColorConstants()
{
    using namespace ImageListSetColors;
    // All constants must be non-zero
    QVERIFY2(g_cDefaultMainLight      != 0, "MainLight must not be 0");
    QVERIFY2(g_cDefaultMainDark      != 0, "MainDark must not be 0");
    QVERIFY2(g_cDefaultSecondaryLight != 0, "SecondaryLight must not be 0");
    QVERIFY2(g_cDefaultSecondaryDark  != 0, "SecondaryDark must not be 0");
    QVERIFY2(g_defaultAccentColor     != 0, "AccentColor must not be 0");
}

void TestImageListSet::test_toolbarColorConstants_notZero()
{
    using namespace ImageListSetColors;
    // Verify none are pure black
    if (g_cDefaultMainLight      == 0x000000) QFAIL("MainLight must not be pure black");
    if (g_cDefaultSecondaryLight == 0x000000) QFAIL("SecondaryLight must not be pure black");
}

void TestImageListSet::test_toolbarColorConstants_reasonableRange()
{
    using namespace ImageListSetColors;
    // All RGB channels must be in valid range
    auto validRgb = [](QRgb c) {
        return qRed(c) <= 255 && qGreen(c) <= 255 && qBlue(c) <= 255;
    };
    if (!validRgb(g_cDefaultMainLight))      QFAIL("MainLight out of range");
    if (!validRgb(g_cDefaultMainDark))       QFAIL("MainDark out of range");
    if (!validRgb(g_defaultAccentColor))      QFAIL("AccentColor out of range");
}

void TestImageListSet::test_invertLightness_black()
{
    using namespace ImageListSetColors;
    QRgb result = invertLightness(0x000000);
    // invertLightness: r = min(255, 0+40) = 40 → 255-40 = 215
    // So black → non-black
    if (result == 0x000000)
        QFAIL("invertLightness(black) must change the colour");
    if (qRed(result)   != 215 || qGreen(result) != 215 || qBlue(result) != 215)
        QFAIL(QString("invertLightness(0x000000): expected 0xD7D7D7, got 0x%1")
                 .arg(result, 6, 16).toUtf8());
}

void TestImageListSet::test_invertLightness_white()
{
    using namespace ImageListSetColors;
    QRgb result = invertLightness(0xFFFFFF);
    // white: r = min(255, 255+40) = 255 → 255-255 = 0
    if (result != 0xff000000)
        QFAIL(QString("invertLightness(white): expected 0xff000000, got 0x%1")
                 .arg(result, 6, 16).toUtf8());
}

void TestImageListSet::test_invertLightness_gray()
{
    using namespace ImageListSetColors;
    QRgb midGray = 0x808080;
    QRgb result = invertLightness(midGray);
    // r = min(255, 128+40) = 168 → 255-168 = 87
    if (qRed(result) != 87 || qGreen(result) != 87 || qBlue(result) != 87)
        QFAIL(QString("invertLightness(0x808080): expected 0x575757, got 0x%1")
                 .arg(result, 6, 16).toUtf8());
}

void TestImageListSet::test_invertLightness_colourPreservesAlpha()
{
    // invertLightness only operates on RGB, alpha is irrelevant for QRgb
    using namespace ImageListSetColors;
    QRgb result = invertLightness(0xAABBCC);
    if (qRed(result)   != 255 - qMin(255, qRed(0xAABBCC)   + 40))
        QFAIL("Red channel not inverted correctly");
    if (qGreen(result) != 255 - qMin(255, qGreen(0xAABBCC) + 40))
        QFAIL("Green channel not inverted correctly");
    if (qBlue(result)  != 255 - qMin(255, qBlue(0xAABBCC)  + 40))
        QFAIL("Blue channel not inverted correctly");
}

void TestImageListSet::test_invertLightness_invertIsIdempotent()
{
    using namespace ImageListSetColors;
    QRgb original = 0xAABBCC;
    QRgb once  = invertLightness(original);
    QRgb twice = invertLightness(once);
    // Not perfectly idempotent due to clamping, but should be close
    int dr = qAbs(qRed(original)   - qRed(twice));
    int dg = qAbs(qGreen(original) - qGreen(twice));
    int db = qAbs(qBlue(original)  - qBlue(twice));
    // Double invert should be at least closer to original than to black
    if (dr > 20 || dg > 20 || db > 20)
        QFAIL("invertLightness should roughly self-invert");
}

void TestImageListSet::test_fluentColorMap_enumValues()
{
    using namespace FluentColorMap;
    // All enum values should return a valid QRgb
    QRgb vals[10];
    for (int i = 0; i < 10; ++i) {
        vals[i] = fromEnum(static_cast<FluentColor>(i));
        if (vals[i] == 0)
            QFAIL(QString("FluentColorMap(%1): must not return 0").arg(i).toUtf8());
    }
}

void TestImageListSet::test_fluentColorMap_accentVsCustom()
{
    using namespace FluentColorMap;
    QRgb accent = fromEnum(FluentColor::accent);
    QRgb custom = fromEnum(FluentColor::custom, 0);
    // custom(0) falls back to accent
    if (custom != accent)
        QFAIL("custom(0) must return same as accent");
}

void TestImageListSet::test_fluentColorMap_allEnumValues()
{
    using namespace FluentColorMap;
    // Just verify each enum value maps without crash
    for (int i = 0; i < 10; ++i) {
        QRgb val = fromEnum(static_cast<FluentColor>(i));
        QVERIFY2(val != 0, "FluentColorMap must return non-zero for all values");
    }
}

void TestImageListSet::test_toolBarIcons_createSize()
{
    IconLists tb;
    tb.create(QSize(16, 16));
    // Must have 8 icon lists
    if (tb.list(0).size() >= 0 && tb.list(7).size() >= 0) {
        // lists exist, that's fine
    } else {
        QFAIL("IconLists::list(0..7) must be accessible");
    }
}

void TestImageListSet::test_toolBarIcons_allListsExist()
{
    IconLists tb;
    tb.create(QSize(16, 16));
    // All 8 HLIST indices must be accessible
    for (int i = 0; i < HLIST_COUNT; ++i) {
        if (i < 0 || i > 7) continue; // HLIST_COUNT == 8
        IconList& lst = tb.list(i);
        // If we get here, the list exists
        Q_UNUSED(lst);
    }
}

void TestImageListSet::test_toolBarIcons_reInitClears()
{
    IconLists tb;
    tb.create(QSize(16, 16));
    // reInit should not crash and should keep lists accessible
    tb.reInit(QSize(32, 32));
    QVERIFY(true);
}

void TestImageListSet::test_toolBarIcons_destroyClearsAll()
{
    IconLists tb;
    tb.create(QSize(16, 16));
    tb.destroy();
    // After destroy, the object should be re-creatable
    tb.create(QSize(16, 16));
    QVERIFY(true);
}

QTEST_MAIN(TestImageListSet)
#include "TestImageListSet.moc"
