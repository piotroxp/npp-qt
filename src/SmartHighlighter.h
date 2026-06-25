// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <QString>
#include <string>

class FindReplaceDlg;
class ScintillaComponent;

class SmartHighlighter
{
public:
    explicit SmartHighlighter(FindReplaceDlg* pFRDlg);
    void highlightView(ScintillaComponent* pHighlightView, ScintillaComponent* unfocusView);
    void highlightViewWithWord(ScintillaComponent* pHighlightView, const std::wstring& word2Hilite);
private:
    FindReplaceDlg* _pFRDlg = nullptr;
};
