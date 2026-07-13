// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QFileInfo>
#include <QMutex>
#include <QWaitCondition>
#include <memory>
#include <vector>
#include <string>

// LangType, EolType, etc. shared across all npp-qt files
#include "NppConstants.h"
#include "ScintillaComponent.h"  // defines ScintillaComponent + using ScintillaEditView = ScintillaComponent

// Forward declarations — break circular include with ScintillaComponent.h
// ScintillaEditView is available via 'using ScintillaEditView = ScintillaComponent;' in ScintillaComponent.h
class Notepad_plus;
class NppParameters;
class Utf8_16_Read;
class Utf8_16_Write;
class FileManager;

// Scintilla Document type (intptr_t from Scintilla.h)
using Document = intptr_t;

class BufferID {
public:
    BufferID() = default;
    BufferID(void* p) : _p(p) {}
    void* _p = nullptr;
    bool operator==(const BufferID& o) const { return _p == o._p; }
    bool operator!=(const BufferID& o) const { return _p != o._p; }
    // get() returns the raw pointer — caller is responsible for null checks.
    // This avoids needing the full Buffer definition in the header.
    void* get() const { return _p; }
    explicit operator bool() const { return _p != nullptr; }
};

// std::hash specialization so BufferID can be used in unordered_set/unordered_map
namespace std {
    template<> struct hash<BufferID> {
        size_t operator()(BufferID id) const noexcept {
            return std::hash<void*>{}(id.get());
        }
    };
}
// Note: BUFFER_INVALID is defined as constexpr in ScintillaComponent.h

// BufferViewInfo — lightweight pair of (buffer, view) used by file-close operations
struct BufferViewInfo {
    BufferID _buffer;
    int _view;

    BufferViewInfo() : _buffer(), _view(0) {}
    BufferViewInfo(BufferID buffer, int view) : _buffer(buffer), _view(view) {}
};

enum class BufferStatus : unsigned { Normal = 0, ReadOnly = 1, Dirty = 2, Untitled = 4 };

enum class EolFormat { Windows = 0, Unix = 1, Mac = 2 };

enum class Encoding { UTF8 = 0, UTF8_BOM = 1, UTF16_LE = 2, UTF16_BE = 3, ASCII = 4, CP1252 = 5 };

enum class DocFileStatus { DOC_REGULAR = 0x01, DOC_UNNAMED = 0x02, DOC_DELETED = 0x04,
    DOC_MODIFIED = 0x08, DOC_NEEDRELOAD = 0x10, DOC_INACCESSIBLE = 0x20 };

// LangType is defined in NppConstants.h (included above)


enum class UniMode {
    uni8Bit = 0, uniUTF8 = 1, uni16BE = 2, uni16LE = 3,
    uniCookie = 4, uniUTF8_NoBOM = 5, uni16BE_NoBOM = 6, uni16LE_NoBOM = 7,
    // Aliases for code that uses bare identifiers
    uni7Bit = uni8Bit
};

// NOTE: Do NOT add `using enum UniMode;` here — NppConstants.h redefines
// uniUTF8_NoBOM/uniCookie as inline constexpr int and including both
// would cause conflicts. Files that need bare enum values should:
//   1. include Buffer.h, and
//   2. add their own `using enum UniMode;` in a local scope, OR
//   3. use UniMode::uni8Bit etc. (preferred)
// Bring enum values into scope ONLY within Buffer.h's implementation block.

// Buffer change notification masks
constexpr int BufferChangeNone       = 0x000;
constexpr int BufferChangeLanguage   = 0x001;
constexpr int BufferChangeDirty     = 0x002;
constexpr int BufferChangeFormat     = 0x004;
constexpr int BufferChangeUnicode    = 0x008;
constexpr int BufferChangeReadonly  = 0x010;
constexpr int BufferChangeStatus    = 0x020;
constexpr int BufferChangeTimestamp  = 0x040;
constexpr int BufferChangeFilename  = 0x080;
constexpr int BufferChangeRecentTag = 0x100;
constexpr int BufferChangeLexing    = 0x200;
constexpr int BufferChangeMask      = 0x3FF;

const wchar_t UNTITLED_STR[] = L"new ";

struct Position {
    intptr_t _position = 0;
    int _scrollWidth = 1;
    int _firstVisibleLine = 0;
    int _offset = 0;
    int _selectionStart = 0;
    int _selectionEnd = 0;
    int _wrapMode = 0;
};

