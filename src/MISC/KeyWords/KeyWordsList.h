// MISC/KeyWords/KeyWordsList.h - Qt6 port of keyword lists for syntax highlighting
#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

class KeyWordsList
{
public:
    KeyWordsList();
    explicit KeyWordsList(const QString& keywords);
    
    // Set keywords from string (space/tab separated)
    void setKeywords(const QString& keywords);
    void addKeyword(const QString& keyword);
    void removeKeyword(const QString& keyword);
    
    // Get keywords
    const QStringList& get() const { return _keywords; }
    QStringList get() { return _keywords; }
    
    // Check if keyword exists
    bool contains(const QString& keyword) const;
    
    // Get keyword at index
    QString at(int index) const;
    
    // Number of keywords
    int count() const { return _keywords.size(); }
    int size() const { return _keywords.size(); }
    bool isEmpty() const { return _keywords.isEmpty(); }
    
    // Merge another keyword list
    void merge(const KeyWordsList& other);
    
    // Clear all keywords
    void clear() { _keywords.clear(); }
    
    // Sort keywords
    void sort();
    
    // Get as single string (for saving)
    QString toSingleString() const;
    
private:
    QStringList _keywords;
    void parseKeywords(const QString& keywords);
};

// Collection of keyword lists for different lexer types
class KeyWordsCollection
{
public:
    static KeyWordsCollection& getInstance();
    
    void addList(const QString& lexerName, const KeyWordsList& keywords);
    KeyWordsList getList(const QString& lexerName) const;
    bool hasList(const QString& lexerName) const;
    QStringList getLexerNames() const;
    
private:
    KeyWordsCollection() = default;
    
    QMap<QString, KeyWordsList> _collection;
};