// =============================================================================
// TestUtf8_16.cpp — Qt Test suite for Utf8_16 encoding conversion
// Covers: BOM constants, BOM detection, UTF-16 LE/BE iteration,
// UTF-8 iteration, full conversion pipeline, heuristic detection.
// =============================================================================

#include <QtTest/QtTest>
#include "Utf8_16.h"

// =============================================================================
// Helpers
// =============================================================================

// Convert QString to raw UTF-16 LE bytes
static QByteArray toUtf16Le(const QString& str)
{
    QByteArray out;
    const ushort* data = str.utf16();
    for (qsizetype i = 0; i < str.size(); ++i) {
        out.append(static_cast<char>( data[i]        & 0xFF));
        out.append(static_cast<char>((data[i] >> 8)  & 0xFF));
    }
    return out;
}

// Convert QString to raw UTF-16 BE bytes
static QByteArray toUtf16Be(const QString& str)
{
    QByteArray out;
    const ushort* data = str.utf16();
    for (qsizetype i = 0; i < str.size(); ++i) {
        out.append(static_cast<char>((data[i] >> 8)  & 0xFF));
        out.append(static_cast<char>( data[i]        & 0xFF));
    }
    return out;
}

// Collect Utf16_Iter output as a byte array
static QByteArray utf16IterCollect(Utf16_Iter& it)
{
    QByteArray result;
    Utf8_16::utf8 ch = 0;
    while (it) {
        ++it;
        while (it.get(&ch)) {
            result.append(static_cast<char>(ch));
        }
    }
    return result;
}

// Collect Utf8_Iter output as QString
static QString utf8IterCollect(Utf8_Iter& it)
{
    QString result;
    Utf8_16::utf16 ch = 0;
    while (it) {
        ++it;
        while (it.get(&ch)) {
            result.append(QChar(ch));
        }
    }
    return result;
}

// =============================================================================
// Test class
// =============================================================================

class TestUtf8_16 : public QObject
{
    Q_OBJECT

private slots:
    // ── BOM constants ─────────────────────────────────────────────────────────
    void test_bomConstants();

    // ── BOM detection ───────────────────────────────────────────────────────
    void test_determineEncodingFromBOM_data();
    void test_determineEncodingFromBOM();

    // ── Utf16_Iter ───────────────────────────────────────────────────────────
    void test_utf16Iter_asciiOnly();
    void test_utf16Iter_twoByteChars();
    void test_utf16Iter_surrogatePairs();
    void test_utf16Iter_leVsBe();
    void test_utf16Iter_reset();

    // ── Utf8_Iter ───────────────────────────────────────────────────────────
    void test_utf8Iter_asciiOnly();
    void test_utf8Iter_twoByteChars();
    void test_utf8Iter_threeByteChars();
    void test_utf8Iter_fourByteSurrogates();
    void test_utf8Iter_beOutput();
    void test_utf8Iter_noBom();
    void test_utf8Iter_reset();

    // ── Utf8_16_Read conversion pipeline ────────────────────────────────────
    void test_readPassthrough();
    void test_readUtf8BomSkip();
    void test_readUtf16LeBom();
    void test_readUtf16BeBom();
    void test_readUtf16LeNoBom();
    void test_readUtf16BeNoBom();
    void test_readUtf8NoBomDetection();
    void test_read7Bit();
    void test_read8Bit();
    void test_readEmpty();

    // ── Heuristic UTF-16 detection ─────────────────────────────────────────
    void test_isLikelyUtf16Le_data();
    void test_isLikelyUtf16Le();
    void test_isLikelyUtf16Be_data();
    void test_isLikelyUtf16Be();

    // ── Edge cases ──────────────────────────────────────────────────────────
    void test_singleByte();
    void test_veryLongAscii();
};

// =============================================================================
// Tests
// =============================================================================

void TestUtf8_16::test_bomConstants()
{
    // UTF-8 BOM: EF BB BF
    const auto* bom8 = Utf8_16::k_Boms[uniUTF8];
    QVERIFY(bom8 != nullptr);
    if (bom8[0] != 0xEF || bom8[1] != 0xBB || bom8[2] != 0xBF)
        QFAIL("UTF-8 BOM mismatch: expected EF BB BF");

    // UTF-16 BE BOM: FE FF
    const auto* bomBe = Utf8_16::k_Boms[uni16BE];
    QVERIFY(bomBe != nullptr);
    if (bomBe[0] != 0xFE || bomBe[1] != 0xFF)
        QFAIL("UTF-16 BE BOM mismatch: expected FE FF");

    // UTF-16 LE BOM: FF FE
    const auto* bomLe = Utf8_16::k_Boms[uni16LE];
    QVERIFY(bomLe != nullptr);
    if (bomLe[0] != 0xFF || bomLe[1] != 0xFE)
        QFAIL("UTF-16 LE BOM mismatch: expected FF FE");

    // uniUnknown BOM should be all zeros
    const auto* bomUnknown = Utf8_16::k_Boms[uniUnknown];
    QVERIFY(bomUnknown != nullptr);
    if (bomUnknown[0] != 0x00 || bomUnknown[1] != 0x00 || bomUnknown[2] != 0x00)
        QFAIL("uniUnknown BOM should be all zeros");
}