// Map position structure — shared between Buffer and DocumentMap
// Used for minimap / document map scrolling sync
struct MapPosition {
    int _firstVisibleLine = 0;
    int _wrapColumn = 0;
    int _height = -1;   // visible line height in pixels (-1 = unchanged)
    int _width = -1;    // visible column width (-1 = unchanged)
    int _iView = 0;     // view index (0 = main, 1 = sub)
    bool _isWrap = false;
    int  _wrapIndentMode = 0;
    int  _firstVisibleDisplayLine = 0;
    int  _firstVisibleDocLine = 0;
    int  _nbLine = 0;
    int  _lastVisibleDocLine = 0;
    int  _higherPos = -1;    // scroll position marker
    int  _KByteInDoc = 512;  // max peek length in KB

    MapPosition() : _firstVisibleLine(0), _wrapColumn(0), _height(-1), _width(-1), _iView(0), _isWrap(false), _wrapIndentMode(0), _firstVisibleDisplayLine(0), _firstVisibleDocLine(0), _nbLine(0), _lastVisibleDocLine(0), _higherPos(-1), _KByteInDoc(512) {}
    MapPosition(int line, int height, int width, int iView)
        : _firstVisibleLine(line), _wrapColumn(0), _height(height), _width(width), _iView(iView), _isWrap(false), _wrapIndentMode(0), _firstVisibleDisplayLine(0), _firstVisibleDocLine(0), _nbLine(0), _lastVisibleDocLine(0), _higherPos(-1), _KByteInDoc(512) {}

    bool isValid() const { return (_firstVisibleDisplayLine != -1); }
};

class Buffer : public QObject {
    Q_OBJECT
    friend class FileManager;
public:
    Buffer(FileManager* pManager, BufferID id, Document doc, DocFileStatus type,
           const wchar_t* fileName, bool isLargeFile);
    Buffer();  // default constructor for simple in-memory buffers
    ~Buffer() override;

    void setFileName(const wchar_t* fn);
    const wchar_t* getFullPathName() const { return _fullPathName.c_str(); }
    const wchar_t* getFileName() const { return _fileName; }
    const wchar_t* getCompactFileName() const { return _compactFileName.c_str(); }
    void refreshCompactFileName();
    void normalizeTabName(std::wstring& tabName);
    BufferID getID() const { return _id; }
    void increaseRecentTag();
    long getRecentTag() const { return _recentTag; }

    bool checkFileState();
    bool isDirty() const { return _isDirty; }
    bool isReadOnly() const { return _isUserReadOnly || _isFileReadOnly; }
    bool isUntitled() const { return int(_currentStatus) & int(DocFileStatus::DOC_UNNAMED); }
    bool isFromNetwork() const { return _isFromNetwork; }
    bool isInaccessible() const { return _isInaccessible; }
    void setInaccessibility(bool val) { _isInaccessible = val; }
    bool getFileReadOnly() const { return _isFileReadOnly; }
    void setFileReadOnly(bool ro);
    bool getUserReadOnly() const { return _isUserReadOnly; }
    void setUserReadOnly(bool ro);
    EolFormat getEolFormat() const { return _eolFormat; }
    void setEolFormat(EolFormat format);
    LangType getLangType() const { return _lang; }
    // Detect language from a file extension (e.g. L".cpp" → L_CPP)
    // Exposed publicly so MainWindow can apply lexer on file open.
    static LangType langFromExtension(const std::wstring& ext);  // public API
    // Returns the language name as a QString for Scintilla lexer lookup
    QString language() const {
        // Return language name for Scintilla lexer lookup.
        // Used by styleToLexerNameMap() in ScintillaComponent.cpp to find the QsciLexer.
        // The actual mapping from LangType → lexer is handled by createLexerForLanguage().
        // Return empty — the lexer is determined by the LangType enum directly.
        return QString();
    }
    const wchar_t* getCommentStart() const { return L""; }
    const wchar_t* getCommentEnd() const { return L""; }
    const wchar_t* getCommentLineSymbol() const { return L""; }
    void setLangType(LangType lang, const wchar_t* userLangName = L"");
    UniMode getUnicodeMode() const { return _unicodeMode; }
    void setUnicodeMode(UniMode mode);
    int getEncoding() const { return _encoding; }
    void setEncoding(int encoding);
    DocFileStatus getStatus() const { return _currentStatus; }
    Document getDocument() { return _doc; }
    void setDirty(bool dirty);
    void setDirty() { _isDirty = true; }

    // Qt-friendly file path access (supplementary to the wchar_t* API)
    void setFilePath(const QString& path);
    QString filePath() const;

