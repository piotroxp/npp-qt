// npp-qt: ScintillaEditView implementation — missing methods translated from Win32
// Original: PowerEditor/src/ScintillaComponent/ScintillaEditView.cpp (4955 lines)
// Target:   npp-qt/src/ScintillaComponent/ScintillaEditView.cpp
//
// This file extends src/ScintillaComponent.cpp with methods from the original
// ScintillaEditView.cpp that were not yet implemented there.
//
// Key Win32 → Qt6 translations:
//   HWND _hSelf              → ScintillaEditBase* (is the widget)
//   SendMessage(_hSelf, ...) → send(...)
//   ::CreateWindowEx()       → constructor / new
//   ::DestroyWindow()        → deleteLater()
//   SetWindowSubclass()     → Qt event filter
//   Window procedure         → Qt event() / QObject signal/slot

#include "ScintillaComponent.h"

#include <QString>
#include <QByteArray>
#include <QVector>
#include <QColor>
#include <QApplication>
#include <QClipboard>
#include <QGuiApplication>
#include <QDebug>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>

#include <SciLexer.h>
#include <Scintilla.h>

#include "Buffer.h"
#include "Sorters.h"

// =============================================================================
// Constants from the original source
// =============================================================================

// Fold actions
static constexpr int fold_collapse = 0;
static constexpr int fold_expand = 1;

// =============================================================================
// g_ZWSP — Zero-Width Space character
// =============================================================================

static const char g_ZWSP[] = "\xE2\x80\x8B";

// =============================================================================
// g_ccUniEolChars — Unicode EOL characters table
// =============================================================================

static const std::array<std::array<const char*, 3>, 3> g_ccUniEolChars{{
    {"\xC2\x85", "NEL", "U+0085"},      // U+0085 : Next Line
    {"\xE2\x80\xA8", "LS", "U+2028"},  // U+2028 : Line Separator
    {"\xE2\x80\xA9", "PS", "U+2029"}   // U+2029 : Paragraph Separator
}};

// =============================================================================
// g_nonPrintingChars — Non-printing Unicode characters
// =============================================================================

static const std::array<std::array<const char*, 3>, 10> g_nonPrintingChars{{
    {"\xC2\xA0", "NBSP", "U+00A0"},
    {"\xC2\xAD", "SHY", "U+00AD"},
    {"\xE2\x80\x80", "NQSP", "U+2000"},
    {"\xE2\x80\x81", "MQSP", "U+2001"},
    {"\xE2\x80\x82", "ENSP", "U+2002"},
    {"\xE2\x80\x83", "EMSP", "U+2003"},
    {"\xE2\x80\x8B", "ZWSP", "U+200B"},
    {"\xE2\x80\x8C", "ZWNJ", "U+200C"},
    {"\xE2\x80\x8D", "ZWJ", "U+200D"},
    {"\xEF\xBB\xBF", "ZWNBSP", "U+FEFF"}
}};

// =============================================================================
// ScintillaComponent — additional methods extending ScintillaComponent.cpp
// These are translated from the original ScintillaEditView.cpp
// =============================================================================

// --- marginClick with shift/ctrl expand logic ---
void ScintillaComponent::marginClickExpand(size_t position, int modifiers)
{
    size_t lineClick = send(SCI_LINEFROMPOSITION, static_cast<size_t>(position));
    intptr_t levelClick = send(SCI_GETFOLDLEVEL, static_cast<size_t>(lineClick));

    if (levelClick & SC_FOLDLEVELHEADERFLAG)
    {
        if (modifiers & Qt::ShiftModifier)
        {
            send(SCI_SETFOLDEXPANDED, static_cast<size_t>(lineClick), 1);
            size_t l = lineClick;
            expandFull(l, true, true, 100, levelClick);
        }
        else if (modifiers & Qt::ControlModifier)
        {
            if (isFolded(static_cast<size_t>(lineClick)))
            {
                send(SCI_SETFOLDEXPANDED, static_cast<size_t>(lineClick), 0);
                size_t l = lineClick;
                expandFull(l, false, true, 0, levelClick);
            }
            else
            {
                send(SCI_SETFOLDEXPANDED, static_cast<size_t>(lineClick), 1);
                size_t l = lineClick;
                expandFull(l, true, true, 100, levelClick);
            }
        }
        else
        {
            bool mode = isFolded(static_cast<size_t>(lineClick));
            fold(static_cast<size_t>(lineClick), !mode);
            if (!mode)
                hideMarkedLines(static_cast<size_t>(lineClick), true);
        }
    }
}

