// FileManager - file management functionality
#pragma once
#include <QString>
#include <vector>
#include "Buffer.h"

class FileManager final {
public:
    static FileManager& getInstance() {
        static FileManager instance;
        return instance;
    }

    FileManager() = default;
    ~FileManager() = default;

    // Disable copying
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    void init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla);

    BufferID loadFile(const wchar_t* filename, Document doc = static_cast<Document>(NULL), int encoding = -1, 
                      const wchar_t* backupFileName = nullptr, FILETIME fileNameTimestamp = {});
    BufferID newEmptyDocument();
    BufferID newPlaceholderDocument(const wchar_t* missingFilename, int whichOne, const wchar_t* userCreatedSessionName);
    BufferID bufferFromDocument(Document doc, bool isMainEditZone);
    BufferID getBufferFromName(const wchar_t* name);
    BufferID getBufferFromDocument(Document doc);

    bool reloadBuffer(BufferID id);
    bool reloadBufferDeferred(BufferID id);
    SavingStatus saveBuffer(BufferID id, const wchar_t* filename, bool isCopy = false);
    bool backupCurrentBuffer();
    bool deleteBufferBackup(BufferID id);
    bool deleteFile(BufferID id);
    bool moveFile(BufferID id, const wchar_t* newFilename);
    bool createEmptyFile(const wchar_t* path);

    int getFileNameFromBuffer(BufferID id, wchar_t* fn2copy);
    size_t docLength(Buffer* buffer) const;
    void removeHotSpot(Buffer* buffer) const;
    size_t nextUntitledNewNumber() const;

    size_t getNbBuffers() const { return _nbBufs; }
    size_t getNbDirtyBuffers() const;
    int getBufferIndexByID(BufferID id);
    Buffer* getBufferByIndex(size_t index);
    Buffer* getBufferByID(BufferID id);

    void beNotifiedOfBufferChange(Buffer* theBuf, int mask);
    void closeBuffer(BufferID, const ScintillaEditView* identifer);
    void addBufferReference(BufferID id, ScintillaEditView* identifer);
    void setLoadedBufferEncodingAndEol(Buffer* buf, const Utf8_16_Read& UnicodeConvertor, int encoding, EolType bkformat);

    void checkFilesystemChanges(bool bCheckOnlyCurrentBuffer);

    void enableAutoDetectEncoding4Loading() { isAutoDetectEncodingDisabled4Loading = false; }
    void disableAutoDetectEncoding4Loading() { isAutoDetectEncodingDisabled4Loading = true; }

private:
    FileManager() = default;
    
    int detectCodepage(char* buf, size_t len);
    bool loadFileData(Document doc, int64_t fileSize, const wchar_t* filename, char* buffer, 
                      Utf8_16_Read* UnicodeConvertor, struct LoadedFileFormat& fileFormat);
    LangType detectLanguageFromTextBeginning(const unsigned char* data, size_t dataLen);

    bool isAutoDetectEncodingDisabled4Loading = false;
    Notepad_plus* _pNotepadPlus = nullptr;
    ScintillaEditView* _pscratchTilla = nullptr;
    Document _scratchDocDefault = 0;
    std::vector<Buffer*> _buffers;
    BufferID _nextBufferID = 0;
    size_t _nbBufs = 0;
};

#define MainFileManager FileManager::getInstance()