    // In-memory text content (supplementary to Scintilla Document API)
    void setContent(const QString& content);
    QString content() const { return _textContent; }

    void setPosition(const Position& pos, const ScintillaEditView* identifier);
    Position& getPosition(const ScintillaEditView* identifier);
    void setHeaderLineState(const std::vector<size_t>& folds, ScintillaEditView* identifier);
    const std::vector<size_t>& getHeaderLineState(const ScintillaEditView* identifier) const;
    bool isUserDefineLangExt() const { return !_userLangExt.empty(); }
    const wchar_t* getUserDefineLangName() const { return _userLangExt.c_str(); }
    bool getNeedsLexing() const { return _needLexer; }
    void setNeedsLexing(bool lex);
    bool isUntitledTabRenamed() const { return _isUntitledTabRenamed; }
    void setUntitledTabRenamedStatus(bool isRenamed) { _isUntitledTabRenamed = isRenamed; }
    int addReference(ScintillaEditView* identifier);
    int removeReference(const ScintillaEditView* identifier);
    void setHideLineChanged(bool isHide, size_t location);
    void setDeferredReload();
    bool getNeedReload() const { return _needReloading; }
    void setNeedReload(bool reload) { _needReloading = reload; }
    std::wstring tabCreatedTimeString() const { return _tabCreatedTimeString; }
    void setTabCreatedTimeStringFromBakFile();
    void setTabCreatedTimeStringWithCurrentTime();
    size_t docLength() const;
    int64_t getFileLength() const;
    enum fileTimeType { ft_created, ft_modified, ft_accessed };
    std::wstring getFileTime(fileTimeType ftt) const;
    bool isLoadedDirty() const { return _isLoadedDirty; }
    void setLoadedDirty(bool val) { _isLoadedDirty = val; }
    bool isUnsync() const { return _isUnsync; }
    void setUnsync(bool val) { _isUnsync = val; }
    bool isSavePointDirty() const { return _isSavePointDirty; }
    void setSavePointDirty(bool val) { _isSavePointDirty = val; }
    bool isLargeFile() const { return _isLargeFile; }
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoringOn() const { return _isMonitoringOn; }
    void updateTimeStamp();
    void reload();
    void setMapPosition(const MapPosition& mp) { _mapPosition = mp; }
    MapPosition getMapPosition() const { return _mapPosition; }
    void langHasBeenSetFromMenu() { _hasLangBeenSetFromMenu = true; }
    bool allowBraceMach() const;
    bool allowAutoCompletion() const;
    bool allowSmartHilite() const;
    bool allowClickableLink() const;
    void setDocColorId(int idx) { _docColorId = idx; }
    int getDocColorId() const { return _docColorId; }
    bool isRTL() const { return _isRTL; }
    void setRTL(bool rtl) { _isRTL = rtl; }
    bool isPinned() const { return _isPinned; }
    void setPinned(bool pinned) { _isPinned = pinned; }
    std::wstring getBackupFileName() const { return _backupFileName; }

    // getLastModifiedFileTimestamp — returns last modified time as wstring
    // Used in session save/restore
    QString getLastModifiedFileTimestamp() const {
        QFileInfo fi(QString::fromWCharArray(_fullPathName.c_str()));
        if (fi.exists()) {
            return fi.lastModified().toString(Qt::ISODate);
        }
        return QString();
    }
    void setBackupFileName(const std::wstring& fn) { _backupFileName = fn; }

    // File monitoring stub (Win32: returns event handle for WaitForMultipleObjects).
    // In Qt6, monitoring is driven by QFileSystemWatcher signals instead.
    void* getMonitoringEvent() const { return nullptr; }

private:
    int indexOfReference(const ScintillaEditView* identifier) const;
    void setStatus(DocFileStatus status) { _currentStatus = status; doNotify(BufferChangeStatus); }
    void doNotify(int mask);

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    FileManager* _pManager = nullptr;
    bool _canNotify = false;
    int _references = 0;
    BufferID _id = 0;  // BUFFER_INVALID is 0
    Document _doc = 0;
    LangType _lang = LangType::L_TEXT;
    std::wstring _userLangExt;
    bool _isDirty = false;
    EolFormat _eolFormat = EolFormat::Windows;
    int _encoding = -1;
    UniMode _unicodeMode = UniMode::uniUTF8;
    bool _isUserReadOnly = false;
    bool _isFromNetwork = false;
    bool _needLexer = false;
    bool _isUntitledTabRenamed = false;
    std::vector<ScintillaEditView*> _referees;
    std::vector<Position> _positions;
    std::vector<std::vector<size_t>> _foldStates;
    DocFileStatus _currentStatus = DocFileStatus::DOC_REGULAR;
    QDateTime _timeStamp;
    bool _isFileReadOnly = false;
    std::wstring _fullPathName;
    wchar_t* _fileName = nullptr;
    std::wstring _compactFileName;
    QString _textContent;   // in-memory text for new/unsaved buffers
    bool _needReloading = false;
    std::wstring _tabCreatedTimeString;
    long _recentTag = -1;
    static long _recentTagCtr;
    int _docColorId = -1;
    std::wstring _backupFileName;
    bool _isModified = false;
    bool _isLoadedDirty = false;
    bool _isUnsync = false;
    bool _isSavePointDirty = false;
    bool _isLargeFile = false;
    QMutex _monitorMutex;
    QWaitCondition _monitorCond;
    bool _isMonitoringOn = false;
    bool _hasLangBeenSetFromMenu = false;
    MapPosition _mapPosition;
    QMutex _reloadGuard;
    bool _isInaccessible = false;
    bool _isRTL = false;
    bool _isPinned = false;
};