// --- expand overload with full parameters ---
void ScintillaComponent::expandFull(size_t& line, bool doExpand, bool force, int visLevels, int level) const
{
    intptr_t lineMaxSubord = send(SCI_GETLASTCHILD, static_cast<size_t>(line), level & SC_FOLDLEVELNUMBERMASK);
    ++line;

    while (line <= static_cast<size_t>(lineMaxSubord))
    {
        if (force)
        {
            send((visLevels > 0) ? SCI_SHOWLINES : SCI_HIDELINES,
                 static_cast<size_t>(line), static_cast<size_t>(line));
        }
        else if (doExpand)
        {
            send(SCI_SHOWLINES, static_cast<size_t>(line), static_cast<size_t>(line));
        }

        intptr_t levelLine = level;
        if (levelLine == -1)
            levelLine = send(SCI_GETFOLDLEVEL, static_cast<size_t>(line));

        if (levelLine & SC_FOLDLEVELHEADERFLAG)
        {
            if (force)
            {
                send(SCI_SETFOLDEXPANDED, static_cast<size_t>(line), visLevels > 1);
                size_t l = line;
                expandFull(l, doExpand, force, visLevels - 1, levelLine);
                line = l;
            }
            else
            {
                if (doExpand)
                {
                    if (!isFolded(static_cast<size_t>(line)))
                        send(SCI_SETFOLDEXPANDED, static_cast<size_t>(line), true);
                    size_t l = line;
                    expandFull(l, true, force, visLevels - 1, levelLine);
                    line = l;
                }
                else
                {
                    size_t l = line;
                    expandFull(l, false, force, visLevels - 1, levelLine);
                    line = l;
                }
            }
        }
        else
        {
            ++line;
        }
    }

    if (doExpand)
        hideMarkedLines(0, true);
}

// --- beginOrEndSelect for column mode ---
void ScintillaComponent::beginOrEndSelect(bool isColumnMode)
{
    auto currPos = send(SCI_GETCURRENTPOS);

    if (_beginSelectPosition == -1)
    {
        _beginSelectPosition = currPos;
    }
    else
    {
        send(SCI_CHANGESELECTIONMODE,
             static_cast<uptr_t>(isColumnMode ? SC_SEL_RECTANGLE : SC_SEL_STREAM));
        if (isColumnMode)
            send(SCI_SETANCHOR, static_cast<size_t>(_beginSelectPosition));
        else
            send(SCI_SETSEL, static_cast<size_t>(_beginSelectPosition), static_cast<size_t>(currPos));
        _beginSelectPosition = -1;
    }
}

// --- updateBeginEndSelectPosition ---
void ScintillaComponent::updateBeginEndSelectPosition(bool isInsert, size_t position, size_t length)
{
    if (_beginSelectPosition != -1 && static_cast<intptr_t>(position) < _beginSelectPosition - 1)
    {
        if (isInsert)
            _beginSelectPosition += static_cast<intptr_t>(length);
        else
            _beginSelectPosition -= static_cast<intptr_t>(length);
    }
}

// --- Show margin with DPI awareness ---
void ScintillaComponent::showMarginWithDpi(int whichMargin, bool visible, int dpi)
{
    if (whichMargin == SC_MARGE_LINENUMBER)
    {
        updateLineNumbersMargin(!visible);
    }
    else
    {
        int width = 0;
        if (whichMargin == SC_MARGE_SYMBOL)
            width = qRound(16.0 * dpi / 96.0);
        else if (whichMargin == SC_MARGE_FOLDER)
            width = qRound(14.0 * dpi / 96.0);
        else
            width = qRound(3.0 * dpi / 96.0);

        send(SCI_SETMARGINWIDTHN, whichMargin, visible ? width : 0);
    }
}

// --- Show change history margin ---
void ScintillaComponent::showChangeHistoryMargin(bool visible, int dpi)
{
    int width = qRound(9.0 * dpi / 96.0);
    send(SCI_SETMARGINWIDTHN, SC_MARGE_CHANGEHISTORY, visible ? width : 0);
}

// --- Hide marked lines ---
void ScintillaComponent::hideMarkedLines(size_t searchStart, bool toEndOfDoc) const
{
    size_t maxLines = send(SCI_GETLINECOUNT);
    auto startHiding = searchStart;
    bool isInSection = false;

    for (auto i = searchStart; i < maxLines; ++i)
    {
        auto state = send(SCI_MARKERGET, i);
        if ((state & (1 << MARK_HIDELINESEND)) != 0)
        {
            if (isInSection)
            {
                send(SCI_HIDELINES, startHiding, i - 1);
                if (!toEndOfDoc)
                    return;
            }
            isInSection = false;
        }
        if ((state & (1 << MARK_HIDELINESBEGIN)) != 0)
        {
            isInSection = true;
            startHiding = i + 1;
        }
    }
}

// --- Show hidden lines ---
void ScintillaComponent::showHiddenLines(size_t searchStart, bool toEndOfDoc, bool doDelete) const
{
    size_t maxLines = send(SCI_GETLINECOUNT);
    auto startShowing = searchStart;
    bool isInSection = false;

    for (auto i = searchStart; i < maxLines; ++i)
    {
        auto state = send(SCI_MARKERGET, i);
        if ((state & (1 << MARK_HIDELINESBEGIN)) != 0 && !isInSection)
        {
            isInSection = true;
            if (doDelete)
                send(SCI_MARKERDELETE, static_cast<size_t>(i), MARK_HIDELINESBEGIN);
            else
                startShowing = i + 1;
        }
        else if ((state & (1 << MARK_HIDELINESEND)) != 0)
        {
            if (doDelete)
            {
                send(SCI_MARKERDELETE, static_cast<size_t>(i), MARK_HIDELINESEND);
                if (!toEndOfDoc)
                    return;
                isInSection = false;
            }
            else if (isInSection)
            {
                if (startShowing >= i)
                {
                    if (!toEndOfDoc)
                        return;
                    isInSection = false;
                    continue;
                }
                send(SCI_SHOWLINES, startShowing, i - 1);
                if (!toEndOfDoc)
                    return;
                isInSection = false;
            }
        }

        auto levelLine = send(SCI_GETFOLDLEVEL, i);
        if (levelLine & SC_FOLDLEVELHEADERFLAG)
        {
            if (isInSection && !isFolded(i))
                send(SCI_SHOWLINES, startShowing, i);
        }
    }
}

