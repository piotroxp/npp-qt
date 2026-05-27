// LocalizationSwitcher.h - Qt6 stub
#pragma once
#include <QObject>
#include <QString>
#include <QVector>

class LocalizationSwitcher : public QObject {
    Q_OBJECT
public:
    LocalizationSwitcher(QObject* parent = nullptr) : QObject(parent) {}
    void setLanguage(const QString& lang) { Q_UNUSED(lang); }
    QString currentLanguage() const { return QString(); }
    QVector<QString> availableLanguages() const { return {}; }
};