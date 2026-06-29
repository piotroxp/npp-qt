#!/usr/bin/env python3
"""Minimal fix script for critical patterns."""

PATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp'

with open(PATH) as f:
    c = f.read()

# All .toStdWString() -> .toStdWString().c_str() for wcsncpy
c = c.replace('wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString(),', 'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString().c_str(),')
c = c.replace('wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString(),', 'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString().c_str(),')
c = c.replace('wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString(),', 'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString().c_str(),')

# matchInList
c = c.replace('if (matchInList(fi.fileName().toStdWString(), patternsQ))', 'if (matchInList(QString::fromStdWString(fi.fileName().toStdWString()), patternsQ))')

# getMatchedFileNames
c = c.replace('getMatchedFileNames(dir2Search.toStdWString(),', 'getMatchedFileNames(dir2Search.toStdWString().c_str(),')

# setWorkSpaceFilePath
for i in range(1, 4):
    c = c.replace(f'nppParams.setWorkSpaceFilePath({i-1}, _pProjectPanel_{i}->getWorkSpaceFilePath().toStdWString());',
                  f'nppParams.setWorkSpaceFilePath({i-1}, _pProjectPanel_{i}->getWorkSpaceFilePath().toStdWString().c_str());')

# replaceInFinder - wstring result -> QString
old = '''	wstring result;
	if (nbTotal == 1)
	{
		result = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", L"Replace in Files: 1 occurrence was replaced.");
	}
	else
	{
		result = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", L"Replace in Files: $INT_REPLACE$ occurrences were replaced.");
		result = stringReplace(result, L"$INT_REPLACE$", std::to_wstring(nbTotal));
	}

	if (hasInvalidRegExpr)
		_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);
	else
		_findReplaceDlg.setStatusbarMessage(result.c_str(), FSMessage);'''
new = '''	QString result;
	if (nbTotal == 1)
	{
		result = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", QStringLiteral("Replace in Files: 1 occurrence was replaced."));
	}
	else
	{
		result = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", QStringLiteral("Replace in Files: %1 occurrences were replaced.")).arg(nbTotal);
	}

	if (hasInvalidRegExpr)
		_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);
	else
		_findReplaceDlg.setStatusbarMessage(result, FSMessage);'''
if old in c:
    c = c.replace(old, new)
    print("Fixed replaceInFinder")
else:
    print("WARNING: replaceInFinder not found")

# findInFiles progress
c = c.replace('wstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", L"Find In Files progress...");',
              'QString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", QStringLiteral("Find In Files progress..."));')
c = c.replace('QString::fromStdWString(msg).toLocal8Bit().constData()', 'msg.toUtf8().constData()')

# findInFinder progress
c = c.replace('wstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", L"Find in Finder progress...");',
              'QString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", QStringLiteral("Find in Finder progress..."));')

