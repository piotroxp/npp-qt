// NppXml — Minimal XML parsing helpers for Notepad++
// Stub for Qt port — uses Qt XML instead of TinyXML2 from Win32 source.
#pragma once

#include <QtCore/QString>
#include <QtCore/QXmlStreamReader>

namespace NppXml {

// Opaque handle for TinyXML2 XML node — mapped to QXmlStreamReader
using Document = QXmlStreamReader*;
using Element = QXmlStreamReader::TokenType;

// Get root element from document
inline Element firstChildElement(Document doc, const char* tagName) {
    if (!doc) return QXmlStreamReader::Invalid;
    doc->readNextStartElement();
    if (doc->hasError() || !doc->isStartElement())
        return QXmlStreamReader::Invalid;
    if (!tagName || doc->name() == QLatin1String(tagName))
        return QXmlStreamReader::StartElement;
    return QXmlStreamReader::Invalid;
}

inline const char* attribute(Element, const char*) {
    return nullptr;
}

inline int intAttribute(Element, const char*, int defaultVal) {
    Q_UNUSED(defaultVal);
    return 0;
}

inline Element nextSiblingElement(Element, const char*) {
    return QXmlStreamReader::EndElement;
}

} // namespace NppXml
