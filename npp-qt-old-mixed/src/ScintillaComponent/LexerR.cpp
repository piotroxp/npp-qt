// Custom QsciLexer for R language
// Syntax: <- assignment, function(){}, library(), if/else/for/while, # comments

#include "LexerR.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerR::LexerR(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "if else for while repeat switch function return break next "
        "TRUE FALSE NULL NA NaN Inf "
        "in library require source setwd getwd rm ls detach data.frame "
        "matrix vector list array factor data.table tidyverse dplyr ggplot "
        "print str summary mean median sd var cov cor abs sqrt log exp "
        "cbind rbind dim names colnames rownames apply lapply sapply tapply "
        "plot hist boxplot barplot points lines text legend axis "
        "read.csv write.csv read.table read.delim save load "
        "install.packages library loadedNamespaces packageVersion "
        "options args help str class is.na is.null as numeric character logical factor "
        "length dim rep seq which order sort unique unlist "
        "paste sprintf substr substring toupper tolower gsub sub "
        "regexpr grepl grep globalenv localenv parent.env "
        "tryCatch stop warning message requireNamespace"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString fns = QStringLiteral(
        "c list data.frame matrix array vector factor "
        "lm glm anova t.test wilcox.test cor.test chisq.test "
        "plot lines points text axis legend title mtext "
        "abline segments arrows polygon rect curve "
        "par layout mfrow mfcol oma omi mai"
    );
    for (const QString& f : fns.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _functions.insert(f);
}

LexerR::~LexerR() {}

QColor LexerR::defaultColor(int style) const
{
    switch (style) {
        case Comment:    return QColor(0x60, 0x60, 0x60);
        case String:     return QColor(0x8B, 0x00, 0x00);
        case Keyword:    return QColor(0x00, 0x00, 0x8B);
        case Number:     return QColor(0x00, 0x8B, 0x8B);
        case Function:   return QColor(0x80, 0x00, 0x80);
        case Operator:   return QColor(0x00, 0x00, 0x00);
        case Dollar:     return QColor(0x00, 0x64, 0x00);
        case Special:     return QColor(0xCC, 0x7F, 0x00);
        default:         return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerR::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerR::description(int style) const
{
    switch (style) {
        case Default:   return tr("Default");
        case Comment:   return tr("Comment");
        case String:    return tr("String");
        case Keyword:   return tr("Keyword");
        case Number:    return tr("Number");
        case Function:  return tr("Function");
        case Operator: return tr("Operator");
        case Dollar:    return tr("Dollar");
        case Special:   return tr("Special");
        default:        return QString();
    }
}

void LexerR::styleText(int start, int end)
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

        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'')) {
            QChar quote = ch;
            int s = pos++;
            while (pos < txtLen && txt.at(pos) != quote && txt.at(pos) != QLatin1Char('\n')) {
                if (txt.at(pos) == QLatin1Char('\\') && pos + 1 < txtLen) ++pos;
                ++pos;
            }
            if (pos < txtLen && txt.at(pos) == quote) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('<') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('-')) {
            styleBytes[pos++] = Operator;
            styleBytes[pos++] = Operator;
            continue;
        }

        if (ch == QLatin1Char('-') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('>')) {
            styleBytes[pos++] = Operator;
            styleBytes[pos++] = Operator;
            continue;
        }

        if (ch == QLatin1Char('$') || ch == QLatin1Char('@')) {
            styleBytes[pos++] = Dollar;
            continue;
        }

        if (ch == QLatin1Char('%')) {
            int s = pos;
            while (pos < txtLen && txt.at(pos) != QLatin1Char('%') && txt.at(pos) != QLatin1Char('\n'))
                ++pos;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('%')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Special;
            continue;
        }

        if (ch.isDigit() || (ch == QLatin1Char('.') && pos + 1 < txtLen && txt.at(pos + 1).isDigit())) {
            int s = pos;
            if (ch == QLatin1Char('.')) ++pos;
            while (pos < txtLen && (txt.at(pos).isDigit() || txt.at(pos) == QLatin1Char('.') ||
                       txt.at(pos) == QLatin1Char('e') || txt.at(pos) == QLatin1Char('E') ||
                       txt.at(pos) == QLatin1Char('L') || txt.at(pos) == QLatin1Char('i')) ||
                      ((txt.at(pos) == QLatin1Char('+') || txt.at(pos) == QLatin1Char('-')) &&
                       pos > s && (txt.at(pos - 1) == QLatin1Char('e') || txt.at(pos - 1) == QLatin1Char('E'))))
                ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('_') || ch == QLatin1Char('.')) {
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
