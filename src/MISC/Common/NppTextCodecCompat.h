// npp-qt: Qt5→Qt6 compatibility shim for QTextCodec
// QTextCodec was removed in Qt6; this provides a minimal replacement using QStringEncoder/QStringDecoder
#pragma once
#include <QString>
#include <QStringConverter>
#include <QByteArray>
#include <cstring>

// Map common MIB enum values and Windows code page IDs to codec names.
// Covers all code pages from EncodingMapper's encoding table.
// Qt6's QStringConverter uses IANA/ICU-style names (windows-*, ISO-*, IBM*, etc.).
inline const char* mibToCodecName(int mib) {
    switch (mib) {
        // === Windows code pages (Windows-125*) ===
        case 1250:  return "windows-1250";
        case 1251:  return "windows-1251";
        case 1252:  return "windows-1252";
        case 1253:  return "windows-1253";
        case 1254:  return "windows-1254";
        case 1255:  return "windows-1255";
        case 1256:  return "windows-1256";
        case 1257:  return "windows-1257";
        case 1258:  return "windows-1258";
        // === ISO-8859-* (Latin N) ===
        case 28591: return "ISO-8859-1";
        case 28592: return "ISO-8859-2";
        case 28593: return "ISO-8859-3";
        case 28594: return "ISO-8859-4";
        case 28595: return "ISO-8859-5";
        case 28596: return "ISO-8859-6";
        case 28597: return "ISO-8859-7";
        case 28598: return "ISO-8859-8";
        case 28599: return "ISO-8859-9";
        case 28603: return "ISO-8859-13";
        case 28604: return "ISO-8859-14";
        case 28605: return "ISO-8859-15";
        // === DOS / OEM code pages ===
        case 437:   return "CP437";
        case 720:   return "CP720";
        case 737:   return "CP737";
        case 775:   return "CP775";
        case 850:   return "CP850";
        case 852:   return "CP852";
        case 855:   return "CP855";
        case 857:   return "CP857";
        case 858:   return "CP858";
        case 860:   return "CP860";
        case 861:   return "CP861";
        case 862:   return "CP862";
        case 863:   return "CP863";
        case 865:   return "CP865";
        case 866:   return "CP866";
        case 869:   return "CP869";
        // === CJK code pages ===
        case 950:   return "Big5";
        case 936:   return "GB18030";
        case 932:   return "Shift_JIS";
        case 949:   return "windows-949";
        case 51949: return "EUC-KR";
        // === Other legacy ===
        case 874:   return "windows-874";
        case 10007: return "x-mac-cyrillic";
        case 21866: return "KOI8-U";
        case 20866: return "KOI8-R";
        // === Standard MIB enums ===
        case 37:   return "IBM037";
        case 201:  return "IBM273";
        case 202:  return "IBM277";
        case 208:  return "IBM870";
        case 209:  return "IBM870";
        case 367:  return "US-ASCII";
        case 65000: return "UTF-7";
        case 65001: return "UTF-8";
        case 106:  return "UTF-8";
        case 1013: return "UTF-16";
        case 1014: return "UTF-16LE";
        case 1015: return "UTF-16BE";
        default:   return nullptr;
    }
}

// Qt6 replacement for QTextCodec::ConverterState
struct NppConverterState {
    int remainingChars = 0;
    QByteArray stateData() const { return {}; }
};

// Minimal QTextCodec replacement using QStringDecoder/QStringEncoder
class NppTextCodec {
public:
    static NppTextCodec* codecForName(const char* name) {
        if (!name) return nullptr;
        auto enc = QStringDecoder::encodingForName(name);
        if (!enc.has_value()) return nullptr;
        return new NppTextCodec(*enc, name);
    }
    static NppTextCodec* codecForMib(int mib) {
        const char* name = mibToCodecName(mib);
        if (!name) return nullptr;
        return codecForName(name);
    }

    // Decode bytes → QString. For incremental use, caller must manage state.
    QString toUnicode(const char* chars, int len, NppConverterState* state = nullptr) const {
        if (!chars || len < 0) len = int(std::strlen(chars));
        // For single-shot decode, remaining chars = 0
        QString result = _decoder.decode(QByteArrayView(chars, len));
        if (state) state->remainingChars = 0;
        return result;
    }

    // Encode QString → QByteArray
    QByteArray fromUnicode(const QString& str) const {
        return _encoder.encode(str).result();
    }

private:
    explicit NppTextCodec(QStringConverter::Encoding enc, const char*) : _decoder(enc), _encoder(enc) {}
    QStringDecoder _decoder;
    QStringEncoder _encoder;
};

// Backward-compatible typedef for code that uses QTextCodec
using QTextCodec = NppTextCodec;
