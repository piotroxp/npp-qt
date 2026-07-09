// Buffer.cpp - Document buffer implementation for Notepad--Qt
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3

#include "Buffer.h"
#include "FileManager.h"
#include "LanguageManager.h"
#include "common/Util.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QApplication>
#include <QTextStream>

// Static member initialization
long Buffer::_recentTagCtr = 0;

namespace {
    // Detect EOL format from content
    [[maybe_unused]]
    EolType detectEolFormat(const char* data, size_t length, EolType defaultFormat = EolType::EOL_LF) {
        if (!data || length == 0) return defaultFormat;

        for (size_t i = 0; i < length; ++i) {
            if (data[i] == '\r') {
                if (i + 1 < length && data[i + 1] == '\n')
                    return EolType::EOL_CRLF;
                return EolType::EOL_CR;
            }
            if (data[i] == '\n')
                return EolType::EOL_LF;
        }
        return defaultFormat;
    }
}

// ============================================================================
// Construction / Lifecycle
// ============================================================================

Buffer::Buffer(FileManager* pManager, BufferID id, const QString& fileName, bool isLargeFile)
    : QObject()
    , _pManager(pManager)
    , _id(id)
    , _isLargeFile(isLargeFile)
{
    _currentStatus = DocFileStatus::DOC_REGULAR;

    // Set the filename which also determines language
    setFileName(fileName);
    updateTimeStamp();
    checkFileState();

    // Enable notifications after initialization
    _canNotify = true;
}

// ============================================================================
// File Name Management
// ============================================================================

void Buffer::setFileName(const QString& filePath) {
    if (_fullPathName == filePath) {
        updateTimeStamp();
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeTimestamp));
        return;
    }

    _fullPathName = filePath;

    // Extract filename portion
    QFileInfo fi(filePath);
    _fileName = fi.fileName();
    _compactFileName = _fileName;

    // Truncate compact name if needed
    const int maxCompactLen = 50;  // Could be made configurable
    if (_compactFileName.length() > maxCompactLen) {
        _compactFileName = _compactFileName.left(maxCompactLen - 3) + "...";
    }

    // Check if from network
    _isFromNetwork = filePath.startsWith("//") || filePath.startsWith("\\\\")
                  || filePath.startsWith("smb://", Qt::CaseInsensitive);

    // Determine language from extension
    QString ext = fi.suffix().toLower();
    LangType determinedLang = LanguageManager::detect(ext.toStdString());

    // Check for special filenames
    if (determinedLang == LangType::L_TEXT && !_fileName.isEmpty()) {
        QString lowerName = _fileName.toLower();
        if (lowerName == "makefile" || lowerName == "gnuakefile")
            determinedLang = LangType::L_MAKEFILE;
        else if (lowerName == "cmakelists.txt")
            determinedLang = LangType::L_MAKEFILE;
        else if (lowerName == "sconstruct" || lowerName == "sconscript" || lowerName == "wscript")
            determinedLang = LangType::L_PYTHON;
        else if (lowerName == "rakefile" || lowerName == "vagrantfile")
            determinedLang = LangType::L_RUBY;
        else if (lowerName == "crontab" || lowerName == "pkbuild" || lowerName == "apkbuild")
            determinedLang = LangType::L_BATCH;
    }

    updateTimeStamp();

    // Set language if not already set from menu
    int langChange = 0;
    if (!_hasLangBeenSetFromMenu && (determinedLang != _langType || _langType == LangType::L_TEXT)) {
        if (_isLargeFile) {
            _langType = LangType::L_TEXT;
        } else {
            _langType = determinedLang;
            langChange = static_cast<int>(BufferStatusInfo::BufferChangeLanguage);
        }
    }

    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeFilename)
           | static_cast<int>(BufferStatusInfo::BufferChangeTimestamp)
           | langChange);
}

void Buffer::refreshCompactFileName() {
    if (_fileName.isEmpty()) {
        _compactFileName = QString();
    } else {
        const int maxCompactLen = 50;
        if (_fileName.length() <= maxCompactLen) {
            _compactFileName = _fileName;
        } else {
            _compactFileName = _fileName.left(maxCompactLen - 3) + "...";
        }
    }
    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeFilename));
}

void Buffer::normalizeTabName(QString& tabName) const {
    if (tabName.isEmpty()) return;

    // Remove leading/trailing whitespace
    tabName = tabName.trimmed();

    // Remove invalid characters
    QString filtered;
    for (QChar ch : tabName) {
        bool isInvalid = false;
        for (const char* p = INVALID_FILENAME_CHARS; *p != '\0'; ++p) {
            if (ch == QChar(*p)) {
                isInvalid = true;
                break;
            }
        }
        if (!isInvalid)
            filtered += ch;
    }
    tabName = filtered;

    // Restrict length (max ~31 chars typical for tabs)
    const int maxLen = 31;
    if (tabName.length() >= maxLen) {
        tabName.truncate(maxLen - 1);
    }

    // Remove leading/trailing whitespace again
    tabName = tabName.trimmed();
}

