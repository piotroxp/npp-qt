// MISC/Exception/MiniDumper.cpp - Qt6 port of Notepad++ crash dump utilities
#include "MiniDumper.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDateTime>

const QString msgTitle = QStringLiteral("Notepad++ crash analysis");

MiniDumper::MiniDumper()
{
}

bool MiniDumper::writeDump(void*)
{
    bool retval = false;

    // On Qt/Linux, we don't have the same crash dump mechanism
    // We simulate the behavior by logging to a file
    QString logPath = QDir::homePath() + "/Notepad++/NppDump.log";
    QFile logFile(logPath);
    
    if (logFile.open(QIODevice::Append | QIODevice::Text))
    {
        QDateTime now = QDateTime::currentDateTime();
        QString entry = QString("Crash at %1\n").arg(now.toString(Qt::ISODate));
        logFile.write(entry.toUtf8());
        logFile.close();
        retval = true;
    }

    // On Linux, we don't have a native crash dump dialog
    // In a full implementation, this would use core dumps or similar
    
    return retval;
}