// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include "GoToLineDlg.h"
#include "ScintillaComponent.h"
#include "NppDarkMode.h"
#include <QCoreApplication>
#include <QIntValidator>

GoToLineDlg::GoToLineDlg(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f), _mode(go2line) {}

void GoToLineDlg::init(void* /*hInst*/, QWidget*& parent, ScintillaComponent** ppEditView)
{
    if (!ppEditView) throw std::runtime_error("GoToLineDlg::init: ppEditView is null.");
    _ppEditView = ppEditView;
    setWindowTitle("Go to Line");
    setModal(true);

    _radioLine = new QRadioButton("Go to line number", this);
    _radioOffset = new QRadioButton("Go to offset", this);
    _radioLine->setChecked(true);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setValidator(new QIntValidator(this));

    _currentLabel = new QLabel(this);
    _lastLabel = new QLabel(this);

    QLabel* curLabel = new QLabel("Current line/offset:", this);
    QLabel* lastLabel = new QLabel("Total lines/offsets:", this);

    QHBoxLayout* curLayout = new QHBoxLayout;
    curLayout->addWidget(curLabel);
    curLayout->addWidget(_currentLabel);
    curLayout->addStretch();

    QHBoxLayout* lastLayout = new QHBoxLayout;
    lastLayout->addWidget(lastLabel);
    lastLayout->addWidget(_lastLabel);
    lastLayout->addStretch();

    QPushButton* okBtn = new QPushButton("OK", this);
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(_radioLine);
    layout->addWidget(_radioOffset);
    layout->addWidget(_lineEdit);
    layout->addLayout(curLayout);
    layout->addLayout(lastLayout);
    layout->addLayout(btnLayout);

    connect(_radioLine, &QRadioButton::toggled, this, &GoToLineDlg::onRadioToggled);
    connect(_radioOffset, &QRadioButton::toggled, this, &GoToLineDlg::onRadioToggled);

    NppDarkMode::instance().applyToWidget(this);
}

void GoToLineDlg::doDialog(bool)
{
    updateLinesNumbers();
    _lineEdit->clear();
    _lineEdit->setFocus();
    exec();
}

long long GoToLineDlg::getLine() const
{
    bool ok = false;
    return _lineEdit->text().toLongLong(&ok, 10);
}

void GoToLineDlg::onRadioToggled(bool)
{
    _mode = _radioLine->isChecked() ? go2line : go2offset;
    updateLinesNumbers();
}

void GoToLineDlg::updateLinesNumbers()
{
    if (!_ppEditView || !*_ppEditView) return;
    ScintillaComponent* sci = *_ppEditView;

    if (_mode == go2line) {
        long long current = sci->currentLine() + 1;
        long long limit = sci->send(SCI_GETLINECOUNT);
        _currentLabel->setText(QString::number(current));
        _lastLabel->setText(QString::number(limit));
    } else {
        long long current = sci->send(SCI_GETCURRENTPOS);
        long long limit = sci->send(SCI_GETLENGTH);
        if (limit > 0) limit--;
        _currentLabel->setText(QString::number(current));
        _lastLabel->setText(QString::number(limit));
    }
}

void GoToLineDlg::accept()
{
    if (!_ppEditView || !*_ppEditView) { QDialog::reject(); return; }
    ScintillaComponent* sci = *_ppEditView;
    sci->send(SCI_SETMODEVENTMASK, 0);
    long long line = getLine();
    if (line != -1) {
        if (_mode == go2line) {
            sci->send(SCI_ENSUREVISIBLE, line - 1);
            sci->send(SCI_GOTOLINE, line - 1);
        } else {
            size_t posToGoto = 0;
            if (line > 0) {
                auto before = sci->send(SCI_POSITIONBEFORE, line);
                posToGoto = sci->send(SCI_POSITIONAFTER, before);
            }
            auto sci_line = sci->send(SCI_LINEFROMPOSITION, posToGoto);
            sci->send(SCI_ENSUREVISIBLE, sci_line);
            sci->send(SCI_GOTOPOS, posToGoto);
        }
    }
    sci->send(SCI_SETMODEVENTMASK, 0x1FF);
    QDialog::accept();
}
