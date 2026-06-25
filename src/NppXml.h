// NppXml — TinyXML2-compatible API over Qt DOM (QDomDocument)
// Parameters.cpp / localization.cpp use this for all config file read/write.
// Design: Document = QDomDocument*, Element = QDomElement (value handle).
// ============================================================================
#pragma once

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QTextStream>
#include <cstdio>   // std::fopen, std::fclose

namespace NppXml {

// ── Type aliases ──────────────────────────────────────────────────────────────
using Document  = QDomDocument*;   // owned pointer; delete via deleteDocument()
using Element   = QDomElement;     // value handle (copy-constructible)
using Node      = QDomNode;
using Attribute = QDomAttr;

// ── Construction / destruction ─────────────────────────────────────────────

// Create a fresh empty XML document. Caller owns the pointer.
inline Document NewDocument() { return new QDomDocument(); }

// Delete a document and all its content.
inline void deleteDocument(Document doc) { delete doc; }

// ── Helper: wchar_t → QString ──────────────────────────────────────────────
inline QString wcharToQString(const wchar_t* path) {
    return path ? QString::fromWCharArray(path) : QString();
}

// ── Load from file ──────────────────────────────────────────────────────────

// Load an XML file into an existing Document (replaces its content).
inline bool loadFile(Document& doc, const wchar_t* filePath) {
    if (!doc) doc = NewDocument();
    QFile f(wcharToQString(filePath));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QString errMsg; int errLine = 0, errCol = 0;
    bool ok = doc->setContent(&f, true, &errMsg, &errLine, &errCol);
    f.close();
    if (!ok) {
        qWarning("NppXml::loadFile error at %d:%d: %s",
                 errLine, errCol, qUtf8Printable(errMsg));
        delete doc; doc = nullptr;
        return false;
    }
    return true;
}

inline bool loadFileShortcut(Document& doc, const wchar_t* p) { return loadFile(doc, p); }
inline bool loadFileNativeLang(Document& doc, const wchar_t* p) { return loadFile(doc, p); }
inline bool loadFileUDL(Document& doc, const wchar_t* p) { return loadFile(doc, p); }
inline bool loadFileContextMenu(Document& doc, const wchar_t* p) { return loadFile(doc, p); }

// ── Save to file ─────────────────────────────────────────────────────────────

inline bool saveFile(Document doc, const wchar_t* filePath) {
    if (!doc) return false;
    QFile f(wcharToQString(filePath));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream ts(&f);
    ts << doc->toString(2);
    f.close();
    return !f.error();
}

inline bool saveFileShortcut(Document doc, const wchar_t* p) { return saveFile(doc, p); }
inline bool saveFileUDL(Document doc, const wchar_t* p) { return saveFile(doc, p); }

// ── Document root helpers ─────────────────────────────────────────────────────

// Get root element of a document
inline Element documentElement(Document doc) {
    return doc ? doc->documentElement() : Element();
}

// firstChildElement on a Document returns its root element
inline Element firstChildElement(Document doc, const char* tagName = nullptr) {
    Element root = documentElement(doc);
    if (!root.isNull() && tagName) {
        return root.firstChildElement(QLatin1String(tagName));
    }
    return root;
}

// Create a child element of the document root (convenience)
inline Element createChildElement(Document doc, const char* tagName) {
    Element root = documentElement(doc);
    if (root.isNull()) {
        root = doc->createElement(QLatin1String(tagName));
        doc->appendChild(root);
        return root;
    }
    return root.appendChild(doc->createElement(QLatin1String(tagName))).toElement();
}

// ── Navigation ───────────────────────────────────────────────────────────────

inline Element firstChildElement(Element elem, const char* tagName = nullptr) {
    if (elem.isNull()) return Element();
    if (tagName)
        return elem.firstChildElement(QLatin1String(tagName));
    return elem.firstChildElement();
}

inline Element nextSiblingElement(Element elem, const char* tagName = nullptr) {
    if (elem.isNull()) return Element();
    Element s = elem.nextSiblingElement();
    if (tagName) {
        while (!s.isNull() && s.tagName() != QLatin1String(tagName))
            s = s.nextSiblingElement();
    }
    return s;
}

inline Node firstChild(Element elem) { return elem.firstChild(); }

inline Element lastChild(Element elem, const char* tagName = nullptr) {
    if (elem.isNull()) return Element();
    if (tagName)
        return elem.lastChildElement(QLatin1String(tagName));
    return elem.lastChildElement();
}

inline Node next(Node n) { return n.nextSibling(); }

inline Node insertEndChild(Element parent, const Node& child) {
    return parent.appendChild(child);
}

inline void deleteChild(Element parent, const Node& child) {
    if (!parent.isNull()) parent.removeChild(child);
}

inline void clear(Element elem) {
    while (!elem.firstChild().isNull())
        elem.removeChild(elem.firstChild());
}

// No-op: QDomDocument always emits XML declaration on save
inline void createNewDeclaration(Document /*doc*/) {}

// ── Attribute access ─────────────────────────────────────────────────────────

inline QString attribute(Element elem, const char* attrName) {
    return elem.isNull() ? QString() : elem.attribute(QLatin1String(attrName));
}

inline QString attribute(Element elem, const char* attrName, const char* defaultValue) {
    if (elem.isNull()) return QString::fromLatin1(defaultValue);
    QString val = elem.attribute(QLatin1String(attrName));
    return val.isNull() ? QString::fromLatin1(defaultValue) : val;
}

inline int intAttribute(Element elem, const char* attrName, int defaultVal = 0) {
    if (elem.isNull()) return defaultVal;
    bool ok = false; int v = elem.attribute(QLatin1String(attrName)).toInt(&ok);
    return ok ? v : defaultVal;
}

inline long long int64Attribute(Element elem, const char* attrName, long long defaultVal = 0) {
    if (elem.isNull()) return defaultVal;
    bool ok = false; long long v = elem.attribute(QLatin1String(attrName)).toLongLong(&ok);
    return ok ? v : defaultVal;
}

inline unsigned long long uint64Attribute(Element elem, const char* attrName,
                                         unsigned long long defaultVal = 0) {
    if (elem.isNull()) return defaultVal;
    bool ok = false;
    unsigned long long v = elem.attribute(QLatin1String(attrName)).toULongLong(&ok);
    return ok ? v : defaultVal;
}

// setAttribute overloads
inline void setAttribute(Element elem, const char* attrName, const char* value) {
    if (!elem.isNull()) elem.setAttribute(QLatin1String(attrName), QLatin1String(value));
}
inline void setAttribute(Element elem, const char* attrName, const QString& value) {
    if (!elem.isNull()) elem.setAttribute(QLatin1String(attrName), value);
}
inline void setAttribute(Element elem, const char* attrName, int value) {
    if (!elem.isNull()) elem.setAttribute(QLatin1String(attrName), value);
}
inline void setAttribute(Element elem, const char* attrName, long long value) {
    if (!elem.isNull()) elem.setAttribute(QLatin1String(attrName), value);
}
inline void setAttribute(Element elem, const char* attrName, unsigned long long value) {
    if (!elem.isNull()) elem.setAttribute(QLatin1String(attrName), QString::number(value));
}

// ── Attribute iteration ──────────────────────────────────────────────────────

inline Attribute firstAttribute(Element elem) {
    if (elem.isNull()) return QDomAttr();
    QDomNamedNodeMap attrs = elem.attributes();
    return attrs.isEmpty() ? QDomAttr() : attrs.item(0).toAttr();
}

inline Attribute next(Attribute attr) {
    return attr.nextSibling().toAttr();
}

inline const char* name(Attribute attr) {
    static thread_local QString s;
    if (attr.isNull()) return "";
    s = attr.name(); return s.toLatin1().constData();
}

// ── Element content ───────────────────────────────────────────────────────────

inline QString text(Element elem) { return elem.isNull() ? QString() : elem.text(); }

inline void setValue(Element elem, const QString& value) {
    if (elem.isNull()) return;
    while (!elem.firstChild().isNull()) elem.removeChild(elem.firstChild());
    elem.appendChild(elem.ownerDocument().createTextNode(value));
}
inline void setValue(Element elem, const char* value) {
    setValue(elem, QString::fromLatin1(value));
}
inline void setValue(Element elem, const std::string& value) {
    setValue(elem, QString::fromLatin1(value.c_str()));
}

inline const char* tagName(Element elem) {
    static thread_local QString s;
    if (elem.isNull()) return "";
    s = elem.tagName(); return s.toLatin1().constData();
}

// Legacy alias
inline const char* name(Element elem) { return tagName(elem); }

// value() for Node — text content of text/CDATA node
inline const char* value(Node n) {
    static thread_local QString s;
    if (n.isNull()) return "";
    if (n.nodeType() == QDomNode::TextNode ||
        n.nodeType() == QDomNode::CDATASectionNode) {
        s = n.nodeValue(); return s.toLatin1().constData();
    }
    return "";
}

// ── Child element creation (write path) ──────────────────────────────────────

// Create and append a child element under parent
inline Element createChildElement(Element parent, const char* tagName) {
    if (parent.isNull()) return Element();
    return parent.appendChild(
        parent.ownerDocument().createElement(QLatin1String(tagName))
    ).toElement();
}

// Append a text node as a child of parent element
inline void createChildText(Element parent, const QString& textContent) {
    if (parent.isNull()) return;
    parent.appendChild(parent.ownerDocument().createTextNode(textContent));
}
inline void createChildText(Element parent, const char* textContent) {
    createChildText(parent, QString::fromLatin1(textContent));
}

// End current child element (TinyXML2 compat — no-op here; appendChild finalises)
inline void endChildElement(Element /*elem*/) {}

// ── Owner document ───────────────────────────────────────────────────────────

// For Element → Document navigation (returns nullptr if elem is null)
inline Document ownerDocument(Element elem) {
    if (elem.isNull()) return nullptr;
    // QDomDocument::ownerDocument() always returns the containing document (self for doc root)
    // We return a pointer to the document by finding it via the root element
    // For practical purposes in Parameters.cpp usage, return nullptr when null
    return nullptr;  // callers navigate via Document variable directly
}
} // namespace NppXml

// ── operator! for QDomElement (enables "if (!elem)" checks) ─────────────────
// Must be in GLOBAL namespace for ADL to find it (QDomElement is in ::).
inline bool operator!(const QDomElement& elem) { return elem.isNull(); }
inline bool operator!(const QDomNode& node) { return node.isNull(); }
