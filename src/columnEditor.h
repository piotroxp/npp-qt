#ifndef COLUMN_EDITOR_H
#define COLUMN_EDITOR_H

// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include <QDialog>
#include "Parameters.h"  // NumBase, ColumnEditorParam, NppGUI
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "ScintillaComponent.h"
#include "NppDarkMode.h"
// NumBase and ColumnEditorParam are defined in Parameters.h

class ColumnEditorDlg : public QDialog
{
    Q_OBJECT
public:
    ColumnEditorDlg() : QDialog() {}
    void init(QWidget* parent, ScintillaComponent** ppEditView);
    void doDialog(bool isRTL = false);

public slots:
    void onRadioToggled(bool textMode);
    void onInsert();

private:
    ScintillaComponent** _ppEditView = nullptr;
    QRadioButton* _radioText = nullptr;
    QRadioButton* _radioNumeric = nullptr;
    QLineEdit* _textEdit = nullptr;
    QSpinBox* _initNumSpin = nullptr;
    QSpinBox* _stepSpin = nullptr;
    QSpinBox* _repeatSpin = nullptr;
    QComboBox* _formatCombo = nullptr;
    QComboBox* _leadingCombo = nullptr;
};

#endif // COLUMN_EDITOR_H
