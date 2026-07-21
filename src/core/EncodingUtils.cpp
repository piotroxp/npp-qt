// core/EncodingUtils.cpp — implementation, see EncodingUtils.h.
//
// Strategy:
//   1. Try Qt6's QStringConverter first (fast, locale-independent).
//   2. Fall back to Qt5Compat's QTextCodec for legacy codepages
//      (Windows-1252, US-ASCII, locale-specific code pages).
//   3. If neither resolves the codec name, return empty.

#include "EncodingUtils.h"
#include <QStringDecoder>
#include <QStringEncoder>

// QTextCodec lives in QtCore5Compat — included only when the project
// has been built against qt6-5compat. The CMakeLists.txt links it
// optionally; this header is harmless even if the symbols aren't used
// (the encoder/decoder fallback is only invoked when QStringConverter
// doesn't recognise the codec name).
#if __has_include(<QTextCodec>)
#  define NPPQT_HAVE_QT5COMPAT_TEXTCODEC 1
#  include <QTextCodec>
#else
#  define NPPQT_HAVE_QT5COMPAT_TEXTCODEC 0
#endif

QStringConverter::Encoding EncodingUtils::encodingForType(EncodingType enc) {
    switch (enc) {
        case EncodingType::UTF_8:
        case EncodingType::UTF_8_BOM:    return QStringConverter::Encoding::Utf8;
        case EncodingType::UTF_16_LE:
        case EncodingType::UTF_16_LE_BOM: return QStringConverter::Encoding::Utf16LE;
        case EncodingType::UTF_16_BE:
        case EncodingType::UTF_16_BE_BOM: return QStringConverter::Encoding::Utf16BE;
        case EncodingType::UTF_32_LE:     return QStringConverter::Encoding::Utf32LE;
        case EncodingType::UTF_32_BE:     return QStringConverter::Encoding::Utf32BE;
        case EncodingType::ISO88591:      return QStringConverter::Encoding::Latin1;
        // The following are not part of QStringConverter::Encoding; the
        // encode/decode helpers route them through QTextCodec (legacy).
        case EncodingType::Windows1252:
        case EncodingType::ASCII_7:
        case EncodingType::ANSI:
        case EncodingType::OEM:
        case EncodingType::EBCDIC:
        case EncodingType::Other:
        case EncodingType::NONE:
        default:                          return QStringConverter::Encoding::Utf8;
    }
}

EncodingType EncodingUtils::encodingFromCodecName(const QString& name) {
    const QString n = name.toUpper().trimmed();
    if (n == "UTF-8")                          return EncodingType::UTF_8;
    if (n == "UTF-8 BOM")                      return EncodingType::UTF_8_BOM;
    if (n == "UTF-16LE" || n == "UTF-16 LE")  return EncodingType::UTF_16_LE;
    if (n == "UTF-16BE" || n == "UTF-16 BE")  return EncodingType::UTF_16_BE;
    if (n == "UTF-32LE" || n == "UTF-32 LE")  return EncodingType::UTF_32_LE;
    if (n == "UTF-32BE" || n == "UTF-32 BE")  return EncodingType::UTF_32_BE;
    if (n == "ISO-8859-1" || n == "LATIN1")   return EncodingType::ISO88591;
    if (n == "WINDOWS-1252" || n == "CP1252") return EncodingType::Windows1252;
    if (n == "US-ASCII"   || n == "ASCII")    return EncodingType::ASCII_7;
    if (n == "ANSI")                          return EncodingType::ANSI;
    if (n == "OEM")                           return EncodingType::OEM;
    return EncodingType::NONE;
}

// Internal helper: resolve a codec name to a QStringConverter::Encoding
// (Qt6 native) when possible, otherwise fall through to QTextCodec.
static QStringConverter::Encoding nativeEncodingForName(const QString& name) {
    auto encOpt = QStringConverter::encodingForName(name.toUtf8().constData());
    return encOpt.value_or(QStringConverter::Encoding::Utf8);
}

#if NPPQT_HAVE_QT5COMPAT_TEXTCODEC
static QTextCodec* legacyCodecForName(const QString& name) {
    return QTextCodec::codecForName(name.toUtf8());
}
#endif

QByteArray EncodingUtils::encodeToBytes(const QString& text, const QString& codecName) {
    // First try Qt6 native.
    auto encOpt = QStringConverter::encodingForName(codecName.toUtf8().constData());
    if (encOpt.has_value()) {
        auto encoder = QStringEncoder(*encOpt);
        if (encoder.isValid()) return encoder.encode(text);
    }
    // Fall back to Qt5Compat QTextCodec.
#if NPPQT_HAVE_QT5COMPAT_TEXTCODEC
    if (QTextCodec* codec = legacyCodecForName(codecName)) {
        return codec->fromUnicode(text);
    }
#endif
    return QByteArray();
}

QString EncodingUtils::decodeFromBytes(const QByteArray& bytes, const QString& codecName) {
    // First try Qt6 native.
    auto encOpt = QStringConverter::encodingForName(codecName.toUtf8().constData());
    if (encOpt.has_value()) {
        auto decoder = QStringDecoder(*encOpt);
        if (decoder.isValid()) return decoder.decode(bytes);
    }
    // Fall back to Qt5Compat QTextCodec.
#if NPPQT_HAVE_QT5COMPAT_TEXTCODEC
    if (QTextCodec* codec = legacyCodecForName(codecName)) {
        return codec->toUnicode(bytes);
    }
#endif
    return QString();
}