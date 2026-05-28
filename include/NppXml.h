// NppXml.h - Qt XML wrapper for notepad-plus configuration
#pragma once

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <memory>

class NppXml {
public:
    NppXml() : _reader(nullptr), _writer(nullptr) {}
    ~NppXml() { close(); }

    bool load(const QString& filePath) {
        close();
        _file.reset(new QFile(filePath));
        if (!_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            _file.reset();
            return false;
        }
        _reader.reset(new QXmlStreamReader(_file.get()));
        return true;
    }

    bool save(const QString& filePath) {
        _file.reset(new QFile(filePath));
        if (!_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
            _file.reset();
            return false;
        }
        _writer.reset(new QXmlStreamWriter(_file.get()));
        _writer->writeStartDocument();
        return true;
    }

    void close() {
        if (_writer) {
            _writer->writeEndDocument();
            _writer.reset();
        }
        if (_reader) {
            _reader.reset();
        }
        if (_file) {
            _file->close();
            _file.reset();
        }
    }

    // Element navigation
    bool readNext() {
        if (!_reader) return false;
        return _reader->readNextStartElement();
    }

    QString name() const {
        if (!_reader) return QString();
        return _reader->name().toString();
    }

    QString readElementText() {
        if (!_reader) return QString();
        return _reader->readElementText();
    }

    bool hasElement(const QString& name) const {
        if (!_reader) return false;
        // Read ahead to find element, return to beginning when done
        // Simplified - just skip forward to first occurrence
        while (!_reader->atEnd()) {
            _reader->readNext();
            if (_reader->tokenType() == QXmlStreamReader::StartElement &&
                _reader->name().toString() == name) {
                return true;
            }
        }
        return false;
    }

    // Write operations
    void writeStartElement(const QString& name) {
        if (_writer) _writer->writeStartElement(name);
    }

    void writeAttribute(const QString& name, const QString& value) {
        if (_writer) _writer->writeAttribute(name, value);
    }

    void writeCharacters(const QString& text) {
        if (_writer) _writer->writeCharacters(text);
    }

    void writeEndElement() {
        if (_writer) _writer->writeEndElement();
    }

    // Root element helpers
    void writeDocType(const QString& rootName, const QString& pubId, 
                    const QString& sysId, const QString& subset) {
        if (_writer) _writer->writeDTD("<!DOCTYPE " + rootName + ">");
    }

    // Static factory
    static std::unique_ptr<NppXml> create() {
        return std::make_unique<NppXml>();
    }

private:
    std::unique_ptr<QFile> _file;
    std::unique_ptr<QXmlStreamReader> _reader;
    std::unique_ptr<QXmlStreamWriter> _writer;
};

using NppXmlPtr = std::unique_ptr<NppXml>;