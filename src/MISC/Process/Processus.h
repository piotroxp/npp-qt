// MISC/Process/Processus.h - Qt6 port of Notepad++ process utilities
#pragma once

#include <QString>

enum progType { WIN32_PROG, CONSOLE_PROG };

class Process
{
public:
    Process(const QString& cmd, const QString& args, const QString& cDir)
        : _command(cmd), _args(args), _curDir(cDir) {}

    void run(bool isElevationRequired = false) const;
    unsigned long runSync(bool isElevationRequired = false) const;

protected:
    QString _command;
    QString _args;
    QString _curDir;
};