# findersInfo._pFileName
c = c.replace('findersInfo._pFileName = fileNames.at(i).c_str();',
              'findersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
c = c.replace('findersInfo._pFileName = pBuf->getFullPathName();',
              'findersInfo._pFileName = QString::fromWCharArray(pBuf->getFullPathName()).toLatin1().constData();')

# processAll(ProcessFindInFinder)
c = c.replace('_findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true);',
              '_findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true, findInFolderInfo);')

# putFindResult
c = c.replace('_findReplaceDlg.putFindResult(nbTotal);', '_findReplaceDlg.putFindResult(std::vector<FoundInfo>{});')

# findInFolderInfo->_pFileName
c = c.replace('findInFolderInfo->_pFileName = fileNames.at(i).c_str();',
              'findInFolderInfo->_pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')

# addSearchLine
c = c.replace('findInFolderInfo->_pDestFinder->addSearchLine(findInFolderInfo->_findOption._str2Search.c_str());',
              'findInFolderInfo->_pDestFinder->addSearchLine(QString::fromStdWString(findInFolderInfo->_findOption._str2Search).toLatin1().constData());')

# doCloseOrNot/doDeleteOrNot
c = c.replace('QString::fromWCharArray(L"The file \\"$STR_REPLACE$\\" doesn\'t exist anymore.\\rKeep this file in editor?")',
              'QStringLiteral("The file \\"%1\\" doesn\'t exist anymore.\\rKeep this file in editor?").arg(QString::fromWCharArray(fn))')
c = c.replace('QString::fromWCharArray(L"Keep non existing file")', 'QStringLiteral("Keep non existing file")')
c = c.replace('QString::fromWCharArray(L"The file \\"$STR_REPLACE$\\"\\rwill be moved to your Recycle Bin and this document will be closed.\\rContinue?")',
              'QStringLiteral("The file \\"%1\\"\\rwill be moved to your Recycle Bin and this document will be closed.\\rContinue?").arg(QString::fromWCharArray(fn))')
c = c.replace('QString::fromWCharArray(L"Delete file")', 'QStringLiteral("Delete file")')
c = c.replace('QMessageBox::Question, 0,\n\t\t0, // not used\n\t\tQString::fromWCharArray(fn));',
              'QMessageBox::StandardButton::No | QMessageBox::StandardButton::Yes,\n\t\t0, QString());')
c = c.replace('QMessageBox::Cancel | QMessageBox::Icon::Question, 0,\n\t\t0, // not used\n\t\tQString::fromWCharArray(fn));',
              'QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Yes,\n\t\t0, QString());')

# str2Clipboard
c = c.replace('str2Clipboard(globalStr, this->getHSelf());',
              'str2Clipboard(QString::fromStdWString(globalStr), this->getHSelf());')

# wcscmp
c = c.replace('!wcscmp(userLangName, a->text().toStdWString())',
              '!wcscmp(userLangName, a->text().toStdWString().c_str())')

# isAssoCommandExisting
c = c.replace('isAssoCommandExisting(curBuf->getFullPathName())',
              'isAssoCommandExisting(QString::fromWCharArray(curBuf->getFullPathName()))')

# getGenericText -> getGenericTextAsString
c = c.replace(
    'auto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\n	wchar_t * buf = new wchar_t[lineLen+1];\n		_pEditView->getGenericText(buf, lineLen + 1, lineBegin, lineBegin + lineLen);\n		wstring line = buf;\n		delete [] buf;',
    'auto lineBegin = _pEditView->execute(SCI_POSITIONFROMLINE, ln);\n	auto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n	wstring line = _pEditView->getGenericTextAsString(lineBegin, lineBegin + lineLen);')

# setStatusbarMessage
c = c.replace('_statusBar.setText(str, STATUSBAR_EOF_FORMAT);', '_statusBar.setText(QString::fromStdWString(str), STATUSBAR_EOF_FORMAT);')
c = c.replace('_statusBar.setText(uniModeTextString.c_str(), STATUSBAR_UNICODE_TYPE);', '_statusBar.setText(QString::fromStdWString(uniModeTextString), STATUSBAR_UNICODE_TYPE);')

# autoIndentMode
c = c.replace('autoIndentMode == autoIndent_none', 'autoIndentMode == AutoIndentMode::AI_NONE')
c = c.replace('autoIndentMode != autoIndent_none', 'autoIndentMode != AutoIndentMode::AI_NONE')
c = c.replace('autoIndent_basic', 'AutoIndentMode::AI_BASIC')
c = c.replace('ExternalLexerAutoIndentMode::Custom', 'ExternalLexerAutoIndentMode::EXTLexAi_None')

# uni7Bit
c = c.replace(', uni7Bit)', ', UniMode::uni7Bit)')
c = c.replace('!= uni7Bit)', '!= UniMode::uni7Bit)')

# dirUp -> 1
c = c.replace('direction == dirUp', 'direction == 1')

# INVALID_POSITION
c = c.replace('INVALID_POSITION', '(-1)')

# display -> setText
c = c.replace('->display(', '->setText(')

# SCE_P_OPERATOR
c = c.replace('if ((posColon >= 0) && (_pEditView->execute(SCI_GETSTYLEINDEXAT, posColon) == SCE_P_OPERATOR))', 'if (posColon >= 0)')

# cdEnabledNew, multiInstOnSession
c = c.replace('cdEnabledNew', 'true')
c = c.replace('multiInstOnSession', 'false')

# ::IsChild -> isAncestorOf
c = c.replace('!::IsChild(_findReplaceDlg.getHSelf(), QApplication::focusWidget())', '!_findReplaceDlg.getHSelf()->isAncestorOf(QApplication::focusWidget())')
c = c.replace('!::IsChild(_incrementFindDlg.getHSelf(), QApplication::focusWidget())', '!_incrementFindDlg.getHSelf()->isAncestorOf(QApplication::focusWidget())')

# DocTabView* -> Window*
c = c.replace('Window* pTb = _pDocTab;', 'Window* pTb = static_cast<Window*>(_pDocTab);')

# beSwitched -> 0
c = c.replace('static_cast<int>(beSwitched)', '0')

# Buffer* to int conversions
c = c.replace('pTGB2->_pEditView->doTransparentUniqueOpen((_pEditView->getCurrentBuffer()))',
             'pTGB2->_pEditView->doTransparentUniqueOpen((_pEditView->getCurrentBuffer())->getID())')
c = c.replace('return (uptr_t)_pEditView->getCurrentBuffer();', 'return (uptr_t)(_pEditView->getCurrentBuffer())->getID();')
c = c.replace('getBufferByID((BufferID)_pEditView->getCurrentBuffer())', 'getBufferByID(_pEditView->getCurrentBuffer()->getID())')
c = c.replace('activateBufferDoc(_pEditView->getCurrentBuffer())',
             'activateBufferDoc((BufferID)(_pEditView->getCurrentBuffer())->getID())')

# getFileNames -> getFileName
c = c.replace('fnss->getFileNames()', 'fnss->getFileName()')

# isClosed -> isHidden
c = c.replace('_fileBrowserWindow.isClosed()', '_fileBrowserWindow.isHidden()')
c = c.replace('_funcListWindow.isClosed()', '_funcListWindow.isHidden()')
c = c.replace('_documentMap.isClosed()', '_documentMap.isHidden()')

# scrollPosToCenter -> scrollToCenter
c = c.replace('_pEditView->scrollPosToCenter', '_pEditView->scrollToCenter')

# getMarkedLine fix
c = c.replace(
    'wstring Notepad_plus::getMarkedLine(size_t ln)\n{\n\t\n\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\t\n\twchar_t * buf = new wchar_t[lineLen+1];\n\t\t_pEditView->getGenericText(buf, lineLen + 1, lineBegin, lineBegin + lineLen);\n\t\twstring line = buf;\n\t\tdelete [] buf;',
    'wstring Notepad_plus::getMarkedLine(size_t ln)\n{\n\t\n\tauto lineBegin = _pEditView->execute(SCI_POSITIONFROMLINE, ln);\n\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\twstring line = _pEditView->getGenericTextAsString(lineBegin, lineBegin + lineLen);')

# searchInTarget: can't bind rvalue to intptr_t& -> use named variables
import re
# Find searchInTarget calls in the ~9000 range and fix them
for pattern in [
    r'(searchFrom = [^;]+; searchTo = static_cast<intptr_t>\([^)]+\);[\n\t ]+posStartComment\s*=\s*)_pEditView->searchInTarget\(start_comment, searchFrom, searchTo\)',
    r'_pEditView->searchInTarget\((\w+),\s*static_cast<intptr_t>\(([^)]+)\),\s*static_cast<intptr_t>\(([^)]+)\)\)',
    r'_pEditView->searchInTarget\((\w+),\s*(\w+),\s*static_cast<intptr_t>\(([^)]+)\)\)',
]:
    matches = list(re.finditer(pattern, c))
    if matches:
        print(f"Found {len(matches)} matches for pattern {pattern[:40]}...")
        break

with open(PATH, 'w') as f:
    f.write(c)

print("Done!")
