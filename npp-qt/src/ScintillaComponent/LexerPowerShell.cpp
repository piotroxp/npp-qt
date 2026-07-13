// Custom QsciLexer for PowerShell
// Full hand-written tokenizer for PowerShell syntax:
//   - Variables: $varname (alphanumeric + underscore)
//   - Operators: -eq -ne -gt -lt -match -replace -like -contains -in -notin -band -bor -bxor ...
//   - Cmdlets: Verb-Noun pattern (Get-Item, Set-Content, Where-Object, ForEach-Object, ...)
//   - Comments: # (single-line) and <# #> (block)
//   - Strings: "..." '...' @"..."@ @'...'@ (here-strings)

#include "LexerPowerShell.h"

#include <Qsci/qsciScintilla.h>

#include <QColor>
#include <QFont>

// ============================================================================
// Construction
// ============================================================================

LexerPowerShell::LexerPowerShell(QObject* parent)
    : QsciLexerCustom(parent)
{
    // Set 0: PowerShell keywords
    const QString kw = QStringLiteral(
        "begin break catch class continue data default do dynamicparam else elseif end "
        "exit filter finally for foreach from function if in inlinescript param process "
        "return switch throw trap try until while"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    // Set 1: PowerShell cmdlet verbs
    // These are matched as part of Verb-Noun cmdlet names (e.g. Get-Item, Set-Content)
    const QString verbs = QStringLiteral(
        "Get Set New Remove Add Clear Close Copy Compare Compress "
        "Convert ConvertFrom ConvertTo Debug Disconnect Edit Enable "
        "Enter Exit Expand Export Find Format Group Hide Import "
        "Initialize Invoke Join Limit Lock Measure Move Out Pop "
        "Protect Push Read Receive Redo Register Rename Reset "
        "Resize Restart Resume Save Search Select Show Skip Sort "
        "Split Step Stop Submit Suspend Switch Sync Test Trace "
        "Unblock Undo Uninstall Union Unlock Unregister Update "
        "Use Wait Watch Write"
    );
    for (const QString& v : verbs.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _cmdlets.insert(v);

    // PowerShell operators
    const QString ops = QStringLiteral(
        "-eq -ne -gt -lt -ge -le -match -notmatch -replace -like -notlike "
        "-contains -notcontains -in -notin -band -bor -bxor -bnot "
        "-split -join -is -isnot -as -and -or -not -band -bor "
        "-plus -minus -multiply -divide -modulo"
    );
    for (const QString& o : ops.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _operators.insert(o);

    // Built-in constants (also used as bare identifiers, but recognized here)
    _variables.insert(QStringLiteral("true"));
    _variables.insert(QStringLiteral("false"));
    _variables.insert(QStringLiteral("null"));
}

LexerPowerShell::~LexerPowerShell()
{
}

// ============================================================================
// QsciLexerCustom interface
// ============================================================================

QColor LexerPowerShell::defaultColor(int style) const
{
    switch (style) {
        case Comment:       return QColor(0x60, 0x60, 0x60);  // dark grey
        case String:        return QColor(0x8B, 0x00, 0x00);  // dark red
        case Keyword:       return QColor(0x00, 0x00, 0x8B);  // dark blue
        case Cmdlet:        return QColor(0x80, 0x00, 0x80);  // purple
        case Variable:      return QColor(0x00, 0x64, 0x00);  // dark green
        case OperatorPS:    return QColor(0x8B, 0x00, 0x8B);  // dark magenta
        case Number:        return QColor(0x00, 0x8B, 0x8B);  // dark cyan
        case HereString:    return QColor(0x00, 0x80, 0x80);  // dark teal
        case BlockComment:  return QColor(0x60, 0x60, 0x60);  // grey
        default:            return QColor(0x00, 0x00, 0x00);  // black
    }
}

QFont LexerPowerShell::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment || style == BlockComment) {
        f.setItalic(true);
    }
    return f;
}

QString LexerPowerShell::description(int style) const
{
    switch (style) {
        case Default:       return tr("Default");
        case Comment:       return tr("Comment");
        case String:        return tr("String");
        case Keyword:       return tr("Keyword");
        case Cmdlet:        return tr("Cmdlet");
        case Variable:      return tr("Variable");
        case OperatorPS:    return tr("Operator");
        case Number:        return tr("Number");
        case HereString:    return tr("Here-String");
        case BlockComment:  return tr("Block Comment");
        default:            return QString();
    }
}

// ============================================================================
// Main styling entry point
// ============================================================================

void LexerPowerShell::styleText(int start, int end)
{
    QsciScintilla* sci = qobject_cast<QsciScintilla*>(editor());
    if (!sci)
        return;

    const int docLen = sci->length();
    if (start >= docLen || start >= end)
        return;
    end = qMin(end, docLen);

    // Get text as a QString for Unicode processing
    // sci->text(start, end) returns QByteArray, convert to QString
    QByteArray raw = sci->text(start, end);
    QString txt = QString::fromUtf8(raw);

    if (txt.isEmpty())
        return;

    // Get the starting line to align styling positions correctly
    int lineStart = sci->lineFromPosition(start);
    int segStart = sci->positionFromLine(lineStart);
    int segLen = end - segStart;

    if (segLen <= 0)
        return;

    // Build style byte array (one byte per character, default = Default)
    QByteArray styleBytes(segLen, static_cast<char>(Default));

    // Track block comment state across calls
    static bool inBlockComment = false;
    static int blockCommentStart = 0;

    const int txtLen = txt.length();

    int pos = 0;
    while (pos < txtLen) {
        QChar ch = txt.at(pos);

        // ---- Block comment: <# ... #> ----
        if (inBlockComment) {
            int endPos = txt.indexOf(QLatin1String("#>"), pos);
            if (endPos < 0) {
                // Rest of text is block comment
                for (int i = pos; i < txtLen; ++i)
                    styleBytes[i] = BlockComment;
                break;
            } else {
                int endTag = endPos + 2;
                for (int i = pos; i < endTag; ++i)
                    styleBytes[i] = BlockComment;
                pos = endTag;
                inBlockComment = false;
                continue;
            }
        }

        // ---- Single-line comment: # ----
        if (ch == QLatin1Char('#')) {
            // Style to end of line
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\n'))
                styleBytes[pos++] = Comment;
            continue;
        }

        // ---- Block comment start: <# ----
        if (ch == QLatin1Char('<') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('#')) {
            int endPos = txt.indexOf(QLatin1String("#>"), pos + 2);
            if (endPos < 0) {
                // No closing tag in this segment
                inBlockComment = true;
                for (int i = pos; i < txtLen; ++i)
                    styleBytes[i] = BlockComment;
                break;
            } else {
                int endTag = endPos + 2;
                for (int i = pos; i < endTag; ++i)
                    styleBytes[i] = BlockComment;
                pos = endTag;
                continue;
            }
        }

        // ---- Here-string: @"..."@ or @'...'@ ----
        if (ch == QLatin1Char('@') && pos + 2 < txtLen) {
            QChar next1 = txt.at(pos + 1);
            QChar next2 = txt.at(pos + 2);
            if ((next1 == QLatin1Char('"') || next1 == QLatin1Char('\'')) &&
                next2 == QLatin1Char('@')) {
                QChar closeCh = (next1 == QLatin1Char('"'))
                                    ? QLatin1Char('@')
                                    : QLatin1Char('\'');
                int searchPos = pos + 3;
                bool found = false;
                while (searchPos + 1 < txtLen) {
                    if (txt.at(searchPos) == closeCh && txt.at(searchPos + 1) == QLatin1Char('@')) {
                        found = true;
                        int endTag = searchPos + 2;
                        for (int i = pos; i < endTag; ++i)
                            styleBytes[i] = HereString;
                        pos = endTag;
                        break;
                    }
                    ++searchPos;
                }
                if (found)
                    continue;
            }
        }

        // ---- Double-quoted string: "..." ----
        if (ch == QLatin1Char('"')) {
            int strStart = pos;
            ++pos;
            while (pos < txtLen) {
                QChar c = txt.at(pos);
                if (c == QLatin1Char('`')) {
                    // Escape sequence: include the backtick
                    ++pos;
                    if (pos < txtLen)
                        ++pos;
                    continue;
                }
                if (c == QLatin1Char('"'))
                    break;
                if (c == QLatin1Char('\n')) {
                    // Unterminated string
                    break;
                }
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('"'))
                ++pos;
            for (int i = strStart; i < pos && i < txtLen; ++i)
                styleBytes[i] = String;
            continue;
        }

        // ---- Single-quoted string: '...' ----
        if (ch == QLatin1Char('\'')) {
            int strStart = pos;
            ++pos;
            while (pos < txtLen) {
                QChar c = txt.at(pos);
                if (c == QLatin1Char('`')) {
                    // Escape sequence: include the backtick
                    ++pos;
                    if (pos < txtLen)
                        ++pos;
                    continue;
                }
                if (c == QLatin1Char('\''))
                    break;
                if (c == QLatin1Char('\n')) {
                    // Unterminated string
                    break;
                }
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('\''))
                ++pos;
            for (int i = strStart; i < pos && i < txtLen; ++i)
                styleBytes[i] = String;
            continue;
        }

        // ---- Variable: $varname ----
        if (ch == QLatin1Char('$')) {
            int varStart = pos;
            ++pos;
            // Variable name: alphanumeric + underscore
            while (pos < txtLen && isWordChar(txt.at(pos)))
                ++pos;
            // Handle ${varname} syntax
            if (pos < txtLen && txt.at(pos) == QLatin1Char('{')) {
                ++pos;
                while (pos < txtLen && txt.at(pos) != QLatin1Char('}') && txt.at(pos) != QLatin1Char('\n'))
                    ++pos;
                if (pos < txtLen && txt.at(pos) == QLatin1Char('}'))
                    ++pos;
            }
            for (int i = varStart; i < pos && i < txtLen; ++i)
                styleBytes[i] = Variable;
            continue;
        }

        // ---- PowerShell operator: -word ----
        if (ch == QLatin1Char('-') && pos + 1 < txtLen && txt.at(pos + 1).isLetter()) {
            int opStart = pos;
            ++pos;
            while (pos < txtLen && isWordChar(txt.at(pos)))
                ++pos;
            QString op = txt.mid(opStart, pos - opStart);
            if (_operators.contains(op)) {
                for (int i = opStart; i < pos && i < txtLen; ++i)
                    styleBytes[i] = OperatorPS;
                continue;
            }
            // Not an operator, fall through to identifier handling
            pos = opStart;
            // advance past the dash
            ++pos;
        }

        // ---- Number ----
        if (ch.isDigit() || (ch == QLatin1Char('.') && pos + 1 < txtLen && txt.at(pos + 1).isDigit())) {
            int numStart = pos;
            bool hasDecimal = (ch == QLatin1Char('.'));
            ++pos;
            while (pos < txtLen) {
                QChar c = txt.at(pos);
                if (c.isDigit()) {
                    ++pos;
                } else if (c == QLatin1Char('.') && !hasDecimal) {
                    hasDecimal = true;
                    ++pos;
                } else if (c == QLatin1Char('x') || c == QLatin1Char('X') ||
                           c == QLatin1Char('b') || c == QLatin1Char('B')) {
                    // Hex/binary literal (e.g., 0xFF, 0b1010)
                    ++pos;
                    while (pos < txtLen && (txt.at(pos).isDigit() ||
                               (txt.at(pos) >= QLatin1Char('a') && txt.at(pos) <= QLatin1Char('f')) ||
                               (txt.at(pos) >= QLatin1Char('A') && txt.at(pos) <= QLatin1Char('F'))))
                        ++pos;
                    break;
                } else {
                    break;
                }
            }
            for (int i = numStart; i < pos && i < txtLen; ++i)
                styleBytes[i] = Number;
            continue;
        }

        // ---- Identifier / keyword / cmdlet ----
        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int identStart = pos;
            while (pos < txtLen && isWordChar(txt.at(pos)))
                ++pos;
            int identLen = pos - identStart;

            // Check if this is a cmdlet (Verb-Noun pattern with hyphen)
            if (pos < txtLen && txt.at(pos) == QLatin1Char('-')) {
                int hyphenPos = pos;
                ++pos;
                int nounStart = pos;
                while (pos < txtLen && isWordChar(txt.at(pos)))
                    ++pos;
                if (pos > nounStart) {
                    QString verb = txt.mid(identStart, identLen);
                    if (_cmdlets.contains(verb)) {
                        // Style the whole Verb-Noun as cmdlet
                        for (int i = identStart; i < pos && i < txtLen; ++i)
                            styleBytes[i] = Cmdlet;
                        continue;
                    }
                }
                // Not a known cmdlet verb, just style identifier
                for (int i = identStart; i < hyphenPos + 1 && i < txtLen; ++i)
                    styleBytes[i] = Default;
                continue;
            }

            // Regular identifier
            QString word = txt.mid(identStart, identLen);
            bool styled = false;
            if (_keywords.contains(word)) {
                for (int i = identStart; i < pos && i < txtLen; ++i)
                    styleBytes[i] = Keyword;
                styled = true;
            } else if (_variables.contains(word)) {
                for (int i = identStart; i < pos && i < txtLen; ++i)
                    styleBytes[i] = Variable;
                styled = true;
            }
            // else: leave as Default
            continue;
        }

        // ---- Everything else: skip ----
        ++pos;
    }

    // Write styled bytes back to Scintilla
    int i = 0;
    while (i < segLen && i < styleBytes.size()) {
        unsigned char style = static_cast<unsigned char>(styleBytes[i]);
        int runStart = i;
        while (i < segLen && i < styleBytes.size() &&
               static_cast<unsigned char>(styleBytes[i]) == style) {
            ++i;
        }
        int length = i - runStart;
        int position = segStart + runStart;
        sci->SendScintilla(QsciScintilla::SCI_STARTSTYLING, position, 0x1F);
        sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, length, style);
    }
}
