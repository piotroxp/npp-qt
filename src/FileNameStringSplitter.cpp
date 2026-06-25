// npp-qt: FileNameStringSplitter implementation
#include "FileNameStringSplitter.h"
#include <QFileInfo>

QString FileNameStringSplitter::baseName() const
{
    // Return everything after the last / or \\
    int lastSlash = _str.lastIndexOf('/');
    int lastBack = _str.lastIndexOf('\\');
    int last = qMax(lastSlash, lastBack);
    if (last < 0) return _str;
    return _str.mid(last + 1);
}

QString FileNameStringSplitter::dirName() const
{
    // Return everything before the last / or \\
    int lastSlash = _str.lastIndexOf('/');
    int lastBack = _str.lastIndexOf('\\');
    int last = qMax(lastSlash, lastBack);
    if (last < 0) return QString();
    return _str.left(last);
}

QString FileNameStringSplitter::extension() const
{
    // Return everything after the last .
    int dot = _str.lastIndexOf('.');
    if (dot < 0) return QString();
    return _str.mid(dot + 1);
}

QString FileNameStringSplitter::baseNameWithoutExtension() const
{
    QString base = baseName();
    int dot = base.lastIndexOf('.');
    if (dot < 0) return base;
    return base.left(dot);
}

int FileNameStringSplitter::componentCount() const
{
    // Count path components separated by / or \\
    int count = 0;
    bool lastWasSep = true;  // Start after the first separator
    for (QChar c : _str) {
        if (c == '/' || c == '\\') {
            if (!lastWasSep) count++;
            lastWasSep = true;
        } else {
            lastWasSep = false;
        }
    }
    if (!lastWasSep) count++;  // Count the last component
    return count;
}

QString FileNameStringSplitter::operator[](int index) const
{
    if (index < 0) return QString();
    QStringList parts;
    QString s = _str;
    s.replace('\\', '/');
    parts = s.split('/', Qt::SkipEmptyParts);
    if (index >= parts.size()) return QString();
    return parts[index];
}
