// npp-qt: UDL XML parser implementation
// See UserLangParser.h for details.
#include "UserLangParser.h"

#include <QColor>
#include <QFile>
#include <QXmlStreamReader>

// ============================================================================
// Utility
// ============================================================================

QColor UserLangParser::parseColor(const QString& hex)
{
    if (hex.isEmpty())
        return QColor();
    // Hex colors are 6 hex digits: RRGGBB
    // Notepad++ uses lowercase or uppercase hex
    bool ok = false;
    int rgb = hex.toInt(&ok, 16);
    if (!ok || hex.length() != 6)
        return QColor();
    int r = (rgb >> 16) & 0xFF;
    int g = (rgb >> 8) & 0xFF;
    int b = rgb & 0xFF;
    return QColor(r, g, b);
}

// Map keyword list name → (setIndex, isPrefix)
// Returns (-1, false) if not a recognized keyword list name.
std::pair<int, bool> UserLangParser::mapKeywordListName(const QString& name)
{
    // Format: "Keywords1", "Keywords2", ... "Keywords8", "Operators1", "Comments", etc.
    // Also: "Numbers, prefix1", "Numbers, extras1", etc.
    if (name == QLatin1String("Comments")) {
        return {0, false};
    } else if (name == QLatin1String("Numbers, prefix1")) {
        return {1, false};
    } else if (name == QLatin1String("Numbers, prefix2")) {
        return {2, false};
    } else if (name == QLatin1String("Numbers, extras1")) {
        return {3, false};
    } else if (name == QLatin1String("Numbers, extras2")) {
        return {4, false};
    } else if (name == QLatin1String("Numbers, suffix1")) {
        return {5, false};
    } else if (name == QLatin1String("Numbers, suffix2")) {
        return {6, false};
    } else if (name == QLatin1String("Numbers, range")) {
        return {7, false};
    } else if (name == QLatin1String("Operators1")) {
        return {8, false};
    } else if (name == QLatin1String("Operators2")) {
        return {9, false};
    } else if (name.startsWith(QLatin1String("Folders in code1, open"))) {
        return {10, false};
    } else if (name.startsWith(QLatin1String("Folders in code1, middle"))) {
        return {11, false};
    } else if (name.startsWith(QLatin1String("Folders in code1, close"))) {
        return {12, false};
    } else if (name.startsWith(QLatin1String("Folders in code2, open"))) {
        return {13, false};
    } else if (name.startsWith(QLatin1String("Folders in code2, middle"))) {
        return {14, false};
    } else if (name.startsWith(QLatin1String("Folders in code2, close"))) {
        return {15, false};
    } else if (name.startsWith(QLatin1String("Folders in comment, open"))) {
        return {16, false};
    } else if (name.startsWith(QLatin1String("Folders in comment, middle"))) {
        return {17, false};
    } else if (name.startsWith(QLatin1String("Folders in comment, close"))) {
        return {18, false};
    } else if (name == QLatin1String("Keywords1")) {
        return {20, false};
    } else if (name == QLatin1String("Keywords2")) {
        return {21, false};
    } else if (name == QLatin1String("Keywords3")) {
        return {22, false};
    } else if (name == QLatin1String("Keywords4")) {
        return {23, false};
    } else if (name == QLatin1String("Keywords5")) {
        return {24, false};
    } else if (name == QLatin1String("Keywords6")) {
        return {25, false};
    } else if (name == QLatin1String("Keywords7")) {
        return {26, false};
    } else if (name == QLatin1String("Keywords8")) {
        return {27, false};
    } else if (name == QLatin1String("Delimiters")) {
        // Delimiters are handled separately in parseStyles via WordsStyle
        return {-1, false};
    }
    return {-1, false};
}

