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

# Fix 1: VerticalFileSwitcher init - HIMAGELIST -> nullptr
do_replace(
    'HIMAGELIST hImgLst = _mainDocTab.getImgLst(tabIconSet);\n\n\t\t_pDocumentListPanel->init(this->getHinst(), this->getHSelf(), hImgLst);',
    'HIMAGELIST hImgLst = _mainDocTab.getImgLst(tabIconSet);\n\n\t\t_pDocumentListPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), nullptr);\n\n\t\tQ_UNUSED(hImgLst);',
    "VerticalFileSwitcher::init: HIMAGELIST -> qApp + nullptr")

# Fix 2: VerticalFileSwitcher create - initializer list -> create(&data)
do_replace(
    '_pDocumentListPanel->create(&data, { IDR_DOCLIST_ICO, IDR_DOCLIST_ICO_DM, IDR_DOCLIST_ICO2 }, isRTL);',
    'data.addInfo = QStringLiteral("%1,%2,%3").arg(IDR_DOCLIST_ICO).arg(IDR_DOCLIST_ICO_DM).arg(IDR_DOCLIST_ICO2);\n\t\t_pDocumentListPanel->create(&data);',
    "VerticalFileSwitcher::create: initializer list -> addInfo + create(&data)")

# Fix 3: title_temp.c_str() - QString -> toStdWString().c_str()
do_replace(
    'if (title_temp.length() < 32)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.c_str());',
    'if (title_temp.length() < 32)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.toStdWString().c_str());',
    "title_temp: QString.c_str() -> toStdWString().c_str()")

# Fix 4: FileBrowser - FB_PANELTITLE/FOLDERASWORKSPACE_NODE undefined
do_replace(
    'NativeLangSpeaker *pNativeSpeaker = nppParams.getNativeLangSpeaker();\n\t\twstring title_temp = pNativeSpeaker->getAttrNameStr(FB_PANELTITLE, FOLDERASWORKSPACE_NODE, "PanelTitle");\n\n\t\tconst int titleLen = 64;\n\t\tstatic wchar_t title[titleLen];\n\t\tif (title_temp.length() < titleLen)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.c_str());\n\t\t\tdata.pszName = title;\n\t\t}',
    '/* FB_PANELTITLE / FOLDERASWORKSPACE_NODE are Win32 constants */\n\t\tdata.pszName = L"File Browser";',
    "FB_PANELTITLE/FOLDERASWORKSPACE_NODE: remove undefined constants")

# Fix 5: PM_PROJECTPANELTITLE undefined + QString/wstring mix
do_replace(
    'wstring title_no = to_wstring (panelID + 1);\n\t\twstring title_temp = pNativeSpeaker->getAttrNameStr(PM_PROJECTPANELTITLE, "ProjectManager", "PanelTitle").toStdWString() + L" " + title_no;\n\t\t(*pProjPanel)->setPanelTitle(QString::fromStdWString(title_temp));\n\t\tdata.pszName = (*pProjPanel)->getPanelTitle().toStdWString();',
    'wstring title_no = to_wstring (panelID + 1);\n\t\tQString titleBase = pNativeSpeaker->getAttrNameStr(QStringLiteral("Project Panel"), "ProjectManager", "PanelTitle");\n\t\tQString title_q = titleBase + QStringLiteral(" ") + QString::fromStdWString(title_no);\n\t\t(*pProjPanel)->setPanelTitle(title_q);\n\t\tstatic wchar_t title_w[128];\n\t\ttitle_q.toWCharArray(title_w);\n\t\tdata.pszName = title_w;',
    "PM_PROJECTPANELTITLE: fix undefined constant + QString/wstring mix")

# Fix 6: remove unused isRTL in ProjectPanel init
do_replace(
    'bool isRTL = pNativeSpeaker->isRTL();\n\t\tDockedWidgetData\tdata{};',
    'DockedWidgetData\tdata{};',
    "remove unused isRTL in launchProjectPanel")

# Fix 7: openWorkSpace redundant toStdWString().toStdWString()
do_replace(
    '(*pProjPanel)->openWorkSpace(QString::fromStdWString(nppParam.getWorkSpaceFilePath(panelID)).toStdWString().c_str());',
    '(*pProjPanel)->openWorkSpace(nppParam.getWorkSpaceFilePath(panelID).c_str());',
    "openWorkSpace: simplify to wstring.c_str()")

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'wb') as f:
    f.write(content)

print(f"\nTotal: {changes} changes")
sys.exit(0)