// npp-qt: UDL XML parser — converts Notepad++ User Defined Language XML files
// into structured data for use with LexerUDL (QsciLexerCustom subclass).
//
// Original UDL schema (Win32 N++): defined in PowerEditor/src/ScintillaComponent/UserDefineDialog.h
// This parser handles the UDL XML format (udlVersion 2.1) and extracts:
//   - Language name and settings
//   - Keyword sets (up to 9 named sets, each a space-separated keyword list)
//   - Named styles with foreground/background colors and font styling
//   - Delimiter rules
//
// The parsed data is consumed by LexerUDL which performs the actual highlighting
// via QsciLexerCustom::styleText().
#pragma once

#include <QColor>
#include <QFont>
#include <QString>
#include <QStringList>
#include <QList>
#include <QXmlStreamReader>

// ============================================================================
// UDL data structures
// ============================================================================

// UdlKeywordSet — a named keyword set from the UDL XML
struct UdlKeywordSet {
    QString name;           // e.g. "Keywords1", "Operators1", "Comments"
    int setIndex = -1;     // keyword set index (0-8), -1 = not set
    QStringList keywords;   // space-separated keywords split into a list
    bool isPrefix = false;  // true = match keywords that are prefixes only
};

// UdlStyle — a named style (foreground/background color, font)
struct UdlStyle {
    int styleId = -1;      // Scintilla style index (0-31)
    QString name;          // e.g. "KEYWORDS1", "DEFAULT", "COMMENTS"
    QColor fgColor;        // foreground color
    QColor bgColor;        // background color
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool hidden = false;   // fontStyle & 8
    QString fontName;       // empty = use default
    int fontSize = 0;       // 0 = use default
};

// UdlDelimiter — delimiter pair for highlighting (e.g. string delimiters)
struct UdlDelimiter {
    int open = -1;         // opening char index
    int close = -1;        // closing char index
    int escape = -1;       // escape char index (-1 if none)
    bool isMultiLine = false;
};

// UdlRule — a begin/end regex rule for region-based highlighting
struct UdlRule {
    int styleId = -1;      // which UdlStyle to apply
    QString beginRE;        // begin regex pattern
    QString endRE;          // end regex pattern (empty = single-line)
    bool onlyAtLineStart = false;
};

// UdlNumberFormat — describes how numbers are formatted
struct UqlNumberFormat {
    QString prefix1;       // e.g. "0x" for hex
    QString prefix2;
    QString suffix1;       // e.g. "L" for long
    QString suffix2;
    QString extras1;       // extra chars allowed in numbers
    QString extras2;
    QString range;         // e.g. "01" for binary
};

// ============================================================================
// UserLangParser — parses UDL XML files
// ============================================================================

class UserLangParser {
public:
    UserLangParser() = default;

    // Load a UDL XML file. Returns true on success.
    bool loadFromFile(const QString& path);

    // Load from a QByteArray (e.g. loaded from resource or QRC)
    bool loadFromData(const QByteArray& data);

    // Language metadata
    QString languageName() const { return _name; }
    QString languageExt() const { return _ext; }
    QString udlVersion() const { return _udlVersion; }
    bool isDarkModeTheme() const { return _isDarkModeTheme; }

    // Settings
    bool isCaseIgnored() const { return _isCaseIgnored; }
    int forcePureLC() const { return _forcePureLC; }  // 0=none, 1=lower, 2=upper
    int decimalSeparator() const { return _decimalSeparator; }  // 0=dot, 1=comma
    bool allowFoldOfComments() const { return _allowFoldOfComments; }
    bool foldCompact() const { return _foldCompact; }

    // Keyword sets (up to 9)
    QList<UdlKeywordSet> keywordSets() const { return _keywordSets; }
    const UdlKeywordSet* keywordSetByIndex(int idx) const;
    const UdlKeywordSet* keywordSetByName(const QString& name) const;

    // Styles (up to 32)
    QList<UdlStyle> styles() const { return _styles; }
    const UdlStyle* styleById(int id) const;
    const UdlStyle* styleByName(const QString& name) const;

    // Delimiters (6 pairs)
    QList<UdlDelimiter> delimiters() const { return _delimiters; }

    // Number format
    UqlNumberFormat numberFormat() const { return _numberFormat; }

    // Rules (regex-based begin/end regions)
    QList<UdlRule> rules() const { return _rules; }

    // Error message
    QString errorString() const { return _error; }

    // Check if valid
    bool isValid() const { return !_name.isEmpty(); }

private:
    // XML parsing helpers
    void parseRoot(QXmlStreamReader& xml);
    void parseUserLang(QXmlStreamReader& xml);
    void parseSettings(QXmlStreamReader& xml);
    void parseKeywordLists(QXmlStreamReader& xml);
    void parseStyles(QXmlStreamReader& xml);
    void parseRules(QXmlStreamReader& xml);

    // Helper: parse a hex color string like "0080FF" into QColor
    QColor parseColor(const QString& hex);

    // Helper: map keyword list name to set index (0-8) and whether it's a prefix
    std::pair<int, bool> mapKeywordListName(const QString& name);

    // Helper: map WordsStyle name to style ID
    int mapStyleNameToId(const QString& name);

    // State
    QString _name;
    QString _ext;
    QString _udlVersion;
    bool _isDarkModeTheme = false;

    bool _isCaseIgnored = false;
    int _forcePureLC = 0;
    int _decimalSeparator = 0;
    bool _allowFoldOfComments = false;
    bool _foldCompact = false;

    QList<UdlKeywordSet> _keywordSets;
    QList<UdlStyle> _styles;
    QList<UdlDelimiter> _delimiters;
    UqlNumberFormat _numberFormat;
    QList<UdlRule> _rules;

    QString _error;
};
