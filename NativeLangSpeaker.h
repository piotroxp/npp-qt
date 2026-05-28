// NativeLangSpeaker.h - Qt6 port stub
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "NppXml.h"

// Use NppXmlDoc directly - it's defined in NppXml.h
class NativeLangSpeaker : public QObject {
    Q_OBJECT
public:
    NativeLangSpeaker(QObject* parent = nullptr) : QObject(parent) {}
    ~NativeLangSpeaker() = default;
    
    void init(NppXmlDoc nativeLangDocRoot, bool loadIfEnglish = false);
    void changeConfigLang(QWidget* hDlg) const;
    void changeLangTabContextMenu(QMenu* hCM) const;
    void getAlternativeNameFromTabContextMenu(QString& output, int cmdID, bool isAlternative, const QString& defaultValue) const;
    
    bool changeDlgLang(QWidget* hDlg, const QString& dlgTagName, QString* title = nullptr);
    void changeLangTabDropContextMenu(QMenu* hCM) const;
    void changeLangTrayIconContextMenu(QMenu* hCM) const;
    QString getSubMenuEntryName(const QString& nodeName) const;
    QString getNativeLangMenuString(int itemID, const QString& inCaseOfFailureStr = QString(), bool removeMarkAnd = false) const;
    QString getShortcutNameString(int itemID) const;
    void changeMenuLang(QMenu* menuHandle) const;
    void changeUserDefineLang(void* userDefineDlg) const;
    void changeFindReplaceDlgLang(void* findReplaceDlg);
    void changePreferenceDlgLang(void* preference) const;
    void changePluginsAdminDlgLang(void* pluginsAdminDlg);
    
    bool getDoSaveOrNotStrings(QString& title, QString& msg) const;
    bool isRTL() const { return _isRTL; }
    bool isEditZoneRTL() const { return _isEditZoneRTL; }
    QString getFileName() const { return _fileName; }
    
    static QString getAttrNameByIdStr(const QString& defaultStr, void* targetNode, 
                                      const QString& nodeL1Value, const QString& nodeL1Name = "id", 
                                      const QString& nodeL2Name = "name");
    
private:
    XmlNode _nativeLang;
    bool _isRTL = false;
    bool _isEditZoneRTL = false;
    QString _fileName;
    QMap<QString, QString> _shortcutMenuEntryNameMap;
};