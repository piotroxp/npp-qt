// npp-qt: Win32→Qt6 semantic lift
// Original: PowerEditor/src/ScintillaComponent/SmartHighlighter.cpp
// Target:   npp-qt/src/ScintillaComponent/SmartHighlighter.cpp
//
// Translates Win32 Scintilla messages to Qt6 ScintillaComponent method calls.
// Win32: pHighlightView->execute(SCI_GETTARGETSTART)
// Qt6:   pHighlightView->send(SCI_GETTARGETSTART)

#include "SmartHighlighter.h"
#include "ScintillaComponent.h"
#include <QString>
#include <QByteArray>
#include <algorithm>

// Forward declaration of FindReplaceDlg methods needed
// (FindReplaceDlg.h is included via SmartHighlighter.h)

// Maximum lines to highlight (prevents performance issues with large files)
static constexpr int MAXLINEHIGHLIGHT = 400;

SmartHighlighter::SmartHighlighter(FindReplaceDlg* pFRDlg)
    : _pFRDlg(pFRDlg)
{
    // Nothing to do — just store the find/replace dialog pointer
}

void SmartHighlighter::highlightView(ScintillaComponent* pHighlightView, ScintillaComponent* unfocusView)
{
    if (!pHighlightView)
        return;

    // Clear existing smart highlight markers
    pHighlightView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);

    // Check if smart highlighting is enabled
    // TODO: Replace with actual NppParameters access in Qt6
    // const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
    // if (!nppGUI._enableSmartHilite)
    //     return;

    // Check if something is selected
    if (pHighlightView->send(SCI_GETSELECTIONEMPTY) == 1)
    {
        // Clear the other view if it's different buffer
        if (unfocusView && unfocusView->isVisible() &&
            unfocusView->currentBuffer() != pHighlightView->currentBuffer())
        {
            unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
        }
        return;
    }

    // Get current position and selection
    auto curPos = pHighlightView->send(SCI_GETCURRENTPOS);
    Sci_CharacterRangeFull range = pHighlightView->getSelection();
    intptr_t textlen = range.cpMax - range.cpMin;

    if (textlen <= 0)
        return;

    // Get selected text
    QString selectedText = pHighlightView->getSelectedText();
    if (selectedText.isEmpty())
        return;

    // Highlight the focused view
    highlightViewWithWord(pHighlightView, selectedText.toStdWString());

    // Highlight the unfocused view if configured
    // TODO: Check nppGUI._smartHiliteOnAnotherView
    if (unfocusView && unfocusView->isVisible())
    {
        if (unfocusView->currentBuffer() != pHighlightView->currentBuffer())
            unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);

        highlightViewWithWord(unfocusView, selectedText.toStdWString());
    }
}

void SmartHighlighter::highlightViewWithWord(ScintillaComponent* pHighlightView, const std::wstring& word2Hilite)
{
    if (!pHighlightView || word2Hilite.empty())
        return;

    // Save target locations for other search functions
    auto originalStartPos = pHighlightView->send(SCI_GETTARGETSTART);
    auto originalEndPos = pHighlightView->send(SCI_GETTARGETEND);

    // Get the range of visible text
    intptr_t firstLine = pHighlightView->send(SCI_GETFIRSTVISIBLELINE);
    intptr_t nbLineOnScreen = pHighlightView->send(SCI_LINESONSCREEN);
    intptr_t nbLines = std::min<intptr_t>(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
    intptr_t lastLine = firstLine + nbLines;
    intptr_t prevDocLineChecked = -1;

    // Convert wide string to UTF-8 for Scintilla
    QString wordQ = QString::fromStdWString(word2Hilite);
    QByteArray wordUtf8 = wordQ.toUtf8();

    // Determine search mode
    // TODO: Read from FindHistory settings via NppParameters
    bool isWordOnly = true;
    bool isCaseSensitive = true;

    unsigned int searchFlags = 0;
    if (isWordOnly)
        searchFlags |= SCFIND_WHOLEWORD;
    if (!isCaseSensitive)
        searchFlags |= SCFIND_MATCHCASE;
    pHighlightView->send(SCI_SETSEARCHFLAGS, searchFlags);

    for (intptr_t currentLine = firstLine; currentLine < lastLine; ++currentLine)
    {
        intptr_t docLine = pHighlightView->send(SCI_DOCLINEFROMVISIBLE, currentLine);
        if (docLine == prevDocLineChecked)
            continue;
        prevDocLineChecked = static_cast<intptr_t>(docLine);

        size_t startPos = pHighlightView->send(SCI_POSITIONFROMLINE, static_cast<size_t>(docLine));
        size_t endPos = pHighlightView->send(SCI_POSITIONFROMLINE, static_cast<size_t>(docLine + 1));

        if (endPos == 0 && docLine == 0)
        {
            // Empty document
            endPos = pHighlightView->send(SCI_GETLENGTH);
        }

        if (endPos == static_cast<size_t>(-1) || endPos < startPos)
        {
            // Past EOF
            endPos = pHighlightView->send(SCI_GETLENGTH);
            if (endPos > startPos)
            {
                // Search the remaining text
                pHighlightView->send(SCI_SETTARGETSTART, startPos);
                pHighlightView->send(SCI_SETTARGETEND, endPos);
                searchAndHighlight(pHighlightView, wordUtf8);
            }
            break;
        }
        else
        {
            pHighlightView->send(SCI_SETTARGETSTART, startPos);
            pHighlightView->send(SCI_SETTARGETEND, endPos);
            searchAndHighlight(pHighlightView, wordUtf8);
        }
    }

    // Restore original targets
    pHighlightView->send(SCI_SETTARGETSTART, originalStartPos);
    pHighlightView->send(SCI_SETTARGETEND, originalEndPos);
}

void SmartHighlighter::searchAndHighlight(ScintillaComponent* pHighlightView, const QByteArray& text)
{
    if (!pHighlightView)
        return;

    intptr_t pos = pHighlightView->send(SCI_SEARCHINTARGET, text.size(),
                                        reinterpret_cast<sptr_t>(text.constData()));

    while (pos >= 0)
    {
        size_t targetStart = pHighlightView->send(SCI_GETTARGETSTART);
        size_t targetEnd = pHighlightView->send(SCI_GETTARGETEND);

        if (targetEnd > targetStart)
        {
            // Apply smart highlight indicator
            pHighlightView->send(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
            pHighlightView->send(SCI_INDICATORFILLRANGE, targetStart, targetEnd - targetStart);
        }

        // Search for next match
        pHighlightView->send(SCI_SETTARGETSTART, targetEnd);
        pos = pHighlightView->send(SCI_SEARCHINTARGET, text.size(),
                                    reinterpret_cast<sptr_t>(text.constData()));
    }
}
