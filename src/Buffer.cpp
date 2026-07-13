// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of PowerEditor/src/ScintillaComponent/Buffer.cpp
// Win32→Qt6: GetFileAttributesEx→QFileInfo, FILETIME→QDateTime,
//   CreateEvent→QWaitCondition, DeleteFile→QFile, CreateDirectory→QDir::mkpath,
//   GetFullPathName→QFileInfo::absoluteFilePath, ::MoveFileEx→QFile::rename,
//   SHFileOperationW→QFile::moveToTrash, getFileAttributesExWithTimeout→QFileInfo

#include "Buffer.h"
#include "Notepad_plus.h"  // Notepad_plus (incomplete type usage)
#include "Utf8_16.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStorageInfo>
#include <QDateTime>
#include <QMutexLocker>
#include <algorithm>
#include <sys/stat.h>

static const int blockSize = 128 * 1024 + 4;
long Buffer::_recentTagCtr = 0;

static int getEOLFormatForm(const char* const data, size_t length, int defvalue = 0)
{
    for (size_t i = 0; i < length; ++i) {
        if (data[i] == 0x0D) {
            if (i + 1 < length && data[i + 1] == 0x0A) return 0;
            return 2;
        }
        if (data[i] == 0x0A) return 1;
    }
    return defvalue;
}

static std::wstring getFileNameFromPath(const std::wstring& path) {
    QString qpath = QString::fromWCharArray(path.c_str());
    int slash = qMax(qpath.lastIndexOf('/'), qpath.lastIndexOf('\\'));
    return qpath.mid(slash + 1).toStdWString();
}

static std::wstring getExtension(const std::wstring& path) {
    QString qpath = QString::fromWCharArray(path.c_str());
    int dot = qpath.lastIndexOf('.');
    if (dot >= 0) return qpath.mid(dot).toStdWString();
    return {};
}

static bool fileExists(const wchar_t* path) {
    return QFile::exists(QString::fromWCharArray(path));
}

static bool isNetworkPath(const wchar_t* path) {
    return QString::fromWCharArray(path).startsWith("//") ||
           QString::fromWCharArray(path).startsWith("\\\\");
}

LangType Buffer::langFromExtension(const std::wstring& ext) {
    QString qext = QString::fromStdWString(ext).mid(1).toLower();
    if (qext == "c" || qext == "h") return LangType::L_C;
    if (qext == "cpp" || qext == "cxx" || qext == "cc" || qext == "hpp" || qext == "c++") return LangType::L_CPP;
    if (qext == "java") return LangType::L_JAVA;
    if (qext == "html" || qext == "htm") return LangType::L_HTML;
    if (qext == "xml") return LangType::L_XML;
    if (qext == "py" || qext == "pyw") return LangType::L_PYTHON;
    if (qext == "js" || qext == "mjs" || qext == "jsx" || qext == "ts" || qext == "tsx") return LangType::L_JAVASCRIPT;
    if (qext == "php") return LangType::L_PHP;
    if (qext == "css" || qext == "scss" || qext == "sass" || qext == "less") return LangType::L_CSS;
    if (qext == "sql") return LangType::L_SQL;
    if (qext == "lua") return LangType::L_LUA;
    if (qext == "rb" || qext == "rake") return LangType::L_RUBY;
    if (qext == "pl" || qext == "pm") return LangType::L_PERL;
    if (qext == "vb" || qext == "vbs") return LangType::L_VB;
    if (qext == "asm" || qext == "masm" || qext == "nasm") return LangType::L_ASM;
    if (qext == "sh" || qext == "bash" || qext == "zsh" || qext == "fish") return LangType::L_BATCH;
    if (qext == "makefile") return LangType::L_MAKEFILE;
    if (qext == "ini" || qext == "conf" || qext == "cfg") return LangType::L_INI;
    if (qext == "props" || qext == "properties") return LangType::L_PROPS;
    if (qext == "bat" || qext == "cmd") return LangType::L_BATCH;
    if (qext == "nsi") return LangType::L_NSIS;
    if (qext == "vhdl" || qext == "vhd") return LangType::L_VHDL;
    if (qext == "lisp" || qext == "el") return LangType::L_LISP;
    if (qext == "scm" || qext == "ss") return LangType::L_SCHEME;
    if (qext == "diff" || qext == "patch") return LangType::L_DIFF;
    if (qext == "cs") return LangType::L_CPP;
    if (qext == "go") return LangType::L_CPP;
    if (qext == "rs") return LangType::L_CPP;
    if (qext == "swift") return LangType::L_CPP;
    if (qext == "kt" || qext == "kts") return LangType::L_CPP;
    if (qext == "jsp") return LangType::L_JAVASCRIPT;
    if (qext == "asp" || qext == "aspx") return LangType::L_ASP;
    if (qext == "cshtml") return LangType::L_HTML;
    if (qext == "yaml" || qext == "yml" || qext == "json" || qext == "md" || qext == "txt") return LangType::L_TEXT;
    return LangType::L_TEXT;
}

