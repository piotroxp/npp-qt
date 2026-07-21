// Sorters.h — Platform-neutral sorting algorithm headers
// Qt6 port stub: algorithms implemented in core/NppCommands.cpp sorting functions
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QString>
#include <QStringList>
#include <QCollator>

// Sorting modes matching N++ IDM_SORT_*
enum class SortMode {
    Ascending         = 0,
    Descending        = 1,
    AscendingNumeric  = 2,
    DescendingNumeric = 3,
    Reverse           = 4,
    Random            = 5,
    AscendingCaseSensitive    = 6,
    DescendingCaseSensitive   = 7,
    AscendingLocaleAware      = 8,
    DescendingLocaleAware     = 9,
};

/// Sort a list of strings according to the given mode
QStringList sortStrings(const QStringList& input, SortMode mode, QCollator collator = {});

/// Sort lines in a document (called from NppCommands)
/// Returns true on success
bool sortDocumentLines(void* scintillaEditView, SortMode mode, bool descending,
                       bool caseSensitive, bool digitAsNumbers,
                       bool ignoreDiacritics, bool ignoreSymbols);
