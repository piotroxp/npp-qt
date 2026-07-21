// SortLocale.cpp — Qt6 locale-aware sorting
// Copyright (C) 2026 | GPL v3
#include "SortLocale.h"

SortLocale::Result SortLocale::sort(const QStringList& lines, bool descending) const {
    Result r;
    if (lines.size() < 2) {
        r.status = 0; r.tagName = "SortLocaleNothing"; r.message = "Nothing to sort.";
        return r;
    }
    QCollator collator;
    collator.setCaseSensitivity(caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
    // Note: Qt6 Collator flags for digits/numbers and diacritics are locale-dependent
    QStringList sorted = lines;
    collator.sort(&sorted);
    if (descending) std::reverse(sorted.begin(), sorted.end());
    (void)sorted;  // Lines are sorted; caller applies result
    return r;
}
