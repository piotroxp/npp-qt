// npp-qt: Buffer header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/Buffer.h
// Target: npp-qt/src/ScintillaComponent/Buffer.h

#pragma once

#include <QMutex>
#include <QString>
#include <QVector>
#include <QFileInfo>
#include <vector>
#include <string>

// Forward declaration
class Utf8_16_Read;

// Document file status flags (Win32 DOC_* → Qt6 equivalents)
enum DocFileStatus {
	DOC_REGULAR      = 0x01,
	DOC_UNNAMED      = 0x02,
	DOC_DELETED      = 0x04,
	DOC_MODIFIED     = 0x08,
	DOC_NEEDRELOAD   = 0x10,
	DOC_INACCESSIBLE = 0x20
};

// Buffer change notification masks
enum BufferStatusInfo {
	BufferChangeNone       = 0x000,
	BufferChangeLanguage   = 0x001,
	BufferChangeDirty      = 0x002,
	BufferChangeFormat     = 0x004,
	BufferChangeUnicode    = 0x008,
	BufferChangeReadonly   = 0x010,
	BufferChangeStatus     = 0x020,
	BufferChangeTimestamp  = 0x040,
	BufferChangeFilename  = 0x080,
	BufferChangeRecentTag  = 0x100,
	BufferChangeLexing    = 0x200,
	BufferChangeMask      = 0x3FF
};

// Save operation result
enum SavingStatus {
	SaveOK                      = 0,
	SaveOpenFailed              = 1,
	SaveWritingFailed           = 2,
	NotEnoughRoom               = 3,
	FullReadOnlySavingForbidden = 4
};

struct BufferViewInfo {
	BufferID _bufID = nullptr;
	int _iView = 0;
	BufferViewInfo() = delete;
	BufferViewInfo(BufferID buf, int view) : _bufID(buf), _iView(view) {}
};

const wchar_t UNTITLED_STR[] = L"new ";

// File manager — manages all document buffers
// Win32: FileManager with CreateFile/ReadFile/WriteFile
// Qt6: FileManager with QFile/QTextStream
class FileManager final {
public:
	void init(void* pNotepadPlus, ScintillaComponent* pscratchTilla);

	void checkFilesystemChanges(bool bCheckOnlyCurrentBuffer);

	size_t getNbBuffers() const { return _nbBufs; }
	size_t getNbDirtyBuffers() const;
	int getBufferIndexByID(BufferID id);
	Buffer* getBufferByIndex(size_t index);
	Buffer* getBufferByID(BufferID id) { return id; }

	void beNotifiedOfBufferChange(Buffer* theBuf, int mask);

	void closeBuffer(BufferID, const ScintillaComponent* identifier);

	void addBufferReference(BufferID id, ScintillaComponent* identifier);

	// Load file — Win32 CreateFile/ReadFile → Qt QFile/QTextStream
	BufferID loadFile(const wchar_t* filename, Document doc = static_cast<Document>(nullptr),
	                  int encoding = -1, const wchar_t* backupFileName = nullptr,
	                  QFileInfo fileTimestamp = {});

	BufferID newEmptyDocument();
	BufferID newPlaceholderDocument(const wchar_t* missingFilename, int whichOne,
	                                const wchar_t* userCreatedSessionName);

	BufferID bufferFromDocument(Document doc, bool isMainEditZone);

	BufferID getBufferFromName(const wchar_t* name);
	BufferID getBufferFromDocument(Document doc);

	void setLoadedBufferEncodingAndEol(Buffer* buf, int encoding, EolType bkformat);
	bool reloadBuffer(BufferID id);
	bool reloadBufferDeferred(BufferID id);
	SavingStatus saveBuffer(BufferID id, const wchar_t* filename, bool isCopy = false);
	bool backupCurrentBuffer();
	bool deleteBufferBackup(BufferID id);
	bool deleteFile(BufferID id);
	bool moveFile(BufferID id, const wchar_t* newFilename);
	bool createEmptyFile(const wchar_t* path);

	static FileManager& getInstance() {
		static FileManager instance;
		return instance;
	}

	int getFileNameFromBuffer(BufferID id, wchar_t* fn2copy);
	size_t docLength(Buffer* buffer) const;
	void removeHotSpot(Buffer* buffer) const;
	size_t nextUntitledNewNumber() const;

	void enableAutoDetectEncoding4Loading() { isAutoDetectEncodingDisabled4Loading = false; }
	void disableAutoDetectEncoding4Loading() { isAutoDetectEncodingDisabled4Loading = true; }

private:
	struct LoadedFileFormat {
		LoadedFileFormat() = default;
		LangType _language = LangType::L_TEXT;
		int _encoding = uni8Bit;
		EolType _eolFormat = EolType::osdefault;
	};

