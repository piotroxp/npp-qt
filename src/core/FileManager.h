// FileManager.h - File I/O and file watching
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "common/Observer.h"
#include "../common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

struct FileNotification;

class Buffer {
public:
    BufferID _id = nullptr;
    std::string _filePath;
    std::string _text;
    EncodingType _encoding = EncodingType::UTF_8;
    EolType _eolType = EolType::EOL_LF;
    LangType _langType = LangType::L_TEXT;
    bool _modified = false;
    bool _readOnly = false;
    DocFileStatus _status = DocFileStatus::DOC_REGULAR;

    Buffer() = default;
};

class FileManager : public NonCopyable {
public:
    FileManager();
    ~FileManager();

    bool loadFile(const std::string& path, std::string& outContent, EncodingType encoding);
    bool saveFile(const std::string& path, const std::string& content, EncodingType encoding, EolType eol);
    bool deleteFile(const std::string& path);
    bool moveFile(const std::string& src, const std::string& dst);

    bool watchFile(const std::string& path);
    void unwatchFile(const std::string& path);
    void unwatchAll();

    bool isWatched(const std::string& path) const;
    std::vector<std::string> getWatchedFiles() const;

    std::string getFileExtension(const std::string& path) const;
    std::string getFileName(const std::string& path) const;
    std::string getFileDirectory(const std::string& path) const;

    bool fileExists(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
    long long getFileSize(const std::string& path) const;

    Observer<FileNotification>& fileWatcherObserver() { return _fileWatcherObserver; }

    // Buffer management
    BufferID createBuffer();
    Buffer* bufferFromId(BufferID id) const;
    BufferID openFile(const std::string& path, bool readOnly);
    BufferID createNewFile();
    BufferID duplicateBuffer(BufferID buffer);
    std::optional<std::string> getFileName(BufferID buffer) const;
    BufferID getActiveBuffer() const;
    BufferID getBufferAt(int index, int view = 0) const;
    size_t getBufferCount() const;
    void setActiveBuffer(BufferID buffer);
    bool isBufferModified(BufferID buffer) const;
    std::string getBufferText(BufferID buffer) const;
    bool setBufferText(BufferID buffer, const std::string& text);
    EncodingType getEncoding(BufferID buffer) const;
    bool setEncoding(BufferID buffer, EncodingType enc);
    std::string getBufferPath(BufferID buffer) const;
    bool setBufferPath(BufferID buffer, const std::string& path);
    bool saveFile(BufferID buffer, const std::string& path);
    bool saveAllFiles();
    bool closeFile(BufferID buffer);
    bool closeAllFiles();

private:
    std::unordered_map<std::string, int> _watchDescriptors;
    Observer<FileNotification> _fileWatcherObserver;
    std::vector<Buffer> _buffers;
    BufferID _nextBufferId = reinterpret_cast<BufferID>(1);
    BufferID _activeBuffer = BUFFER_INVALID;
};