// FileManager's status codes — defined here so FileManager methods can use it
enum QtSavingStatus { QtSaveOK = 0, QtSaveOpenFailed = 1, QtSaveWritingFailed = 2, QtNotEnoughRoom = 3, QtReadOnlyForbidden = 4 };

class FileManager final {
public:
    void init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla);
    void checkFilesystemChanges(bool bCheckOnlyCurrentBuffer);
    size_t getNbBuffers() const { return _nbBufs; }
    size_t getNbDirtyBuffers() const;
    int getBufferIndexByID(BufferID id);
    Buffer* getBufferByIndex(size_t index);
    Buffer* getBufferByID(BufferID id);
    void beNotifiedOfBufferChange(Buffer* theBuf, int mask);
    void closeBuffer(BufferID, const ScintillaEditView* identifer);
    void addBufferReference(BufferID id, ScintillaEditView* identifer);
    BufferID loadFile(const wchar_t* filename, Document doc = 0, int encoding = -1, const wchar_t* backupFileName = nullptr);
    BufferID newEmptyDocument();
    BufferID newPlaceholderDocument(const wchar_t* missingFilename, int whichOne, const wchar_t* userCreatedSessionName);
    BufferID bufferFromDocument(Document doc, bool isMainEditZone);
    BufferID getBufferFromName(const wchar_t* name);
    BufferID getBufferFromDocument(Document doc);
    void setLoadedBufferEncodingAndEol(Buffer* buf, const Utf8_16_Read& UnicodeConvertor, int encoding, int bkformat);
    bool reloadBuffer(BufferID id);
    bool reloadBufferDeferred(BufferID id);
    QtSavingStatus saveBuffer(BufferID id, const wchar_t* filename, bool isCopy = false);
    bool backupCurrentBuffer();
    bool deleteBufferBackup(BufferID id);
    bool deleteFile(BufferID id);
    bool moveFile(BufferID id, const wchar_t* newFileName);
    bool createEmptyFile(const wchar_t* path);
    FileManager();
    static FileManager& getInstance() { static FileManager instance; return instance; }
    int getFileNameFromBuffer(BufferID id, wchar_t* fn2copy);
    size_t docLength(Buffer* buffer) const;
    void removeHotSpot(Buffer* buffer) const;
    size_t nextUntitledNewNumber() const;
    void enableAutoDetectEncoding4Loading() { _isAutoDetectEncodingDisabled4Loading = false; }
    void disableAutoDetectEncoding4Loading() { _isAutoDetectEncodingDisabled4Loading = true; }

private:
    struct LoadedFileFormat { LangType _language = LangType::L_TEXT; int _encoding = 0; int _eolFormat = 0; };
    ~FileManager();
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    int detectCodepage(char* buf, size_t len);
    bool loadFileData(Document doc, int64_t fileSize, const wchar_t* filename, char* buffer, Utf8_16_Read* UnicodeConvertor, LoadedFileFormat& fileFormat);
    LangType detectLanguageFromTextBeginning(const unsigned char* data, size_t dataLen);

    bool _isAutoDetectEncodingDisabled4Loading = false;
    Notepad_plus* _pNotepadPlus = nullptr;
    ScintillaEditView* _pscratchTilla = nullptr;
    Document _scratchDocDefault = 0;
    std::vector<Buffer*> _buffers;
    BufferID _nextBufferID{nullptr};
    size_t _nbBufs = 0;
};

#define MainFileManager FileManager::getInstance()
