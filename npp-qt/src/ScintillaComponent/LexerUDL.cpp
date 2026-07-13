// npp-qt: LexerUDL implementation
// See LexerUDL.h for details.
#include "LexerUDL.h"
#include "UserLangParser.h"

#include <Qsci/qsciScintilla.h>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

// ============================================================================
// Construction
// ============================================================================

LexerUDL::LexerUDL(const UserLangParser& parser, QObject* parent)
    : QsciLexerCustom(parent), _parser(parser)
{
    // Set case sensitivity from parser settings
    _caseSensitivity = parser.isCaseIgnored() ? Qt::CaseInsensitive : Qt::CaseSensitive;

    buildKeywordSets();
    buildDelimiters();

    // Build number regex pattern
    // Format: [prefix][digits][extras][suffix]
    // e.g. 0x[0-9A-Fa-f]+L for C-style hex long
    rebuildNumberRegex();
}

LexerUDL::~LexerUDL() = default;

void LexerUDL::rebuildNumberRegex()
{
    // Build a regex for number detection based on UDL number format.
    // The UDL number format uses keywords to specify allowed prefixes/suffixes.
    // Example: prefix1="0x", suffix1="L", extras1="_"
    const UqlNumberFormat& nf = _parser.numberFormat();

    QString prefixPart;
    if (!nf.prefix1.isEmpty() || !nf.prefix2.isEmpty()) {
        QStringList prefixes;
        if (!nf.prefix1.isEmpty()) prefixes.append(QRegularExpression::escape(nf.prefix1));
        if (!nf.prefix2.isEmpty()) prefixes.append(QRegularExpression::escape(nf.prefix2));
        prefixPart = QStringLiteral("(?:%1)?").arg(prefixes.join(QLatin1Char('|')));
    }

    QString suffixPart;
    if (!nf.suffix1.isEmpty() || !nf.suffix2.isEmpty()) {
        QStringList suffixes;
        if (!nf.suffix1.isEmpty()) suffixes.append(QRegularExpression::escape(nf.suffix1));
        if (!nf.suffix2.isEmpty()) suffixes.append(QRegularExpression::escape(nf.suffix2));
        suffixPart = QStringLiteral("(?:%1)*").arg(suffixes.join(QLatin1Char('|')));
    }

    QString extrasPart;
    if (!nf.extras1.isEmpty() || !nf.extras2.isEmpty()) {
        QStringList extras;
        if (!nf.extras1.isEmpty()) extras.append(QRegularExpression::escape(nf.extras1));
        if (!nf.extras2.isEmpty()) extras.append(QRegularExpression::escape(nf.extras2));
        extrasPart = QStringLiteral("[%1]*").arg(extras.join());
    }

    // Basic number pattern: optional prefix + digits with optional extras + optional suffix
    // Accepts decimal, hex (0x), octal, binary (0b) with the right prefixes
    QString pattern = QStringLiteral(R"((?:%1)[0-9a-zA-Z]%2%3)")
                          .arg(prefixPart)
                          .arg(extrasPart)
                          .arg(suffixPart);

    // Try to compile; fall back to a simple number pattern on failure
    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);
    if (re.isValid()) {
        _numberRegex = re;
    } else {
        // Fallback: simple decimal number
        _numberRegex = QRegularExpression(QStringLiteral(R"(\b\d+(\.\d+)?[LlFfDd]?\b)"));
    }
}

// ============================================================================
// Build keyword sets and delimiters from parser
// ============================================================================

