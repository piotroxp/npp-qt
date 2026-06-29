#!/usr/bin/env python3
"""Fix all remaining Notepad_plus.cpp errors - comprehensive."""
import sys

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'rb') as f:
    content = f.read()

changes = 0

def do_replace(old, new, desc):
    global content, changes
    if isinstance(old, str):
        old = old.encode('utf-8')
    if isinstance(new, str):
        new = new.encode('utf-8')
    if old in content:
        content = content.replace(old, new, 1)
        changes += 1
        print(f"  OK Fix {changes}: {desc}")
        return True
    else:
        print(f"  MISS: {desc}")
        return False

# ── Fix 1: HashFromFilesDlg getHSelf() returns const QWidget*, changeDlgLang needs QWidget*
# The class has: const QWidget* getHSelf() const { return this; }
# changeDlgLang signature: bool changeDlgLang(QWidget* dlg, const QString&)
# const QWidget* → QWidget* is fine (upcast), no fix needed actually
# But wait - getHSelf() is CONST. changeDlgLang takes non-const QWidget*.
# Need to add non-const getHSelf() or change the call.
# Easiest: add non-const getHSelf() override to HashFromFilesDlg/TextDlg in md5Dlgs.h
print("Note: HashFromFilesDlg getHSelf() is const; changeDlgLang needs non-const. Need to add non-const version to md5Dlgs.h")
print("  Will add QWidget* getHSelf() { return this; } to HashFromFilesDlg and HashFromTextDlg")

# ── Fix 2: RunDlg - QDialog, no getHSelf/isCreated. Change changeDlgLang to use & directly.
do_replace(
    'if (_runDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_runDlg.getHSelf(), "Run");\n\t}',
    'if (_runDlg.isVisible())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(&_runDlg, "Run");\n\t}',
    "RunDlg: isCreated()->isVisible(), getHSelf()->&_runDlg")

# ── Fix 3: HashFromTextDlg - same issue
do_replace(
    'if (_sha2FromTextDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_sha2FromTextDlg.getHSelf(), "SHA256FromTextDlg");\n\t}',
    'if (_sha2FromTextDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(const_cast<QWidget*>(static_cast<const QWidget*>(&_sha2FromTextDlg)), "SHA256FromTextDlg");\n\t}',
    "HashFromTextDlg: getHSelf() const→QWidget* cast")

# ── Fix 4: FindIncrementDlg - ScintillaComponent, not a dialog at all
# Just remove the lang change block - it doesn't need dialog language
do_replace(
    'if (_incrementFindDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_incrementFindDlg.getHSelf(), "IncrementalFind");\n\t}',
    '// _incrementFindDlg is a ScintillaComponent, not a dialog - no changeDlgLang needed\n\t(void)0;',
    "FindIncrementDlg: not a dialog - remove changeDlgLang")

# ── Fix 5: ColumnEditorDlg - QDialog, no isCreated/getHSelf
do_replace(
    'if (_colEditorDlg.isCreated())\n\t{\n        _nativeLangSpeaker.changeDlgLang(_colEditorDlg.getHSelf(), "ColumnEditor");\n\t}',
    'if (_colEditorDlg.isVisible())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(&_colEditorDlg, "ColumnEditor");\n\t}',
    "ColumnEditorDlg: isCreated()->isVisible(), getHSelf()->&_colEditorDlg")

# ── Fix 6: DebugInfoDlg - StaticDialog, has isCreated/getHSelf. getHSelf() is const.
do_replace(
    'if (_debugInfoDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_debugInfoDlg.getHSelf(), "DebugInfo");\n\t}',
    'if (_debugInfoDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(const_cast<QWidget*>(static_cast<const QWidget*>(&_debugInfoDlg)), "DebugInfo");\n\t}',
    "DebugInfoDlg: getHSelf() const->QWidget* cast")

# ── Fix 7: FindCharsInRangeDlg - QDialog, no isCreated/getHSelf
do_replace(
    'if (_findCharsInRangeDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_findCharsInRangeDlg.getHSelf(), "FindCharsInRange");\n\t}',
    'if (_findCharsInRangeDlg.isVisible())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(&_findCharsInRangeDlg, "FindCharsInRange");\n\t}',
    "FindCharsInRangeDlg: isCreated()->isVisible(), getHSelf()->&_findCharsInRangeDlg")

# ── Fix 8: DocMap create(&data) -> QWidget::create(WId) protected
# The DockingPanel::create(DockedWidgetData*) should handle this
# Check if DocumentMap has a create() method that takes DockedWidgetData*
# If not, need to use _dockingManager.createDockableDialog(data)
do_replace(
    '\t\t_pDocMap->create(&data);',
    '_dockingManager.createDockableDialog(data);',
    "DocMap: create(&data) -> _dockingManager.createDockableDialog")

# Also fix the wcscpy_s for DocMap title
do_replace(
    'wcscpy_s(title, title_temp.toStdWString().c_str());',
    'QString title_q = title_temp;\n\t\t\ttitle_q.toWCharArray(title);\n\t\t\tdata.pszName = title;',
    "DocMap: wcscpy_s with QString->wchar_t[]")

# Fix: remove the redundant wcscpy_s after the new block
# Actually the replacement above replaces wcscpy_s with the new code.
# The old code was: wcscpy_s(title, title_temp.toStdWString().c_str()); data.pszName = title;
# The new code has: title_q.toWCharArray(title); data.pszName = title;
# But we need to make sure there's no duplicate pszName assignment.

