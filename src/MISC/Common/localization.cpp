// Ported to Qt6
#include "localization.h"
#include <QAction>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QTextStream>
#include <QXmlStreamReader>
#include "Common.h"
#include "menuCmdID.h"

static const MenuPosition g_menuFolderPositions[] = {
    { 0,  -1, -1, "file" },
    { 1,  -1, -1, "edit" },
    { 2,  -1, -1, "search" },
    { 3,  -1, -1, "view" },
    { 4,  -1, -1, "encoding" },
    { 5,  -1, -1, "language" },
    { 6,  -1, -1, "settings" },
    { 7,  -1, -1, "tools" },
    { 8,  -1, -1, "macro" },
    { 9,  -1, -1, "run" },
    { 10, -1, -1, "Plugins" },
    { 11, -1, -1, "Window" },
    { 0,   2, -1, "file-openFolder" },
    { 0,  13, -1, "file-closeMore" },
    { 0,  22, -1, "file-recentFiles" },
    { 1,  11, -1, "edit-insert" },
    { 1,  12, -1, "edit-copyToClipboard" },
    { 1,  13, -1, "edit-indent" },
    { 1,  14, -1, "edit-convertCaseTo" },
    { 1,  15, -1, "edit-lineOperations" },
    { 1,  16, -1, "edit-comment" },
    { 1,  17, -1, "edit-autoCompletion" },
    { 1,  18, -1, "edit-eolConversion" },
    { 1,  19, -1, "edit-blankOperations" },
    { 1,  20, -1, "edit-pasteSpecial" },
    { 1,  21, -1, "edit-onSelection" },
    { 1,  23, -1, "edit-multiSelectALL" },
    { 1,  24, -1, "edit-multiSelectNext" },
    { 1,  33, -1, "edit-readonlyInNotepad++" },
    { 2,  18, -1, "search-changeHistory" },
    { 2,  20, -1, "search-markAll" },
    { 2,  21, -1, "search-markOne" },
    { 2,  22, -1, "search-unmarkAll" },
    { 2,  23, -1, "search-jumpUp" },
    { 2,  24, -1, "search-jumpDown" },
    { 2,  25, -1, "search-copyStyledText" },
    { 2,  27, -1, "search-bookmark" },
    { 3,   5, -1, "view-currentFileIn" },
    { 3,   7, -1, "view-showSymbol" },
    { 3,   8, -1, "view-zoom" },
    { 3,   9, -1, "view-moveCloneDocument" },
    { 3,  10, -1, "view-tab" },
    { 3,  19, -1, "view-collapseLevel" },
    { 3,  20, -1, "view-uncollapseLevel" },
    { 3,  24, -1, "view-project" },
    { 4,   5, -1, "encoding-characterSets" },
    { 4,   5,  0, "encoding-arabic" },
    { 4,   5,  1, "encoding-baltic" },
    { 4,   5,  2, "encoding-celtic" },
    { 4,   5,  3, "encoding-cyrillic" },
    { 4,   5,  4, "encoding-centralEuropean" },
    { 4,   5,  5, "encoding-chinese" },
    { 4,   5,  6, "encoding-easternEuropean" },
    { 4,   5,  7, "encoding-greek" },
    { 4,   5,  8, "encoding-hebrew" },
    { 4,   5,  9, "encoding-japanese" },
    { 4,   5, 10, "encoding-korean" },
    { 4,   5, 11, "encoding-northEuropean" },
    { 4,   5, 12, "encoding-thai" },
    { 4,   5, 13, "encoding-turkish" },
    { 4,   5, 14, "encoding-westernEuropean" },
    { 4,   5, 15, "encoding-vietnamese" },
    { 5,  25, -1, "language-userDefinedLanguage" },
    { 6,   4, -1, "settings-import" },
    { 7,   0, -1, "tools-md5" },
    { 7,   1, -1, "tools-sha1" },
    { 7,   2, -1, "tools-sha256" },
    { 7,   3, -1, "tools-sha512" },
    { 11,  0, -1, "window-sortby"},
    { -1,  -1, -1, "" }
};