// Buffer ================================================================
Buffer::Buffer(FileManager* pManager, BufferID id, Document doc, DocFileStatus type,
               const wchar_t* fileName, bool isLargeFile)
    : QObject(nullptr)
    , _pManager(pManager), _id(id), _doc(doc), _isLargeFile(isLargeFile)
{
    _currentStatus = type;
    setFileName(fileName);
    updateTimeStamp();
    checkFileState();
    _canNotify = true;
}

Buffer::Buffer()
    : QObject(nullptr)
    , _pManager(nullptr), _id(0), _doc(0)
    , _isLargeFile(false)
    , _currentStatus(DocFileStatus::DOC_UNNAMED)
    , _fileName(nullptr)
{
    _isModified = false;
    _canNotify = true;
}

Buffer::~Buffer() = default;

void Buffer::doNotify(int mask)
{
    if (_canNotify && _pManager)
        _pManager->beNotifiedOfBufferChange(this, mask);
}

void Buffer::setDirty(bool dirty) { _isDirty = dirty; doNotify(BufferChangeDirty); }
void Buffer::setEncoding(int encoding) { _encoding = encoding; doNotify(BufferChangeUnicode | BufferChangeDirty); }
void Buffer::setUnicodeMode(UniMode mode) { _unicodeMode = mode; doNotify(BufferChangeUnicode | BufferChangeDirty); }

void Buffer::setFilePath(const QString& path) {
    _fullPathName = path.toStdWString();
    delete[] _fileName;
    QFileInfo fi(path);
    _fileName = new wchar_t[fi.fileName().size() + 1];
    fi.fileName().toWCharArray(_fileName);
    _fileName[fi.fileName().size()] = 0;
    refreshCompactFileName();
    updateTimeStamp();
}

QString Buffer::filePath() const {
    return QString::fromWCharArray(_fullPathName.c_str());
}

void Buffer::setContent(const QString& content) {
    _textContent = content;
    _isDirty = true;
    doNotify(BufferChangeDirty);
}

void Buffer::setLangType(LangType lang, const wchar_t* userLangName)
{
    if (lang == _lang && lang != LangType::L_USER) return;
    _lang = lang;
    if (_lang == LangType::L_USER) _userLangExt = userLangName ? userLangName : L"";
    else if (_lang == LangType::L_ASCII) _encoding = 437;
    _needLexer = true;
    doNotify(BufferChangeLanguage | BufferChangeLexing);
}

void Buffer::updateTimeStamp()
{
    QFileInfo fi(QString::fromWCharArray(_fullPathName.c_str()));
    QDateTime ts = fi.exists() ? fi.lastModified() : QDateTime::currentDateTime();
    if (ts != _timeStamp) { _timeStamp = ts; doNotify(BufferChangeTimestamp); }
}

void Buffer::setFileName(const wchar_t* fn)
{
    _fullPathName = fn ? fn : L"";
    if (_fullPathName.empty()) {
        _fileName = nullptr;
        _compactFileName = L"";
    } else {
        _fileName = const_cast<wchar_t*>(getFileNameFromPath(_fullPathName).c_str());
        const int tabCompactLabelLen = 30;
        if (tabCompactLabelLen == 0 || wcslen(_fileName) <= static_cast<size_t>(tabCompactLabelLen))
            _compactFileName = _fileName;
        else
            _compactFileName = QString::fromWCharArray(_fileName).left(tabCompactLabelLen - 3).toStdWString() + L"...";
    }
    _isFromNetwork = isNetworkPath(fn);
    LangType det = langFromExtension(getExtension(_fullPathName));
    if (det == LangType::L_TEXT && _fileName) {
        QString name = QString::fromWCharArray(_fileName);
        if (name.compare("makefile", Qt::CaseInsensitive) == 0 ||
            name.compare("GNUmakefile", Qt::CaseInsensitive) == 0)
            det = LangType::L_MAKEFILE;
        else if (name.compare("SConstruct", Qt::CaseInsensitive) == 0 ||
                 name.compare("SConscript", Qt::CaseInsensitive) == 0)
            det = LangType::L_PYTHON;
    }
    if (!_hasLangBeenSetFromMenu && det != _lang)
        _lang = _isLargeFile ? LangType::L_TEXT : det;
    int notifMask = BufferChangeFilename | BufferChangeTimestamp;
    if (!_hasLangBeenSetFromMenu && det != _lang) notifMask |= BufferChangeLanguage;
    doNotify(notifMask);
}

