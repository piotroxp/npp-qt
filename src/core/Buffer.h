// Buffer.h - Document buffer for Notepad--Qt
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3

#pragma once

#include "common/Types.h"
#include "common/NonCopyable.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include "common/FileWatcherAdapter.h"
#include <QVector>
#include <QMutex>
#include <string>
#include <vector>
#include <unordered_map>

class ScintillaEditor;
class FileManager;

// ============================================================================
// Position tracking for view-specific cursor/fold state
// ============================================================================
struct Position {
    int _firstVisibleLine = 0;
    int _caretLine = 0;
    int _caretColumn = 0;
    int _offsetX = 0;
    int _scrollWidth = 1;
};

// ============================================================================
// Map position for snapshot/backup system
// ============================================================================
struct MapPosition {
    int _firstVisibleLine = 0;
    int _firstVisibleColumn = 0;
    int _caretLine = 0;
    int _caretColumn = 0;
    int _offsetX = 0;
    int _wrapMode = 0;
};

// ============================================================================
// Main Buffer class - represents a document in the editor
// ============================================================================
class Buffer;
class Application;

class Buffer : public QObject {
    Q_OBJECT

    friend class FileManager;

public:
    // ========================================================================
    // Lifecycle
    // ========================================================================

    // Constructor: creates a buffer with given ID and document
    // type must be either DOC_REGULAR or DOC_UNNAMED
    Buffer(FileManager* pManager, const QString& fileName, bool isLargeFile = false);
    ~Buffer() = default;

    // Disable copying
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // ========================================================================
    // Identification
    // ========================================================================

    BufferID getID() const { return const_cast<Buffer*>(this); }

    // ========================================================================
    // File name/path management
    // ========================================================================

    // Sets the full path and determines language from extension
    void setFileName(const QString& filePath);
    const QString& getFullPathName() const { return _fullPathName; }
    const QString& getFileName() const { return _fileName; }
    const QString& getCompactFileName() const { return _compactFileName; }
    void refreshCompactFileName();

    // Normalize tab name by removing invalid characters
    void normalizeTabName(QString& tabName) const;

    // ========================================================================
    // Recent tracking
    // ========================================================================

    void increaseRecentTag();
    long getRecentTag() const { return _recentTag; }

    // ========================================================================
    // File state checking
    // ========================================================================

    // Check if file has changed on disk (deleted, modified, etc.)
    // Returns true if status changed, false otherwise
    bool checkFileState();

    // Reload file from disk
    void reload();

    // Mark buffer for deferred reload on next activation
    void setDeferredReload();

    bool getNeedReload() const { return _needReloading; }
    void setNeedReload(bool reload) { _needReloading = reload; }

    // ========================================================================
    // Dirty/modified state
    // ========================================================================

    bool isDirty() const { return _isDirty; }
    void setDirty(bool dirty);

    bool isModified() const { return _isModified; }
    void setModifiedStatus(bool isModified) { _isModified = isModified; }

    // Save point state (for undo/redo tracking)
    bool isSavePointDirty() const { return _isSavePointDirty; }
    void setSavePointDirty(bool val) { _isSavePointDirty = val; }

    // Sync state: buffer should always be dirty if unsynchronized with disk
    bool isUnsync() const { return _isUnsync; }
    void setUnsync(bool val) { _isUnsync = val; }

    // Track initial dirty state at load time
    bool isLoadedDirty() const { return _isLoadedDirty; }
    void setLoadedDirty(bool val) { _isLoadedDirty = val; }

    // ========================================================================
    // Read-only state
    // ========================================================================

    bool isReadOnly() const { return _isUserReadOnly || _isFileReadOnly; }
    bool getFileReadOnly() const { return _isFileReadOnly; }
    void setFileReadOnly(bool ro);
    bool getUserReadOnly() const { return _isUserReadOnly; }
    void setUserReadOnly(bool ro);

    // ========================================================================
    // Document type/status
    // ========================================================================

    bool isUntitled() const { return _currentStatus == DocFileStatus::DOC_UNNAMED; }
    bool isFromNetwork() const { return _isFromNetwork; }
    bool isInaccessible() const { return _isInaccessible; }
    void setInaccessibility(bool val) { _isInaccessible = val; }
    DocFileStatus getStatus() const { return _currentStatus; }

