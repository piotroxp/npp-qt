// MISC/Common/Sorters.h - Qt6 port of Notepad++ sorting utilities
#pragma once

#include <QtAlgorithms>
#include <QString>
#include <QStringList>

// Base interface for line sorting.
class ISorter
{
private:
    bool _isDescending = true;
    size_t _fromColumn = 0;
    size_t _toColumn = 0;

protected:
    bool isDescending() const { return _isDescending; }

    QString getSortKey(const QString& input) {
        if (isSortingSpecificColumns()) {
            if (input.length() < static_cast<int>(_fromColumn)) {
                return QString();
            } else if (_fromColumn == _toColumn) {
                return input.mid(static_cast<int>(_fromColumn));
            } else {
                return input.mid(static_cast<int>(_fromColumn), static_cast<int>(_toColumn - _fromColumn));
            }
        } else {
            return input;
        }
    }

    bool isSortingSpecificColumns() { return _toColumn != 0; }

public:
    ISorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : _isDescending(isDescending), _fromColumn(fromColumn), _toColumn(toColumn) {}
    virtual ~ISorter() {}
    virtual void sort(QStringList& lines) = 0;
};

// Implementation of lexicographic sorting of lines.
class LexicographicSorter : public ISorter
{
public:
    LexicographicSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

    void sort(QStringList& lines) override {
        if (isSortingSpecificColumns()) {
            if (isDescending()) {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a) > getSortKey(b);
                });
            } else {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a) < getSortKey(b);
                });
            }
        } else {
            if (isDescending()) {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a > b;
                });
            } else {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a < b;
                });
            }
        }
    }
};

// Implementation of lexicographic sorting of lines, ignoring character casing
class LexicographicCaseInsensitiveSorter : public ISorter
{
public:
    LexicographicCaseInsensitiveSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

    void sort(QStringList& lines) override {
        if (isSortingSpecificColumns()) {
            if (isDescending()) {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a).toLower() > getSortKey(b).toLower();
                });
            } else {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a).toLower() < getSortKey(b).toLower();
                });
            }
        } else {
            if (isDescending()) {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a.toLower() > b.toLower();
                });
            } else {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a.toLower() < b.toLower();
                });
            }
        }
    }
};

// Implementation of length wise sorting of lines.
class LineLengthSorter : public ISorter
{
public:
    LineLengthSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

    void sort(QStringList& lines) override {
        if (isSortingSpecificColumns()) {
            if (isDescending()) {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a).length() > getSortKey(b).length();
                });
            } else {
                std::stable_sort(lines.begin(), lines.end(), [this](const QString& a, const QString& b) {
                    return getSortKey(a).length() < getSortKey(b).length();
                });
            }
        } else {
            if (isDescending()) {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a.length() > b.length();
                });
            } else {
                std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                    return a.length() < b.length();
                });
            }
        }
    }
};

class ReverseSorter : public ISorter
{
public:
    ReverseSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

    void sort(QStringList& lines) override {
        QList<QString> reversed;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            reversed.append(*it);
        }
        lines = reversed;
    }
};

class RandomSorter : public ISorter
{
public:
    unsigned seed;

    RandomSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {
        seed = static_cast<unsigned>(QDateTime::currentMSecsSinceEpoch());
    }

    void sort(QStringList& lines) override {
        std::random_shuffle(lines.begin(), lines.end());
    }
};

// Integer sorter
class IntegerSorter : public ISorter
{
public:
    IntegerSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

    void sort(QStringList& lines) override {
        if (isDescending()) {
            std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                bool ok1, ok2;
                qint64 val1 = a.toLongLong(&ok1);
                qint64 val2 = b.toLongLong(&ok2);
                if (ok1 && ok2) return val1 > val2;
                if (ok1) return true;
                if (ok2) return false;
                return a > b;
            });
        } else {
            std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
                bool ok1, ok2;
                qint64 val1 = a.toLongLong(&ok1);
                qint64 val2 = b.toLongLong(&ok2);
                if (ok1 && ok2) return val1 < val2;
                if (ok1) return false;
                if (ok2) return true;
                return a < b;
            });
        }
    }
};

// Decimal comma sorter (for European number formats)
class DecimalCommaSorter : public ISorter
{
public:
    DecimalCommaSorter(bool isDescending, size_t fromColumn, size_t toColumn)
        : ISorter(isDescending, fromColumn, toColumn) {}

protected:
    QString prepareStringForConversion(const QString& input) {
        QString admissable = getSortKey(input);
        admissable = admissable.simplified().remove(' ');
        admissable.replace(',', '.');
        return admissable;
    }

public:
    void sort(QStringList& lines) override {
        QList<QPair<QString, double>> pairs;
        for (const QString& line : lines) {
            QString prep = prepareStringForConversion(line);
            bool ok;
            double val = prep.toDouble(&ok);
            if (ok) {
                pairs.append(qMakePair(line, val));
            } else {
                pairs.append(qMakePair(line, 0.0));
            }
        }

        if (isDescending()) {
            std::sort(pairs.begin(), pairs.end(), [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                return a.second > b.second;
            });
        } else {
            std::sort(pairs.begin(), pairs.end(), [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                return a.second < b.second;
            });
        }

        lines.clear();
        for (const auto& pair : pairs) {
            lines.append(pair.first);
        }
    }
};