	FileManager() = default;
	~FileManager();

	FileManager(const FileManager&) = delete;
	FileManager& operator=(const FileManager&) = delete;
	FileManager(FileManager&&) = delete;
	FileManager& operator=(FileManager&&) = delete;

	int detectCodepage(char* buf, size_t len);
	bool isAutoDetectEncodingDisabled4Loading = false;

	// Win32: loadFileData uses CreateFile/ReadFile → Qt: QFile
	bool loadFileData(Document doc, int64_t fileSize, const wchar_t* filename,
	                  char* buffer, int encoding, LoadedFileFormat& fileFormat);
	LangType detectLanguageFromTextBeginning(const unsigned char* data, size_t dataLen);

	void* _pNotepadPlus = nullptr;
	ScintillaComponent* _pscratchTilla = nullptr;
	Document _scratchDocDefault = 0;
	std::vector<Buffer*> _buffers;
	BufferID _nextBufferID = nullptr;
	size_t _nbBufs = 0;
};

#define MainFileManager FileManager::getInstance()

// Buffer — represents a single document
class Buffer final {
	friend class FileManager;
public:
	// Constructor: Win32 CreateFile/GetFileAttributes → Qt QFile/QFileInfo
	Buffer(FileManager* pManager, BufferID id, Document doc, DocFileStatus type,
	       const wchar_t* fileName, bool isLargeFile);

	// Set file name and determine language from extension
	void setFileName(const wchar_t* fn);

	const wchar_t* getFullPathName() const { return _fullPathName.c_str(); }
	const wchar_t* getFileName() const { return _fileName; }
	const wchar_t* getCompactFileName() const { return _compactFileName.c_str(); }
	void refreshCompactFileName();
	void normalizeTabName(std::wstring& tabName);

	BufferID getID() const { return _id; }

	void increaseRecentTag() {
		_recentTag = ++_recentTagCtr;
		doNotify(BufferChangeRecentTag);
	}
	long getRecentTag() const { return _recentTag; }

	bool checkFileState();

	bool isDirty() const { return _isDirty; }
	bool isReadOnly() const { return (_isUserReadOnly || _isFileReadOnly); }
	bool isUntitled() const { return ((_currentStatus & DOC_UNNAMED) == DOC_UNNAMED); }
	bool isFromNetwork() const { return _isFromNetwork; }
	bool isInaccessible() const { return _isInaccessible; }
	void setInaccessibility(bool val) { _isInaccessible = val; }

	bool getFileReadOnly() const { return _isFileReadOnly; }
	void setFileReadOnly(bool ro);

	bool getUserReadOnly() const { return _isUserReadOnly; }
	void setUserReadOnly(bool ro);

	EolType getEolFormat() const { return _eolFormat; }
	void setEolFormat(EolType format);

	LangType getLangType() const { return _lang; }
	void setLangType(LangType lang, const wchar_t* userLangName = L"");

	UniMode getUnicodeMode() const { return _unicodeMode; }
	void setUnicodeMode(UniMode mode);

	int getEncoding() const { return _encoding; }
	void setEncoding(int encoding);

	DocFileStatus getStatus() const { return _currentStatus; }
	Document getDocument() { return _doc; }
	void setDirty(bool dirty);

	void setPosition(const Position& pos, const ScintillaComponent* identifier);
	Position& getPosition(const ScintillaComponent* identifier);

	void setHeaderLineState(const std::vector<size_t>& folds, ScintillaComponent* identifier);
	const std::vector<size_t>& getHeaderLineState(const ScintillaComponent* identifier) const;

	bool isUserDefineLangExt() const { return (_userLangExt[0] != '\0'); }
	const wchar_t* getUserDefineLangName() const { return _userLangExt.c_str(); }

	const char* getCommentLineSymbol() const;
	const char* getCommentStart() const;
	const char* getCommentEnd() const;

	bool getNeedsLexing() const { return _needLexer; }
	void setNeedsLexing(bool lex);

	bool isUntitledTabRenamed() const { return _isUntitledTabRenamed; }
	void setUntitledTabRenamedStatus(bool isRenamed) { _isUntitledTabRenamed = isRenamed; }

	// Reference counting
	int addReference(ScintillaComponent* identifier);
	int removeReference(const ScintillaComponent* identifier);

	void setHideLineChanged(bool isHide, size_t location);
	void setDeferredReload();
	bool getNeedReload() const { return _needReloading; }
	void setNeedReload(bool reload) { _needReloading = reload; }

