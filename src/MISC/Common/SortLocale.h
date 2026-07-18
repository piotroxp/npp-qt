// SortLocale.h — Qt6 locale-aware sorting
// Copyright (C) 2026 | GPL v3
#pragma once
#include <QString>
#include <QCollator>
#include <QStringList>

class SortLocale {
public:
    SortLocale() = default;
    struct Result { int status = 0; QString tagName; QString message; };
    bool caseSensitive = false;
    bool digitsAsNumbers = true;
    bool ignoreDiacritics = false;
    bool ignoreSymbols = false;
    Result sort(const QStringList& lines, bool descending) const;
};
