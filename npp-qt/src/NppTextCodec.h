// npp-qt: Qt5 QTextCodec compatibility shim for Qt6
// Qt6 removed QTextCodec; this provides the subset actually used by Notepad++.
// Usage: #include "NppTextCodec.h" (replaces #include <QTextCodec>)

#pragma once
#include <QString>
#include <QStringEncoder>
#include <QStringDecoder>
#include <QByteArray>
#include <QStringView>
#include <cstring>

// Map MIB numbers to Qt6 encoding enum or string name.
// Qt6 QStringEncoder::Encoding: Utf8, Latin1, System, Utf16LE, Utf16BE, Utf32LE, Utf32BE
// All other encodings use string-based QStringEncoder.
static QStringEncoder::Encoding _mibEnum(int mib) {
    switch (mib) {
        case   6: return QStringEncoder::Latin1;
        case   5: return QStringEncoder::System;
        case 2250: return QStringEncoder::Utf8;
        case 1015: return QStringEncoder::Utf16LE;
        case 1014: return QStringEncoder::Utf16BE;
        case 1013: return QStringEncoder::Utf16;
        case 1017: return QStringEncoder::Utf32;
        case 1018: return QStringEncoder::Utf32BE;
        case 1019: return QStringEncoder::Utf32LE;
        default:   return QStringEncoder::Utf8;
    }
}

static const char* _mibName(int mib) {
    switch (mib) {
        case   4: return "SJIS";
        case  10: return "EUC-JP";
        case  17: return "Shift_JIS";
        case  18: return "Big5";
        case  38: return "EUC-KR";
        case  85: return "GB18030";
        case 101: return "UTF-7";
        default:  return nullptr;
    }
}

// Qt5-compatible ConverterState
class NppConverterState {
public:
    int remainingChars = 0;  // bytes not consumed (Qt5 semantics)

    // For fromUnicode: stateData() returns encoder residual bytes
    class StateData {
    public:
        StateData() = default;
        explicit StateData(const QByteArray& d) : _d(d) {}
        size_t size() const { return _d.size(); }
    private:
        QByteArray _d;
        friend class NppEncoder;
    };
    StateData stateData() const { return StateData(_encState); }

private:
    QByteArray _encState;  // encoder residual state
    friend class NppEncoder;
    friend class NppDecoder;
};

// Encoder wrapper — mimics QTextCodec's fromUnicode (Qt5)
class NppEncoder {
public:
    explicit NppEncoder(const char* name) : _enc(name), _hasError(false) {}
    explicit NppEncoder(QStringEncoder::Encoding e) : _enc(e), _hasError(false) {}
    explicit NppEncoder(int mib)
        : _enc(_mibEnum(mib) != QStringEncoder::Utf8 || !_mibName(mib)
                   ? QStringEncoder(_mibEnum(mib))
                   : QStringEncoder(_mibName(mib) ? _mibName(mib) : "UTF-8")),
          _hasError(false) {}

    // 2-arg: stateless encode
    QByteArray fromUnicode(const QString& str) { return _enc.encode(str); }

    // 3-arg: stateful encode — Qt5 compatible
    QByteArray fromUnicode(const QString& str, NppConverterState* state) {
        QByteArray result = _enc.encode(str);
        if (state) {
            state->remainingChars = 0;
            state->_encState.clear();
        }
        return result;
    }

    bool hasError() const { return _hasError; }

private:
    mutable QStringEncoder _enc;
    mutable bool _hasError;
};

// Decoder wrapper — mimics QTextCodec's toUnicode (Qt5)
class NppDecoder {
public:
    explicit NppDecoder(const char* name) : _dec(name) {}
    explicit NppDecoder(QStringEncoder::Encoding e) : _dec(e) {}
    explicit NppDecoder(int mib)
        : _dec(_mibEnum(mib) != QStringEncoder::Utf8 || !_mibName(mib)
                   ? QStringDecoder(_mibEnum(mib))
                   : QStringDecoder(_mibName(mib))) {}

    // 2-arg: stateless decode
    QString toUnicode(const char* in, int len) { return _dec.decode(QByteArrayView(in, len)); }

    // 3-arg: stateful decode — Qt5 compatible
    // remainingChars: in Qt5, tracks trailing incomplete multi-byte sequence bytes.
    // Qt6 QStringDecoder tracks this internally but doesn't expose it publicly;
    // we approximate conservatively by returning 0 (fully consumed).
    QString toUnicode(const char* in, int len, NppConverterState* state) {
        QString result = _dec.decode(QByteArrayView(in, len));
        if (state)
            state->remainingChars = 0;
        return result;
    }

private:
    mutable QStringDecoder _dec;
};

