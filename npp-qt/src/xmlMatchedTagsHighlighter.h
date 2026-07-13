// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
// xmlMatchedTagsHighlighter: highlights matching XML/HTML tags
// Win32→Qt6: SH* path APIs → Qt equivalents; SCI_FINDTEXTFULL → send(SCI_FINDTEXTFULL,...)
#include <vector>
#include <utility>
#include "ScintillaComponent.h"

class XmlMatchedTagsHighlighter
{
public:
    explicit XmlMatchedTagsHighlighter(ScintillaComponent* pEditView);
    void tagMatch(bool doHiliteAttr);

private:
    ScintillaComponent* _pEditView = nullptr;

    struct XmlMatchedTagsPos {
        intptr_t tagOpenStart = 0;
        intptr_t tagNameEnd = 0;
        intptr_t tagOpenEnd = 0;
        intptr_t tagCloseStart = 0;
        intptr_t tagCloseEnd = 0;
    };

    struct FindResult {
        intptr_t start = 0;
        intptr_t end = 0;
        bool success = false;
    };

    bool getXmlMatchedTagsPos(XmlMatchedTagsPos& tagsPos);
    static bool isWhitespace(intptr_t ch) { return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'; }
    FindResult findText(const char* text, intptr_t start, intptr_t end, int flags = 0);
    FindResult findOpenTag(const std::string& tagName, intptr_t start, intptr_t end);
    FindResult findCloseTag(const std::string& tagName, intptr_t start, intptr_t end);
    intptr_t findCloseAngle(intptr_t startPosition, intptr_t endPosition);
    std::vector<std::pair<intptr_t, intptr_t>> getAttributesPos(intptr_t start, intptr_t end);
};