void LexerUDL::buildKeywordSets()
{
    _keywordSets.clear();
    _commentStyle = CommentStyle();

    for (const UdlKeywordSet& ks : _parser.keywordSets()) {
        KeywordSetData kd;
        kd.isPrefix = ks.isPrefix;
        kd.caseIgnored = _parser.isCaseIgnored();

        for (const QString& kw : ks.keywords) {
            if (kw.isEmpty())
                continue;
            // Skip comment marker codes like "00", "01", "02" (prefix indices)
            // These appear in Comments keyword lists
            if (kw.length() <= 2 && kw[0].isDigit())
                continue;

            if (kd.caseIgnored) {
                kd.keywords.insert(kw.toLower());
            } else {
                kd.keywords.insert(kw);
            }
            kd.rawKeywords += kw + QLatin1Char(' ');
        }

        // Extract comment markers from Comments keyword list
        if (ks.name == QLatin1String("Comments")) {
            // The Comments keyword list format:
            // "00<line-comment-prefix> 01 <EOL-marker> 02<block-comment-start> 03<block-comment-end> 04"
            // e.g. "--  02/* 03*/ " for SQL
            // e.g. "#  02/* 03*/ " for Bash
            const QString& raw = ks.keywords.join(QLatin1Char(' '));
            QStringList parts = raw.split(QRegExp(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            // Parts like "00--", "02/*", "03*/"
            for (const QString& part : parts) {
                if (part.length() < 2)
                    continue;
                bool ok = false;
                int idx = part.left(2).toInt(&ok, 10);
                if (!ok)
                    continue;
                QString content = part.mid(2);
                if (idx == 0 && !content.isEmpty()) {
                    _commentStyle.lineComment = content;
                    _commentStyle.hasLineComment = true;
                } else if (idx == 2 && !content.isEmpty()) {
                    _commentStyle.blockCommentStart = content;
                    _commentStyle.hasBlockComment = true;
                } else if (idx == 3 && !content.isEmpty()) {
                    _commentStyle.blockCommentEnd = content;
                }
            }
        }

        if (kd.keywords.isEmpty() && ks.setIndex != 0)
            continue;  // skip empty non-keyword sets

        // Map to the correct style ID
        int styleId = -1;
        switch (ks.setIndex) {
            case 0:  styleId = UdlStyleId::Comments; break;
            case 8:  styleId = UdlStyleId::Operators; break;
            case 10: styleId = UdlStyleId::FolderCode1; break;
            case 11: styleId = UdlStyleId::FolderCode1; break; // middle — use same
            case 12: styleId = UdlStyleId::FolderCode1; break; // close — use same
            case 13: styleId = UdlStyleId::FolderCode2; break;
            case 14: styleId = UdlStyleId::FolderCode2; break;
            case 15: styleId = UdlStyleId::FolderCode2; break;
            case 16: styleId = UdlStyleId::FolderComment; break;
            case 17: styleId = UdlStyleId::FolderComment; break;
            case 18: styleId = UdlStyleId::FolderComment; break;
            case 20: styleId = UdlStyleId::Keywords1; break;
            case 21: styleId = UdlStyleId::Keywords2; break;
            case 22: styleId = UdlStyleId::Keywords3; break;
            case 23: styleId = UdlStyleId::Keywords4; break;
            case 24: styleId = UdlStyleId::Keywords5; break;
            case 25: styleId = UdlStyleId::Keywords6; break;
            case 26: styleId = UdlStyleId::Keywords7; break;
            case 27: styleId = UdlStyleId::Keywords8; break;
            default: break;
        }

        if (styleId >= 0) {
            _keywordSets[styleId] = kd;
        }
    }
}

void LexerUDL::buildDelimiters()
{
    _delimiters.clear();

    const QList<UdlDelimiter> delims = _parser.delimiters();
    int delimStyleId = UdlStyleId::Delimiters1;

    for (const UdlDelimiter& d : delims) {
        if (delimStyleId > UdlStyleId::Delimiters8)
            break;
        if (d.open < 0 && d.close < 0)
            continue;

        DelimiterData dd;
        dd.open = (d.open >= 0) ? QChar(d.open) : QChar();
        dd.close = (d.close >= 0) ? QChar(d.close) : QChar();
        dd.escape = (d.escape >= 0) ? QChar(d.escape) : QChar();
        dd.styleId = delimStyleId++;
        // String-like delimiters: same open and close char (e.g. "..." or '...')
        // Bracket-like: different chars (e.g. [...] or {...])
        dd.isString = (d.open >= 0 && d.close >= 0 && d.open == d.close);
        _delimiters.append(dd);
    }
}

// ============================================================================
// QsciLexerCustom interface
// ============================================================================

QColor LexerUDL::defaultColor(int style) const
{
    const UdlStyle* st = _parser.styleById(style);
    if (st && st->fgColor.isValid())
        return st->fgColor;
    return QsciLexerCustom::defaultColor(style);
}

QFont LexerUDL::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    const UdlStyle* st = _parser.styleById(style);
    if (st) {
        if (!st->fontName.isEmpty())
            f.setFamily(st->fontName);
        if (st->fontSize > 0)
            f.setPointSize(st->fontSize);
        f.setBold(st->bold);
        f.setItalic(st->italic);
        f.setUnderline(st->underline);
    }
    return f;
}

QString LexerUDL::description(int style) const
{
    const UdlStyle* st = _parser.styleById(style);
    if (st)
        return st->name;
    return QStringLiteral("Style %1").arg(style);
}

// ============================================================================
// Main styling entry point
// ============================================================================

void LexerUDL::styleText(int start, int end)
{
    // Retrieve the editor widget
    QsciScintilla* edit = editor();
    if (!edit)
        return;

    // Get the text segment to style
    // Scintilla text positions are in bytes for non-Unicode, but QsciScintilla
    // provides UTF-8 positions.
    const int docLen = edit->length();
    if (start >= docLen || start >= end)
        return;

    end = qMin(end, docLen);

    // Fetch text as QString (Unicode)
    // QsciScintilla::text() returns the whole document; we use SendScintilla
    // to get a range.
    QString text;
    {
        QByteArray bytes = edit->text(start, end - 1);
        text = QString::fromUtf8(bytes);
    }

    if (text.isEmpty())
        return;

    // Start styling from the start of the line containing 'start'
    // This ensures proper handling of line-based constructs (line comments).
    int lineStart = edit->lineFromPosition(start);
    int segmentStart = edit->positionFromLine(lineStart);

    // Build a working buffer for style bytes (default style = 0)
    // Each style byte corresponds to one character in the text.
    int segmentLen = end - segmentStart;
    QByteArray styleBytes(segmentLen, 0);

    // Track nesting state: are we inside a block comment?
    bool inBlockComment = false;
    int blockCommentStart = 0;
    int blockCommentEnd = 0;

    // Track nesting for delimiter pairs (simple: no nested string literals)
    QVector<int> delimStack;  // stack of (position, delimiterIndex)

    // Pass 1: Apply base styles (keywords, numbers, operators)
    applyKeywords(segmentStart, segmentLen, text, styleBytes);

    // Pass 2: Apply delimiter/string highlighting (override keywords)
    applyDelimiters(segmentStart, segmentLen, text, styleBytes, delimStack);

    // Pass 3: Apply comment highlighting (highest priority, overrides all)
    applyComments(segmentStart, segmentLen, text, styleBytes, inBlockComment, blockCommentStart, blockCommentEnd);

    // Pass 4: Apply number highlighting
    applyNumbers(segmentStart, segmentLen, text, styleBytes);

    // Write styles back to Scintilla
    // SCI_SETSTYLINGEX: style a range of text with a byte array of style bytes.
    // We use individual SCI_STARTSTYLING + SCI_SETSTYLING calls per style change.
    applyStyledBytes(segmentStart, segmentLen, styleBytes);
}

// ============================================================================
// Apply keyword-based highlighting
// ============================================================================

void LexerUDL::applyKeywords(int segStart, int segLen,
                               const QString& text,
                               QByteArray& styleBytes)
{
    Q_UNUSED(segStart);
    Q_UNUSED(segLen);

    // For each keyword set, find all occurrences in the text.
    for (auto it = _keywordSets.constBegin(); it != _keywordSets.constEnd(); ++it) {
        int styleId = it.key();
        const KeywordSetData& kd = it.value();

        if (kd.keywords.isEmpty())
            continue;

        // Skip comment styles (handled separately)
        if (styleId == UdlStyleId::Comments ||
            styleId == UdlStyleId::LineComments)
            continue;

        // For each keyword, scan the text for matches.
        // For prefix keywords (e.g., Markdown link prefixes), match at word start.
        // For normal keywords, match as whole words.
        for (const QString& kw : kd.keywords) {
            if (kw.isEmpty())
                continue;

            if (kd.isPrefix) {
                // Prefix keyword: matches at the start of a word
                QRegularExpression re(
                    QStringLiteral("\\b%1\\b").arg(QRegularExpression::escape(kw)),
                    _caseSensitivity == Qt::CaseInsensitive
                        ? QRegularExpression::CaseInsensitiveOption
                        : QRegularExpression::NoPatternOption);
                QRegularExpressionMatchIterator it2 = re.globalMatch(text);
                while (it2.hasNext()) {
                    QRegularExpressionMatch m = it2.next();
                    int pos = m.capturedStart();
                    int len = m.capturedLength();
                    for (int i = 0; i < len && (pos + i) < styleBytes.size(); ++i) {
                        if (styleBytes[pos + i] == 0)
                            styleBytes[pos + i] = static_cast<char>(styleId);
                    }
                }
            } else {
                // Whole-word match using word boundary
                QRegularExpression re(
                    QStringLiteral("\\b%1\\b").arg(QRegularExpression::escape(kw)),
                    _caseSensitivity == Qt::CaseInsensitive
                        ? QRegularExpression::CaseInsensitiveOption
                        : QRegularExpression::NoPatternOption);
                QRegularExpressionMatchIterator it2 = re.globalMatch(text);
                while (it2.hasNext()) {
                    QRegularExpressionMatch m = it2.next();
                    int pos = m.capturedStart();
                    int len = m.capturedLength();
                    for (int i = 0; i < len && (pos + i) < styleBytes.size(); ++i) {
                        if (styleBytes[pos + i] == 0)
                            styleBytes[pos + i] = static_cast<char>(styleId);
                    }
                }
            }
        }
    }
}

// ============================================================================
// Apply comment highlighting
// ============================================================================

void LexerUDL::applyComments(int segStart, int segLen,
                             const QString& text,
                             QByteArray& styleBytes,
                             bool& inBlockComment,
                             int& blockCommentStart,
                             int& blockCommentEnd)
{
    Q_UNUSED(segStart);
    Q_UNUSED(blockCommentEnd);

    const CommentStyle& cs = _commentStyle;

    if (cs.hasLineComment) {
        // Style all text from line comment prefix to end of line
        QStringList lines = text.split(QLatin1Char('\n'));
        int pos = 0;
        for (const QString& line : lines) {
            if (line.startsWith(cs.lineComment, _caseSensitivity)) {
                int matchLen = line.length();
                for (int i = 0; i < matchLen && pos + i < styleBytes.size(); ++i) {
                    styleBytes[pos + i] = UdlStyleId::Comments;
                }
            }
            pos += line.length() + 1; // +1 for '\n'
        }
    }

    if (cs.hasBlockComment) {
        // Find all block comment regions
        QRegularExpression re(
            QRegularExpression::escape(cs.blockCommentStart) +
            QStringLiteral("(.*?)") +
            QRegularExpression::escape(cs.blockCommentEnd),
            QRegularExpression::DotMatchesEverythingOption |
            (_caseSensitivity == Qt::CaseInsensitive
                ? QRegularExpression::CaseInsensitiveOption
                : QRegularExpression::NoPatternOption));

        QRegularExpressionMatchIterator it = re.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            int start = m.capturedStart();
            int end = m.capturedEnd();
            for (int i = start; i < end && i < styleBytes.size(); ++i) {
                styleBytes[i] = UdlStyleId::Comments;
            }
        }
    }
}

