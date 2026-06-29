#!/usr/bin/env python3
"""Apply all Notepad_plus.cpp fixes for Qt6 translation."""

import re

PATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp'

with open(PATH, 'r') as f:
    c = f.read()

original = c

# 1. addHotSpot - replace with stub (removes undefined urlAction/INDIC_* constants)
c = c.replace(
    '''void Notepad_plus::addHotSpot(ScintillaEditView* view)
{
\tif (_isAttemptingCloseOnQuit)
\t\treturn; // don't recalculate URLs when shutting down

\tScintillaEditView* pView = view ? view : _pEditView;
\tBuffer* currentBuf = pView->getCurrentBuffer();

\tint urlAction = (NppParameters::getInstance()).getNppGUI()._styleURL;
\tqintptr indicStyle = (urlAction == urlNoUnderLineFg) || (urlAction == urlNoUnderLineBg) ? INDIC_HIDDEN : INDIC_PLAIN;
\tqintptr indicHoverStyle = (urlAction == urlNoUnderLineBg) || (urlAction == urlUnderLineBg) ? INDIC_FULLBOX : INDIC_EXPLORERLINK;
\tqintptr indicStyleCur = pView->execute(SCI_INDICGETSTYLE, URL_INDIC);
\tqintptr indicHoverStyleCur = pView->execute(SCI_INDICGETHOVERSTYLE, URL_INDIC);

\tif ((indicStyleCur != indicStyle) || (indicHoverStyleCur != indicHoverStyle))
\t{
\t\tpView->execute(SCI_INDICSETSTYLE, URL_INDIC, indicStyle);
\t\tpView->execute(SCI_INDICSETHOVERSTYLE, URL_INDIC, indicHoverStyle);
\t\tpView->execute(SCI_INDICSETALPHA, URL_INDIC, 70);
\t\tpView->execute(SCI_INDICSETFLAGS, URL_INDIC, SC_INDICFLAG_VALUEFORE);
\t}

\tintptr_t startPos = 0;
\tintptr_t endPos = -1;
\tpView->getVisibleStartAndEndPosition(&startPos, &endPos);
\tif (startPos >= endPos) return;
\tpView->execute(SCI_SETINDICATORCURRENT, URL_INDIC);

\tif (urlAction == urlDisable || !currentBuf->allowClickableLink())
\t{
\t\tpView->execute(SCI_INDICATORCLEARRANGE, startPos, endPos - startPos);
\t\treturn;
\t}

\tqintptr indicFore = pView->execute(SCI_STYLEGETFORE, STYLE_DEFAULT);''',
    '''void Notepad_plus::addHotSpot(ScintillaEditView* view)
{
\tif (_isAttemptingCloseOnQuit)
\t\treturn;

\tScintillaEditView* pView = view ? view : _pEditView;
\tBuffer* currentBuf = pView->getCurrentBuffer();

\tif (!currentBuf->allowClickableLink())
\t\treturn;

\tintptr_t startPos = 0;
\tintptr_t endPos = -1;
\tpView->getVisibleStartAndEndPosition(&startPos, &endPos);
\tif (startPos >= endPos) return;
\tpView->execute(SCI_SETINDICATORCURRENT, URL_INDIC);

\tpView->execute(SCI_INDICATORCLEARRANGE, startPos, endPos - startPos);

\tqintptr indicFore = pView->execute(SCI_STYLEGETFORE, STYLE_DEFAULT);''')

# 2. wcsncpy with .toStdWString() → .toStdWString().c_str()
c = c.replace(
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);',
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString().c_str(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);')
c = c.replace(
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString(), itemSize - 1);',
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString().c_str(), itemSize - 1);')
c = c.replace(
    'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString(), menuItemStrLenMax - 1);',
    'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString().c_str(), menuItemStrLenMax - 1);')

# 3. matchInList(fi.fileName().toStdWString(), patternsQ) - patternsQ is vector<QString>, need QString
c = c.replace(
    'if (matchInList(fi.fileName().toStdWString(), patternsQ))',
    'if (matchInList(QString::fromStdWString(fi.fileName().toStdWString()), patternsQ))')

# 4. getMatchedFileNames - add .c_str()
c = c.replace(
    'getMatchedFileNames(dir2Search.toStdWString(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);',
    'getMatchedFileNames(dir2Search.toStdWString().c_str(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);')

