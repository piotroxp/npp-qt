// tests/test_charset_conversion.cpp — covers EncodingUtils (the codec
// helpers extracted from Application) and the round-trip behaviour that
// Application::onConvertToCharset relies on.
//
// These tests don't spin up an Application (that would require a full
// QApplication + MainWindow). EncodingUtils is pure functions over
// EncodingType and QString; the round-trip is a focused check that
// encode-then-decode-as-another-codec produces a defensible result.
//
// Wave 11 task #2 — charset conversion.

#include "../core/EncodingUtils.h"
#include "../common/Types.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <cassert>
#include <cstdio>

namespace {

int g_passed = 0;
int g_failed = 0;

void check(const char* name, bool cond) {
    if (cond) {
        ++g_passed;
        std::fprintf(stderr, "PASS  %s\n", name);
    } else {
        ++g_failed;
        std::fprintf(stderr, "FAIL  %s\n", name);
    }
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // ── encodingForType: standard encodings map ──────────────────────────────
    check("UTF_8 -> Utf8",     EncodingUtils::encodingForType(EncodingType::UTF_8)     == QStringConverter::Encoding::Utf8);
    check("UTF_16_LE -> LE",   EncodingUtils::encodingForType(EncodingType::UTF_16_LE) == QStringConverter::Encoding::Utf16LE);
    check("UTF_16_BE -> BE",   EncodingUtils::encodingForType(EncodingType::UTF_16_BE) == QStringConverter::Encoding::Utf16BE);
    check("UTF_32_LE -> LE32", EncodingUtils::encodingForType(EncodingType::UTF_32_LE) == QStringConverter::Encoding::Utf32LE);
    check("UTF_32_BE -> BE32", EncodingUtils::encodingForType(EncodingType::UTF_32_BE) == QStringConverter::Encoding::Utf32BE);
    check("ISO88591 -> Latin1",EncodingUtils::encodingForType(EncodingType::ISO88591)  == QStringConverter::Encoding::Latin1);

    // ── encodingFromCodecName: standard names map ───────────────────────────
    check("\"UTF-8\" -> UTF_8",         EncodingUtils::encodingFromCodecName("UTF-8")         == EncodingType::UTF_8);
    check("\"UTF-16 LE\" -> UTF_16_LE", EncodingUtils::encodingFromCodecName("UTF-16 LE")     == EncodingType::UTF_16_LE);
    check("\"UTF-16BE\" -> UTF_16_BE",  EncodingUtils::encodingFromCodecName("UTF-16BE")      == EncodingType::UTF_16_BE);
    check("\"ISO-8859-1\" -> ISO88591", EncodingUtils::encodingFromCodecName("ISO-8859-1")   == EncodingType::ISO88591);
    check("\"latin1\" -> ISO88591",     EncodingUtils::encodingFromCodecName("latin1")        == EncodingType::ISO88591);
    check("\"Windows-1252\" -> W1252",  EncodingUtils::encodingFromCodecName("Windows-1252") == EncodingType::Windows1252);
    check("\"us-ascii\" -> ASCII_7",    EncodingUtils::encodingFromCodecName("us-ascii")      == EncodingType::ASCII_7);
    check("\"ANSI\" -> ANSI",           EncodingUtils::encodingFromCodecName("ANSI")          == EncodingType::ANSI);
    check("unknown -> NONE",            EncodingUtils::encodingFromCodecName("EBCDIC-CP-US")  == EncodingType::NONE);

    // ── encodeToBytes / decodeFromBytes: round-trip on ASCII text ──────────
    {
        const QString in = QStringLiteral("Hello, world!\n\t 12345");
        QByteArray utf8Bytes = EncodingUtils::encodeToBytes(in, "UTF-8");
        check("encode ASCII to UTF-8 yields 21 bytes",
              utf8Bytes.size() == 21);
        QString back = EncodingUtils::decodeFromBytes(utf8Bytes, "UTF-8");
        check("UTF-8 round-trip preserves ASCII text",
              back == in);
    }

    // ── Latin-1 specific char: encode to UTF-8, decode as Latin-1 ──────────
    {
        // "café" — 'é' is U+00E9, encodes to 2 bytes in UTF-8 (0xC3 0xA9),
        // and to 1 byte (0xE9) in ISO-8859-1 (Latin-1).
        const QString in = QStringLiteral("café");
        QByteArray utf8Bytes = EncodingUtils::encodeToBytes(in, "UTF-8");
        check("UTF-8 bytes for 'café' is 5 bytes (2-byte é)",
              utf8Bytes.size() == 5);
        // Re-decode those UTF-8 bytes as Latin-1: byte sequence 0x63 0x61
        // 0x66 0xC3 0xA9 in Latin-1 maps to U+0063 U+0061 U+0066 U+00C3 U+00A9,
        // i.e. 5 chars: c, a, f, Ã, ©. This is *expected* lossy behaviour.
        QString lossy = EncodingUtils::decodeFromBytes(utf8Bytes, "ISO-8859-1");
        QString expected;
        expected.append(QChar(0x0063)); // c
        expected.append(QChar(0x0061)); // a
        expected.append(QChar(0x0066)); // f
        expected.append(QChar(0x00C3)); // Ã
        expected.append(QChar(0x00A9)); // ©
        check("UTF-8 bytes decoded as Latin-1 produces 5 chars 'cafÃ©' (lossy)",
              lossy == expected);
    }

    // ── Round-trip: ISO-8859-1 bytes decode correctly under that codec ─────
    {
        // Build Latin-1 bytes for "ñandú" (ñ = 0xF1, ú = 0xFA)
        QByteArray lat1Bytes;
        lat1Bytes.append(static_cast<char>(0xF1)); // ñ
        lat1Bytes.append('a');
        lat1Bytes.append('n');
        lat1Bytes.append('d');
        lat1Bytes.append(static_cast<char>(0xFA)); // ú
        QString decoded = EncodingUtils::decodeFromBytes(lat1Bytes, "ISO-8859-1");
        check("ISO-8859-1 bytes decode as 'ñandú'",
              decoded == QStringLiteral("ñandú"));
    }

    // ── Windows-1252: a name that QStringConverter::Encoding doesn't have ──
    {
        const QString in = QStringLiteral("naïve");
        QByteArray w1252Bytes = EncodingUtils::encodeToBytes(in, "Windows-1252");
        check("encode 'naïve' to Windows-1252 yields 5 bytes (1-byte ï)",
              w1252Bytes.size() == 5);
        QString back = EncodingUtils::decodeFromBytes(w1252Bytes, "Windows-1252");
        check("Windows-1252 round-trip preserves 'naïve'",
              back == in);
    }

    // ── Unknown codec names return empty (caller should handle) ────────────
    // Use a truly-bogus name — Qt5Compat's QTextCodec has hundreds of aliases,
    // so almost anything resolves. We pick a name that definitely doesn't.
    {
        QByteArray bytes = EncodingUtils::encodeToBytes(QStringLiteral("test"),
                                                       "NPPQT_NOT_A_REAL_CODEC_XYZ");
        // Either empty (unknown) or non-empty (alias-resolved); both are valid.
        // Just verify the call doesn't crash.
        check("encode with likely-bogus codec name doesn't crash", true);
        QString text = EncodingUtils::decodeFromBytes(QByteArray("test"),
                                                     "NPPQT_NOT_A_REAL_CODEC_XYZ");
        check("decode with likely-bogus codec name doesn't crash", true);
        // Suppress unused-variable warnings.
        (void)bytes;
        (void)text;
    }

    // ── File-level round-trip: write Latin-1 file, read it back ────────────
    // Note: the test text must contain only Latin-1 representable characters
    // (U+0000..U+00FF). The em-dash '—' (U+2014) is NOT in Latin-1, so we use
    // plain text for the round-trip.
    {
        QTemporaryDir tmp;
        if (tmp.isValid()) {
            const QString path = tmp.filePath("latin1.txt");
            const QString in = QStringLiteral("año nuevo, España"); // all <= U+00FF
            QFile f(path);
            if (f.open(QIODevice::WriteOnly)) {
                f.write(EncodingUtils::encodeToBytes(in, "ISO-8859-1"));
                f.close();
            }
            QFile rf(path);
            if (rf.open(QIODevice::ReadOnly)) {
                QByteArray raw = rf.readAll();
                QString back = EncodingUtils::decodeFromBytes(raw, "ISO-8859-1");
                check("file round-trip: 'año nuevo, España' survives Latin-1",
                      back == in);
            } else {
                check("file round-trip: file reopen", false);
            }
        } else {
            check("file round-trip: QTemporaryDir valid", false);
        }
    }

    std::fprintf(stderr, "\ncharset conversion: %d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}