void TestUtf8_16::test_determineEncodingFromBOM_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("expected");

    QTest::newRow("utf8_bom")      << QByteArray("\xEF\xBB\xBFhello", 8) << int(uniUTF8);
    QTest::newRow("utf16be_bom")   << QByteArray("\xFE\xFF" "\x00""h", 5) << int(uni16BE);
    QTest::newRow("utf16le_bom")   << QByteArray("\xFF\xFE" "h\x00", 5)   << int(uni16LE);
    QTest::newRow("no_bom")        << QByteArray("hello")                  << int(uni8Bit);
    QTest::newRow("too_short")     << QByteArray("\xEF", 1)                 << int(uni8Bit);
    QTest::newRow("partial_utf16le") << QByteArray("\xFF", 1)              << int(uni8Bit);
}

void TestUtf8_16::test_determineEncodingFromBOM()
{
    QFETCH(QByteArray, data);
    QFETCH(int, expected);

    UniMode result = Utf8_16_Read::determineEncodingFromBOM(
        reinterpret_cast<const unsigned char*>(data.constData()),
        static_cast<size_t>(data.size()));

    if (result != static_cast<UniMode>(expected))
        QFAIL(QString("BOM detection: expected %1, got %2").arg(expected).arg(int(result)).toUtf8().constData());
}

void TestUtf8_16::test_utf16Iter_asciiOnly()
{
    // "AB" in UTF-16 LE: 41 00 42 00
    QByteArray buf("\x41\x00\x42\x00", 4);
    Utf16_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16LE);
    QByteArray result = utf16IterCollect(it);
    if (result != QByteArray("AB"))
        QFAIL(QString("ASCII UTF-16 LE: expected 'AB', got '%1'").arg(QString::fromLatin1(result)).toUtf8().constData());
}

void TestUtf8_16::test_utf16Iter_twoByteChars()
{
    // 'é' U+00E9 in UTF-16 LE: E9 00
    QByteArray buf("\xE9\x00""A\x00", 4);
    Utf16_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16LE);
    QByteArray result = utf16IterCollect(it);
    // 'é' in UTF-8 = C3 A9; 'A' = 41
    if (result.size() != 3)
        QFAIL(QString("Two-byte UTF-16: expected 3 bytes, got %1").arg(result.size()).toUtf8().constData());
}

void TestUtf8_16::test_utf16Iter_surrogatePairs()
{
    // 😀 U+1F600 in UTF-16 LE surrogate pair: 3D D8 00 D8
    QByteArray buf("\x3D\xD8\x00\xD8", 4);
    Utf16_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16LE);
    QByteArray result = utf16IterCollect(it);
    // 😀 in UTF-8 = F0 9F 98 80 (4 bytes)
    if (result.size() != 4)
        QFAIL(QString("Surrogate pair: expected 4 UTF-8 bytes, got %1").arg(result.size()).toUtf8().constData());
}

void TestUtf8_16::test_utf16Iter_leVsBe()
{
    QByteArray le("\x41\x00\x42\x00", 4);
    QByteArray be("\x00\x41\x00\x42", 4);

    Utf16_Iter itLe;
    itLe.set(reinterpret_cast<const Utf8_16::ubyte*>(le.constData()), le.size(), uni16LE);
    Utf16_Iter itBe;
    itBe.set(reinterpret_cast<const Utf8_16::ubyte*>(be.constData()), be.size(), uni16BE);

    if (utf16IterCollect(itLe) != utf16IterCollect(itBe))
        QFAIL("LE and BE UTF-16 must produce identical UTF-8 output");
}

void TestUtf8_16::test_utf16Iter_reset()
{
    QByteArray buf("\x41\x00\x42\x00", 4);
    Utf16_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16LE);
    ++it; ++it; // consume two output bytes

    it.reset();
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16LE);

    QByteArray result = utf16IterCollect(it);
    if (result != QByteArray("AB"))
        QFAIL("UTF-16 Iter reset failed: expected 'AB'");
}

