// npp-qt: Qt5→Qt6 compatibility shim for QTextCodec
// QTextCodec was removed in Qt6; this provides a minimal replacement using QStringEncoder/QStringDecoder
#pragma once
#include <QString>
#include <QStringConverter>
#include <QByteArray>
#include <cstring>

// Map common MIB enum values to codec names
inline const char* mibToCodecName(int mib) {
    switch (mib) {
        case 37:   return "IBM037";
        case 201:  return "IBM273";
        case 202:  return "IBM277";
        case 208:  return "IBM870";
        case 209:  return "IBM870";
        case 225:  return "windows-1251";
        case 226:  return "windows-1252";
        case 850:  return "IBM850";
        case 858:  return "IBM858";
        case 936:  return "GB18030";
        case 949:  return "EUC-KR";
        case 950:  return "Big5";
        case 367:  return "US-ASCII";
        case 106:  return "UTF-8";
        case 1013: return "UTF-16";
        case 1014: return "UTF-16LE";
        case 1015: return "UTF-16BE";
        case 1017: return "UTF-32";
        case 1018: return "UTF-32LE";
        case 1019: return "UTF-32BE";
        case 4:    return "ISO-8859-1";
        case 5:    return "ISO-8859-2";
        case 6:    return "ISO-8859-3";
        case 7:    return "ISO-8859-4";
        case 8:    return "ISO-8859-5";
        case 9:    return "ISO-8859-6";
        case 10:   return "ISO-8859-7";
        case 11:   return "ISO-8859-8";
        case 13:   return "ISO-8859-9";
        case 14:   return "ISO-8859-10";
        case 15:   return "ISO-8859-11";
        case 16:   return "ISO-8859-13";
        case 17:   return "ISO-8859-14";
        case 18:   return "ISO-8859-15";
        case 109:  return "ISO-8859-16";
        case 20866: return "KOI8-R";
        case 21866: return "KOI8-U";
        case 28591: return "ISO-8859-1";
        case 28592: return "ISO-8859-2";
        case 28597: return "ISO-8859-7";
        case 65000: return "UTF-7";
        case 65001: return "UTF-8";
        // windows-* MIBs (derived from source)
        case 1250:  return "windows-1250";
        case 1251:  return "windows-1251";
        case 1252:  return "windows-1252";
        case 1253:  return "windows-1253";
        case 1254:  return "windows-1254";
        case 1255:  return "windows-1255";
        case 1256:  return "windows-1256";
        case 1257:  return "windows-1257";
        case 1258:  return "windows-1258";
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
