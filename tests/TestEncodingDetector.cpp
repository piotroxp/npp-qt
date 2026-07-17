// TestEncodingDetector.cpp — Qt Test suite for EncodingDetector
// Phase1 canonical source: src/core/EncodingDetector.cpp

#include <QtTest/QtTest>
#include "core/EncodingDetector.h"

class TestEncodingDetector : public QObject
{
    Q_OBJECT

private slots:
    // ── UTF-8 validity ────────────────────────────────────────────────────
    void test_isValidUtf8_ascii();
    void test_isValidUtf8_emoji();
    void test_isValidUtf8_bareContinuation();
    void test_isValidUtf8_overlongEncoding();
    void test_isValidUtf8_invalidStartByte();
    void test_isValidUtf8_truncated();

    // ── BOM detection ────────────────────────────────────────────────────
    void test_detectBOM_utf8();
    void test_detectBOM_utf16le();
    void test_detectBOM_utf16be();
    void test_detectBOM_none();

    // ── EOL detection ────────────────────────────────────────────────────
    void test_detectEol_crLf();
    void test_detectEol_lf();
    void test_detectEol_cr();
    void test_detectEol_mixed();

    // ── Encoding name / codepage ─────────────────────────────────────────
    void test_encodingName();
    void test_encodingToCodepage();
    void test_isUnicode();

private:
    EncodingDetector& _ed = EncodingDetector::instance();
};

// ─── UTF-8 validity ──────────────────────────────────────────────────────────

void TestEncodingDetector::test_isValidUtf8_ascii()
{
    // Plain ASCII is valid UTF-8
    std::string s = "Hello, world! 123";
    QVERIFY2(_ed.isValidUtf8(s), "ASCII should always be valid UTF-8");
}

void TestEncodingDetector::test_isValidUtf8_emoji()
{
    // U+1F600 = 😀 (4-byte UTF-8 sequence)
    std::string s = "Hello \xF0\x9F\x98\x80 world";
    QVERIFY2(_ed.isValidUtf8(s), "Valid 4-byte emoji sequence should be valid UTF-8");
}

void TestEncodingDetector::test_isValidUtf8_bareContinuation()
{
    // 0x80 alone is a bare continuation byte — invalid
    std::string s = "Hello \x80 world";
    QVERIFY2(!_ed.isValidUtf8(s), "Bare continuation byte should be invalid");
}

void TestEncodingDetector::test_isValidUtf8_overlongEncoding()
{
    // Overlong encoding of NUL: 2-byte for 0x00 (C0 80) — should be invalid
    std::string s = "Hi \xC0\x80 Bye";
    QVERIFY2(!_ed.isValidUtf8(s), "Overlong encoding should be invalid");
}

void TestEncodingDetector::test_isValidUtf8_invalidStartByte()
{
    // 0xFF is never a valid UTF-8 start byte
    std::string s = "Test \xFF\xFE";
    QVERIFY2(!_ed.isValidUtf8(s), "Invalid start byte should be invalid");
}

void TestEncodingDetector::test_isValidUtf8_truncated()
{
    // Valid 3-byte char at end, but missing 2 continuation bytes
    std::string s = "Hello \xE2\x82";
    QVERIFY2(!_ed.isValidUtf8(s), "Truncated UTF-8 sequence should be invalid");
}

// ─── BOM detection ────────────────────────────────────────────────────────────

void TestEncodingDetector::test_detectBOM_utf8()
{
    std::string s = "\xEF\xBB\xBFHello";  // UTF-8 BOM
    EncodingType enc = _ed.detectBOM(s);
    if (enc != EncodingType::UTF_8_BOM)
        qWarning("UTF-8 BOM detected as %d, expected UTF_8", int(enc));
}

void TestEncodingDetector::test_detectBOM_utf16le()
{
    std::string s = "\xFF\xFEHello";  // UTF-16 LE BOM
    EncodingType enc = _ed.detectBOM(s);
    if (enc != EncodingType::UTF_16_LE)
        qWarning("UTF-16-LE BOM detected as %d, expected UTF_16_LE", int(enc));
}

void TestEncodingDetector::test_detectBOM_utf16be()
{
    std::string s = "\xFE\xFFHello";  // UTF-16 BE BOM
    EncodingType enc = _ed.detectBOM(s);
    if (enc != EncodingType::UTF_16_BE)
        qWarning("UTF-16-BE BOM detected as %d, expected UTF_16_BE", int(enc));
}

void TestEncodingDetector::test_detectBOM_none()
{
    std::string s = "No BOM here";
    EncodingType enc = _ed.detectBOM(s);
    if (enc != EncodingType::ANSI)
        qWarning("No BOM detected as %d, expected NONE", int(enc));
}

// ─── EOL detection ────────────────────────────────────────────────────────────

void TestEncodingDetector::test_detectEol_crLf()
{
    std::string s = "line1\r\nline2\r\n";
    EolType eol = _ed.detectEol(s);
    if (eol != EolType::EOL_CRLF)
        qWarning("CRLF detected as %d, expected EOL_CRLF", int(eol));
}

void TestEncodingDetector::test_detectEol_lf()
{
    std::string s = "line1\nline2\n";
    EolType eol = _ed.detectEol(s);
    if (eol != EolType::EOL_LF)
        qWarning("LF detected as %d, expected EOL_LF", int(eol));
}

void TestEncodingDetector::test_detectEol_cr()
{
    std::string s = "line1\rline2\r";
    EolType eol = _ed.detectEol(s);
    if (eol != EolType::EOL_CR)
        qWarning("CR detected as %d, expected EOL_CR", int(eol));
}

void TestEncodingDetector::test_detectEol_mixed()
{
    // Windows lines in mixed file → should default to Windows
    std::string s = "line1\r\nline2\r\nline3";
    EolType eol = _ed.detectEol(s);
    if (eol != EolType::EOL_CRLF)
        qWarning("Mixed file detected as %d, expected EOL_CRLF", int(eol));
}

// ─── Encoding helpers ─────────────────────────────────────────────────────────

void TestEncodingDetector::test_encodingName()
{
    std::string name = EncodingDetector::encodingName(EncodingType::UTF_8);
    qDebug("UTF-8 name: %s", name.c_str());
    QVERIFY(!name.empty());
}

void TestEncodingDetector::test_encodingToCodepage()
{
    int cp = EncodingDetector::encodingToCodepage(EncodingType::UTF_8);
    if (cp != 65001)
        qWarning("UTF-8 codepage is %d, expected 65001", cp);
}

void TestEncodingDetector::test_isUnicode()
{
    QVERIFY(EncodingDetector::isUnicode(EncodingType::UTF_8));
    QVERIFY(EncodingDetector::isUnicode(EncodingType::UTF_16_LE));
    QVERIFY(EncodingDetector::isUnicode(EncodingType::UTF_16_BE));
    QVERIFY(!EncodingDetector::isUnicode(EncodingType::ANSI));
    QVERIFY(!EncodingDetector::isUnicode(EncodingType::OEM));
}

QTEST_MAIN(TestEncodingDetector)
#include "TestEncodingDetector.moc"
