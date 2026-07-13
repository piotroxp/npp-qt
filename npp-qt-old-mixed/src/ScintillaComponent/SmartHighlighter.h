// npp-qt: SmartHighlighter class — original: ScintillaComponent/SmartHighlighter.h
// Win32→Qt6 semantic lift: ScintillaEditView* → ScintillaComponent*
#pragma once

#include <string>

// Forward declaration — breaks circular include with ScintillaComponent.h
class ScintillaComponent;
class FindReplaceDlg;

// SmartHighlighter — highlights words in the editor view
// Translates from Win32: Scintilla_Execute(SCI_GETTARGETSTART/END) → send(SCI_GETTARGETSTART/END)
class SmartHighlighter
{
public:
    explicit SmartHighlighter(FindReplaceDlg* pFRDlg);
    void highlightView(ScintillaComponent* pHighlightView, ScintillaComponent* unfocusView);
    void highlightViewWithWord(ScintillaComponent* pHighlightView, const std::wstring& word2Hilite);

private:
    // Internal helper: searches and applies highlight indicator
    void searchAndHighlight(ScintillaComponent* pHighlightView, const QByteArray& textUtf8);

    FindReplaceDlg* _pFRDlg = nullptr;
};
