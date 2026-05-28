#include "UserDefineDialog.h"

void UserDefineDialog::initWithoutScintilla() { _started = true; }
void UserDefineDialog::createDialog(bool isRTL) { Q_UNUSED(isRTL); _started = true; show(); }
void UserDefineDialog::setScintilla(ScintillaEditView* pScintilla) { Q_UNUSED(pScintilla); }
void UserDefineDialog::doDialog(bool toShow, bool isRTL) { Q_UNUSED(isRTL); _started = true; setVisible(toShow); }
void UserDefineDialog::styleChanged() {}
void UserDefineDialog::styleUpdated(const wchar_t* styleName) { Q_UNUSED(styleName); }
