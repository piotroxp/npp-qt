// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include "columnEditor.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QMessageBox>

void ColumnEditorDlg::init(void* /*hInst*/, QWidget*& parent, ScintillaComponent** ppEditView)
{
    if (!ppEditView) throw std::runtime_error("ColumnEditorDlg::init: ppEditView is null.");
    _ppEditView = ppEditView;
    setWindowTitle("Column Editor");
    setModal(true);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    _radioText = new QRadioButton("Text", this);
    _radioNumeric = new QRadioButton("Numeric", this);
    _radioText->setChecked(true);
    QHBoxLayout* radioLayout = new QHBoxLayout;
    radioLayout->addWidget(_radioText);
    radioLayout->addWidget(_radioNumeric);
    radioLayout->addStretch();

    _textEdit = new QLineEdit(this);
    _textEdit->setPlaceholderText("Enter text to insert");

    QFormLayout* numLayout = new QFormLayout;
    _initNumSpin = new QSpinBox(this);
    _initNumSpin->setRange(0, 99999999);
    _initNumSpin->setValue(0);
    _stepSpin = new QSpinBox(this);
    _stepSpin->setRange(-99999999, 99999999);
    _stepSpin->setValue(1);
    _repeatSpin = new QSpinBox(this);
    _repeatSpin->setRange(1, 99999999);
    _repeatSpin->setValue(1);
    _formatCombo = new QComboBox(this);
    _formatCombo->addItems({"Decimal", "Binary", "Octal", "Hexadecimal"});
    _leadingCombo = new QComboBox(this);
    _leadingCombo->addItems({"None", "Space", "Zero"});
    numLayout->addRow("Initial:", _initNumSpin);
    numLayout->addRow("Step:", _stepSpin);
    numLayout->addRow("Repeat:", _repeatSpin);
    numLayout->addRow("Format:", _formatCombo);
    numLayout->addRow("Leading:", _leadingCombo);
    numLayout->setEnabled(false); // disabled until numeric selected

    QPushButton* insertBtn = new QPushButton("Insert", this);
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(insertBtn, &QPushButton::clicked, this, &ColumnEditorDlg::onInsert);
    connect(_radioText, &QRadioButton::toggled, this, &ColumnEditorDlg::onRadioToggled);
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(insertBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(radioLayout);
    mainLayout->addWidget(_textEdit);
    mainLayout->addLayout(numLayout);
    mainLayout->addLayout(btnLayout);
    NppDarkMode::instance().applyToWidget(this);
}

void ColumnEditorDlg::doDialog(bool)
{
    _textEdit->clear();
    _initNumSpin->setValue(0);
    exec();
}

void ColumnEditorDlg::onRadioToggled(bool textMode)
{
    QFormLayout* numLayout = qobject_cast<QFormLayout*>(layout()->itemAt(1)->layout());
    if (numLayout) numLayout->setEnabled(!textMode);
    _textEdit->setEnabled(textMode);
}

void ColumnEditorDlg::onInsert()
{
    if (!_ppEditView || !*_ppEditView) { reject(); return; }
    ScintillaComponent* sci = *_ppEditView;

    if (_radioText->isChecked()) {
        QString text = _textEdit->text();
        sci->send(SCI_BEGINUNDOACTION);
        sci->send(SCI_TARGETFROMSELECTION);
        sci->send(SCI_REPLACETARGET, -1, reinterpret_cast<sptr_t>(text.toUtf8().constData()));
        sci->send(SCI_ENDUNDOACTION);
    } else {
        // Numeric column mode insertion
        int base = _formatCombo->currentIndex() + 10; // 10=dec,11=bin,12=oct,13=hex
        int init = _initNumSpin->value();
        int step = _stepSpin->value();
        int repeat = _repeatSpin->value();
        int leading = _leadingCombo->currentIndex(); // 0=none,1=space,2=zero

        // Build the string
        QString result;
        for (int i = 0; i < repeat; ++i) {
            int val = init + i * step;
            if (base == 10) result += QString::number(val);
            else if (base == 11) result += QString::number(val, 2);
            else if (base == 12) result += QString::number(val, 8);
            else result += QString::number(val, 16).toUpper();
            if (leading == 1) result += " ";
            else if (leading == 2) result += "0";
            if (i < repeat - 1) result += "\n";
        }
        sci->send(SCI_BEGINUNDOACTION);
        sci->send(SCI_TARGETFROMSELECTION);
        sci->send(SCI_REPLACETARGET, -1, reinterpret_cast<sptr_t>(result.toUtf8().constData()));
        sci->send(SCI_ENDUNDOACTION);
    }
    accept();
}
