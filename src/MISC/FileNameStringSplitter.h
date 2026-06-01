// MISC/FileNameStringSplitter.h - Qt6 port of Notepad++ file name splitting
#pragma once

#include <QString>
#include <QStringList>
#include <string>
#include <vector>

typedef QStringList stringVector;

class FileNameStringSplitter
{
public:
    FileNameStringSplitter(const wchar_t* fileNameStr)
        : FileNameStringSplitter(QString::fromWCharArray(fileNameStr ? fileNameStr : L""))
    {}

    FileNameStringSplitter(const QString& fileNameStr)
    {
        QString current;
        bool isInsideQuotes = false;

        for (QChar ch : fileNameStr)
        {
            if (ch == QChar('"'))
            {
                isInsideQuotes = !isInsideQuotes;
                continue;
            }

            if (!isInsideQuotes && ch.isSpace())
            {
                if (!current.isEmpty())
                {
                    _fileNames.append(current);
                    current.clear();
                }
                continue;
            }

            current.append(ch);
        }

        if (!current.isEmpty())
            _fileNames.append(current);
    }

    const wchar_t* getFileName(size_t index) const {
        if (index >= static_cast<size_t>(_fileNames.size()))
            return nullptr;
        _wstringCache.resize(_fileNames.size());
        if (_wstringCache[index].empty())
            _wstringCache[index] = _fileNames.at(static_cast<int>(index)).toStdWString();
        return _wstringCache[index].c_str();
    }

    std::vector<std::wstring> getFileNames() const {
        std::vector<std::wstring> out;
        out.reserve(_fileNames.size());
        for (const QString& s : _fileNames)
            out.push_back(s.toStdWString());
        return out;
    }

    int size() const {
        return _fileNames.size();
    }

private:
    stringVector _fileNames;
    mutable std::vector<std::wstring> _wstringCache;
};