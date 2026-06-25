// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include "SmartHighlighter.h"
#include "ScintillaComponent.h"
#include "FindReplaceDlg.h"
#include "Parameters.h"
#include <cmath>

constexpr int MAXLINEHIGHLIGHT = 400;

SmartHighlighter::SmartHighlighter(FindReplaceDlg* pFRDlg) : _pFRDlg(pFRDlg) {}

void SmartHighlighter::highlightViewWithWord(ScintillaComponent* sci, const std::wstring& word2Hilite)
{
    auto originalStartPos = sci->send(SCI_GETTARGETSTART);
    auto originalEndPos = sci->send(SCI_GETTARGETEND);

    intptr_t firstLine = sci->send(SCI_GETFIRSTVISIBLELINE);
    intptr_t nbLineOnScreen = sci->send(SCI_LINESONSCREEN);
    intptr_t nbLines = qMin<intptr_t>(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
    intptr_t lastLine = firstLine + nbLines;
    intptr_t prevDocLineChecked = -1;

    bool isWordOnly = true;
    bool isCaseSensentive = true;
    const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

    if (nppGUI._smartHiliteUseFindSettings) {
        const FindHistory& findHistory = NppParameters::getInstance().getFindHistory();
        isWordOnly = findHistory._isMatchWord;
        isCaseSensentive = findHistory._isMatchCase;
    } else {
        isWordOnly = nppGUI._smartHiliteWordOnly;
        isCaseSensentive = nppGUI._smartHiliteCaseSensitive;
    }

    int flags = (isCaseSensentive ? SCFIND_MATCHCASE : 0) | (isWordOnly ? SCFIND_WHOLEWORD : 0);

    for (intptr_t currentLine = firstLine; currentLine < lastLine; ++currentLine) {
        intptr_t docLine = sci->send(SCI_DOCLINEFROMVISIBLE, currentLine);
        if (docLine == prevDocLineChecked) continue;
        prevDocLineChecked = docLine;

        size_t startPos = sci->send(SCI_POSITIONFROMLINE, docLine);
        size_t endPos = sci->send(SCI_POSITIONFROMLINE, docLine + 1);
        if (endPos == size_t(-1)) {
            endPos = sci->send(SCI_GETLENGTH);
            _pFRDlg->processRange(ProcessMarkAll_2, word2Hilite, nullptr, flags, -1, sci);
            break;
        } else {
            _pFRDlg->processRange(ProcessMarkAll_2, word2Hilite, nullptr, flags, -1, sci);
        }
    }

    sci->send(SCI_SETTARGETSTART, originalStartPos);
    sci->send(SCI_SETTARGETEND, originalEndPos);
}

void SmartHighlighter::highlightView(ScintillaComponent* sci, ScintillaComponent* unfocusView)
{
    sci->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);

    const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
    if (!nppGUI._enableSmartHilite || sci->send(SCI_GETSELECTIONEMPTY)) {
        if (nppGUI._smartHiliteOnAnotherView && unfocusView && unfocusView->isVisible()
            && unfocusView->currentBuffer() != sci->currentBuffer()) {
            unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
        }
        return;
    }

    auto curPos = sci->send(SCI_GETCURRENTPOS);
    bool isWordOnly = nppGUI._smartHiliteUseFindSettings
        ? NppParameters::getInstance().getFindHistory()._isMatchWord
        : nppGUI._smartHiliteWordOnly;

    if (isWordOnly) {
        auto wordStart = sci->send(SCI_WORDSTARTPOSITION, curPos, true);
        auto wordEnd = sci->send(SCI_WORDENDPOSITION, wordStart, true);
        if (wordStart == wordEnd) return;
    }

    // Get selected text as QString, convert to wstring
    QString selText = sci->getSelectedText();
    if (selText.isEmpty()) return;
    std::wstring text2Find = selText.toStdWString();

    highlightViewWithWord(sci, text2Find);

    if (nppGUI._smartHiliteOnAnotherView && unfocusView && unfocusView->isVisible()) {
        if (unfocusView->currentBuffer() != sci->currentBuffer())
            unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
        highlightViewWithWord(unfocusView, text2Find);
    }
}
