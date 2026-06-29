#!/usr/bin/env python3
"""Fix all Notepad_plus.cpp type mismatch errors."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'r') as f:
    lines = f.readlines()

# Build line index
N = len(lines)
content = ''.join(lines)

changes = 0

def do_replace(old, new, desc):
    global content, changes
    if old in content:
        content = content.replace(old, new, 1)
        changes += 1
        print(f"  ✓ Fix {changes}: {desc}")
    else:
        print(f"  ✗ MISSED: {desc}")

# ── Fix 1: getMatchedFileNames – vector<wstring> patterns → QStringList ──
old = '\tQDirIterator it(QString::fromWCharArray(dir), QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);'
new = '\tQStringList patternsQ;\n\tfor (const auto& p : patterns)\n\t\tpatternsQ.append(QString::fromStdWString(p));\n\n\tQDirIterator it(QString::fromWCharArray(dir), QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);'
do_replace(old, new, "getMatchedFileNames: add patternsQ conversion")

old = '\t\t\tif (!matchInExcludeDirList(fi.fileName(), patterns, level))'
new = '\t\t\tif (!matchInExcludeDirList(fi.fileName(), patternsQ, level))'
do_replace(old, new, "getMatchedFileNames: matchInExcludeDirList uses patternsQ")

old = '\t\t\tif (matchInList(fi.fileName().toStdWString(), patterns))'
new = '\t\t\tif (matchInList(fi.fileName().toStdWString(), patternsQ))'
do_replace(old, new, "getMatchedFileNames: matchInList uses patternsQ")

# ── Fix 2: replaceInOpenFiles – processAll 4th arg removed ──
old = 'int nbReplaced = _findReplaceDlg.processAll(ProcessReplaceAll, FindReplaceDlg::_env, true, &findersInfo);'
new = 'int nbReplaced = _findReplaceDlg.processAll(ProcessReplaceAll, FindReplaceDlg::_env, true);'
do_replace(old, new, "replaceInOpenFiles: processAll 4th arg removed")

# ── Fix 3: findInFinderFiles – processAll 4th arg removed ──
old = '\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true, findInFolderInfo);'
new = '\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true);'
do_replace(old, new, "findInFinderFiles: processAll 4th arg removed")

# ── Fix 4: findAll – processAll 4th arg removed ──
old = '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindAll, FindReplaceDlg::_env, isEntireDoc, &findersInfo);'
new = '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindAll, FindReplaceDlg::_env, isEntireDoc);'
do_replace(old, new, "findAll: processAll 4th arg removed")

# ── Fix 5: replaceInFiles – processAll 4th arg removed ──
old = '\t\tint nb = _findReplaceDlg.processAll(ProcessReplaceAll, FindReplaceDlg::_env, isEntireDoc, &findersInfo);'
new = '\t\tint nb = _findReplaceDlg.processAll(ProcessReplaceAll, FindReplaceDlg::_env, isEntireDoc);'
do_replace(old, new, "replaceInFiles: processAll 4th arg removed")

# ── Fix 6: setStatusbarMessageWithRegExprErr – wrong overload ──
old = '_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);'
new = '_findReplaceDlg.setStatusbarMessageWithRegExprErr(_pEditView);'
do_replace(old, new, "setStatusbarMessageWithRegExprErr: use _pEditView")

# ── Fix 7: replaceTarget – wchar_t → QString conversion ──
old = '_pEditView->replaceTarget(L"\\0", nbReplaced, nbReplaced);'
new = '_pEditView->replaceTarget(QStringLiteral("\\0"), nbReplaced, nbReplaced);'
do_replace(old, new, "replaceTarget: wchar_t -> QString")

old = '_pEditView->replaceTarget(L"", fromPos, toPos);'
new = '_pEditView->replaceTarget(QString(), fromPos, toPos);'
do_replace(old, new, "replaceTarget: L\"\" -> QString()")

old = '\tchar* text2display = (wchar_t*)_pEditView->getTextBetween(posStart, posEnd).c_str();'
new = '\tQString text2display = _pEditView->getTextBetween(posStart, posEnd);'
do_replace(old, new, "text2display: char* wchar_t -> QString")

# ── Fix 8: messageBox – const char* defaultMessage/title ──
old = '_nativeLangSpeaker.messageBox("file-changed-dialog", this->getHSelf(), L"File is being modified by another application. Do you want to reload it?", L"Button", QMessageBox::Warning, 0, 0);'
new = '_nativeLangSpeaker.messageBox("file-changed-dialog", this->getHSelf(), QStringLiteral("File is being modified by another application. Do you want to reload it?"), QStringLiteral("Button"), QMessageBox::Warning, 0, 0);'
do_replace(old, new, "messageBox: file-changed-dialog wstring->QString")

old = '_nativeLangSpeaker.messageBox("file-changed-dialog", this->getHSelf(), L"File was modified. Should it be reloaded?", L"Button", QMessageBox::Warning, 0, 0);'
new = '_nativeLangSpeaker.messageBox("file-changed-dialog", this->getHSelf(), QStringLiteral("File was modified. Should it be reloaded?"), QStringLiteral("Button"), QMessageBox::Warning, 0, 0);'
do_replace(old, new, "messageBox: file-changed-dialog 2 wstring->QString")

# ── Fix 9: setDisplayFormat – QString::fromWCharArray ──
old = '_statusBar.setText(getDisplayFormatStr(format).c_str(), STATUSBAR_DOC_TYPE);'
new = '_statusBar.setText(QString::fromStdWString(getDisplayFormatStr(format)), STATUSBAR_DOC_TYPE);'
do_replace(old, new, "setDisplayFormat: wstring->QString for setText")

# ── Fix 10: setLangStatus – wstring->QString ──
old = '_statusBar.setText(getLangDesc(langType).c_str(), STATUSBAR_DOC_TYPE);'
new = '_statusBar.setText(QString::fromStdWString(getLangDesc(langType)), STATUSBAR_DOC_TYPE);'
do_replace(old, new, "setLangStatus: wstring->QString for setText")

# ── Fix 11: matchInList inside findFiles – patterns vector<wstring> → QStringList ──
old = '\t\t\tif (matchInList(fi.fileName().toStdWString(), patterns))'
new = '\t\t\tif (matchInList(fi.fileName(), patternsQ))'
do_replace(old, new, "createFilelistForFiles: matchInList uses patternsQ")

# ── Fix 12: matchInExcludeDirList inside findFiles – patterns vector<wstring> → QStringList ──
old = '\t\t\tif (!matchInExcludeDirList(fi.fileName(), patterns, level))'
new = '\t\t\tif (!matchInExcludeDirList(fi.fileName(), patternsQ, level))'
do_replace(old, new, "createFilelistForFiles: matchInExcludeDirList uses patternsQ")

# ── Fix 13: findFiles add patternsQ conversion ──
# Need to add patternsQ before the QDirIterator inside findFiles
# This is trickier – look for the specific block
old = '\tQDirIterator it(filterDir, QDir::Files);\n\twhile (it.hasNext())\n\t{\n\t\tit.next();\n\t\tQFileInfo fi = it.fileInfo();\n\t\tif (matchInList(fi.fileName().toStdWString(), patterns))\n\t\t{\n\t\t\tfileNames.push_back(dir + fi.fileName().toStdWString());\n\t\t}\n\t}'
new = '\tQStringList patternsQ;\n\tfor (const auto& p : patterns)\n\t\tpatternsQ.append(QString::fromStdWString(p));\n\n\tQDirIterator it(filterDir, QDir::Files);\n\twhile (it.hasNext())\n\t{\n\t\tit.next();\n\t\tQFileInfo fi = it.fileInfo();\n\t\tif (matchInList(fi.fileName(), patternsQ))\n\t\t{\n\t\t\tfileNames.push_back(dir + fi.fileName().toStdWString());\n\t\t}\n\t}'
do_replace(old, new, "findFiles: add patternsQ conversion")

# ── Fix 14: Win32 URL functions – guarded with #ifdef _WIN32 ──
# The URL parsing block
old = '''\t\tint len = 0;
		scanToUrlEnd (text, textLen, start + schemeLen, & len);
		if (len)
		{
			len += schemeLen;
			URL_COMPONENTS url;
			memset (& url, 0, sizeof(url));
			url.dwStructSize = sizeof(url);
			bool r  = InternetCrackUrl(& text [start], len, 0, & url);
			if (r)
			{
				removeUnwantedTrailingCharFromEnclosedUrl(start, & text [0], & len);

				while (removeUnwantedTrailingCharFromUrl (& text [start], & len));
				*segmentLen = len;
				return text + start;
			}
		}
	}
	return nullptr;
}

bool Notepad_plus::isUrl(const wchar_t * url) const
{'''
new = '''\t\tint len = 0;
		scanToUrlEnd (text, textLen, start + schemeLen, & len);
		if (len)
		{
			len += schemeLen;
#ifdef _WIN32
			URL_COMPONENTS url;
			memset (& url, 0, sizeof(url));
			url.dwStructSize = sizeof(url);
			bool r  = InternetCrackUrl(& text [start], len, 0, & url);
			if (r)
			{
				removeUnwantedTrailingCharFromEnclosedUrl(start, & text [0], & len);

				while (removeUnwantedTrailingCharFromUrl (& text [start], & len));
				*segmentLen = len;
				return text + start;
			}
#else
			// On non-Windows, accept any scheme+host pattern as a URL
			(void)start; (void)len;
			*segmentLen = len;
			return text + start;
#endif
		}
	}
	return nullptr;
}

bool Notepad_plus::isUrl(const wchar_t * url) const
{'''
do_replace(old, new, "URL functions: guard with #ifdef _WIN32")

# ── Fix 15: wcsnicmp – no such function, use _wcsnicmp or Qt equivalent ──
old = '\t\tif (i <= remainingLength && wcsnicmp(url, p, i) == 0) return true;'
new = '\t\tif (i <= remainingLength && _wcsnicmp(url, p, i) == 0) return true;'
do_replace(old, new, "wcsnicmp -> _wcsnicmp")

# ── Fix 16: URL indicator colors – Sci_RangeToFormat equivalent ──
old = '''\tint SCI_INDICGETSTYLE = 2099;
			int SCI_INDICSETSTYLE = 2099;
			if (!_pEditView->execute(SCI_INDICGETSTYLE, INDIC_URL, 0))
				_pEditView->execute(SCI_INDICSETSTYLE, INDIC_URL, INDIC_URL);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_URL, urlNoUnderLineFg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_URL, urlNoUnderLineBg);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_SETINDICATORCURRENT, INDIC_URL);'''
new = '''\tint urlNoUnderLineFg = _pEditView->execute(SCI_STYLEGETFORE, STYLE_URL);
			int urlNoUnderLineBg = _pEditView->execute(SCI_STYLEGETBACK, STYLE_URL);
			int urlUnderLineBg   = _pEditView->execute(SCI_STYLEGETFORE, STYLE_HLINK);
			_pEditView->execute(SCI_INDICSETSTYLE, INDIC_URL, INDIC_SQUIGGLE);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_URL, urlNoUnderLineFg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_URL, urlNoUnderLineBg);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_SETINDICATORCURRENT, INDIC_URL);'''
do_replace(old, new, "URL indicator colors: use existing Sciilla styles")

# ── Write back ──
with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w') as f:
    f.write(content)

print(f"\nTotal changes: {changes}")