// ============================================================================
// Apply delimiter (string/bracket) highlighting
// ============================================================================

void LexerUDL::applyDelimiters(int segStart, int segLen,
                                const QString& text,
                                QByteArray& styleBytes,
                                QVector<int>& /*delimStack*/)
{
    Q_UNUSED(segStart);
    Q_UNUSED(delimStack);

    for (const DelimiterData& dd : _delimiters) {
        QChar openChar = dd.open;
        QChar closeChar = dd.close;
        QChar escapeChar = dd.escape;

        if (openChar.isNull() && closeChar.isNull())
            continue;

        bool stringMode = !openChar.isNull() && openChar == closeChar;

        if (stringMode) {
            // String literal mode: style content between matching delimiters
            // E.g. "hello world" with escape character \
            int i = 0;
            while (i < text.length()) {
                if (!openChar.isNull() && text[i] == openChar) {
                    // Start of string
                    int stringStart = i;
                    bool escaped = false;
                    ++i;
                    // Scan to closing delimiter or end of text
                    while (i < text.length()) {
                        if (escaped) {
                            escaped = false;
                            ++i;
                            continue;
                        }
                        if (!escapeChar.isNull() && text[i] == escapeChar) {
                            escaped = true;
                            ++i;
                            continue;
                        }
                        if (!closeChar.isNull() && text[i] == closeChar) {
                            // Style the whole string including delimiters
                            for (int j = stringStart; j <= i && j < styleBytes.size(); ++j) {
                                if (styleBytes[j] == 0 || styleBytes[j] == UdlStyleId::Comments)
                                    styleBytes[j] = static_cast<char>(dd.styleId);
                            }
                            ++i;
                            break;
                        }
                        ++i;
                    }
                } else {
                    ++i;
                }
            }
        } else {
            // Bracket/operator mode: style individual delimiter characters
            for (int i = 0; i < text.length() && i < styleBytes.size(); ++i) {
                if (!openChar.isNull() && text[i] == openChar) {
                    if (styleBytes[i] == 0)
                        styleBytes[i] = static_cast<char>(dd.styleId);
                }
                if (!closeChar.isNull() && text[i] == closeChar) {
                    if (styleBytes[i] == 0)
                        styleBytes[i] = static_cast<char>(dd.styleId);
                }
            }
        }
    }
}