// ============================================================================
// State Notifications
// ============================================================================

void Buffer::doNotify(int mask) {
    if (_canNotify && _pManager) {
        _pManager->notifyBufferChanged(this, mask);
    }
}

// ============================================================================
// Dirty/Modified State
// ============================================================================

void Buffer::setDirty(bool dirty) {
    if (_isDirty != dirty) {
        _isDirty = dirty;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeDirty));
        emit dirtyChanged(dirty);
    }
}

// setModifiedStatus is inline in header

// ============================================================================
// Encoding
// ============================================================================

void Buffer::setEncoding(EncodingType encoding) {
    if (_encoding != encoding) {
        _encoding = encoding;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeUnicode)
               | static_cast<int>(BufferStatusInfo::BufferChangeDirty));
        emit encodingChanged(encoding);
    }
}

void Buffer::setLegacyEncoding(int encoding) {
    if (_legacyEncoding != encoding) {
        _legacyEncoding = encoding;
        // Also update the Qt encoding type
        if (encoding == -1) {
            _encoding = EncodingType::UTF_8;
        } else if (encoding == 65001) {  // CP_UTF8
            _encoding = EncodingType::UTF_8;
        } else {
            _encoding = EncodingType::ANSI;  // Could be more specific
        }
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeUnicode)
               | static_cast<int>(BufferStatusInfo::BufferChangeDirty));
        emit encodingChanged(_encoding);
    }
}

// ============================================================================
// EOL Format
// ============================================================================

void Buffer::setEolFormat(EolType format) {
    if (_eolFormat != format) {
        _eolFormat = format;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeFormat));
        emit formatChanged(format);
    }
}

// ============================================================================
// Language/Lexer
// ============================================================================

void Buffer::setLangType(LangType lang, const QString& userLangName) {
    if (lang == _langType && lang != LangType::L_USER)
        return;

    _langType = lang;
    if (lang == LangType::L_USER) {
        _userLangExt = userLangName;
    } else if (lang == LangType::L_TEXT) {
        _legacyEncoding = 437;  // Default to DOS 437 for ASCII
    }

    _needLexing = true;
    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeLanguage)
           | static_cast<int>(BufferStatusInfo::BufferChangeLexing));
    emit languageChanged(lang);
    emit lexingNeeded();
}

void Buffer::setNeedsLexing(bool lex) {
    if (_needLexing != lex) {
        _needLexing = lex;
        if (lex) {
            doNotify(static_cast<int>(BufferStatusInfo::BufferChangeLexing));
            emit lexingNeeded();
        }
    }
}

// ============================================================================
// Read-Only State
// ============================================================================

void Buffer::setFileReadOnly(bool ro) {
    if (_isFileReadOnly != ro) {
        _isFileReadOnly = ro;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeReadonly));
        emit readonlyChanged(ro);
    }
}

void Buffer::setUserReadOnly(bool ro) {
    _isUserReadOnly = ro;
    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeReadonly));
    emit readonlyChanged(ro);
}

// ============================================================================
// Recent Tag
// ============================================================================

void Buffer::increaseRecentTag() {
    _recentTag = ++_recentTagCtr;
    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeRecentTag));
    emit recentTagChanged(_recentTag);
}

// ============================================================================
// File State Checking
// ============================================================================

