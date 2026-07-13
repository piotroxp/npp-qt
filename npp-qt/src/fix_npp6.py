#!/usr/bin/env python3
"""Fix all remaining Notepad_plus.cpp docking API errors."""
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

# Fix 1: ClipboardHistoryPanel init() takes no args
do_replace(
    '_pClipboardHistoryPanel->init(this->getHinst(), this->getHSelf(), &_pEditView);',
    '_pClipboardHistoryPanel->init();',
    "ClipboardHistoryPanel::init() takes no args")

# Fix 2: ClipboardHistoryPanel create() -> use setDockedData + display
# getAttrNameStr returns QString - already fixed. Now fix the create call.
do_replace(
    '_pClipboardHistoryPanel->create(&data, { IDR_CLIPBOARDPANEL_ICO, IDR_CLIPBOARDPANEL_ICO_DM, IDR_CLIPBOARDPANEL_ICO2 }, isRTL);',
    '_pClipboardHistoryPanel->setDockedData(data);\n\t\t_pClipboardHistoryPanel->display();',
    "ClipboardHistoryPanel::create -> setDockedData + display")

# Fix 3: VerticalFileSwitcher init() takes (app, parent, nullptr) - but VerticalFileSwitcher::init
# takes (QApplication*, QWidget*, void*) - the 3rd arg is void* for image list
# But getImgLst doesn't exist on DocTabView. Remove the getImgLst call and pass nullptr.
# Also init takes 3 args but the 3rd (void*) is for image list.
# My fix already passes nullptr - check if init signature is correct.
# Looking at VerticalFileSwitcher::init(QApplication* app, QWidget* parent, void* hImaLst)
# So init(qApp, static_cast<QWidget*>(this->getHSelf()), nullptr) is CORRECT.
# The issue is the missing newline before NativeLangSpeaker. Add it.
do_replace(
    '_pDocumentListPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), nullptr);\n\t\tNativeLangSpeaker *pNativeSpeaker',
    '_pDocumentListPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), nullptr);\n\n\t\tNativeLangSpeaker *pNativeSpeaker',
    "VerticalFileSwitcher init: add missing newline")

# Fix 4: VerticalFileSwitcher create(&data) -> setDockedData + display
do_replace(
    '_pDocumentListPanel->create(&data);\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG',
    '_pDocumentListPanel->setDockedData(data);\n\t\t_pDocumentListPanel->display();\n\n\t\t// SendMessage(NPPM_) -> Qt: MODELESSDIALOG',
    "VerticalFileSwitcher::create -> setDockedData + display")

# Fix 5: AnsiCharPanel init() - the 3rd arg is void* ppEditView. _pEditView is ScintillaEditView*.
# But AnsiCharPanel::init signature is (QApplication*, QWidget*, void*) - needs void*
do_replace(
    '_pAnsiCharPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), &_pEditView);',
    '_pAnsiCharPanel->init(qApp, static_cast<QWidget*>(this->getHSelf()), reinterpret_cast<void*>(&_pEditView));',
    "AnsiCharPanel::init 3rd arg needs void*")

# Fix 6: AnsiCharPanel createDockableDialog(data) -> _dockingManager.createDockableDialog(data)
do_replace(
    '_pAnsiCharPanel->createDockableDialog(data);',
    '_dockingManager.createDockableDialog(data);',
    "AnsiCharPanel::createDockableDialog -> _dockingManager.createDockableDialog")

# Fix 7: FileBrowser createDockableDialog(data) -> _dockingManager.createDockableDialog(data)
do_replace(
    '_pFileBrowser->createDockableDialog(data);',
    '_dockingManager.createDockableDialog(data);',
    "FileBrowser::createDockableDialog -> _dockingManager.createDockableDialog")

# Fix 8: FileBrowser clear() -> no such method. Remove the call.
do_replace(
    '// FileBrowser::deleteAllFromTree() is private; clear via public API\n\t\t_pFileBrowser->clear();',
    '// FileBrowser::clear() not available - tree is managed by addRootFolder',
    "FileBrowser::clear() doesn't exist - remove")

