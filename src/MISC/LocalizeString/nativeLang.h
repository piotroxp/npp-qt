// MISC/LocalizeString/nativeLang.h - Qt6 port of native language/localization
#pragma once

#include <QString>
#include <QMap>
#include <QTranslator>
#include <QVector>

class NativeLang
{
public:
    static NativeLang& getInstance();
    
    // Load language file
    bool loadLanguage(const QString& langFilePath);
    
    // Get translated string
    QString getNativeStr(const QString& key) const;
    QString getNativeMenuName(const QString& menuName, const QString& subMenuName = QString()) const;
    
    // Language info
    QString getLanguageName() const { return _languageName; }
    QString getLanguageFile() const { return _currentLangFile; }
    bool isLoaded() const { return _isLoaded; }
    
    // Available languages
    QStringList getAvailableLanguages() const;
    
    // Static constants for common UI strings
    static const QString strFile;
    static const QString strEdit;
    static const QString strSearch;
    static const QString strView;
    static const QString strEncoding;
    static const QString strTools;
    static const QString strRun;
    
private:
    NativeLang();
    ~NativeLang() = default;
    
    NativeLang(const NativeLang&) = delete;
    NativeLang& operator=(const NativeLang&) = delete;
    
    bool _isLoaded;
    QString _languageName;
    QString _currentLangFile;
    QMap<QString, QString> _stringTable;
    QVector<QTranslator*> _translators;
    
    void clearStringTable();
    void parseLanguageFile(const QString& filePath);
};

// Utility class for localized strings
class LocalizedString
{
public:
    LocalizedString() : _key(), _translation() {}
    LocalizedString(const QString& key) : _key(key), _translation(NativeLang::getInstance().getNativeStr(key)) {}
    
    void setKey(const QString& key) {
        _key = key;
        _translation = NativeLang::getInstance().getNativeStr(key);
    }
    
    QString get() const { return _translation; }
    QString toString() const { return _translation; }
    const char* toUtf8() const { return _translation.toUtf8().constData(); }
    
    operator QString() const { return _translation; }
    
private:
    QString _key;
    QString _translation;
};