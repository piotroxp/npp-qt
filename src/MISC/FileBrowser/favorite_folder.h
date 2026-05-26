// MISC/FileBrowser/favorite_folder.h - Qt6 port of favorite folders for file browser
#pragma once

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QFileSystemModel>

class FavoriteFolder
{
public:
    FavoriteFolder() {}
    FavoriteFolder(const QString& name, const QString& path)
        : _name(name), _path(path), _order(0) {}
    
    QString name() const { return _name; }
    void setName(const QString& name) { _name = name; }
    
    QString path() const { return _path; }
    void setPath(const QString& path) { _path = path; }
    
    int order() const { return _order; }
    void setOrder(int order) { _order = order; }
    
    bool isValid() const { return !_path.isEmpty(); }
    
    // For comparison
    bool operator==(const FavoriteFolder& other) const {
        return _name == other._name && _path == other._path;
    }
    
private:
    QString _name;
    QString _path;
    int _order;
};

class FavoriteFolderManager
{
public:
    static FavoriteFolderManager& getInstance();
    
    // Favorites management
    void addFavorite(const QString& name, const QString& path);
    void removeFavorite(const QString& name);
    void renameFavorite(const QString& oldName, const QString& newName);
    
    // Get all favorites
    QList<FavoriteFolder> getFavorites() const;
    QStringList getFavoritePaths() const;
    
    // Save/Load
    void save();
    void load();
    
    // Check if path is a favorite
    bool isFavorite(const QString& path) const;
    
private:
    FavoriteFolderManager();
    ~FavoriteFolderManager() = default;
    
    FavoriteFolderManager(const FavoriteFolderManager&) = delete;
    FavoriteFolderManager& operator=(const FavoriteFolderManager&) = delete;
    
    QList<FavoriteFolder> _favorites;
    static const QString settingsKey;
};