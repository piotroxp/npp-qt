// FindCharsInRange.h - Qt port
#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include "ScintillaEditView.h"
// Forward declaration - define intPtr_t if not available
#ifndef intPtr_t
using intPtr_t = qintptr;
#endif

class FindCharsInRange : public QDialog {
    Q_OBJECT
public:
    FindCharsInRange() : QDialog() { _charFrom = 'a'; _charTo = 'z'; _rangePos = 0; _scanOffset = 0; }
    void init(void* hInst, void* hParent, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); Q_UNUSED(hParent); _ppEditView = ppEditView; }
    void doDialog();
    void findAllCharsIn(bool next);  // Fixed: removed space in method name
    
private slots:
    void findNextChar();
    
private:
    ScintillaEditView** _ppEditView = nullptr;
    QLineEdit* _charFromEdit = nullptr;
    QLineEdit* _charToEdit = nullptr;
    QSpinBox* _rangeSpin = nullptr;
    char _charFrom = 0;
    char _charTo = 0;
    size_t _rangePos = 0;
    size_t _scanOffset = 0;
};