void Buffer::refreshCompactFileName()
{
    if (!_fileName || !_fileName[0]) { _compactFileName = L""; }
    else {
        const int tcl = 30;
        if (tcl == 0 || wcslen(_fileName) <= static_cast<size_t>(tcl))
            _compactFileName = _fileName;
        else
            _compactFileName = QString::fromWCharArray(_fileName).left(tcl - 3).toStdWString() + L"...";
    }
    doNotify(BufferChangeFilename);
}

bool Buffer::checkFileState()
{
    if (isUntitled() || isMonitoringOn()) return false;
    QFileInfo fi(QString::fromWCharArray(_fullPathName.c_str()));
    bool exists = fi.exists();
    if (_currentStatus == DocFileStatus::DOC_INACCESSIBLE && !exists) {
        _currentStatus = DocFileStatus::DOC_DELETED;
        _isInaccessible = true; _isFileReadOnly = true; _isModified = false;
        _timeStamp = QDateTime();
        doNotify(BufferChangeStatus | BufferChangeReadonly | BufferChangeTimestamp);
        return true;
    }
    if (_currentStatus != DocFileStatus::DOC_DELETED && !exists) {
        _currentStatus = DocFileStatus::DOC_DELETED;
        _isFileReadOnly = false; _isDirty = true; _timeStamp = QDateTime();
        doNotify(BufferChangeStatus | BufferChangeReadonly | BufferChangeTimestamp);
        return true;
    }
    if (_currentStatus == DocFileStatus::DOC_DELETED && exists) {
        _isFileReadOnly = !fi.isWritable();
        _currentStatus = DocFileStatus::DOC_MODIFIED;
        _timeStamp = fi.lastModified();
        QMutexLocker lock(&_reloadGuard);
        doNotify(BufferChangeStatus | BufferChangeReadonly | BufferChangeTimestamp);
        return true;
    }
    if (exists) {
        int mask = 0;
        bool ro = !fi.isWritable();
        if (ro != _isFileReadOnly) { _isFileReadOnly = ro; mask |= BufferChangeReadonly; }
        QDateTime ts = fi.lastModified();
        if (ts != _timeStamp) {
            _timeStamp = ts; mask |= BufferChangeTimestamp;
            _currentStatus = DocFileStatus::DOC_MODIFIED; mask |= BufferChangeStatus;
        }
        if (mask) { QMutexLocker lock(&_reloadGuard); doNotify(mask); return true; }
        return false;
    }
    return false;
}

void Buffer::reload()
{
    QFileInfo fi(QString::fromWCharArray(_fullPathName.c_str()));
    if (fi.exists()) {
        _timeStamp = fi.lastModified();
        _currentStatus = DocFileStatus::DOC_NEEDRELOAD;
        doNotify(BufferChangeTimestamp | BufferChangeStatus);
    }
}

int64_t Buffer::getFileLength() const
{
    if (_currentStatus == DocFileStatus::DOC_UNNAMED) return -1;
    QFileInfo fi(QString::fromWCharArray(_fullPathName.c_str()));
    return fi.exists() ? fi.size() : -1;
}

std::wstring Buffer::getFileTime(fileTimeType ftt) const
{
    QFileInfo fi;
    if (_currentStatus == DocFileStatus::DOC_UNNAMED && !_backupFileName.empty())
        fi = QFileInfo(QString::fromStdWString(_backupFileName));
    else
        fi = QFileInfo(QString::fromWCharArray(_fullPathName.c_str()));
    if (!fi.exists()) return L"";
    QDateTime dt;
    switch (ftt) {
        case ft_created:  dt = fi.birthTime();  break;
        case ft_modified: dt = fi.lastModified(); break;
        default:          dt = fi.lastRead();  break;
    }
    return dt.toString("yyyy-MM-dd hh:mm:ss").toStdWString();
}

void Buffer::setPosition(const Position& pos, const ScintillaEditView* id) {
    int idx = indexOfReference(id);
    if (idx != -1) _positions[idx] = pos;
}

Position& Buffer::getPosition(const ScintillaEditView* id) {
    static Position dummy;
    int idx = indexOfReference(id);
    return (idx == -1) ? dummy : _positions[idx];
}

void Buffer::setHeaderLineState(const std::vector<size_t>& folds, ScintillaEditView* id) {
    int idx = indexOfReference(id);
    if (idx != -1) _foldStates[idx] = folds;
}

const std::vector<size_t>& Buffer::getHeaderLineState(const ScintillaEditView* id) const {
    static std::vector<size_t> dummy;
    int idx = indexOfReference(id);
    return (idx == -1) ? dummy : _foldStates[idx];
}

