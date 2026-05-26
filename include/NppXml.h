// NppXml.h - XML parsing utilities for NppParameters
#pragma once

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QIODevice>
#include <QVector>
#include <QMap>

struct XmlNode {
    QString name;
    QMap<QString, QString> attributes;
    QString text;
    QVector<XmlNode> children;
    
    XmlNode() {}
    XmlNode(const QString& n) : name(n) {}
    
    QString attribute(const QString& key, const QString& def = QString()) const {
        return attributes.value(key, def);
    }
    
    XmlNode* findChild(const QString& childName) {
        for (auto& child : children) {
            if (child.name == childName)
                return &child;
        }
        return nullptr;
    }
};

class NppXmlDoc {
public:
    NppXmlDoc() : _root() {}
    
    bool load(const QString& filePath);
    bool save(const QString& filePath) const;
    
    XmlNode& root() { return _root; }
    const XmlNode& root() const { return _root; }
    
    bool isValid() const { return _isValid; }
    QString errorString() const { return _errorString; }
    
    // XPath-like queries
    XmlNode* findNode(const QString& path);
    QVector<XmlNode*> findNodes(const QString& path);
    
private:
    XmlNode _root;
    bool _isValid = false;
    QString _errorString;
    
    void parseElement(QXmlStreamReader& xml, XmlNode& node);
};

class NppXmlWriter {
public:
    static bool writeElement(QIODevice* device, const XmlNode& node, int indent = 0);
    static QByteArray nodeToByteArray(const XmlNode& node);
};

// Convenience class for building XML
class NppXmlBuilder {
public:
    NppXmlBuilder(const QString& rootName);
    
    NppXmlBuilder& addChild(const QString& name);
    NppXmlBuilder& addText(const QString& text);
    NppXmlBuilder& addAttr(const QString& key, const QString& value);
    NppXmlBuilder& addAttr(const QString& key, int value);
    
    XmlNode build();
    
private:
    XmlNode _root;
    QVector<XmlNode*> _stack;
};