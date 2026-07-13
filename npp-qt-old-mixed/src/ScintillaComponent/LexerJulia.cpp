// Custom QsciLexer for Julia
// Syntax: function, module, using, import, @macro, ::type, # comments

#include "LexerJulia.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerJulia::LexerJulia(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "begin break catch continue do else elseif end export finally "
        "for function global if import in let local macro module "
        "mutable return struct try using where while "
        "abstract type alias bitstype primitive "
        "true false nothing missing undef "
        "const global local baremodule isa where"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString fns = QStringLiteral(
        "println print map reduce collect zip enumerate "
        "filter findfirst findall getindex setindex push! pop! "
        "size length keys values pairs eachindex "
        "reshape transpose inv det eigvals svd qr lu chol "
        "rand randn randint shuffle split join replace "
        "parse float int string Symbol Array Vector Matrix Dict Set Tuple NamedTuple "
        "open close read write readline readlines countlines "
        "typeof convert promote eltype length size ndims "
        "Threads.@spawn @threads @parallel @async"
    );
    for (const QString& f : fns.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _functions.insert(f);
}

LexerJulia::~LexerJulia() {}

QColor LexerJulia::defaultColor(int style) const
{
    switch (style) {
        case Comment:        return QColor(0x60, 0x60, 0x60);
        case String:         return QColor(0x8B, 0x00, 0x00);
        case Keyword:        return QColor(0x00, 0x00, 0x8B);
        case Number:         return QColor(0x00, 0x8B, 0x8B);
        case Macro:          return QColor(0xCC, 0x7F, 0x00);
        case TypeAnnotation: return QColor(0x00, 0x64, 0x00);
        case Operator:       return QColor(0x80, 0x00, 0x80);
        case Function:       return QColor(0x80, 0x00, 0x80);
        default:             return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerJulia::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerJulia::description(int style) const
{
    switch (style) {
        case Default:         return tr("Default");
        case Comment:         return tr("Comment");
        case String:          return tr("String");
        case Keyword:         return tr("Keyword");
        case Number:          return tr("Number");
        case Macro:           return tr("Macro");
        case TypeAnnotation:  return tr("Type Annotation");
        case Operator:        return tr("Operator");
        case Function:        return tr("Function");
        default:              return QString();
    }
}

void LexerJulia::styleText(int start, int end)
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
            int s = pos;
            if (pos + 2 < txtLen && txt.at(pos + 1) == QLatin1Char('"') && txt.at(pos + 2) == QLatin1Char('"')) {
                pos += 3;
                while (pos + 2 < txtLen) {
                    if (txt.at(pos) == QLatin1Char('"') && txt.at(pos + 1) == QLatin1Char('"') && txt.at(pos + 2) == QLatin1Char('"')) {
                        pos += 3; break;
                    }
                    ++pos;
                }
            } else {
                ++pos;
                while (pos < txtLen && txt.at(pos) != QLatin1Char('"') && txt.at(pos) != QLatin1Char('\n')) {
                    if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                    ++pos;
                }
                if (pos < txtLen && txt.at(pos) == QLatin1Char('"')) ++pos;
            }
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('\'')) {
            int s = pos++;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\'') && txt.at(pos) != QLatin1Char('\n')) {
                if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('\'')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('@')) {
            int s = pos++;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Macro;
            continue;
        }

        if (ch == QLatin1Char(':') && pos + 1 < txtLen && !txt.at(pos + 1).isSpace()) {
            int s = pos++;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch.isDigit() || (ch == QLatin1Char('.') && pos + 1 < txtLen && txt.at(pos + 1).isDigit())) {
            int s = pos;
            ++pos;
            while (pos < txtLen) {
                QChar c = txt.at(pos);
                if (c == QLatin1Char('.')) { ++pos; continue; }
                if (c == QLatin1Char('e') || c == QLatin1Char('E') || c == QLatin1Char('f')) { ++pos; continue; }
                if ((c == QLatin1Char('+') || c == QLatin1Char('-')) &&
                    pos > s && (txt.at(pos - 1) == QLatin1Char('e') || txt.at(pos - 1) == QLatin1Char('E'))) { ++pos; continue; }
                if (c.isDigit()) { ++pos; continue; }
                break;
            }
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            QString w = txt.mid(s, pos - s);
            if (_keywords.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Keyword;
            } else if (_functions.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Function;
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
