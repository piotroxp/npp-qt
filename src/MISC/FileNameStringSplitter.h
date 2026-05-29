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
        const int filePathLength = 260; // MAX_PATH
        QString str;
        bool isInsideQuotes = false;
        int i = 0;
        bool fini = false;

        const QChar* pStr = fileNameStr.constData();
        
        while (!fini && i < filePathLength)
        {
            if (*pStr == '"')
            {
                if (isInsideQuotes)
                {
                    str[i] = '\0';
                    if (!str.isEmpty() && i > 0)
                        _fileNames.append(str);
                    i = 0;
                    str.clear();
                }
                isInsideQuotes = !isInsideQuotes;
                ++pStr;
            }
            else if (*pStr == ' ')
            {
                if (isInsideQuotes)
                {
                    str[i] = *pStr;
                    ++i;
                }
                else
                {
                    str[i] = '\0';
                    if (!str.isEmpty() && i > 0)
                        _fileNames.append(str);
                    i = 0;
                    str.clear();
                }
                ++pStr;
            }
            else if (*pStr == '\0')
            {
                str[i] = *pStr;
                if (!str.isEmpty() && i > 0)
                    _fileNames.append(str);
                fini = true;
            }
            else
            {
                str[i] = *pStr;
                ++i;
                ++pStr;
            }
        }
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