// EncodingMapper.cpp — Qt6 port of Win32 codepage → encoding name mapping
// Ported from PowerEditor/src/EncodingMapper.cpp
// Copyright (C) 2026 | GPL v3

#include <cstring>
#include <QString>
#include <QStringView>
#include "core/EncodingMapper.h"
#include "NppSciCompat.h"

// Codepage → Qt encoding name mapping table
// Generated from Win32 codepage definitions (same order as Win32 N++)
static const struct CodePageEntry {
    int codePage;
    const char* qtName;          // Qt5/6 encoding name
    const char* aliasList;       // Space-separated aliases (Win32 compatible)
} codePageTable[] = {
    {1250,  "windows-1250",   "windows-1250"},
    {1251,  "windows-1251",   "windows-1251"},
    {1252,  "windows-1252",   "windows-1252"},
    {1253,  "windows-1253",   "windows-1253"},
    {1254,  "windows-1254",   "windows-1254"},
    {1255,  "windows-1255",   "windows-1255"},
    {1256,  "windows-1256",   "windows-1256"},
    {1257,  "windows-1257",   "windows-1257"},
    {1258,  "windows-1258",   "windows-1258"},
    {28591, "ISO-8859-1",     "latin1 ISO_8859-1 ISO-8859-1 CP819 IBM819 csISOLatin1 iso-ir-100 l1"},
    {28592, "ISO-8859-2",     "latin2 ISO_8859-2 ISO-8859-2 csISOLatin2 iso-ir-101 l2"},
    {28593, "ISO-8859-3",     "latin3 ISO_8859-3 ISO-8859-3 csISOLatin3 iso-ir-109 l3"},
    {28594, "ISO-8859-4",     "latin4 ISO_8859-4 ISO-8859-4 csISOLatin4 iso-ir-110 l4"},
    {28595, "ISO-8859-5",     "ISO-8859-5 csISOLatinCyrillic iso-ir-144"},
    {28596, "ISO-8859-6",     "ISO-8859-6 csISOLatinArabic iso-ir-127 ASMO-708 ECMA-114"},
    {28597, "ISO-8859-7",     "ISO-8859-7 greek8 iso-ir-126 ELOT_928 ECMA-118"},
    {28598, "ISO-8859-8",     "ISO-8859-8 csISOLatinHebrew iso-ir-138"},
    {28599, "ISO-8859-9",     "latin5 ISO_8859-9 ISO-8859-9 csISOLatin5 iso-ir-148 l5"},
    {-1,    "",                ""},                                          // ISO-8859-10 unused
    {-1,    "",                ""},                                          // ISO-8859-11 unused
    {28603, "ISO-8859-13",    "ISO_8859-13 ISO-8859-13"},
    {28604, "ISO-8859-14",    "iso-celtic latin8 ISO_8859-14 ISO-8859-14 18 iso-ir-199"},
    {28605, "ISO-8859-15",    "Latin-9 ISO_8859-15 ISO-8859-15"},
    {-1,    "",                ""},                                          // ISO-8859-16 unused
    {437,   "CP437",          "IBM437 cp437 437 csPC8CodePage437"},
    {720,   "CP720",          "IBM720 cp720 oem720 720"},
    {737,   "CP737",          "IBM737 cp737 oem737 737"},
    {775,   "CP775",          "IBM775 cp775 oem775 775"},
    {850,   "CP850",          "IBM850 cp850 oem850 850"},
    {852,   "CP852",          "IBM852 cp852 oem852 852"},
    {855,   "CP855",          "IBM855 cp855 oem855 855 csIBM855"},
    {857,   "CP857",          "IBM857 cp857 oem857 857"},
    {858,   "CP858",          "IBM858 cp858 oem858 858"},
    {860,   "CP860",          "IBM860 cp860 oem860 860"},
    {861,   "CP861",          "IBM861 cp861 oem861 861"},
    {862,   "CP862",          "IBM862 cp862 oem862 862"},
    {863,   "CP863",          "IBM863 cp863 oem863 863"},
    {865,   "CP865",          "IBM865 cp865 oem865 865"},
    {866,   "CP866",          "IBM866 cp866 oem866 866"},
    {869,   "CP869",          "IBM869 cp869 oem869 869"},
    {950,   "big5",           "big5 csBig5"},
    {936,   "GBK",             "gb2312 gbk csGB2312 gb18030"},
    {932,   "Shift_JIS",      "Shift_JIS MS_Kanji csShiftJIS csWindows31J"},
    {949,   "windows-949",    "windows-949 korean"},
    {51949, "euc-kr",         "euc-kr csEUCKR"},
    {874,   "windows-874",    "tis-620"},
    {10007, "x-mac-cyrillic", "x-mac-cyrillic xmaccyrillic"},
    {21866, "KOI8-U",         "koi8_u"},
    {20866, "KOI8-R",         "koi8_r csKOI8R"},
};