// Map WordsStyle name → Scintilla style ID
// These IDs match Scintilla's SCE_USER_* constants used by the N++ UDL lexer.
int UserLangParser::mapStyleNameToId(const QString& name)
{
    // Map the UI name from the XML to the internal style ID.
    // These must match ScintillaEditView's UDL style → style ID mapping.
    if (name == QLatin1String("DEFAULT"))          return 0;
    else if (name == QLatin1String("COMMENTS"))    return 1;
    else if (name == QLatin1String("LINE COMMENTS")) return 2;
    else if (name == QLatin1String("NUMBERS"))     return 3;
    else if (name == QLatin1String("KEYWORDS1"))   return 4;
    else if (name == QLatin1String("KEYWORDS2"))   return 5;
    else if (name == QLatin1String("KEYWORDS3"))   return 6;
    else if (name == QLatin1String("KEYWORDS4"))   return 7;
    else if (name == QLatin1String("KEYWORDS5"))   return 8;
    else if (name == QLatin1String("KEYWORDS6"))   return 9;
    else if (name == QLatin1String("KEYWORDS7"))   return 10;
    else if (name == QLatin1String("KEYWORDS8"))   return 11;
    else if (name == QLatin1String("OPERATORS"))   return 12;
    else if (name == QLatin1String("FOLDER IN CODE1"))  return 13;
    else if (name == QLatin1String("FOLDER IN CODE2"))  return 14;
    else if (name == QLatin1String("FOLDER IN COMMENT")) return 15;
    else if (name == QLatin1String("DELIMITERS1")) return 16;
    else if (name == QLatin1String("DELIMITERS2")) return 17;
    else if (name == QLatin1String("DELIMITERS3")) return 18;
    else if (name == QLatin1String("DELIMITERS4")) return 19;
    else if (name == QLatin1String("DELIMITERS5")) return 20;
    else if (name == QLatin1String("DELIMITERS6")) return 21;
    else if (name == QLatin1String("DELIMITERS7")) return 22;
    else if (name == QLatin1String("DELIMITERS8")) return 23;
    else if (name == QLatin1String("COMMENT"))    return 1;   // alias
    else if (name == QLatin1String("NUMBER"))     return 3;   // alias
    else if (name == QLatin1String("STRING"))      return 16;  // alias (first delimiter)
    else if (name == QLatin1String("STRING1"))     return 16;
    else if (name == QLatin1String("STRING2"))     return 17;
    else if (name == QLatin1String("STRING3"))     return 18;
    else if (name == QLatin1String("STRING4"))     return 19;
    else if (name == QLatin1String("STRING5"))     return 20;
    // Check for "KEYWORDS9" through "KEYWORDS32" (rare but possible)
    for (int i = 9; i <= 32; ++i) {
        if (name == QStringLiteral("KEYWORDS%1").arg(i))
            return 3 + i;  // IDs after the fixed ones
    }
    return -1;
}

// ============================================================================
// Load from file
// ============================================================================

bool UserLangParser::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _error = QStringLiteral("Cannot open file: %1").arg(path);
        return false;
    }
    return loadFromData(file.readAll());
}

bool UserLangParser::loadFromData(const QByteArray& data)
{
    QXmlStreamReader xml(data);
    if (xml.hasError()) {
        _error = QStringLiteral("XML parse error: %1").arg(xml.errorString());
        return false;
    }

    // Reset state
    _name.clear();
    _ext.clear();
    _udlVersion.clear();
    _isDarkModeTheme = false;
    _isCaseIgnored = false;
    _forcePureLC = 0;
    _decimalSeparator = 0;
    _allowFoldOfComments = false;
    _foldCompact = false;
    _keywordSets.clear();
    _styles.clear();
    _delimiters.clear();
    _rules.clear();
    _numberFormat = {};
    _error.clear();

    // Parse root element
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("NotepadPlus")) {
                parseRoot(xml);
            }
        }
    }

    if (xml.hasError()) {
        _error = QStringLiteral("XML error at line %1: %2")
                     .arg(xml.lineNumber())
                     .arg(xml.errorString());
        return false;
    }

    return isValid();
}

// ============================================================================
// XML parsing helpers
// ============================================================================

void UserLangParser::parseRoot(QXmlStreamReader& xml)
{
    // Root: <NotepadPlus>
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("UserLang")) {
                parseUserLang(xml);
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("NotepadPlus"))
                break;
        }
    }
}

void UserLangParser::parseUserLang(QXmlStreamReader& xml)
{
    // Attributes: name, ext, udlVersion, darkModeTheme
    const QXmlStreamAttributes attrs = xml.attributes();

    _name = attrs.value(QLatin1String("name")).toString();
    _ext  = attrs.value(QLatin1String("ext")).toString();
    _udlVersion = attrs.value(QLatin1String("udlVersion")).toString();
    _isDarkModeTheme = attrs.value(QLatin1String("darkModeTheme")) == QLatin1String("yes");

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("Settings")) {
                parseSettings(xml);
            } else if (ename == QLatin1String("KeywordLists")) {
                parseKeywordLists(xml);
            } else if (ename == QLatin1String("Styles")) {
                parseStyles(xml);
            } else if (ename == QLatin1String("Rules")) {
                parseRules(xml);
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("UserLang"))
                break;
        }
    }
}

