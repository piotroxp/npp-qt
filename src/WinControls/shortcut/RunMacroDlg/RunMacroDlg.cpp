// RunMacroDlg.cpp — Qt6 translation of macro run dialog
#include "RunMacroDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

RunMacroDlg::RunMacroDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Run Macro");
    setModal(true);
    resize(350, 150);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Macro selection
    QLabel* macroLabel = new QLabel("Select macro to run:", this);
    mainLayout->addWidget(macroLabel);

    _macroCombo = new QComboBox(this);
    mainLayout->addWidget(_macroCombo);

    // Repeat options
    _multiCheck = new QCheckBox("Run multiple times", this);
    mainLayout->addWidget(_multiCheck);

    QHBoxLayout* timesLayout = new QHBoxLayout;
    timesLayout->addWidget(new QLabel("Times:", this));
    _timesSpin = new QSpinBox(this);
    _timesSpin->setMinimum(1);
    _timesSpin->setMaximum(9999);
    _timesSpin->setValue(1);
    timesLayout->addWidget(_timesSpin);
    timesLayout->addStretch();

    QGroupBox* repeatGroup = new QGroupBox(this);
    repeatGroup->setLayout(timesLayout);
    mainLayout->addWidget(repeatGroup);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    QPushButton* okBtn = new QPushButton("OK", this);
    connect(okBtn, &QPushButton::clicked, this, &RunMacroDlg::onOKClicked);
    btnLayout->addWidget(okBtn);

    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);

    connect(_multiCheck, &QCheckBox::toggled, this, &RunMacroDlg::onMultiToggled);
    connect(_macroCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RunMacroDlg::onMacroChanged);
}

RunMacroDlg::~RunMacroDlg() = default;

void RunMacroDlg::doDialog(bool isRTL)
{
    if (isRTL)
        setLayoutDirection(Qt::RightToLeft);

    initMacroList();
    show();
}

void RunMacroDlg::initMacroList()
{
    _macroCombo->clear();
    // Add "Current recorded macro" if applicable
    _macroCombo->addItem("Current recorded macro");
    // In real impl, would add saved macros from parameters
    _macroCombo->setCurrentIndex(0);
    _macroIndex = 0;
}

int RunMacroDlg::getMacroToExec() const
{
    return _macroIndex - 1; // -1 for current macro, else index
}

void RunMacroDlg::onOKClicked()
{
    emit runMacro(getMacroToExec(), getTimes());
    accept();
}

void RunMacroDlg::onCancelClicked()
{
    reject();
}

void RunMacroDlg::onMultiToggled(bool checked)
{
    _timesSpin->setEnabled(checked);
    if (checked)
        _times = _timesSpin->value();
    else
        _times = 1;
}

void RunMacroDlg::onMacroChanged(int index)
{
    _macroIndex = index;
}

int RunMacroDlg::run_dlgProc(QEvent* event)
{
    Q_UNUSED(event);
    return 0;
}

bool RunMacroDlg::event(QEvent* event)
{
    return QDialog::event(event);
}
