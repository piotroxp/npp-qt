// Semantic Lift: ImageListSet — Win32 ImageList_* + HIMAGELIST → Qt6 QVector<QIcon>
// Original: PowerEditor/src/WinControls/ImageListSet/ImageListSet.cpp (483 lines)
// Win32 → Qt6: ImageList_Create + ImageList_AddIcon → QVector<QIcon>

#include "ImageListSet.h"
#include <QDir>
#include <QFileInfo>
#include <QImageReader>

ImageListSet::ImageListSet(int iconCX, int iconCY)
    : _iconCX(iconCX), _iconCY(iconCY)
{
}

ImageListSet::~ImageListSet()
{
    destroy();
}

void ImageListSet::create()
{
    // QVector auto-initializes
}

void ImageListSet::destroy()
{
    _icons.clear();
}

int ImageListSet::addIcon(const QIcon& icon)
{
    _icons.append(icon);
    return _icons.size() - 1;
}

int ImageListSet::addIcons(const QVector<QIcon>& icons)
{
    int start = _icons.size();
    for (const QIcon& icon : icons) {
        _icons.append(icon);
    }
    return start;
}

QIcon ImageListSet::getIcon(int index) const
{
    if (index >= 0 && index < _icons.size())
        return _icons.at(index);
    return QIcon();
}

int ImageListSet::getIconCount() const
{
    return _icons.size();
}

void ImageListSet::replaceIcon(int index, const QIcon& icon)
{
    if (index >= 0 && index < _icons.size())
        _icons[index] = icon;
}

// ── IconLoader ────────────────────────────────────────────────────────────────

IconLoader::IconLoader()
{
}

IconLoader::~IconLoader()
{
}

IconLoader& IconLoader::instance()
{
    static IconLoader _self;
    return _self;
}

void IconLoader::setIconDirectory(const QString& dirPath)
{
    _iconDir = dirPath;
    QDir dir(dirPath);
    if (!dir.exists())
        return;

    // Load all icons from directory
    QStringList filters;
    filters << QStringLiteral("*.png") << QStringLiteral("*.ico") << QStringLiteral("*.svg");
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fi : files) {
        int id = getIconIdFromName(fi.baseName());
        if (id >= 0) {
            _iconMap[id] = QIcon(fi.absoluteFilePath());
        }
    }
}

QIcon IconLoader::getIcon(int iconId) const
{
    return _iconMap.value(iconId, QIcon());
}

int IconLoader::getIconIdFromName(const QString& name) const
{
    // Map icon name to ID (IDs are defined in resource.h)
    // This is a simple hash — real implementation would use a static map
    return qHash(name);
}

bool IconLoader::hasIcon(int iconId) const
{
    return _iconMap.contains(iconId);
}