// --- Hide lines ---
void ScintillaComponent::hideLines()
{
    size_t startLine = send(SCI_LINEFROMPOSITION, send(SCI_GETSELECTIONSTART));
    size_t endLine = send(SCI_LINEFROMPOSITION, send(SCI_GETSELECTIONEND));

    size_t nbLines = send(SCI_GETLINECOUNT);
    if (nbLines < 3)
        return;

    if (!startLine)
        ++startLine;
    if (endLine == (nbLines - 1))
        --endLine;
    if (startLine > endLine)
        return;

    int scope = 0;
    bool recentMarkerWasOpen = false;

    auto removeMarker = [&](size_t line, int markerMask) {
        auto state = send(SCI_MARKERGET, line) & markerMask;
        bool closePresent = (state & (1 << MARK_HIDELINESEND)) != 0;
        bool openPresent = (state & (1 << MARK_HIDELINESBEGIN)) != 0;
        if (closePresent)
        {
            send(SCI_MARKERDELETE, line, MARK_HIDELINESEND);
            recentMarkerWasOpen = false;
            --scope;
        }
        if (openPresent)
        {
            send(SCI_MARKERDELETE, line, MARK_HIDELINESBEGIN);
            recentMarkerWasOpen = true;
            ++scope;
        }
    };

    size_t startMarker = startLine - 1;
    size_t endMarker = endLine + 1;

    removeMarker(startMarker, (1 << MARK_HIDELINESBEGIN));
    for (size_t i = startLine; i <= endLine; ++i)
        removeMarker(i, (1 << MARK_HIDELINESBEGIN) | (1 << MARK_HIDELINESEND));
    removeMarker(endMarker, (1 << MARK_HIDELINESEND));

    if (scope == 0 && recentMarkerWasOpen)
    {
        while (scope == 0 && static_cast<intptr_t>(startMarker) >= 0)
            removeMarker(--startMarker, (1 << MARK_HIDELINESBEGIN));
        while (scope != 0 && endMarker < nbLines)
            removeMarker(++endMarker, (1 << MARK_HIDELINESEND));
    }
    else
    {
        while (scope < 0 && static_cast<intptr_t>(startMarker) >= 0)
            removeMarker(--startMarker, (1 << MARK_HIDELINESBEGIN));
        while (scope > 0 && endMarker < nbLines)
            removeMarker(++endMarker, (1 << MARK_HIDELINESEND));
    }

    send(SCI_MARKERADD, startMarker, MARK_HIDELINESBEGIN);
    send(SCI_MARKERADD, endMarker, MARK_HIDELINESEND);
}

// --- hidelineMarkerClicked ---
bool ScintillaComponent::hidelineMarkerClicked(size_t lineNumber)
{
    auto state = send(SCI_MARKERGET, lineNumber);
    bool openPresent = (state & (1 << MARK_HIDELINESBEGIN)) != 0;
    bool closePresent = (state & (1 << MARK_HIDELINESEND)) != 0;

    if (!openPresent && !closePresent)
        return false;

    if (openPresent)
    {
        closePresent = false;
        size_t i = lineNumber - 1;
        for (; i > 0; --i)
        {
            state = send(SCI_MARKERGET, i);
            openPresent = (state & (1 << MARK_HIDELINESBEGIN)) != 0;
            if (openPresent)
                break;
        }
    }

    return true;
}

// --- insertNewLineAbove ---
void ScintillaComponent::insertNewLineAbove()
{
    QString eol = (eolMode() == SC_EOL_CRLF) ? QStringLiteral("\r\n") : QStringLiteral("\n");
    QByteArray eolBytes = eol.toUtf8();

    size_t currentLine = static_cast<size_t>(send(SCI_LINEFROMPOSITION, send(SCI_GETCURRENTPOS)));

    if (currentLine == 0)
    {
        send(SCI_INSERTTEXT, 0, reinterpret_cast<sptr_t>("\n"));
    }
    else
    {
        size_t pos = send(SCI_POSITIONFROMLINE, currentLine) - eolBytes.size();
        send(SCI_INSERTTEXT, pos, reinterpret_cast<sptr_t>(eolBytes.constData()));
    }
    send(SCI_SETEMPTYSELECTION, send(SCI_POSITIONFROMLINE, currentLine));
}