int Buffer::indexOfReference(const ScintillaEditView* id) const {
    for (size_t i = 0; i < _referees.size(); ++i)
        if (_referees[i] == id) return static_cast<int>(i);
    return -1;
}

int Buffer::addReference(ScintillaEditView* id) {
    if (indexOfReference(id) != -1) return _references;
    _referees.push_back(id);
    _positions.push_back(Position());
    _foldStates.push_back({});
    return ++_references;
}

int Buffer::removeReference(const ScintillaEditView* id) {
    int idx = indexOfReference(id);
    if (idx == -1) return _references;
    _referees.erase(_referees.begin() + idx);
    _positions.erase(_positions.begin() + idx);
    _foldStates.erase(_foldStates.begin() + idx);
    return --_references;
}

void Buffer::setHideLineChanged(bool, size_t) {}
void Buffer::setDeferredReload() { _isModified = false; _needReloading = true; doNotify(BufferChangeDirty); }
bool Buffer::allowBraceMach() const { return !_isLargeFile; }
bool Buffer::allowAutoCompletion() const { return !_isLargeFile; }
bool Buffer::allowSmartHilite() const { return !_isLargeFile; }
bool Buffer::allowClickableLink() const { return !_isLargeFile; }
void Buffer::setUserReadOnly(bool ro) { _isUserReadOnly = ro; doNotify(BufferChangeReadonly); }
void Buffer::setFileReadOnly(bool ro) { _isFileReadOnly = ro; doNotify(BufferChangeReadonly); }
void Buffer::setNeedsLexing(bool lex) { _needLexer = lex; doNotify(BufferChangeLexing); }
void Buffer::setEolFormat(EolFormat format) { _eolFormat = format; doNotify(BufferChangeFormat); }
void Buffer::setTabCreatedTimeStringFromBakFile() {
    if (!_isFromNetwork && _currentStatus == DocFileStatus::DOC_UNNAMED)
        _tabCreatedTimeString = getFileTime(ft_created);
}
void Buffer::setTabCreatedTimeStringWithCurrentTime() {
    if (_currentStatus == DocFileStatus::DOC_UNNAMED)
        _tabCreatedTimeString = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss").toStdWString();
}
void Buffer::startMonitoring() { _isMonitoringOn = true; }
void Buffer::stopMonitoring() { _isMonitoringOn = false; _monitorCond.wakeOne(); }
size_t Buffer::docLength() const { return _pManager ? _pManager->docLength(const_cast<Buffer*>(this)) : 0; }

// FileManager ===========================================================
FileManager::FileManager() = default;

FileManager::~FileManager() { for (Buffer* b : _buffers) delete b; }

void FileManager::init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla)
{
    _pNotepadPlus = pNotepadPlus;
    _pscratchTilla = pscratchTilla;
    _pscratchTilla->send(SCI_SETUNDOCOLLECTION, false);
    _scratchDocDefault = _pscratchTilla->send(SCI_GETDOCPOINTER);
    _pscratchTilla->send(SCI_ADDREFDOCUMENT, 0, static_cast<sptr_t>(_scratchDocDefault));
}

void FileManager::checkFilesystemChanges(bool bCheckOnlyCurrentBuffer)
{
    if (bCheckOnlyCurrentBuffer) {
        if (_pNotepadPlus) _pNotepadPlus->getCurrentBuffer()->checkFileState();
    } else {
        for (int i = int(_nbBufs) - 1; i >= 0; --i)
            _buffers[i]->checkFileState();
    }
}

size_t FileManager::getNbDirtyBuffers() const {
    size_t n = 0;
    for (size_t i = 0; i < _nbBufs; ++i)
        if (_buffers[i]->isDirty()) ++n;
    return n;
}

int FileManager::getBufferIndexByID(BufferID id) {
    for (size_t i = 0; i < _nbBufs; ++i)
        if (_buffers[i]->getID() == id) return static_cast<int>(i);
    return -1;
}

Buffer* FileManager::getBufferByIndex(size_t index) {
    return (index >= _buffers.size()) ? nullptr : _buffers.at(index);
}

void FileManager::beNotifiedOfBufferChange(Buffer* theBuf, int mask) {
    if (_pNotepadPlus) _pNotepadPlus->notifyBufferChanged(theBuf, mask);
}

void FileManager::addBufferReference(BufferID buffer, ScintillaEditView* identifier) {
    Buffer* buf = getBufferByID(buffer);
    if (buf) buf->addReference(identifier);
}

void FileManager::closeBuffer(BufferID id, const ScintillaEditView* identifier)
{
    int index = getBufferIndexByID(id);
    Buffer* buf = getBufferByIndex(index);
    if (!buf) return;
    int refs = buf->removeReference(identifier);
    if (!refs) {
        _pscratchTilla->send(SCI_RELEASEDOCUMENT, 0, static_cast<sptr_t>(buf->getDocument()));
        _buffers.erase(_buffers.begin() + index);
        delete buf;
        --_nbBufs;
    }
}

