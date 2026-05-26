// GoToLineDlg.h - Qt port
#pragma once
#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "ScintillaEditView.h"
class ScintillaEditView;
class GoToLineDlg : public QDialog {
    Q_OBJECT
public:
    GoToLineDlg() = default;
    void init(HINSTANCE hInst, HWND hPere, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); Q_UNUSED(hPere); _ppEditView = ppEditView; }
    void doDialog(bool isRTL = false);
    void display(bool toShow = true) override;
    void updateLinesNumbers() const;
protected:
    enum mode { go2line, go2offsset };
    mode _mode = go2line;
    intptr_t run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    ScintillaEditView** _ppEditView = nullptr;
    QLineEdit* _lineEdit = nullptr;
    QLabel* _maxLabel = nullptr;
    long long getLine() const {
        if (!_lineEdit) return -1;
        return _lineEdit->text().toLongLong();
    }
};
