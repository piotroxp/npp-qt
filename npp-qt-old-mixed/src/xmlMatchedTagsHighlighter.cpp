// npp-qt: XmlMatchedTagsHighlighter implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/xmlMatchedTagsHighlighter.cpp
// Target: npp-qt/src/ScintillaComponent/xmlMatchedTagsHighlighter.cpp
//
// Tags matching routine rewritten by Dave Brotherstone (May 2012)
// to remove need for regular expression searches.

#include "xmlMatchedTagsHighlighter.h"
#include "ScintillaComponent.h"
#include "Buffer.h"

#include <QFileInfo>

#include <SciLexer.h>

using namespace std;

std::vector<std::pair<intptr_t, intptr_t>> XmlMatchedTagsHighlighter::getAttributesPos(intptr_t start, intptr_t end)
{
	vector<pair<intptr_t, intptr_t>> attributes;

	intptr_t bufLen = end - start + 1;
	char* buf = new char[bufLen + 1];
	_pEditView->getText(buf, start, end);

	enum {
		attr_invalid,
		attr_key,
		attr_pre_assign,
		attr_assign,
		attr_string,
		attr_single_quot_string,
		attr_value,
		attr_valid
	} state = attr_invalid;

	int startPos = -1;
	int oneMoreChar = 1;
	int i = 0;
	for (; i < bufLen; ++i)
	{
		switch (buf[i])
		{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (state == attr_key)
					state = attr_pre_assign;
				else if (state == attr_value)
				{
					state = attr_valid;
					oneMoreChar = 0;
				}
				break;
			case '=':
				if (state == attr_key || state == attr_pre_assign)
					state = attr_assign;
				else if (state == attr_assign || state == attr_value)
					state = attr_invalid;
				break;
			case '"':
				if (state == attr_string)
				{
					state = attr_valid;
					oneMoreChar = 1;
				}
				else if (state == attr_key || state == attr_pre_assign || state == attr_value)
					state = attr_invalid;
				else if (state == attr_assign)
					state = attr_string;
				break;
			case '\'':
				if (state == attr_single_quot_string)
				{
					state = attr_valid;
					oneMoreChar = 1;
				}
				else if (state == attr_key || state == attr_pre_assign || state == attr_value)
					state = attr_invalid;
				else if (state == attr_assign)
					state = attr_single_quot_string;
				break;
			default:
				if (state == attr_invalid)
				{
					state = attr_key;
					startPos = i;
				}
				else if (state == attr_pre_assign)
					state = attr_invalid;
				else if (state == attr_assign)
					state = attr_value;
		}

		if (state == attr_valid)
		{
			attributes.push_back(pair<intptr_t, intptr_t>(start + startPos, start + i + oneMoreChar));
			state = attr_invalid;
		}
	}

	if (state == attr_value)
		attributes.push_back(pair<intptr_t, intptr_t>(start + startPos, start + i - 1));

	delete[] buf;
	return attributes;
}

