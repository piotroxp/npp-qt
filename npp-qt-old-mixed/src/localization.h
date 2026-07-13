#pragma once

#include <QString>
#include <QMap>
#include <QTranslator>
#include <QMenu>

class NativeLangSpeaker
{
public:
    NativeLangSpeaker();
    ~NativeLangSpeaker();

    void init();
    bool setLanguage(const QString& langName);
    bool setLanguageFromXml(const QString& xmlPath);
    QString getCurrentLanguageName() const;
    QString getCurrentLangPath() const;
    QString getTranslationFileName() const;

    // Get localized string by id (int or string)
    QString getLocalizedStringFromID(const QString& id, const QString& defaultVal = QString()) const;
    QString getLocalizedStringFromID(int id, const QString& defaultVal = QString()) const;
    QString getLocalizedStringFromID(const char* id, const QString& defaultVal = QString()) const;
    QString getLocalizedStringFromID(int id, const char* defaultVal) const;

    void updateEachMenuEntryText(QMenu* menu, const QString& idPrefix);
    void resetLanguages();
    QString getLanguageNameFromFileName(const QString& fileName) const;

private:
    void loadDefaultStrings();

    static QTranslator g_translator;
    static QTranslator g_notepadTranslator;
    static QString g_currentLanguage;
    static QString g_currentLangPath;
    static QMap<QString, QString> g_stringMap;
};

