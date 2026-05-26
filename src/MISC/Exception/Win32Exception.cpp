// MISC/Exception/Win32Exception.cpp - Qt6 port of Win32 exception handling
#include "Win32Exception.h"
#include <QMessageBox>
#include <csignal>

void Win32Exception::installHandler()
{
    // On Qt/Linux, we use signal handlers instead
    // This is a placeholder for cross-platform exception handling
}

void Win32Exception::removeHandler()
{
    // Remove signal handlers
}

void Win32Exception::translate(unsigned, void*)
{
    // Translate exception on Qt side
    // On Linux, exceptions are handled via signals
}