void TestUtf8_16::test_utf8Iter_asciiOnly()
{
    QByteArray buf("AB", 2);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);
    if (utf8IterCollect(it) != QStringLiteral("AB"))
        QFAIL("UTF-8 ASCII iteration failed");
}

void TestUtf8_16::test_utf8Iter_twoByteChars()
{
    // UTF-8 'é' = C3 A9
    QByteArray buf("\xC3\xA9""A", 3);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);
    if (utf8IterCollect(it) != QStringLiteral("éA"))
        QFAIL("UTF-8 two-byte char iteration failed");
}

void TestUtf8_16::test_utf8Iter_threeByteChars()
{
    // UTF-8 '€' = E2 82 AC
    QByteArray buf("\xE2\x82\xAC""E", 4);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);
    if (utf8IterCollect(it) != QStringLiteral("€E"))
        QFAIL("UTF-8 three-byte char iteration failed");
}

void TestUtf8_16::test_utf8Iter_fourByteSurrogates()
{
    // UTF-8 '😀' = F0 9F 98 80
    QByteArray buf("\xF0\x9F\x98\x80""X", 5);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);
    if (utf8IterCollect(it) != QStringLiteral("😀X"))
        QFAIL("UTF-8 four-byte surrogate iteration failed");
}

void TestUtf8_16::test_utf8Iter_beOutput()
{
    // Raw UTF-16 BE bytes for 'AB': 00 41 00 42
    QByteArray buf("\x00\x41\x00\x42", 4);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uni16BE);

    QString result = utf8IterCollect(it);
    if (result != QStringLiteral("AB"))
        QFAIL(QString("UTF-8 Iter BE output: expected 'AB', got '%1'").arg(result).toUtf8().constData());
}

void TestUtf8_16::test_utf8Iter_noBom()
{
    QByteArray buf("print('hello')", 15);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8_NoBOM);
    if (utf8IterCollect(it) != QStringLiteral("print('hello')"))
        QFAIL("UTF-8 NoBOM iteration failed");
}

void TestUtf8_16::test_utf8Iter_reset()
{
    QByteArray buf("AB", 2);
    Utf8_Iter it;
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);
    ++it; ++it; // consume both chars

    it.reset();
    it.set(reinterpret_cast<const Utf8_16::ubyte*>(buf.constData()), buf.size(), uniUTF8);

    if (utf8IterCollect(it) != QStringLiteral("AB"))
        QFAIL("UTF-8 Iter reset failed: expected 'AB'");
}

void TestUtf8_16::test_readPassthrough()
{
    const char* raw = "Hello, world!";
    Utf8_16_Read reader;
    QByteArray out = reader.convert(raw, static_cast<size_t>(strlen(raw)));
    if (out != QByteArray(raw))
        QFAIL("Plain 8-bit passthrough failed");
}

void TestUtf8_16::test_readUtf8BomSkip()
{
    // UTF-8 with BOM
    QByteArray in = QByteArray("\xEF\xBB\xBF" "café", 8);
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    if (out != QByteArray("café"))
        QFAIL(QString("UTF-8 BOM skip: expected 'café', got '%1'").arg(QString::fromUtf8(out)).toUtf8().constData());
}

void TestUtf8_16::test_readUtf16LeBom()
{
    QByteArray in = QByteArray("\xFF\xFE", 2) + toUtf16Le(QStringLiteral("AB"));
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    if (out != QByteArray("AB"))
        QFAIL("UTF-16 LE BOM conversion failed");
}

void TestUtf8_16::test_readUtf16BeBom()
{
    QByteArray in = QByteArray("\xFE\xFF", 2) + toUtf16Be(QStringLiteral("AB"));
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    if (out != QByteArray("AB"))
        QFAIL("UTF-16 BE BOM conversion failed");
}

void TestUtf8_16::test_readUtf16LeNoBom()
{
    // Chinese characters "中文" → zero bytes in LE at even positions
    QByteArray in = toUtf16Le(QStringLiteral("中文"));
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    // "中文" in UTF-8 = E4 B8 AD E6 96 87
    if (out != QByteArray("\xE4\xB8\xAD\xE6\x96\x87"))
        QFAIL(QString("UTF-16 LE NoBOM: expected Chinese UTF-8 bytes, got %1").arg(
            QString::fromUtf8(out).toUtf8().toHex()).toUtf8());
}

void TestUtf8_16::test_readUtf16BeNoBom()
{
    QByteArray in = toUtf16Be(QStringLiteral("日本語"));
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    QVERIFY2(!out.isEmpty(), "UTF-16 BE NoBOM produced empty output");
}

