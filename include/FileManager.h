// FileManager.h - Manages file operations and document buffering
#pragma once

#include "Buffer.h"
#include "ScintillaEditView.h"
#include <QString>
#include <QVector>
#include <QMap>
#include <memory>

class FileManager {
public:
    FileManager();
    ~FileManager();
    
    // Buffer management
    BufferID loadFile(const QString& filePath, int encoding = -1);
    bool saveFile(BufferID buffer, const QString& filePath, bool createBackup = false);
    bool reloadFile(BufferID buffer);
    
    // Document management
    BufferID getBufferFromId(int bufferId) const;
    int getIdFromBuffer(BufferID buffer) const;
    
    // Close operations
    bool closeFile(BufferID buffer, bool force = false);
    bool closeAll(bool force = false);
    
    // Dirty state
    bool isDirty(BufferID buffer) const;
    void setDirty(BufferID buffer, bool dirty);
    
    // Internal buffer allocation
    BufferID allocateBuffer();
    void releaseBuffer(BufferID buffer);
    
private:
    int _nextBufferId;
    QMap<BufferID, int> _bufferIdMap;  // reverse map
    QMap<BufferID, std::unique_ptr<Buffer>> _buffers;
    QMap<QString, BufferID> _filePathMap;
    
    BufferID doLoad(const QString& filePath, int encoding);
    BufferID doCreate(const QString& content, const QString& name);
};

using BufferID = int;
#define BUFFER_INVALID (-1)