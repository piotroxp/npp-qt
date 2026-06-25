// Ported to Qt6
#pragma once
#include <QMap>
#include <QString>
#include <QTranslator>

class FindReplaceDlg;
class PreferenceDlg;
class ShortcutMapper;
class UserDefineDialog;
class PluginsAdminDlg;

struct MenuPosition {
    int _x = -1;
    int _y = -1;
    int _z = -1;
    const char* _id = nullptr;
    static const MenuPosition& getMenuPosition(const char* id);
};

class NativeLangSpeaker : public QObject {
    Q_OBJECT
public:
    NativeLangSpeaker(QObject* parent = nullptr) : QObject(parent) {}
    void init(const QString& languageFilePath, bool loadIfEnglish = false);
    void changeConfigLang(QWidget* dlg) const;
    void changeLangTabContextMenu(QMenu* hCM) const;
    void getAlternativeNameFromTabContextMenu(QString& output, int cmdID, bool isAlternative,
                                         const QString& defaultValue) const;
    bool changeDlgLang(QWidget* dlg, const QString& dlgTagName, QString* title = nullptr) const;
    void changeLangTabDropContextMenu(QMenu* hCM) const;
    void changeLangTrayIconContexMenu(QMenu* hCM) const;
    QString getSubMenuEntryName(const char* nodeName) const;
    QString getNativeLangMenuString(int itemID, const QString& inCaseOfFailureStr = QString(),
                                bool removeMarkAnd = false) const;
    QString getShortcutNameString(int itemID) const;
    void changeMenuLang(QMenu* menuHandle) const;
    static void changeStyleCtrlsLang(QWidget* hDlg, int* idArray, const char** translatedText);
    void changeUserDefineLang(UserDefineDialog* userDefineDlg) const;
    void changeUserDefineLangPopupDlg(QWidget* hDlg) const;
    void changeFindReplaceDlgLang(FindReplaceDlg& findReplaceDlg);
    void changePreferenceDlgLang(PreferenceDlg& preference) const;
    void changePluginsAdminDlgLang(PluginsAdminDlg& pluginsAdminDlg);
    bool getDoSaveOrNotStrings(QString& title, QString& msg) const;
    bool isRTL() const { return _isRTL; }
    bool isEditZoneRTL() const { return _isEditZoneRTL; }
    static int getLangEncoding() { return 1066; }
    bool getMsgBoxLang(const char* msgBoxTagName, QWidget* hWnd, QString& title, QString& message) const;
    QString getShortcutMapperLangStr(const char* nodeName, const QString& defaultStr) const;
    QString getProjectPanelLangMenuStr(const char* nodeName, int cmdID, const QString& defaultStr) const;
    QString getDlgLangMenuStr(const char* firstLevelNodeName, const char* secondLevelNodeName,
                             int cmdID, const QString& defaultStr) const;
    QString getCmdLangStr(const QVector<const char*>& nodeNames, int cmdID, const QString& defaultStr) const;
    QString getAttrNameStr(const QString& defaultStr, const char* nodeL1Name,
                         const char* nodeL2Name, const char* nodeL3Name = "name") const;
    static QString getAttrNameByIdStr(const QString& defaultStr, const QString& targetNode,
                                    const char* nodeL1Value, const char* nodeL1Name = "id",
                                    const char* nodeL2Name = "name");
    QString getLocalizedStrFromID(const char* strID, const QString& defaultString) const;
    void getMainMenuEntryName(QString& dest, QMenu* hMenu, const char* menuId,
                            const QString& defaultDest);
    void resetShortcutMenuNameMap() { _shortcutMenuEntryNameMap.clear(); }
    int messageBox(const char* msgBoxTagName, QWidget* parent, const QString& defaultMessage,
                  const QString& defaultTitle, int msgBoxType, int intInfo = 0,
                  const QString& strInfo = QString()) const;
private:
    static constexpr int _nativeLangEncoding = 1066;
    QString _languageFilePath;
    bool _isRTL = false;
    bool _isEditZoneRTL = false;
    QString _fileName;
    QMap<QString, QString> _shortcutMenuEntryNameMap;
    QTranslator _translator;
};