bool Buffer::checkFileState() {
    // Untitled documents don't have file system state
    if (isUntitled() || isMonitoringOn())
        return false;

    QFileInfo fi(_fullPathName);
    bool fileExists = fi.exists() && fi.isFile();

    bool statusChanged = false;

    if (_currentStatus == DocFileStatus::DOC_INACCESSIBLE && !fileExists) {
        // Document absent on first load - set readonly and not dirty
        _currentStatus = DocFileStatus::DOC_DELETED;
        _isInaccessible = true;
        _isFileReadOnly = true;
        _isDirty = false;
        _timeStamp = QDateTime();
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeStatus)
               | static_cast<int>(BufferStatusInfo::BufferChangeReadonly)
               | static_cast<int>(BufferStatusInfo::BufferChangeTimestamp));
        statusChanged = true;
    }
    else if (_currentStatus != DocFileStatus::DOC_DELETED && !fileExists) {
        // Document has been deleted
        _currentStatus = DocFileStatus::DOC_DELETED;
        _isFileReadOnly = false;
        _isDirty = true;  // Dirty since no match with filesystem
        _timeStamp = QDateTime();
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeStatus)
               | static_cast<int>(BufferStatusInfo::BufferChangeReadonly)
               | static_cast<int>(BufferStatusInfo::BufferChangeTimestamp));
        statusChanged = true;
        emit fileDeleted();
    }
    else if (_currentStatus == DocFileStatus::DOC_DELETED && fileExists) {
        // Document has returned from grave
        _isFileReadOnly = !fi.isWritable();
        _currentStatus = DocFileStatus::DOC_MODIFIED;
        _timeStamp = fi.lastModified();

        QMutexLocker locker(&_reloadGuard);
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeStatus)
               | static_cast<int>(BufferStatusInfo::BufferChangeReadonly)
               | static_cast<int>(BufferStatusInfo::BufferChangeTimestamp));
        statusChanged = true;
        emit fileCreated();
    }
    else if (fileExists) {
        int mask = static_cast<int>(BufferStatusInfo::BufferChangeStatus);  // Status always changes
        bool changed = false;

        // Check readonly
        bool fileReadOnly = !fi.isWritable();
        if (fileReadOnly != _isFileReadOnly) {
            _isFileReadOnly = fileReadOnly;
            mask |= static_cast<int>(BufferStatusInfo::BufferChangeReadonly);
            changed = true;
        }

        // Check timestamp
        QDateTime liveTime = fi.lastModified();
        if (_timeStamp.isNull() || liveTime > _timeStamp) {
            _timeStamp = liveTime;
            mask |= static_cast<int>(BufferStatusInfo::BufferChangeTimestamp);
            _currentStatus = DocFileStatus::DOC_MODIFIED;
            changed = true;
            emit fileExternallyModified();
        }

        if (changed) {
            QMutexLocker locker(&_reloadGuard);
            doNotify(mask);
            return true;
        }
    }

    return statusChanged;
}

void Buffer::reload() {
    QFileInfo fi(_fullPathName);
    if (fi.exists() && fi.isFile()) {
        _timeStamp = fi.lastModified();
        _currentStatus = DocFileStatus::DOC_NEEDRELOAD;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeTimestamp)
               | static_cast<int>(BufferStatusInfo::BufferChangeStatus));
    }
}

void Buffer::setDeferredReload() {
    // When reloading, set to false since it should be marked as clean
    _isDirty = false;
    _needReloading = true;
    doNotify(static_cast<int>(BufferStatusInfo::BufferChangeDirty));
}

// ============================================================================
// File Timestamps
// ============================================================================

void Buffer::updateTimeStamp() {
    QFileInfo fi(_fullPathName);
    if (fi.exists() && fi.isFile()) {
        QDateTime newTime = fi.lastModified();
        if (_timeStamp.isNull() || newTime != _timeStamp) {
            _timeStamp = newTime;
            doNotify(static_cast<int>(BufferStatusInfo::BufferChangeTimestamp));
            emit timestampChanged(_timeStamp);
        }
    }
}

QDateTime Buffer::getLastModifiedTimestamp() const {
    // Check backup file first if available
    if (!_backupFileName.isEmpty()) {
        QFileInfo bakFi(_backupFileName);
        if (bakFi.exists() && bakFi.isFile()) {
            return bakFi.lastModified();
        }
    }
    return _timeStamp;
}

QString Buffer::getFileTimeString(FileTimeType ftt) const {
    QFileInfo fi(_currentStatus == DocFileStatus::DOC_UNNAMED ? _backupFileName : _fullPathName);

    QDateTime time;
    switch (ftt) {
        case FileTimeType::Created:
            // Qt6 doesn't expose creation time directly; use metadata change time as fallback
            time = fi.metadataChangeTime();
            break;
        case FileTimeType::Modified:
            time = fi.lastModified();
            break;
        case FileTimeType::Accessed:
            time = fi.lastRead();
            break;
    }

    if (!time.isValid())
        return QString();

    return time.toString("yyyy-MM-dd hh:mm:ss");
}

int64_t Buffer::getFileLength() const {
    if (_currentStatus == DocFileStatus::DOC_UNNAMED)
        return -1;

    QFileInfo fi(_fullPathName);
    if (fi.exists() && fi.isFile())
        return fi.size();
    return -1;
}

// ============================================================================
// View References
// ============================================================================

int Buffer::indexOfReference(const ScintillaEditor* editor) const {
    int idx = _referees.indexOf(const_cast<ScintillaEditor*>(editor));
    return idx;
}

int Buffer::addReference(ScintillaEditor* editor) {
    if (indexOfReference(editor) != -1)
        return _references;

    _referees.append(editor);
    _positions.append(Position());
    _foldStates.append(std::vector<size_t>());
    ++_references;
    return _references;
}

