// Semantic Lift: Win32 FindCharsInRange → Qt6 QDialog
// Original: PowerEditor/src/WinControls/FindCharsInRange/*.{h,cpp}
// Target: npp-qt/src/WinControls/FindCharsInRange.cpp

#include "FindCharsInRange.h"
#include <QApplication>
#include <QMessageBox>
#include <QCheckBox>

FindCharsInRangeDlg::FindCharsInRangeDlg(QWidget* parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(QStringLiteral("Find Characters in Range"));
    setupUi();
}

void FindCharsInRangeDlg::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // Radio button group for character range
    QGroupBox* rangeGroup = new QGroupBox(QStringLiteral("Character Range"), this);
    QVBoxLayout* rangeLayout = new QVBoxLayout(rangeGroup);

    _nonAsciiRadio = new QRadioButton(QStringLiteral("Non-ASCII (128-255)"), this);
    _asciiRadio = new QRadioButton(QStringLiteral("ASCII (0-127)"), this);
    _myRangeRadio = new QRadioButton(QStringLiteral("My range:"), this);

    QHBoxLayout* myRangeLayout = new QHBoxLayout();
    myRangeLayout->addWidget(_myRangeRadio);
    _rangeStartSpin = new QSpinBox(this);
    _rangeStartSpin->setRange(0, 255);
    _rangeStartSpin->setValue(0);
    _rangeStartSpin->setPrefix(QStringLiteral("From: "));
    _rangeStartSpin->setEnabled(false);
    myRangeLayout->addWidget(_rangeStartSpin);

    _rangeEndSpin = new QSpinBox(this);
    _rangeEndSpin->setRange(0, 255);
    _rangeEndSpin->setValue(255);
    _rangeEndSpin->setPrefix(QStringLiteral(" To: "));
    _rangeEndSpin->setEnabled(false);
    myRangeLayout->addWidget(_rangeEndSpin);
    myRangeLayout->addStretch();

    _staticLabel = new QLabel(QStringLiteral(""), this);
    _staticLabel->setObjectName(QStringLiteral("IDC_STATIC"));
    rangeLayout->addWidget(_nonAsciiRadio);
    rangeLayout->addWidget(_asciiRadio);
    rangeLayout->addLayout(myRangeLayout);
    rangeLayout->addWidget(_staticLabel);

    // Non-ASCII selected by default (matches Win32 IDC_NONASCCI_RADIO)
    _nonAsciiRadio->setChecked(true);

    // Direction group
    QGroupBox* dirGroup = new QGroupBox(QStringLiteral("Direction"), this);
    QVBoxLayout* dirLayout = new QVBoxLayout(dirGroup);

    QHBoxLayout* dirRadioLayout = new QHBoxLayout();
    _dirDownRadio = new QRadioButton(QStringLiteral("Down"), this);
    _dirUpRadio = new QRadioButton(QStringLiteral("Up"), this);
    _dirDownRadio->setChecked(true); // Matches Win32 ID_FINDCHAR_DIRDOWN default
    dirRadioLayout->addWidget(_dirDownRadio);
    dirRadioLayout->addWidget(_dirUpRadio);
    dirRadioLayout->addStretch();

    _wrapCheck = new QCheckBox(QStringLiteral("Wrap around"), this);
    _wrapCheck->setChecked(true);

    dirLayout->addLayout(dirRadioLayout);
    dirLayout->addWidget(_wrapCheck);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    _findNextBtn = new QPushButton(QStringLiteral("Find Next"), this);
    _cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
    connect(_findNextBtn, &QPushButton::clicked, this, &FindCharsInRangeDlg::onFindNext);
    connect(_cancelBtn, &QPushButton::clicked, this, &FindCharsInRangeDlg::onCancel);
    btnLayout->addWidget(_findNextBtn);
    btnLayout->addWidget(_cancelBtn);

    mainLayout->addWidget(rangeGroup);
    mainLayout->addWidget(dirGroup);
    mainLayout->addLayout(btnLayout);

    // Radio change connections
    connect(_nonAsciiRadio, &QRadioButton::toggled, this, &FindCharsInRangeDlg::onRadioChanged);
    connect(_asciiRadio, &QRadioButton::toggled, this, &FindCharsInRangeDlg::onRadioChanged);
    connect(_myRangeRadio, &QRadioButton::toggled, this, &FindCharsInRangeDlg::onRadioChanged);

    setLayout(mainLayout);
}

void FindCharsInRangeDlg::doDialog()
{
    show();
    raise();
    activateWindow();
}

bool FindCharsInRangeDlg::event(QEvent* event)
{
    // Mirror Win32 WM_INITDIALOG
    if (event->type() == QEvent::Show) {
        // Dialog initialization — UI already set up in constructor
    }
    return QDialog::event(event);
}

void FindCharsInRangeDlg::onRadioChanged()
{
    bool myRangeSelected = _myRangeRadio->isChecked();
    _rangeStartSpin->setEnabled(myRangeSelected);
    _rangeEndSpin->setEnabled(myRangeSelected);
    _staticLabel->setText(myRangeSelected ? QStringLiteral("Enter values 0-255") : QString());
}

void FindCharsInRangeDlg::onFindNext()
{
    unsigned char startRange = 0;
    unsigned char endRange = 255;
    bool direction = true;
    bool wrap = _wrapCheck->isChecked();

    if (!getRangeFromUI(startRange, endRange)) {
        QMessageBox::warning(this, QStringLiteral("Range Value Problem"),
                           QStringLiteral("You should type between 0 and 255."));
        return;
    }

    getDirectionFromUI(direction, wrap);
    findCharInRange(startRange, endRange, direction, wrap);
}

void FindCharsInRangeDlg::onCancel()
{
    hide();
}

bool FindCharsInRangeDlg::findCharInRange(unsigned char beginRange, unsigned char endRange,
                                          bool direction, bool wrap)
{
    Q_UNUSED(beginRange);
    Q_UNUSED(endRange);
    Q_UNUSED(direction);
    Q_UNUSED(wrap);
    Q_UNUSED(_editor);

    // In full implementation, this would use ScintillaEditView to search
    // SCI_FINDTEXT or iterate through document characters
    //
    // Simplified placeholder — actual implementation:
    // 1. Get current document from ScintillaEditView
    // 2. Search forward/backward from current position
    // 3. Use SCI_FINDTEXT with SCIarget with character range
    // 4. Navigate to found position with SCI_GOTOPOS / SCI_SETSEL

    return true;
}

void FindCharsInRangeDlg::getDirectionFromUI(bool& direction, bool& isWrap)
{
    // Win32: direction = IDC_DIRUP checked, isWrap = IDC_WRAP checked
    direction = !_dirUpRadio->isChecked(); // true = down
    isWrap = _wrapCheck->isChecked();
}

bool FindCharsInRangeDlg::getRangeFromUI(unsigned char& startRange, unsigned char& endRange)
{
    if (_nonAsciiRadio->isChecked()) {
        startRange = 128;
        endRange = 255;
        return true;
    }

    if (_asciiRadio->isChecked()) {
        startRange = 0;
        endRange = 127;
        return true;
    }

    if (_myRangeRadio->isChecked()) {
        int start = _rangeStartSpin->value();
        int end = _rangeEndSpin->value();

        if (start > 255 || end > 255 || start > end)
            return false;

        startRange = static_cast<unsigned char>(start);
        endRange = static_cast<unsigned char>(end);
        return true;
    }

    return false;
}