	std::wstring tabCreatedTimeString() const { return _tabCreatedTimeString; }
	void setTabCreatedTimeStringFromBakFile();
	void setTabCreatedTimeStringWithCurrentTime();

	size_t docLength() const {
		assert(_pManager != nullptr);
		return _pManager->docLength(_id);
	}

	int64_t getFileLength() const;
	QFileInfo getFileInfo() const { return _fileInfo; }
	QDateTime getLastModified() const { return _lastModified; }

	enum fileTimeType { ft_created, ft_modified, ft_accessed };
	std::wstring getFileTime(fileTimeType ftt) const;
	std::wstring getTimeString(QDateTime rawtime) const;

	Lang* getCurrentLang() const;

	bool isModified() const { return _isModified; }
	void setModifiedStatus(bool isModified) { _isModified = isModified; }

	std::wstring getBackupFileName() const { return _backupFileName; }
	void setBackupFileName(const std::wstring& fileName) { _backupFileName = fileName; }

	// File timestamps (Win32 FILETIME → Qt QDateTime)
	QDateTime getLastModifiedFileTimestamp() const { return _lastModified; }
	QDateTime getLastModifiedTimestamp() const;

	bool isLoadedDirty() const { return _isLoadedDirty; }
	void setLoadedDirty(bool val) { _isLoadedDirty = val; }

	bool isUnsync() const { return _isUnsync; }
	void setUnsync(bool val) { _isUnsync = val; }

	bool isSavePointDirty() const { return _isSavePointDirty; }
	void setSavePointDirty(bool val) { _isSavePointDirty = val; }

	bool isLargeFile() const { return _isLargeFile; }

	// File monitoring (Win32 CreateEvent → Qt QTimer / QFileSystemWatcher)
	void startMonitoring();
	bool isMonitoringOn() const { return _isMonitoringOn; }
	void stopMonitoring();

	void updateTimeStamp();
	void reload();

	void setMapPosition(const MapPosition& mapPosition) { _mapPosition = mapPosition; }
	MapPosition getMapPosition() const { return _mapPosition; }

	void langHasBeenSetFromMenu() { _hasLangBeenSetFromMenu = true; }

	bool allowBraceMach() const;
	bool allowAutoCompletion() const;
	bool allowSmartHilite() const;
	bool allowClickableLink() const;

	void setDocColorId(int idx) { _docColorId = idx; }
	int getDocColorId() { return _docColorId; }

	bool isRTL() const { return _isRTL; }
	void setRTL(bool isRTL) { _isRTL = isRTL; }

	bool isPinned() const { return _isPinned; }
	void setPinned(bool isPinned) { _isPinned = isPinned; }

private:
	int indexOfReference(const ScintillaComponent* identifier) const;
	void setStatus(DocFileStatus status) {
		_currentStatus = status;
		doNotify(BufferChangeStatus);
	}
	void doNotify(int mask);

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;

private:
	FileManager* _pManager = nullptr;
	bool _canNotify = false;
	int _references = 0;
	BufferID _id = nullptr;

	Document _doc;
	LangType _lang = LangType::L_TEXT;
	std::wstring _userLangExt;
	bool _isDirty = false;
	EolType _eolFormat = EolType::osdefault;
	int _encoding = -1;
	UniMode _unicodeMode = UniMode::uniUTF8;

	bool _isUserReadOnly = false;
	bool _isFromNetwork = false;
	bool _needLexer = false;
	bool _isUntitledTabRenamed = false;

	std::vector<ScintillaComponent*> _referees;
	std::vector<Position> _positions;
	std::vector<std::vector<size_t>> _foldStates;

	DocFileStatus _currentStatus = DOC_REGULAR;
	QDateTime _lastModified;
	QFileInfo _fileInfo;

	bool _isFileReadOnly = false;
	std::wstring _fullPathName;
	wchar_t* _fileName = nullptr;
	std::wstring _compactFileName;
	bool _needReloading = false;

	std::wstring _tabCreatedTimeString;

	long _recentTag = -1;
	static long _recentTagCtr;

	int _docColorId = -1;

	std::wstring _backupFileName;
	bool _isModified = false;
	bool _isLoadedDirty = false;
	bool _isUnsync = false;
	bool _isLargeFile = false;
	bool _isSavePointDirty = false;

	bool _isMonitoringOn = false;

	bool _hasLangBeenSetFromMenu = false;

	MapPosition _mapPosition;

	QMutex _reloadFromDiskRequestGuard;
	bool _isInaccessible = false;
	bool _isRTL = false;
	bool _isPinned = false;
};