    // ========================================================================
    // Encoding
    // ========================================================================

    EncodingType getEncoding() const { return _encoding; }
    void setEncoding(EncodingType encoding);

    // Legacy encoding support (codepage)
    int getLegacyEncoding() const { return _legacyEncoding; }
    void setLegacyEncoding(int encoding);

    // ========================================================================
    // EOL format
    // ========================================================================

    EolType getEolFormat() const { return _eolFormat; }
    void setEolFormat(EolType format);

    // ========================================================================
    // Language/lexer
    // ========================================================================

    LangType getLangType() const { return _langType; }
    void setLangType(LangType lang, const QString& userLangName = QString());

    bool isUserDefineLangExt() const { return !_userLangExt.isEmpty(); }
    const QString& getUserDefineLangName() const { return _userLangExt; }

    bool getNeedsLexing() const { return _needLexing; }
    void setNeedsLexing(bool lex);

    // Mark that language was explicitly set from menu (not auto-detected)
    void langHasBeenSetFromMenu() { _hasLangBeenSetFromMenu = true; }

    // ========================================================================
    // View references (for split views)
    // ========================================================================

    // Add/remove editor references (for split/multi-view support)
    // Returns reference count after operation
    int addReference(ScintillaEditor* editor);
    int removeReference(const ScintillaEditor* editor);

    // Get/set position for a specific editor view
    void setPosition(const Position& pos, const ScintillaEditor* editor);
    Position& getPosition(const ScintillaEditor* editor);

    // Get/set fold state for a specific editor view
    void setHeaderLineState(const std::vector<size_t>& folds, ScintillaEditor* editor);
    const std::vector<size_t>& getHeaderLineState(const ScintillaEditor* editor) const;

    // ========================================================================
    // Line change markers (for VCS integration)
    // ========================================================================

    void setHideLineChanged(bool isHide, size_t location);

    // ========================================================================
    // File timestamp tracking
    // ========================================================================

    void updateTimeStamp();
    QDateTime getLastModifiedFileTimestamp() const { return _timeStamp; }
    QDateTime getLastModifiedTimestamp() const;

    enum class FileTimeType { Created, Modified, Accessed };
    QString getFileTimeString(FileTimeType ftt) const;

    // ========================================================================
    // Document length
    // ========================================================================

    size_t getDocumentLength() const { return _documentLength; }
    void setDocumentLength(size_t len) { _documentLength = len; }

    // Get file length on disk (-1 if not accessible)
    int64_t getFileLength() const;

    // ========================================================================
    // Partial load (large file) tracking
    // ========================================================================

    qint64 totalFileSize() const { return _totalFileSize; }
    void setTotalFileSize(qint64 size) { _totalFileSize = size; }
    bool isPartialLoad() const { return _isPartialLoad; }
    void setPartialLoad(bool v) { _isPartialLoad = v; }

    // ========================================================================
    // Backup system
    // ========================================================================

    const QString& getBackupFileName() const { return _backupFileName; }
    void setBackupFileName(const QString& fileName) { _backupFileName = fileName; }

    QString getTabCreatedTimeString() const { return _tabCreatedTimeString; }
    void setTabCreatedTimeStringFromBackup();
    void setTabCreatedTimeStringWithCurrentTime();

    // ========================================================================
    // Large file handling
    // ========================================================================

    bool isLargeFile() const { return _isLargeFile; }

    // Feature availability based on large file restrictions
    bool allowBraceMatch() const;
    bool allowAutoCompletion() const;
    bool allowSmartHilite() const;
    bool allowClickableLink() const;

    // ========================================================================
    // File monitoring (for external change detection)
    // ========================================================================

    void startMonitoring();
    void stopMonitoring();
    bool isMonitoringOn() const { return _isMonitoringOn; }

    // ========================================================================
    // Map/snapshot position
    // ========================================================================

    void setMapPosition(const MapPosition& mapPos) { _mapPosition = mapPos; }
    MapPosition getMapPosition() const { return _mapPosition; }

    // ========================================================================
    // RTL language support
    // ========================================================================

