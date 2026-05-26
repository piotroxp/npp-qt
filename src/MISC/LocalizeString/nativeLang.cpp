// MISC/LocalizeString/nativeLang.cpp - Qt6 port of native language/localization
#include "nativeLang.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QXmlStreamReader>

const QString NativeLang::strFile = "File";
const QString NativeLang::strEdit = "Edit";
const QString NativeLang::strSearch = "Search";
const QString NativeLang::strView = "View";
const QString NativeLang::strEncoding = "Encoding";
const QString NativeLang::strTools = "Tools";
const QString NativeLang::strRun = "Run";

NativeLang::NativeLang()
    : _isLoaded(false)
    , _languageName("English")
    , _currentLangFile()
{
}

NativeLang& NativeLang::getInstance()
{
    static NativeLang instance;
    return instance;
}

bool NativeLang::loadLanguage(const QString& langFilePath)
{
    clearStringTable();
    
    QFile file(langFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    
    parseLanguageFile(langFilePath);
    
    _currentLangFile = langFilePath;
    _isLoaded = true;
    
    file.close();
    return true;
}

void NativeLang::parseLanguageFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        
        if (xml.isStartElement()) {
            if (xml.name() == "Item") {
                QString key = xml.attributes().value("name").toString();
                xml.readNext();
                if (xml.isCharacters() && !xml.text().isNull()) {
                    _stringTable[key] = xml.text().toString();
                }
            }
        }
    }
    
    file.close();
}

void NativeLang::clearStringTable()
{
    _stringTable.clear();
}

QString NativeLang::getNativeStr(const QString& key) const
{
    auto it = _stringTable.find(key);
    if (it != _stringTable.end())
        return it.value();
    return key; // Return key itself if not found
}

QString NativeLang::getNativeMenuName(const QString& menuName, const QString& subMenuName) const
{
    QString key = menuName;
    if (!subMenuName.isEmpty())
        key += "/" + subMenuName;
    
    auto it = _stringTable.find(key);
    if (it != _stringTable.end())
        return it.value();
    
    // Try just the menu name
    it = _stringTable.find(menuName);
    if (it != _stringTable.end())
        return it.value();
    
    return menuName;
}

QStringList NativeLang::getAvailableLanguages() const
{
    QStringList languages;
    
    // Look for .xml language files in standard locations
    QStringList searchPaths = {
        "/usr/share/notepad++/nativeLang/",
        QDir::homePath() + "/.notepad++/nativeLang/",
        ":/i18n/"
    };
    
    for (const QString& path : searchPaths) {
        QDir dir(path);
        if (dir.exists()) {
            QStringList files = dir.entryList(QStringList() << "*.xml");
            for (const QString& file : files) {
                QString langName = QFileInfo(file).baseName();
                if (!languages.contains(langName))
                    languages.append(langName);
            }
        }
    }
    
    return languages;
}