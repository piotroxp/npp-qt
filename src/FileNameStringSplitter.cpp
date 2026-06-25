// npp-qt: FileNameStringSplitter implementation
#include "FileNameStringSplitter.h"
#include <QFileInfo>

QString FileNameStringSplitter::baseName() const
{
    // Return everything after the last / or \
    const QChar fwd = QChar(u'/');
    const QChar bck = QChar(u'\\');
    int ixFwd = _str.lastIndexOf(fwd);
    int ixBck = _str.lastIndexOf(bck);
    int ix = qMax(ixFwd, ixBck);
    if (ix < 0) return _str;
    return _str.mid(ix + 1);
}

QString FileNameStringSplitter::dirName() const
{
    // Return everything before the last / or \
    const QChar fwd = QChar(u'/');
    const QChar bck = QChar(u'\\');
    int ixFwd = _str.lastIndexOf(fwd);
    int ixBck = _str.lastIndexOf(bck);
    int ix = qMax(ixFwd, ixBck);
    if (ix < 0) return QString();
    return _str.left(ix);
}

QString FileNameStringSplitter::extension() const
{
    // Return everything after the last .
    int ixDot = _str.lastIndexOf(QChar(u'.'));
    if (ixDot < 0) return QString();
    return _str.mid(ixDot + 1);
}

QString FileNameStringSplitter::baseNameWithoutExtension() const
{
    QString base = baseName();
    int ixDot = base.lastIndexOf(QChar(u'.'));
    if (ixDot < 0) return base;
    return base.left(ixDot);
}

int FileNameStringSplitter::componentCount() const
{
    // Count path components separated by / or \
    int nComponents = 0;
    const QChar fwd = QChar(u'/');
    const QChar bck = QChar(u'\\');
    bool prevSep = true;
    for (QChar c : _str) {
        if (c == fwd || c == bck) {
            if (!prevSep) nComponents++;
            prevSep = true;
        } else {
            prevSep = false;
        }
    }
    if (!prevSep) nComponents++;
    return nComponents;
}

QString FileNameStringSplitter::operator[](int index) const
{
    if (index < 0) return QString();
    QStringList parts;
    QString s = _str;
    const QChar bck = QChar(u'\\');
    const QChar fwd = QChar(u'/');
    s.replace(bck, fwd);
    parts = s.split(fwd, Qt::SkipEmptyParts);
    if (index >= parts.size()) return QString();
    return parts[index];
}
