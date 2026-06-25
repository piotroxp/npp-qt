// Semantic Lift: Win32 RunMacroDlg → Qt6 RunMacroDlg
// Original: PowerEditor/src/WinControls/shortcut/RunMacroDlg.h
// Target: npp-qt/src/WinControls/RunMacroDlg.h

#pragma once

#include "StaticDialog.h"
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

// Dialog ID
#define IDD_RUN_MACRO_DLG 1900

// Control IDs
#define IDC_MACRO_COMBO      (IDD_RUN_MACRO_DLG + 1)
#define IDC_M_RUN_MULTI       (IDD_RUN_MACRO_DLG + 2)
#define IDC_M_RUN_TIMES        (IDD_RUN_MACRO_DLG + 3)
#define IDC_M_RUN_EOF          (IDD_RUN_MACRO_DLG + 4)
#define IDC_MACRO2RUN_STATIC  (IDD_RUN_MACRO_DLG + 5)
#define IDC_TIMES_STATIC      (IDD_RUN_MACRO_DLG + 6)

// =============================================================================
// RunMacroDlg — "Run Macro" dialog
// Shows list of defined macros, allows run-count configuration.
// Mirrors Win32 RunMacroDlg with a QDialog + signal-based API.
// =============================================================================

class RunMacroDlg : public StaticDialog
{
    Q_OBJECT

public:
    RunMacroDlg() = default;
    ~RunMacroDlg() override = default;

    // Show the dialog (mirrors Win32 doDialog)
    void doDialog(bool isRTL = false);

    // Populate the macro combo box
    void initMacroList();

    // Check whether "run multiple times" is selected
    bool isMulti() const { return _multiCheckBox && _multiCheckBox->isChecked(); }

    // Get run count
    int getTimes() const { return _times; }

    // Get selected macro index
    int getMacro2Exec() const;

signals:
    // Emitted when user selects Run
    void runMacro(int macroIndex, int times);

    // Emitted when cancelled
    void cancelled();

public slots:
    void onRunClicked();
    void onCancelClicked();
    void onMacroSelected(int index);
    void onTimesChanged(const QString& text);
    void onMultiToggled(bool checked);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    QComboBox* _macroCombo = nullptr;
    QSpinBox* _timesSpin = nullptr;
    QCheckBox* _multiCheckBox = nullptr;
    QCheckBox* _eofCheckBox = nullptr;
    QLabel* _timesLabel = nullptr;
    QLabel* _macroLabel = nullptr;

    int _times = 1;
    int _macroIndex = 0;
};
