// npp-qt: Sorting utilities for list/table sorting
#pragma once
#include <QString>
#include <QStringList>
#include <QCollator>

namespace Sorters {
    // Sort order
    enum SortOrder { Ascending, Descending };
    // Sort mode
    enum SortMode { Lexical, Numeric, LexicalNoCase };

    // Sort a QStringList
    void sort(QStringList& list, SortOrder order = Ascending,
              SortMode mode = Lexical, bool useLocale = false);

    // Case-insensitive lexical comparison
    bool lessThanNoCase(const QString& a, const QString& b);

    // Natural sort (e.g., "file2" < "file10")
    bool naturalLessThan(const QString& a, const QString& b);

    // Numeric comparison
    bool numericLessThan(const QString& a, const QString& b);

    // Locale-aware collation
    class LocaleCollator {
    public:
        explicit LocaleCollator(const QString& locale = QString());
        bool lessThan(const QString& a, const QString& b) const;
    private:
        QCollator _collator;
    };
}
