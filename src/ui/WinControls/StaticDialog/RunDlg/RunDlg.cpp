// RunDlg.cpp - Qt6 "Run" dialog
#include "RunDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcessEnvironment>

RunDlg::RunDlg(QWidget* parent)
    : StaticDialog(parent) {
    setupUi();
    readSettings();
}

void RunDlg::setupUi() {
    setWindowTitle(tr("Run"));
    setMinimumWidth(400);
    auto* layout = new QVBoxLayout(this);

    auto* cmdLayout = new QHBoxLayout();
    cmdLayout->addWidget(new QLabel(tr("Command:")));
    _cmdEdit = new QLineEdit(this);
    _cmdEdit->setObjectName("RunCommandEdit");
    cmdLayout->addWidget(_cmdEdit, 1);
    layout->addLayout(cmdLayout);

    auto* btnLayout = new QHBoxLayout();
    auto* browseBtn = new QPushButton(tr("Browse..."), this);
    connect(browseBtn, &QPushButton::clicked, this, &RunDlg::onBrowse);
    btnLayout->addWidget(browseBtn);
    btnLayout->addStretch();

    auto* runBtn = new QPushButton(tr("Run"), this);
    runBtn->setDefault(true);
    connect(runBtn, &QPushButton::clicked, this, &RunDlg::onRun);
    btnLayout->addWidget(runBtn);

    auto* closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(closeBtn);

    layout->addLayout(btnLayout);

    _process = new QProcess(this);
    connect(_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &RunDlg::onProcessFinished);
}

void RunDlg::readSettings() {
    // TODO: Load from QSettings
}

void RunDlg::saveSettings() {
    // TODO: Save to QSettings
}

void RunDlg::onBrowse() {
    QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
        QString(), tr("Executables (*);;All Files (*)"));
    if (!file.isEmpty()) {
        _cmdEdit->setText(file);
    }
}

void RunDlg::onRun() {
    QString cmd = _cmdEdit->text().trimmed();
    if (cmd.isEmpty()) {
        QMessageBox::warning(this, tr("Run"), tr("Please enter a command."));
        return;
    }
    if (_process->state() != QProcess::NotRunning) {
        QMessageBox::information(this, tr("Run"), tr("A process is already running."));
        return;
    }
    // Extract working directory from command path
    QStringList args;
    QString program;
    if (cmd.startsWith('"')) {
        int endQuote = cmd.indexOf('"', 1);
        if (endQuote > 0) {
            program = cmd.mid(1, endQuote - 1);
            args = cmd.mid(endQuote + 1).trimmed().split(' ', Qt::SkipEmptyParts);
        }
    } else {
        args = cmd.split(' ', Qt::SkipEmptyParts);
        program = args.takeFirst();
    }

    _process->setWorkingDirectory(_lastWorkingDir.isEmpty() ? QCoreApplication::applicationDirPath()
                                                            : _lastWorkingDir);
    _process->start(program, args);
}

void RunDlg::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
    QString output = QString::fromLocal8Bit(_process->readAllStandardOutput());
    QString error = QString::fromLocal8Bit(_process->readAllStandardError());
    QString msg = tr("Exit code: %1\n%2").arg(exitCode).arg(output);
    if (!error.isEmpty()) {
        msg += tr("\nStderr:\n%1").arg(error);
    }
    QMessageBox::information(this, tr("Process Finished"), msg);
}
