// MISC/Exception/MiniDumper.h - Qt6 port of Notepad++ crash dump utilities
#pragma once

#include <QString>

class MiniDumper
{
public:
    MiniDumper();
    bool writeDump(void* pExceptionInfo);
};