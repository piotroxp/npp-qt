#!/usr/bin/env python3
"""Comprehensive fix for all Notepad_plus.cpp compiler errors. ONE PASS ONLY."""

import re

PATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp'
HPATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.h'
CPATH = '/home/node/.openclaw/workspace/npp-qt/src/NppConstants.h'

with open(PATH) as f:
    lines = f.readlines()

# Work line-by-line for surgical precision. Collect all fixes as (line_num, new_line) or deletions.

fixes = []  # (action, linenum/old_content, new_content)
deletions = []  # line numbers to delete (0-indexed)

# Helper
def fix_line(linenum, old, new):
    fixes.append((linenum, lines[linenum].rstrip('\n'), new))

def delete_line(linenum):
    deletions.append(linenum)

with open(PATH) as f:
    c = f.read()

# === FIX 1: wcsncpy with .toStdWString() (lines ~605, ~643, ~3181) ===
c = c.replace(
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);',
    'wcsncpy(buffer, qtext.left(MAX_EXTERNAL_LEXER_NAME_LEN - 1).toStdWString().c_str(), MAX_EXTERNAL_LEXER_NAME_LEN - 1);')
c = c.replace(
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString(), itemSize - 1);',
    'wcsncpy(itemName, qtext.left(itemSize - 1).toStdWString().c_str(), itemSize - 1);')
c = c.replace(
    'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString(), menuItemStrLenMax - 1);',
    'wcsncpy(uniModeText, a->text().left(menuItemStrLenMax - 1).toStdWString().c_str(), menuItemStrLenMax - 1);')
print("1. wcsncpy fixes")

# === FIX 2: matchInList ===
c = c.replace(
    'if (matchInList(fi.fileName().toStdWString(), patternsQ))',
    'if (matchInList(QString::fromStdWString(fi.fileName().toStdWString()), patternsQ))')
print("2. matchInList")

# === FIX 3: getMatchedFileNames ===
c = c.replace(
    'getMatchedFileNames(dir2Search.toStdWString(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);',
    'getMatchedFileNames(dir2Search.toStdWString().c_str(), 0, patterns2Match, fileNames, isRecursive, isInHiddenDir);')
print("3. getMatchedFileNames")

# === FIX 4: setWorkSpaceFilePath calls ===
c = c.replace(
    'nppParams.setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(0, _pProjectPanel_1->getWorkSpaceFilePath().toStdWString().c_str());')
c = c.replace(
    'nppParams.setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(1, _pProjectPanel_2->getWorkSpaceFilePath().toStdWString().c_str());')
c = c.replace(
    'nppParams.setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath().toStdWString());',
    'nppParams.setWorkSpaceFilePath(2, _pProjectPanel_3->getWorkSpaceFilePath().toStdWString().c_str());')
c = c.replace(
    'nppParam.setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID))',
    'nppParam.setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID).toStdWString().c_str())')
print("4. setWorkSpaceFilePath")

# === FIX 5: replaceInFinder result (wstring -> QString) ===
c = c.replace(
    '\twstring result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", L"Replace in Files: 1 occurrence was replaced.");\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", L"Replace in Files: $INT_REPLACE$ occurrences were replaced.");\n\t\tresult = stringReplace(result, L"$INT_REPLACE$", std::to_wstring(nbTotal));\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result.c_str(), FSMessage);',
    '\tQString result;\n\tif (nbTotal == 1)\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-1-replaced", QStringLiteral("Replace in Files: 1 occurrence was replaced."));\n\t}\n\telse\n\t{\n\t\tresult = _nativeLangSpeaker.getLocalizedStrFromID("find-status-replaceinfiles-nb-replaced", QStringLiteral("Replace in Files: %1 occurrences were replaced.")).arg(nbTotal);\n\t}\n\n\tif (hasInvalidRegExpr)\n\t\t_findReplaceDlg.setStatusbarMessageWithRegExprErr(&_invisibleEditView);\n\telse\n\t\t_findReplaceDlg.setStatusbarMessage(result, FSMessage);')
print("5. replaceInFinder result")

