// Custom QsciLexer for Haskell
// Syntax: module, import, data, type, class, instance, deriving, -> => ::

#include "LexerHaskell.h"
#include <Qsci/qsciScintilla.h>
#include <QColor>
#include <QFont>

LexerHaskell::LexerHaskell(QObject* parent)
    : QsciLexerCustom(parent)
{
    const QString kw = QStringLiteral(
        "as case class data default deriving do else family forall foreign "
        "hiding if import in infix infixl infixr instance let module newtype "
        "of qualified then type where"
    );
    for (const QString& w : kw.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _keywords.insert(w);

    const QString cl = QStringLiteral(
        "Eq Ord Show Read Bounded Enum Num Integral Fractional Floating "
        "Real RealFrac RealFloat Monoid Foldable Functor Applicative Monad "
        "Semigroup Semigroup Semigroup Monoid"
    );
    for (const QString& c : cl.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _classes.insert(c);

    const QString bi = QStringLiteral(
        "True False Nothing Just Left Right IO return fail mapM filterM "
        "putStrLn putStr print getLine interact unlines words lines "
        "concat zip unzip sort nub lookup maybe either fst snd curry uncurry "
        "id const flip sequence sequence_ mapM_ forM_ void"
    );
    for (const QString& b : bi.split(QLatin1Char(' '), Qt::SkipEmptyParts))
        _builtins.insert(b);
}

LexerHaskell::~LexerHaskell() {}

QColor LexerHaskell::defaultColor(int style) const
{
    switch (style) {
        case Comment:   return QColor(0x60, 0x60, 0x60);
        case String:    return QColor(0x8B, 0x00, 0x00);
        case Keyword:   return QColor(0x00, 0x00, 0x8B);
        case Number:    return QColor(0x00, 0x8B, 0x8B);
        case Type:      return QColor(0x00, 0x64, 0x00);
        case Class:     return QColor(0x00, 0x64, 0x00);
        case Function:  return QColor(0x80, 0x00, 0x80);
        case Operator:  return QColor(0x00, 0x00, 0x00);
        case Import:    return QColor(0x80, 0x00, 0x80);
        default:        return QColor(0x00, 0x00, 0x00);
    }
}

QFont LexerHaskell::defaultFont(int style) const
{
    QFont f(QLatin1String("Courier New"), 10);
    if (style == Comment) f.setItalic(true);
    return f;
}

QString LexerHaskell::description(int style) const
{
    switch (style) {
        case Default:  return tr("Default");
        case Comment:  return tr("Comment");
        case String:   return tr("String");
        case Keyword:  return tr("Keyword");
        case Number:   return tr("Number");
        case Type:     return tr("Type");
        case Class:    return tr("Class");
        case Function: return tr("Function");
        case Operator: return tr("Operator");
        case Import:   return tr("Import");
        default:       return QString();
    }
}

void LexerHaskell::styleText(int start, int end)
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

        if (ch == QLatin1Char('-') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('-')) {
            while (pos < txtLen && txt.at(pos) != QLatin1Char('\n'))
                styleBytes[pos++] = Comment;
            continue;
        }

        if (ch == QLatin1Char('{') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('-')) {
            int s = pos; pos += 2;
            while (pos + 1 < txtLen) {
                if (txt.at(pos) == QLatin1Char('-') && txt.at(pos + 1) == QLatin1Char('}')) { pos += 2; break; }
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

        if (ch == QLatin1Char('\'')) {
            int s = pos++;
            if (pos < txtLen && txt.at(pos) != QLatin1Char('\'')) ++pos;
            if (pos < txtLen && txt.at(pos) == QLatin1Char('\'')) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = String;
            continue;
        }

        if (ch == QLatin1Char('-') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('>')) {
            styleBytes[pos++] = Operator;
            styleBytes[pos++] = Operator;
            continue;
        }

        if (ch == QLatin1Char('=') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char('>')) {
            styleBytes[pos++] = Operator;
            styleBytes[pos++] = Operator;
            continue;
        }

        if (ch == QLatin1Char(':') && pos + 1 < txtLen && txt.at(pos + 1) == QLatin1Char(':')) {
            styleBytes[pos++] = Operator;
            styleBytes[pos++] = Operator;
            continue;
        }

        if (ch.isDigit()) {
            int s = pos;
            ++pos;
            while (pos < txtLen && (txt.at(pos).isDigit() || txt.at(pos) == QLatin1Char('.') ||
                       txt.at(pos) == QLatin1Char('e') || txt.at(pos) == QLatin1Char('E') ||
                       txt.at(pos) == QLatin1Char('_'))) ++pos;
            for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Number;
            continue;
        }

        if (ch.isLetter() || ch == QLatin1Char('\'') || ch == QLatin1Char('_')) {
            int s = pos;
            while (pos < txtLen && isWordChar(txt.at(pos))) ++pos;
            QString w = txt.mid(s, pos - s);
            if (_keywords.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Keyword;
            } else if (_classes.contains(w)) {
                for (int i = s; i < pos && i < txtLen; ++i) styleBytes[i] = Class;
            } else if (!w.isEmpty() && w.at(0).isUpper()) {
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
