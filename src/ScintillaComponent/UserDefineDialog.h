// UserDefineDialog.h - Qt port
#pragma once
#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "ScintillaEditView.h"

class UserDefineDialog : public QDialog {
    Q_OBJECT
public:
    UserDefineDialog() : QDialog() {}
    void init(HINSTANCE hInst, HWND hParent, ScintillaEditView** pEditView) { Q_UNUSED(hInst); Q_UNUSED(hParent); _pEditView = pEditView; }
    void initWithoutScintilla();
    void createDialog(bool isRTL = true);
    bool isDialogStarted() const { return _started; }
    void destroy() { hide(); close(); }
    void setScintilla(ScintillaEditView* pScintilla);
    void doDialog(bool toShow, bool isRTL);
    void styleChanged();
    void styleUpdated(const wchar_t* styleName);
private:
    ScintillaEditView** _pEditView = nullptr;
    int _currentIndex = 0;
    int _currentTheme = 0;
    bool _started = false;
    enum { WORDList1 = 0, WORDList2, WORDList3, WORDList4, WORDList5, WORDList6, WORDList7, WORDList8 };
};