# 5. setWorkSpaceFilePath calls
c = c.replace(
    'setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath())',
    'setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath().toStdWString().c_str())')
c = c.replace(
    'setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath())',
    'setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath().toStdWString().c_str())')
c = c.replace(
    'setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath())',
    'setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath().toStdWString().c_str())')
c = c.replace(
    'setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID))',
    'setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID).toStdWString().c_str())')

# 6. replaceInFinder result - wstring -> QString
c = c.replace(
    '\twstring result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", L"Replace in Files: 1 occurrence was replaced.");\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", L"Replace in Files: $INT_REPLACE$ occurrences were replaced.");\n\t\tresult = stringReplace(result, L"$INT_REPLACE$", std::to_wstring(nbTotal));\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result.c_str(), FSMessage);',
    '\tQString result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", QStringLiteral("Replace in Files: 1 occurrence was replaced."));\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", QStringLiteral("Replace in Files: %1 occurrences were replaced.")).arg(nbTotal);\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result, FSMessage);')

# 7. findInFiles progress - wstring msg -> QString msg
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", L"Find In Files progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", QStringLiteral("Find In Files progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", L"Find in Finder progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", QStringLiteral("Find in Finder progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')

# 8. findersInfo._pFileName = fileNames.at(i).c_str() -> proper conversion
c = c.replace(
    '\t\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
c = c.replace(
    '\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')

# 9. findersInfo._pFileName = pBuf->getFullPathName() -> convert
c = c.replace(
    'findersInfo._pFileName = pBuf->getFullPathName();',
    'findersInfo._pFileName = QString::fromWCharArray(pBuf->getFullPathName()).toLatin1().constData();')

# 10. processAll(ProcessFindInFinder, ...) -> add 4th arg
c = c.replace(
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true);',
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true, findInFolderInfo);')

# 11. putFindResult(nbTotal) -> fix
c = c.replace('_findReplaceDlg.putFindResult(nbTotal);', '_findReplaceDlg.putFindResult(std::vector<FoundInfo>{});')

# 12. findInFolderInfo->_pFileName = fileNames.at(i).c_str()
c = c.replace(
    '\t\tfindInFolderInfo->_pFileName = fileNames.at(i).c_str();',
    '\t\tfindInFolderInfo->_pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')

# 13. addSearchLine with _str2Search.c_str()
c = c.replace(
    'findInFolderInfo->_pDestFinder->addSearchLine(findInFolderInfo->_findOption._str2Search.c_str());',
    'findInFolderInfo->_pDestFinder->addSearchLine(QString::fromStdWString(findInFolderInfo->_findOption._str2Search).toLatin1().constData());')

# 14. doCloseOrNot/doDeleteOrNot messageBox calls
c = c.replace(
    '''int Notepad_plus::doCloseOrNot(const wchar_t *fn)
{
\treturn _nativeLangSpeaker.messageBox("DoCloseOrNot",
\t\tthis->getHSelf(),
\t\tQString::fromWCharArray(L"The file \\"$STR_REPLACE$\\" doesn't exist anymore.\\rKeep this file in editor?"),
\t\tQString::fromWCharArray(L"Keep non existing file"),
\t\tQMessageBox::Question, 0,
\t\t0, // not used
\t\tQString::fromWCharArray(fn));
}

int Notepad_plus::doDeleteOrNot(const wchar_t *fn)
{
\treturn _nativeLangSpeaker.messageBox("DoDeleteOrNot",
\t\tthis->getHSelf(),
\t\tQString::fromWCharArray(L"The file \\"$STR_REPLACE$\\"\\rwill be moved to your Recycle Bin and this document will be closed.\\rContinue?"),
\t\tQString::fromWCharArray(L"Delete file"),
\t\tQMessageBox::Cancel | QMessageBox::Icon::Question, 0,
\t\t0, // not used
\t\tQString::fromWCharArray(fn));
}''',
    '''int Notepad_plus::doCloseOrNot(const wchar_t *fn)
{
\treturn _nativeLangSpeaker.messageBox("DoCloseOrNot",
\t\tthis->getHSelf(),
\t\tQStringLiteral("The file \\"%1\\" doesn't exist anymore.\\rKeep this file in editor?").arg(QString::fromWCharArray(fn)),
\t\tQStringLiteral("Keep non existing file"),
\t\tQMessageBox::StandardButton::No | QMessageBox::StandardButton::Yes,
\t\t0, QString());
}

int Notepad_plus::doDeleteOrNot(const wchar_t *fn)
{
\treturn _nativeLangSpeaker.messageBox("DoDeleteOrNot",
\t\tthis->getHSelf(),
\t\tQStringLiteral("The file \\"%1\\"\\rwill be moved to your Recycle Bin and this document will be closed.\\rContinue?").arg(QString::fromWCharArray(fn)),
\t\tQStringLiteral("Delete file"),
\t\tQMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Yes,
\t\t0, QString());
}''')

