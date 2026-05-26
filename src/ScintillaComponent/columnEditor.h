// columnEditor.h - Qt port
#pragma once
#include <QDialog>
#include <QTableWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include "ScintillaEditView.h"
enum Order { Incr = 0, Decr = 1, Insert = 2 };
enum Action { Fill_Every_N = 0, Fill_On_Selection = 1, Fill_On_All_Found = 2, Delete = 3, Revert = 4 };
class columnEditor : public QDialog {
    Q_OBJECT
public:
    columnEditor() : QDialog() { _fillColumn = 0; _fillIncDec = 0; _numericValue = 0; _numericStep = 1; _nbRows = 0; _currentIndex = 0; }
    void init(HINSTANCE hInst, HWND hParent, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); Q_UNUSED(hParent); _ppEditView = ppEditView; }
    void doModal(bool isRTL = false);
protected:
    int run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) { Q_UNUSED(message); Q_UNUSED(wParam); Q_UNUSED(lParam); return 0; }
private slots:
    void fill(); void insertNumber(); void deleteCol(); void revert();
private:
    ScintillaEditView** _ppEditView = nullptr;
    int _fillColumn = 0;
    int _fillIncDec = 0;
    int _numericValue = 0;
    int _numericStep = 0;
    int _nbRows = 0;
    int _currentIndex = 0;
};