// Main codec class — mimics QTextCodec API surface used by Notepad++
class NppTextCodec {
public:
    // Static factory methods (Qt5 API: returns non-const pointer to static instance)
    static NppTextCodec* codecForMib(int mib) {
        static NppTextCodec _utf8(2250);
        static NppTextCodec _utf16le(1015);
        static NppTextCodec _utf16be(1014);
        static NppTextCodec _utf16(1013);
        static NppTextCodec _utf32(1017);
        static NppTextCodec _utf32be(1018);
        static NppTextCodec _utf32le(1019);
        static NppTextCodec _latin1(6);
        static NppTextCodec _system(5);
        static NppTextCodec _shiftjis(17);
        static NppTextCodec _big5(18);
        static NppTextCodec _euckr(38);
        static NppTextCodec _gb18030(85);
        static NppTextCodec _eucjp(10);
        static NppTextCodec _utf7(101);

        switch (mib) {
            case   4:
            case  17: return &_shiftjis;
            case   5: return &_system;
            case   6: return &_latin1;
            case  10: return &_eucjp;
            case  18: return &_big5;
            case  38: return &_euckr;
            case  85: return &_gb18030;
            case 101: return &_utf7;
            case 1013: return &_utf16;
            case 1014: return &_utf16be;
            case 1015: return &_utf16le;
            case 1017: return &_utf32;
            case 1018: return &_utf32be;
            case 1019: return &_utf32le;
            case 2250:
            case 106:  return &_utf8;
            default:   return &_utf8;
        }
    }

    static NppTextCodec* codecForName(const char* name) {
        if (!name) return nullptr;
        static NppTextCodec _utf8("UTF-8");
        static NppTextCodec _utf16le("UTF-16LE");
        static NppTextCodec _utf16be("UTF-16BE");
        static NppTextCodec _utf16("UTF-16");
        static NppTextCodec _latin1("ISO-8859-1");
        static NppTextCodec _system("System");
        static NppTextCodec _windows1252("windows-1252");
        static NppTextCodec _shiftjis("Shift_JIS");
        static NppTextCodec _big5("Big5");
        static NppTextCodec _euckr("EUC-KR");
        static NppTextCodec _gb18030("GB18030");

        if (strcmp(name, "UTF-8") == 0 || strcmp(name, "utf-8") == 0) return &_utf8;
        if (strcmp(name, "UTF-16LE") == 0) return &_utf16le;
        if (strcmp(name, "UTF-16BE") == 0) return &_utf16be;
        if (strcmp(name, "UTF-16") == 0) return &_utf16;
        if (strcmp(name, "ISO-8859-1") == 0 || strcmp(name, "latin1") == 0) return &_latin1;
        if (strcmp(name, "windows-1252") == 0) return &_windows1252;
        if (strcmp(name, "System") == 0 || strcmp(name, "SystemDefault") == 0) return &_system;
        if (strcmp(name, "Shift_JIS") == 0 || strcmp(name, "SJIS") == 0) return &_shiftjis;
        if (strcmp(name, "Big5") == 0) return &_big5;
        if (strcmp(name, "EUC-KR") == 0) return &_euckr;
        if (strcmp(name, "GB18030") == 0) return &_gb18030;

        // Generic fallback (up to 4 distinct names cached)
        static NppTextCodec _cache[4] = {
            NppTextCodec("UTF-8"), NppTextCodec("UTF-8"),
            NppTextCodec("UTF-8"), NppTextCodec("UTF-8")
        };
        static int _idx = 0;
        _cache[_idx % 4] = NppTextCodec(name);
        return &_cache[_idx++ % 4];
    }

    // Constructors
    explicit NppTextCodec(const char* name)
        : _name(name), _enc(name), _dec(name), _mib(0) {}

    explicit NppTextCodec(int mib)
        : _name(_mibName(mib) ? _mibName(mib) : "UTF-8"),
          _enc(mib), _dec(mib), _mib(mib) {}

    // 2-arg instance methods
    QString toUnicode(const char* in, int len) { return _dec.toUnicode(in, len); }
    QString toUnicode(const QByteArray& ba) { return toUnicode(ba.constData(), ba.size()); }
    QByteArray fromUnicode(const QString& str) { return _enc.fromUnicode(str); }
    QByteArray fromUnicode(const QStringView& sv) { return _enc.fromUnicode(sv.toString()); }

    // 3-arg stateful methods (Qt5 API)
    QString toUnicode(const char* in, int len, NppConverterState* state) {
        return _dec.toUnicode(in, len, state);
    }
    QByteArray fromUnicode(const QString& str, NppConverterState* state) {
        return _enc.fromUnicode(str, state);
    }

    const char* name() const { return _name; }
    bool isValid() const { return _name && _name[0]; }

    // Qt5 API alias
    using ConverterState = NppConverterState;

private:
    const char* _name;
    NppEncoder _enc;
    NppDecoder _dec;
    int _mib;
};

// Remap QTextCodec → NppTextCodec so existing call sites compile unchanged
#define QTextCodec NppTextCodec
