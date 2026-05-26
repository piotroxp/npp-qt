// xmlMatchedTagsHighlighter.h - Qt port (already cross-platform - regex-based)
#pragma once
#include <vector>
#include <string>
#include <regex>
#include "ScintillaEditView.h"
class xmlMatchedTagsHighlighter {
public:
    xmlMatchedTagsHighlighter() : _tagHighlighted(-1), _currentTagStart(-1), _currentTagEnd(-1) {}
    void init(ScintillaEditView* pEditView) { _pEditView = pEditView; }
    void highlight(intPtr_t currentPos, bool isInsertion);
    void clear();
    void removeHighlightTag(intPtr_t startPos, intPtr_t endPos);
    void removeCanceledTag(intPtr_t startPos, intPtr_t endPos);
private:
    static const wchar_t* const TAGRX;
    bool getTagRange(std::wsregex_iterator& it, intPtr_t startPos);
    void highlightClosingTag(const std::wsmatch& match);
    void highlightOpenTag(const std::wsmatch& match);
    void highlightSelfClosingTag(const std::wsmatch& match);
    void highlightOpenTagOnly(const std::wsmatch& match);
    void _highlightClosingTagOnly(const std::wsmatch& match);
    ScintillaEditView* _pEditView = nullptr;
    intPtr_t _tagHighlighted = 0;
    intPtr_t _currentTagStart = 0;
    intPtr_t _currentTagEnd = 0;
    std::vector<intPtr_t> _matchedStarts;
    std::vector<intPtr_t> _matchedEnds;
};