void TestUtf8_16::test_readUtf8NoBomDetection()
{
    // Valid UTF-8 without BOM
    QByteArray raw("\xC3\xA9\xC3\xA0\xC3\xB9", 6); // é à ù
    Utf8_16_Read reader;
    QByteArray out = reader.convert(raw);
    if (out.size() != 6)
        QFAIL(QString("UTF-8 NoBOM: expected 6 bytes, got %1").arg(out.size()));
}

void TestUtf8_16::test_read7Bit()
{
    const char* raw = "simple text";
    Utf8_16_Read reader;
    QByteArray out = reader.convert(raw, static_cast<size_t>(strlen(raw)));
    if (out != QByteArray(raw))
        QFAIL("7-bit ASCII passthrough failed");
}

void TestUtf8_16::test_read8Bit()
{
    // Non-UTF-8 8-bit bytes
    QByteArray raw("\x80\x81\x82", 3);
    Utf8_16_Read reader;
    QByteArray out = reader.convert(raw);
    if (out != raw)
        QFAIL("8-bit passthrough failed");
}

void TestUtf8_16::test_readEmpty()
{
    Utf8_16_Read reader;
    QByteArray out = reader.convert(QByteArray());
    if (!out.isEmpty())
        QFAIL(QString("Empty buffer: expected empty output, got %1 bytes").arg(out.size()));
}

void TestUtf8_16::test_isLikelyUtf16Le_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("expected");

    // LE pattern: even-index bytes are 0 (high bytes of LE words)
    QByteArray leAscii;
    for (int i = 0; i < 200; ++i) {
        leAscii.append(static_cast<char>('A' + (i % 26))); // odd byte = low byte
        leAscii.append(char(0));                            // even byte = 0 (high byte)
    }
    QTest::newRow("le_like") << leAscii << true;

    // Normal ASCII: non-zero at even positions
    QByteArray ascii;
    for (int i = 0; i < 200; ++i) {
        ascii.append(static_cast<char>('A' + (i % 26)));
        ascii.append(static_cast<char>('a' + (i % 26)));
    }
    QTest::newRow("ascii_normal") << ascii << false;

    QTest::newRow("too_short")  << QByteArray("\x41\x00", 2) << false;
    QTest::newRow("odd_length") << QByteArray("\x41\x00\x42", 3) << false;
}

void TestUtf8_16::test_isLikelyUtf16Le()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, expected);

    bool result = Utf8_16_Read::isLikelyUtf16Le(
        reinterpret_cast<const unsigned char*>(data.constData()),
        static_cast<size_t>(data.size()));

    if (result != expected)
        QFAIL(QString("isLikelyUtf16Le: expected %1, got %2").arg(expected).arg(result).toUtf8().constData());
}

void TestUtf8_16::test_isLikelyUtf16Be_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("expected");

    // BE pattern: even-index bytes are 0 (high bytes of BE words)
    QByteArray beAscii;
    for (int i = 0; i < 200; ++i) {
        beAscii.append(char(0));
        beAscii.append(static_cast<char>('A' + (i % 26)));
    }
    QTest::newRow("be_like") << beAscii << true;

    QByteArray ascii;
    for (int i = 0; i < 200; ++i) {
        ascii.append(static_cast<char>('A' + (i % 26)));
        ascii.append(static_cast<char>('a' + (i % 26)));
    }
    QTest::newRow("ascii_normal") << ascii << false;

    QTest::newRow("too_short") << QByteArray("\x00\x41", 2) << false;
}

void TestUtf8_16::test_isLikelyUtf16Be()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, expected);

    bool result = Utf8_16_Read::isLikelyUtf16Be(
        reinterpret_cast<const unsigned char*>(data.constData()),
        static_cast<size_t>(data.size()));

    if (result != expected)
        QFAIL(QString("isLikelyUtf16Be: expected %1, got %2").arg(expected).arg(result).toUtf8().constData());
}

void TestUtf8_16::test_singleByte()
{
    QByteArray in("A", 1);
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    if (out.size() != 1 || out[0] != 'A')
        QFAIL("Single byte passthrough failed");
}

void TestUtf8_16::test_veryLongAscii()
{
    QString longStr(10000, QChar('x'));
    QByteArray in = longStr.toUtf8();
    Utf8_16_Read reader;
    QByteArray out = reader.convert(in);
    if (out.size() != in.size())
        QFAIL(QString("Long ASCII: expected %1 bytes, got %2").arg(in.size()).arg(out.size()).toUtf8().constData());
}

QTEST_MAIN(TestUtf8_16)
#include "TestUtf8_16.moc"