BufferID FileManager::loadFile(const wchar_t* filename, Document doc, int encoding, const wchar_t* backupFileName)
{
    if (!filename) return 0;
    const wchar_t* pPath = filename;
    if (!fileExists(pPath) && backupFileName) pPath = backupFileName;
    int64_t fileSize = -1;
    if (pPath) { QFileInfo fi(QString::fromWCharArray(pPath)); if (fi.exists()) fileSize = fi.size(); }
    if (fileSize < 0) return 0;
    bool ownDoc = !doc;
    if (!doc) doc = _pscratchTilla->send(SCI_CREATEDOCUMENT, 0, static_cast<sptr_t>(0));
    QFileInfo fi(QString::fromWCharArray(filename));
    QString absPath = fi.absoluteFilePath();
    bool isSnapshot = backupFileName && fileExists(backupFileName);
    if (isSnapshot && !fileExists(absPath.toStdWString().c_str()))
        absPath = QString::fromWCharArray(filename);
    Utf8_16_Read UnicodeConvertor;
    LoadedFileFormat loadedFileFormat;
    loadedFileFormat._encoding = encoding;
    loadedFileFormat._language = LangType::L_TEXT;
    char* data = new char[blockSize + 8];
    bool loadRes = loadFileData(doc, fileSize, absPath.toStdWString().c_str(), data, &UnicodeConvertor, loadedFileFormat);
    delete[] data;
    if (loadRes) {
        Buffer* newBuf = new Buffer(this, _nextBufferID, doc, DocFileStatus::DOC_REGULAR, absPath.toStdWString().c_str(), false);
        if (backupFileName) {
            newBuf->setBackupFileName(backupFileName);
            if (!fileExists(absPath.toStdWString().c_str()))
                newBuf->setFileName(filename);
        }
        if (newBuf->getLangType() == LangType::L_ASCII)
            newBuf->setEncoding(437);
        setLoadedBufferEncodingAndEol(newBuf, UnicodeConvertor, loadedFileFormat._encoding, loadedFileFormat._eolFormat);
        _buffers.push_back(newBuf);
        ++_nbBufs;
        _nextBufferID = newBuf->getID();
        return newBuf->getID();
    } else {
        if (ownDoc) _pscratchTilla->send(SCI_RELEASEDOCUMENT, 0, static_cast<sptr_t>(doc));
        return 0;
    }
}

bool FileManager::reloadBuffer(BufferID id)
{
    Buffer* buf = getBufferByID(id);
    if (!buf) return false;
    Document doc = buf->getDocument();
    Utf8_16_Read unicodeConvertor;
    LoadedFileFormat loadedFileFormat;
    loadedFileFormat._encoding = buf->getEncoding();
    loadedFileFormat._language = buf->getLangType();
    buf->setLoadedDirty(false);
    QFileInfo fi(QString::fromWCharArray(buf->getFullPathName()));
    int64_t fileSize = fi.exists() ? fi.size() : 0;
    char* data = new char[blockSize + 8];
    buf->_canNotify = false;
    bool res = loadFileData(doc, fileSize, buf->getFullPathName(), data, &unicodeConvertor, loadedFileFormat);
    buf->_canNotify = true;
    delete[] data;
    if (res) {
        buf->setUnsync(false);
        buf->setDirty(false);
        buf->setSavePointDirty(false);
        setLoadedBufferEncodingAndEol(buf, unicodeConvertor, loadedFileFormat._encoding, loadedFileFormat._eolFormat);
    }
    return res;
}

void FileManager::setLoadedBufferEncodingAndEol(Buffer* buf, const Utf8_16_Read&, int encoding, int bkformat)
{
    UniMode unimode2Set = UniMode::uniUTF8;
    int encoding2Set = encoding;
    if (encoding2Set == -1) unimode2Set = UniMode::uni8Bit;
    else {
        encoding2Set = (encoding2Set == 65001) ? -1 : encoding2Set;
        unimode2Set = UniMode::uniUTF8_NoBOM;
    }
    buf->setEncoding(encoding2Set);
    buf->setUnicodeMode(unimode2Set);
    if (bkformat != 0) buf->setEolFormat(static_cast<EolFormat>(bkformat));
}

bool FileManager::reloadBufferDeferred(BufferID id) {
    Buffer* buf = getBufferByID(id);
    if (buf) { buf->setDeferredReload(); return true; }
    return false;
}

