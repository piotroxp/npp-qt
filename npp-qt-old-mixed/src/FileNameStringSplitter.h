// npp-qt: String splitting utilities
#pragma once
#include <QString>
#include <QStringList>

#include <string>
// Splits a string by a delimiter character
inline QStringList splitString(const QString& str, QChar delim) {
    return str.split(delim, Qt::SkipEmptyParts);
}

// Splits a string by delimiter string
inline QStringList splitString(const QString& str, const QString& delim) {
    return str.split(delim, Qt::SkipEmptyParts);
}

// FileNameStringSplitter: utility for parsing file paths and names
class FileNameStringSplitter {
public:
    explicit FileNameStringSplitter(const QString& str) : _str(str) {}
    explicit FileNameStringSplitter(const wchar_t* str) : _str(QString::fromWCharArray(str)) {}
    explicit FileNameStringSplitter(const std::wstring& str) : _str(QString::fromStdWString(str)) {}
    ~FileNameStringSplitter() = default;

    // Get the base name (last component after last / or \\)
    QString baseName() const;

    // Get the directory name (everything before last / or \\)
    QString dirName() const;

    // Get the extension (after last .)
    QString extension() const;

    // Get the file name without extension
    QString baseNameWithoutExtension() const;

    // Count the number of path components
    int componentCount() const;

    // Get the nth component (0-indexed from left)
    QString operator[](int index) const;
    int size() const { return componentCount(); }
    QString getFileName(int index) const { return (*this)[index]; }
    // Return all components as a QStringList (for command-line batch processing)
    QStringList fileNames() const {
        QStringList result;
        for (int i = 0; i < componentCount(); ++i) result.push_back((*this)[i]);
        return result;
    }

private:
    const QString& _str;
};
