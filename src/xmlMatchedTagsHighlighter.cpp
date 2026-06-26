// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of xmlMatchedTagsHighlighter.cpp — XML/HTML tag matching
// Win32→Qt6: SCI_FINDTEXTFULL → send(SCI_FINDTEXTFULL,...)
// The algorithm is identical to Win32; only the Scintilla call API changes.
// Win32: GetFileAttributesEx/ShPathApi... → QFileInfo/QDir
#include "xmlMatchedTagsHighlighter.h"
#include "ScintillaComponent.h"
#include <QFileInfo>
#include <QDir>

using namespace std;

XmlMatchedTagsHighlighter::XmlMatchedTagsHighlighter(ScintillaComponent* pEditView)
    : _pEditView(pEditView)
{}

vector<pair<intptr_t, intptr_t>> XmlMatchedTagsHighlighter::getAttributesPos(intptr_t start, intptr_t end)
{
    vector<pair<intptr_t, intptr_t>> attributes;
    intptr_t bufLen = end - start + 1;
    if (bufLen <= 0 || bufLen > 100000) return attributes; // sanity check

    QByteArray buf(bufLen + 1, '\0');
    if (!_pEditView) return attributes;
    // Use Scintilla to get text range
    _pEditView->send(SCI_GETTEXTRANGE, start,
        reinterpret_cast<sptr_t>(buf.data()));
    buf.data()[bufLen] = '\0';

    enum State { attr_invalid, attr_key, attr_pre_assign, attr_assign,
                 attr_string, attr_single_quot_string, attr_value, attr_valid };
    State state = attr_invalid;
    int startPos = -1;
    int oneMoreChar = 1;

    for (int i = 0; i < bufLen; ++i) {
        char ch = buf.data()[i];
        switch (ch) {
            case ' ': case '\t': case '\n': case '\r':
                if (state == attr_key) state = attr_pre_assign;
                else if (state == attr_value) { state = attr_valid; oneMoreChar = 0; }
                break;
            case '=':
                if (state == attr_key || state == attr_pre_assign) state = attr_assign;
                else if (state == attr_assign || state == attr_value) state = attr_invalid;
                break;
            case '"':
                if (state == attr_string) { state = attr_valid; oneMoreChar = 1; }
                else if (state == attr_key || state == attr_pre_assign || state == attr_value) state = attr_invalid;
                else if (state == attr_assign) state = attr_string;
                break;
            case '\'':
                if (state == attr_single_quot_string) { state = attr_valid; oneMoreChar = 1; }
                else if (state == attr_key || state == attr_pre_assign || state == attr_value) state = attr_invalid;
                else if (state == attr_assign) state = attr_single_quot_string;
                break;
            default:
                if (state == attr_invalid) { state = attr_key; startPos = i; }
                else if (state == attr_pre_assign) state = attr_invalid;
                else if (state == attr_assign) state = attr_value;
        }
        if (state == attr_valid) {
            attributes.push_back({start + startPos, start + i + oneMoreChar});
            state = attr_invalid;
        }
    }
    if (state == attr_value)
        attributes.push_back({start + startPos, start + bufLen - 1});
    return attributes;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findOpenTag(
    const string& tagName, intptr_t start, intptr_t end)
{
    string search = "<" + tagName;
    FindResult result;
    result.success = false;
    intptr_t styleAt;
    intptr_t searchStart = start;
    intptr_t searchEnd = end;
    bool forwardSearch = (start < end);
    FindResult r;  // declared outside loop so it survives past the loop scope

    do {
        r = findText(search.c_str(), searchStart, searchEnd, 0);
        if (r.success) {
            intptr_t nextChar = _pEditView ? _pEditView->send(SCI_GETCHARAT, r.end) : 0;
            styleAt = _pEditView ? _pEditView->send(SCI_GETSTYLEAT, r.start) : 0;
            if (styleAt != 4 && styleAt != 5 && styleAt != 6 && styleAt != 9) { // not in string/comment
                if (nextChar == '>') {
                    result = r; result.success = true; result.end = r.end + 1;
                    break;
                } else if (isWhitespace(nextChar)) {
                    intptr_t closeAngle = findCloseAngle(r.end, forwardSearch ? end : start);
                    if (closeAngle != -1 && _pEditView && _pEditView->send(SCI_GETCHARAT, closeAngle - 1) != '/')
                        { result = r; result.success = true; result.end = closeAngle + 1; break; }
                }
            }
        }
        if (r.success) searchStart = forwardSearch ? r.end + 1 : r.start - 1;
    } while (r.success && !result.success);
    if (!result.success && r.success) { result.start = r.start; result.end = r.end; }
    return result;
}

intptr_t XmlMatchedTagsHighlighter::findCloseAngle(intptr_t startPosition, intptr_t endPosition)
{
    if (startPosition > endPosition) swap(startPosition, endPosition);
    if (!_pEditView) return -1;
    intptr_t searchStart = startPosition;
    while (searchStart < endPosition) {
        FindResult r = findText(">", searchStart, endPosition, 0);
        if (!r.success) break;
        intptr_t style = _pEditView->send(SCI_GETSTYLEAT, r.start);
        if (style != 4 && style != 5 && style != 6 && style != 9) // not in string/comment
            return r.start;
        searchStart = r.end + 1;
    }
    return -1;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findCloseTag(
    const string& tagName, intptr_t start, intptr_t end)
{
    string search = "</" + tagName;
    FindResult result;
    result.success = false;
    intptr_t searchStart = start;
    intptr_t searchEnd = end;
    bool forwardSearch = (start < end);

    do {
        FindResult r = findText(search.c_str(), searchStart, searchEnd, 0);
        if (!r.success) break;
        intptr_t nextChar = _pEditView ? _pEditView->send(SCI_GETCHARAT, r.end) : 0;
        intptr_t styleAt = _pEditView ? _pEditView->send(SCI_GETSTYLEAT, r.start) : 0;
        if (styleAt != 4 && styleAt != 5 && styleAt != 6 && styleAt != 9) {
            if (nextChar == '>') {
                result = r; result.success = true; result.end = r.end + 1; break;
            } else if (isWhitespace(nextChar)) {
                intptr_t wp = r.end;
                while (_pEditView && wp < end && isWhitespace(_pEditView->send(SCI_GETCHARAT, wp))) wp++;
                if (_pEditView && _pEditView->send(SCI_GETCHARAT, wp) == '>')
                    { result = r; result.success = true; result.end = wp + 1; break; }
            }
        }
        searchStart = forwardSearch ? r.end + 1 : r.start - 1;
    } while (true);
    return result;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findText(
    const char* text, intptr_t start, intptr_t end, int flags)
{
    FindResult returnValue;
    returnValue.success = false;
    if (!_pEditView) return returnValue;
    _pEditView->send(SCI_SETTARGETSTART, start);
    _pEditView->send(SCI_SETTARGETEND, end);
    intptr_t result = _pEditView->send(SCI_SEARCHINTARGET, strlen(text), reinterpret_cast<sptr_t>(text));
    if (result == -1) return returnValue;
    returnValue.success = true;
    returnValue.start = _pEditView->send(SCI_GETTARGETSTART);
    returnValue.end = _pEditView->send(SCI_GETTARGETEND);
    return returnValue;
}

bool XmlMatchedTagsHighlighter::getXmlMatchedTagsPos(XmlMatchedTagsPos& xmlTags)
{
    if (!_pEditView) return false;
    bool tagFound = false;
    intptr_t caret = _pEditView->send(SCI_GETCURRENTPOS);
    intptr_t searchStartPoint = caret;
    FindResult openFound = findText("<", searchStartPoint, 0, 0);
    intptr_t styleAt = _pEditView->send(SCI_GETSTYLEAT, openFound.start);
    searchStartPoint = openFound.start - 1;
    while (openFound.success &&
           (styleAt == 4 || styleAt == 5 || styleAt == 6 || styleAt == 9) &&
           searchStartPoint > 0) {
        openFound = findText("<", searchStartPoint, 0, 0);
        styleAt = _pEditView->send(SCI_GETSTYLEAT, openFound.start);
        searchStartPoint = openFound.start - 1;
    }
    if (!openFound.success || styleAt == 4) return false; // in CDATA

    FindResult closeFound = findText(">", openFound.start, caret, 0);
    intptr_t closeStyle = _pEditView->send(SCI_GETSTYLEAT, closeFound.start);
    intptr_t csStart = closeFound.start;
    while (closeFound.success &&
           (closeStyle == 4 || closeStyle == 5 || closeStyle == 6 || closeStyle == 9) &&
           csStart <= caret) {
        csStart = closeFound.end;
        closeFound = findText(">", csStart, caret, 0);
        closeStyle = _pEditView->send(SCI_GETSTYLEAT, closeFound.start);
    }

    if (!closeFound.success) {
        intptr_t nextChar = _pEditView->send(SCI_GETCHARAT, openFound.start + 1);
        if (nextChar == '/') {
            // Cursor in close tag
            xmlTags.tagCloseStart = openFound.start;
            intptr_t docLen = _pEditView->send(SCI_GETLENGTH);
            FindResult endClose = findText(">", caret, docLen, 0);
            if (endClose.success) xmlTags.tagCloseEnd = endClose.end;
            intptr_t position = openFound.start + 2;
            intptr_t docLen2 = _pEditView->send(SCI_GETLENGTH);
            string tagName;
            nextChar = _pEditView->send(SCI_GETCHARAT, position);
            while (position < docLen2 && !isWhitespace(nextChar) && nextChar != '/' && nextChar != '>' && nextChar != '"' && nextChar != '\'') {
                tagName.push_back(static_cast<char>(nextChar));
                ++position;
                nextChar = _pEditView->send(SCI_GETCHARAT, position);
            }
            if (!tagName.empty()) {
                intptr_t currentEndPoint = xmlTags.tagCloseStart;
                intptr_t openTagsRemaining = 1;
                FindResult nextOpenTag;
                do {
                    nextOpenTag = findOpenTag(tagName, currentEndPoint, 0);
                    if (nextOpenTag.success) {
                        --openTagsRemaining;
                        FindResult inbetweenClose;
                        intptr_t currentStartPos = nextOpenTag.end;
                        intptr_t closeTagsFound = 0;
                        bool forward = (currentStartPos < currentEndPoint);
                        do {
                            inbetweenClose = findCloseTag(tagName, currentStartPos, currentEndPoint);
                            if (inbetweenClose.success) {
                                ++closeTagsFound;
                                currentStartPos = forward ? inbetweenClose.end : inbetweenClose.start - 1;
                            }
                        } while (inbetweenClose.success);
                        if (closeTagsFound == 0 && openTagsRemaining == 0) {
                            xmlTags.tagOpenStart = nextOpenTag.start;
                            xmlTags.tagOpenEnd = nextOpenTag.end + 1;
                            xmlTags.tagNameEnd = nextOpenTag.start + static_cast<int>(tagName.size()) + 1;
                            tagFound = true;
                        } else {
                            openTagsRemaining += closeTagsFound;
                            currentEndPoint = nextOpenTag.start;
                        }
                    }
                } while (!tagFound && openTagsRemaining > 0 && nextOpenTag.success);
            }
        } else {
            // Cursor in open tag
            xmlTags.tagOpenStart = openFound.start;
            intptr_t position = openFound.start + 1;
            intptr_t docLen = _pEditView->send(SCI_GETLENGTH);
            string tagName;
            nextChar = _pEditView->send(SCI_GETCHARAT, position);
            while (position < docLen && !isWhitespace(nextChar) && nextChar != '/' && nextChar != '>' && nextChar != '"' && nextChar != '\'') {
                tagName.push_back(static_cast<char>(nextChar));
                ++position;
                nextChar = _pEditView->send(SCI_GETCHARAT, position);
            }
            if (!tagName.empty()) {
                xmlTags.tagNameEnd = openFound.start + static_cast<int>(tagName.size()) + 1;
                intptr_t closeAnglePos = findCloseAngle(position, docLen);
                if (closeAnglePos != -1) {
                    xmlTags.tagOpenEnd = closeAnglePos + 1;
                    if (_pEditView->send(SCI_GETCHARAT, closeAnglePos - 1) == '/') {
                        xmlTags.tagCloseEnd = -1;
                        xmlTags.tagCloseStart = -1;
                        tagFound = true;
                    } else {
                        intptr_t currentStartPos = xmlTags.tagOpenEnd;
                        intptr_t closeTagsRemaining = 1;
                        FindResult nextCloseTag;
                        do {
                            nextCloseTag = findCloseTag(tagName, currentStartPos, docLen);
                            if (nextCloseTag.success) {
                                --closeTagsRemaining;
                                FindResult inbetweenOpen;
                                intptr_t currentEndPos = nextCloseTag.start;
                                intptr_t openTagsFound = 0;
                                do {
                                    inbetweenOpen = findOpenTag(tagName, currentStartPos, currentEndPos);
                                    if (inbetweenOpen.success) { ++openTagsFound; currentStartPos = inbetweenOpen.end; }
                                } while (inbetweenOpen.success);
                                if (openTagsFound == 0 && closeTagsRemaining == 0) {
                                    xmlTags.tagCloseStart = nextCloseTag.start;
                                    xmlTags.tagCloseEnd = nextCloseTag.end + 1;
                                    tagFound = true;
                                } else {
                                    closeTagsRemaining += openTagsFound;
                                    currentStartPos = nextCloseTag.end;
                                }
                            }
                        } while (!tagFound && closeTagsRemaining > 0 && nextCloseTag.success);
                    }
                }
            }
        }
    }
    return tagFound;
}

void XmlMatchedTagsHighlighter::tagMatch(bool doHiliteAttr)
{
    if (!_pEditView) return;
    _pEditView->clearIndicator(31); // SCE_UNIVERSAL_TAGMATCH
    _pEditView->clearIndicator(32); // SCE_UNIVERSAL_TAGATTR

    // Only works for markup languages
    // Get language from buffer (stub - just check if it's XML-like)
    Q_UNUSED(doHiliteAttr);

    XmlMatchedTagsPos xmlTags;
    if (getXmlMatchedTagsPos(xmlTags)) {
        _pEditView->send(SCI_SETINDICATORCURRENT, 31);
        int openTagTailLen = 2;
        if (xmlTags.tagCloseStart != -1 && xmlTags.tagCloseEnd != -1) {
            _pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagCloseStart, xmlTags.tagCloseEnd - xmlTags.tagCloseStart);
            openTagTailLen = 1;
        }
        _pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagOpenStart, xmlTags.tagNameEnd - xmlTags.tagOpenStart);
        _pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagOpenEnd - openTagTailLen, openTagTailLen);

        if (doHiliteAttr && xmlTags.tagOpenEnd > xmlTags.tagNameEnd && xmlTags.tagOpenEnd > 0) {
            vector<pair<intptr_t, intptr_t>> attrs = getAttributesPos(xmlTags.tagNameEnd, xmlTags.tagOpenEnd - openTagTailLen);
            _pEditView->send(SCI_SETINDICATORCURRENT, 32);
            for (auto& attr : attrs)
                _pEditView->send(SCI_INDICATORFILLRANGE, attr.first, attr.second - attr.first);
        }

        if (_pEditView->send(SCI_GETINDENTATIONGUIDES) != 0 && xmlTags.tagCloseStart != -1) {
            intptr_t colAtCaret = _pEditView->send(SCI_GETCOLUMN, xmlTags.tagOpenStart);
            intptr_t colOpposite = _pEditView->send(SCI_GETCOLUMN, xmlTags.tagCloseStart);
            intptr_t lineAtCaret = _pEditView->send(SCI_LINEFROMPOSITION, xmlTags.tagOpenStart);
            intptr_t lineOpposite = _pEditView->send(SCI_LINEFROMPOSITION, xmlTags.tagCloseStart);
            if (xmlTags.tagCloseStart != -1 && lineAtCaret != lineOpposite)
                _pEditView->send(SCI_BRACEHIGHLIGHT, xmlTags.tagOpenStart, xmlTags.tagCloseEnd - 1);
        }
    }
}
