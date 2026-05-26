// MISC/FileNameStringSplitter.h - Qt6 port of Notepad++ file name splitting
#pragma once

#include <QString>
#include <QStringList>

typedef QStringList stringVector;

class FileNameStringSplitter
{
public:
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

    const stringVector& getFileNames() const {
        return _fileNames;
    }

    const QString getFileName(size_t index) const {
        if (index >= static_cast<size_t>(_fileNames.size()))
            return QString();
        return _fileNames.at(static_cast<int>(index));
    }

    int size() const {
        return _fileNames.size();
    }

private:
    stringVector _fileNames;
};