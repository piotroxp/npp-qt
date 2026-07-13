// Custom QsciLexer for Nim
// Syntax: proc, template, macro, {.pragma.}, import, type, let/var/const

#include "LexerNim.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerNim::LexerNim(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "addr and as asm atomic bind block break case cast concept const "
        "continue converter defer discard distinct do elif else end enum "
        "except export finally for from func if import in include "
        "interface iterator let macro method mixin nil not object of or "
        "out proc ptr raise ref result return static struct template try "
        "tuple type using var when while with without yield"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString ty = QStringLiteral(
        "int int8 int16 int32 int64 uint uint8 uint16 uint32 uint64 "
        "float float32 float64 bool char string cstring pointer "
        "array seq set openarray varargs pointer proc ref object "
        "typedesc auto voidSome None Some"
    );
    for (const QString& t : ty.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _types.insert(t);

    const QString bi = QStringLiteral(
        "echo new len cap abs min max chr ord pred succ inc dec shl shr "
        "newSeq open close readFile writeFile quit inc dec "
        "unsafeAddr unsafeNew raise div mod"
    );
    for (const QString& b : bi.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _builtins.insert(b);
}

LexerNim::~LexerNim() {}

QColor LexerNim::defaultColor(int style) const
{
    switch (style) {
        case Comment:     return QColor(0x60, 0x60, 0x60);
        case String:      return QColor(0x8B, 0x00, 0x00);
        case Keyword:     return QColor(0x00, 0x00, 0x8B);
        case Number:      return QColor(0x00, 0x8B, 0x8B);
        case Pragma:      return QColor(0xCC, 0x7F, 0x00);
        case Type:        return QColor(0x00, 0x64, 0x00);
        case Procedure:   return QColor(0x80, 0x00, 0x80);
        case Builtin:     return QColor(0x80, 0x00, 0x64);
        default:          return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerNim::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerNim::description(int style) const
{
    switch (style) {
        case Default:     return tr("Default");
        case Comment:     return tr("Comment");
        case String:      return tr("String");
        case Keyword:     return tr("Keyword");
        case Number:      return tr("Number");
        case Pragma:     return tr("Pragma");
        case Type:        return tr("Type");
        case Procedure:  return tr("Procedure");
        case Builtin:    return tr("Builtin");
        default:          return QString();
    }
}

void LexerNim::styleText(int start, int end)
{
    QsciScintilla* sci = qobject_cast<QsciScintilla*>(editor());
    if (!sci) return;
    const int docLen = sci->length();
    if (start >= docLen || start >= end) return;
    end = qMin(end, docLen);

    QByteArray raw = sci->text(start, end);
    QString txt = QString::fromUtf8(raw);
    if (txt.isEmpty()) return;

    int lineStart = sci->lineFromPosition(start);
    int segStart = sci->positionFromLine(lineStart);
    int segLen = end - segStart;
    if (segLen <= 0) return;

    QByteArray styleBytes(segLen, static_cast<char>(Default));
    const int txtLen = txt.length();

    int pos = 0;
    while (pos < txtLen) {
        QChar ch = txt.at(pos);

        if (ch == QLatin1Char('#')) {
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\n'))
                styleBytes[pos++] = Comment;
            continue;
        }

        if (ch == QLatin1Char('{') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('.')) {
            int s = pos;
            pos += 2;
            while (pos + 1 < txtLen && txt.at(pos) != QLatin1Char('.') || txt.at(pos + 1) != QLatin1Char('}')) {
                if (txt.at(pos) == QLatin1Char('}')) { ++pos; break; }
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('.')) ++pos;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('}')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Pragma;
            continue;
        }

        if (ch == QLatin1Char('"')) {
            int s = pos++;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('"') && txt.at(pos) != QLatin1Char('\n')) {
                if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('"')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('r') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('"')) {
            int s = pos;
            pos += 2;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('"') && txt.at(pos) != QLatin1Char('\n')) ++pos;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('"')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('"') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('"') && pos + 2 < txtLen && txt.at(pos + 2) == QLatin1Char('"')) {
            int s = pos;
            pos += 3;
            while (pos + 2 < txtLen) {
                if (txt.at(pos) == QLatin1Char('"') && txt.at(pos + 1) == QLatin1Char('"') && txt.at(pos + 2) == QLatin1Char('"')) {
                    pos += 3; break;
                }
                ++pos;
            }
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('\'')) {
            int s = pos++;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\'') && txt.at(pos) != QLatin1Char('\n')) ++pos;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('\'')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch.isDigit()) {
            int s = pos;
            if (ch == QLatin1Char('0') && pos + 1 < txtLen) {
                QChar nxt = txt.at(pos + 1);
                if (nxt == QLatin1Char('x') || nxt == QLatin1Char('X')) { ++pos; ++pos;
                    while (pos < txtLen && (txt.at(pos).isDigit() ||
                               (txt.at(pos) >= QLatin1Char('a') && txt.at(pos) <= QLatin1Char('f')) ||
                               (txt.at(pos) >= QLatin1Char('A') && txt.at(pos) <= QLatin1Char('F')))) ++pos;
                    for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number; continue;
                }
                if (nxt == QLatin1Char('b') || nxt == QLatin1Char('B')) { ++pos; ++pos;
                    while (pos < txtLen && (txt.at(pos) == QLatin1Char('0') || txt.at(pos) == QLatin1Char('1'))) ++pos;
                    for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number; continue;
                }
            }
            ++pos;
            while (pos < txtLen && (txt.at(pos).isDigit() || txt.at(pos) == QLatin1Char('.') ||
                       txt.at(pos) == QLatin1Char('_') || txt.at(pos) == QLatin1Char('\''))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            QString w = txt.mid(s, pos - s);
            if (_keywords.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Keyword;
            } else if (_types.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Type;
            } else if (_builtins.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Builtin;
            } else if (w == QStringLiteral("proc") || w == QStringLiteral("method") ||
                       w == QStringLiteral("iterator") || w == QStringLiteral("converter")) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Procedure;
            }
            continue;
        }

        ++pos;
    }

    int i = 0;
    while (i < segLen && i < styleBytes.size()) {
        unsigned char style = static_cast<unsigned char>(styleBytes[i]);
        int runStart = i;
        while (i < segLen && i < styleBytes.size() && static_cast<unsigned char>(styleBytes[i]) == style) ++i;
        sci->SendScintilla(QsciScintilla::SCI_STARTSTYLING, segStart + runStart, 0x1F);
        sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, i - runStart, style);
    }
}
