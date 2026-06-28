#!/usr/bin/env python3
"""Fix remaining Notepad_plus.cpp errors."""

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
        return False

# ── Fix 1: MultiByteToWideChar / WideCharToMultiByte – inline wrappers ──
# Add helper lambdas before the function that uses them
old = 'void Notepad_plus::processURLIndic(ScintillaEditView* pView, int startPos, int endPos)\n{'
new = '''static int winMultiByteToWideChar(unsigned int cp, const char* src, int srcLen, wchar_t* dst, int dstLen)
{
#ifdef _WIN32
    return ::MultiByteToWideChar(cp, 0, src, srcLen, dst, dstLen);
#else
    Q_UNUSED(cp);
    if (!dst) return srcLen;
    int n = qMin(srcLen, dstLen - 1);
    for (int i = 0; i < n; ++i) dst[i] = static_cast<wchar_t>(static_cast<unsigned char>(src[i]));
    dst[n] = 0;
    return n;
#endif
}

static int winWideCharToMultiByte(unsigned int cp, const wchar_t* src, int srcLen, char* dst, int dstLen)
{
#ifdef _WIN32
    return ::WideCharToMultiByte(cp, 0, src, srcLen, dst, dstLen, nullptr, nullptr);
#else
    Q_UNUSED(cp);
    if (!dst) return srcLen;
    int n = qMin(srcLen, dstLen - 1);
    for (int i = 0; i < n; ++i) dst[i] = static_cast<char>(src[i]);
    dst[n] = 0;
    return n;
#endif
}

void Notepad_plus::processURLIndic(ScintillaEditView* pView, int startPos, int endPos)
{'''
do_replace(old, new, "Add Win32 wrappers for MultiByteToWideChar/WideCharToMultiByte")

# Update the actual call sites to use the wrappers
do_replace('int wideTextLen = MultiByteToWideChar(cp, 0, encodedText, static_cast<int>(endPos - startPos + 1), (LPWSTR) wideText, static_cast<int>(endPos - startPos + 1)) - 1;',
           'int wideTextLen = winMultiByteToWideChar(cp, encodedText, static_cast<int>(endPos - startPos + 1), wideText, static_cast<int>(endPos - startPos + 1)) - 1;',
           "MultiByteToWideChar -> winMultiByteToWideChar")

do_replace('lenEncoded = WideCharToMultiByte(cp, 0, & wideText [startWide], lenWide, NULL, 0, NULL, NULL);',
           'lenEncoded = winWideCharToMultiByte(cp, & wideText [startWide], lenWide, nullptr, 0);',
           "WideCharToMultiByte -> winWideCharToMultiByte")

# ── Fix 2: autoIndent_none → AutoIndentMode::none ──
do_replace('\tif (nppGui._maintainIndent == autoIndent_none)',
           '\tif (nppGui._maintainIndent == AutoIndentMode::none)',
           "autoIndent_none -> AutoIndentMode::none")

# ── Fix 3: FS_PROJECTPANELTITLE → hardcoded string ──
do_replace("\twstring title_temp = pNativeSpeaker->getAttrNameStr(FS_PROJECTPANELTITLE, \"DocList\", \"PanelTitle\");",
           "\tQString title_temp = pNativeSpeaker->getAttrNameStr(QStringLiteral(\"Document List\"), \"DocList\", \"PanelTitle\");",
           "FS_PROJECTPANELTITLE -> QStringLiteral string")

do_replace("\tstatic wchar_t title[32];\n\tif (title_temp.length() < 32)\n\t{\n\t\twcscpy_s(title, title_temp.c_str());",
           "\tstatic wchar_t title[32];\n\tif (title_temp.length() < 32)\n\t{\n\t\twcscpy_s(title, title_temp.toStdWString().c_str());",
           "title_temp QString: wcscpy_s fix")

# ── Fix 4: isClosed() → !isVisible() ──
do_replace('\tif (!_pDocumentListPanel->isClosed()) // if doclist is open',
           '\tif (_pDocumentListPanel->isVisible()) // if doclist is open',
           "isClosed() -> isVisible()")

# ── Fix 5: destroy() → deleteLater() (protected) ──
do_replace('\t\t\t_pDocumentListPanel->destroy();\n\t\t\t_pDocumentListPanel = nullptr;\n\n\t\t\t//relaunch with new icons\n\t\t\tlaunchDocumentListPanel(changeFromBtnCmd);',
           '\t\t\t_pDocumentListPanel->deleteLater();\n\t\t\t_pDocumentListPanel = nullptr;\n\n\t\t\t//relaunch with new icons\n\t\t\tlaunchDocumentListPanel(changeFromBtnCmd);',
           "destroy() -> deleteLater() in changeDocumentListIconSet (first)")

do_replace('\t\t\t_pDocumentListPanel->destroy();\n\t\t\t_pDocumentListPanel = nullptr;\n\n\t\t\t//relaunch doclist with new icons and close it\n\t\t\tlaunchDocumentListPanel(changeFromBtnCmd);',
           '\t\t\t_pDocumentListPanel->deleteLater();\n\t\t\t_pDocumentListPanel = nullptr;\n\n\t\t\t//relaunch doclist with new icons and close it\n\t\t\tlaunchDocumentListPanel(changeFromBtnCmd);',
           "destroy() -> deleteLater() in changeDocumentListIconSet (second)")

# ── Fix 6: ProjectPanel createDockableDialog → create(&data) ──
do_replace('\t\t(*pProjPanel)->createDockableDialog(data);',
           '\t\t(*pProjPanel)->create(&data);',
           "ProjectPanel::createDockableDialog -> create(&data)")

# ── Fix 7: setWorkSpaceFilePath – already converted, but needs to pass const wchar_t* ──
# Current: (*pProjPanel)->setWorkSpaceFilePath(QString::fromStdWString(nppParam.getWorkSpaceFilePath(panelID)).toStdWString().c_str());
# Check what signature ProjectPanel::setWorkSpaceFilePath expects
do_replace('\t\t(*pProjPanel)->setWorkSpaceFilePath(QString::fromStdWString(nppParam.getWorkSpaceFilePath(panelID)).toStdWString().c_str());',
           '\t\t(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID).c_str());',
           "setWorkSpaceFilePath: use wstring.c_str() directly")

# ── Write back ──
with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print(f"\nTotal changes: {changes}")