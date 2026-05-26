// MISC/LocalizeString/LocalizedDialog.cpp - Qt6 port of localized dialog support
#include "LocalizedStr.h"
#include <QTranslator>
#include <QCoreApplication>

LocalizedDialog::LocalizedDialog()
{
}

LocalizedDialog::~LocalizedDialog()
{
    clearTranslators();
}

void LocalizedDialog::addTranslator(QTranslator* translator)
{
    if (translator && !_translators.contains(translator)) {
        _translators.append(translator);
        QCoreApplication::installTranslator(translator);
    }
}

void LocalizedDialog::removeTranslator(QTranslator* translator)
{
    if (translator) {
        int index = _translators.indexOf(translator);
        if (index >= 0) {
            QCoreApplication::removeTranslator(translator);
            _translators.remove(index);
        }
    }
}

void LocalizedDialog::clearTranslators()
{
    for (QTranslator* translator : _translators) {
        QCoreApplication::removeTranslator(translator);
        delete translator;
    }
    _translators.clear();
}

void LocalizedDialog::updateTranslatorConnections()
{
    // This would connect to language change signals for runtime retranslation
}