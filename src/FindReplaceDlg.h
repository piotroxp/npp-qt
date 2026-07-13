// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
// FindReplaceDlg: Find/Replace dialog + Find-in-Files + Finder (results panel)
// Win32→Qt6: This is a large port (7077 lines) — full implementation pending.
// Stub provides the interface and key structures; full Qt port needed for
// the complex Finder (search results panel) and the regex search engine.
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressBar>
#include <QString>
#include <QLabel>
#include <QTextBrowser>
#include <QThread>
// Forward-declare to avoid QsciScintilla circular include dependency
class ScintillaComponent;
class Buffer;
// DockingDlgInterface is defined in DockingWnd.h — include it for the canonical definition
#include "WinControls/DockingWnd.h"
#include <map>
#include <string>
#include "NppConstants.h"
#include <vector>

#define FIND_INVALID_REGULAR_EXPRESSION -2
#define FINDREPLACE_MAXLENGTH2SAVE 2048
#define FINDTEMPSTRING_MAXSIZE 1024*1024

enum DIALOG_TYPE { FIND_DLG, REPLACE_DLG, FINDINFILES_DLG, FINDINPROJECTS_DLG, MARK_DLG };
enum InWhat { ALL_OPEN_DOCS, FILES_IN_DIR, CURRENT_DOC, CURR_DOC_SELECTION, FILES_IN_PROJECTS };
enum SearchIncrementalType { NotIncremental, FirstIncremental, NextIncremental };
enum SearchType { FindNormal, FindExtended, FindRegex };
enum ProcessOperation { ProcessFindAll, ProcessReplaceAll, ProcessCountAll,
    ProcessMarkAll, ProcessMarkAll_2, ProcessMarkAll_IncSearch, ProcessMarkAllExt, ProcessFindInFinder };
enum FindStatus { FSFound, FSNotFound, FSTopReached, FSEndReached, FSMessage, FSNoMessage, FSWarning };

struct FindOption {
    bool _isWholeWord = true;
    bool _isMatchCase = true;
    bool _isWrapAround = true;
    bool _whichDirection = true; // true=DOWN
    SearchIncrementalType _incrementalType = NotIncremental;
    SearchType _searchType = FindNormal;
    bool _doPurge = false;
    bool _doMarkLine = false;
    bool _isInSelection = false;
    std::wstring _str2Search;
    std::wstring _str4Replace;
    std::wstring _filters;
    std::wstring _directory;
    bool _isRecursive = true;
    bool _isInHiddenDir = false;
    bool _dotMatchesNewline = false;
    bool _isMatchLineNumber = false;
};

struct FindReplaceInfo {
    const wchar_t* _txt2find = nullptr;
    const wchar_t* _txt2replace = nullptr;
    intptr_t _startRange = -1;
    intptr_t _endRange = -1;
};

struct FoundInfo {
    intptr_t _start = 0, _end = 0;
    size_t _lineNumber = 0;
    std::wstring _fullPath;
    FoundInfo(intptr_t s, intptr_t e, size_t ln, const wchar_t* fp)
        : _start(s), _end(e), _lineNumber(ln), _fullPath(fp ? fp : L"") {}
};

// FindersInfo is defined in NppConstants.h (already included)
// Background worker for Find-in-Files (lives in a QThread)
class FindInFilesWorker : public QObject
{
    Q_OBJECT
public:
    explicit FindInFilesWorker(QObject* parent = nullptr);
    void configure(const QString& directory, const QString& filters,
                   const QString& searchText, const QString& replaceText,
                   bool matchCase, bool wholeWord, bool isRegex,
                   bool isRecursive, bool isInHidden, bool dotMatchesNewline,
                   bool doReplace);
    void abort();

public slots:
    void run();

signals:
    void progress(int current, int total, const QString& fileName);
    void resultReady(int line, int64_t start, int64_t end,
                     const QString& fileName, const QString& lineContent);
    void finished(int totalMatches, int totalFiles);

private:
    QString _directory, _filters, _searchText, _replaceText;
    bool _matchCase = false, _wholeWord = false, _isRegex = false;
    bool _isRecursive = true, _isInHidden = false, _dotMatchesNewline = false;
    bool _doReplace = false;
    bool _abort = false;
    int _totalMatches = 0, _totalFiles = 0;
};

class FindReplaceDlg : public QWidget
{
    Q_OBJECT
public:
    explicit FindReplaceDlg(QWidget* parent = nullptr);
    ~FindReplaceDlg() override;

    // getHSelf() returns the widget handle (mirrors Win32 getHSelf())
    QWidget* getHSelf() const { return const_cast<FindReplaceDlg*>(this); }

    void init(void* hInst, QWidget*& parent, ScintillaComponent** ppEditView);
    void doDialog(DIALOG_TYPE whichType, bool isRTL = false);
    void create(DIALOG_TYPE whichType);
    void showFifTab();