bool FileManager::deleteFile(BufferID id)
{
    if (id == 0) return false;
    Buffer* buf = getBufferByID(id);
    if (!buf) return false;
    QString path = QString::fromWCharArray(buf->getFullPathName());
    if (!fileExists(path.toStdWString().c_str())) return false;
    return QFile::moveToTrash(path);
}

bool FileManager::moveFile(BufferID id, const wchar_t* newFileName)
{
    if (id == 0) return false;
    Buffer* buf = getBufferByID(id);
    if (!buf) return false;
    QString src = QString::fromWCharArray(buf->getFullPathName());
    QString dst = QString::fromWCharArray(newFileName);
    if (!QFile::exists(src)) return false;
    bool ok = QFile::rename(src, dst);
    if (ok) buf->setFileName(newFileName);
    return ok;
}

bool FileManager::backupCurrentBuffer()
{
    Buffer* buffer = _pNotepadPlus ? _pNotepadPlus->getCurrentBuffer() : nullptr;
    if (!buffer || buffer->isLargeFile()) return false;
    if (!buffer->isDirty()) {
        QString bfp = QString::fromStdWString(buffer->getBackupFileName());
        if (!bfp.isEmpty()) { QFile::remove(bfp); buffer->setBackupFileName({}); }
        return true;
    }
    if (!buffer->_isModified) return true;
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(buffer->getDocument()));
    size_t lengthDoc = _pscratchTilla->send(SCI_GETLENGTH);
    char* buf = (char*)_pscratchTilla->send(SCI_GETCHARACTERPOINTER);
    QString userPath = QString::fromWCharArray(NppParameters::getInstance().getUserPath().c_str());
    QString backupDir = userPath + "/backup";
    QDir().mkpath(backupDir);
    QString fileName = QString::fromWCharArray(buffer->getFileName());
    QString backupPath = backupDir + "/" + fileName + "@" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    buffer->setBackupFileName(backupPath.toStdWString());
    QFile f(backupPath);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(buf, lengthDoc);
    f.close();
    buffer->setTabCreatedTimeStringFromBakFile();
    buffer->_isModified = false;
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, _scratchDocDefault);
    return true;
}

bool FileManager::deleteBufferBackup(BufferID id)
{
    Buffer* buffer = getBufferByID(id);
    if (!buffer) return true;
    QString backupFilePath = QString::fromStdWString(buffer->getBackupFileName());
    if (!backupFilePath.isEmpty()) {
        buffer->setBackupFileName({});
        return QFile::remove(backupFilePath);
    }
    return true;
}

QtSavingStatus FileManager::saveBuffer(BufferID id, const wchar_t* filename, bool isCopy)
{
    Buffer* buffer = getBufferByID(id);
    if (!buffer) return QtSaveOpenFailed;
    QFileInfo fi(QString::fromWCharArray(filename));
    QString absPath = fi.absoluteFilePath();
    QStorageInfo storage(fi.absolutePath());
    if (storage.isValid() && storage.bytesAvailable() < buffer->docLength())
        return QtNotEnoughRoom;
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(buffer->getDocument()));
    size_t lengthDoc = _pscratchTilla->send(SCI_GETLENGTH);
    char* buf = (char*)_pscratchTilla->send(SCI_GETCHARACTERPOINTER);
    QFile f(absPath);
    if (!f.open(QIODevice::WriteOnly)) {
        _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(_scratchDocDefault));
        return QtSaveOpenFailed;
    }
    qint64 written = f.write(buf, lengthDoc);
    f.close();
    if (written != static_cast<qint64>(lengthDoc)) {
        _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(_scratchDocDefault));
        return QtSaveWritingFailed;
    }
    if (!buffer->isLargeFile() && buffer->getLangType() == LangType::L_TEXT) {
        LangType detectedLang = detectLanguageFromTextBeginning((unsigned char*)buf, lengthDoc);
        if (detectedLang != LangType::L_TEXT) {
            buffer->_lang = detectedLang;
            buffer->doNotify(BufferChangeFilename | BufferChangeTimestamp | BufferChangeLanguage);
        }
    }
    buffer->setDirty(false);
    buffer->setUnsync(false);
    buffer->setSavePointDirty(false);
    buffer->setStatus(DocFileStatus::DOC_REGULAR);
    buffer->checkFileState();
    _pscratchTilla->send(SCI_SETSAVEPOINT);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(_scratchDocDefault));
    QString bfp = QString::fromStdWString(buffer->getBackupFileName());
    if (!bfp.isEmpty()) { buffer->setBackupFileName({}); QFile::remove(bfp); }
    if (isCopy) return QtSaveOK;
    buffer->setFileName(filename);
    return QtSaveOK;
}