const MenuPosition& MenuPosition::getMenuPosition(const char* id)
{
    for (int i = 0; g_menuFolderPositions[i]._x != -1; ++i)
        if (strcmp(g_menuFolderPositions[i]._id, id) == 0) return g_menuFolderPositions[i];
    static const MenuPosition lastPos{-1,-1,-1,""};
    return lastPos;
}

void NativeLangSpeaker::init(const QString& languageFilePath, bool loadIfEnglish)
{
    _languageFilePath = languageFilePath;
    QFileInfo fi(languageFilePath);
    _fileName = fi.fileName();
    if (!loadIfEnglish && _fileName.compare("english.xml", Qt::CaseInsensitive) == 0) {
        _isRTL = false; _isEditZoneRTL = false; return;
    }
    if (!languageFilePath.isEmpty()) {
        _translator.load(QLocale(), "npp", "_", fi.absolutePath());
        qApp->installTranslator(&_translator);
    }
    QFile file(languageFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader xml(&file);
        while (!xml.atEnd()) {
            if (xml.readNext() == QXmlStreamReader::StartElement)
                if (xml.name() == "Native-Langue") {
                    QStringRef rtl = xml.attributes().value("RTL");
                    _isRTL = (rtl == "yes");
                    if (_isRTL) {
                        QStringRef ezrtl = xml.attributes().value("editZoneRTL");
                        _isEditZoneRTL = ezrtl.isNull() ? true : (ezrtl != "no");
                    } else { _isEditZoneRTL = false; }
                    _fileName = xml.attributes().value("filename").toString();
                    break;
                }
        }
        file.close();
    }
}

static QString purifyMenuString(const QString& s)
{
    QString result = s;
    result.replace("&&", "\x01");
    result.remove('&');
    result.replace("\x01", "&");
    if (result.endsWith("...")) result.chop(3);
    return result;
}

static QString xmlAttribute(const QDomElement& elem, const char* attr) {
    return elem.attribute(QString::fromLatin1(attr));
}

static int xmlIntAttribute(const QDomElement& elem, const char* attr, int def=0) {
    bool ok; int v = elem.attribute(QString::fromLatin1(attr)).toInt(&ok); return ok ? v : def;
}

QString NativeLangSpeaker::getSubMenuEntryName(const char* nodeName) const
{
    if (_languageFilePath.isEmpty()) return QString();
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return QString(); }
    file.close();
    QDomElement root = doc.documentElement();
    QDomElement np = root.firstChildElement("NotepadPlus");
    np = np.firstChildElement("Menu").firstChildElement("Main");
    for (QDomElement child = np.firstChildElement("SubEntries").firstChildElement("Item");
         !child.isNull(); child = child.nextSiblingElement("Item")) {
        QString idName = xmlAttribute(child, "subMenuId");
        if (!idName.isEmpty() && idName == QLatin1String(nodeName))
            return string2wstring(xmlAttribute(child, "name"), _nativeLangEncoding);
    }
    return QString();
}

QString NativeLangSpeaker::getNativeLangMenuString(int itemID, const QString& fail, bool removeMark) const
{
    if (_languageFilePath.isEmpty()) return fail;
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return fail;
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return fail; }
    file.close();
    QDomElement root = doc.documentElement();
    QDomElement np = root.firstChildElement("NotepadPlus").firstChildElement("Menu").firstChildElement("Main").firstChildElement("Commands");
    for (QDomElement child = np.firstChildElement("Item"); !child.isNull(); child = child.nextSiblingElement("Item")) {
        if (xmlIntAttribute(child, "id", 0) == itemID) {
            QString name = xmlAttribute(child, "name");
            if (!name.isEmpty())
                return removeMark ? purifyMenuString(name) : string2wstring(name, _nativeLangEncoding);
        }
    }
    return fail;
}