static constexpr size_t NUM_ENCODINGS = sizeof(codePageTable) / sizeof(codePageTable[0]);

// Case-insensitive alias matching (replaces Win32 _stricmp)
static bool isInAliasList(const QString& token, const char* list)
{
    if (token.isEmpty() || !list || !list[0])
        return false;

    const QStringList aliases = QString::fromLatin1(list).split(' ',
        Qt::SkipEmptyParts);
    for (const QString& alias : aliases) {
        if (alias.compare(token, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

EncodingMapper& EncodingMapper::getInstance()
{
    static EncodingMapper instance;
    return instance;
}

int EncodingMapper::getEncodingFromIndex(int index) const
{
    if (index < 0 || static_cast<size_t>(index) >= NUM_ENCODINGS)
        return -1;
    return codePageTable[index].codePage;
}

int EncodingMapper::getIndexFromEncoding(int encoding) const
{
    if (encoding == -1)
        return -1;
    for (size_t i = 0; i < NUM_ENCODINGS; ++i) {
        if (codePageTable[i].codePage == encoding)
            return static_cast<int>(i);
    }
    return -1;
}

int EncodingMapper::getEncodingFromString(const char* encodingAlias) const
{
    if (!encodingAlias)
        return -1;

    const QString alias = QString::fromLatin1(encodingAlias).trimmed();

    // UTF-8 short-circuit
    if (alias.compare(u"utf-8"_s, Qt::CaseInsensitive) == 0 ||
        alias.compare(u"utf8"_s, Qt::CaseInsensitive) == 0)
        return SC_CP_UTF8;

    // Search all entries
    for (const auto& entry : codePageTable) {
        if (entry.codePage == -1)
            continue;
        if (isInAliasList(alias, entry.aliasList))
            return entry.codePage;
    }
    return -1;
}

QString EncodingMapper::qtEncodingFromCodePage(int codePage)
{
    if (codePage == SC_CP_UTF8)
        return u"UTF-8"_s;
    for (const auto& entry : codePageTable) {
        if (entry.codePage == codePage && entry.qtName[0])
            return QString::fromLatin1(entry.qtName);
    }
    // Fallback: return "CP" + number
    if (codePage > 0)
        return u"CP%1"_s.arg(codePage);
    return u"ISO-8859-1"_s;  // fallback
}

int EncodingMapper::codePageFromQtEncoding(const QString& encoding)
{
    for (const auto& entry : codePageTable) {
        if (entry.codePage == -1)
            continue;
        if (QString::fromLatin1(entry.aliasList).split(' ',
                Qt::SkipEmptyParts).contains(encoding, Qt::CaseInsensitive))
            return entry.codePage;
    }
    // Try direct codepage parsing
    if (encoding.startsWith(u"CP"_s, Qt::CaseInsensitive)) {
        bool ok = false;
        int cp = encoding.mid(2).toInt(&ok);
        if (ok) return cp;
    }
    return -1;
}