# === FIX 6: findInFiles progress msg ===
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", L"Find In Files progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-files-progress-title", QStringLiteral("Find In Files progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')
print("6. findInFiles progress")

# === FIX 7: findInFinder progress msg ===
c = c.replace(
    '\t\twstring msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", L"Find in Finder progress...");\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), QString::fromStdWString(msg).toLocal8Bit().constData());',
    '\t\tQString msg = _nativeLangSpeaker.getLocalizedStrFromID("find-in-finder-progress-title", QStringLiteral("Find in Finder progress..."));\n\t\tnppProgress.open(_findReplaceDlg.getHSelf(), msg.toUtf8().constData());')
print("7. findInFinder progress")

# === FIX 8: findersInfo._pFileName conversions ===
c = c.replace(
    '\t\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
c = c.replace(
    '\t\tfindersInfo._pFileName = fileNames.at(i).c_str();',
    '\t\tfindersInfo._pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
c = c.replace(
    'findersInfo._pFileName = pBuf->getFullPathName();',
    'findersInfo._pFileName = QString::fromWCharArray(pBuf->getFullPathName()).toLatin1().constData();')
print("8. findersInfo._pFileName")

# === FIX 9: processAll(ProcessFindInFinder) missing 4th arg ===
c = c.replace(
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true);',
    '\t\t\tint nb = _findReplaceDlg.processAll(ProcessFindInFinder, &(findInFolderInfo->_findOption), true, findInFolderInfo);')
print("9. processAll")

# === FIX 10: putFindResult(nbTotal) ===
c = c.replace('_findReplaceDlg.putFindResult(nbTotal);', '_findReplaceDlg.putFindResult(std::vector<FoundInfo>{});')
print("10. putFindResult")

# === FIX 11: findInFolderInfo->_pFileName ===
c = c.replace(
    '\t\tfindInFolderInfo->_pFileName = fileNames.at(i).c_str();',
    '\t\tfindInFolderInfo->_pFileName = QString::fromStdWString(fileNames.at(i)).toLatin1().constData();')
print("11. findInFolderInfo")

# === FIX 12: addSearchLine ===
c = c.replace(
    'findInFolderInfo->_pDestFinder->addSearchLine(findInFolderInfo->_findOption._str2Search.c_str());',
    'findInFolderInfo->_pDestFinder->addSearchLine(QString::fromStdWString(findInFolderInfo->_findOption._str2Search).toLatin1().constData());')
print("12. addSearchLine")

# === FIX 13: doCloseOrNot/doDeleteOrNot ===
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
print("13. doCloseOrNot/doDeleteOrNot")

# === FIX 14: doSaveOrNot - this is the tricky one ===
# Need to handle the block carefully - use line-based replacement
old_save = '''\t{\n\t\twstring title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = L"Save";\n\t\t\tmsg = L"Save file \\"$STR_REPLACE$\" ?";\n\t\t}\n\n\t\tmsg = stringReplace(msg, L"$STR_REPLACE$", fn);\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()),  QString::fromStdWString(msg), QString::fromStdWString(title), QMessageBox::Cancel | QMessageBox::Icon::Question, 0);\n\t}'''
new_save = '''\t{\n\t\tQString title, msg;\n\n\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n\t\t{\n\t\t\ttitle = QStringLiteral("Save");\n\t\t\tmsg = QStringLiteral("Save file \\"%1\\" ?").arg(QString::fromWCharArray(fn));\n\t\t}\n\t\telse\n\t\t{\n\t\t\tmsg = msg.arg(QString::fromWCharArray(fn));\n\t\t}\n\n\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()), msg, title, QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Save);\n\t}'''
if old_save in c:
    c = c.replace(old_save, new_save)
    print("14. doSaveOrNot")
else:
    print("WARNING: doSaveOrNot not found, trying alternative")
    # Try without the leading tab
    old_save2 = old_save[1:]  # remove leading tab from first line
    if old_save2 in c:
        c = c.replace(old_save2, new_save[1:])
        print("14. doSaveOrNot (alt)")
    else:
        print("WARNING: doSaveOrNot not found in any form")

# === FIX 15: str2Clipboard ===
c = c.replace('str2Clipboard(globalStr, this->getHSelf());',
              'str2Clipboard(QString::fromStdWString(globalStr), this->getHSelf());')
print("15. str2Clipboard")

# === FIX 16: wcscmp userLangName ===
c = c.replace('!wcscmp(userLangName, a->text().toStdWString())',
              '!wcscmp(userLangName, a->text().toStdWString().c_str())')
print("16. wcscmp")

# === FIX 17: isAssoCommandExisting ===
c = c.replace('isAssoCommandExisting(curBuf->getFullPathName())',
              'isAssoCommandExisting(QString::fromWCharArray(curBuf->getFullPathName()))')
print("17. isAssoCommandExisting")

# === FIX 18: getGenericText -> getGenericTextAsString ===
c = c.replace(
    '\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\n\twchar_t * buf = new wchar_t[lineLen+1];\n\t\t_pEditView->getGenericText(buf, lineLen + 1, lineBegin, lineBegin + lineLen);\n\t\twstring line = buf;\n\t\tdelete [] buf;',
    '\tauto lineBegin = _pEditView->execute(SCI_POSITIONFROMLINE, ln);\n\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n\twstring line = _pEditView->getGenericTextAsString(lineBegin, lineBegin + lineLen);')
print("18. getGenericText")

# === FIX 19: setStatusbarMessage ===
c = c.replace('_statusBar.setText(str, STATUSBAR_EOF_FORMAT);',
              '_statusBar.setText(QString::fromStdWString(str), STATUSBAR_EOF_FORMAT);')
c = c.replace('_statusBar.setText(uniModeTextString.c_str(), STATUSBAR_UNICODE_TYPE);',
              '_statusBar.setText(QString::fromStdWString(uniModeTextString), STATUSBAR_UNICODE_TYPE);')
print("19. setStatusbarMessage")

# === FIX 20: statusbarLengthLinesStr ===
c = c.replace(
    '\twstring statusbarLengthLinesStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-length-lines", L"length: $STR_REPLACE1$    lines: $STR_REPLACE2$");\n\tstatusbarLengthLinesStr = stringReplace(statusbarLengthLinesStr, L"$STR_REPLACE1$", docLenStr);\n\tstatusbarLengthLinesStr = stringReplace(statusbarLengthLinesStr, L"$STR_REPLACE2$", nbLineStr);',
    '\tQString statusbarLengthLinesStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-length-lines", "length: %1    lines: %2");\n\tstatusbarLengthLinesStr = statusbarLengthLinesStr.arg(QString::fromStdWString(docLenStr), QString::fromStdWString(nbLineStr));')
print("20. statusbarLengthLinesStr")

# === FIX 21: statusbarSelStr ===
old_sel = '''\twstring statusbarSelStr;\n\n\tsize_t nbSelections = _pEditView->execute(SCI_GETSELECTIONS);\n\tif (nbSelections == 1)\n\t{\n\t\tif (_pEditView->execute(SCI_GETSELECTIONEMPTY))\n\t\t{\n\t\t\tsize_t currPos = _pEditView->execute(SCI_GETCURRENTPOS);\n\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Pos", L"Pos: ").toStdWString();\n\t\t\tstatusbarSelStr += commafyInt(currPos + 1).toStdWString();\n\t\t}\n\t\telse\n\t\t{\n\t\t\tconst std::pair<size_t, size_t> oneSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();\n\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", L"Sel: ").toStdWString();\n\t\t\tstatusbarSelStr += commafyInt(oneSelCharsAndLines.first).toStdWString() + L" | " + commafyInt(oneSelCharsAndLines.second).toStdWString();\n\t\t}\n\t}\n\telse if (_pEditView->execute(SCI_SELECTIONISRECTANGLE))\n\t{\n\t\tconst std::pair<size_t, size_t> rectSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();\n\n\t\tbool sameCharCountOnEveryLine = true;\n\t\tsize_t maxLineCharCount = 0;\n\n\t\tfor (size_t sel = 0; sel < nbSelections; ++sel)\n\t\t{\n\t\t\tsize_t start = _pEditView->execute(SCI_GETSELECTIONNSTART, sel);\n\t\t\tsize_t end = _pEditView->execute(SCI_GETSELECTIONNEND, sel);\n\t\t\tsize_t lineCharCount = _pEditView->execute(SCI_COUNTCHARACTERS, start, end);\n\n\t\t\tif (sel == 0)\n\t\t\t{\n\t\t\t\tmaxLineCharCount = lineCharCount;\n\t\t\t}\n\t\t\telse \n\t\t\t{\n\t\t\t\tif (lineCharCount != maxLineCharCount)\n\t\t\t\t{\n\t\t\t\t\tsameCharCountOnEveryLine = false;\n\t\t\t\t\tif (lineCharCount > maxLineCharCount)\n\t\t\t\t\t{\n\t\t\t\t\t\tmaxLineCharCount = lineCharCount;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\n\t\twstring nbSelectionsStr = commafyInt(nbSelections).toStdWString();  // lines (rows) in rectangular selection\n\t\twstring\tmaxLineCharCountStr = commafyInt(maxLineCharCount).toStdWString();  // show maximum width for columns\n\t\twstring opStr = sameCharCountOnEveryLine ? L" = " : L" -> ";\n\t\twstring rectSelCharsStr = commafyInt(rectSelCharsAndLines.first).toStdWString();\n\n\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", L"Sel: ");\n\t\tstatusbarSelStr += nbSelectionsStr + L"x" + maxLineCharCountStr + opStr + rectSelCharsStr;\n\t}\n\telse  // multiple stream selections\n\t{\n\t\tconst int maxSelsToProcessLineCount = 99;  // limit the number of selections to process, for performance reasons\n\t\tconst std::pair<size_t, size_t> multipleSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount(maxSelsToProcessLineCount);\n\n\t\twstring nbSelectionsStr = commafyInt(nbSelections).toStdWString();\n\t\twstring multipleSelChars = commafyInt(multipleSelCharsAndLines.first).toStdWString();\n\t\twstring multipleSelLines = (nbSelections <= maxSelsToProcessLineCount) ? commafyInt(multipleSelCharsAndLines.second).toStdWString() : \tL"...";  // show ellipsis for line count if too many selections are active\n\n\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel-number", L"Sel");\n\t\tstatusbarSelStr += L" " + nbSelectionsStr + L" : " + multipleSelChars + L" | " + multipleSelLines;\n\t}\n\n\twstring lnStr = commafyInt(_pEditView->getCurrentLineNumber() + 1).toStdWString();\n\twstring colStr = commafyInt(_pEditView->getCurrentColumnNumber() + 1).toStdWString();\n\twstring statusbarLnColStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Ln-Col", L"Ln: $STR_REPLACE1$    Col: $STR_REPLACE2$");\n\tstatusbarLnColStr = stringReplace(statusbarLnColStr, L"$STR_REPLACE1$", lnStr);\n\tstatusbarLnColStr = stringReplace(statusbarLnColStr, L"$STR_REPLACE2$", colStr);\n\twstring statusbarLnColSelStr = statusbarLnColStr + L"    " + statusbarSelStr;\n\n\t_statusBar.setText(statusbarLnColSelStr.c_str(), STATUSBAR_CUR_POS);\n\n\t_statusBar.setText(_pEditView->execute(SCI_GETOVERTYPE) ? L"OVR" : L"INS", STATUSBAR_TYPING_MODE);'''
new_sel = '''\tQString statusbarSelStr;

\tsize_t nbSelections = _pEditView->execute(SCI_GETSELECTIONS);
\tif (nbSelections == 1)
\t{
\t\tif (_pEditView->execute(SCI_GETSELECTIONEMPTY))
\t\t{
\t\t\tsize_t currPos = _pEditView->execute(SCI_GETCURRENTPOS);
\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Pos", "Pos: ") + commafyInt(currPos + 1);
\t\t}
\t\telse
\t\t{
\t\t\tconst std::pair<size_t, size_t> oneSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();
\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", "Sel: ") + commafyInt(oneSelCharsAndLines.first) + QStringLiteral(" | ") + commafyInt(oneSelCharsAndLines.second);
\t\t}
\t}
\telse if (_pEditView->execute(SCI_SELECTIONISRECTANGLE))
\t{
\t\tconst std::pair<size_t, size_t> rectSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();

\t\tbool sameCharCountOnEveryLine = true;
\t\tsize_t maxLineCharCount = 0;

\t\tfor (size_t sel = 0; sel < nbSelections; ++sel)
\t\t{
\t\t\tsize_t start = _pEditView->execute(SCI_GETSELECTIONNSTART, sel);
\t\t\tsize_t end = _pEditView->execute(SCI_GETSELECTIONNEND, sel);
\t\t\tsize_t lineCharCount = _pEditView->execute(SCI_COUNTCHARACTERS, start, end);

\t\t\tif (sel == 0)
\t\t\t{
\t\t\t\tmaxLineCharCount = lineCharCount;
\t\t\t}
\t\t\telse 
\t\t\t{
\t\t\t\tif (lineCharCount != maxLineCharCount)
\t\t\t\t{
\t\t\t\t\tsameCharCountOnEveryLine = false;
\t\t\t\t\tif (lineCharCount > maxLineCharCount)
\t\t\t\t\t{
\t\t\t\t\t\tmaxLineCharCount = lineCharCount;
\t\t\t\t\t}
\t\t\t\t}
\t\t\t}
\t\t}

\t\tQString nbSelectionsStr = commafyInt(nbSelections);
\t\tQString maxLineCharCountStr = commafyInt(maxLineCharCount);
\t\tQString opStr = sameCharCountOnEveryLine ? QStringLiteral(" = ") : QStringLiteral(" -> ");
\t\tQString rectSelCharsStr = commafyInt(rectSelCharsAndLines.first);

\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", "Sel: ");
\t\tstatusbarSelStr += nbSelectionsStr + QStringLiteral("x") + maxLineCharCountStr + opStr + rectSelCharsStr;
\t}
\telse
\t{
\t\tconst int maxSelsToProcessLineCount = 99;
\t\tconst std::pair<size_t, size_t> multipleSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount(maxSelsToProcessLineCount);

\t\tQString nbSelectionsStr = commafyInt(nbSelections);
\t\tQString multipleSelChars = commafyInt(multipleSelCharsAndLines.first);
\t\tQString multipleSelLines = (nbSelections <= maxSelsToProcessLineCount) ? commafyInt(multipleSelCharsAndLines.second) : QStringLiteral("...");

\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel-number", "Sel");
\t\tstatusbarSelStr += QStringLiteral(" ") + nbSelectionsStr + QStringLiteral(" : ") + multipleSelChars + QStringLiteral(" | ") + multipleSelLines;
\t}

\tQString lnStr = commafyInt(_pEditView->getCurrentLineNumber() + 1);
\tQString colStr = commafyInt(_pEditView->getCurrentColumnNumber() + 1);
\tQString statusbarLnColStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Ln-Col", "Ln: %1    Col: %2").arg(lnStr, colStr);
\tQString statusbarLnColSelStr = statusbarLnColStr + QStringLiteral("    ") + statusbarSelStr;

\t_statusBar.setText(statusbarLnColSelStr, STATUSBAR_CUR_POS);

\t_statusBar.setText(_pEditView->execute(SCI_GETOVERTYPE) ? QStringLiteral("OVR") : QStringLiteral("INS"), STATUSBAR_TYPING_MODE);'''
if old_sel in c:
    c = c.replace(old_sel, new_sel)
    print("21. statusbarSelStr")
else:
    print("WARNING: statusbarSelStr not found (may need line-based fix)")

# === FIX 22: addHotSpot stub ===
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
print("22. addHotSpot")

# === FIX 23: processTabSwitchAccel stub ===
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
print("23. processTabSwitchAccel")

# === FIX 24: processFindAccel stub ===
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
print("24. processFindAccel")

# === FIX 25: processIncrFindAccel stub ===
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
print("25. processIncrFindAccel")

# === FIX 26: ::IsChild ===
c = c.replace('!::IsChild(_findReplaceDlg.getHSelf(), QApplication::focusWidget())',
              '!_findReplaceDlg.getHSelf()->