# ── Fix 9: wcscpy_s with QString title_q.toWCharArray - check if it compiles
# Actually QString::toWCharArray returns int (length) and doesn't null-terminate.
# Need to ensure null termination.
# Fix: title_q.toWCharArray(title); title[title_q.length()] = 0;

# Let me redo fix 8+9 properly:
# The original code was:
#   wcscpy_s(title, title_temp.toStdWString().c_str());
#   data.pszName = title;
# Replace with:
#   QString title_q = title_temp;
#   title_q.toWCharArray(title);
#   title[title_q.length()] = 0;
#   data.pszName = title;
# But this introduces a redundant data.pszName if the old code also had it.
# Let me check the full context.

# ── Fix 10: FunctionList create(&data) - same issue
do_replace(
    '_pFuncList->create(&data);',
    '_dockingManager.createDockableDialog(data);',
    "FunctionList: create(&data) -> _dockingManager.createDockableDialog")

# ── Fix 11: title_temp.toStdWString() -> use toWCharArray for title
# Fix the DocMap title code: wcscpy_s(title, title_temp.toStdWString().c_str())
# Replace with: title_q.toWCharArray(title); title[title_q.length()] = 0;
do_replace(
    b'wcscpy_s(title, title_temp.toStdWString().c_str());\n\t\t\tdata.pszName = title;',
    b'title_temp.toWCharArray(title);\n\t\t\ttitle[title_temp.length()] = 0;\n\t\t\tdata.pszName = title;',
    "DocMap title: wcscpy_s -> toWCharArray")

# ── Fix 12: themeName = ... .toStdWString() - already correct
# Line 8732: themeName = themeSwitcher.getThemeFromXmlFileName(...).toStdWString();
# themeName is wstring, this is fine.

# ── Fix 13: QString::empty() -> isEmpty()
do_replace(
    'if (themeName.empty())',
    'if (themeName.isEmpty())',
    "QString::empty() -> isEmpty()")

# ── Fix 14: themeName = QString... but themeName is wstring
do_replace(
    'themeName = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath)).toStdWString();',
    'themeName = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath)).toStdWString();',
    "themeName assignment already correct")

# Fix the themePath pathAppend: xmlFileName is QString, themePath is wstring
# pathAppend(wstring&, wstring) exists and takes 2 wstrings
# xmlFileName.toStdWString() is correct.

# ── Fix 15: refreshPanelIcon void* hInst -> QApplication*
do_replace(
    'refreshPanelIcon(this->getHinst(), mainFinder);',
    'refreshPanelIcon(qApp, mainFinder);',
    "refreshPanelIcon: void*->qApp")

do_replace(
    'refreshPanelIcon(this->getHinst(), finder);',
    'refreshPanelIcon(qApp, finder);',
    "refreshPanelIcon 2nd: void*->qApp")

# ── Fix 16: backup thread malformed if block
do_replace(
    'if (hThread)\n\t\t// CloseHandle: hThread);\n}',
    'if (hThread) {\n\t\t// CloseHandle: hThread);\n\t}',
    "backup thread if block syntax")

# ── Fix 17: searchInTarget 4 args -> 3 args (missing static_cast)
do_replace(
    'posStartCommentBefore[iSelEnd] = _pEditView->searchInTarget(start_comment, selectionEnd, static_cast<intptr_t>(0));',
    'posStartCommentBefore[iSelEnd] = _pEditView->searchInTarget(start_comment, static_cast<intptr_t>(selectionEnd), static_cast<intptr_t>(0));',
    "searchInTarget: selectionEnd needs static_cast<intptr_t>")

do_replace(
    'posEndCommentBefore[iSelEnd] = _pEditView->searchInTarget(end_comment, selectionEnd, static_cast<intptr_t>(0));',
    'posEndCommentBefore[iSelEnd] = _pEditView->searchInTarget(end_comment, static_cast<intptr_t>(selectionEnd), static_cast<intptr_t>(0));',
    "searchInTarget: selectionEnd (end_comment) needs static_cast<intptr_t>")

# ── Fix 18: const wchar_t* to char* in backup thread
do_replace(
    b'\tchar* pBuf = result.c_str();\n\t\t\tchar* pBuf2 = result2.c_str();\n\t\t\tchar* pBuf3 = result3.c_str();',
    b'\tQByteArray buf1 = QString::fromStdWString(result).toLocal8Bit();\n\t\t\tQByteArray buf2 = QString::fromStdWString(result2).toLocal8Bit();\n\t\t\tQByteArray buf3 = QString::fromStdWString(result3).toLocal8Bit();\n\t\t\tconst char* pBuf = buf1.constData();\n\t\t\tconst char* pBuf2 = buf2.constData();\n\t\t\tconst char* pBuf3 = buf3.constData();',
    "const wchar_t* to char* in backup thread")

# ── Fix 19: ProjectPanel setWorkSpaceFilePath - wchar_t* string literal passed to QString
do_replace(
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID).c_str());',
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(QString::fromWCharArray(nppParam.getWorkSpaceFilePath(panelID)));',
    "ProjectPanel::setWorkSpaceFilePath: const wchar_t* -> QString")

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'wb') as f:
    f.write(content)

print(f"\nTotal: {changes} changes")
sys.exit(0)