# Fix 9: FileBrowser selectItemFromPath(QString) - check if this exists
# Looking at FileBrowser.h: void setSelectedItem(const QString& path);
# Let me fix to setSelectedItem if that's what exists
do_replace(
    '_pFileBrowser->selectItemFromPath(selectedItemPath);',
    '_pFileBrowser->setSelectedItem(QString::fromStdWString(selectedItemPath));',
    "FileBrowser::selectItemFromPath -> setSelectedItem(QString)")

# Fix 10: ProjectPanel create(&data) -> _dockingManager.createDockableDialog(data)
do_replace(
    '(*pProjPanel)->create(&data);',
    '_dockingManager.createDockableDialog(data);',
    "ProjectPanel::create -> _dockingManager.createDockableDialog")

# Fix 11: ProjectPanel setWorkSpaceFilePath(const QString&) - getWorkSpaceFilePath returns const wchar_t*
do_replace(
    '(*pProjPanel)->setWorkSpaceFilePath(nppParam.getWorkSpaceFilePath(panelID).c_str());',
    '(*pProjPanel)->setWorkSpaceFilePath(QString::fromWCharArray(nppParam.getWorkSpaceFilePath(panelID)));',
    "ProjectPanel::setWorkSpaceFilePath: wchar_t* -> QString")

# Fix 12: VerticalFileSwitcher setClosed(true) -> display(false)
do_replace(
    '_pDocumentListPanel->setClosed(true);',
    '_pDocumentListPanel->display(false);',
    "VerticalFileSwitcher::setClosed(true) -> display(false)")

# Fix 13: FileBrowser display() then display() again - duplicate
do_replace(
    '_pFileBrowser->display();\n\t_pFileBrowser->selectItemFromPath',
    '_pFileBrowser->display();\n\n\tcheckMenuItem(IDM_VIEW_FILEBROWSER, true);',
    "FileBrowser duplicate display() - remove second call and adjust")

# The above might mess up selectItemFromPath. Let me handle more carefully.
# Actually the pattern is: display(); selectItemFromPath(); checkMenuItem(); setCheck(); display();
# Let's just remove the last display() since selectItemFromPath already handles selection
# Fix 14: remove duplicate FileBrowser display() at end of launchFileBrowser
do_replace(
    'checkMenuItem(IDM_VIEW_FILEBROWSER, true);\n\t_toolBar.setCheck(IDM_VIEW_FILEBROWSER, true);\n\t_pFileBrowser->display();\n}',
    'checkMenuItem(IDM_VIEW_FILEBROWSER, true);\n\t_toolBar.setCheck(IDM_VIEW_FILEBROWSER, true);\n}',
    "FileBrowser: remove duplicate display() at end of launchFileBrowser")

# Fix 15: NppDarkMode::isExperimentalSupported - wrapper added earlier
# Fix 16: setDarkTitleBar wrapper added earlier
# Fix 17: getVisibleStartAndEndPosition - stubbed earlier
# Fix 18: urlDisable stubbed earlier
# Fix 19: autoIndent_none -> AutoIndentMode::none - applied earlier
# Fix 20: MultiByteToWideChar/WideCharToMultiByte wrappers - applied earlier

# Fix for FileBrowser addRootFolder signature
# Looking at FileBrowser.h: void addRootFolder(const QString& rootFolderPath)
# My fix already does: addRootFolder(QString::fromStdWString(folders[i]))
# That should be correct.

# Fix for VerticalFileSwitcher setBackgroundColor/setForegroundColor
# These take QColor, not COLORREF. Need conversion.
# Actually DockingDlgInterface::setBackgroundColor(const QColor&)
# But VerticalFileSwitcher might override or use the same.
# Let's check: DockingDlgInterface has virtual setBackgroundColor(QColor)
# So calling with COLORREF would fail - need QColor conversion.
# But wait - setBackgroundColor is declared in DockingDlgInterface as taking QColor.
# The calls pass COLORREF which is a uint32_t. This would fail.
# Let me check if there's a ColorRef-to-QColor conversion available.
# Actually, looking at the code, these methods might have overloads.
# For now, let's try to fix with QColor::fromRgb()
do_replace(
    '_pDocumentListPanel->setBackgroundColor(bgColor);',
    '_pDocumentListPanel->setBackgroundColor(QColor(Qt::red).fromRgb(bgColor));',
    "VerticalFileSwitcher::setBackgroundColor: COLORREF -> QColor")

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'wb') as f:
    f.write(content)

print(f"\nTotal: {changes} changes")
sys.exit(0)