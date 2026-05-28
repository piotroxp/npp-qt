#include "FindReplaceDlg.h"

FindOption FindReplaceDlg::_options;
FindOption* FindReplaceDlg::_env = &FindReplaceDlg::_options;

FindReplaceDlg::FindReplaceDlg() = default;
FindReplaceDlg::~FindReplaceDlg() = default;
void FindReplaceDlg::init(HINSTANCE hInst, HWND hPere, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); Q_UNUSED(hPere); _ppEditView = ppEditView; }
void FindReplaceDlg::create(int dialogID, bool isRTL, bool msgDestParent, bool toShow) { Q_UNUSED(dialogID); Q_UNUSED(isRTL); Q_UNUSED(msgDestParent); setVisible(toShow); }
void FindReplaceDlg::initOptionsFromDlg() {}
void FindReplaceDlg::doDialog(DIALOG_TYPE whichType, bool isRTL, bool toShow) { _currentStatus = whichType; Q_UNUSED(isRTL); setVisible(toShow); }
bool FindReplaceDlg::processFindNext(const wchar_t* txt2find, const FindOption* options, int* oFindStatus, int findNextType) { Q_UNUSED(txt2find); Q_UNUSED(options); Q_UNUSED(oFindStatus); Q_UNUSED(findNextType); return false; }
bool FindReplaceDlg::processReplace(const wchar_t* txt2find, const wchar_t* txt2replace, const FindOption* options) { Q_UNUSED(txt2find); Q_UNUSED(txt2replace); Q_UNUSED(options); return false; }
int FindReplaceDlg::markAll(const wchar_t* txt2find, int styleID) { Q_UNUSED(txt2find); Q_UNUSED(styleID); return 0; }
int FindReplaceDlg::processAll(ProcessOperation op, const FindOption* opt, bool isEntire, void* pFindersInfo, int colourStyleID) { Q_UNUSED(op); Q_UNUSED(opt); Q_UNUSED(isEntire); Q_UNUSED(pFindersInfo); Q_UNUSED(colourStyleID); return 0; }
void FindReplaceDlg::replaceAllInOpenedDocs() {}
void FindReplaceDlg::findAllIn(InWhat op) { Q_UNUSED(op); }
void FindReplaceDlg::setSearchText(const wchar_t* txt2find) { Q_UNUSED(txt2find); }
void FindReplaceDlg::saveFindHistory() {}
intptr_t FindReplaceDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) { Q_UNUSED(message); Q_UNUSED(wParam); Q_UNUSED(lParam); return 0; }
