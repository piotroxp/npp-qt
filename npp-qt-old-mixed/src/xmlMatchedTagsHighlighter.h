// npp-qt: XmlMatchedTagsHighlighter header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/xmlMatchedTagsHighlighter.h
// Target: npp-qt/src/ScintillaComponent/xmlMatchedTagsHighlighter.h

#pragma once

#include <string>
#include <vector>
#include <utility>

class ScintillaComponent;

// XML/HTML tag matching highlighter
// Uses Scintilla's SCI_INDICATORFILLRANGE to highlight matched tags.
// Minimal Win32 dependencies — primarily pure Scintilla API.
class XmlMatchedTagsHighlighter
{
public:
	explicit XmlMatchedTagsHighlighter(ScintillaComponent* pEditView) : _pEditView(pEditView) {}
	~XmlMatchedTagsHighlighter() = default;

	// Main entry point — find and highlight matching XML/HTML tags
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

	// Allowed whitespace characters in XML
	static bool isWhitespace(intptr_t ch) {
		return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
	}

	FindResult findText(const char* text, intptr_t start, intptr_t end, int flags = 0);
	FindResult findOpenTag(const std::string& tagName, intptr_t start, intptr_t end);
	FindResult findCloseTag(const std::string& tagName, intptr_t start, intptr_t end);
	intptr_t findCloseAngle(intptr_t startPosition, intptr_t endPosition);

	std::vector<std::pair<intptr_t, intptr_t>> getAttributesPos(intptr_t start, intptr_t end);
};
