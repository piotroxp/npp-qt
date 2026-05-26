// MISC/Process/Processus.cpp - Qt6 port of Notepad++ process utilities
#include "Processus.h"
#include <QProcess>
#include <QMessageBox>

void Process::run(bool isElevationRequired) const
{
    Q_UNUSED(isElevationRequired); // On Linux, elevation is handled differently
    
    QStringList args = _args.isEmpty() ? QStringList() : _args.split(' ', Qt::SkipEmptyParts);
    QProcess* proc = new QProcess();
    proc->setWorkingDirectory(_curDir);
    proc->start(_command, args);
}

unsigned long Process::runSync(bool isElevationRequired) const
{
    Q_UNUSED(isElevationRequired);
    
    QProcess proc;
    proc.setWorkingDirectory(_curDir);
    
    QStringList args = _args.isEmpty() ? QStringList() : _args.split(' ', Qt::SkipEmptyParts);
    proc.start(_command, args);
    
    if (!proc.waitForFinished(-1)) {
        throw QString("Process failed: ") + proc.errorString();
    }
    
    return proc.exitCode();
}