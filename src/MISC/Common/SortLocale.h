// MISC/Common/SortLocale.h - Qt6 port of Notepad++ locale-based sorting
#pragma once

#include <QString>
#include <QCollator>

class ScintillaEditView; // Forward declaration

class SortLocale
{
public:
    struct Result
    {
        int status = 0;
        QString tagName;
        QString message;
    };
    
    QString localeName;
    bool caseSensitive = false;
    bool digitsAsNumbers = true;
    bool ignoreDiacritics = false;
    bool ignoreSymbols = false;
    
    Result sort(ScintillaEditView* sci, bool descending) const;
};