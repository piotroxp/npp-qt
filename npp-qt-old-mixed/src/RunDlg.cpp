// npp-qt: Run dialog — execute shell command and show output
#include "RunDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QScrollBar>
#include <QFileDialog>

RunDlg::RunDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Run"));
    setModal(false);
    resize(600, 400);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Command input
    QHBoxLayout* cmdLay = new QHBoxLayout;
    cmdLay->addWidget(new QLabel(QStringLiteral("Command:")));
    _commandEdit = new QLineEdit;
    _browseButton = new QPushButton(QStringLiteral("Browse..."));
    cmdLay->addWidget(_commandEdit, 1);
    cmdLay->addWidget(_browseButton);
    mainLay->addLayout(cmdLay);

    // Output
    _outputEdit = new QPlainTextEdit;
    _outputEdit->setReadOnly(true);
    mainLay->addWidget(_outputEdit, 1);

    // Buttons
    QHBoxLayout* btnLay = new QHBoxLayout;
    btnLay->addStretch();
    _runButton = new QPushButton(QStringLiteral("Run"));
    _cancelButton = new QPushButton(QStringLiteral("Close"));
    btnLay->addWidget(_runButton);
    btnLay->addWidget(_cancelButton);
    mainLay->addLayout(btnLay);

    connect(_runButton, &QPushButton::clicked, this, &RunDlg::onRun);
    connect(_cancelButton, &QPushButton::clicked, this, &RunDlg::onCancel);
    connect(_browseButton, &QPushButton::clicked, this, &RunDlg::onBrowse);
}

RunDlg::~RunDlg() = default;

void RunDlg::init(void* /*hInst*/, QWidget*& /*parent*/)
{
    // No-op: dialog fully constructed in constructor
}

void RunDlg::doDialog() { show(); raise(); }

void RunDlg::display(int, bool)
{
    show();
    raise();
    _commandEdit->setFocus();
}

void RunDlg::onRun()
{
    QString cmd = _commandEdit->text().trimmed();
    if (cmd.isEmpty()) return;

    _outputEdit->clear();
    _runButton->setEnabled(false);

    QProcess* proc = new QProcess(this);
    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc]() {
        _outputEdit->appendPlainText(QString::fromLocal8Bit(proc->readAllStandardOutput()));
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, proc]() {
        _outputEdit->appendPlainText(QStringLiteral("[stderr] ") + QString::fromLocal8Bit(proc->readAllStandardError()));
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus) {
        _outputEdit->appendPlainText(QStringLiteral("\n[Exit code: %1]").arg(exitCode));
        _runButton->setEnabled(true);
        _outputEdit->verticalScrollBar()->setValue(_outputEdit->verticalScrollBar()->maximum());
    });

    proc->start(cmd);
}

void RunDlg::onCancel() { hide(); }

void RunDlg::onBrowse()
{
    QString filter = QStringLiteral("Executables (*.exe);;All files (*)");
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Select Executable"),
                                               QString(), filter);
    if (!path.isEmpty())
        _commandEdit->setText(path);
}
