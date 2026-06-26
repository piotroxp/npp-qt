// Semantic Lift: Win32 RunMacroDlg → Qt6 RunMacroDlg
// Original: PowerEditor/src/WinControls/shortcut/RunMacroDlg.cpp
// Target: npp-qt/src/WinControls/RunMacroDlg.cpp

#include "RunMacroDlg.h"
#include "NppDarkMode.h"
#include "NppConstants.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QSpinBox>

// TODO: depends on NppParameters, ScintillaEditView, MacroShortcut
// These are stubbed for now; real implementation needs those classes.

void RunMacroDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    if (!isVisible()) {
        create(IDD_RUN_MACRO_DLG, isRTL);
    } else {
        // Reload macro list in case it changed
        initMacroList();
    }
    show();
    raise();
    activateWindow();
}

void RunMacroDlg::initMacroList()
{
    if (!isVisible()) return;

    if (_macroCombo) {
        _macroCombo->clear();
        // TODO: Get macro list from NppParameters::getInstance()
        // For now, add a placeholder
        _macroCombo->addItem(QStringLiteral("No macros available (stub)"));
    }
}

void RunMacroDlg::onRunClicked()
{
    emit runMacro(_macroIndex, _times);
    hide();
}

void RunMacroDlg::onCancelClicked()
{
    emit cancelled();
    hide();
}

void RunMacroDlg::onMacroSelected(int index)
{
    _macroIndex = index;
}

void RunMacroDlg::onTimesChanged(const QString& text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (ok && val >= 1) {
        _times = val;
    } else {
        _times = 1;
    }
}

void RunMacroDlg::onMultiToggled(bool checked)
{
    if (_timesSpin) {
        _timesSpin->setEnabled(checked);
    }
    if (_timesLabel) {
        _timesLabel->setEnabled(checked);
    }
}

int RunMacroDlg::getMacro2Exec() const
{
    // Mirrors Win32: if current recorded macro is present, index is shifted by -1
    // TODO: depends on NppParameters::getInstance().getMacroList()
    return _macroIndex;
}

intptr_t RunMacroDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
        case WM_SHOWWINDOW: {
            NppDarkMode::instance().applyToWidget(window());

            // Create child controls if not yet created
            if (!_macroCombo) {
                QVBoxLayout* mainLayout = new QVBoxLayout(window());
                mainLayout->setContentsMargins(12, 12, 12, 12);
                mainLayout->setSpacing(8);

                // Macro label + combo
                _macroLabel = new QLabel(QStringLiteral("Macro:"), window());
                _macroCombo = new QComboBox(window());
                _macroCombo->setObjectName(QString::number(IDC_MACRO_COMBO));

                // Multi-run section
                _multiCheckBox = new QCheckBox(QStringLiteral("Run multiple times"), window());
                _multiCheckBox->setObjectName(QString::number(IDC_M_RUN_MULTI));
                _multiCheckBox->setChecked(true);

                _timesLabel = new QLabel(QStringLiteral("Times:"), window());
                _timesSpin = new QSpinBox(window());
                _timesSpin->setObjectName(QString::number(IDC_M_RUN_TIMES));
                _timesSpin->setMinimum(1);
                _timesSpin->setMaximum(9999);
                _timesSpin->setValue(_times);

                // Run button
                QPushButton* runBtn = new QPushButton(QStringLiteral("Run"), window());
                runBtn->setDefault(true);
                QPushButton* cancelBtn = new QPushButton(QStringLiteral("Cancel"), window());

                // Layout
                mainLayout->addWidget(_macroLabel);
                mainLayout->addWidget(_macroCombo);
                mainLayout->addWidget(_multiCheckBox);

                QHBoxLayout* timesLayout = new QHBoxLayout();
                timesLayout->addWidget(_timesLabel);
                timesLayout->addWidget(_timesSpin);
                mainLayout->addLayout(timesLayout);

                mainLayout->addStretch();
                mainLayout->addWidget(runBtn);
                mainLayout->addWidget(cancelBtn);

                // Connections
                connect(runBtn, &QPushButton::clicked, this, &RunMacroDlg::onRunClicked);
                connect(cancelBtn, &QPushButton::clicked, this, &RunMacroDlg::onCancelClicked);
                connect(_macroCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                        this, &RunMacroDlg::onMacroSelected);
                connect(_multiCheckBox, &QCheckBox::toggled,
                        this, &RunMacroDlg::onMultiToggled);
                connect(_timesSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                        this, [this](int val) { _times = val; });

                // Init macro list
                initMacroList();
            }

            return TRUE;
        }

        default:
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}
