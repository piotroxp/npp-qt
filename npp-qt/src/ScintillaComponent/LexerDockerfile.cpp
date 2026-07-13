// Custom QsciLexer for Dockerfile
// Syntax: INSTRUCTION arguments, # comments, ${VAR} variables

#include "LexerDockerfile.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerDockerfile::LexerDockerfile(QObject* parent)
    : QsciLexerCustom(parent)
{
    // Dockerfile instructions
    const QString kw = QStringLiteral(
        "FROM RUN CMD LABEL ENV EXPOSE ADD COPY ENTRYPOINT "
        "VOLUME USER WORKDIR ARG MAINTAINER ONBUILD STOPSIGNAL "
        "HEALTHCHECK SHELL OVERLAY"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    // Common flags
    const QString fl = QStringLiteral(
        "-t -p -v --rm -e -u -w -c -d -i -it --name "
        "--network --privileged --env-file --cpus --memory"
    );
    for (const QString& f : fl.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _flags.insert(f);
}

LexerDockerfile::~LexerDockerfile() {}

QColor LexerDockerfile::defaultColor(int style) const
{
    switch (style) {
        case Comment:     return QColor(0x60, 0x60, 0x60);
        case Keyword:     return QColor(0x00, 0x00, 0x8B);   // dark blue
        case Instruction: return QColor(0x80, 0x00, 0x80);   // purple
        case String:      return QColor(0x8B, 0x00, 0x00);   // dark red
        case Variable:    return QColor(0x00, 0x64, 0x00);   // dark green
        case Number:      return QColor(0x00, 0x8B, 0x8B);   // dark cyan
        case Flag:        return QColor(0xCC, 0x7F, 0x00);   // brown
        default:          return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerDockerfile::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerDockerfile::description(int style) const
{
    switch (style) {
        case Default:      return tr("Default");
        case Comment:      return tr("Comment");
        case Keyword:      return tr("Keyword");
        case Instruction: return tr("Instruction");
        case String:       return tr("String");
        case Variable:     return tr("Variable");
        case Number:       return tr("Number");
        case Flag:         return tr("Flag");
        default:           return QString();
    }
}

void LexerDockerfile::styleText(int start, int end)
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
            int s = pos++;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('"') && txt.at(pos) != QLatin1Char('\n')) {
                if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == QLatin1Char('"')) ++pos;
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

        if (ch == QLatin1Char('$')) {
            int s = pos++;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('{')) {
                ++pos;
                while (pos < txtLen && txt.at(pos) != QLatin1Char('}') && txt.at(pos) != QLatin1Char('\n'))
                    ++pos;
                if (pos < txtLen && txt.at(pos) == QLatin1Char('}')) ++pos;
            } else {
                while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            }
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Variable;
            continue;
        }

        if (ch == QLatin1Char('-') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('-')) {
            int s = pos;
            while (pos < txtLen && !txt.at(pos).isSpace() && txt.at(pos) != QLatin1Char('\n'))
                ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Flag;
            continue;
        }

        if (ch == QLatin1Char('-') && pos + 1 < txtLen && (txt.at(pos + 1).isLetter() || txt.at(pos + 1) == QLatin1Char('-'))) {
            int s = pos;
            ++pos;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            QString w = txt.mid(s, pos - s);
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = _flags.contains(w) ? Flag : Default;
            continue;
        }

        if (ch.isDigit()) {
            int s = pos;
            while (pos < txtLen && (txt.at(pos).isDigit() || txt.at(pos) == QLatin1Char('.') || txt.at(pos) == QLatin1Char(':')))
                ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && (txt.at(pos).isLetterOrNumber() || txt.at(pos) == QLatin1Char('_') || txt.at(pos) == QLatin1Char('-')))
                ++pos;
            QString w = txt.mid(s, pos - s).toUpper();
            // Check if it's the first token on a line (instruction)
            int lineOff = s;
            while (lineOff > 0 && txt.at(lineOff - 1).isSpace()) --lineOff;
            bool lineStart = (lineOff == 0);
            for (int i = s; i < pos && i < txtLen; ++i)
                styleBytes[i] = (_keywords.contains(w) && lineStart) ? Instruction : Default;
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
