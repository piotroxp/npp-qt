#!/usr/bin/env python3
"""Fix all remaining Notepad_plus.cpp errors - exact byte-level."""
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

# Fix 1: GoToLineDlg - isCreated/getHSelf not available
do_replace(
    b'\tif (_goToLineDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_goToLineDlg.getHSelf(), "GoToLine");\n\t}',
    b'\tif (_goToLineDlg.isVisible())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(&_goToLineDlg, "GoToLine");\n\t}',
    "GoToLineDlg: isCreated()->isVisible(), getHSelf()->&")

# Fix 2: RunMacroDlg - isCreated/getHSelf not available
do_replace(
    b'\tif (_runMacroDlg.isCreated())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(_runMacroDlg.getHSelf(), "MultiMacro");\n\t}',
    b'\tif (_runMacroDlg.isVisible())\n\t{\n\t\t_nativeLangSpeaker.changeDlgLang(&_runMacroDlg, "MultiMacro");\n\t}',
    "RunMacroDlg: isCreated()->isVisible(), getHSelf()->&")

# Fix 3: ProjectPanel setWorkSpaceFilePath - getWorkSpaceFilePath returns const wchar_t*
do_replace(
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID));',
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID));',
    "setWorkSpaceFilePath: const wchar_t* already correct")
# Actually the issue is: getWorkSpaceFilePath(panelID) returns const wchar_t*
# and setWorkSpaceFilePath(QString) needs QString. 
# Fix: convert wchar_t* to QString
do_replace(
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID));',
    b'\t\t(*pProjPanel)->setWorkSpaceFilePath(QString::fromWCharArray(nppParam.getWorkSpaceFilePath(panelID)));',
    "setWorkSpaceFilePath: wchar_t* -> QString")

# Fix 4: ProjectPanel openWorkSpace - wchar_t* is correct
do_replace(
    b'\t\t(*pProjPanel)->openWorkSpace(nppParam.getWorkSpaceFilePath(panelID).c_str());',
    b'\t\t(*pProjPanel)->openWorkSpace(nppParam.getWorkSpaceFilePath(panelID));',
    "openWorkSpace: remove redundant .c_str()")

# Fix 5: title_temp.c_str() - title_temp is QString, wcscpy_s needs wchar_t*
do_replace(
    b'\t\t\twcscpy_s(title, title_temp.c_str());\n\t\t\tdata.pszName = title;',
    b'\t\t\ttitle_temp.toWCharArray(title);\n\t\t\ttitle[title_temp.length()] = 0;\n\t\t\tdata.pszName = title;',
    "wcscpy_s: QString::toWCharArray")

# Fix 6: Add missing opening brace for showQuote (const was removed from declaration)
do_replace(
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nvoid Notepad_plus::showQuote(const QuoteParams* quote)\n{',
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nbool Notepad_plus::showQuote(const QuoteParams* quote)\n{',
    "showQuote: return bool, open brace")

# Fix 7: xmlFileName.empty() -> isEmpty()
do_replace(
    b'\t\tif (!xmlFileName.empty())',
    b'\t\tif (!xmlFileName.isEmpty())',
    "QString::empty() -> isEmpty()")

# Fix 8: Remove duplicate declaration of themeName_q inside if block
do_replace(
    b'\t\t\tQString themeName_q = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath));\n\t\t}\n\t\telse\n\t\t{\n\t\t\t//use _stylerPath;',
    b'\t\t\tthemeName_q = themeSwitcher.getThemeFromXmlFileName(QString::fromStdWString(themePath));\n\t\t}\n\t\telse\n\t\t{\n\t\t\t//use _stylerPath;',
    "Remove duplicate themeName_q declaration")

# Fix 9: themeName = getDefaultThemeLabel().toStdWString() - OK, keep as-is
# Fix 10: selectThemeByName(themeName.c_str()) - OK, keep as-is

# Fix 11: refreshPanelIcon void* hInst -> qApp
do_replace(
    b'\t\t\trefreshPanelIcon(this->getHinst(), panel);',
    b'\t\t\trefreshPanelIcon(qApp, panel);',
    "refreshPanelIcon: void* -> qApp")

# Fix 12: WINAPI calling convention - remove WINAPI from function definition
# The WINAPI was a leftover from Win32. It's a calling convention modifier.
do_replace(
    b'unsigned int WINAPI Notepad_plus::backupDocument(void * /*param*/)\n{',
    b'unsigned int Notepad_plus::backupDocument(void * /*param*/)\n{',
    "backupDocument: remove WINAPI calling convention")

# Fix 13: Missing return at end of showQuote
# showQuote was "void Notepad_plus::showQuote(...)" but declaration says "bool"
# Actually wait - I added "bool" to the definition above. But the declaration in the header
# was changed from "const" to non-const. The return type should be "void" (original).
# Let me check: the original was "void showQuote(const QuoteParams* quote) const"
# I removed the trailing "const". The return type is "void".
# But I changed it to "bool" in Fix 6 above. Let me fix that.
# Actually let me just revert the showQuote changes and fix the const issue differently.

# Revert showQuote fix to use void return
do_replace(
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nbool Notepad_plus::showQuote(const QuoteParams* quote)\n{',
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nvoid Notepad_plus::showQuote(const QuoteParams* quote)\n{',
    "showQuote: restore void return type")

# Fix the closing brace issue - showQuote needs to open its function
# The function declaration at line 8577 has no opening brace before it
# The previous function closes at 8575. Add { after the showQuote declaration
do_replace(
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nvoid Notepad_plus::showQuote(const QuoteParams* quote)\n{\n\tstatic TextPlayerParams params;',
    b'//-- showQuote: Displays a quote string from quotes.xml\n//\nvoid Notepad_plus::showQuote(const QuoteParams* quote)\n{\n\tstatic TextPlayerParams params;',
    "showQuote: ensure opening brace")

# Fix 14: pathAppend(themePath, xmlFileName) - xmlFileName is QString, themePath is wstring
# pathAppend takes (wstring&, const wstring&). xmlFileName.toStdWString() is needed.
# Check if this is already fixed - it should be.
do_replace(
    b'\t\t\tpathAppend(themePath, xmlFileName.toStdWString());\n\t\t\tpathAppend(themePath, xmlFileName.toStdWString());',
    b'\t\t\tpathAppend(themePath, xmlFileName.toStdWString());\n\t\t\tpathAppend(themePath, xmlFileName.toStdWString());',
    "pathAppend xmlFileName already has toStdWString")

# Fix 15: themePath.empty() - themePath is wstring, wstring has .empty()
do_replace(
    b'\t\tif (themePath.empty() || !doesFileExist(themePath.c_str()))',
    b'\t\tif (themePath.empty() || !doesFileExist(QString::fromStdWString(themePath)))',
    "doesFileExist: wstring.c_str() -> QString::fromStdWString(themePath)")

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'wb') as f:
    f.write(content)

print(f"\nTotal: {changes} changes")
sys.exit(0)