    // Find operations
    bool processFindNext(const wchar_t* txt2find, const FindOption* options = nullptr,
                          FindStatus* oFindStatus = nullptr);
    bool processReplace(const wchar_t* txt2find, const wchar_t* txt2replace,
                         const FindOption* options = nullptr);
    int markAll(const wchar_t* txt2find, int styleID);
    int processAll(ProcessOperation op, const FindOption* opt, bool isEntire = false);
    int processAll(ProcessOperation op, const FindOption* opt, bool isEntire, FindersInfo* findersInfo);
    int processAll(ProcessOperation op, const FindersInfo::FindOption* opt, bool isEntire, FindersInfo* findersInfo);
    int processRange(ProcessOperation op, const std::wstring& findReplaceInfo,
                     const FindOption* opt, int colourStyleID, int extraArg,
                     ScintillaComponent* view2Process);
    int processRange(ProcessOperation op, const FindReplaceInfo& fri,
                     const FindOption* opt, int colourStyleID = -1,
                     ScintillaComponent* view2Process = nullptr);

    int replaceAllInOpenedDocs();
    bool allowCopyAction() const { return true; }  // stub for copy find result
    void* getMainFinder();
    std::vector<void*> getFindersOfFinder();
    void setSearchText(const wchar_t* txt2find);
    void setFindInFilesDirFilter(const wchar_t* dir, const wchar_t* filters);
    void enableFindInFilesControls(bool, bool);
    // Set the current Scintilla editor — called when active tab changes
    void setCurrentEditor(ScintillaComponent* editor) { _currentEditor = editor; }
    ScintillaComponent* currentEditor() const { return _currentEditor; }
    void saveFindHistory();

    // Results
    void beginNewFilesSearch();
    void finishFilesSearch(int count, int searchedCount, bool searchedEntireNotSelection);
    void focusOnFinder();
    void clearAllFinderResults();
    // Win32 API stub: sets status bar regex error message
    void setStatusbarMessageWithRegExprErr(ScintillaComponent* view);

    const std::wstring& getText2search() const;
    const FindOption& getCurrentOptions() const { return _options; }
    static FindOption _options;
    static FindOption* _env;  // Win32 compat: pointer default for _options

    // Qt-port stubs for Notepad_plus.cpp compatibility
    void setStatusbarMessage(const QString& msg, int delayMs = 0);
    QString getDir2Search() const;
    QString getAndValidatePatterns(bool* isRecursive = nullptr, bool* isInHiddenDir = nullptr) const;
    void putFindResult(const std::vector<struct FoundInfo>& results);
    void display(bool doShow);

signals:
    void searchCompleted(int count);
    void fifFinished(int totalMatches, int totalFiles);
    void fifResultFileOpenRequested(const QString& filePath, int lineNumber);

public slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onFindAll();
    void onMarkAll();
    void onClose();

private:
    ScintillaComponent** _ppEditView = nullptr;
    ScintillaComponent* _currentEditor = nullptr;
    DIALOG_TYPE _currentStatus = FIND_DLG;
    QTabWidget* _tab = nullptr;
    QLineEdit* _findEdit = nullptr;
    QLineEdit* _replaceEdit = nullptr;
    QPushButton* _findNextBtn = nullptr;
    QPushButton* _replaceBtn = nullptr;
    QPushButton* _replaceAllBtn = nullptr;
    QWidget* _findTab = nullptr;
    QWidget* _replaceTab = nullptr;
    QWidget* _findInFilesTab = nullptr;
    QWidget* _markTab = nullptr;
    QCheckBox* _wholeWordChk = nullptr;
    QCheckBox* _matchCaseChk = nullptr;
    QCheckBox* _wrapChk = nullptr;

    std::vector<FoundInfo> _foundInfos;
    bool _isRTL = false;
    QPushButton* _closeBtn = nullptr;

    void setupFindTab();
    void setupReplaceTab();
    void updateStatus(const QString& message, FindStatus status);
    int findInFiles(const FindOption& opt);

    // Find in Files UI members
    QLineEdit* _fifDirEdit = nullptr;
    QComboBox* _filterCombo = nullptr;
    QPushButton* _fifFindBtn = nullptr;
    QPushButton* _fifReplaceBtn = nullptr;
    QToolButton* _fifBrowseBtn = nullptr;
    QPushButton* _fifCancelBtn = nullptr;
    QCheckBox* _regexChk = nullptr;
    QCheckBox* _isRecursiveChk = nullptr;
    QCheckBox* _isInHiddenChk = nullptr;
    QLineEdit* _fifReplaceEdit = nullptr;
    QTextBrowser* _fifResultsEdit = nullptr;
    QProgressBar* _fifProgressBar = nullptr;
    QLabel* _fifStatusLabel = nullptr;
    int _filesSearchedCount = 0;
    int _filesFoundCount = 0;
    QString _lastStatusMsg;

    // Find in Files methods
    void setupFindInFilesTab();
    void stopFifSearch();
    void onFifBrowse();
    void onFifCancel();
    void onFifFinished(int totalMatches, int totalFiles);
    void onWorkerProgress(int current, int total, const QString& fileName);
    void onFifResultAppend(int line, int64_t start, int64_t end,
                           const QString& fileName, const QString& lineContent);
    void startFifSearch(bool isReplace = false);
    unsigned int buildSearchFlags(const FindOption* opt) const;
    void setDir2Search(const QString& dir);
    void onFifResultClicked(const QUrl& link);

    QThread* _fifThread = nullptr;
    FindInFilesWorker* _fifWorker = nullptr;
};
