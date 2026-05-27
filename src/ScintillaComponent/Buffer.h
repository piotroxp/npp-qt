// Buffer.h - Qt port
#pragma once
#include <mutex>
#include <QObject>
#include <QFile>
#include <QTextCodec>
#include "Utf8_16.h"
#include "WindowsCompat.h"
#include <Scintilla.h>  // For sptr_t, uptr_t

// Forward declarations
class Notepad_plus;
class Buffer;
class FileManager;
class ScintillaEditView;

// Typedefs
typedef Buffer* BufferID;
#define BUFFER_INVALID nullptr
typedef sptr_t Document;

// Enums
enum DocFileStatus {
    DOC_REGULAR = 0x01, DOC_UNNAMED = 0x02, DOC_DELETED = 0x04, DOC_MODIFIED = 0x08,
    DOC_NEEDRELOAD = 0x10, DOC_INACCESSIBLE = 0x20
};

enum BufferStatusInfo {
    BufferChangeNone = 0x000, BufferChangeLanguage = 0x001, BufferChangeDirty = 0x002,
    BufferChangeFormat = 0x004, BufferChangeUnicode = 0x008, BufferChangeReadonly = 0x010,
    BufferChangeStatus = 0x020, BufferChangeTimestamp = 0x040, BufferChangeFilename = 0x080,
    BufferChangeRecentTag = 0x100, BufferChangeLexing = 0x200, BufferChangeMask = 0x3FF
};

enum class EolType { OsDefault = -1, Windows = 0, Unix = 1, Mac = 2, Unknown = 3 };
enum class LangType { L_TEXT = 0, L_C = 2, L_CPP = 3, L_HTML = 8, L_XML = 9, L_PYTHON = 18, L_USER = 15, L_EXTERNAL = 100, L_EXTERNAL_LANG = 200 };
enum SavingStatus { SaveOK = 0, SaveOpenFailed = 1, SaveWritingFailed = 2, NotEnoughRoom = 3, FullReadOnlySavingForbidden = 4 };
enum class UniMode { uni8Bit = 0, uniUTF8 = 1, uni16BE = 2, uni16LE = 3, uniCookie = 4, uniUTF8Bom = 5, uniEnd = 6 };

const wchar_t UNTITLED_STR[] = L"new ";

// Structs
struct BufferViewInfo {
    BufferID _bufID = nullptr;
    int _iView = 0;
    BufferViewInfo() = delete;
    BufferViewInfo(BufferID buf, int view) : _bufID(buf), _iView(view) {}
};

struct Position {
    intptr_t _pos = 0; intptr_t _scrollPos = 0; int _marker = -1;
    int _selectionStart = 0; int _selectionEnd = 0; int _currentLength = 0;
    int _nbChar = 0; bool _isSel = false; bool _isHighlights = false;
    bool _isWordWrap = false; int _lineWrapMode = 0;
    intptr_t _XOffset = 0;
};

struct MapPosition {
    int _firstVisibleLine = 0; int _firstVisibleDigit = 0; int _offset = 0;
    int _scrollWidth = 0; int _caretPos = 0; int _lastSelAnchorPos = 0;
};

// Buffer class
class Buffer {
public:
    Buffer(const wchar_t* filePathName, Document doc, bool isMainDoc = true);
    ~Buffer();

    BufferID getID() const { return this; }
    const wchar_t* getFullPath() const { return _fullPathName.c_str(); }
    const wchar_t* getFileName() const { return _fileName.c_str(); }
    Document getNavigator() const { return _doc; }
    DocFileStatus getStatus() const { return _status; }
    SavingStatus isModified() const { return (_status & DOC_MODIFIED) != 0 ? SavingStatus::SaveOK : SavingStatus::SaveOpenFailed; }
    bool isReadOnly() const { return (_status & DOC_INACCESSIBLE) != 0; }
    EolType getEolFormat() const { return _eolFormat; }
    LangType getLangType() const { return _langType; }
    UniMode getUnicodeMode() const { return _encoding; }

    void setStatus(DocFileStatus status) { _status = status; }
    void setDirty(bool dirty) { if (dirty) _status = static_cast<DocFileStatus>(_status | DOC_MODIFIED); else _status = static_cast<DocFileStatus>(_status & ~DOC_MODIFIED); }
    void setFileName(const wchar_t* name) { _fileName = name; }
    void setFullPathName(const wchar_t* path) { _fullPathName = path; }
    void setEolFormat(EolType eol) { _eolFormat = eol; }
    void setLangType(LangType lang) { _langType = lang; }
    void setEncoding(UniMode enc) { _encoding = enc; }
    void setHidden(bool hide) { Q_UNUSED(hide); }

    bool isUntitled() const { return (_status & DOC_UNNAMED) != 0; }
    bool isDirty() const { return (_status & DOC_MODIFIED) != 0; }

    bool getRulerBarEnabled() const { return _rulerBarEnabled; }
    void setRulerBarEnabled(bool enable) { _rulerBarEnabled = enable; }

    bool getCurrentLineHilitingEnabled() const { return _currentLineHilitingEnabled; }
    void setCurrentLineHilitingEnabled(bool enable) { _currentLineHilitingEnabled = enable; }

private:
    Document _doc = 0;
    std::wstring _fullPathName;
    std::wstring _fileName;
    DocFileStatus _status = DOC_REGULAR;
    EolType _eolFormat = EolType::Windows;
    LangType _langType = LangType::L_TEXT;
    UniMode _encoding = UniMode::uni8Bit;
    FILETIME _fileTime{};
    int _referenceCount = 0;
    int _lastSeenTabIndex = 0;
    Position _pos;
    MapPosition _mapPos;
    bool _rulerBarEnabled = false;
    bool _currentLineHilitingEnabled = false;
};

// FileManager class
class FileManager {
public:
    void init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla);
    BufferID loadFile(const wchar_t* filename, Document doc = 0, int encoding = -1);
    BufferID newEmptyDocument();
    BufferID getBufferByID(BufferID id) { return id; }
    static FileManager& getInstance() { static FileManager instance; return instance; }
private:
    std::vector<Buffer*> _buffers;
};

#define MainFileManager FileManager::getInstance()