#!/usr/bin/env python3
"""Fix remaining Notepad_plus.cpp errors (lines 605-3200)."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp') as f:
    c = f.read()

orig = c

# 1. wcsncpy with .toStdWString() -> missing .c_str()
c = c.replace(
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);',
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString().c_str(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);')

# 2. wcsncpy with itemName
c = c.replace(
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString(), itemSize - 1);',
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString().c_str(), itemSize - 1);')

# 3. setWorkSpaceFilePath with .toStdWString()
c = c.replace(
    'nppParams.setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath().toStdWString().c_str());')
c = c.replace(
    'nppParams.setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath().toStdWString().c_str());')
c = c.replace(
    'nppParams.setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath().toStdWString().c_str());')

# 4. matchInList(fi.fileName().toStdWString(), patternsQ) -> QString
c = c.replace(
    'if (matchInList(fi.fileName().toStdWString(), patternsQ))',
    'if (matchInList(QString::fromStdWString(fi.fileName().toStdWString()), patternsQ))')

# 5. getMatchedFileNames - add .c_str()
c = c.replace(
    'getMatchedFileNames(dir2Search.toStdWString(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);',
    'getMatchedFileNames(dir2Search.toStdWString().c_str(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);')

# 6. replaceInFinder result (already fixed earlier - check if still needed)
# Lines 2075-2095: wstring result -> QString result
c = c.replace(
    '\twstring result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", L"Replace in Files: 1 occurrence was replaced.");\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", L"Replace in Files: $INT_REPLACE$ occurrences were replaced.");\n\t\tresult = stringReplace(result, L"$INT_REPLACE$", std::to_wstring(nbTotal));\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result.c_str(), FSMessage);',
    '\tQString result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", QStringLiteral("Replace in Files: 1 occurrence was replaced."));\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", QStringLiteral("Replace in Files: %1 occurrences were replaced.")).arg(nbTotal);\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result, FSMessage);')

# 7. findInFiles progress - wstring msg -> QString msg
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", L"Find In Files progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", QStringLiteral("Find In Files progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')

# 8. findInFinder progress
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", L"Find in Finder progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", QStringLiteral("Find in Finder progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')

# 9. findersInfo._pFileName = fileNames.at(i).c_str() (findInFinder loop)
# This appears in multiple places in findInFinder
c = c.replace(
    '\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
c = c.replace(
    '\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')

# 10. findersInfo._pFileName = pBuf->getFullPathName() 
c = c.replace(
    'findersInfo._pFileName = pBuf->getFullPathName();',
    'findersInfo._pFileName = QString::fromWCharArray(pBuf->getFullPathName()).toLatin1().constData();')

# 11. processAll(ProcessFindInFinder) - add 4th arg
c = c.replace(
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true);',
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true, findInFolderInfo);')

# 12. putFindResult(nbTotal) - fix
c = c.replace('_findReplaceDlg.putFindResult(nbTotal);', '_findReplaceDlg.putFindResult(std::vector<FoundInfo>{});')

# 13. findInFolderInfo->_pFileName = fileNames.at(i).c_str()
c = c.replace(
    '\t\tfindInFolderInfo->_pFileName = fileNames.at(i).c_str();',
    '\t\tfindInFolderInfo->_pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')

# 14. addSearchLine with c_str()
c = c.replace(
    'findInFolderInfo->_pDestFinder->addSearchLine(findInFolderInfo->_findOption._str2Search.c_str());',
    'findInFolderInfo->_pDestFinder->addSearchLine(QString::fromStdWString(findInFolderInfo->_findOption._str2Search).toLatin1().constData());')

# 15. doCloseOrNot/doDeleteOrNot - messageBox
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

# 16. doSaveOrNot
c = c.replace(
    '\t\twstring title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = L"Save";\n\t\t\tmsg = L"Save file \\"$STR_REPLACE$\" ?";\n\t\t}\n\n\t\tmsg = stringReplace(msg, L"$STR_REPLACE$", fn);\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()),  QString::fromStdWString(msg), QString::fromStdWString(title), QMessageBox::Cancel | QMessageBox::Icon::Question, 0);',
    '\t\tQString title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = QStringLiteral("Save");\n\t\t\tmsg = QStringLiteral("Save file \\"%1\\" ?").arg(QString::fromWCharArray(fn));\n\t\t}\n\t\telse\n\t\t{\n\t\t\tmsg = msg.arg(QString::fromWCharArray(fn));\n\t\t}\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()), msg, title, QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Save);')

# 17. str2Clipboard
c = c.replace('str2Clipboard(globalStr, this->getHSelf());',
              'str2Clipboard(QString::fromStdWString(globalStr), this->getHSelf());')

# 18. wcscmp
c = c.replace('!wcscmp(userLangName, a->text().toStdWString())',
              '!wcscmp(userLangName, a->text().toStdWString().c_str())')

# 19. isAssoCommandExisting
c = c.replace('isAssoCommandExisting(curBuf->getFullPathName())',
              'isAssoCommandExisting(QString::fromWCharArray(curBuf->getFullPathName()))')

# 20. uniModeText setText
c = c.replace('wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString(), menuItemStrLenMax - 1);',
              'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString().c_str(), menuItemStrLenMax - 1);')

# 21. setStatusbarMessage with uniModeTextString.c_str()
c = c.replace('_statusBar.setText(uniModeTextString.c_str(), STATUSBAR_UNICODE_TYPE);',
              '_statusBar.setText(QString::fromStdWString(uniModeTextString), STATUSBAR_UNICODE_TYPE);')

# 22. autoIndent fix
c = c.replace('autoIndentMode != autoIndent_none', 'autoIndentMode != AutoIndentMode::AI_NONE')
c = c.replace('autoIndentMode == autoIndent_none', 'autoIndentMode == AutoIndentMode::AI_NONE')

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w') as f:
    f.write(c)

n_changes = sum(1 for a, b in zip(orig.split('\n'), c.split('\n')) if a != b)
print(f"fix3.py: {n_changes} lines changed")
