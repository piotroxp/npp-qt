// Buffer class - holds document state
// Semantic lift: Buffer.h/Buffer.cpp from Notepad++

#pragma once

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QFileInfo>

enum class BufferStatus {
    Normal = 0,
    ReadOnly = 1,
    Dirty = 2,
    Untitled = 4
};

// BufferStatus bitwise operators for enum class
inline BufferStatus operator&(BufferStatus a, BufferStatus b) { return static_cast<BufferStatus>(static_cast<int>(a) & static_cast<int>(b)); }
inline BufferStatus operator|(BufferStatus a, BufferStatus b) { return static_cast<BufferStatus>(static_cast<int>(a) | static_cast<int>(b)); }
inline BufferStatus operator~(BufferStatus a) { return static_cast<BufferStatus>(~static_cast<int>(a)); }
inline BufferStatus& operator|=(BufferStatus& a, BufferStatus b) { a = a | b; return a; }
inline BufferStatus& operator&=(BufferStatus& a, BufferStatus b) { a = a & b; return a; }

enum class EolFormat {
    Windows = 0,
    Unix = 1,
    Mac = 2
};

enum class Encoding {
    UTF8 = 0,
    UTF8_BOM = 1,
    UTF16_LE = 2,
    UTF16_BE = 3,
    ASCII = 4,
    CP1252 = 5
};

class Buffer : public QObject
{
    Q_OBJECT

public:
    Buffer(QObject* parent = nullptr);
    ~Buffer() override;

    void setContent(const QString& content);
    QString content() const { return _content; }

    void setFilePath(const QString& path);
    QString filePath() const { return _filePath; }
    bool isUntitled() const { return _filePath.isEmpty(); }

    bool isDirty() const { return static_cast<bool>(_status & BufferStatus::Dirty); }
    void setDirty(bool dirty);
    
    bool isReadOnly() const { return static_cast<bool>(_status & BufferStatus::ReadOnly); }
    void setReadOnly(bool readOnly);

    bool isPinned() const { return _isPinned; }
    void setPinned(bool pinned) { _isPinned = pinned; }

    Encoding encoding() const { return _encoding; }
    void setEncoding(Encoding enc) { _encoding = enc; }

    EolFormat eolFormat() const { return _eolFormat; }
    void setEolFormat(EolFormat format) { _eolFormat = format; }

    QDateTime lastModified() const { return _lastModified; }
    void updateLastModified();

    int id() const { return _id; }

    QString language() const { return _language; }
    void setLanguage(const QString& lang) { _language = lang; }

signals:
    void contentChanged();
    void statusChanged();
    void pathChanged(const QString& oldPath, const QString& newPath);

private:
    int _id;
    QString _content;
    QString _filePath;
    BufferStatus _status = BufferStatus::Normal;
    bool _isPinned = false;
    Encoding _encoding = Encoding::UTF8;
    EolFormat _eolFormat = EolFormat::Windows;
    QDateTime _lastModified;
    QString _language;

    static int _nextId;
};

inline Buffer::Buffer(QObject* parent)
    : QObject(parent)
    , _id(++_nextId)
{
}

inline Buffer::~Buffer() = default;

inline void Buffer::setContent(const QString& content)
{
    if (_content != content) {
        _content = content;
        _status |= BufferStatus::Dirty;
        emit contentChanged();
    }
}

inline void Buffer::setFilePath(const QString& path)
{
    QString oldPath = _filePath;
    _filePath = path;
    if (!path.isEmpty()) {
        QFileInfo fi(path);
        _lastModified = fi.lastModified();
    }
    if (oldPath != path) {
        emit pathChanged(oldPath, path);
    }
}

inline void Buffer::setDirty(bool dirty)
{
    if (dirty) {
        _status |= BufferStatus::Dirty;
    } else {
        _status &= ~BufferStatus::Dirty;
    }
    emit statusChanged();
}

inline void Buffer::setReadOnly(bool readOnly)
{
    if (readOnly) {
        _status |= BufferStatus::ReadOnly;
    } else {
        _status &= ~BufferStatus::ReadOnly;
    }
    emit statusChanged();
}

inline void Buffer::updateLastModified()
{
    if (!_filePath.isEmpty()) {
        QFileInfo fi(_filePath);
        _lastModified = fi.lastModified();
    }
}