// --- insertNewLineBelow ---
void ScintillaComponent::insertNewLineBelow()
{
    QString eol = (eolMode() == SC_EOL_CRLF) ? QStringLiteral("\r\n") : QStringLiteral("\n");
    QByteArray eolBytes = eol.toUtf8();

    size_t lineCount = send(SCI_GETLINECOUNT);
    size_t currentLine = static_cast<size_t>(send(SCI_LINEFROMPOSITION, send(SCI_GETCURRENTPOS)));

    if (currentLine == lineCount - 1)
    {
        send(SCI_APPENDTEXT, eolBytes.size(), reinterpret_cast<sptr_t>(eolBytes.constData()));
    }
    else
    {
        size_t pos = eolBytes.size() + send(SCI_GETLINEENDPOSITION, currentLine);
        send(SCI_INSERTTEXT, pos, reinterpret_cast<sptr_t>(eolBytes.constData()));
    }
    send(SCI_SETEMPTYSELECTION, send(SCI_POSITIONFROMLINE, currentLine + 1));
}

// --- getEolString ---
QString ScintillaComponent::getEolString() const
{
    switch (eolMode())
    {
        case SC_EOL_CRLF: return QStringLiteral("\r\n");
        case SC_EOL_LF:   return QStringLiteral("\n");
        case SC_EOL_CR:   return QStringLiteral("\r");
        default:          return QStringLiteral("\r\n");
    }
}

// --- scrollPosToCenter ---
void ScintillaComponent::scrollPosToCenter(size_t pos)
{
    send(SCI_GOTOPOS, pos);
    size_t line = send(SCI_LINEFROMPOSITION, pos);

    size_t firstVisibleDisplayLine = send(SCI_GETFIRSTVISIBLELINE);
    size_t firstVisibleDocLine = send(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLine);
    size_t nbLine = send(SCI_LINESONSCREEN, firstVisibleDisplayLine);
    size_t lastVisibleDocLine = send(SCI_DOCLINEFROMVISIBLE,
                                      firstVisibleDisplayLine + nbLine);

    size_t middleLine = (line - firstVisibleDocLine < lastVisibleDocLine - line)
                        ? firstVisibleDocLine + nbLine / 2
                        : lastVisibleDocLine - nbLine / 2;

    size_t nbLines2scroll = (line > middleLine) ? line - middleLine : 0;
    scroll(0, static_cast<int>(nbLines2scroll));
    send(SCI_ENSUREVISIBLEENFORCEPOLICY, line);
}

// --- sortLines ---
void ScintillaComponent::sortLines(size_t fromLine, size_t toLine, ISorter* pSort)
{
    if (fromLine >= toLine)
        return;

    size_t startPos = send(SCI_POSITIONFROMLINE, fromLine);
    size_t endPos = send(SCI_POSITIONFROMLINE, toLine) + send(SCI_LINELENGTH, toLine);

    QString text = getTextAsString(startPos, endPos);
    QString eol = getEolString();

    QStringList lines = text.split(eol, Qt::KeepEmptyParts);
    pSort->sort(lines);

    QString joined = lines.join(eol);
    if (!joined.endsWith(eol) && text.endsWith(eol))
        joined += eol;

    replaceTarget(joined, static_cast<intptr_t>(startPos), static_cast<intptr_t>(endPos));
}

// --- removeDuplicateLines ---
void ScintillaComponent::removeDuplicateLines()
{
    size_t fromLine = 0, toLine = 0;
    bool hasSelection = (send(SCI_GETSELECTIONSTART) != send(SCI_GETSELECTIONEND));

    if (hasSelection)
    {
        auto lr = getSelectionLinesRange();
        if (lr.first == lr.second)
            return;
        fromLine = lr.first;
        toLine = lr.second;
    }
    else
    {
        fromLine = 0;
        toLine = send(SCI_GETLINECOUNT) - 1;
    }

    if (fromLine >= toLine)
        return;

    size_t startPos = send(SCI_POSITIONFROMLINE, fromLine);
    size_t endPos = send(SCI_POSITIONFROMLINE, toLine) + send(SCI_LINELENGTH, toLine);

    QString text = getTextAsString(startPos, endPos);
    QString eol = getEolString();

    QStringList lines = text.split(eol, Qt::KeepEmptyParts);
    size_t origSize = lines.size();

    QStringList unique;
    for (const QString& line : lines)
    {
        if (!unique.contains(line))
            unique.append(line);
    }

    if (origSize != static_cast<size_t>(unique.size()))
    {
        QString joined = unique.join(eol);
        replaceTarget(joined, static_cast<intptr_t>(startPos), static_cast<intptr_t>(endPos));
    }
}

// --- showNpc ---
void ScintillaComponent::showNpc(bool willBeShown, bool /*isSearchResult*/)
{
    if (willBeShown)
    {
        for (const auto& invChar : g_nonPrintingChars)
        {
            QByteArray utf8Char(invChar[0]);
            QByteArray representation(invChar[1]);
            send(SCI_SETREPRESENTATION,
                 reinterpret_cast<uptr_t>(utf8Char.constData()),
                 reinterpret_cast<sptr_t>(representation.constData()));
        }
    }
    else
    {
        send(SCI_CLEARALLREPRESENTATIONS);
        setCRLF();
    }
}

