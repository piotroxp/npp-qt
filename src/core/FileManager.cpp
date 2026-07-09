// FileManager.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FileManager.h"
#include "../common/Util.h"
#include <fstream>
#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

FileManager::FileManager() = default;
FileManager::~FileManager() { unwatchAll(); }

bool FileManager::loadFile(const std::string& path, std::string& outContent, EncodingType encoding) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs) return false;
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string bytes(size, '\0');
    if (!ifs.read(bytes.data(), size)) return false;
    outContent = bytes;
    return true;
}

bool FileManager::saveFile(const std::string& path, const std::string& content, EncodingType encoding, EolType eol) {
    (void)eol;
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) return false;
    ofs.write(content.data(), content.size());
    return ofs.good();
}

bool FileManager::deleteFile(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

bool FileManager::moveFile(const std::string& src, const std::string& dst) {
    return fs::copy_file(src, dst) && std::remove(src.c_str()) == 0;
}

bool FileManager::watchFile(const std::string& path) {
    _watchDescriptors[path] = 1;
    return true;
}

void FileManager::unwatchFile(const std::string& path) {
    _watchDescriptors.erase(path);
}

void FileManager::unwatchAll() {
    _watchDescriptors.clear();
}

bool FileManager::isWatched(const std::string& path) const {
    return _watchDescriptors.find(path) != _watchDescriptors.end();
}

std::vector<std::string> FileManager::getWatchedFiles() const {
    std::vector<std::string> files;
    for (auto& [f, _] : _watchDescriptors) files.push_back(f);
    return files;
}

std::string FileManager::getFileExtension(const std::string& path) const {
    return fs::path(path).extension().string();
}

std::string FileManager::getFileName(const std::string& path) const {
    return fs::path(path).filename().string();
}

std::string FileManager::getFileDirectory(const std::string& path) const {
    return fs::path(path).parent_path().string();
}

bool FileManager::fileExists(const std::string& path) const {
    return fs::exists(path);
}

bool FileManager::isDirectory(const std::string& path) const {
    return fs::is_directory(path);
}

long long FileManager::getFileSize(const std::string& path) const {
    return fs::file_size(path);
}

// Buffer management
BufferID FileManager::createBuffer() {
    Buffer buf;
    buf._id = _nextBufferId++;
    _buffers.push_back(buf);
    return buf._id;
}

Buffer* FileManager::bufferFromId(BufferID id) const {
    for (auto& b : _buffers) {
        if (b._id == id) return const_cast<Buffer*>(&b);
    }
    return nullptr;
}

BufferID FileManager::openFile(const std::string& path, bool readOnly) {
    Buffer buf;
    buf._id = _nextBufferId++;
    buf._filePath = path;
    buf._readOnly = readOnly;
    std::string content;
    if (loadFile(path, content, EncodingType::UTF_8)) {
        buf._text = content;
    }
    _buffers.push_back(buf);
    return buf._id;
}

BufferID FileManager::createNewFile() {
    return createBuffer();
}

BufferID FileManager::duplicateBuffer(BufferID buffer) {
    Buffer* src = bufferFromId(buffer);
    if (!src) return BUFFER_INVALID;
    Buffer dup;
    dup._id = _nextBufferId++;
    dup._text = src->_text;
    dup._encoding = src->_encoding;
    dup._eolType = src->_eolType;
    dup._langType = src->_langType;
    dup._modified = true;
    _buffers.push_back(dup);
    return dup._id;
}

std::optional<std::string> FileManager::getFileName(BufferID buffer) const {
    Buffer* buf = const_cast<FileManager*>(this)->bufferFromId(buffer);
    if (!buf) return std::nullopt;
    return fs::path(buf->_filePath).filename().string();
}

BufferID FileManager::getActiveBuffer() const { return _activeBuffer; }

BufferID FileManager::getBufferAt(int index, int) const {
    if (index < 0 || index >= (int)_buffers.size()) return BUFFER_INVALID;
    return _buffers[index]._id;
}

size_t FileManager::getBufferCount() const {
    return _buffers.size();
}

void FileManager::setActiveBuffer(BufferID buffer) { _activeBuffer = buffer; }

bool FileManager::isBufferModified(BufferID buffer) const {
    Buffer* buf = const_cast<FileManager*>(this)->bufferFromId(buffer);
    return buf ? buf->_modified : false;
}

std::string FileManager::getBufferText(BufferID buffer) const {
    Buffer* buf = const_cast<FileManager*>(this)->bufferFromId(buffer);
    return buf ? buf->_text : "";
}

bool FileManager::setBufferText(BufferID buffer, const std::string& text) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf) return false;
    buf->_text = text;
    buf->_modified = true;
    return true;
}

EncodingType FileManager::getEncoding(BufferID buffer) const {
    Buffer* buf = const_cast<FileManager*>(this)->bufferFromId(buffer);
    return buf ? buf->_encoding : EncodingType::UTF_8;
}

bool FileManager::setEncoding(BufferID buffer, EncodingType enc) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf) return false;
    buf->_encoding = enc;
    return true;
}

std::string FileManager::getBufferPath(BufferID buffer) const {
    Buffer* buf = const_cast<FileManager*>(this)->bufferFromId(buffer);
    return buf ? buf->_filePath : "";
}

bool FileManager::setBufferPath(BufferID buffer, const std::string& path) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf) return false;
    buf->_filePath = path;
    return true;
}


bool FileManager::saveFile(BufferID buffer, const std::string& path) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf) return false;
    std::string savePath = path.empty() ? buf->_filePath : path;
    if (savePath.empty()) return false;
    return saveFile(savePath, buf->_text, buf->_encoding, buf->_eolType);
}

bool FileManager::saveAllFiles() {
    for (auto& b : _buffers) {
        if (b._modified && !b._filePath.empty()) {
            if (!saveFile(b._id, b._filePath)) return false;
        }
    }
    return true;
}

bool FileManager::closeFile(BufferID buffer) {
    for (auto it = _buffers.begin(); it != _buffers.end(); ++it) {
        if (it->_id == buffer) {
            _buffers.erase(it);
            return true;
        }
    }
    return false;
}

bool FileManager::closeAllFiles() {
    _buffers.clear();
    return true;
}