QString NativeLangSpeaker::getShortcutNameString(int itemID) const
{
    if (_languageFilePath.isEmpty()) return QString();
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return QString(); }
    file.close();
    QDomElement root = doc.documentElement();
    QDomElement np = root.firstChildElement("Dialog").firstChildElement("ShortcutMapper").firstChildElement("MainCommandNames");
    for (QDomElement child = np.firstChildElement("Item"); !child.isNull(); child = child.nextSiblingElement("Item"))
        if (xmlIntAttribute(child, "id", 0) == itemID)
            return string2wstring(xmlAttribute(child, "name"), _nativeLangEncoding);
    return QString();
}

void NativeLangSpeaker::getMainMenuEntryName(QString& dest, QMenu* hMenu, const char* menuId, const QString& defaultDest)
{
    auto it = _shortcutMenuEntryNameMap.find(QLatin1String(menuId));
    if (it != _shortcutMenuEntryNameMap.end()) { dest = it.value(); return; }
    const MenuPosition& menuPos = MenuPosition::getMenuPosition(menuId);
    if (menuPos._x != -1 && menuPos._y == -1 && menuPos._z == -1) {
        if (QMenuBar* mb = hMenu->menuBar()) {
            QList<QAction*> actions = mb->actions();
            if (menuPos._x >= 0 && menuPos._x < actions.size()) {
                dest = actions[menuPos._x]->text();
                dest.remove('&');
                _shortcutMenuEntryNameMap[QLatin1String(menuId)] = dest;
                return;
            }
        }
    }
    _shortcutMenuEntryNameMap[QLatin1String(menuId)] = defaultDest;
    dest = defaultDest;
}

void NativeLangSpeaker::changeMenuLang(QMenu*) const {}

void NativeLangSpeaker::changeLangTabContextMenu(QMenu*) const {}

void NativeLangSpeaker::getAlternativeNameFromTabContextMenu(QString& out, int, bool, const QString& defaultVal) const
{
    out = defaultVal;
}

void NativeLangSpeaker::changeLangTabDropContextMenu(QMenu*) const {}
void NativeLangSpeaker::changeLangTrayIconContexMenu(QMenu*) const {}
void NativeLangSpeaker::changeConfigLang(QWidget*) const {}

bool NativeLangSpeaker::changeDlgLang(QWidget* dlg, const QString& dlgTagName, QString* title) const
{
    if (!dlg || _languageFilePath.isEmpty()) return false;
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return false; }
    file.close();
    QDomElement root = doc.documentElement();
    QDomElement dlgNode = root.firstChildElement("Dialog").firstChild().toElement();
    for (QDomElement child = dlgNode.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        if (child.tagName() == dlgTagName) { dlgNode = child; break; }
    if (dlgNode.isNull()) return false;
    QString titleStr = xmlAttribute(dlgNode, "title");
    if (!titleStr.isEmpty()) {
        QString t = string2wstring(titleStr, _nativeLangEncoding);
        dlg->setWindowTitle(t);
        if (title) *title = t;
    }
    for (QDomElement child = dlgNode.firstChildElement("Item"); !child.isNull(); child = child.nextSiblingElement("Item")) {
        int id = xmlIntAttribute(child, "id", -1);
        QString name = xmlAttribute(child, "name");
        if (id >= 0 && !name.isEmpty()) {
            if (QWidget* w = dlg->findChild<QWidget*>(QString::number(id))) {
                if (QLabel* l = qobject_cast<QLabel*>(w)) l->setText(string2wstring(name, _nativeLangEncoding));
                else if (QPushButton* b = qobject_cast<QPushButton*>(w)) b->setText(string2wstring(name, _nativeLangEncoding));
                else if (QCheckBox* c = qobject_cast<QCheckBox*>(w)) c->setText(string2wstring(name, _nativeLangEncoding));
                else if (QRadioButton* r = qobject_cast<QRadioButton*>(w)) r->setText(string2wstring(name, _nativeLangEncoding));
            }
        }
    }
    return true;
}

