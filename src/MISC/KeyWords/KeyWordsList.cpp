// MISC/KeyWords/KeyWordsList.cpp - Qt6 port of keyword lists for syntax highlighting
#include "KeyWordsList.h"
#include <QRegularExpression>
#include <QStringList>

KeyWordsList::KeyWordsList()
{
}

KeyWordsList::KeyWordsList(const QString& keywords)
{
    setKeywords(keywords);
}

void KeyWordsList::setKeywords(const QString& keywords)
{
    _keywords.clear();
    parseKeywords(keywords);
}

void KeyWordsList::parseKeywords(const QString& keywords)
{
    // Split by whitespace (space or tab)
    QStringList parts = keywords.split(QRegularExpression("[\\s]+"), Qt::SkipEmptyParts);
    for (const QString& part : parts) {
        if (!part.isEmpty())
            _keywords.append(part);
    }
}

void KeyWordsList::addKeyword(const QString& keyword)
{
    if (!keyword.isEmpty() && !_keywords.contains(keyword))
        _keywords.append(keyword);
}

void KeyWordsList::removeKeyword(const QString& keyword)
{
    _keywords.removeAll(keyword);
}

bool KeyWordsList::contains(const QString& keyword) const
{
    return _keywords.contains(keyword);
}

QString KeyWordsList::at(int index) const
{
    if (index >= 0 && index < _keywords.size())
        return _keywords.at(index);
    return QString();
}

void KeyWordsList::merge(const KeyWordsList& other)
{
    for (const QString& kw : other._keywords) {
        if (!_keywords.contains(kw))
            _keywords.append(kw);
    }
}

void KeyWordsList::sort()
{
    _keywords.sort();
}

QString KeyWordsList::toSingleString() const
{
    return _keywords.join(" ");
}

KeyWordsCollection& KeyWordsCollection::getInstance()
{
    static KeyWordsCollection instance;
    return instance;
}

void KeyWordsCollection::addList(const QString& lexerName, const KeyWordsList& keywords)
{
    _collection[lexerName] = keywords;
}

KeyWordsList KeyWordsCollection::getList(const QString& lexerName) const
{
    auto it = _collection.find(lexerName);
    if (it != _collection.end())
        return it.value();
    return KeyWordsList();
}

bool KeyWordsCollection::hasList(const QString& lexerName) const
{
    return _collection.contains(lexerName);
}

QStringList KeyWordsCollection::getLexerNames() const
{
    return _collection.keys();
}