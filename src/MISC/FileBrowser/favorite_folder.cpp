// MISC/FileBrowser/favorite_folder.cpp - Qt6 port of favorite folders for file browser
#include "favorite_folder.h"

const QString FavoriteFolderManager::settingsKey = "Favorites/Folders";

FavoriteFolderManager::FavoriteFolderManager()
{
    load();
}

FavoriteFolderManager& FavoriteFolderManager::getInstance()
{
    static FavoriteFolderManager instance;
    return instance;
}

void FavoriteFolderManager::addFavorite(const QString& name, const QString& path)
{
    FavoriteFolder fav(name, path);
    fav.setOrder(_favorites.size());
    
    // Check if already exists
    for (int i = 0; i < _favorites.size(); ++i) {
        if (_favorites[i].path() == path)
            return;
    }
    
    _favorites.append(fav);
    save();
}

void FavoriteFolderManager::removeFavorite(const QString& name)
{
    for (int i = 0; i < _favorites.size(); ++i) {
        if (_favorites[i].name() == name) {
            _favorites.removeAt(i);
            save();
            return;
        }
    }
}

void FavoriteFolderManager::renameFavorite(const QString& oldName, const QString& newName)
{
    for (int i = 0; i < _favorites.size(); ++i) {
        if (_favorites[i].name() == oldName) {
            _favorites[i].setName(newName);
            save();
            return;
        }
    }
}

QList<FavoriteFolder> FavoriteFolderManager::getFavorites() const
{
    return _favorites;
}

QStringList FavoriteFolderManager::getFavoritePaths() const
{
    QStringList paths;
    for (const FavoriteFolder& fav : _favorites) {
        paths.append(fav.path());
    }
    return paths;
}

void FavoriteFolderManager::save()
{
    QSettings settings;
    settings.beginGroup("Favorites");
    settings.remove("");
    
    int index = 0;
    for (const FavoriteFolder& fav : _favorites) {
        settings.setValue(QString("Folder%1/Name").arg(index), fav.name());
        settings.setValue(QString("Folder%1/Path").arg(index), fav.path());
        settings.setValue(QString("Folder%1/Order").arg(index), fav.order());
        ++index;
    }
    
    settings.endGroup();
}

void FavoriteFolderManager::load()
{
    _favorites.clear();
    
    QSettings settings;
    settings.beginGroup("Favorites");
    
    QStringList groups = settings.childGroups();
    for (const QString& group : groups) {
        QString name = settings.value(QString("%1/Name").arg(group)).toString();
        QString path = settings.value(QString("%1/Path").arg(group)).toString();
        int order = settings.value(QString("%1/Order").arg(group)).toInt();
        
        if (!name.isEmpty() && !path.isEmpty()) {
            FavoriteFolder fav(name, path);
            fav.setOrder(order);
            _favorites.append(fav);
        }
    }
    
    settings.endGroup();
}

bool FavoriteFolderManager::isFavorite(const QString& path) const
{
    for (const FavoriteFolder& fav : _favorites) {
        if (fav.path() == path)
            return true;
    }
    return false;
}