bool XmlMatchedTagsHighlighter::getXmlMatchedTagsPos(XmlMatchedTagsPos& xmlTags)
{
	bool tagFound = false;
	intptr_t caret = _pEditView->send(SCI_GETCURRENTPOS);
	intptr_t searchStartPoint = caret;
	long long styleAt;
	FindResult openFound;

	// Search back for the previous open angle bracket
	do
	{
		openFound = findText("<", searchStartPoint, 0, 0);
		styleAt = _pEditView->send(SCI_GETSTYLEAT, openFound.start);
		searchStartPoint = openFound.start - 1;
	} while (openFound.success &&
	         (styleAt == SCE_H_DOUBLESTRING || styleAt == SCE_H_SINGLESTRING || styleAt == SCE_H_COMMENT) &&
	         searchStartPoint > 0);

	if (openFound.success && styleAt != SCE_H_CDATA)
	{
		// Found the "<" before the caret, now check there's no > between that position and the caret
		FindResult closeFound;
		searchStartPoint = openFound.start;
		do
		{
			closeFound = findText(">", searchStartPoint, caret, 0);
			styleAt = _pEditView->send(SCI_GETSTYLEAT, closeFound.start);
			searchStartPoint = closeFound.end;
		} while (closeFound.success &&
		         (styleAt == SCE_H_DOUBLESTRING || styleAt == SCE_H_SINGLESTRING || styleAt == SCE_H_COMMENT) &&
		         searchStartPoint <= caret);

		if (!closeFound.success)
		{
			intptr_t nextChar = _pEditView->send(SCI_GETCHARAT, openFound.start + 1);

			// Cursor in close tag
			if ('/' == nextChar)
			{
				xmlTags.tagCloseStart = openFound.start;
				intptr_t docLength = _pEditView->send(SCI_GETLENGTH);
				FindResult endCloseTag = findText(">", caret, docLength, 0);
				if (endCloseTag.success)
					xmlTags.tagCloseEnd = endCloseTag.end;

				intptr_t position = openFound.start + 2;
				std::string tagName;
				nextChar = _pEditView->send(SCI_GETCHARAT, position);
				while (position < docLength && !isWhitespace(nextChar) && nextChar != '/' &&
				       nextChar != '>' && nextChar != '"' && nextChar != '\'')
				{
					tagName.push_back(static_cast<char>(nextChar));
					++position;
					nextChar = _pEditView->send(SCI_GETCHARAT, position);
				}

				if (!tagName.empty())
				{
					intptr_t currentEndPoint = xmlTags.tagCloseStart;
					intptr_t openTagsRemaining = 1;
					FindResult nextOpenTag;
					do
					{
						nextOpenTag = findOpenTag(tagName, currentEndPoint, 0);
						if (nextOpenTag.success)
						{
							--openTagsRemaining;
							intptr_t currentStartPosition = nextOpenTag.end;
							intptr_t closeTagsFound = 0;
							bool forwardSearch = (currentStartPosition < currentEndPoint);

							FindResult inbetweenCloseTag;
							do
							{
								inbetweenCloseTag = findCloseTag(tagName, currentStartPosition, currentEndPoint);
								if (inbetweenCloseTag.success)
								{
									++closeTagsFound;
									if (forwardSearch)
										currentStartPosition = inbetweenCloseTag.end;
									else
										currentStartPosition = inbetweenCloseTag.start - 1;
								}
							} while (inbetweenCloseTag.success);

							if (closeTagsFound == 0 && openTagsRemaining == 0)
							{
								xmlTags.tagOpenStart = nextOpenTag.start;
								xmlTags.tagOpenEnd = nextOpenTag.end + 1;
								xmlTags.tagNameEnd = nextOpenTag.start + static_cast<int32_t>(tagName.size()) + 1;
								tagFound = true;
							}
							else
							{
								openTagsRemaining += closeTagsFound;
								currentEndPoint = nextOpenTag.start;
							}
						}
					} while (!tagFound && openTagsRemaining > 0 && nextOpenTag.success);
				}
			}
			else
			{
				// Cursor in open tag
				intptr_t position = openFound.start + 1;
				intptr_t docLength = _pEditView->send(SCI_GETLENGTH);
				xmlTags.tagOpenStart = openFound.start;
				std::string tagName;
				nextChar = _pEditView->send(SCI_GETCHARAT, position);
				while (position < docLength && !isWhitespace(nextChar) && nextChar != '/' &&
				       nextChar != '>' && nextChar != '"' && nextChar != '\'')
				{
					tagName.push_back(static_cast<char>(nextChar));
					++position;
					nextChar = _pEditView->send(SCI_GETCHARAT, position);
				}

				if (!tagName.empty())
				{
					xmlTags.tagNameEnd = openFound.start + static_cast<int32_t>(tagName.size()) + 1;
					intptr_t closeAnglePosition = findCloseAngle(position, docLength);
					if (-1 != closeAnglePosition)
					{
						xmlTags.tagOpenEnd = closeAnglePosition + 1;
						if (_pEditView->send(SCI_GETCHARAT, closeAnglePosition - 1) == '/')
						{
							xmlTags.tagCloseEnd = -1;
							xmlTags.tagCloseStart = -1;
							tagFound = true;
						}
						else
						{
							intptr_t currentStartPosition = xmlTags.tagOpenEnd;
							intptr_t closeTagsRemaining = 1;
							FindResult nextCloseTag;
							do
							{
								nextCloseTag = findCloseTag(tagName, currentStartPosition, docLength);
								if (nextCloseTag.success)
								{
									--closeTagsRemaining;
									intptr_t currentEndPosition = nextCloseTag.start;
									intptr_t openTagsFound = 0;
									FindResult inbetweenOpenTag;
									do
									{
										inbetweenOpenTag = findOpenTag(tagName, currentStartPosition, currentEndPosition);
										if (inbetweenOpenTag.success)
										{
											++openTagsFound;
											currentStartPosition = inbetweenOpenTag.end;
										}
									} while (inbetweenOpenTag.success);

									if (openTagsFound == 0 && closeTagsRemaining == 0)
									{
										xmlTags.tagCloseStart = nextCloseTag.start;
										xmlTags.tagCloseEnd = nextCloseTag.end + 1;
										tagFound = true;
									}
									else
									{
										closeTagsRemaining += openTagsFound;
										currentStartPosition = nextCloseTag.end;
									}
								}
							} while (!tagFound && closeTagsRemaining > 0 && nextCloseTag.success);
						}
					}
				}
			}
		}
	}
	return tagFound;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findOpenTag(const std::string& tagName,
                                                                           intptr_t start, intptr_t end)
{
	std::string search("<");
	search.append(tagName);
	FindResult openTagFound;
	openTagFound.success = false;
	FindResult result;
	intptr_t nextChar = 0;
	intptr_t styleAt;
	intptr_t searchStart = start;
	intptr_t searchEnd = end;
	bool forwardSearch = (start < end);

	do
	{
		result = findText(search.c_str(), searchStart, searchEnd, 0);
		if (result.success)
		{
			nextChar = _pEditView->send(SCI_GETCHARAT, result.end);
			styleAt = _pEditView->send(SCI_GETSTYLEAT, result.start);

			if (styleAt != SCE_H_CDATA && styleAt != SCE_H_DOUBLESTRING &&
			    styleAt != SCE_H_SINGLESTRING && styleAt != SCE_H_COMMENT)
			{
				if (nextChar == '>')
				{
					openTagFound.end = result.end;
					openTagFound.success = true;
				}
				else if (isWhitespace(nextChar))
				{
					intptr_t closeAnglePosition = findCloseAngle(result.end, forwardSearch ? end : start);
					if (-1 != closeAnglePosition && '/' != _pEditView->send(SCI_GETCHARAT, closeAnglePosition - 1))
					{
						openTagFound.end = closeAnglePosition;
						openTagFound.success = true;
					}
				}
			}
		}

		if (forwardSearch)
			searchStart = result.end + 1;
		else
			searchStart = result.start - 1;

	} while (result.success && !openTagFound.success);

	openTagFound.start = result.start;
	return openTagFound;
}

intptr_t XmlMatchedTagsHighlighter::findCloseAngle(intptr_t startPosition, intptr_t endPosition)
{
	FindResult closeAngle;
	bool isValidClose = false;
	intptr_t returnPosition = -1;

	if (startPosition > endPosition)
	{
		intptr_t temp = endPosition;
		endPosition = startPosition;
		startPosition = temp;
	}

	do
	{
		isValidClose = false;
		closeAngle = findText(">", startPosition, endPosition);
		if (closeAngle.success)
		{
			auto style = _pEditView->send(SCI_GETSTYLEAT, closeAngle.start);
			if (style != SCE_H_DOUBLESTRING && style != SCE_H_SINGLESTRING && style != SCE_H_COMMENT)
			{
				returnPosition = closeAngle.start;
				isValidClose = true;
			}
			else
			{
				startPosition = closeAngle.end;
			}
		}
	} while (closeAngle.success && !isValidClose);

	return returnPosition;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findCloseTag(const std::string& tagName,
                                                                            intptr_t start, intptr_t end)
{
	std::string search("</");
	search.append(tagName);
	FindResult closeTagFound;
	closeTagFound.success = false;
	FindResult result;
	intptr_t nextChar;
	intptr_t searchStart = start;
	intptr_t searchEnd = end;
	bool forwardSearch = (start < end);
	bool validCloseTag = false;

	do
	{
		validCloseTag = false;
		result = findText(search.c_str(), searchStart, searchEnd, 0);
		if (result.success)
		{
			nextChar = _pEditView->send(SCI_GETCHARAT, result.end);
			auto styleAt = _pEditView->send(SCI_GETSTYLEAT, result.start);

			if (forwardSearch)
				searchStart = result.end + 1;
			else
				searchStart = result.start - 1;

			if (styleAt != SCE_H_CDATA && styleAt != SCE_H_SINGLESTRING &&
			    styleAt != SCE_H_DOUBLESTRING && styleAt != SCE_H_COMMENT)
			{
				if (nextChar == '>')
				{
					validCloseTag = true;
					closeTagFound.start = result.start;
					closeTagFound.end = result.end;
					closeTagFound.success = true;
				}
				else if (isWhitespace(nextChar))
				{
					intptr_t whitespacePoint = result.end;
					do
					{
						++whitespacePoint;
						nextChar = _pEditView->send(SCI_GETCHARAT, whitespacePoint);
					} while (isWhitespace(nextChar));

					if (nextChar == '>')
					{
						validCloseTag = true;
						closeTagFound.start = result.start;
						closeTagFound.end = whitespacePoint;
						closeTagFound.success = true;
					}
				}
			}
		}
	} while (result.success && !validCloseTag);

	return closeTagFound;
}

XmlMatchedTagsHighlighter::FindResult XmlMatchedTagsHighlighter::findText(const char* text, intptr_t start, intptr_t end, int flags)
{
	FindResult returnValue;

	Sci_TextToFindFull search{};
	search.lpstrText = const_cast<char*>(text);
	search.chrg.cpMin = static_cast<Sci_Position>(start);
	search.chrg.cpMax = static_cast<Sci_Position>(end);

	LangType lang = _pEditView->getCurrentBuffer()->getLangType();
	QFileInfo fi(QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFileName()));
	QString ext = fi.suffix().toLower();
	if (lang == LangType::L_XML || (lang == LangType::L_HTML && ext == "xhtml"))
		flags = flags | SCFIND_MATCHCASE;

	intptr_t result = _pEditView->send(SCI_FINDTEXTFULL, flags, reinterpret_cast<sptr_t>(&search));
	if (-1 == result)
	{
		returnValue.success = false;
	}
	else
	{
		returnValue.success = true;
		returnValue.start = search.chrgText.cpMin;
		returnValue.end = search.chrgText.cpMax;
	}
	return returnValue;
}

void XmlMatchedTagsHighlighter::tagMatch(bool doHiliteAttr)
{
	_pEditView->clearIndicator(SCE_UNIVERSAL_TAGMATCH);
	_pEditView->clearIndicator(SCE_UNIVERSAL_TAGATTR);

	LangType lang = _pEditView->getCurrentBuffer()->getLangType();

	if (lang != LangType::L_XML && lang != LangType::L_HTML &&
	    lang != LangType::L_PHP && lang != LangType::L_ASP && lang != LangType::L_JSP)
		return;

	// Check if we're inside a code block
	if (lang == LangType::L_PHP || lang == LangType::L_ASP || lang == LangType::L_JSP)
	{
		std::string codeBeginTag = lang == LangType::L_PHP ? "<?" : "<%";
		std::string codeEndTag = lang == LangType::L_PHP ? "?>" : "%>";

		const intptr_t caret = 1 + _pEditView->send(SCI_GETCURRENTPOS);
		const FindResult startFound = findText(codeBeginTag.c_str(), caret, 0, 0);
		const FindResult endFound = findText(codeEndTag.c_str(), caret, 0, 0);

		if (startFound.success)
		{
			if (!endFound.success)
				return;
			else if (endFound.success && endFound.start <= startFound.end)
				return;
		}
	}

	// Save original targets and search options
	auto originalStartPos = _pEditView->send(SCI_GETTARGETSTART);
	auto originalEndPos = _pEditView->send(SCI_GETTARGETEND);
	auto originalSearchFlags = _pEditView->send(SCI_GETSEARCHFLAGS);

	XmlMatchedTagsPos xmlTags;

	if (getXmlMatchedTagsPos(xmlTags))
	{
		_pEditView->send(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_TAGMATCH);
		int openTagTailLen = 2;

		if (xmlTags.tagCloseStart != -1 && xmlTags.tagCloseEnd != -1)
		{
			_pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagCloseStart,
			                 xmlTags.tagCloseEnd - xmlTags.tagCloseStart);
			openTagTailLen = 1;
		}

		_pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagOpenStart,
		                 xmlTags.tagNameEnd - xmlTags.tagOpenStart);
		_pEditView->send(SCI_INDICATORFILLRANGE, xmlTags.tagOpenEnd - openTagTailLen, openTagTailLen);

		if (doHiliteAttr)
		{
			vector<pair<intptr_t, intptr_t>> attributes = getAttributesPos(
				xmlTags.tagNameEnd, xmlTags.tagOpenEnd - openTagTailLen);
			_pEditView->send(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_TAGATTR);
			for (size_t i = 0, len = attributes.size(); i < len; ++i)
			{
				_pEditView->send(SCI_INDICATORFILLRANGE, attributes[i].first,
				                 attributes[i].second - attributes[i].first);
			}
		}

		if (_pEditView->isShownIndentGuide())
		{
			intptr_t columnAtCaret = _pEditView->send(SCI_GETCOLUMN, xmlTags.tagOpenStart);
			intptr_t columnOpposite = _pEditView->send(SCI_GETCOLUMN, xmlTags.tagCloseStart);
			intptr_t lineAtCaret = _pEditView->send(SCI_LINEFROMPOSITION, xmlTags.tagOpenStart);
			intptr_t lineOpposite = _pEditView->send(SCI_LINEFROMPOSITION, xmlTags.tagCloseStart);

			if (xmlTags.tagCloseStart != -1 && lineAtCaret != lineOpposite)
			{
				_pEditView->send(SCI_BRACEHIGHLIGHT, xmlTags.tagOpenStart, xmlTags.tagCloseEnd - 1);
				_pEditView->send(SCI_SETHIGHLIGHTGUIDE,
				                 (columnAtCaret < columnOpposite) ? columnAtCaret : columnOpposite);
			}
		}
	}

	// Restore original targets
	_pEditView->send(SCI_SETTARGETRANGE, originalStartPos, originalEndPos);
	_pEditView->send(SCI_SETSEARCHFLAGS, originalSearchFlags);
}