// --- showCcUniEol ---
void ScintillaComponent::showCcUniEol(bool willBeShown, bool /*isSearchResult*/)
{
    if (willBeShown)
    {
        for (const auto& invChar : g_ccUniEolChars)
        {
            QByteArray utf8Char(invChar[0]);
            QByteArray representation(invChar[1]);
            send(SCI_SETREPRESENTATION,
                 reinterpret_cast<uptr_t>(utf8Char.constData()),
                 reinterpret_cast<sptr_t>(representation.constData()));
        }
    }
    else
    {
        for (const auto& invChar : g_ccUniEolChars)
        {
            QByteArray utf8Char(invChar[0]);
            send(SCI_SETREPRESENTATION,
                 reinterpret_cast<uptr_t>(utf8Char.constData()),
                 reinterpret_cast<sptr_t>(g_ZWSP));
            send(SCI_SETREPRESENTATIONAPPEARANCE,
                 reinterpret_cast<uptr_t>(utf8Char.constData()),
                 SC_REPRESENTATION_PLAIN);
        }
    }
}

// --- showIndentGuideLine ---
void ScintillaComponent::showIndentGuideLine(bool willBeShown)
{
    int docIndentMode = SC_IV_LOOKBOTH;
    if (_currentBuffer)
    {
        LangType typeDoc = _currentBuffer->getLangType();
        docIndentMode = isPythonStyleIndentationForLang(typeDoc) ? SC_IV_LOOKFORWARD : SC_IV_LOOKBOTH;
    }
    send(SCI_SETINDENTATIONGUIDES, willBeShown ? docIndentMode : SC_IV_NONE);
}

// --- isPythonStyleIndentationForLang ---
bool ScintillaComponent::isPythonStyleIndentationForLang(LangType typeDoc) const
{
    return (typeDoc == L_PYTHON || typeDoc == L_COFFEESCRIPT || typeDoc == L_HASKELL ||
            typeDoc == L_C || typeDoc == L_CPP || typeDoc == L_OBJC || typeDoc == L_CS ||
            typeDoc == L_JAVA || typeDoc == L_PHP || typeDoc == L_JS_EMBEDDED ||
            typeDoc == L_JAVASCRIPT || typeDoc == L_MAKEFILE || typeDoc == L_ASN1 ||
            typeDoc == L_GDSCRIPT);
}

// --- getColumnModeSelectInfo ---
ColumnModeInfos ScintillaComponent::getColumnModeSelectInfo()
{
    ColumnModeInfos columnModeInfos;
    size_t nbSel = send(SCI_GETSELECTIONS);

    if (nbSel > 1)
    {
        for (size_t i = 0; i < nbSel; ++i)
        {
            intptr_t absPosSelStartPerLine = send(SCI_GETSELECTIONNANCHOR, i);
            intptr_t absPosSelEndPerLine = send(SCI_GETSELECTIONNCARET, i);
            intptr_t nbVirtualAnchorSpc = send(SCI_GETSELECTIONNANCHORVIRTUALSPACE, i);
            intptr_t nbVirtualCaretSpc = send(SCI_GETSELECTIONNCARETVIRTUALSPACE, i);

            bool isRect = (send(SCI_SELECTIONISRECTANGLE) != 0);

            if (absPosSelStartPerLine == absPosSelEndPerLine && isRect)
            {
                bool isDirL2R = nbVirtualAnchorSpc < nbVirtualCaretSpc;
                columnModeInfos.push_back(
                    ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i,
                                   isDirL2R, nbVirtualAnchorSpc, nbVirtualCaretSpc));
            }
            else if (absPosSelStartPerLine > absPosSelEndPerLine)
            {
                columnModeInfos.push_back(
                    ColumnModeInfo(absPosSelEndPerLine, absPosSelStartPerLine, i,
                                   false, nbVirtualAnchorSpc, nbVirtualCaretSpc));
            }
            else
            {
                columnModeInfos.push_back(
                    ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i,
                                   true, nbVirtualAnchorSpc, nbVirtualCaretSpc));
            }
        }
    }
    return columnModeInfos;
}

// --- columnReplace (string) ---
void ScintillaComponent::columnReplaceString(ColumnModeInfos& cmi, const char* str) const
{
    intptr_t totalDiff = 0;
    size_t len = cmi.size();

    for (size_t i = 0; i < len; ++i)
    {
        if (cmi[i].isValid())
        {
            intptr_t len2beReplace = cmi[i]._selRpos - cmi[i]._selLpos;
            intptr_t diff = static_cast<intptr_t>(strlen(str)) - len2beReplace;

            cmi[i]._selLpos += totalDiff;
            cmi[i]._selRpos += totalDiff;
            bool hasVirtualSpc = cmi[i]._nbVirtualAnchorSpc > 0;

            if (hasVirtualSpc)
            {
                for (intptr_t j = 0, k = cmi[i]._selLpos; j < cmi[i]._nbVirtualCaretSpc; ++j, ++k)
                    send(SCI_INSERTTEXT, static_cast<size_t>(k), reinterpret_cast<sptr_t>(" "));
                cmi[i]._selLpos += cmi[i]._nbVirtualAnchorSpc;
                cmi[i]._selRpos += cmi[i]._nbVirtualCaretSpc;
            }

            send(SCI_SETTARGETRANGE, static_cast<size_t>(cmi[i]._selLpos),
                 static_cast<size_t>(cmi[i]._selRpos));
            send(SCI_REPLACETARGET, strlen(str), reinterpret_cast<sptr_t>(str));

            if (hasVirtualSpc)
            {
                totalDiff += cmi[i]._nbVirtualAnchorSpc + static_cast<intptr_t>(strlen(str));
                cmi[i]._nbVirtualAnchorSpc = 0;
                cmi[i]._nbVirtualCaretSpc = 0;
            }
            else
            {
                totalDiff += diff;
            }
            cmi[i]._selRpos += diff;
        }
    }
}