bool NativeLangSpeaker::getMsgBoxLang(const char* msgBoxTagName, QWidget*, QString& title, QString& message) const
{
    title.clear(); message.clear();
    if (_languageFilePath.isEmpty()) return false;
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return false; }
    file.close();
    QDomElement root = doc.documentElement().firstChildElement("MessageBox").firstChildElement();
    for (QDomElement child = root; !child.isNull(); child = child.nextSiblingElement())
        if (child.tagName() == QString::fromLatin1(msgBoxTagName)) {
            QString t = xmlAttribute(child, "title");
            QString m = xmlAttribute(child, "message");
            if (!t.isEmpty() && !m.isEmpty()) {
                title = string2wstring(t, _nativeLangEncoding);
                message = string2wstring(m, _nativeLangEncoding);
                return true;
            }
        }
    return false;
}

int NativeLangSpeaker::messageBox(const char* msgBoxTagName, QWidget* parent, const QString& defaultMsg,
                                 const QString& defaultTitle, int msgBoxType, int intInfo,
                                 const QString& strInfo) const
{
    QString title, message;
    if (!getMsgBoxLang(msgBoxTagName, parent, title, message)) {
        title = defaultTitle; message = defaultMsg;
    }
    message.replace("$INT_REPLACE$", QString::number(intInfo));
    if (!strInfo.isNull()) {
        title.replace("$STR_REPLACE$", strInfo);
        message.replace("$STR_REPLACE$", strInfo);
    }
    QMessageBox::StandardButtons btns = QMessageBox::NoButton;
    int mb = msgBoxType & 0x0F;
    if (mb == 1) btns = QMessageBox::Ok;
    else if (mb == 3) btns = QMessageBox::Ok | QMessageBox::Cancel;
    else if (mb == 4) btns = QMessageBox::Yes | QMessageBox::No;
    QMessageBox::Icon icon = QMessageBox::NoIcon;
    if (msgBoxType & 0x10) icon = QMessageBox::Critical;
    else if (msgBoxType & 0x20) icon = QMessageBox::Question;
    else if (msgBoxType & 0x30) icon = QMessageBox::Warning;
    else if (msgBoxType & 0x40) icon = QMessageBox::Information;
    QMessageBox mbx(icon, title, message, btns, parent);
    if (_isRTL) mbx.setLayoutDirection(Qt::RightToLeft);
    return mbx.exec();
}

QString NativeLangSpeaker::getLocalizedStrFromID(const char* strID, const QString& defaultString) const
{
    if (_languageFilePath.isEmpty()) return defaultString;
    QFile file(_languageFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return defaultString;
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return defaultString; }
    file.close();
    QDomElement node = doc.documentElement().firstChildElement("MiscStrings").firstChildElement(QString::fromLatin1(strID));
    if (node.isNull()) return defaultString;
    QString v = xmlAttribute(node, "value");
    return v.isEmpty() ? defaultString : v;
}

bool NativeLangSpeaker::getDoSaveOrNotStrings(QString& title, QString& msg) const
{
    return getMsgBoxLang("DoSaveOrNot", nullptr, title, msg);
}

void NativeLangSpeaker::changeStyleCtrlsLang(QWidget*, int*, const char**) {}
void NativeLangSpeaker::changeUserDefineLang(UserDefineDialog*) const {}
void NativeLangSpeaker::changeUserDefineLangPopupDlg(QWidget*) const {}
void NativeLangSpeaker::changeFindReplaceDlgLang(FindReplaceDlg&) {}
void NativeLangSpeaker::changePreferenceDlgLang(PreferenceDlg&) const {}
void NativeLangSpeaker::changePluginsAdminDlgLang(PluginsAdminDlg&) {}
QString NativeLangSpeaker::getShortcutMapperLangStr(const char*, const QString& def) const { return def; }
QString NativeLangSpeaker::getProjectPanelLangMenuStr(const char*, int, const QString& def) const { return def; }
QString NativeLangSpeaker::getDlgLangMenuStr(const char*, const char*, int, const QString& def) const { return def; }
QString NativeLangSpeaker::getCmdLangStr(const QVector<const char*>&, int, const QString& def) const { return def; }
QString NativeLangSpeaker::getAttrNameStr(const QString& def, const char*, const char*, const char*) const { return def; }
QString NativeLangSpeaker::getAttrNameByIdStr(const QString& def, const QString&, const char*, const char*, const char*) { return def; }
