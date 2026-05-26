// MISC/Encoding/EncodingConfiguration.cpp - Qt6 port of encoding configuration
#include "EncodingConfiguration.h"

const QString EncodingConfiguration::settingsGroup = "Encoding";
const QString EncodingConfiguration::defaultEncodingKey = "DefaultEncoding";
const QString EncodingConfiguration::autoDetectKey = "AutoDetectEncoding";
const QString EncodingConfiguration::saveWithBOMKey = "SaveWithBOM";

EncodingConfiguration::EncodingConfiguration()
    : _defaultEncoding(65001)  // UTF-8
    , _autoDetect(true)
    , _saveWithBOM(false)
{
    loadSettings();
}

EncodingConfiguration& EncodingConfiguration::getInstance()
{
    static EncodingConfiguration instance;
    return instance;
}

void EncodingConfiguration::setDefaultEncoding(int codepage)
{
    _defaultEncoding = codepage;
}

int EncodingConfiguration::getDefaultEncoding() const
{
    return _defaultEncoding;
}

void EncodingConfiguration::setEncodingPreference(int codepage, const QString& description)
{
    _preferences[codepage] = description;
}

QMap<int, QString> EncodingConfiguration::getEncodingPreferences() const
{
    return _preferences;
}

void EncodingConfiguration::saveSettings()
{
    QSettings settings;
    settings.beginGroup(settingsGroup);
    settings.setValue(defaultEncodingKey, _defaultEncoding);
    settings.setValue(autoDetectKey, _autoDetect);
    settings.setValue(saveWithBOMKey, _saveWithBOM);
    settings.endGroup();
}

void EncodingConfiguration::loadSettings()
{
    QSettings settings;
    settings.beginGroup(settingsGroup);
    _defaultEncoding = settings.value(defaultEncodingKey, 65001).toInt();
    _autoDetect = settings.value(autoDetectKey, true).toBool();
    _saveWithBOM = settings.value(saveWithBOMKey, false).toBool();
    settings.endGroup();
}

void EncodingConfiguration::setAutoDetectEncoding(bool enable)
{
    _autoDetect = enable;
}

bool EncodingConfiguration::isAutoDetectEncoding() const
{
    return _autoDetect;
}

void EncodingConfiguration::setSaveWithBOM(bool enable)
{
    _saveWithBOM = enable;
}

bool EncodingConfiguration::isSaveWithBOM() const
{
    return _saveWithBOM;
}