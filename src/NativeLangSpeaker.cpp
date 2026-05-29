#include "NativeLangSpeaker.h"
#include <QMenu>

void NativeLangSpeaker::init(NppXmlDoc* nativeLangDocRoot, bool loadIfEnglish) { _nativeLang = nativeLangDocRoot; Q_UNUSED(loadIfEnglish); }
void NativeLangSpeaker::changeConfigLang(QWidget* hDlg) const { Q_UNUSED(hDlg); }
void NativeLangSpeaker::changeLangTabContextMenu(QMenu* hCM) const { Q_UNUSED(hCM); }
void NativeLangSpeaker::getAlternativeNameFromTabContextMenu(QString& output, int cmdID, bool isAlternative, const QString& defaultValue) const { Q_UNUSED(cmdID); Q_UNUSED(isAlternative); output = defaultValue; }
bool NativeLangSpeaker::changeDlgLang(QWidget* hDlg, const QString& dlgTagName, QString* title) { Q_UNUSED(hDlg); Q_UNUSED(dlgTagName); if (title) title->clear(); return false; }
void NativeLangSpeaker::changeLangTabDropContextMenu(QMenu* hCM) const { Q_UNUSED(hCM); }
void NativeLangSpeaker::changeLangTrayIconContextMenu(QMenu* hCM) const { Q_UNUSED(hCM); }
QString NativeLangSpeaker::getSubMenuEntryName(const QString& nodeName) const { return nodeName; }
QString NativeLangSpeaker::getNativeLangMenuString(int itemID, const QString& inCaseOfFailureStr, bool removeMarkAnd) const { Q_UNUSED(itemID); Q_UNUSED(removeMarkAnd); return inCaseOfFailureStr; }
QString NativeLangSpeaker::getShortcutNameString(int itemID) const { Q_UNUSED(itemID); return QString(); }
void NativeLangSpeaker::changeMenuLang(QMenu* menuHandle) const { Q_UNUSED(menuHandle); }
void NativeLangSpeaker::changeUserDefineLang(void* userDefineDlg) const { Q_UNUSED(userDefineDlg); }
void NativeLangSpeaker::changeFindReplaceDlgLang(void* findReplaceDlg) { Q_UNUSED(findReplaceDlg); }
void NativeLangSpeaker::changePreferenceDlgLang(void* preference) const { Q_UNUSED(preference); }
void NativeLangSpeaker::changePluginsAdminDlgLang(void* pluginsAdminDlg) { Q_UNUSED(pluginsAdminDlg); }
bool NativeLangSpeaker::getDoSaveOrNotStrings(QString& title, QString& msg) const { title.clear(); msg.clear(); return false; }
QString NativeLangSpeaker::getAttrNameByIdStr(const QString& defaultStr, void* targetNode, const QString& nodeL1Value, const QString& nodeL1Name, const QString& nodeL2Name)
{ Q_UNUSED(targetNode); Q_UNUSED(nodeL1Value); Q_UNUSED(nodeL1Name); Q_UNUSED(nodeL2Name); return defaultStr; }