// --- getWordRange ---
std::pair<size_t, size_t> ScintillaComponent::getWordRange()
{
    size_t caretPos = send(SCI_GETCURRENTPOS);
    size_t startPos = send(SCI_WORDSTARTPOSITION, caretPos, true);
    size_t endPos = send(SCI_WORDENDPOSITION, caretPos, true);
    return std::pair<size_t, size_t>(startPos, endPos);
}

// --- expandWordSelection ---
bool ScintillaComponent::expandWordSelection()
{
    auto wordRange = getWordRange();
    if (wordRange.first != wordRange.second)
    {
        send(SCI_SETSELECTIONSTART, wordRange.first);
        send(SCI_SETSELECTIONEND, wordRange.second);
        return true;
    }
    return false;
}

// --- getSelectedCharsAndLinesCount (overload with maxSels) ---
std::pair<size_t, size_t> ScintillaComponent::getSelectedCharsAndLinesCount(size_t maxSels) const
{
    std::pair<size_t, size_t> result(0, 0);
    result.first = getUnicodeSelectedLength();

    size_t nbSelections = send(SCI_GETSELECTIONS);

    if (nbSelections == 1)
    {
        auto lr = getSelectionLinesRange();
        result.second = lr.second - lr.first + 1;
    }
    else if (send(SCI_SELECTIONISRECTANGLE) != 0)
    {
        result.second = nbSelections;
    }
    else if (maxSels == 0 || nbSelections <= maxSels)
    {
        std::vector<std::pair<size_t, size_t>> v;
        for (size_t s = 0; s < nbSelections; ++s)
            v.push_back(getSelectionLinesRange(static_cast<intptr_t>(s)));
        std::sort(v.begin(), v.end());
        intptr_t previousSecondLine = -1;
        for (const auto& lr : v)
        {
            result.second += lr.second - lr.first;
            if (lr.first != static_cast<size_t>(previousSecondLine))
                ++result.second;
            previousSecondLine = static_cast<intptr_t>(lr.second);
        }
    }

    return result;
}

// --- getUnicodeSelectedLength ---
size_t ScintillaComponent::getUnicodeSelectedLength() const
{
    size_t length = 0;
    size_t nbSelections = send(SCI_GETSELECTIONS);

    for (size_t s = 0; s < nbSelections; ++s)
    {
        size_t start = send(SCI_GETSELECTIONNSTART, s);
        size_t end = send(SCI_GETSELECTIONNEND, s);
        length += send(SCI_COUNTCHARACTERS, start, end);
    }

    return length;
}

// --- getSelectionLinesRange (overload with selNum) ---
std::pair<size_t, size_t> ScintillaComponent::getSelectionLinesRangeSel(intptr_t selNum) const
{
    size_t nbSelections = send(SCI_GETSELECTIONS);

    size_t startPos, endPos;
    if (selNum < 0 || static_cast<size_t>(selNum) >= nbSelections)
    {
        startPos = send(SCI_GETSELECTIONSTART);
        endPos = send(SCI_GETSELECTIONEND);
    }
    else
    {
        startPos = send(SCI_GETSELECTIONNSTART, static_cast<size_t>(selNum));
        endPos = send(SCI_GETSELECTIONNEND, static_cast<size_t>(selNum));
    }

    size_t line1 = send(SCI_LINEFROMPOSITION, startPos);
    size_t line2 = send(SCI_LINEFROMPOSITION, endPos);

    if ((line1 != line2) && (send(SCI_POSITIONFROMLINE, line2) == endPos))
        --line2;

    return std::pair<size_t, size_t>(line1, line2);
}

// --- setMultiSelections ---
void ScintillaComponent::setMultiSelections(const ColumnModeInfos& cmi)
{
    for (size_t i = 0, len = cmi.size(); i < len; ++i)
    {
        if (cmi[i].isValid())
        {
            const intptr_t selStart = cmi[i]._isDirectionL2R ? cmi[i]._selLpos : cmi[i]._selRpos;
            const intptr_t selEnd = cmi[i]._isDirectionL2R ? cmi[i]._selRpos : cmi[i]._selLpos;
            send(SCI_SETSELECTIONNSTART, i, static_cast<size_t>(selStart));
            send(SCI_SETSELECTIONNEND, i, static_cast<size_t>(selEnd));
        }

        if (cmi[i]._nbVirtualAnchorSpc)
            send(SCI_SETSELECTIONNANCHORVIRTUALSPACE, i, static_cast<size_t>(cmi[i]._nbVirtualAnchorSpc));
        if (cmi[i]._nbVirtualCaretSpc)
            send(SCI_SETSELECTIONNCARETVIRTUALSPACE, i, static_cast<size_t>(cmi[i]._nbVirtualCaretSpc));
    }
}