size_t FileManager::nextUntitledNewNumber() const
{
    std::vector<size_t> usedNumbers;
    for (size_t i = 0; i < _buffers.size(); ++i) {
        Buffer* buf = _buffers.at(i);
        if (buf->isUntitled()) {
            QString name = QString::fromWCharArray(buf->getFileName());
            if (name.startsWith("new ")) {
                bool ok = false;
                int num = name.mid(4).toInt(&ok);
                if (ok) usedNumbers.push_back(num);
            }
        }
    }
    size_t newNumber = 1;
    while (std::find(usedNumbers.begin(), usedNumbers.end(), newNumber) != usedNumbers.end())
        ++newNumber;
    return newNumber;
}

BufferID FileManager::newEmptyDocument()
{
    QString newTitle = "new " + QString::number(nextUntitledNewNumber());
    Document doc = _pscratchTilla->send(SCI_CREATEDOCUMENT, 0, static_cast<sptr_t>(0));
    if (!doc) return 0;
    Buffer* newBuf = new Buffer(this, _nextBufferID, doc, DocFileStatus::DOC_UNNAMED, newTitle.toStdWString().c_str(), false);
    newBuf->setTabCreatedTimeStringWithCurrentTime();
    _buffers.push_back(newBuf);
    ++_nbBufs;
    _nextBufferID = newBuf->getID();
    return newBuf->getID();
}

BufferID FileManager::newPlaceholderDocument(const wchar_t* missingFilename, int whichOne, const wchar_t*)
{
    Q_UNUSED(whichOne);
    BufferID buf = MainFileManager.newEmptyDocument();
    if (buf == 0) return 0;
    if (_pNotepadPlus) _pNotepadPlus->loadBufferIntoView(buf, whichOne);
    getBufferByID(buf)->setFileName(missingFilename);
    getBufferByID(buf)->_currentStatus = DocFileStatus::DOC_INACCESSIBLE;
    return buf;
}

BufferID FileManager::bufferFromDocument(Document doc, bool isMainEditZone)
{
    QString newTitle = isMainEditZone
        ? "new " + QString::number(nextUntitledNewNumber())
        : "newNonMainEditZoneInvisibleTitle ";
    Buffer* newBuf = new Buffer(this, _nextBufferID, doc, DocFileStatus::DOC_UNNAMED, newTitle.toStdWString().c_str(), false);
    _buffers.push_back(newBuf);
    ++_nbBufs;
    _nextBufferID = newBuf->getID();
    return newBuf->getID();
}

int FileManager::detectCodepage(char*, size_t) { return -1; }

LangType FileManager::detectLanguageFromTextBeginning(const unsigned char* data, size_t dataLen)
{
    if (dataLen <= 3) return LangType::L_TEXT;
    // Check BOM
    if ((data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) ||
        (data[0] == 0xFE && data[1] == 0xFF) || (data[0] == 0xFF && data[1] == 0xFE))
        return LangType::L_TEXT;
    // Skip whitespace
    size_t i = 0;
    for (; i < dataLen; ++i)
        if (data[i] != ' ' && data[i] != '\t' && data[i] != '\n' && data[i] != '\r') break;
    if (i == dataLen) return LangType::L_TEXT;
    size_t maxlen = std::min<size_t>(40, dataLen - i);
    std::string buf2Test((const char*)data + i, maxlen);
    size_t crnl = std::min(buf2Test.find('\r'), buf2Test.find('\n'));
    if (crnl != std::string::npos && crnl < maxlen) buf2Test.resize(crnl);
    if (buf2Test.compare(0, 2, "#!") == 0) {
        if (buf2Test.find("sh") != std::string::npos) return LangType::L_BATCH;
        if (buf2Test.find("python") != std::string::npos) return LangType::L_PYTHON;
        if (buf2Test.find("perl") != std::string::npos) return LangType::L_PERL;
        if (buf2Test.find("php") != std::string::npos) return LangType::L_PHP;
        if (buf2Test.find("ruby") != std::string::npos) return LangType::L_RUBY;
        if (buf2Test.find("node") != std::string::npos) return LangType::L_JAVASCRIPT;
    }
    if (buf2Test.find("<?xml") == 0) return LangType::L_XML;
    if (buf2Test.find("<?php") == 0) return LangType::L_PHP;
    if (buf2Test.find("<html") == 0 || buf2Test.find("<HTML") == 0) return LangType::L_HTML;
    if (buf2Test.find("<!DOCTYPE html") == 0) return LangType::L_HTML;
    if (buf2Test.find("<?") == 0) return LangType::L_PHP;
    return LangType::L_TEXT;
}

