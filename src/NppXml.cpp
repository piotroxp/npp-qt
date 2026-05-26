// NppXml.cpp - XML parsing utilities for NppParameters
#include "NppXml.h"

bool NppXmlDoc::load(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _errorString = QString("Cannot open file: %1").arg(filePath);
        return false;
    }
    
    QXmlStreamReader xml(&file);
    
    if (xml.atEnd()) {
        _isValid = true;
        file.close();
        return true;
    }
    
    try {
        parseElement(xml, _root);
        _isValid = true;
    } catch (...) {
        _errorString = xml.errorString();
        _isValid = false;
    }
    
    file.close();
    return _isValid;
}

bool NppXmlDoc::save(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    
    NppXmlWriter::writeElement(&file, _root);
    
    xml.writeEndDocument();
    file.close();
    return true;
}

void NppXmlDoc::parseElement(QXmlStreamReader& xml, XmlNode& node) {
    if (xml.tokenType() != QXmlStreamReader::StartElement) {
        xml.readNext();
        return;
    }
    
    node.name = xml.name().toString();
    
    // Parse attributes
    for (const QXmlStreamAttribute& attr : xml.attributes()) {
        node.attributes[attr.name().toString()] = attr.value().toString();
    }
    
    // Parse children and text
    while (!xml.atEnd()) {
        xml.readNext();
        
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            XmlNode child(xml.name().toString());
            parseElement(xml, child);
            node.children.append(child);
        } else if (xml.tokenType() == QXmlStreamReader::EndElement) {
            break;
        } else if (xml.tokenType() == QXmlStreamReader::Characters) {
            if (!xml.isWhitespace()) {
                node.text += xml.text().toString();
            }
        }
    }
}

XmlNode* NppXmlDoc::findNode(const QString& path) {
    QStringList parts = path.split('/');
    XmlNode* current = &_root;
    
    for (const QString& part : parts) {
        if (!current) return nullptr;
        current = current->findChild(part);
    }
    
    return current;
}

QVector<XmlNode*> NppXmlDoc::findNodes(const QString& path) {
    QVector<XmlNode*> result;
    // Simple implementation - can be extended for full XPath support
    XmlNode* node = findNode(path);
    if (node) {
        result.append(node);
    }
    return result;
}

bool NppXmlWriter::writeElement(QIODevice* device, const XmlNode& node, int indent) {
    QXmlStreamWriter xml(device);
    xml.setAutoFormatting(true);
    
    QString indentStr(indent * 2, ' ');
    
    xml.writeCharacters("\n");
    xml.writeCharacters(indentStr);
    xml.writeStartElement(node.name);
    
    // Write attributes
    for (auto it = node.attributes.constBegin(); it != node.attributes.constEnd(); ++it) {
        xml.writeAttribute(it.key(), it.value());
    }
    
    // Write text
    if (!node.text.isEmpty()) {
        xml.writeCharacters(node.text);
    }
    
    // Write children
    for (const XmlNode& child : node.children) {
        writeElement(device, child, indent + 1);
    }
    
    xml.writeEndElement();
    
    return true;
}

QByteArray NppXmlWriter::nodeToByteArray(const XmlNode& node) {
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    
    QXmlStreamWriter xml(&buffer);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    
    writeElement(&buffer, node);
    
    xml.writeEndDocument();
    
    return data;
}

NppXmlBuilder::NppXmlBuilder(const QString& rootName) {
    _root.name = rootName;
    _stack.push_back(&_root);
}

NppXmlBuilder& NppXmlBuilder::addChild(const QString& name) {
    XmlNode child(name);
    _stack.back()->children.append(child);
    _stack.push_back(&(_stack.back()->children.last()));
    return *this;
}

NppXmlBuilder& NppXmlBuilder::addText(const QString& text) {
    _stack.back()->text = text;
    return *this;
}

NppXmlBuilder& NppXmlBuilder::addAttr(const QString& key, const QString& value) {
    _stack.back()->attributes[key] = value;
    return *this;
}

NppXmlBuilder& NppXmlBuilder::addAttr(const QString& key, int value) {
    _stack.back()->attributes[key] = QString::number(value);
    return *this;
}

XmlNode NppXmlBuilder::build() {
    return _root;
}