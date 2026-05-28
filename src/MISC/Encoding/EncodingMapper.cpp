// MISC/Encoding/EncodingMapper.cpp - Qt6 port of encoding mapping
#include "MISC/Common/QTextCodecCompat.h"
#include "EncodingMapper.h"
#include <QTextCodec>
#include <QtCore5Compat/QTextCodec>

EncodingMapper::EncodingMapper()
{
    // Initialize encoding profiles
    _encodings.append(EncodingProfile(CP_UTF8, "UTF-8", "UTF-8", true));
    _encodings.append(EncodingProfile(CP_WIN_1252, "Windows-1252 (Western European)", "Windows-1252", true));
    _encodings.append(EncodingProfile(CP_WIN_1250, "Windows-1250 (Central European)", "Windows-1250", false));
    _encodings.append(EncodingProfile(CP_WIN_1251, "Windows-1251 (Cyrillic)", "Windows-1251", false));
    _encodings.append(EncodingProfile(CP_WIN_1253, "Windows-1253 (Greek)", "Windows-1253", false));
    _encodings.append(EncodingProfile(CP_WIN_1254, "Windows-1254 (Turkish)", "Windows-1254", false));
    _encodings.append(EncodingProfile(CP_WIN_1255, "Windows-1255 (Hebrew)", "Windows-1255", false));
    _encodings.append(EncodingProfile(CP_WIN_1256, "Windows-1256 (Arabic)", "Windows-1256", false));
    _encodings.append(EncodingProfile(CP_WIN_1257, "Windows-1257 (Baltic)", "Windows-1257", false));
    _encodings.append(EncodingProfile(CP_WIN_1258, "Windows-1258 (Vietnamese)", "Windows-1258", false));
    _encodings.append(EncodingProfile(CP_WIN_437, "OEM - United States", "IBM437", true));
    _encodings.append(EncodingProfile(CP_WIN_850, "OEM - Multilingual Latin 1", "IBM850", true));
    _encodings.append(EncodingProfile(CP_WIN_852, "OEM - Latin 2", "IBM852", false));
    _encodings.append(EncodingProfile(CP_WIN_855, "OEM - Cyrillic (Russian)", "IBM855", false));
    _encodings.append(EncodingProfile(CP_WIN_866, "OEM - Cyrillic (DOS)", "IBM866", false));
    _encodings.append(EncodingProfile(CP_WIN_932, "ANSI - Japanese", "Shift-JIS", false));
    _encodings.append(EncodingProfile(CP_WIN_936, "ANSI - Simplified Chinese", "GB2312", false));
    _encodings.append(EncodingProfile(CP_WIN_949, "ANSI - Korean", "EUC-KR", false));
    _encodings.append(EncodingProfile(CP_WIN_950, "ANSI - Traditional Chinese", "Big5", false));
    _encodings.append(EncodingProfile(CP_ISO_8859_1, "ISO-8859-1 (Latin 1)", "ISO-8859-1", true));
    _encodings.append(EncodingProfile(CP_ISO_8859_2, "ISO-8859-2 (Latin 2)", "ISO-8859-2", false));
    _encodings.append(EncodingProfile(CP_ISO_8859_15, "ISO-8859-15 (Latin 9)", "ISO-8859-15", true));
    _encodings.append(EncodingProfile(CP_UTF16LE, "UTF-16 LE", "UTF-16LE", false));
    _encodings.append(EncodingProfile(CP_UTF16BE, "UTF-16 BE", "UTF-16BE", false));
    
    // Build lookup maps
    for (int i = 0; i < _encodings.size(); ++i) {
        _cpToIndex[_encodings[i].codepage] = i;
        _nameToIndex[_encodings[i].name.toLower()] = i;
    }
}

EncodingMapper& EncodingMapper::getInstance()
{
    static EncodingMapper instance;
    return instance;
}

int EncodingMapper::getEncodingIndexFromCP(int codepage) const
{
    auto it = _cpToIndex.find(codepage);
    if (it != _cpToIndex.end())
        return it.value();
    return -1;
}

int EncodingMapper::getEncodingIndexFromName(const QString& name) const
{
    auto it = _nameToIndex.find(name.toLower());
    if (it != _nameToIndex.end())
        return it.value();
    return -1;
}

int EncodingMapper::getCPFromEncodingIndex(int index) const
{
    if (index >= 0 && index < _encodings.size())
        return _encodings[index].codepage;
    return -1;
}

QString EncodingMapper::getEncodingFromIndex(int index) const
{
    if (index >= 0 && index < _encodings.size())
        return _encodings[index].encoding;
    return QString();
}

QString EncodingMapper::getNameFromIndex(int index) const
{
    if (index >= 0 && index < _encodings.size())
        return _encodings[index].name;
    return QString();
}

int EncodingMapper::encodingNameToCodePage(const QString& encodingName) const
{
    QTextCodec* codec = QTextCodec::codecForName(encodingName.toUtf8());
    if (codec) {
        QByteArray name = codec->name();
        // Map common encoding names to codepages
        if (name == "UTF-8") return CP_UTF8;
        if (name == "ISO-8859-1") return CP_ISO_8859_1;
    }
    
    // Direct lookup
    int idx = getEncodingIndexFromName(encodingName);
    if (idx >= 0)
        return getCPFromEncodingIndex(idx);
    
    return -1;
}

QString EncodingMapper::codePageToEncodingName(int codePage) const
{
    int idx = getEncodingIndexFromCP(codePage);
    if (idx >= 0)
        return getEncodingFromIndex(idx);
    
    // Try to find via QTextCodec
    // Note: QTextCodec removed in Qt6 - use QStringConverter
    // For encoding conversion use QStringConverter directly
    QTextDecoder* decoder = nullptr; // placeholder
    
    return QString();
}