    bool isRTL() const { return _isRTL; }
    void setRTL(bool rtl) { _isRTL = rtl; }

    // ========================================================================
    // Pinned tab support
    // ========================================================================

    bool isPinned() const { return _isPinned; }
    void setPinned(bool pinned) { _isPinned = pinned; }

    // ========================================================================
    // Document color
    // ========================================================================

    int getDocColorId() const { return _docColorId; }
    void setDocColorId(int id) { _docColorId = id; }

    // ========================================================================
    // Untitled tab naming
    // ========================================================================

    bool isUntitledTabRenamed() const { return _isUntitledTabRenamed; }
    void setUntitledTabRenamedStatus(bool renamed) { _isUntitledTabRenamed = renamed; }

    // ========================================================================
    // Comment symbols (for language)
    // ========================================================================

    QString getCommentLineSymbol() const;
    QString getCommentStart() const;
    QString getCommentEnd() const;

    // ========================================================================
    // Internal document pointer (for Scintilla)
    // ========================================================================

    void* getDocument() const { return _document; }
    void setDocument(void* doc) { _document = doc; }

    // ========================================================================
    // Access editors
    // ========================================================================

    const QVector<ScintillaEditor*>& editors() const { return _referees; }

signals:
    // Content changes
    void contentChanged();
    void dirtyChanged(bool isDirty);
    void encodingChanged(EncodingType encoding);
    void languageChanged(LangType langType);
    void fileReloaded();

    // State changes
    void fileNameChanged(const QString& fullPath);
    void timestampChanged(const QDateTime& timestamp);
    void statusChanged(DocFileStatus status);
    void readonlyChanged(bool isReadOnly);
    void formatChanged(EolType format);
    void lexingNeeded();
    void recentTagChanged(long tag);

    // File system events
    void fileDeleted();
    void fileCreated();
    void fileExternallyModified(const QString& path);

    friend class Application;
private:
    // ========================================================================
    // Private helpers
    // ========================================================================

    int indexOfReference(const ScintillaEditor* editor) const;

    void doNotify(int mask);

    void setStatus(DocFileStatus status);

    bool isNetworkPath(const QString& path) const;

    // ========================================================================
    // Members
    // ========================================================================

    FileManager* _pManager = nullptr;
    bool _canNotify = false;
    int _references = 0;
    void* _document = nullptr;

    // Document properties
    LangType _langType = LangType::L_TEXT;
    QString _userLangExt;
    bool _isDirty = false;
    EolType _eolFormat = EolType::EOL_LF;
    EncodingType _encoding = EncodingType::UTF_8;
    int _legacyEncoding = -1;  // codepage for legacy encoding
    bool _isUserReadOnly = false;
    bool _needLexing = false;
    bool _isUntitledTabRenamed = false;
    size_t _documentLength = 0;

    // View references
    QVector<ScintillaEditor*> _referees;
    QVector<Position> _positions;
    QVector<std::vector<size_t>> _foldStates;

    // Environment properties
    DocFileStatus _currentStatus = DocFileStatus::DOC_REGULAR;
    QDateTime _timeStamp;
    bool _isFileReadOnly = false;
    QString _fullPathName;
    QString _fileName;
    QString _compactFileName;
    bool _needReloading = false;
    QString _tabCreatedTimeString;
    long _recentTag = -1;
    static long _recentTagCtr;

    int _docColorId = -1;

    // Backup system
    QString _backupFileName;
    bool _isModified = false;
    bool _isLoadedDirty = false;
    bool _isUnsync = false;
    bool _isSavePointDirty = false;
    bool _isLargeFile = false;
    qint64 _totalFileSize = 0;
    bool _isPartialLoad = false;

    // File monitoring
    FileWatcherAdapter* _fileWatcher = nullptr;
    bool _isMonitoringOn = false;

    bool _hasLangBeenSetFromMenu = false;

    MapPosition _mapPosition;

    QMutex _reloadGuard;

    bool _isInaccessible = false;
    bool _isFromNetwork = false;
    bool _isRTL = false;
    bool _isPinned = false;
};

// Invalid characters for file names
constexpr const char* INVALID_FILENAME_CHARS = "/\\:*?\"<>|\t\r\n";