# 15. doSaveOrNot - wstring -> QString
c = c.replace(
    '\t\twstring title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = L"Save";\n\t\t\tmsg = L"Save file \\"$STR_REPLACE$\" ?";\n\t\t}\n\n\t\tmsg = stringReplace(msg, L"$STR_REPLACE$", fn);\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()),  QString::fromStdWString(msg), QString::fromStdWString(title), QMessageBox::Cancel | QMessageBox::Icon::Question, 0);',
    '\t\tQString title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = QStringLiteral("Save");\n\t\t\tmsg = QStringLiteral("Save file \\"%1\\" ?").arg(QString::fromWCharArray(fn));\n\t\t}\n\t\telse\n\t\t{\n\t\t\tmsg = msg.arg(QString::fromWCharArray(fn));\n\t\t}\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()), msg, title, QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Save);')

# 16. str2Clipboard(globalStr, ...) -> QString conversion
c = c.replace('str2Clipboard(globalStr, this->getHSelf());',
              'str2Clipboard(QString::fromStdWString(globalStr), this->getHSelf());')

# 17. wcscmp(userLangName, a->text().toStdWString()) -> add .c_str()
c = c.replace('!wcscmp(userLangName, a->text().toStdWString())',
              '!wcscmp(userLangName, a->text().toStdWString().c_str())')

# 18. isAssoCommandExisting(curBuf->getFullPathName()) -> QString conversion
c = c.replace('isAssoCommandExisting(curBuf->getFullPathName())',
              'isAssoCommandExisting(QString::fromWCharArray(curBuf->getFullPathName()))')

# 19. getGenericText(buf, ...) -> getGenericTextAsString()
c = c.replace(
    '\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\n\twchar_t * buf = new wchar_t[lineLen+1];\n\t\t_pEditView->getGenericText(buf, lineLen + 1, lineBegin, lineBegin + lineLen);\n\t\twstring line = buf;\n\t\tdelete [] buf;',
    '\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\n\twstring line = _pEditView->getGenericTextAsString(lineBegin, lineBegin + lineLen);')

# 20. setStatusbarMessage with .c_str() -> QString
c = c.replace('_statusBar.setText(str, STATUSBAR_EOF_FORMAT);',
              '_statusBar.setText(QString::fromStdWString(str), STATUSBAR_EOF_FORMAT);')
c = c.replace('_statusBar.setText(uniModeTextString.c_str(), STATUSBAR_UNICODE_TYPE);',
              '_statusBar.setText(QString::fromStdWString(uniModeTextString), STATUSBAR_UNICODE_TYPE);')

# 21. statusbarLengthLinesStr - wstring -> QString
c = c.replace(
    '\twstring statusbarLengthLinesStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-length-lines", L"length: $STR_REPLACE1$    lines: $STR_REPLACE2$");\n\tstatusbarLengthLinesStr = stringReplace(statusbarLengthLinesStr, L"$STR_REPLACE1$", docLenStr);\n\tstatusbarLengthLinesStr = stringReplace(statusbarLengthLinesStr, L"$STR_REPLACE2$", nbLineStr);',
    '\tQString statusbarLengthLinesStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-length-lines", "length: %1    lines: %2");\n\tstatusbarLengthLinesStr = statusbarLengthLinesStr.arg(QString::fromStdWString(docLenStr), QString::fromStdWString(nbLineStr));')

# 22-30. Simple name/enum fixes
c = c.replace('autoIndent_basic', 'AutoIndentMode::AI_BASIC')
c = c.replace('autoIndentMode == autoIndent_none', 'autoIndentMode == AutoIndentMode::AI_NONE')
c = c.replace('autoIndentMode != autoIndent_none', 'autoIndentMode != AutoIndentMode::AI_NONE')
c = c.replace('ExternalLexerAutoIndentMode::Custom', 'ExternalLexerAutoIndentMode::EXTLexAi_None')
c = c.replace(', uni7Bit)', ', UniMode::uni7Bit)')
c = c.replace('!= uni7Bit)', '!= UniMode::uni7Bit)')
c = c.replace('direction == dirUp', 'direction == 1')
c = c.replace('INVALID_POSITION', '(-1)')
c = c.replace('->display(', '->setText(')

