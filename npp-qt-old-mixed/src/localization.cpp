// Semantic Lift: localization — Win32 NativeLangSpeaker + XML → Qt6 QTranslator + .qm
// Original: PowerEditor/src/localization.cpp (1543 lines)
// Win32 → Qt6: NativeLangSpeaker + .xml → QTranslator + QLocale

#include "localization.h"
#include "Parameters.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QJsonObject>
#include <QJsonDocument>

static QString g_currentLanguage;
static QString g_currentLangPath;
static QMap<QString, QString> g_stringMap;
static QTranslator g_translator;
static QTranslator g_notepadTranslator;

NativeLangSpeaker::NativeLangSpeaker()
{
}

NativeLangSpeaker::~NativeLangSpeaker()
{
}

void NativeLangSpeaker::init()
{
    // Load default (English) strings
    loadDefaultStrings();
}

void NativeLangSpeaker::loadDefaultStrings()
{
    g_stringMap.clear();

    // Default English strings — these match localizationString.h constants
    // The actual translations come from .qm files
#include "localizationString.h"
}

bool NativeLangSpeaker::setLanguageFromXml(const QString& xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader reader(&file);
    g_stringMap.clear();

    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            if (reader.name() == QStringLiteral("Localizations")) {
                // Notepad++ XML format — parse translations
                for (auto& attr : reader.attributes()) {
                    if (attr.name().toString() == QStringLiteral("languageName")) {
                        g_currentLanguage = attr.value().toString();
                    }
                }
            } else if (reader.name() == QStringLiteral("Item")) {
                QString key = reader.attributes().value(QStringLiteral("id")).toString();
                reader.readNext();
                if (reader.tokenType() == QXmlStreamReader::Characters) {
                    g_stringMap[key] = reader.text().toString();
                }
            }
        }
    }

    file.close();
    g_currentLangPath = xmlPath;
    return true;
}

bool NativeLangSpeaker::setLanguage(const QString& langName)
{
    // Try to load from .qm file
    QStringList searchPaths = {
        Parameters::getInstance().getPluginConfigsPath() + QStringLiteral("/.."),
        QStringLiteral(":/translations"),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath),
    };

    for (const QString& basePath : searchPaths) {
        QString qmPath = basePath + QStringLiteral("/npp_") + langName + QStringLiteral(".qm");
        if (QFile::exists(qmPath)) {
            if (g_translator.load(qmPath)) {
                qApp->installTranslator(&g_translator);
                g_currentLanguage = langName;
                g_currentLangPath = qmPath;
                return true;
            }
        }

        // Also try bare language name
        qmPath = basePath + QStringLiteral("/") + langName + QStringLiteral(".qm");
        if (QFile::exists(qmPath)) {
            if (g_translator.load(qmPath)) {
                qApp->installTranslator(&g_translator);
                g_currentLanguage = langName;
                g_currentLangPath = qmPath;
                return true;
            }
        }
    }

    return false;
}

QString NativeLangSpeaker::getCurrentLanguageName() const
{
    return g_currentLanguage;
}

QString NativeLangSpeaker::getCurrentLangPath() const
{
    return g_currentLangPath;
}

QString NativeLangSpeaker::getLocalizedStringFromID(const QString& id, const QString& defaultVal) const
{
    return g_stringMap.value(id, defaultVal);
}

QString NativeLangSpeaker::getLocalizedStringFromID(int id, const QString& defaultVal) const
{
    return g_stringMap.value(QString::number(id), defaultVal);
}

QString NativeLangSpeaker::getLocalizedStringFromID(const char* id, const QString& defaultVal) const
{
    return g_stringMap.value(QString::fromLatin1(id), defaultVal);
}

QString NativeLangSpeaker::getLocalizedStringFromID(int id, const char* defaultVal) const
{
    return g_stringMap.value(QString::number(id), QString::fromLatin1(defaultVal));
}

QString NativeLangSpeaker::getLanguageNameFromFileName(const QString& fileName) const
{
    // Extract language name from filename like "npp_chinese_simplified.qm"
    QString name = fileName;
    name.remove(QStringLiteral("npp_"));
    name.remove(QStringLiteral(".qm"));
    name.replace(QStringLiteral("_"), QStringLiteral(" "));
    return name;
}

void NativeLangSpeaker::updateEachMenuEntryText(QMenu* menu, const QString& idPrefix)
{
    if (!menu)
        return;

    const QList<QAction*> actions = menu->actions();
    for (QAction* action : actions) {
        QString id = idPrefix + QStringLiteral("_") + action->data().toString();
        QString text = getLocalizedStringFromID(id, QString());
        if (!text.isEmpty()) {
            action->setText(text);
        }

        // Recurse into submenus
        if (action->menu()) {
            updateEachMenuEntryText(action->menu(), idPrefix);
        }
    }
}

void NativeLangSpeaker::resetLanguages()
{
    qApp->removeTranslator(&g_translator);
    qApp->removeTranslator(&g_notepadTranslator);
    g_stringMap.clear();
    g_currentLanguage.clear();
    g_currentLangPath.clear();
}

QString NativeLangSpeaker::getTranslationFileName() const
{
    if (g_currentLangPath.isEmpty())
        return QString();
    QFileInfo fi(g_currentLangPath);
    return fi.fileName();
}
