// MISC/Encoding/EncodingConfiguration.h - Qt6 port of encoding configuration
#pragma once

#include <QString>
#include <QSettings>
#include <QMap>

class EncodingConfiguration
{
public:
    static EncodingConfiguration& getInstance();
    
    // Default encoding
    void setDefaultEncoding(int codepage);
    int getDefaultEncoding() const;
    
    // Encoding preferences
    void setEncodingPreference(int codepage, const QString& description);
    QMap<int, QString> getEncodingPreferences() const;
    
    // Save/Load settings
    void saveSettings();
    void loadSettings();
    
    // Encoding detection settings
    void setAutoDetectEncoding(bool enable);
    bool isAutoDetectEncoding() const;
    
    // Save with encoding settings
    void setSaveWithBOM(bool enable);
    bool isSaveWithBOM() const;
    
private:
    EncodingConfiguration();
    ~EncodingConfiguration() = default;
    
    EncodingConfiguration(const EncodingConfiguration&) = delete;
    EncodingConfiguration& operator=(const EncodingConfiguration&) = delete;
    
    int _defaultEncoding;
    QMap<int, QString> _preferences;
    bool _autoDetect;
    bool _saveWithBOM;
    
    static const QString settingsGroup;
    static const QString defaultEncodingKey;
    static const QString autoDetectKey;
    static const QString saveWithBOMKey;
};