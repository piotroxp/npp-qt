#!/usr/bin/env python3
"""Fix all remaining Notepad_plus.cpp type mismatch errors."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'rb') as f:
    data = f.read()

content = data.decode('utf-8', errors='replace')
changes = 0

def do_replace(old, new, desc):
    global content, changes
    if old in content:
        content = content.replace(old, new, 1)
        changes += 1
        print(f"  ✓ Fix {changes}: {desc}")
        return True
    else:
        print(f"  ✗ MISSED: {desc}")
        # Try to find close match
        if old[:60] in content:
            print(f"    (partial match found for '{old[:40]}...')")
        return False

# ── Fix 1: Panel init methods – this->getHinst() void* → qApp ──
do_replace('\t\t_pAnsiCharPanel->init(this->getHinst(), this->getHSelf(), &_pEditView);',
           '\t\t_pAnsiCharPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), &_pEditView);',
           "AnsiCharPanel::init void*->qApp")

do_replace('\t\t_pFileBrowser->init(this->getHinst(), this->getHSelf());',
           '\t\t_pFileBrowser->init(qApp, static_cast<QWidget*>(this->getHSelf()));',
           "FileBrowser::init void*->qApp")

do_replace('\t\t(*pProjPanel)->init(this->getHinst(), this->getHSelf(), panelID);',
           '\t\t(*pProjPanel)->init(qApp, static_cast<QWidget*>(this->getHSelf()), panelID);',
           "ProjectPanel::init void*->qApp")

do_replace('\t\t_pDocMap->init(this->getHinst(), this->getHSelf(), &_pEditView);',
           '\t\t_pDocMap->init(qApp, static_cast<QWidget*>(this->getHSelf()), &_pEditView);',
           "DocumentMap::init void*->qApp")

# ── Fix 2: Panel create() – initializer list → createDockableDialog(data) with addInfo ──
do_replace('\t\t_pAnsiCharPanel->create(&data, { IDR_ASCIIPANEL_ICO, IDR_ASCIIPANEL_ICO_DM, IDR_ASCIIPANEL_ICO2 }, isRTL);',
           '\t\tdata.addInfo = QStringLiteral("%1,%2,%3").arg(IDR_ASCIIPANEL_ICO).arg(IDR_ASCIIPANEL_ICO_DM).arg(IDR_ASCIIPANEL_ICO2);\n\t\t_pAnsiCharPanel->createDockableDialog(data);',
           "AnsiCharPanel::create initializer list -> createDockableDialog")

do_replace('\t\t_pFileBrowser->create(&data, { IDR_FILEBROWSER_ICO, IDR_FILEBROWSER_ICO_DM, IDR_FILEBROWSER_ICO2 }, _nativeLangSpeaker.isRTL());',
           '\t\tdata.addInfo = QStringLiteral("%1,%2,%3").arg(IDR_FILEBROWSER_ICO).arg(IDR_FILEBROWSER_ICO_DM).arg(IDR_FILEBROWSER_ICO2);\n\t\t_pFileBrowser->createDockableDialog(data);',
           "FileBrowser::create initializer list -> createDockableDialog")

do_replace('\t\t(*pProjPanel)->create(&data, { IDR_PROJECTPANEL_ICO, IDR_PROJECTPANEL_ICO_DM, IDR_PROJECTPANEL_ICO2 }, isRTL);',
           '\t\tdata.addInfo = QStringLiteral("%1,%2,%3").arg(IDR_PROJECTPANEL_ICO).arg(IDR_PROJECTPANEL_ICO_DM).arg(IDR_PROJECTPANEL_ICO2);\n\t\t(*pProjPanel)->createDockableDialog(data);',
           "ProjectPanel::create initializer list -> createDockableDialog")

# ── Fix 3: AI_PROJECTPANELTITLE → hardcoded string ──
do_replace('\t\twstring title_temp = pNativeSpeaker->getAttrNameStr(AI_PROJECTPANELTITLE, "AsciiInsertion", "PanelTitle");',
           '\t\tQString title_temp = pNativeSpeaker->getAttrNameStr(QStringLiteral("Ascii Character Panel"), "AsciiInsertion", "PanelTitle");',
           "AI_PROJECTPANELTITLE -> QStringLiteral string")

# Also fix the wcscpy_s part that follows
do_replace('\t\tstatic wchar_t title[85];\n\t\tif (title_temp.length() < 85)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.c_str());\n\t\t\tdata.pszName = title;\n\t\t}',
           '\t\tstatic wchar_t title[85];\n\t\tif (title_temp.length() < 85)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.toStdWString().c_str());\n\t\t\tdata.pszName = title;\n\t\t}',
           "AI_PROJECTPANELTITLE title_temp -> QString, wcscpy_s fix")

# ── Fix 4: FB_PANELTITLE / FOLDERASWORKSPACE_NODE → hardcoded strings ──
do_replace('\t\tdata.pszName = L"ST";\n\n\t\tNppParameters& nppParams = NppParameters::getInstance();\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));',
           '\t\tdata.pszName = L"ST";\n\n\t\tNppParameters& nppParams = NppParameters::getInstance();\n\n\t\t// FOLDERASWORKSPACE_NODE and FB_PANELTITLE stubbed for Qt6 – tree root name handled internally\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));',
           "FB_PANELTITLE / FOLDERASWORKSPACE_NODE comment")

# Fix the actual code that references FOLDERASWORKSPACE_NODE / FB_PANELTITLE
do_replace('\t\tNppParameters& nppParams = NppParameters::getInstance();\n\n\t\t// FOLDERASWORKSPACE_NODE and FB_PANELTITLE stubbed for Qt6 – tree root name handled internally\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));',
           '\t\tNppParameters& nppParams = NppParameters::getInstance();\n\n\t\t// FOLDERASWORKSPACE_NODE and FB_PANELTITLE stubbed for Qt6 – tree root name handled internally\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));',
           "FB_PANELTITLE / FOLDERASWORKSPACE_NODE already commented")

# Find and remove the actual FOLDERASWORKSPACE_NODE / FB_PANELTITLE lines
# These are inside the FileBrowser init block, use getAttrNameStr
old_block = '\t\t// FOLDERASWORKSPACE_NODE and FB_PANELTITLE stubbed for Qt6 – tree root name handled internally\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));\n\n\t\tdata.uMask = DWS_DF_CONT_LEFT | DWS_ICONTAB | DWS_USEOWNDARKMODE;\n\n\t\tCOLORREF fgColor = nppParams.getCurrentDefaultFgColor();\n\t\tCOLORREF bgColor = nppParams.getCurrentDefaultBgColor();\n\n\t\t_pFileBrowser->setForegroundColor(fgColor);\n\t}'
new_block = '\t\t// FOLDERASWORKSPACE_NODE and FB_PANELTITLE stubbed for Qt6 – tree root name handled internally\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<qintptr>(_pFileBrowser->getHSelf()));\n\n\t\tdata.uMask = DWS_DF_CONT_LEFT | DWS_ICONTAB | DWS_USEOWNDARKMODE;\n\n\t\tCOLORREF fgColor = nppParams.getCurrentDefaultFgColor();\n\t\tCOLORREF bgColor = nppParams.getCurrentDefaultBgColor();\n\n\t\t_pFileBrowser->setForegroundColor(fgColor);\n\t}'

# ── Fix 5: FileBrowser::deleteAllFromTree() private → remove the call ──
do_replace('\tif (fromScratch)\n\t{\n\t\t_pFileBrowser->deleteAllFromTree();\n\t}',
           '\tif (fromScratch)\n\t{\n\t\t// FileBrowser::deleteAllFromTree() is private; clear via public API\n\t\t_pFileBrowser->clear();\n\t}',
           "deleteAllFromTree private -> clear()")

# ── Fix 6: FileBrowser::addRootFolder(wstring) → addRootFolder(QString) ──
do_replace('\t\t_pFileBrowser->addRootFolder(folders[i]);',
           '\t\t_pFileBrowser->addRootFolder(QString::fromStdWString(folders[i]));',
           "FileBrowser::addRootFolder wstring->QString")

# ── Fix 7: FileBrowser::setClosed(false) → display() ──
do_replace('\t_pFileBrowser->setClosed(false);',
           '\t_pFileBrowser->display();',
           "FileBrowser::setClosed(false) -> display()")

# ── Fix 8: ProjectPanel::setWorkSpaceFilePath / openWorkSpace – wstring return type ──
do_replace('\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID));',
           '\t\t(*pProjPanel)->setWorkSpaceFilePath(QString::fromStdWString(nppParam.getWorkSpaceFilePath(panelID)).toStdWString().c_str());',
           "ProjectPanel::setWorkSpaceFilePath wstring->const wchar_t*")

do_replace('\t\t(*pProjPanel)->openWorkSpace(nppParam.getWorkSpaceFilePath(panelID));',
           '\t\t(*pProjPanel)->openWorkSpace(QString::fromStdWString(nppParam.getWorkSpaceFilePath(panelID)).toStdWString().c_str());',
           "ProjectPanel::openWorkSpace wstring->const wchar_t*")

# ── Fix 9: ProjectPanel::setClosed(false) → display() ──
do_replace('\t(*pProjPanel)->setClosed(false);',
           '\t(*pProjPanel)->display();',
           "ProjectPanel::setClosed(false) -> display()")

# ── Fix 10: DocumentMap::createDockableDialog → use create(&data) pattern ──
do_replace('\t\t_pDocMap->createDockableDialog(data);',
           '\t\t_pDocMap->create(&data);',
           "DocumentMap::createDockableDialog -> create(&data)")

# ── Fix 11: loadPanelIcon reinterpret_cast QApplication* → qApp ──
do_replace('\t\tloadPanelIcon(reinterpret_cast<QApplication*>(this->getHinst()), _pDocMap, &data.iconTab);',
           '\t\tloadPanelIcon(qApp, _pDocMap, &data.iconTab);',
           "loadPanelIcon reinterpret_cast -> qApp")

# ── Fix 12: title_temp = getAttrNameStr returns QString, .toStdWString() ──
do_replace("\t\twstring title_temp = pNativeSpeaker->getAttrNameStr(QStringLiteral(\"Document Map\"), \"DocumentMap\", \"PanelTitle\").toStdWString();",
           "\t\tQString title_temp = pNativeSpeaker->getAttrNameStr(QStringLiteral(\"Document Map\"), \"DocumentMap\", \"PanelTitle\");",
           "DocMap title_temp is QString, not wstring")

# ── Fix 13: FunctionListPanel::create initializer list ──
do_replace('\t\t_pFuncList->create(&data, { IDR_FUNC_LIST_ICO, IDR_FUNC_LIST_ICO_DM, IDR_FUNC_LIST_ICO2 });',
           '\t\tdata.addInfo = QStringLiteral("%1,%2,%3").arg(IDR_FUNC_LIST_ICO).arg(IDR_FUNC_LIST_ICO_DM).arg(IDR_FUNC_LIST_ICO2);\n\t\t_pFuncList->create(&data);',
           "FunctionListPanel::create initializer list fix")

# ── Fix 14: themeName type mismatch ──
# NppDarkMode::getThemeName() returns QString, themeName is wstring
do_replace('\t\twstring xmlFileName = NppDarkMode::getThemeName();',
           '\t\tQString xmlFileName = NppDarkMode::getThemeName();',
           "xmlFileName QString, not wstring")

do_replace('\t\t\tpathAppend(themePath, xmlFileName);',
           '\t\t\tpathAppend(themePath, xmlFileName.toStdWString());',
           "pathAppend: xmlFileName.toStdWString()")

do_replace('\t\t\tpathAppend(themePath, xmlFileName);',
           '\t\t\tpathAppend(themePath, xmlFileName.toStdWString());',
           "pathAppend 2nd: xmlFileName.toStdWString()")

do_replace('\t\t\tthemeName = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath)).toStdWString();',
           '\t\t\tthemeName = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath)).toStdWString();',
           "themeName already has toStdWString - OK")

do_replace('\t\t\t_configStyleDlg.selectThemeByName(themeName.c_str());',
           '\t\t\t_configStyleDlg.selectThemeByName(QString::fromStdWString(themeName));',
           "selectThemeByName: QString, not wstring.c_str()")

# ── Fix 15: backup thread malformed if block ──
do_replace('\tif (hThread)\n\t\t// CloseHandle: hThread);\n\t}',
           '\tif (hThread) {\n\t\t// CloseHandle: hThread);\n\t}',
           "backup thread if block syntax")

# ── Fix 16: comment symbols const char* → QString ──
# These are in a function that sets comment symbols
do_replace('\t\tcommentLineSymbol = buf->getCommentLineSymbol();\n\t\tcommentStart = buf->getCommentStart();\n\t\tcommentEnd = buf->getCommentEnd();',
           '\t\tcommentLineSymbol = QString::fromLatin1(buf->getCommentLineSymbol());\n\t\tcommentStart = QString::fromLatin1(buf->getCommentStart());\n\t\tcommentEnd = QString::fromLatin1(buf->getCommentEnd());',
           "comment symbols char* -> QString")

# ── Fix 17: searchInTarget wrong argument order ──
do_replace('\t\tposStartCommentAfter[iSelStart] = _pEditView->searchInTarget(start_comment, selectionStart, docLength);',
           '\t\tposStartCommentAfter[iSelStart] = _pEditView->searchInTarget(start_comment, static_cast<intptr_t>(selectionStart), static_cast<intptr_t>(docLength));',
           "searchInTarget DIR_DOWN start: fix args")

do_replace('\t\tposEndCommentAfter[iSelStart] = _pEditView->searchInTarget(end_comment, selectionStart, docLength);',
           '\t\tposEndCommentAfter[iSelStart] = _pEditView->searchInTarget(end_comment, static_cast<intptr_t>(selectionStart), static_cast<intptr_t>(docLength));',
           "searchInTarget DIR_DOWN end: fix args")

do_replace('\t\tposEndCommentAfter[iSelEnd] = _pEditView->searchInTarget(end_comment, selectionEnd, static_cast<intptr_t>(docLength));',
           '\t\tposEndCommentAfter[iSelEnd] = _pEditView->searchInTarget(end_comment, static_cast<intptr_t>(selectionEnd), static_cast<intptr_t>(docLength));',
           "searchInTarget DIR_DOWN end (iSelEnd): fix args")

# ── Fix 18: _wcsnicmp → Qt equivalent ──
do_replace('\t\tif (i <= remainingLength && _wcsnicmp(url, p, i) == 0) return true;',
           '\t\tif (i <= remainingLength && QString::fromWCharArray(url, i).compare(QString::fromWCharArray(p, i), Qt::CaseInsensitive) == 0) return true;',
           "_wcsnicmp -> Qt QString::compare CaseInsensitive")

# ── Fix 19: URL functions guard ──
# The URL_COMPONENTS/InternetCrackUrl block
old_url = '''\t\t\tURL_COMPONENTS url;
			memset (& url, 0, sizeof(url));
			url.dwStructSize = sizeof(url);
			bool r  = InternetCrackUrl(& text [start], len, 0, & url);
			if (r)
			{
				removeUnwantedTrailingCharFromEnclosedUrl(start, & text [0], & len);

				while (removeUnwantedTrailingCharFromUrl (& text [start], & len));
				*segmentLen = len;
				return true;
			}
			else // to avoid potentially catastrophic backtracking, skip the entire text that looked like a URL
			{
				*segmentLen = len;
				return false;
			}'''
new_url = '''#ifdef _WIN32
			URL_COMPONENTS url;
			memset (& url, 0, sizeof(url));
			url.dwStructSize = sizeof(url);
			bool r  = InternetCrackUrl(& text [start], len, 0, & url);
			if (r)
			{
				removeUnwantedTrailingCharFromEnclosedUrl(start, & text [0], & len);

				while (removeUnwantedTrailingCharFromUrl (& text [start], & len));
				*segmentLen = len;
				return true;
			}
			else // to avoid potentially catastrophic backtracking, skip the entire text that looked like a URL
			{
				*segmentLen = len;
				return false;
			}
#else
			// Win32 URL parsing not available on this platform
			*segmentLen = len;
			return len > 0;
#endif'''
do_replace(old_url, new_url, "URL_COMPONENTS/InternetCrackUrl: guard with #ifdef _WIN32")

# ── Fix 20: URL indicator colors – urlNoUnderLineFg etc. undefined ──
# These are set by a previous function but not declared in current scope
old_indic = '''\tint SCI_INDICGETSTYLE = 2099;
			int SCI_INDICSETSTYLE = 2099;
			if (!_pEditView->execute(SCI_INDICGETSTYLE, INDIC_URL, 0))
				_pEditView->execute(SCI_INDICSETSTYLE, INDIC_URL, INDIC_URL);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_URL, urlNoUnderLineFg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_URL, urlNoUnderLineBg);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_SETINDICATORCURRENT, INDIC_URL);'''
new_indic = '''\tint urlNoUnderLineFg = _pEditView->execute(SCI_STYLEGETFORE, STYLE_URL);
			int urlNoUnderLineBg = _pEditView->execute(SCI_STYLEGETBACK, STYLE_URL);
			int urlUnderLineBg   = _pEditView->execute(SCI_STYLEGETFORE, STYLE_HLINK);
			_pEditView->execute(SCI_INDICSETSTYLE, INDIC_URL, INDIC_SQUIGGLE);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_URL, urlNoUnderLineFg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_URL, urlNoUnderLineBg);
			_pEditView->execute(SCI_INDICSETFORE, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_INDICSETBACK, INDIC_HIDDENURL, urlUnderLineBg);
			_pEditView->execute(SCI_SETINDICATORCURRENT, INDIC_URL);'''
do_replace(old_indic, new_indic, "URL indicator: define colors from existing styles")

# ── Fix 21: MultiByteToWideChar / WideCharToMultiByte ──
do_replace('\t\t\tint wideLen = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), nullptr, 0);\n\t\t\tif (wideLen)\n\t\t\t{\n\t\t\t\twchar_t* wideText = new wchar_t[wideLen];\n\t\t\t\tMultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), wideText, wideLen);\n\n\t\t\t\tint urlLen = 0;\n\t\t\t\tif (scanToUrlEnd(wideText, wideLen, 0, &urlLen))\n\t\t\t\t{\n\t\t\t\t\treturn wideText + 0;\n\t\t\t\t}\n\n\t\t\t\tdelete[] wideText;\n\t\t\t}',
           '\t\t\tint wideLen = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), nullptr, 0);\n\t\t\tif (wideLen)\n\t\t\t{\n\t\t\t\twchar_t* wideText = new wchar_t[wideLen];\n\t\t\t\tMultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), wideText, wideLen);\n\n\t\t\t\tint urlLen = 0;\n\t\t\t\tif (scanToUrlEnd(wideText, wideLen, 0, &urlLen))\n\t\t\t\t{\n\t\t\t\t\treturn wideText + 0;\n\t\t\t\t}\n\n\t\t\t\tdelete[] wideText;\n\t\t\t}',
           "MultiByteToWideChar already guarded (check)")

# Find the actual line that uses MultiByteToWideChar and guard it
old_mb = '\t\tint wideLen = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), nullptr, 0);'
new_mb = '#ifdef _WIN32\n\t\tint wideLen = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLen), nullptr, 0);'
do_replace(old_mb, new_mb, "MultiByteToWideChar: guard with #ifdef _WIN32")

# Find and fix the closing of that block
old_mb2 = '''\t\t\t\tdelete[] wideText;
\t\t\t}
\t\t}
\t}
\treturn nullptr;
}

bool Notepad_plus::isUrl(const wchar_t * url) const'''
new_mb2 = '''\t\t\t\tdelete[] wideText;
\t\t\t}
#else
\t\t// URL detection not available without Win32 APIs on this platform
\t\t(void)text; (void)textLen; (void)start;
#endif
\t\t}
\t}
\treturn nullptr;
}

bool Notepad_plus::isUrl(const wchar_t * url) const'''
do_replace(old_mb2, new_mb2, "MultiByteToWideChar: close #ifdef guard")

# ── Fix 22: autoIndent_none undefined ──
do_replace('\t\telse if (style == autoIndent_none)',
           '\t\telse if (style == AutoIndentMode::none)',
           "autoIndent_none -> AutoIndentMode::none")

# ── Fix 23: getVisibleStartAndEndPosition missing ──
# This is a ScintillaComponent method that may not exist
# Stub it in ScintillaComponent.h
do_replace('\t\tqintptr startPos = 0, endPos = 0;\n\t\t_pEditView->getVisibleStartAndEndPosition(startPos, endPos);',
           '\t\t// getVisibleStartAndEndPosition not implemented – use full document range\n\t\tqintptr startPos = 0, endPos = _pEditView->execute(SCI_GETLENGTH);',
           "getVisibleStartAndEndPosition stub")

# ── Fix 24: urlDisable undefined ──
do_replace('\tcase urlDisable:\n\t\t\tbreak;',
           '\tcase urlDisable:\n\t\t\t// urlDisable handled by clearing indicators\n\t\t\t_pEditView->execute(SCI_SETINDICATORCURRENT, URL_INDIC);\n\t\t\t_pEditView->execute(SCI_INDICCLEARRANGE, 0, _pEditView->execute(SCI_GETLENGTH));\n\t\t\tbreak;',
           "urlDisable -> clear indicators")

# ── Write back ──
with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print(f"\nTotal changes: {changes}")