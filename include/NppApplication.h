// NppApplication.h - Qt6 port
#pragma once

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include <QLibraryInfo>
#include <QStandardPaths>

class NppApplication : public QApplication {
    Q_OBJECT

public:
    NppApplication(int& argc, char* argv[])
        : QApplication(argc, argv)
    {
        // Load translation files
        loadTranslations();
    }

    ~NppApplication() = default;

    // Settings management
    QSettings* settings() { return &_settings; }
    const QSettings* settings() const { return &_settings; }

    void loadSettings() {
        QString configPath = QDir(QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)).filePath("notepad++.ini");
        _settings.setPath(QSettings::IniFormat, QSettings::UserScope, configPath);
    }

    QString getSettingsPath(const QString& fileName) const {
        return QDir(QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation)).filePath(fileName);
    }

private:
    void loadTranslations() {
        QString locale = QLocale::system().name();
        
        // Try to load Qt translator
        QString qtTransPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        _qtTranslator.load(QString("qt_%1").arg(locale), qtTransPath);
        installTranslator(&_qtTranslator);

        // Try to load npp translator
        QString nppTransPath = QStandardPaths::locate(
            QStandardPaths::AppDataLocation, 
            QString("i18n/npp-%1.qm").arg(locale));
        if (!nppTransPath.isEmpty()) {
            _nppTranslator.load(nppTransPath);
            installTranslator(&_nppTranslator);
        }
    }

    QSettings _settings;
    QTranslator _qtTranslator;
    QTranslator _nppTranslator;
};