// ============================================================================
// Apply number highlighting
// ============================================================================

void LexerUDL::applyNumbers(int segStart, int segLen,
                             const QString& text,
                             QByteArray& styleBytes)
{
    Q_UNUSED(segStart);

    if (_numberRegex.pattern().isEmpty())
        return;

    QRegularExpressionMatchIterator it = _numberRegex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        int pos = m.capturedStart();
        int len = m.capturedLength();
        for (int i = 0; i < len && (pos + i) < styleBytes.size(); ++i) {
            // Only apply if not already styled as a comment or string
            if (styleBytes[pos + i] == 0)
                styleBytes[pos + i] = UdlStyleId::Numbers;
        }
    }
}

// ============================================================================
// Write styled bytes back to Scintilla
// ============================================================================

void LexerUDL::applyStyledBytes(int segStart, int segLen, const QByteArray& styleBytes)
{
    QsciScintilla* edit = editor();
    if (!edit)
        return;

    // Scintilla's SCI_SETSTYLINGEX takes a pointer and length.
    // Each byte in the style array corresponds to one character position.
    // We need to call SCI_STARTSTYLING at segStart, then SCI_SETSTYLING
    // for each contiguous run of the same style.
    int i = 0;
    while (i < segLen && i < styleBytes.size()) {
        unsigned char style = static_cast<unsigned char>(styleBytes[i]);
        int runStart = i;
        while (i < segLen && i < styleBytes.size() &&
               static_cast<unsigned char>(styleBytes[i]) == style) {
            ++i;
        }
        // Apply styling from runStart to i (length = i - runStart)
        int length = i - runStart;
        int position = segStart + runStart;
        edit->SendScintilla(QsciScintilla::SCI_STARTSTYLING, position, style);
        edit->SendScintilla(QsciScintilla::SCI_SETSTYLING, length, style);
    }
}

// ============================================================================
// Folding support
// ============================================================================

int LexerUDL::braceFlag(int style) const
{
    Q_UNUSED(style);
    // Return SC_FOLDFLAG_* constants for folder markers.
    // Currently returns 0 (no folder markers in default implementation).
    return 0;
}