bool FileManager::loadFileData(Document doc, int64_t fileSize, const wchar_t* fn, char* data, Utf8_16_Read* uc, LoadedFileFormat& lf)
{
    QFile fobj(QString::fromWCharArray(fn));
    if (!fobj.open(QIODevice::ReadOnly)) return false;
    _pscratchTilla->send(SCI_SETSTATUS, 0);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(doc));
    if (_pscratchTilla->send(SCI_GETREADONLY))
        _pscratchTilla->send(SCI_SETREADONLY, false);
    _pscratchTilla->send(SCI_CLEARALL);
    _pscratchTilla->send(SCI_SETUNDOCOLLECTION, false);
    // Set lexer based on language
    if (lf._language < LangType::L_EXTERNAL) {
        // Use null lexer initially, MainWindow applies the real lexer
        _pscratchTilla->send(SCI_SETILEXER, 0, static_cast<sptr_t>(0));
    }
    if (lf._encoding != -1)
        _pscratchTilla->send(SCI_SETCODEPAGE, SC_CP_UTF8);
    int incompleteMultibyteChar = 0;
    do {
        qint64 len = fobj.read(data + incompleteMultibyteChar, blockSize - incompleteMultibyteChar);
        if (len <= 0) break;
        len += incompleteMultibyteChar;
        if (lf._encoding != -1) {
            _pscratchTilla->send(SCI_APPENDTEXT, len, reinterpret_cast<sptr_t>(data));
        } else {
            uc->convert(data, len);  // populate internal buffer
            int convLen = static_cast<int>(uc->getNewSize());
            _pscratchTilla->send(SCI_APPENDTEXT, static_cast<sptr_t>(convLen), reinterpret_cast<sptr_t>(uc->getNewBuf()));
        }
    } while (!fobj.atEnd());
    // Detect EOL from raw data
    int eolFormat = getEOLFormatForm(data, qMin<qint64>(1024, fobj.size()), 0);
    if (eolFormat == 0) lf._eolFormat = 0; // Windows CRLF
    else if (eolFormat == 1) lf._eolFormat = 1; // Unix LF
    else lf._eolFormat = 0;
    _pscratchTilla->send(SCI_EMPTYUNDOBUFFER);
    _pscratchTilla->send(SCI_SETSAVEPOINT);
    _pscratchTilla->send(SCI_SETUNDOCOLLECTION, true);
    if (_pscratchTilla->send(SCI_GETREADONLY))
        _pscratchTilla->send(SCI_SETREADONLY, true);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(_scratchDocDefault));
    return true;
}

BufferID FileManager::getBufferFromName(const wchar_t* name)
{
    for (auto buf : _buffers) {
        if (QString::fromWCharArray(name).compare(QString::fromWCharArray(buf->getFullPathName()), Qt::CaseInsensitive) == 0) {
            if (!buf->_referees.empty() && buf->_referees[0]->isVisible())
                return buf->getID();
        }
    }
    return 0;
}

BufferID FileManager::getBufferFromDocument(Document doc)
{
    for (size_t i = 0; i < _nbBufs; ++i)
        if (_buffers[i]->getDocument() == doc) return _buffers[i]->getID();
    return 0;
}

bool FileManager::createEmptyFile(const wchar_t* path)
{
    QFile f(QString::fromWCharArray(path));
    if (f.exists()) return true;
    if (f.open(QIODevice::WriteOnly)) { f.close(); return true; }
    return false;
}

int FileManager::getFileNameFromBuffer(BufferID id, wchar_t* fn2copy)
{
    if (getBufferIndexByID(id) == -1) return -1;
    const Buffer* buf = getBufferByID(id);
    if (fn2copy && buf) {
        const wchar_t* src = buf->getFullPathName();
        wcscpy(fn2copy, src);
    }
    return buf ? static_cast<int>(wcslen(buf->getFullPathName())) : 0;
}

size_t FileManager::docLength(Buffer* buffer) const
{
    sptr_t curDoc = _pscratchTilla->send(SCI_GETDOCPOINTER);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(buffer->getDocument()));
    size_t len = _pscratchTilla->send(SCI_GETLENGTH);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, curDoc);
    return len;
}

void FileManager::removeHotSpot(Buffer* buffer) const
{
    sptr_t curDoc = _pscratchTilla->send(SCI_GETDOCPOINTER);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, static_cast<sptr_t>(buffer->getDocument()));
    size_t docLen = _pscratchTilla->send(SCI_GETLENGTH);
    _pscratchTilla->send(SCI_SETINDICATORCURRENT, 8); // URL indicator
    _pscratchTilla->send(SCI_INDICATORCLEARRANGE, 0, docLen);
    _pscratchTilla->send(SCI_SETDOCPOINTER, 0, curDoc);
}

// === STUB for linker-missing methods ===

void Buffer::increaseRecentTag() {
    // Stub: update recent file timestamp — no-op for Qt6
}
