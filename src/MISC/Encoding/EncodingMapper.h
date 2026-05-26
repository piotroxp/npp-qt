// MISC/Encoding/EncodingMapper.h - Qt6 port of encoding mapping
#pragma once

#include <QString>
#include <QMap>

struct EncodingProfile {
    int codepage;
    QString name;
    QString encoding;
    bool isWestern;
    
    EncodingProfile(int cp, const QString& n, const QString& enc, bool western = false)
        : codepage(cp), name(n), encoding(enc), isWestern(western) {}
};

class EncodingMapper
{
public:
    static EncodingMapper& getInstance();
    
    int getEncodingIndexFromCP(int codepage) const;
    int getEncodingIndexFromName(const QString& name) const;
    int getCPFromEncodingIndex(int index) const;
    QString getEncodingFromIndex(int index) const;
    QString getNameFromIndex(int index) const;
    
    // Additional encoding lookup methods
    int encodingNameToCodePage(const QString& encodingName) const;
    QString codePageToEncodingName(int codePage) const;
    
private:
    EncodingMapper();
    ~EncodingMapper() = default;
    
    EncodingMapper(const EncodingMapper&) = delete;
    EncodingMapper& operator=(const EncodingMapper&) = delete;
    
    QMap<int, int> _cpToIndex;  // codepage -> index
    QMap<QString, int> _nameToIndex;  // name -> index
    QList<EncodingProfile> _encodings;
};

// Common encoding codepage constants
const int CP_ACP = 0;
const int CP_MACCP = 2;
const int CP_OEMCP = 1;
const int CP_UTF7 = 65000;
const int CP_UTF8 = 65001;
const int CP_UTF16LE = 1200;
const int CP_UTF16BE = 1201;

// Common language codepages
const int CP_WIN_1250 = 1250;  // Central European
const int CP_WIN_1251 = 1251;  // Cyrillic
const int CP_WIN_1252 = 1252;  // Western European
const int CP_WIN_1253 = 1253;  // Greek
const int CP_WIN_1254 = 1254;  // Turkish
const int CP_WIN_1255 = 1255;  // Hebrew
const int CP_WIN_1256 = 1256;  // Arabic
const int CP_WIN_1257 = 1257;  // Baltic
const int CP_WIN_1258 = 1258;  // Vietnamese
const int CP_WIN_437 = 437;   // DOS US
const int CP_WIN_850 = 850;   // DOS Latin 1
const int CP_WIN_852 = 852;   // DOS Central European
const int CP_WIN_855 = 855;   // DOS Cyrillic
const int CP_WIN_866 = 866;   // DOS Cyrillic Russian
const int CP_WIN_932 = 932;   // Japanese
const int CP_WIN_936 = 936;   // Simplified Chinese
const int CP_WIN_949 = 949;   // Korean
const int CP_WIN_950 = 950;   // Traditional Chinese
const int CP_ISO_8859_1 = 28591;  // Latin 1
const int CP_ISO_8859_2 = 28592;  // Latin 2
const int CP_ISO_8859_15 = 28605; // Latin 9
const int CP_BIG5 = 950;
const int CP_GB2312 = 936;
const int CP_SHIFT_JIS = 932;
const int CP_EUC_JP = 51932;