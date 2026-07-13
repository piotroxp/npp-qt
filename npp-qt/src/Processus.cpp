// npp-qt: Process management
#include "Processus.h"
#include <QDebug>

Process::Process(QObject* parent)
    : QObject(parent)
{
    _process = new QProcess(this);
    connect(_process, &QProcess::finished, this, &Process::onFinished);
    connect(_process, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(_process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
    connect(_process, &QProcess::errorOccurred, this, &Process::onErrorOccurred);
}

Process::~Process() = default;

void Process::run(const QString& command, const QStringList& args)
{
    run(QString(), command, args);
}

void Process::run(const QString& workDir, const QString& command, const QStringList& args)
{
    _curCmd = command;
    _curArgs = args;
    _curWorkDir = workDir;

    if (!workDir.isEmpty()) {
        _process->setWorkingDirectory(workDir);
    }
    _process->start(command, args);
}

void Process::kill()
{
    if (_process->state() != QProcess::NotRunning) {
        _process->kill();
    }
}

bool Process::isRunning() const
{
    return _process->state() != QProcess::NotRunning;
}

int Process::exitCode() const
{
    return _process->exitCode();
}

QString Process::readStdOut()
{
    return QString::fromLocal8Bit(_process->readAllStandardOutput());
}

QString Process::readStdErr()
{
    return QString::fromLocal8Bit(_process->readAllStandardError());
}

bool Process::waitForFinished(int timeoutMs)
{
    if (timeoutMs < 0) {
        return _process->waitForFinished();
    } else {
        return _process->waitForFinished(timeoutMs);
    }
}

Process::ExitStatus Process::exitStatus() const
{
    return _process->exitStatus() == QProcess::NormalExit ? NormalExit : CrashExit;
}

void Process::onFinished(int exitCode, QProcess::ExitStatus status)
{
    emit processFinished(exitCode, status);
}

void Process::onReadyReadStandardOutput()
{
    emit readyReadStdOut(readStdOut());
}

void Process::onReadyReadStandardError()
{
    emit readyReadStdErr(readStdErr());
}

void Process::onErrorOccurred(QProcess::ProcessError error)
{
    qWarning() << "Process error:" << error << _curCmd;
    emit errorOccurred(error);
}
