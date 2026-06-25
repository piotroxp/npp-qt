// npp-qt: Locale-aware sorting utilities
#pragma once
#include <QString>
#include <QCollator>

// SortLocale: provides locale-aware string comparison
class SortLocale {
public:
    explicit SortLocale(const QString& localeName = QString());
    ~SortLocale() = default;

    // Compare two strings using locale rules
    int compare(const QString& a, const QString& b) const;

    // Check if a < b using locale rules
    bool lessThan(const QString& a, const QString& b) const;

    // Set sort options
    void setIgnoreCase(bool ignoreCase);
    void setIgnorePunctuation(bool ignorePunctuation);
    void setNumericMode(bool numeric);
    void setSortUpperFirst(bool upperFirst);

private:
    QCollator _collator;
};
