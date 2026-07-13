// Custom QsciLexer for Zig
// Syntax: fn, struct, enum, const, var, comptime, defer, @builtin

#include "LexerZig.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerZig::LexerZig(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "addr and anyframe anytype asm await break catch comptime "
        "const continue defer else enum errdefer export extern "
        "false fn for if inline noalias noinline nosuspend null "
        "or pub return struct suspend switch test threadlocal true "
        "try type undefined union unreachable usingnamespace var volatile while"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString ty = QStringLiteral(
        "void bool u8 u16 u32 u64 u128 usize "
        "i8 i16 i32 i64 i128 isize f16 f32 f64 f128 "
        "c_char c_short c_ushort c_int c_uint c_long c_ulong "
        "c_longlong c_ulonglong c_longdouble c_void anyopaque "
        "type noreturn anyerror promise"
    );
    for (const QString& t : ty.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _types.insert(t);

    const QString bi = QStringLiteral(
        "@addWithOverflow @alignOf @as @bitCast @breakpoint @byteSwap "
        "@divExact @divFloor @divTrunc @embedFile @enumToInt @fence "
        "@frameAddress @frameSize @import @intCast @intToFloat "
        "@max @min @mod @panic @popCount @ptrCast @ptrToInt @rem "
        "@resume @sizeOf @sqrt @sqrt @strCat @strCmp @test @this "
        "@truncate @try @typeInfo @typeName @unionInit @vectorType @volatile"
    );
    for (const QString& b : bi.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _builtins.insert(b);
}

LexerZig::~LexerZig() {}

QColor LexerZig::defaultColor(int style) const
{
    switch (style) {
        case Comment:  return QColor(0x60, 0x60, 0x60);
        case String:   return QColor(0x8B, 0x00, 0x00);
        case Keyword:  return QColor(0x00, 0x00, 0x8B);
        case Number:   return QColor(0x00, 0x8B, 0x8B);
        case Builtin:  return QColor(0xCC, 0x7F, 0x00);
        case Type:     return QColor(0x00, 0x64, 0x00);
        case Attribute: return QColor(0x80, 0x00, 0x80);
        case Function: return QColor(0x80, 0x00, 0x80);
        default:       return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerZig::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerZig::description(int style) const
{
    switch (style) {
        case Default:   return tr("Default");
        case Comment:   return tr("Comment");
        case String:    return tr("String");
        case Keyword:   return tr("Keyword");
        case Number:    return tr("Number");
        case Builtin:   return tr("Builtin");
        case Type:     return tr("Type");
        case Attribute: return tr("Attribute");
        case Function:  return tr("Function");
        default:        return QString();
    }
}

void LexerZig::styleText(int start, int end)
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

        if (ch == QLatin1Char('/') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('/')) {
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\n'))
                styleBytes[pos++] = Comment;
            continue;
        }

        if (ch == QLatin1Char('/') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('*')) {
            int s = pos; pos += 2;
            while (pos + 1 < txtLen) {
                if (txt.at(pos) == QLatin1Char('*') && txt.at(pos + 1) == QLatin1Char('/')) { pos += 2; break; }
                ++pos;
            }
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Comment;
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

        if (ch == QLatin1Char('@')) {
            int s = pos++;
            while (pos < txtLen && (txt.at(pos).isLetterOrNumber() || txt.at(pos) == QLatin1Char('_'))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Builtin;
            continue;
        }

        if (ch.isDigit()) {
            int s = pos;
            ++pos;
            while (pos < txtLen && (txt.at(pos).isDigit() || txt.at(pos) == QLatin1Char('.') ||
                       txt.at(pos) == QLatin1Char('_') || txt.at(pos) == QLatin1Char('e') ||
                       txt.at(pos) == QLatin1Char('E'))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && (txt.at(pos).isLetterOrNumber() || txt.at(pos) == QLatin1Char('_'))) ++pos;
            QString w = txt.mid(s, pos - s);
            if (_keywords.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Keyword;
            } else if (_types.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Type;
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
