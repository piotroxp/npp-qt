// npp-qt: FileNameStringSplitter implementation
#include "FileNameStringSplitter.h"
#include <QFileInfo>

static const QChar FWD_SLASH = u'/';
static const QChar BCK_SLASH = u'\\';

QString FileNameStringSplitter::baseName() const
{
    int posFwd = _str.lastIndexOf(FWD_SLASH);
    int posBck = _str.lastIndexOf(BCK_SLASH);
    int ix = qMax(posFwd, posBck);
    if (ix < 0) return _str;
    return _str.mid(ix + 1);
}

QString FileNameStringSplitter::dirName() const
{
    int posFwd = _str.lastIndexOf(FWD_SLASH);
    int posBck = _str.lastIndexOf(BCK_SLASH);
    int ix = qMax(posFwd, posBck);
    if (ix < 0) return QString();
    return _str.left(ix);
}

QString FileNameStringSplitter::extension() const
{
    int ixDot = _str.lastIndexOf(u'.');
    if (ixDot < 0) return QString();
    return _str.mid(ixDot + 1);
}

QString FileNameStringSplitter::baseNameWithoutExtension() const
{
    QString base = baseName();
    int ixDot = base.lastIndexOf(u'.');
    if (ixDot < 0) return base;
    return base.left(ixDot);
}

int FileNameStringSplitter::componentCount() const
{
    int count = 0;
    bool wasSep = true;
    for (QChar ch : _str) {
        if (ch == FWD_SLASH || ch == BCK_SLASH) {
            if (!wasSep) count++;
            wasSep = true;
        } else {
            wasSep = false;
        }
    }
    if (!wasSep) count++;
    return count;
}

QString FileNameStringSplitter::operator[](int index) const
{
    if (index < 0) return QString();
    QString s = _str;
    s.replace(BCK_SLASH, FWD_SLASH);
    QStringList parts = s.split(FWD_SLASH, Qt::SkipEmptyParts);
    if (index >= parts.size()) return QString();
    return parts[index];
}