void UserLangParser::parseSettings(QXmlStreamReader& xml)
{
    // <Settings>
    //   <Global caseIgnored="no" allowFoldOfComments="no" foldCompact="no"
    //            forcePureLC="0" decimalSeparator="0" />
    //   <Prefix Keywords1="no" Keywords2="no" ... Keywords8="no" />
    // </Settings>
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("Global")) {
                QXmlStreamAttributes attrs = xml.attributes();
                _isCaseIgnored = (attrs.value(QLatin1String("caseIgnored")) == QLatin1String("yes"));
                _allowFoldOfComments = (attrs.value(QLatin1String("allowFoldOfComments")) == QLatin1String("yes"));
                _foldCompact = (attrs.value(QLatin1String("foldCompact")) == QLatin1String("yes"));
                _forcePureLC = attrs.value(QLatin1String("forcePureLC")).toString().toInt();
                _decimalSeparator = attrs.value(QLatin1String("decimalSeparator")).toString().toInt();
            } else if (ename == QLatin1String("Prefix")) {
                // <Prefix Keywords1="no" Keywords2="yes" ... />
                QXmlStreamAttributes attrs = xml.attributes();
                for (UdlKeywordSet& ks : _keywordSets) {
                    QStringRef attrVal = attrs.value(ks.name);
                    if (!attrVal.isNull()) {
                        ks.isPrefix = (attrVal == QLatin1String("yes"));
                    }
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("Settings"))
                break;
        }
    }
}

void UserLangParser::parseKeywordLists(QXmlStreamReader& xml)
{
    // <KeywordLists>
    //   <Keywords name="Comments">00-- 01 02/* 03*/ 04</Keywords>
    //   <Keywords name="Keywords1">SELECT FROM WHERE ...</Keywords>
    //   <Keywords name="Delimiters">00" 01\ 02" 03</Keywords>
    // </KeywordLists>

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("Keywords")) {
                QXmlStreamAttributes attrs = xml.attributes();
                QString kName = attrs.value(QLatin1String("name")).toString();
                QString kValue = xml.readElementText(QXmlStreamReader::SkipChildElements).trimmed();

                // Check for Delimiters
                if (kName == QLatin1String("Delimiters")) {
                    // Format: "OO CC EE OO CC EE ..." or "OO CC EE OO"
                    // where OO=open char code, CC=escape char code, EE=close char code
                    // Indices 0-5 for first pair, 6-11 for second, etc.
                    // Each entry is 2 hex digits (ASCII code).
                    // Format in the XML text:
                    //   "index0_open index0_escape index0_close index1_open index1_escape index1_close ..."
                    // But Notepad++ encodes it as pairs of quoted chars with indices.
                    // Actually the format is: "index0 openChar index0 escape index0 close index1 openChar ..."
                    // Looking at the actual XML: 00" 01\ 02" 03
                    // 00 = pair 0, opening char = "
                    // 01 = pair 0, escape char = \
                    // 02 = pair 0, closing char = "
                    // 03 = pair 1, opening char = (blank / unescaped)
                    // etc.

                    QStringList parts = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    UdlDelimiter delim;
                    int pairIndex = -1;
                    for (const QString& part : parts) {
                        if (part.length() < 2)
                            continue;
                        // First 2 chars are the index (e.g., "00", "01", "02")
                        bool ok = false;
                        int idx = part.left(2).toInt(&ok, 10);
                        if (!ok)
                            continue;
                        // The rest is the character value (or empty for blank)
                        QString val = part.length() > 2 ? part.mid(2) : QString();
                        if (val.isEmpty())
                            val = QString();

                        int pair = idx / 3;  // 3 entries per pair
                        int role = idx % 3;  // 0=open, 1=escape, 2=close

                        if (pair != pairIndex) {
                            if (delim.open >= 0 || delim.close >= 0) {
                                _delimiters.append(delim);
                            }
                            delim = UdlDelimiter();
                            pairIndex = pair;
                        }

                        if (role == 0) {
                            delim.open = val.isEmpty() ? -1 : val[0].unicode();
                        } else if (role == 1) {
                            delim.escape = val.isEmpty() ? -1 : val[0].unicode();
                        } else if (role == 2) {
                            delim.close = val.isEmpty() ? -1 : val[0].unicode();
                        }
                    }
                    // Append last delimiter
                    if (delim.open >= 0 || delim.close >= 0) {
                        _delimiters.append(delim);
                    }
                    continue;
                }

                // Check for Numbers format
                if (kName == QLatin1String("Numbers, prefix1")) {
                    _numberFormat.prefix1 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 1;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, prefix2")) {
                    _numberFormat.prefix2 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 2;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, extras1")) {
                    _numberFormat.extras1 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 3;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, extras2")) {
                    _numberFormat.extras2 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 4;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, suffix1")) {
                    _numberFormat.suffix1 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 5;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, suffix2")) {
                    _numberFormat.suffix2 = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 6;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else if (kName == QLatin1String("Numbers, range")) {
                    _numberFormat.range = kValue;
                    UdlKeywordSet ks;
                    ks.name = kName;
                    ks.setIndex = 7;
                    ks.keywords = kValue.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    _keywordSets.append(ks);
                } else {
                    // Standard keyword list
                    auto [setIdx, isPrefix] = mapKeywordListName(kName);
                    if (setIdx >= 0) {
                        UdlKeywordSet ks;
                        ks.name = kName;
                        ks.setIndex = setIdx;
                        // Split keywords by whitespace
                        ks.keywords = kValue.split(QRegExp(QStringLiteral("\\s+")),
                                                   Qt::SkipEmptyParts);
                        // Check if any keyword contains the prefix marker
                        // Notepad++ uses "00keyword" style prefixes in some lists
                        // (e.g., Comments: "00// 01 02/* 03*/ 04")
                        // Here the raw text is stored as-is; isPrefix is set in <Prefix>
                        // element.
                        _keywordSets.append(ks);
                    }
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("KeywordLists"))
                break;
        }
    }
}

