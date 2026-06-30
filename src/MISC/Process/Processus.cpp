// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// npp-qt: Win32 ShellExecute/ShellExecuteEx → Qt process launch

#include "Processus.h"
#include <QProcess>
#include <QString>
#include "Common.h"


// Split a whitespace-separated string into arguments (cross-Qt-version, no QRegExp needed).
static QStringList splitArgs(const QString& input)
{
    QStringList result;
    QString current;
    for (QChar c : input) {
        if (c.isSpace()) {
            if (!current.isEmpty()) {
                result.append(current);
                current.clear();
            }
        } else {
            current.append(c);
        }
    }
    if (!current.isEmpty()) {
        result.append(current);
    }
    return result;
}


void Process::run(bool /*isElevationRequired*/) const
{
    // Launch asynchronously via QProcess::startDetached — the OS resolves
    // the command from PATH, similar to ShellExecute(..., "open", ...).
    // Note: elevation (runas) cannot be replicated on Linux without pkexec/kdesu;
    // the bool parameter is accepted for API compatibility but ignored here.
    QString prog = QString::fromStdWString(_command);
    QStringList argList = splitArgs(QString::fromStdWString(_args));
    QString workingDir = QString::fromStdWString(_curDir);

    QProcess p;
    if (!workingDir.isEmpty()) {
        p.setWorkingDirectory(workingDir);
    }
    p.startDetached(prog, argList);
}


int Process::runSync(bool /*isElevationRequired*/) const
{
    // Synchronous launch: blocks until the process exits.
    // Mirrors ShellExecuteEx + WaitForSingleObject + GetExitCodeProcess.
    QString prog = QString::fromStdWString(_command);
    QStringList args = splitArgs(QString::fromStdWString(_args));

    QProcess p;
    if (!_curDir.empty()) {
        p.setWorkingDirectory(QString::fromStdWString(_curDir));
    }

    // QProcess::execute() blocks and returns the exit code.
    // On failure (program not found, etc.) it throws QProcess::FailedToStart.
    int exitCode = QProcess::execute(prog, args);

    if (exitCode < 0) {
        // Negative exit codes from QProcess mean abnormal termination;
        // treat as an error with the OS error string.
        throw GetLastErrorAsString(static_cast<unsigned long>(exitCode));
    }

    return exitCode;
}