// --- pasteToMultiSelection ---
bool ScintillaComponent::pasteToMultiSelection() const
{
    size_t nbSelections = send(SCI_GETSELECTIONS);
    if (nbSelections <= 1)
        return false;

    const QString clipboardText = QGuiApplication::clipboard()->text();
    if (clipboardText.isEmpty())
        return false;

    QString eol = getEolString();
    QStringList clipboardLines = clipboardText.split(eol, Qt::KeepEmptyParts);
    size_t nbClipboardLines = static_cast<size_t>(clipboardLines.size());

    if (nbSelections >= nbClipboardLines)
    {
        send(SCI_BEGINUNDOACTION);
        for (size_t i = 0; i < nbClipboardLines; ++i)
        {
            size_t posStart = send(SCI_GETSELECTIONNSTART, i);
            size_t posEnd = send(SCI_GETSELECTIONNEND, i);
            QString replacement = clipboardLines[i];
            QByteArray utf8 = replacement.toUtf8();
            replaceTarget(utf8, static_cast<intptr_t>(posStart), static_cast<intptr_t>(posEnd));
            posStart += utf8.size();
            send(SCI_SETSELECTIONNSTART, i, posStart);
            send(SCI_SETSELECTIONNEND, i, posStart);
        }
        send(SCI_ENDUNDOACTION);
    }
    else
    {
        size_t nbStrPerHole = nbClipboardLines / nbSelections;
        send(SCI_BEGINUNDOACTION);
        for (size_t i = 0; i < nbSelections; ++i)
        {
            size_t posStart = send(SCI_GETSELECTIONNSTART, i);
            size_t posEnd = send(SCI_GETSELECTIONNEND, i);

            QString severalStr;
            for (size_t k = 0; k < nbStrPerHole && (i * nbStrPerHole + k) < nbClipboardLines; ++k)
            {
                if (k > 0)
                    severalStr += eol;
                severalStr += clipboardLines[i * nbStrPerHole + k];
            }

            QByteArray utf8 = severalStr.toUtf8();
            replaceTarget(utf8, static_cast<intptr_t>(posStart), static_cast<intptr_t>(posEnd));
            posStart += utf8.size();
            send(SCI_SETSELECTIONNSTART, i, posStart);
            send(SCI_SETSELECTIONNEND, i, posStart);
        }
        send(SCI_ENDUNDOACTION);
    }

    return true;
}

