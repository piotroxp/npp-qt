// MISC/LocalizeString/LocalizedStr.h - Qt6 port of localized string support
#pragma once
#include <QTranslator>
#include <QCoreApplication>

#include <QString>
#include <QMap>
#include <QVector>

class LocalizedDialog
{
public:
    LocalizedDialog();
    virtual ~LocalizedDialog();
    
    // Translation management
    void addTranslator(QTranslator* translator);
    void removeTranslator(QTranslator* translator);
    void clearTranslators();
    
    // Retranslate all widgets in the dialog
    virtual void retranslate() = 0;
    
protected:
    QVector<QTranslator*> _translators;
    
    void updateTranslatorConnections();
};

// Simple localization helper class
class LocalizedStr
{
public:
    LocalizedStr() : _text() {}
    LocalizedStr(const QString& text) : _text(text) {}
    
    void set(const QString& text) { _text = text; }
    QString get() const { return _text; }
    
    // For use with Qt's automatic translation
    QString operator()() const { return _text; }
    operator QString() const { return _text; }
    
private:
    QString _text;
};

// Translation context helper
class TranslateContext
{
public:
    TranslateContext(const QString& context) : _context(context) {}
    
    QString operator()(const QString& source) const {
        return QCoreApplication::translate(_context.toUtf8().constData(), 
                                           source.toUtf8().constData());
    }
    
private:
    QString _context;
};