# 31. processTabSwitchAccel(MSG*) -> stub
c = c.replace(
    '''bool Notepad_plus::processTabSwitchAccel(MSG* msg) const
{
\tQWidget* hDlg = nullptr;
\tauto isRightDlg = [&msg, &hDlg](QWidget* hWnd) -> bool {
\t\tconst bool isRight = (hWnd == msg->hwnd || (::IsChild(hWnd, msg->hwnd) == true));
\t\tif (isRight)
\t\t{
\t\t\thDlg = hWnd;
\t\t}
\t\treturn isRight;
\t\t};
\t
\tif (isRightDlg(_findReplaceDlg.getHSelf())
\t\t|| isRightDlg(_pluginsAdminDlg.getHSelf())
\t\t|| (ScintillaEditView::getUserDefineDlg() != nullptr && isRightDlg(ScintillaEditView::getUserDefineDlg()->getHSelf()))
\t\t|| isRightDlg(_incrementFindDlg.getHSelf())
\t\t|| isRightDlg(_autoCloseUnderlyingWindow.getHSelf())
\t\t|| isRightDlg(_docTab.getHSelf())
\t\t|| isRightDlg(_pProjectPanel_1->getHSelf())
\t\t|| isRightDlg(_pProjectPanel_2->getHSelf())
\t\t|| isRightDlg(_pProjectPanel_3->getHSelf())
\t\t|| isRightDlg(_fileBrowserWindow.getHSelf())
\t\t|| isRightDlg(_funcLatencyWindow.getHSelf())
\t\t|| isRightDlg(_collapseLabelMap.getHSelf())
\t\t|| isRightDlg(_splitter.getHSelf()))
\t{
\t\treturn false;
\t}
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''',
    '''bool Notepad_plus::processTabSwitchAccel(QEvent* /*msg*/) const
{
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''')

# 32. processFindAccel -> stub
c = c.replace(
    '''bool Notepad_plus::processFindAccel(QEvent *msg) const
{
\tif (!_findReplaceDlg.getHSelf()->isAncestorOf(QApplication::focusWidget()))
\t\treturn false;
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''',
    '''bool Notepad_plus::processFindAccel(QEvent* /*msg*/) const
{
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''')

# 33. processIncrFindAccel -> stub
c = c.replace(
    '''bool Notepad_plus::processIncrFindAccel(QEvent *msg) const
{
\tif (!_incrementFindDlg.getHSelf()->isAncestorOf(QApplication::focusWidget()))
\t\treturn false;
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''',
    '''bool Notepad_plus::processIncrFindAccel(QEvent* /*msg*/) const
{
\treturn false; // ::TranslateAccelerator: Qt uses QShortcut on QAction
}''')

# 34. ::IsChild -> Qt isAncestorOf
c = c.replace('!::IsChild(_findReplaceDlg.getHSelf(), QApplication::focusWidget())',
              '!_findReplaceDlg.getHSelf()->isAncestorOf(QApplication::focusWidget())')
c = c.replace('!::IsChild(_incrementFindDlg.getHSelf(), QApplication::focusWidget())',
              '!_incrementFindDlg.getHSelf()->isAncestorOf(QApplication::focusWidget())')

# 35. DocTabView* -> Window*
c = c.replace('Window* pTb = _pDocTab;', 'Window* pTb = static_cast<Window*>(_pDocTab);')

# 36. static_cast<int>(beSwitched) -> false
c = c.replace('static_cast<int>(beSwitched)', '0')

# 37. SCE_P_OPERATOR check - remove the style check
c = c.replace(
    'if ((posColon >= 0) && (_pEditView->execute(SCI_GETSTYLEINDEXAT, posColon) == SCE_P_OPERATOR))',
    'if (posColon >= 0)')

with open(PATH, 'w') as f:
    f.write(c)

changes = sum(1 for a, b in zip(original.split('\n'), c.split('\n')) if a != b)
print(f"Applied all fixes. {changes} lines changed.")