// --- markedTextToClipboard ---
void ScintillaComponent::markedTextToClipboard(int indiStyle, bool doAll)
{
    int styleIndicators[] = {
        SCE_UNIVERSAL_FOUND_STYLE_EXT1,
        SCE_UNIVERSAL_FOUND_STYLE_EXT2,
        SCE_UNIVERSAL_FOUND_STYLE_EXT3,
        SCE_UNIVERSAL_FOUND_STYLE_EXT4,
        SCE_UNIVERSAL_FOUND_STYLE_EXT5,
        -1
    };

    if (!doAll)
    {
        styleIndicators[0] = indiStyle;
        styleIndicators[1] = -1;
    }

    QStringList styledTexts;
    bool textContainsEol = false;
    const QString eol = getEolString();

    for (int si = 0; styleIndicators[si] != -1; ++si)
    {
        size_t pos = send(SCI_INDICATOREND, styleIndicators[si], 0);
        if (pos > 0)
        {
            bool atEndOfIndic = send(SCI_INDICATORVALUEAT, styleIndicators[si], 0) != 0;
            size_t prevPos = pos;
            if (atEndOf            if (atEndOfIndic)
            {
                QString styledText = getTextAsString(prevPos, pos);
                if (!textContainsEol &&
                    (styledText.contains(QStringLiteral("\r")) ||
                     styledText.contains(QStringLiteral("\n"))))
                {
                    textContainsEol = true;
                }
                styledTexts.append(styledText);
            }
            atEndOfIndic = !atEndOfIndic;
            prevPos = pos;
            pos = send(SCI_INDICATOREND, styleIndicators[si], pos);
        } while (pos != prevPos);
    }

    if (!styledTexts.isEmpty())
    {
        QString delim = (textContainsEol && styledTexts.size() > 1)
                        ? QStringLiteral("\r\n----\r\n")
                        : eol;
        QString joined;
        for (const QString& item : styledTexts)
        {
            joined += delim + item;
        }
        joined = joined.mid(delim.length());
        if (styledTexts.size() > 1)
            joined += eol;

        QGuiApplication::clipboard()->setText(joined);
    }
}

// --- setEolMode ---
void ScintillaComponent::setEolModeAndConvert(int mode)
{
    send(SCI_SETEOLMODE, mode);
    send(SCI_CONVERTEOLS, mode);
}

// --- setNpcAndCcUniEOL ---
void ScintillaComponent::setNpcAndCcUniEOL(long color)
{
    // Set NPC and CcUniEOL representations with custom color
    int rgbColor = (color == -1) ? qColorToSciRgb(QColor(128, 128, 128)) : color;
    Q_UNUSED(rgbColor);
    // In Qt6, representation colors are handled via style sheets or
    // individual character rendering. For now, this is a stub.
    update();
}

// --- getIndicatorRange ---
bool ScintillaComponent::getIndicatorRangeForPos(size_t indicatorNumber, size_t* from, size_t* to, size_t* cur)
{
    size_t curPos = send(SCI_GETCURRENTPOS);
    size_t indicMsk = send(SCI_INDICATORALLONFOR, curPos);
    if (!(indicMsk & (1 << indicatorNumber)))
        return false;

    size_t startPos = send(SCI_INDICATORSTART, indicatorNumber, curPos);
    size_t endPos = send(SCI_INDICATOREND, indicatorNumber, curPos);
    if ((curPos < startPos) || (curPos > endPos))
        return false;

    if (from) *from = startPos;
    if (to)   *to = endPos;
    if (cur)  *cur = curPos;
    return true;
}

// --- line operations: currentLinesUp/Down ---
void ScintillaComponent::currentLinesUp() const
{
    send(SCI_MOVESELECTEDLINESUP);
}

void ScintillaComponent::currentLinesDown() const
{
    send(SCI_MOVESELECTEDLINESDOWN);
    send(SCI_SCROLLRANGE, send(SCI_GETSELECTIONEND), send(SCI_GETSELECTIONSTART));
}

// --- getSelectedCharsAndLinesCount (overload without params) ---
std::pair<size_t, size_t> ScintillaComponent::getSelectedCharsAndLinesCount() const
{
    return getSelectedCharsAndLinesCount(0);
}

// --- columnReplace (numeric sequence) ---
void ScintillaComponent::columnReplaceNumeric(ColumnModeInfos& cmi,
                                              size_t initial, size_t incr,
                                              size_t repeat, int format, int lead) const
{
    if (cmi.empty())
        return;

    // Base format: 0=dec, 1=hex, 2=oct, 3=bin, 4=hex_uppercase
    int base = 10;
    bool useUpper = false;
    if (format == 1)
        base = 16;
    else if (format == 2)
        base = 8;
    else if (format == 3)
        base = 2;
    else if (format == 4)
    {
        base = 16;
        useUpper = true;
    }

    // Compute numbers for each column
    std::vector<size_t> numbers;
    size_t curNumber = initial;
    size_t kiMaxSize = cmi.size();

    while (numbers.size() < kiMaxSize)
    {
        for (size_t i = 0; i < repeat; i++)
        {
            numbers.push_back(curNumber);
            if (numbers.size() >= kiMaxSize)
                break;
        }
        curNumber += incr;
    }

    // Compute max digits needed
    size_t kibInit = 1, kibEnd = 1;
    if (initial > 0)
    {
        kibInit = 0;
        size_t t = initial;
        while (t > 0) { ++kibInit; t /= base; }
    }
    if (!numbers.empty())
    {
        kibEnd = 0;
        size_t t = numbers.back();
        while (t > 0) { ++kibEnd; t /= base; }
    }
    size_t kib = qMax(kibInit, kibEnd);
    if (kib == 0) kib = 1;

    intptr_t totalDiff = 0;
    size_t len = cmi.size();

    for (size_t i = 0; i < len; ++i)
    {
        if (cmi[i].isValid())
        {
            intptr_t len2beReplaced = cmi[i]._selRpos - cmi[i]._selLpos;
            intptr_t diff = static_cast<intptr_t>(kib) - len2beReplaced;

            cmi[i]._selLpos += totalDiff;
            cmi[i]._selRpos += totalDiff;

            bool hasVirtualSpc = (cmi[i]._nbVirtualAnchorSpc > 0 || cmi[i]._nbVirtualCaretSpc > 0);
            if (hasVirtualSpc)
            {
                intptr_t vc = qMax(cmi[i]._nbVirtualAnchorSpc, cmi[i]._nbVirtualCaretSpc);
                intptr_t va = qMin(cmi[i]._nbVirtualAnchorSpc, cmi[i]._nbVirtualCaretSpc);
                for (intptr_t j = 0, k = cmi[i]._selLpos; j < vc; ++j, ++k)
                    send(SCI_INSERTTEXT, static_cast<size_t>(k), reinterpret_cast<sptr_t>(" "));
                cmi[i]._selLpos += va;
                cmi[i]._selRpos += vc;
            }

            // Format number
            QString numStr;
            if (base == 10)
                numStr = QString::number(numbers.at(i));
            else if (base == 16)
                numStr = useUpper ? QString::number(numbers.at(i), 16).toUpper()
                                  : QString::number(numbers.at(i), 16);
            else if (base == 8)
                numStr = QString::number(numbers.at(i), 8);
            else if (base == 2)
                numStr = QString::number(numbers.at(i), 2);

            // Pad with leading zeros if needed
            if (lead == 1 && static_cast<size_t>(numStr.length()) < kib)
                numStr = numStr.rightJustified(static_cast<int>(kib), '0');

            QByteArray utf8 = numStr.toUtf8();
            send(SCI_SETTARGETRANGE, static_cast<size_t>(cmi[i]._selLpos),
                 static_cast<size_t>(cmi[i]._selRpos));
            send(SCI_REPLACETARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));

            if (hasVirtualSpc)
            {
                totalDiff += cmi[i]._nbVirtualAnchorSpc + static_cast<intptr_t>(utf8.size());
                cmi[i]._nbVirtualAnchorSpc = 0;
                cmi[i]._nbVirtualCaretSpc = 0;
            }
            else
            {
                totalDiff += diff;
            }
            cmi[i]._selRpos += diff;
        }
    }
}