int Buffer::removeReference(const ScintillaEditor* editor) {
    int idx = indexOfReference(editor);
    if (idx == -1)
        return _references;

    _referees.remove(idx);
    _positions.remove(idx);
    _foldStates.remove(idx);
    --_references;
    return _references;
}

void Buffer::setPosition(const Position& pos, const ScintillaEditor* editor) {
    int idx = indexOfReference(editor);
    if (idx == -1) return;
    _positions[idx] = pos;
}

Position& Buffer::getPosition(const ScintillaEditor* editor) {
    int idx = indexOfReference(editor);
    if (idx == -1) {
        static Position dummy;
        return dummy;
    }
    return _positions[idx];
}

void Buffer::setHeaderLineState(const std::vector<size_t>& folds, ScintillaEditor* editor) {
    int idx = indexOfReference(editor);
    if (idx == -1) return;

    // Deep copy
    _foldStates[idx].clear();
    for (size_t f : folds) {
        _foldStates[idx].push_back(f);
    }
}

const std::vector<size_t>& Buffer::getHeaderLineState(const ScintillaEditor* editor) const {
    int idx = indexOfReference(editor);
    if (idx == -1) {
        static std::vector<size_t> dummy;
        return dummy;
    }
    return _foldStates[idx];
}

// ============================================================================
// Line Change Markers (VCS integration)
// ============================================================================

void Buffer::setHideLineChanged(bool isHide, size_t location) {
    // Notify all editors to update their line change markers
    for (ScintillaEditor* editor : _referees) {
        if (editor) {
            // This would call back into the editor to update markers
            // The actual implementation depends on editor integration
            Q_UNUSED(isHide);
            Q_UNUSED(location);
        }
    }
}

// ============================================================================
// Large File Feature Restrictions
// ============================================================================

bool Buffer::allowBraceMatch() const {
    // TODO: Read from application settings for large file restrictions
    // For now, always allow
    return !_isLargeFile;
}

bool Buffer::allowAutoCompletion() const {
    return !_isLargeFile;
}

bool Buffer::allowSmartHilite() const {
    return !_isLargeFile;
}

bool Buffer::allowClickableLink() const {
    return !_isLargeFile;
}

// ============================================================================
// File Monitoring
// ============================================================================

void Buffer::startMonitoring() {
    if (_isMonitoringOn) return;

    _isMonitoringOn = true;

    // Create file watcher if needed
    if (!_fileWatcher) {
        _fileWatcher = new QFileSystemWatcher(this);
        connect(_fileWatcher, &QFileSystemWatcher::fileChanged,
                this, [this](const QString& path) {
            Q_UNUSED(path);
            checkFileState();
        });
    }

    if (!_fullPathName.isEmpty()) {
        _fileWatcher->addPath(_fullPathName);
    }
}

void Buffer::stopMonitoring() {
    if (!_isMonitoringOn) return;

    _isMonitoringOn = false;

    if (_fileWatcher && !_fullPathName.isEmpty()) {
        _fileWatcher->removePath(_fullPathName);
    }
}

// ============================================================================
// Backup System
// ============================================================================

void Buffer::setTabCreatedTimeStringFromBackup() {
    if (!_isFromNetwork && _currentStatus == DocFileStatus::DOC_UNNAMED) {
        _tabCreatedTimeString = getFileTimeString(FileTimeType::Created);
    }
}

void Buffer::setTabCreatedTimeStringWithCurrentTime() {
    if (_currentStatus == DocFileStatus::DOC_UNNAMED) {
        _tabCreatedTimeString = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
}

// ============================================================================
// Comment Symbols
// ============================================================================

QString Buffer::getCommentLineSymbol() const {
    // Get from LanguageManager
    std::string sym = LanguageManager::instance().getCommentLine(_langType);
    return QString::fromStdString(sym);
}

QString Buffer::getCommentStart() const {
    std::string start = LanguageManager::instance().getCommentStart(_langType);
    return QString::fromStdString(start);
}

QString Buffer::getCommentEnd() const {
    std::string end = LanguageManager::instance().getCommentEnd(_langType);
    return QString::fromStdString(end);
}

// ============================================================================
// Status
// ============================================================================

void Buffer::setStatus(DocFileStatus status) {
    if (_currentStatus != status) {
        _currentStatus = status;
        doNotify(static_cast<int>(BufferStatusInfo::BufferChangeStatus));
        emit statusChanged(status);
    }
}

// ============================================================================
// Private Helpers
// ============================================================================

bool Buffer::isNetworkPath(const QString& path) const {
    return path.startsWith("//") || path.startsWith("\\\\")
        || path.startsWith("smb://", Qt::CaseInsensitive)
        || path.startsWith("nfs://", Qt::CaseInsensitive)
        || path.startsWith("sftp://", Qt::CaseInsensitive);
}
