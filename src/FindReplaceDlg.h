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
#include "ScintillaComponent.h"
#include "Buffer.h"
#include <map>
#include <string>
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

class FindReplaceDlg : public QWidget
{
    Q_OBJECT
public:
    FindReplaceDlg();
    ~FindReplaceDlg() override;

    // getHSelf() returns the widget handle (mirrors Win32 getHSelf())
    QWidget* getHSelf() const { return const_cast<FindReplaceDlg*>(this); }

    void init(QWidget* parent, ScintillaComponent** ppEditView);
    void doDialog(DIALOG_TYPE whichType, bool isRTL = false);
    void create(DIALOG_TYPE whichType);

    // Find operations
    bool processFindNext(const wchar_t* txt2find, const FindOption* options = nullptr,
                          FindStatus* oFindStatus = nullptr);
    bool processReplace(const wchar_t* txt2find, const wchar_t* txt2replace,
                         const FindOption* options = nullptr);
    int markAll(const wchar_t* txt2find, int styleID);
    int processAll(ProcessOperation op, const FindOption* opt, bool isEntire = false);
    int processRange(ProcessOperation op, const std::wstring& findReplaceInfo,
                     const FindOption* opt, int colourStyleID, int extraArg,
                     ScintillaComponent* view2Process);
    int processRange(ProcessOperation op, const FindReplaceInfo& fri,
                     const FindOption* opt, int colourStyleID = -1,
                     ScintillaComponent* view2Process = nullptr);

    void replaceAllInOpenedDocs();
    void setSearchText(const wchar_t* txt2find);
    void setFindInFilesDirFilter(const wchar_t* dir, const wchar_t* filters);
    void enableFindInFilesControls(bool, bool);
    void saveFindHistory();

    // Results
    void beginNewFilesSearch();
    void finishFilesSearch(int count, int searchedCount, bool searchedEntireNotSelection);
    void focusOnFinder();
    void clearAllFinderResults();

    const std::wstring& getText2search() const;
    const FindOption& getCurrentOptions() const { return _options; }
    static FindOption _options;
    // Win32 compat: _env was a pointer default; use static _options as default
    static FindOption* _env;

signals:
    void searchCompleted(int count);

public slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onFindAll();
    void onMarkAll();
    void onClose();

private:
    ScintillaComponent** _ppEditView = nullptr;
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

    std::vector<FoundInfo> _foundInfos;
    bool _isRTL = false;
    QPushButton* _closeBtn = nullptr;

    void setupFindTab();
    void setupReplaceTab();
    void updateStatus(const QString& message, FindStatus status);
    int findInFiles(const FindOption& opt);
};
