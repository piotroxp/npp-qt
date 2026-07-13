#pragma once

#include <QWidget>
#include <QIcon>
#include <QVector>
#include <QHash>
#include <QString>

class ImageListSet
{
public:
    explicit ImageListSet(int iconCX, int iconCY);
    ~ImageListSet();

    // Create/destroy
    void create();
    void destroy();

    // Add icons
    int addIcon(const QIcon& icon);
    int addIcons(const QVector<QIcon>& icons);

    // Get
    QIcon getIcon(int index) const;
    int getIconCount() const;

    // Replace
    void replaceIcon(int index, const QIcon& icon);

    // Size
    int getIconCX() const { return _iconCX; }
    int getIconCY() const { return _iconCY; }

private:
    int _iconCX;
    int _iconCY;
    QVector<QIcon> _icons;
};

// Global icon loader (singleton)
class IconLoader
{
public:
    static IconLoader& instance();
    void setIconDirectory(const QString& dirPath);

    QIcon getIcon(int iconId) const;
    int getIconIdFromName(const QString& name) const;
    bool hasIcon(int iconId) const;

private:
    IconLoader();
    ~IconLoader();
    QHash<int, QIcon> _iconMap;
    QString _iconDir;
};