void UserLangParser::parseStyles(QXmlStreamReader& xml)
{
    // <Styles>
    //   <WordsStyle name="DEFAULT" fgColor="000000" bgColor="FFFFFF"
    //               colorStyle="0" fontName="" fontStyle="0" nesting="0" />
    // </Styles>
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            const QStringRef ename = xml.name();
            if (ename == QLatin1String("WordsStyle")) {
                QXmlStreamAttributes attrs = xml.attributes();
                QString styleName = attrs.value(QLatin1String("name")).toString();

                // Skip unnamed or empty styles
                if (styleName.isEmpty())
                    continue;

                int styleId = mapStyleNameToId(styleName);
                // If name is not a known style ID, assign next available ID
                if (styleId < 0) {
                    styleId = 24 + _styles.size();  // start at 24 for custom styles
                }

                UdlStyle st;
                st.styleId = styleId;
                st.name = styleName;
                st.fgColor = parseColor(attrs.value(QLatin1String("fgColor")).toString());
                st.bgColor = parseColor(attrs.value(QLatin1String("bgColor")).toString());

                // fontStyle: 0=none, 1=bold, 2=italic, 3=bold+italic,
                //            4=underline, 5=bold+underline, 6=italic+underline, 7=bold+italic+underline, 8=hidden
                int fontStyle = attrs.value(QLatin1String("fontStyle")).toString().toInt();
                st.bold      = (fontStyle & 1) != 0;
                st.italic    = (fontStyle & 2) != 0;
                st.underline = (fontStyle & 4) != 0;
                st.hidden    = (fontStyle & 8) != 0;

                QString fn = attrs.value(QLatin1String("fontName")).toString();
                if (!fn.isEmpty())
                    st.fontName = fn;

                int fs = attrs.value(QLatin1String("fontSize")).toString().toInt();
                if (fs > 0)
                    st.fontSize = fs;

                _styles.append(st);
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("Styles"))
                break;
        }
    }
}

void UserLangParser::parseRules(QXmlStreamReader& xml)
{
    // <Rules>
    //   <Rule .../>
    // </Rules>
    // (Notepad++ UDL supports begin/end regex rules via the Rules section)
    Q_UNUSED(xml);
    // Note: The preinstalled UDL files don't contain Rules elements,
    // but the full UDL schema supports them.
    // For now, rules are handled as follows:
    // - Keyword-based highlighting is done by matching keywords in styleText()
    // - Delimiter-based highlighting (strings) is done by matching delimiter pairs
    // - Advanced rules (begin/end regions) would need full regex region support
    //   which can be added as needed.
}

// ============================================================================
// Lookup helpers
// ============================================================================

const UdlKeywordSet* UserLangParser::keywordSetByIndex(int idx) const
{
    for (const UdlKeywordSet& ks : _keywordSets) {
        if (ks.setIndex == idx)
            return &ks;
    }
    return nullptr;
}

const UdlKeywordSet* UserLangParser::keywordSetByName(const QString& name) const
{
    for (const UdlKeywordSet& ks : _keywordSets) {
        if (ks.name == name)
            return &ks;
    }
    return nullptr;
}

const UdlStyle* UserLangParser::styleById(int id) const
{
    for (const UdlStyle& st : _styles) {
        if (st.styleId == id)
            return &st;
    }
    return nullptr;
}

const UdlStyle* UserLangParser::styleByName(const QString& name) const
{
    for (const UdlStyle& st : _styles) {
        if (st.name == name)
            return &st;
    }
    return nullptr;
}
