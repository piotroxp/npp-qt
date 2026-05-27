// FindReplaceDlg.h - Qt port
#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include "ScintillaEditView.h"
enum DIALOG_TYPE { FIND_DLG, REPLACE_DLG, FINDINFILES_DLG, FINDINFILES_DLG2, MARK_DLG };
#define DIR_DOWN true
#define DIR_UP false
enum InWhat { ALL_OPEN_DOCS, FILES_IN_DIR, CURRENT_DOC, CURR_DOC_SELECTION, FILES_IN_PROJECTS };
enum SearchIncrementalType { NotIncremental, FirstIncremental, NextIncremental };
enum SearchType { FindNormal, FindExtended, FindRegex };
enum ProcessOperation { ProcessFindAll, ProcessReplaceAll, ProcessCountAll, ProcessMarkAll, ProcessMarkAll_2, ProcessMarkAll_IncSearch, ProcessMarkAllExt, ProcessFindInFinder };
struct FindOption {
    bool _isWholeWord = true; bool _isMatchCase = true; bool _isWrapAround = true; bool _whichDirection = DIR_DOWN;
    SearchIncrementalType _incrementalType = NotIncremental; SearchType _searchType = FindNormal;
    bool _doPurge = false; bool _doMarkLine = false; bool _isInSelection = false;
    std::wstring _str2Search; std::wstring _str4Replace; std::wstring _filters; std::wstring _directory;
    bool _isRecursive = true; bool _isInHiddenDir = false;
    bool _isProjectPanel_1 = false; bool _isProjectPanel_2 = false; bool _isProjectPanel_3 = false;
    bool _dotMatchesNewline = false; bool _isMatchLineNumber = false;
};
class Finder : public QWidget { Q_OBJECT public: Finder() { _scintView.init(nullptr, nullptr); } ScintillaEditView _scintView; };
class FindReplaceDlg : public QDialog {
    Q_OBJECT
public:
    static FindOption _options;
    static FindOption* _env;
    FindReplaceDlg();
    ~FindReplaceDlg() override;
    void init(HINSTANCE hInst, HWND hPere, ScintillaEditView** ppEditView);
    void create(int dialogID, bool isRTL = false, bool msgDestParent = true, bool toShow = true);
    void initOptionsFromDlg();
    void doDialog(DIALOG_TYPE whichType, bool isRTL = false, bool toShow = true);
    bool processFindNext(const wchar_t* txt2find, const FindOption* options = NULL, int* oFindStatus = NULL, int findNextType = 0);
    bool processReplace(const wchar_t* txt2find, const wchar_t* txt2replace, const FindOption* options = NULL);
    int markAll(const wchar_t* txt2find, int styleID);
    int processAll(ProcessOperation op, const FindOption* opt, bool isEntire = false, void* pFindersInfo = nullptr, int colourStyleID = -1);
    void replaceAllInOpenedDocs();
    void findAllIn(InWhat op);
    void setSearchText(const wchar_t* txt2find);
    Finder* getMainFinder() const { return _pFinder; }
    DIALOG_TYPE getCurrentStatus() { return _currentStatus; }
    void saveFindHistory();
protected:
    intptr_t run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
private:
    QTabWidget* _tab = nullptr;
    QLineEdit* _findCombo = nullptr;
    QLineEdit* _replaceCombo = nullptr;
    QCheckBox* _wholeWordCheck = nullptr;
    QCheckBox* _matchCaseCheck = nullptr;
    QCheckBox* _wrapAroundCheck = nullptr;
    QComboBox* _directionCombo = nullptr;
    QComboBox* _searchModeCombo = nullptr;
    QPushButton* _findNextBtn = nullptr;
    QPushButton* _replaceBtn = nullptr;
    QPushButton* _replaceAllBtn = nullptr;
    Finder* _pFinder = nullptr;
    DIALOG_TYPE _currentStatus = FIND_DLG;
    ScintillaEditView** _ppEditView = nullptr;
    void enableFindDlgItem(int dlgItemID, bool isEnable = true);
};
