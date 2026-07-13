// Custom QsciLexer for GraphQL
// Syntax: query, mutation, subscription, type, fragment, input, enum, union

#include "LexerGraphQL.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerGraphQL::LexerGraphQL(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "query mutation subscription fragment extend directive "
        "type input enum union interface scalar implements "
        "schema directivelocation on "
        "true false null "
        "QUERY MUTATION SUBSCRIPTION FRAGMENT EXTEND DIRECTIVE "
        "TYPE INPUT ENUM UNION INTERFACE SCALAR IMPLEMENTS "
        "SCHEMA DIRECTIVELOCATION ON"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString ty = QStringLiteral(
        "String Int Float Boolean ID "
        "Query Mutation Subscription __Schema __Type __Field __InputValue __EnumValue"
    );
    for (const QString& t : ty.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _types.insert(t);
}

LexerGraphQL::~LexerGraphQL() {}

QColor LexerGraphQL::defaultColor(int style) const
{
    switch (style) {
        case Comment:   return QColor(0x60, 0x60, 0x60);
        case String:   return QColor(0x8B, 0x00, 0x00);
        case Keyword:   return QColor(0x00, 0x00, 0x8B);
        case TypeName: return QColor(0x00, 0x64, 0x00);
        case Field:     return QColor(0x00, 0x00, 0x00);
        case Argument: return QColor(0x80, 0x00, 0x80);
        case Variable:  return QColor(0xCC, 0x7F, 0x00);
        case Directive: return QColor(0x80, 0x00, 0x80);
        default:        return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerGraphQL::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerGraphQL::description(int style) const
{
    switch (style) {
        case Default:   return tr("Default");
        case Comment:   return tr("Comment");
        case String:    return tr("String");
        case Keyword:   return tr("Keyword");
        case TypeName: return tr("Type Name");
        case Field:     return tr("Field");
        case Argument: return tr("Argument");
        case Variable:  return tr("Variable");
        case Directive: return tr("Directive");
        default:        return QString();
    }
}

void LexerGraphQL::styleText(int start, int end)
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

        if (ch == QLatin1Char('"')) {
            if (pos + 2 < txtLen && txt.at(pos + 1) == QLatin1Char('"') && txt.at(pos + 2) == QLatin1Char('"')) {
                int s = pos;
                pos += 3;
                while (pos + 2 < txtLen) {
                    if (txt.at(pos) == QLatin1Char('"') && txt.at(pos + 1) == QLatin1Char('"') && txt.at(pos + 2) == QLatin1Char('"')) {
                        pos += 3; break;
                    }
                    ++pos;
                }
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            } else {
                int s = pos++;
                while (pos < txtLen && txt.at(pos) != QLatin1Char('"') && txt.at(pos) != QLatin1Char('\n')) {
                    if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                    ++pos;
                }
                if (pos < txtLen && txt.at(pos) == QLatin1Char('"')) ++pos;
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            }
            continue;
        }

        if (ch == QLatin1Char('$')) {
            int s = pos++;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Variable;
            continue;
        }

        if (ch == QLatin1Char('@')) {
            int s = pos++;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Directive;
            continue;
        }

        if (ch == QLatin1Char(':') && pos + 1 < txtLen && !txt.at(pos + 1).isSpace()) {
            int s = pos++;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Argument;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            QString w = txt.mid(s, pos - s);
            // Type names start with uppercase
            if (!w.isEmpty() && w.at(0).isUpper()) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = TypeName;
            } else if (_keywords.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Keyword;
            } else if (_types.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = TypeName;
            } else if (pos < txtLen && txt.at(pos) == QLatin1Char('(')) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Field;
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
